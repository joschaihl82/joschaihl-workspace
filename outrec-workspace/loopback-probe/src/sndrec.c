/* sndrec.c
   Probe ALSA Loopback capture subdevices, Bluetooth-like playback devices,
   and Pulse/PipeWire monitor sources. Dynamic detection of devices/subdevices.
   Test duration per device: 200 ms.
   Compile: gcc -O2 -o probe_all_dynamic sndrec.c -lasound -lpulse-simple -lpulse -lm
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <alsa/asoundlib.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>

/* Ensure M_PI is available on platforms where it's not defined */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RECORD_MS 200
#define DEFAULT_RATE 44100u
#define DEFAULT_CHANNELS 2
#define THRESHOLD_16BIT 2000
#define MAX_DEVICES 64   /* safety cap when enumerating devices */

volatile sig_atomic_t stop_requested = 0;
static void sigint_handler(int sig){ (void)sig; stop_requested = 1; }

/* case-insensitive substring */
static int ci_contains(const char *hay, const char *needle) {
    if (!hay || !needle) return 0;
    size_t hn = strlen(hay), nn = strlen(needle);
    for (size_t i = 0; i + nn <= hn; ++i) {
        size_t j;
        for (j = 0; j < nn; ++j) {
            if (tolower((unsigned char)hay[i+j]) != tolower((unsigned char)needle[j])) break;
        }
        if (j == nn) return 1;
    }
    return 0;
}

/* --- ALSA generic open/configure (capture or playback) --- */
static const snd_pcm_format_t candidate_formats[] = {
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_S24_LE,
    SND_PCM_FORMAT_UNKNOWN
};
static const unsigned int candidate_rates[] = {
    44100u, 48000u, 96000u, 32000u, 0u
};

static int open_and_configure_generic(const char *devstr, snd_pcm_stream_t mode,
                                      snd_pcm_t **out_handle, snd_pcm_format_t *out_format,
                                      unsigned int *out_rate, unsigned int *out_channels,
                                      snd_pcm_uframes_t *out_period)
{
    snd_pcm_t *handle = NULL;
    int err;
    if ((err = snd_pcm_open(&handle, devstr, mode, 0)) < 0) return -1;

    snd_pcm_hw_params_t *hw = NULL;
    snd_pcm_hw_params_malloc(&hw);
    snd_pcm_hw_params_any(handle, hw);

    snd_pcm_uframes_t period_size = 1024;
    int success = 0;
    snd_pcm_format_t fmt_try = SND_PCM_FORMAT_UNKNOWN;
    unsigned int rate_try = 0;
    unsigned int channels_try = DEFAULT_CHANNELS;

    for (const snd_pcm_format_t *pf = candidate_formats; *pf != SND_PCM_FORMAT_UNKNOWN && !success; ++pf) {
        fmt_try = *pf;
        for (const unsigned int *pr = candidate_rates; *pr != 0 && !success; ++pr) {
            rate_try = *pr;
            snd_pcm_hw_params_any(handle, hw);
            if ((err = snd_pcm_hw_params_set_access(handle, hw, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) continue;
            if ((err = snd_pcm_hw_params_set_format(handle, hw, fmt_try)) < 0) continue;
            unsigned int rr = rate_try;
            if ((err = snd_pcm_hw_params_set_rate_near(handle, hw, &rr, 0)) < 0) continue;
            if (abs((int)rr - (int)rate_try) > 1000) continue;
            if ((err = snd_pcm_hw_params_set_channels(handle, hw, channels_try)) < 0) continue;
            if ((err = snd_pcm_hw_params_set_period_size_near(handle, hw, &period_size, 0)) < 0) continue;
            if ((err = snd_pcm_hw_params(handle, hw)) < 0) continue;
            success = 1;
            break;
        }
    }

    if (!success) {
        snd_pcm_hw_params_free(hw);
        snd_pcm_close(handle);
        return -2;
    }

    unsigned int actual_rate = 0;
    snd_pcm_hw_params_get_rate(hw, &actual_rate, 0);
    unsigned int actual_channels = 0;
    snd_pcm_hw_params_get_channels(hw, &actual_channels);
    snd_pcm_uframes_t actual_period = 0;
    snd_pcm_hw_params_get_period_size(hw, &actual_period, 0);
    snd_pcm_format_t actual_format = fmt_try;

    snd_pcm_hw_params_free(hw);

    *out_handle = handle;
    *out_format = actual_format;
    *out_rate = actual_rate;
    *out_channels = actual_channels;
    *out_period = actual_period;
    return 0;
}

/* --- ALSA capture peak check (RECORD_MS ms) --- */
static int record_and_check_peak(snd_pcm_t *handle, snd_pcm_format_t format, unsigned int rate, unsigned int channels, snd_pcm_uframes_t period_size, int *out_peak)
{
    int bytes_per_sample;
    if (format == SND_PCM_FORMAT_S16_LE) bytes_per_sample = 2;
    else if (format == SND_PCM_FORMAT_S32_LE) bytes_per_sample = 4;
    else if (format == SND_PCM_FORMAT_S24_LE) bytes_per_sample = 4;
    else return -1;

    size_t frame_bytes = (size_t)channels * bytes_per_sample;
    snd_pcm_uframes_t total_frames = (snd_pcm_uframes_t)((uint64_t)rate * (uint64_t)RECORD_MS / 1000u);
    if (total_frames == 0) total_frames = 1;
    snd_pcm_uframes_t frames_left = total_frames;
    size_t buf_frames = period_size ? period_size : 256;
    if (buf_frames > frames_left) buf_frames = frames_left;
    size_t bufbytes = buf_frames * frame_bytes;
    uint8_t *buffer = malloc(bufbytes);
    if (!buffer) return -1;

    int peak = 0;
    while (frames_left > 0 && !stop_requested) {
        snd_pcm_uframes_t to_read = (frames_left > buf_frames) ? buf_frames : frames_left;
        int r = snd_pcm_readi(handle, buffer, to_read);
        if (r == -EPIPE) { snd_pcm_prepare(handle); frames_left -= to_read; continue; }
        if (r < 0) { free(buffer); return -1; }
        if (r == 0) { frames_left -= to_read; continue; }
        if (format == SND_PCM_FORMAT_S16_LE) {
            int16_t *p = (int16_t*)buffer;
            int samples = r * channels;
            for (int i = 0; i < samples; ++i) { int v = abs((int)p[i]); if (v > peak) peak = v; }
        } else {
            int32_t *p = (int32_t*)buffer;
            int samples = r * channels;
            for (int i = 0; i < samples; ++i) {
                int v = abs((int)(p[i] >> (format == SND_PCM_FORMAT_S24_LE ? 8 : 0)));
                if (v > peak) peak = v;
            }
        }
        frames_left -= r;
    }
    free(buffer);
    int threshold = THRESHOLD_16BIT;
    if (format == SND_PCM_FORMAT_S32_LE || format == SND_PCM_FORMAT_S24_LE) threshold = THRESHOLD_16BIT * 32768;
    if (out_peak) *out_peak = peak;
    return (peak > threshold) ? 1 : 0;
}

/* --- ALSA playback write test (used for Bluetooth-like devices) --- */
static int playback_write_test(snd_pcm_t *handle, snd_pcm_format_t format, unsigned int rate, unsigned int channels, snd_pcm_uframes_t period_size)
{
    int bytes_per_sample;
    if (format == SND_PCM_FORMAT_S16_LE) bytes_per_sample = 2;
    else if (format == SND_PCM_FORMAT_S32_LE) bytes_per_sample = 4;
    else if (format == SND_PCM_FORMAT_S24_LE) bytes_per_sample = 4;
    else return -1;

    size_t frame_bytes = (size_t)channels * bytes_per_sample;
    snd_pcm_uframes_t total_frames = (snd_pcm_uframes_t)((uint64_t)rate * (uint64_t)RECORD_MS / 1000u);
    if (total_frames == 0) total_frames = 1;
    snd_pcm_uframes_t frames_left = total_frames;
    size_t buf_frames = period_size ? period_size : 256;
    if (buf_frames > frames_left) buf_frames = frames_left;
    size_t bufbytes = buf_frames * frame_bytes;
    uint8_t *buffer = malloc(bufbytes);
    if (!buffer) return -1;

    double freq = 440.0;
    double phase = 0.0;
    /* use acos(-1.0) to get PI portably */
    double phase_inc = 2.0 * acos(-1.0) * freq / (double)rate;
    int wrote_any = 0;

    while (frames_left > 0 && !stop_requested) {
        snd_pcm_uframes_t to_write = (frames_left > buf_frames) ? buf_frames : frames_left;
        if (format == SND_PCM_FORMAT_S16_LE) {
            int16_t *p = (int16_t*)buffer;
            for (snd_pcm_uframes_t f = 0; f < to_write; ++f) {
                int16_t sample = (int16_t)(sin(phase) * 10000.0);
                phase += phase_inc;
                for (unsigned int ch = 0; ch < channels; ++ch) p[f*channels + ch] = sample;
            }
        } else {
            int32_t *p = (int32_t*)buffer;
            for (snd_pcm_uframes_t f = 0; f < to_write; ++f) {
                int32_t sample = (int32_t)(sin(phase) * 1000000000.0);
                phase += phase_inc;
                for (unsigned int ch = 0; ch < channels; ++ch) p[f*channels + ch] = sample;
            }
        }
        int r = snd_pcm_writei(handle, buffer, to_write);
        if (r == -EPIPE) { snd_pcm_prepare(handle); frames_left -= to_write; continue; }
        if (r < 0) { free(buffer); return -1; }
        if (r > 0) wrote_any = 1;
        frames_left -= r;
    }
    free(buffer);
    return wrote_any ? 1 : 0;
}

/* --- Pulse/ PipeWire: list sources via pactl and test monitor sources with libpulse-simple --- */
static int test_pulse_monitors(void)
{
    FILE *fp = popen("pactl list short sources 2>/dev/null", "r");
    if (!fp) return -1;
    char line[512];
    while (fgets(line, sizeof(line), fp) && !stop_requested) {
        char *save = NULL;
        char *tok = strtok_r(line, "\t\n", &save);
        if (!tok) continue;
        tok = strtok_r(NULL, "\t\n", &save);
        if (!tok) continue;
        char source_name[256];
        strncpy(source_name, tok, sizeof(source_name)-1);
        source_name[sizeof(source_name)-1] = '\0';
        if (!ci_contains(source_name, "monitor")) continue;

        fprintf(stderr, "Gefundene Pulse/ PipeWire Monitor-Quelle: %s\n", source_name);

        pa_sample_spec ss;
        ss.format = PA_SAMPLE_S16LE;
        ss.rate = DEFAULT_RATE;
        ss.channels = DEFAULT_CHANNELS;
        int error = 0;
        pa_simple *s = pa_simple_new(NULL, "probe_all", PA_STREAM_RECORD, source_name, "probe", &ss, NULL, NULL, &error);
        if (!s) {
            fprintf(stderr, "  pa_simple_new failed for %s: %s\n", source_name, pa_strerror(error));
            ss.format = PA_SAMPLE_S32LE;
            ss.rate = 48000;
            ss.channels = DEFAULT_CHANNELS;
            error = 0;
            s = pa_simple_new(NULL, "probe_all", PA_STREAM_RECORD, source_name, "probe", &ss, NULL, NULL, &error);
            if (!s) {
                fprintf(stderr, "  pa_simple_new fallback failed for %s: %s\n", source_name, pa_strerror(error));
                continue;
            }
        }

        size_t bytes_per_sample = (ss.format == PA_SAMPLE_S16LE) ? 2 : 4;
        size_t frame_bytes = bytes_per_sample * ss.channels;
        size_t total_frames = (ss.rate * RECORD_MS) / 1000;
        if (total_frames == 0) total_frames = 1;
        size_t buf_frames = 1024;
        if (buf_frames > total_frames) buf_frames = total_frames;
        size_t bufbytes = buf_frames * frame_bytes;
        uint8_t *buffer = malloc(bufbytes);
        if (!buffer) { pa_simple_free(s); continue; }

        int peak = 0;
        size_t frames_left = total_frames;
        while (frames_left > 0 && !stop_requested) {
            size_t to_read = (frames_left > buf_frames) ? buf_frames : frames_left;
            if (pa_simple_read(s, buffer, to_read * frame_bytes, &error) < 0) {
                fprintf(stderr, "  pa_simple_read error: %s\n", pa_strerror(error));
                break;
            }
            if (ss.format == PA_SAMPLE_S16LE) {
                int16_t *p = (int16_t*)buffer;
                int samples = to_read * ss.channels;
                for (int i = 0; i < samples; ++i) { int v = abs((int)p[i]); if (v > peak) peak = v; }
            } else {
                int32_t *p = (int32_t*)buffer;
                int samples = to_read * ss.channels;
                for (int i = 0; i < samples; ++i) {
                    int v = abs((int)(p[i] >> 0));
                    if (v > peak) peak = v;
                }
            }
            frames_left -= to_read;
        }

        free(buffer);
        pa_simple_free(s);

        int threshold = THRESHOLD_16BIT;
        if (ss.format == PA_SAMPLE_S32LE) threshold = THRESHOLD_16BIT * 32768;
        if (peak > threshold) {
            printf("Found signal on Pulse source: %s\n", source_name);
            printf("Used sample spec: format=%s rate=%u channels=%u peak=%d\n",
                   (ss.format==PA_SAMPLE_S16LE) ? "S16_LE" : "S32_LE",
                   ss.rate, ss.channels, peak);
            pclose(fp);
            return 1;
        } else {
            fprintf(stderr, "  Kein Signal (peak=%d)\n", peak);
        }
    }
    pclose(fp);
    return 0;
}

/* --- helper: try to get PCM name via snd_pcm_info (correct ALSA API) --- */
static int get_pcm_name(int card, int device, int subdevice, snd_pcm_stream_t stream, char *name_buf, size_t name_buf_len) {
    char ctlname[32];
    snprintf(ctlname, sizeof(ctlname), "hw:%d", card);
    snd_ctl_t *ctl = NULL;
    if (snd_ctl_open(&ctl, ctlname, 0) < 0) return 0;
    snd_pcm_info_t *pinfo = NULL;
    snd_pcm_info_malloc(&pinfo);
    snd_pcm_info_set_device(pinfo, device);
    snd_pcm_info_set_subdevice(pinfo, subdevice);
    snd_pcm_info_set_stream(pinfo, stream);
    int rc = snd_ctl_pcm_info(ctl, pinfo);
    if (rc < 0) {
        snd_pcm_info_free(pinfo);
        snd_ctl_close(ctl);
        return 0;
    }
    const char *pname = snd_pcm_info_get_name(pinfo);
    if (pname && name_buf && name_buf_len > 0) {
        strncpy(name_buf, pname, name_buf_len-1);
        name_buf[name_buf_len-1] = '\0';
    }
    snd_pcm_info_free(pinfo);
    snd_ctl_close(ctl);
    return 1;
}

/* --- main: enumerate cards, enumerate devices dynamically, test accordingly --- */
int main(void)
{
    signal(SIGINT, sigint_handler);

    int card = -1;
    if (snd_card_next(&card) < 0) { fprintf(stderr, "snd_card_next failed\n"); return 2; }
    if (card < 0) { fprintf(stderr, "Keine ALSA-Karten gefunden\n"); return 2; }

    snd_ctl_card_info_t *info;
    snd_ctl_card_info_malloc(&info);

    while (card >= 0 && !stop_requested) {
        char ctlname[32];
        snprintf(ctlname, sizeof(ctlname), "hw:%d", card);
        snd_ctl_t *ctl = NULL;
        if (snd_ctl_open(&ctl, ctlname, 0) >= 0) {
            if (snd_ctl_card_info(ctl, info) >= 0) {
                const char *name = snd_ctl_card_info_get_name(info);
                const char *longname = snd_ctl_card_info_get_longname(info);
                const char *card_ident = name ? name : longname ? longname : "(unknown)";

                fprintf(stderr, "Untersuche Card %d: %s (%s)\n", card, name?name:"(unknown)", longname?longname:"(no longname)");

                /* enumerate PCM devices dynamically: try device indices 0..MAX_DEVICES-1 */
                for (int dev = 0; dev < MAX_DEVICES && !stop_requested; ++dev) {
                    /* try to get capture subdevices count */
                    snd_pcm_info_t *pinfo = NULL;
                    snd_pcm_info_malloc(&pinfo);
                    snd_pcm_info_set_device(pinfo, dev);
                    snd_pcm_info_set_subdevice(pinfo, 0);
                    snd_pcm_info_set_stream(pinfo, SND_PCM_STREAM_CAPTURE);
                    int rc = snd_ctl_pcm_info(ctl, pinfo);
                    if (rc >= 0) {
                        unsigned int sub_count = snd_pcm_info_get_subdevices_count(pinfo);
                        char devname[128] = {0};
                        const char *pname = snd_pcm_info_get_name(pinfo);
                        if (pname) strncpy(devname, pname, sizeof(devname)-1);
                        snd_pcm_info_free(pinfo);

                        /* If this device has capture subdevices and card is loopback, test them */
                        if (sub_count > 0 && ci_contains(card_ident, "loopback")) {
                            fprintf(stderr, "  Capture-Device %d hat %u Subdevices (Name: %s)\n", dev, sub_count, devname[0]?devname:"(unknown)");
                            for (unsigned int sub = 0; sub < sub_count && !stop_requested; ++sub) {
                                char devstr[64];
                                snprintf(devstr, sizeof(devstr), "hw:%d,%d,%d", card, dev, (int)sub);
                                char pcm_name[128] = {0};
                                if (!get_pcm_name(card, dev, sub, SND_PCM_STREAM_CAPTURE, pcm_name, sizeof(pcm_name)))
                                    strncpy(pcm_name, longname ? longname : "(unknown)", sizeof(pcm_name)-1);
                                fprintf(stderr, "    Teste Capture %s  PCM-Name: %s\n", devstr, pcm_name);

                                snd_pcm_t *handle = NULL;
                                snd_pcm_format_t fmt = SND_PCM_FORMAT_UNKNOWN;
                                unsigned int rate = 0;
                                unsigned int channels = 0;
                                snd_pcm_uframes_t period = 0;
                                int orc = open_and_configure_generic(devstr, SND_PCM_STREAM_CAPTURE, &handle, &fmt, &rate, &channels, &period);
                                char used_dev[128];
                                strncpy(used_dev, devstr, sizeof(used_dev)-1); used_dev[sizeof(used_dev)-1]=0;
                                if (orc < 0) {
                                    char plugdev[64];
                                    snprintf(plugdev, sizeof(plugdev), "plughw:%d,%d", card, dev);
                                    fprintf(stderr, "      hw open/config failed for %s, trying %s ...\n", devstr, plugdev);
                                    orc = open_and_configure_generic(plugdev, SND_PCM_STREAM_CAPTURE, &handle, &fmt, &rate, &channels, &period);
                                    if (orc < 0) { fprintf(stderr, "      Fehler beim Testen von %s\n", devstr); continue; }
                                    strncpy(used_dev, plugdev, sizeof(used_dev)-1); used_dev[sizeof(used_dev)-1]=0;
                                }

                                fprintf(stderr, "      Geöffnet: %s  PCM-Name: %s  Format:%s Rate:%u Ch:%u Period:%lu\n",
                                        used_dev, pcm_name,
                                        (fmt==SND_PCM_FORMAT_S16_LE)?"S16_LE":(fmt==SND_PCM_FORMAT_S32_LE)?"S32_LE":(fmt==SND_PCM_FORMAT_S24_LE)?"S24_LE":"UNKNOWN",
                                        rate, channels, (unsigned long)period);

                                int peak = 0;
                                int found = record_and_check_peak(handle, fmt, rate, channels, period, &peak);
                                snd_pcm_close(handle);

                                if (found == 1) {
                                    printf("Found signal on %s (capture)\n", used_dev);
                                    printf("Card: %s\n", card_ident);
                                    printf("PCM-Name: %s\n", pcm_name);
                                    printf("Format: %s Rate: %u Channels: %u Peak: %d\n",
                                           (fmt==SND_PCM_FORMAT_S16_LE)?"S16_LE":(fmt==SND_PCM_FORMAT_S32_LE)?"S32_LE":(fmt==SND_PCM_FORMAT_S24_LE)?"S24_LE":"UNKNOWN",
                                           rate, channels, peak);
                                    snd_ctl_close(ctl);
                                    snd_ctl_card_info_free(info);
                                    return 0;
                                } else {
                                    fprintf(stderr, "      Kein Signal (peak=%d)\n", peak);
                                }
                            }
                        } else {
                            /* no capture subdevices or not loopback; already freed pinfo */
                        }
                    } else {
                        snd_pcm_info_free(pinfo);
                    }

                    /* now check playback side for this device (use subdevice count from playback stream) */
                    snd_pcm_info_t *pinfo2 = NULL;
                    snd_pcm_info_malloc(&pinfo2);
                    snd_pcm_info_set_device(pinfo2, dev);
                    snd_pcm_info_set_subdevice(pinfo2, 0);
                    snd_pcm_info_set_stream(pinfo2, SND_PCM_STREAM_PLAYBACK);
                    int rc2 = snd_ctl_pcm_info(ctl, pinfo2);
                    if (rc2 >= 0) {
                        unsigned int sub_count2 = snd_pcm_info_get_subdevices_count(pinfo2);
                        char devname2[128] = {0};
                        const char *pname2 = snd_pcm_info_get_name(pinfo2);
                        if (pname2) strncpy(devname2, pname2, sizeof(devname2)-1);
                        snd_pcm_info_free(pinfo2);

                        /* If card looks like Bluetooth/BlueZ or bluez-like, test playback subdevices */
                        if (sub_count2 > 0 && (ci_contains(card_ident, "bluetooth") || ci_contains(card_ident, "bluez") || ci_contains(card_ident, "bt"))) {
                            fprintf(stderr, "  Playback-Device %d hat %u Subdevices (Name: %s)\n", dev, sub_count2, devname2[0]?devname2:"(unknown)");
                            for (unsigned int sub = 0; sub < sub_count2 && !stop_requested; ++sub) {
                                char devstr[64];
                                snprintf(devstr, sizeof(devstr), "hw:%d,%d,%d", card, dev, (int)sub);
                                char pcm_name[128] = {0};
                                if (!get_pcm_name(card, dev, sub, SND_PCM_STREAM_PLAYBACK, pcm_name, sizeof(pcm_name)))
                                    strncpy(pcm_name, longname ? longname : "(unknown)", sizeof(pcm_name)-1);
                                fprintf(stderr, "    Teste Playback %s  PCM-Name: %s\n", devstr, pcm_name);

                                snd_pcm_t *ph = NULL;
                                snd_pcm_format_t pf = SND_PCM_FORMAT_UNKNOWN;
                                unsigned int prate = 0;
                                unsigned int pchannels = 0;
                                snd_pcm_uframes_t pperiod = 0;
                                int prc = open_and_configure_generic(devstr, SND_PCM_STREAM_PLAYBACK, &ph, &pf, &prate, &pchannels, &pperiod);
                                char used_dev[128];
                                strncpy(used_dev, devstr, sizeof(used_dev)-1); used_dev[sizeof(used_dev)-1]=0;
                                if (prc < 0) {
                                    char plugdev[64];
                                    snprintf(plugdev, sizeof(plugdev), "plughw:%d,%d", card, dev);
                                    fprintf(stderr, "      hw open/config failed for %s, trying %s ...\n", devstr, plugdev);
                                    prc = open_and_configure_generic(plugdev, SND_PCM_STREAM_PLAYBACK, &ph, &pf, &prate, &pchannels, &pperiod);
                                    if (prc < 0) { fprintf(stderr, "      Fehler beim Öffnen von %s\n", devstr); continue; }
                                    strncpy(used_dev, plugdev, sizeof(used_dev)-1); used_dev[sizeof(used_dev)-1]=0;
                                }

                                fprintf(stderr, "      Geöffnet: %s  PCM-Name: %s  Format:%s Rate:%u Ch:%u Period:%lu\n",
                                        used_dev, pcm_name,
                                        (pf==SND_PCM_FORMAT_S16_LE)?"S16_LE":(pf==SND_PCM_FORMAT_S32_LE)?"S32_LE":(pf==SND_PCM_FORMAT_S24_LE)?"S24_LE":"UNKNOWN",
                                        prate, pchannels, (unsigned long)pperiod);

                                int wrote = playback_write_test(ph, pf, prate, pchannels, pperiod);
                                snd_pcm_close(ph);
                                if (wrote == 1) {
                                    printf("Playback device accepts writes: %s\n", used_dev);
                                    printf("Card: %s\n", card_ident);
                                    printf("PCM-Name: %s\n", pcm_name);
                                    printf("Format: %s Rate: %u Channels: %u\n",
                                           (pf==SND_PCM_FORMAT_S16_LE)?"S16_LE":(pf==SND_PCM_FORMAT_S32_LE)?"S32_LE":(pf==SND_PCM_FORMAT_S24_LE)?"S24_LE":"UNKNOWN",
                                           prate, pchannels);
                                    snd_ctl_close(ctl);
                                    snd_ctl_card_info_free(info);
                                    return 0;
                                } else if (wrote == 0) {
                                    fprintf(stderr, "      Keine Frames geschrieben (device evtl. nicht verbunden)\n");
                                } else {
                                    fprintf(stderr, "      Fehler beim Schreiben auf %s\n", used_dev);
                                }
                            }
                        }
                    } else {
                        snd_pcm_info_free(pinfo2);
                    }
                } /* end device loop */

            }
            snd_ctl_close(ctl);
        }
        if (snd_card_next(&card) < 0) break;
    }

    /* If nothing found yet, try Pulse/ PipeWire monitors */
    fprintf(stderr, "Kein ALSA Loopback/Bluetooth Treffer. Versuche Pulse/ PipeWire Monitor-Quellen (pactl)...\n");
    int pulse_rc = test_pulse_monitors();
    if (pulse_rc == 1) return 0;

    snd_ctl_card_info_free(info);
    fprintf(stderr, "Kein geeignetes Gerät mit Signal/Write-Akzeptanz gefunden.\n");
    fprintf(stderr, "Stelle sicher, dass Wiedergabe/Verbindung aktiv ist und ggf. dass Wiedergabe auf Loopback geroutet ist.\n");
    return 1;
}
