#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define FRAMES_PER_BUFFER 512
#define TARGET_FREQ 14500.0
#define TWO_PI 6.283185307179586476925286766559

const int SAMPLE_RATE = 44100; // Change to a constant variable

// Simple low-pass filter
void lowPassFilter(double *input, double *output, int length, double cutoff) {
    double rc = 1.0 / (cutoff * TWO_PI);
    double dt = 1.0 / SAMPLE_RATE;
    double alpha = dt / (rc + dt);

    output[0] = input[0]; // Initialize the first sample
    for (int i = 1; i < length; i++) {
        output[i] = output[i - 1] + alpha * (input[i] - output[i - 1]);
    }
}

// Simple high-pass filter
void highPassFilter(double *input, double *output, int length, double cutoff) {
    double rc = 1.0 / (cutoff * TWO_PI);
    double dt = 1.0 / SAMPLE_RATE;
    double alpha = rc / (rc + dt);

    output[0] = input[0]; // Initialize the first sample
    for (int i = 1; i < length; i++) {
        output[i] = alpha * (output[i - 1] + input[i] - input[i - 1]);
    }
}

int main() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    short *buffer;
    double *tempBuffer;
    double *lowPassed;
    double *highPassed;

    // Open PCM device for playback
    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Unable to open PCM device\n");
        return 1;
    }

    // Allocate hardware parameters
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate_near(handle, params, (unsigned int*)&SAMPLE_RATE, 0); // Cast to unsigned int pointer
    snd_pcm_hw_params_set_channels(handle, params, 1);
    snd_pcm_hw_params(handle, params);
    snd_pcm_hw_params_free(params);

    // Allocate buffers
    buffer = (short *)malloc(FRAMES_PER_BUFFER * sizeof(short));
    tempBuffer = (double *)malloc(FRAMES_PER_BUFFER * sizeof(double));
    lowPassed = (double *)malloc(FRAMES_PER_BUFFER * sizeof(double));
    highPassed = (double *)malloc(FRAMES_PER_BUFFER * sizeof(double));

    // Process audio in real-time
    while (1) {
        // Read from default input device
        snd_pcm_readi(handle, buffer, FRAMES_PER_BUFFER);

        // Convert to double for processing
        for (int i = 0; i < FRAMES_PER_BUFFER; i++) {
            tempBuffer[i] = (double)buffer[i];
        }

        // Apply low-pass filter
        lowPassFilter(tempBuffer, lowPassed, FRAMES_PER_BUFFER, TARGET_FREQ);

        // Apply high-pass filter
        highPassFilter(lowPassed, highPassed, FRAMES_PER_BUFFER, TARGET_FREQ);

        // Write back to output device
        for (int i = 0; i < FRAMES_PER_BUFFER; i++) {
            buffer[i] = (short)(highPassed[i]);
        }
        snd_pcm_writei(handle, buffer, FRAMES_PER_BUFFER);
    }

    // Cleanup
    snd_pcm_close(handle);
    free(buffer);
    free(tempBuffer);
    free(lowPassed);
    free(highPassed);

    return 0;
}
