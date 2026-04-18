// dosimeter.c - plain C console version with ALSA click
// Build: gcc -O2 -o dosimeter dosimeter.c -lasound -lm

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <math.h>

// Map noise type to rad/m²
double translateNoiseToRad(int noiseType) {
    switch (noiseType) {
        case 0: return 0.1;
        case 1: return 0.5;
        case 2: return 1.0;
        case 3: return 2.0;
        case 4: return 5.0;
        default: return 0.0;
    }
}

// Play a short click via ALSA
void play_click() {
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *params;
    unsigned int rate = 44100;
    int chans = 1;
    int dur_ms = 10;
    snd_pcm_uframes_t frames = rate * dur_ms / 1000;
    short *buf = malloc(sizeof(short) * frames);
    double freq = 2000.0, amp = 32000.0;

    for (snd_pcm_uframes_t i = 0; i < frames; ++i) {
        double t = (double)i / rate;
        buf[i] = (short)(amp * sin(2.0 * M_PI * freq * t) * exp(-t * 100));
    }

    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "ALSA open error\n");
        free(buf);
        return;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, chans);
    snd_pcm_hw_params_set_rate_near(pcm, params, &rate, NULL);
    snd_pcm_hw_params(pcm, params);

    snd_pcm_writei(pcm, buf, frames);
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(buf);
}

int main(void) {
    srand(time(NULL));
    double accumulated = 0.0;
    int count = 0;

    printf("Starting dosimeter simulation (Ctrl+C to stop)...\n");

    while (1) {
        int noiseType = rand() % 5;
        double rad = translateNoiseToRad(noiseType);
        accumulated += rad;
        count++;

        printf("[%d] Current: %.2f rad/m² | Accumulated: %.2f rad/m²\n",
               count, rad, accumulated);

        play_click();

        if (rad > 3.0) {
            printf("!!! ALERT: High radiation detected !!!\n");
        }

        usleep(500000); // 0.5s delay
    }

    return 0;
}

