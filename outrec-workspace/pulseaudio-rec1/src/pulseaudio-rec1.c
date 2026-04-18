// record_pulse_mp3_only.c
// Compile: gcc -o record_pulse_mp3_only record_pulse_mp3_only.c -lpulse-simple -lpulse -lmp3lame

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <lame/lame.h>

static volatile int running = 1;
static void int_handler(int sig) { (void)sig; running = 0; }

#define PCM_BUF_BYTES 8192

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [--device <pulse_source>] [--bitrate <kbps>]\n"
        "Defaults: device=default.monitor, bitrate=192\n", prog);
}

int main(int argc, char *argv[]) {
    const char *device = "default.monitor";
    int bitrate = 192;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--device") == 0 && i+1 < argc) {
            device = argv[++i];
        } else if (strcmp(argv[i], "--bitrate") == 0 && i+1 < argc) {
            bitrate = atoi(argv[++i]);
            if (bitrate <= 0) bitrate = 192;
        } else if (strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char outname[256];
    strftime(outname, sizeof(outname), "%Y-%m-%d-%H-%M-%S.mp3", t);

    printf("Aufnahme von PulseAudio Quelle: %s\n", device);
    printf("Ausgabe (MP3): %s\n", outname);
    printf("Stop mit Ctrl+C\n");

    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    int error;
    pa_simple *s = pa_simple_new(NULL, "record_pulse_mp3_only", PA_STREAM_RECORD,
                                 device, "record", &ss, NULL, NULL, &error);
    if (!s) {
        fprintf(stderr, "PulseAudio Fehler: %s\n", pa_strerror(error));
        return 1;
    }

    FILE *out = fopen(outname, "wb");
    if (!out) {
        perror("fopen");
        pa_simple_free(s);
        return 1;
    }

    lame_t lame = lame_init();
    if (!lame) {
        fprintf(stderr, "LAME init failed\n");
        fclose(out);
        pa_simple_free(s);
        return 1;
    }
    lame_set_in_samplerate(lame, ss.rate);
    lame_set_num_channels(lame, ss.channels);
    lame_set_brate(lame, bitrate);
    lame_set_quality(lame, 5); // 0 best, 9 worst
    if (lame_init_params(lame) < 0) {
        fprintf(stderr, "LAME init_params failed\n");
        lame_close(lame);
        fclose(out);
        pa_simple_free(s);
        return 1;
    }

    int16_t pcm_buf[PCM_BUF_BYTES / 2];
    unsigned char mp3buf[PCM_BUF_BYTES];

    while (running) {
        if (pa_simple_read(s, pcm_buf, sizeof(pcm_buf), &error) < 0) {
            fprintf(stderr, "PulseAudio read error: %s\n", pa_strerror(error));
            break;
        }

        int bytes_read = (int)sizeof(pcm_buf);
        int samples_per_channel = bytes_read / (2 * ss.channels);

        int mp3bytes = lame_encode_buffer_interleaved(lame, pcm_buf, samples_per_channel, mp3buf, sizeof(mp3buf));
        if (mp3bytes < 0) {
            fprintf(stderr, "LAME encode error: %d\n", mp3bytes);
            break;
        }
        if (mp3bytes > 0) {
            if (fwrite(mp3buf, 1, mp3bytes, out) != (size_t)mp3bytes) {
                perror("fwrite");
                break;
            }
        }
    }

    // flush encoder
    int flush_bytes = lame_encode_flush(lame, mp3buf, sizeof(mp3buf));
    if (flush_bytes > 0) fwrite(mp3buf, 1, flush_bytes, out);

    lame_close(lame);
    fclose(out);
    pa_simple_free(s);

    printf("Aufnahme beendet. Datei: %s\n", outname);
    return 0;
}
