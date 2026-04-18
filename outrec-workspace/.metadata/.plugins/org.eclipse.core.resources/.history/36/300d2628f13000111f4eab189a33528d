/* alsa_loop_rec.c
   Record from ALSA capture device and write simple WAV (S16_LE).
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <stdint.h>

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
  h.length_of_fmt = 16; h.format_type = 1; h.channels = channels;
  h.sample_rate = rate; h.bits_per_sample = bits;
  h.byterate = rate * channels * bits/8;
  h.block_align = channels * bits/8;
  memcpy(h.data_chunk_header,"data",4); h.data_size = data_bytes;
  fwrite(&h, sizeof(h), 1, f);
}

int main(int argc,char **argv){
  if(argc<5){ fprintf(stderr,"Usage: %s <device> <out.wav> <rate> <channels>\n",argv[0]); return 1; }
  char *dev = argv[1]; char *out = argv[2];
  unsigned int rate = atoi(argv[3]); int channels = atoi(argv[4]);
  snd_pcm_t *handle; snd_pcm_hw_params_t *hw; int err;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
  if((err = snd_pcm_open(&handle, dev, SND_PCM_STREAM_CAPTURE, 0))<0){
    fprintf(stderr,"Cannot open device %s (%s)\n", dev, snd_strerror(err)); return 1;
  }
  snd_pcm_hw_params_malloc(&hw);
  snd_pcm_hw_params_any(handle, hw);
  snd_pcm_hw_params_set_access(handle, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle, hw, format);
  snd_pcm_hw_params_set_rate_near(handle, hw, &rate, 0);
  snd_pcm_hw_params_set_channels(handle, hw, channels);
  snd_pcm_hw_params(handle, hw);
  snd_pcm_hw_params_free(hw);
  int frames = 1024;
  int bytes_per_frame = channels * 2;
  char *buffer = malloc(frames * bytes_per_frame);
  FILE *f = fopen(out,"wb");
  write_wav_header(f, channels, rate, 16, 0);
  uint32_t data_written = 0;
  while(1){
    int r = snd_pcm_readi(handle, buffer, frames);
    if(r == -EPIPE){ snd_pcm_prepare(handle); continue; }
    if(r < 0){ fprintf(stderr,"Read error: %s\n", snd_strerror(r)); break; }
    fwrite(buffer, bytes_per_frame, r, f);
    data_written += r * bytes_per_frame;
    fflush(f);
  }
  fseek(f,0,SEEK_SET);
  write_wav_header(f, channels, rate, 16, data_written);
  fclose(f);
  free(buffer);
  snd_pcm_close(handle);
  return 0;
}
