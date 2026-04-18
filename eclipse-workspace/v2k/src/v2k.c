#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512
#define TARGET_FREQ 14500.0
#define NUM_SAMPLES (FRAMES_PER_BUFFER)

// Function to handle audio processing
static int audioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData) {
    float *in = (float *)inputBuffer;
    float *out = (float *)outputBuffer;

    // FFT setup
    fftw_complex *data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * NUM_SAMPLES);
    fftw_complex *outData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * NUM_SAMPLES);
    fftw_plan plan = fftw_plan_dft_1d(NUM_SAMPLES, data, outData, FFTW_FORWARD, FFTW_ESTIMATE);

    // Fill data for FFT
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        data[i][0] = in[i];   // Real Part
        data[i][1] = 0.0;     // Imaginary Part
    }

    // Execute FFT
    fftw_execute(plan);

    // Find the peak frequency
    double maxMagnitude = 0;
    int peakIndex = 0;
    for (int i = 0; i < NUM_SAMPLES / 2; i++) {
        double magnitude = sqrt(outData[i][0] * outData[i][0] + outData[i][1] * outData[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            peakIndex = i;
        }
    }

    // Check if peak frequency is within threshold of the target frequency
    double frequency = (double)peakIndex * SAMPLE_RATE / NUM_SAMPLES;
    if (fabs(frequency - TARGET_FREQ) < 50.0) { // 50 Hz tolerance
        // Output or process signal as needed
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            out[i] = in[i]; // Echo or modify if needed
        }
    } else {
        // Set output to zero if outside frequency range
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            out[i] = 0;
        }
    }

    fftw_destroy_plan(plan);
    fftw_free(data);
    fftw_free(outData);

    return paContinue;
}

int main() {
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    PaStream *stream;
    stream = Pa_OpenDefaultStream(&stream,
                                   1,          // input channels
                                   1,          // output channels
                                   paFloat32,  // sample format
                                   SAMPLE_RATE,
                                   FRAMES_PER_BUFFER,
                                   audioCallback,
                                   NULL);

    if (stream == NULL) {
        fprintf(stderr, "PortAudio error: Unable to open stream\n");
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: Unable to start stream\n");
        return 1;
    }

    printf("Press Enter to stop...\n");
    getchar();

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: Unable to stop stream\n");
        return 1;
    }

    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}
