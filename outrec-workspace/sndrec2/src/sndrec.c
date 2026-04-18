/* alsa_loop_auto.c
   Try multiple ALSA capture devices (loopback, default, pulse) and record 44.1kHz stereo S16_LE
   to a WAV file named YYMMDD-HHMMSS.wav. Ctrl+C stops and finalizes header.
*/
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include <stdint.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;
void handle_sigint(int sig){ (void)sig; running = 0; }

struct wav_hdr {
  char riff[4]; uint32_t overall_size; char wave[4];
  char fmt_chunk_marker[4]; uint32_t length_of_fmt; uint16_t format_type;
  uint16_t channels; uint32_t sample_rate; uint32_t byterate;
  uint16_t block_align; uint16_t bits_per_sample;
  char data_chunk_header[4]; uint32_t data_size;
};

static void write_wav_header(FILE *f, int channels, int rate, int bits, uint32_t data_bytes) {
  struct wav_hdr h;
  memcpy(h.riff,"RIFF",4); h.overall_size = 36 + data_bytes;
  memcpy(h.wave,"WAVE",4); memcpy(h.fmt_chunk_marker,"fmt ",4);
  h.length_of_fmt = 16; h.format_type = 1; h.channels = (uint16_t)channels;
  h.sample_rate = (uint32_t)rate; h.bits_per_sample = (uint16_t)bits;
  h.byterate = rate * channels * bits/8;
  h.block_align = channels * bits/8;
  memcpy(h.data_chunk_header,"data",4); h.data_size = data_bytes;
  fwrite(&h, sizeof(h), 1, f);
}

/* Try to open a capture PCM for a given device string. Returns 0 on success and sets handle. */
static int try_open_device(const char *dev, snd_pcm_t **handle, unsigned int rate, int channels, snd_pcm_format_t format, snd_pcm_uframes_t *out_period) {
  int err;
  snd_pcm_t *h = NULL;
  if ((err = snd_pcm_open(&h, dev, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf(stderr, "Versuche %s ... fehlgeschlagen: %s\n", dev, snd_strerror(err));
    return -1;
  }
  snd_pcm_hw_params_t *hw = NULL;
  snd_pcm_hw_params_malloc(&hw);
  snd_pcm_hw_params_any(h, hw);
  snd_pcm_hw_params_set_access(h, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(h, hw, format);
  snd_pcm_hw_params_set_rate_near(h, hw, &rate, 0);
  snd_pcm_hw_params_set_channels(h, hw, channels);
  snd_pcm_uframes_t period_size = 1024;
  snd_pcm_hw_params_set_period_size_near(h, hw, &period_size, 0);
  if ((err = snd_pcm_hw_params(h, hw)) < 0) {
    fprintf(stderr, "hw_params für %s fehlgeschlagen: %s\n", dev, snd_strerror(err));
    snd_pcm_hw_params_free(hw);
    snd_pcm_close(h);
    return -1;
  }
  snd_pcm_hw_params_free(hw);
  *handle = h;
  if (out_period) *out_period = period_size;
  fprintf(stderr, "Erfolgreich geöffnet: %s (rate=%u, channels=%d, period=%lu)\n", dev, rate, channels, (unsigned long)period_size);
  return 0;
}

/* Print short list of cards to help debugging */
static void print_cards_brief(void) {
  int card = -1;
  if (snd_card_next(&card) < 0 || card < 0) {
    fprintf(stderr, "Keine ALSA-Karten gefunden.\n");
    return;
  }
  fprintf(stderr, "Gefundene ALSA-Karten:\n");
  snd_ctl_card_info_t *info;
  snd_ctl_card_info_malloc(&info);
  while (card >= 0) {
    char ctlname[32];
    snprintf(ctlname, sizeof(ctlname), "hw:%d", card);
    snd_ctl_t *ctl = NULL;
    if (snd_ctl_open(&ctl, ctlname, 0) >= 0) {
      if (snd_ctl_card_info(ctl, info) >= 0) {
        const char *name = snd_ctl_card_info_get_name(info);
        const char *longname = snd_ctl_card_info_get_longname(info);
        fprintf(stderr, "  card %d: %s - %s\n", card, name?name:"(null)", longname?longname:"(null)");
      }
      snd_ctl_close(ctl);
    }
    if (snd_card_next(&card) < 0) break;
  }
  snd_ctl_card_info_free(info);
}

int main(void){
  unsigned int rate = 44100;
  int channels = 2;
  snd_pcm_t *handle = NULL;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
  snd_pcm_uframes_t period_size = 0;
  const char *candidates[] = {
    "hw:Loopback,1,0",
    "plughw:Loopback,1,0",
    "hw:Loopback,0,0",
    "plughw:Loopback,0,0",
    "default",
    "pulse",
    "plughw:0,0",
    NULL
  };

  signal(SIGINT, handle_sigint);

  fprintf(stderr, "Suche nach Loopback/geeignetem Capture-Device...\n");
  int i = 0;
  int opened = 0;
  while (candidates[i]) {
    if (try_open_device(candidates[i], &handle, rate, channels, format, &period_size) == 0) { opened = 1; break; }
    i++;
  }

  if (!opened) {
    fprintf(stderr, "Kein geeignetes Capture-Device geöffnet.\n");
    print_cards_brief();
    fprintf(stderr, "Wenn du Loopback verwenden willst: sudo modprobe snd-aloop\n");
    return 2;
  }

  int bytes_per_sample = 2;
  int bytes_per_frame = channels * bytes_per_sample;
  size_t bufsize = period_size * bytes_per_frame;
  char *buffer = malloc(bufsize);
  if (!buffer) { fprintf(stderr, "malloc fehlgeschlagen\n"); snd_pcm_close(handle); return 1; }

  time_t t = time(NULL);
  struct tm tm;
  localtime_r(&t, &tm);
  char filename[64];
  strftime(filename, sizeof(filename), "%y%m%d-%H%M%S.wav", &tm);

  FILE *f = fopen(filename, "wb");
  if (!f) { fprintf(stderr, "Kann Datei %s nicht öffnen\n", filename); free(buffer); snd_pcm_close(handle); return 1; }

  write_wav_header(f, channels, rate, bytes_per_sample * 8, 0);
  uint32_t data_written = 0;

  fprintf(stderr, "Aufnahme gestartet -> %s (Device: %s). Ctrl+C zum Beenden.\n", filename, candidates[i]);

  while (running) {
    int r = snd_pcm_readi(handle, buffer, period_size);
    if (r == -EPIPE) {
      fprintf(stderr, "Overrun, prepare...\n");
      snd_pcm_prepare(handle);
      continue;
    }
    if (r < 0) {
      fprintf(stderr, "Read error: %s\n", snd_strerror(r));
      break;
    }
    if (r > 0) {
      size_t wrote = fwrite(buffer, bytes_per_frame, r, f);
      if (wrote != (size_t)r) {
        fprintf(stderr, "Fehler beim Schreiben\n");
        break;
      }
      data_written += r * bytes_per_frame;
    }
  }

  fseek(f, 0, SEEK_SET);
  write_wav_header(f, channels, rate, bytes_per_sample * 8, data_written);
  fclose(f);
  free(buffer);
  snd_pcm_close(handle);

  fprintf(stderr, "Aufnahme beendet. Datei: %s (Datenbytes: %u)\n", filename, data_written);
  return 0;
}
