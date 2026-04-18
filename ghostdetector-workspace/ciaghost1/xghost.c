#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <errno.h>

// --- V4L2 Headers for /dev/video0 access ---
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <unistd.h>

// SDL2 Headers for Visualization
#include <SDL2/SDL.h>

// --- Configuration ---
#define PATTERN_SIZE 64 // N x N resolution of the Ghost Image
#define MAX_PATTERNS (PATTERN_SIZE * PATTERN_SIZE)
#define V4L2_DEVICE "/dev/video0"
#define V4L2_BUFFER_COUNT 4
#define FRAME_DELAY_MS 50 // Delay to limit frame rate (20 FPS)

// --- Data Structures ---

typedef struct {
    double *data;
    int width;
    int height;
} Image;

typedef struct {
    double data[3][3];
} Kernel3x3;

typedef struct {
    double A;        // Normalisation factor (Lamp intensity)
    double sigma;    // Detector noise (std dev)
} ExperimentParams;

// --- V4L2 Buffer Management ---
struct buffer {
    void   *start;
    size_t length;
};
static struct buffer *g_buffers = NULL;

// --- SDL/V4L2 Global Pointers ---
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
int gV4L2_fd = -1; 
int g_running = 1;

// --- Forward Declarations & Utility Stubs (Defined below main loop) ---
Image create_image(int w, int h);
void free_image(Image img);
Image generate_raster_pattern(int N, int index);
Kernel3x3 get_edge_detection_kernel();
Image convolve_cyclic(Image src, Kernel3x3 k);
double measure_hardware(Image object, Image pattern, ExperimentParams params);
double measure_experimental_coefficient(Image object, Image theoretical_pattern, ExperimentParams params);
Image reconstruct_image(double *coefficients, Image *reconstruction_basis);
static int xioctl(int fd, int request, void *arg);
int V4L2_init_capture();
Image V4L2_capture_object();
int SDL_init_and_create_window();
void SDL_RenderImage(Image img, int x, int y, int size);
void initialize_simulation(Image **basis_ptr, Kernel3x3 *kernel_ptr);
void process_and_render_frame(Image object, Image *basis, Kernel3x3 K, ExperimentParams params);


// =================================================================
// SECTION 1: V4L2 HARDWARE ABSTRACTION (V4L2_init_capture, V4L2_capture_object, xioctl)
// ... (Content identical to previous response, ensuring core capture logic remains) ...
// =================================================================

static int xioctl(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

int V4L2_init_capture() {
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    
    gV4L2_fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
    if (gV4L2_fd < 0) {
        perror("[V4L2 ERROR] Cannot open /dev/video0");
        return -1;
    }
    
    // Set Format
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = PATTERN_SIZE;
    fmt.fmt.pix.height = PATTERN_SIZE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; 
    
    if (xioctl(gV4L2_fd, VIDIOC_S_FMT, &fmt) == -1) {
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
        if (xioctl(gV4L2_fd, VIDIOC_S_FMT, &fmt) == -1) {
            close(gV4L2_fd);
            return -1;
        }
    }

    // Request Buffers
    memset(&req, 0, sizeof(req));
    req.count = V4L2_BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(gV4L2_fd, VIDIOC_REQBUFS, &req) == -1) { close(gV4L2_fd); return -1; }
    g_buffers = calloc(req.count, sizeof(*g_buffers));

    // Map Buffers
    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(gV4L2_fd, VIDIOC_QUERYBUF, &buf) == -1) { return -1; }
        g_buffers[i].length = buf.length;
        g_buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, gV4L2_fd, buf.m.offset);
        if (g_buffers[i].start == MAP_FAILED) { return -1; }
    }
    
    // Enqueue Buffers and Start Streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = type;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(gV4L2_fd, VIDIOC_QBUF, &buf) == -1) { return -1; }
    }
    if (xioctl(gV4L2_fd, VIDIOC_STREAMON, &type) == -1) { return -1; }
    
    printf("[V4L2 INFO] Streaming started.\n");
    return 0;
}

Image V4L2_capture_object() {
    struct v4l2_buffer buf;
    fd_set fds;
    struct timeval tv;
    
    if (gV4L2_fd < 0) { return create_image(PATTERN_SIZE, PATTERN_SIZE); }
    
    // Wait for frame
    FD_ZERO(&fds);
    FD_SET(gV4L2_fd, &fds);
    tv.tv_sec = 0; 
    tv.tv_usec = 50000; // 50ms timeout
    if (select(gV4L2_fd + 1, &fds, NULL, NULL, &tv) <= 0) { return create_image(PATTERN_SIZE, PATTERN_SIZE); }

    // Dequeue a buffer
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(gV4L2_fd, VIDIOC_DQBUF, &buf) == -1) { return create_image(PATTERN_SIZE, PATTERN_SIZE); }
    
    // Convert Buffer Data to Image (Y Channel only)
    Image object = create_image(PATTERN_SIZE, PATTERN_SIZE);
    unsigned char *frame_data = (unsigned char *)g_buffers[buf.index].start;
    
    // Assuming YUYV (Y channel at even indices).
    for (int i = 0; i < PATTERN_SIZE * PATTERN_SIZE; i++) {
        unsigned char pixel_value = frame_data[i * 2]; 
        object.data[i] = (double)pixel_value / 255.0; 
    }

    // Enqueue the buffer again
    if (xioctl(gV4L2_fd, VIDIOC_QBUF, &buf) == -1) { } // Ignore error, continue running
    
    return object;
}

// =================================================================
// SECTION 2: SDL2 VISUALIZATION
// =================================================================

int SDL_init_and_create_window() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    gWindow = SDL_CreateWindow("CIA-Ghostviewer v15.0 - LIVE", 
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                               800, 400, // Initial size
                               SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED); // MAXIMIZED flag
    if (gWindow == NULL) { return -1; }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL) { return -1; }
    return 0;
}

void SDL_RenderImage(Image img, int x, int y, int size) {
    if (gRenderer == NULL || img.data == NULL || img.width == 0) return;

    int cell_size = size / img.width;
    double max_val = 0.0;
    for (int i = 0; i < img.width * img.height; i++) {
        if (fabs(img.data[i]) > max_val) max_val = fabs(img.data[i]);
    }

    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {
            double val = img.data[i * img.width + j];
            int gray;
            
            if (max_val > 0.0) {
                gray = (int)(255.0 * fabs(val) / max_val);
            } else {
                gray = 0;
            }

            if (val > 0) {
                SDL_SetRenderDrawColor(gRenderer, gray, gray/3, gray/3, 255); 
            } else {
                SDL_SetRenderDrawColor(gRenderer, gray/3, gray/3, gray, 255); 
            }

            SDL_Rect rect = { x + j * cell_size, y + i * cell_size, cell_size, cell_size };
            SDL_RenderFillRect(gRenderer, &rect);
        }
    }
}

// =================================================================
// SECTION 3: CORE ALGORITHM (Refactored)
// =================================================================

void initialize_simulation(Image **basis_ptr, Kernel3x3 *kernel_ptr) {
    // 1. Initialize Kernel
    *kernel_ptr = get_edge_detection_kernel();

    // 2. Pre-calculate the Standard Raster Basis
    *basis_ptr = (Image*)malloc(sizeof(Image) * MAX_PATTERNS);
    printf("Pre-calculating %d raster basis patterns...\n", MAX_PATTERNS);
    for (int i = 0; i < MAX_PATTERNS; i++) {
        (*basis_ptr)[i] = generate_raster_pattern(PATTERN_SIZE, i);
    }
}

void process_and_render_frame(Image object, Image *basis, Kernel3x3 K, ExperimentParams params) {
    double *coeffs_A = (double*)malloc(sizeof(double) * MAX_PATTERNS);
    double *coeffs_B = (double*)malloc(sizeof(double) * MAX_PATTERNS);

    for(int j = 0; j < MAX_PATTERNS; j++) {
        
        // METHOD A: POST-PROCESSING MEASUREMENT
        double s1 = measure_hardware(object, basis[j], params);
        double s2 = measure_hardware(object, basis[j], params); 
        coeffs_A[j] = (s1 + s2) / 2.0;
        
        // METHOD B: BASIS-PROCESSING MEASUREMENT
        Image modified_pattern = convolve_cyclic(basis[j], K);
        coeffs_B[j] = measure_experimental_coefficient(object, modified_pattern, params);
        free_image(modified_pattern);
    }
    
    // RECONSTRUCTION & FILTERING
    Image reconstructed_A_unfiltered = reconstruct_image(coeffs_A, basis);
    Image final_image_A = convolve_cyclic(reconstructed_A_unfiltered, K); 
    Image final_image_B = reconstruct_image(coeffs_B, basis); 
    
    // --- Visualization ---
    int w, h;
    SDL_GetWindowSize(gWindow, &w, &h);
    int img_size = h - 100;

    SDL_SetRenderDrawColor(gRenderer, 50, 50, 50, 255);
    SDL_RenderClear(gRenderer);
    
    // Object (V4L2 Input) - Left
    SDL_RenderImage(object, w/4 - img_size/2, 50, img_size);
    // Post-Processed Result (Method A) - Center
    SDL_RenderImage(final_image_A, w/2 - img_size/2, 50, img_size);
    // Basis-Processed Result (Method B) - Right
    SDL_RenderImage(final_image_B, 3*w/4 - img_size/2, 50, img_size);
    
    SDL_RenderPresent(gRenderer);
    
    // Cleanup images created in this loop
    free_image(reconstructed_A_unfiltered);
    free_image(final_image_A);
    free_image(final_image_B);
    free(coeffs_A);
    free(coeffs_B);
}

void main_application_loop(Image *standard_basis, Kernel3x3 K, ExperimentParams params) {
    SDL_Event e;
    Uint32 frame_start;
    int frame_time;

    while (g_running) {
        frame_start = SDL_GetTicks();

        // 1. Handle SDL Events (Quit)
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                g_running = 0;
            }
        }
        
        // 2. Capture New Frame
        Image object = V4L2_capture_object();

        // Check for empty image (V4L2 timeout/error)
        if (object.data == NULL) {
            SDL_Delay(FRAME_DELAY_MS);
            continue;
        }

        // 3. Process and Render
        process_and_render_frame(object, standard_basis, K, params);

        // 4. Cleanup and Delay for Frame Rate Control
        free_image(object);

        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY_MS) {
            SDL_Delay(FRAME_DELAY_MS - frame_time);
        }
    }
}

// =================================================================
// SECTION 4: MAIN and UTILITIES
// =================================================================

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (SDL_init_and_create_window() < 0) { return 1; }
    
    if (V4L2_init_capture() != 0) {
        fprintf(stderr, "V4L2 initialization failed. Cannot start viewer.\n");
        // Fallback: Use dummy image if V4L2 is essential, but for now, exit.
        return 1;
    }
    
    Image *standard_basis = NULL;
    Kernel3x3 K;
    initialize_simulation(&standard_basis, &K);

    ExperimentParams params = { .A = 20.0, .sigma = 1.0 }; 

    main_application_loop(standard_basis, K, params);
    
    // --- Cleanup ---
    if (gV4L2_fd >= 0) { close(gV4L2_fd); }
    if (g_buffers) free(g_buffers);
    if (standard_basis) {
        for (int i = 0; i < MAX_PATTERNS; i++) free_image(standard_basis[i]);
        free(standard_basis);
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    
    return 0;
}

// --- Utility function implementations (from previous steps) ---

Image create_image(int w, int h) {
    Image img;
    img.width = w;
    img.height = h;
    img.data = (double*)calloc(w * h, sizeof(double));
    if (img.data == NULL) { exit(EXIT_FAILURE); }
    return img;
}
void free_image(Image img) { if (img.data) free(img.data); }

Image generate_raster_pattern(int N, int index) {
    Image img = create_image(N, N);
    if (index >= 0 && index < N * N) { img.data[index] = 1.0; }
    return img;
}

Kernel3x3 get_edge_detection_kernel() {
    Kernel3x3 k = { .data = { { 0.0, -1.0, 0.0}, {-1.0, 0.0, 1.0}, { 0.0, 1.0, 0.0} } };
    return k;
}

Image convolve_cyclic(Image src, Kernel3x3 k) {
    Image result = create_image(src.width, src.height);
    int w = src.width;
    int h = src.height;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            double sum = 0.0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int img_y = (y + ky + h) % h;
                    int img_x = (x + kx + w) % w;
                    sum += src.data[img_y * w + img_x] * k.data[ky + 1][kx + 1];
                }
            }
            result.data[y * w + x] = sum;
        }
    }
    return result;
}

double measure_hardware(Image object, Image pattern, ExperimentParams params) {
    double dot_product = 0.0;
    for (int i = 0; i < object.width * object.height; i++) {
        dot_product += object.data[i] * pattern.data[i];
    }
    double noise = ((double)rand() / RAND_MAX - 0.5) * 2.0 * params.sigma; 
    return (dot_product + noise) / params.A;
}

double measure_experimental_coefficient(Image object, Image theoretical_pattern, ExperimentParams params) {
    Image p_pos = create_image(theoretical_pattern.width, theoretical_pattern.height);
    Image p_neg = create_image(theoretical_pattern.width, theoretical_pattern.height);
    
    for (int i = 0; i < theoretical_pattern.width * theoretical_pattern.height; i++) {
        double value = theoretical_pattern.data[i];
        p_pos.data[i] = fmax(0.0, value);
        p_neg.data[i] = fmax(0.0, -value);
    }
    
    double signal_pos = measure_hardware(object, p_pos, params);
    double signal_neg = measure_hardware(object, p_neg, params);
    
    free_image(p_pos);
    free_image(p_neg);
    
    return signal_pos - signal_neg;
}

Image reconstruct_image(double *coefficients, Image *reconstruction_basis) {
    Image I = create_image(PATTERN_SIZE, PATTERN_SIZE);

    for (int j = 0; j < MAX_PATTERNS; j++) {
        double coeff = coefficients[j];
        for (int i = 0; i < MAX_PATTERNS; i++) {
            I.data[i] += coeff * reconstruction_basis[j].data[i];
        }
    }
    return I;
}

