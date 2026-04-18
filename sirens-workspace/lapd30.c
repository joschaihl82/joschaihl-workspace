#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

// --- KONSTANTEN ---
#define SAMPLE_RATE 44100       // Standard-Audio-Sample-Rate
#define CHANNELS 1              // Mono-Audio
#define BUFFER_SIZE 256         // Frames pro ALSA-Buffer
#define PI 3.14159265358979323846

// Sirenen-Parameter
#define F_BASE 261.63           // Grundfrequenz C4 (tiefster Ton)
#define MOD_HALFTONES 10.0      // Modulationsbereich (Pitch-Shift) in Halbtönen

// --- GLOBALE VARIABLEN ---
// Speichert den Zustand der Synthese für kontinuierliche Töne
static double time_counter = 0.0;
static double lfo_phase = 0.0; 

// --- FUNKTIONEN ---

/**
 * @brief Initialisiert das ALSA-PCM-Gerät und setzt Parameter.
 * @param handle Zeiger auf den ALSA-Handle.
 * @return 0 bei Erfolg, -1 bei Fehler.
 */
int init_alsa(snd_pcm_t **handle) {
    int rc;
    snd_pcm_hw_params_t *params;
    
    // 1. Gerät öffnen
    rc = snd_pcm_open(handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Fehler: ALSA PCM-Gerät kann nicht geöffnet werden: %s\n", snd_strerror(rc));
        return -1;
    }

    // 2. Hardware-Parameter zuweisen
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(*handle, params);

    // 3. Parameter setzen (Format, Kanäle, Rate)
    // Interleaved Access (i), 16-bit signed, Little Endian Format
    snd_pcm_hw_params_set_access(*handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(*handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(*handle, params, CHANNELS);
    
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(*handle, params, &rate, 0);

    // 4. Puffergröße setzen (Frames)
    snd_pcm_uframes_t frames = BUFFER_SIZE;
    snd_pcm_hw_params_set_period_size_near(*handle, params, &frames, 0);
    
    // 5. Parameter anwenden
    rc = snd_pcm_hw_params(*handle, params);
    if (rc < 0) {
        fprintf(stderr, "Fehler: ALSA Hardware-Parameter können nicht gesetzt werden: %s\n", snd_strerror(rc));
        snd_pcm_close(*handle);
        return -1;
    }

    printf("ALSA initialisiert: Rate=%u Hz, Frames=%lu\n", rate, frames);
    return 0;
}

/**
 * @brief Generiert Audio-Frames für die Sirene.
 * @param buffer Der Puffer, in den die 16-Bit-Samples geschrieben werden.
 * @param frames Die Anzahl der zu generierenden Frames.
 * @param lfo_rate Die Frequenz des LFOs (Hz) für Wail oder Yelp.
 */
void generate_siren_audio(short *buffer, int frames, double lfo_rate) {
    for (int i = 0; i < frames; i++) {
        // 1. LFO (Dreieckswelle) Phase aktualisieren
        // Der Modulo-Reset sorgt für einen kontinuierlichen, nicht-unterbrochenen LFO
        lfo_phase = fmod(lfo_phase + 2.0 * PI * lfo_rate / SAMPLE_RATE, 2.0 * PI);
        
        // Dreieckswelle von -1.0 bis 1.0 (approximiert durch Arcsin(Sin(Phase)))
        double lfo_value = 2.0 / PI * asin(sin(lfo_phase)); 

        // 2. Frequenzberechnung (Pitch-Shift)
        // current_frequency = F_BASE * 2^(LFO_Value * Halbtöne / 12)
        double current_frequency = F_BASE * pow(2.0, (lfo_value * MOD_HALFTONES) / 12.0);

        // 3. Time-Counter zur Ton-Generierung (Phasenakku)
        time_counter += current_frequency / SAMPLE_RATE;
        // Modulo 1.0 hält den Zähler im Bereich [0, 1]
        time_counter = fmod(time_counter, 1.0);

        // 4. Rechteckwelle (Signal-Generierung)
        double signal = (time_counter < 0.5) ? 1.0 : -1.0; 
        
        // 5. Konvertierung und Speichern im 16-Bit-Buffer
        // Multipliziere mit 32767.0 für maximale Amplitude eines 'short'
        // Eine leichte Reduzierung (z.B. 0.8) vermeidet Clipping
        buffer[i] = (short)(signal * 32767.0 * 0.8);
    }
}

/**
 * @brief Spielt einen Modus (Wail oder Yelp) für eine bestimmte Dauer.
 * @param handle Der ALSA-Handle.
 * @param rate Die LFO-Rate (0.4 Hz für Wail, 2.5 Hz für Yelp).
 * @param duration_sec Die Dauer in Sekunden.
 */
void play_siren_mode(snd_pcm_t *handle, double rate, double duration_sec) {
    short *buffer = malloc(BUFFER_SIZE * CHANNELS * sizeof(short));
    int total_frames = (int)(duration_sec * SAMPLE_RATE);
    int frames_remaining = total_frames;

    while (frames_remaining > 0) {
        int frames_to_write = frames_remaining > BUFFER_SIZE ? BUFFER_SIZE : frames_remaining;

        // Audio generieren
        generate_siren_audio(buffer, frames_to_write, rate);
        
        // Audio ausgeben
        int rc = snd_pcm_writei(handle, buffer, frames_to_write);
        
        if (rc == -EPIPE) {
            // Buffer Underrun (XRUN): Wiederherstellen
            snd_pcm_prepare(handle);
            continue; 
        } else if (rc < 0) {
            fprintf(stderr, "Fehler beim Schreiben des Audio-Frames: %s\n", snd_strerror(rc));
            break;
        }

        frames_remaining -= rc;
    }
    
    free(buffer);
}

// --- HAUPTPROGRAMM ---
int main() {
    snd_pcm_t *handle = NULL;

    // 1. ALSA initialisieren
    if (init_alsa(&handle) != 0) {
        return 1;
    }

    // Optional: Warten, bis der Benutzer bereit ist (gibt Zeit, Lautstärke anzupassen)
    printf("\nBereit. Drücken Sie Enter, um die Sirene zu starten (Achtung: laut).\n");
    getchar();
    
    // 2. WAIL-Modus starten (Langsamer Ton)
    printf("\n--- WAIL (0.4 Hz) für 6 Sekunden ---\n");
    play_siren_mode(handle, 0.4, 6.0);
    
    // 3. Kurze Pause
    usleep(200000); // 200ms Pause
    
    // 4. YELP-Modus starten (Schneller Ton)
    printf("\n--- YELP (2.5 Hz) für 6 Sekunden ---\n");
    play_siren_mode(handle, 2.5, 6.0);

    // 5. Cleanup
    printf("\nBeendet. ALSA schliessen.\n");
    snd_pcm_drain(handle); // Warte, bis alle Samples gespielt wurden
    snd_pcm_close(handle);

    return 0;
}
