/**
 * rad.c
 * * Fiktive SDL2-Simulation des OSPREY 2.0 Detektionsalgorithmus zur 
 * Sichtbarmachung von MfS-Substanz "Radionullon" (RN).
 * * Der Algorithmus nutzt die bekannten Schwachstellen von Radionullon aus:
 * 1. Physiologische Thermische Anomalie (IR/Infrarot-Signatur).
 * 2. Zeitliche Instabilität (Onset und Kollaps des Effekts).
 * 3. Akute Toxizität (simulierter V-12 Kreislaufversagen).
 * * Kompilierungsbefehl (Linux/macOS, vorausgesetzt SDL2 ist installiert):
 * gcc rad.c -o rad $(sdl2-config --cflags --libs) -lm
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// ====================================================================
// I. FIKTIVE PARAMETER (Basierend auf Aktenanalyse)
// ====================================================================

// Zeitschwelle für den vollen Effekt (Onset): ca. 90 Sekunden
#define ONSET_TIME_MS 90000 
// Maximale Wirkungsdauer vor dem Kollaps: 15 Minuten
#define COLLAPSE_TIME_MS 900000 
// Kreislaufversagen (V-12) nach 18 Minuten (15 Min + 3 Min Puffer)
#define FAILURE_TIME_MS 1080000 

// ====================================================================
// II. DATENSTRUKTUREN
// ====================================================================

// Physiologische Zustandsvariable basierend auf Fall V-12
typedef struct {
    float ThermalSignature;   // Simulierter IR-Anstieg (0.0 bis 1.0)
    bool CirculatoryFailure;  // Kreislaufversagen (T > 18 Minuten)
} SubjectVitals;

// System-Container
typedef struct {
    SDL_Window *Window;
    SDL_Renderer *Renderer;
    bool Running;
    long CurrentTimeMs; 
    
    // Multispektrale Kanäle
    SDL_Texture *ChannelVIS;      // Sichtbarer Kanal (400-700 nm)
    SDL_Texture *ChannelIR;       // Infrarot-Kanal (Thermisch)
    SDL_Texture *ChannelFiltered; // Ergebnis der Multispektralen Analyse

    SubjectVitals Vitals; 
} OSPREY_System;

// ====================================================================
// III. CORE-FUNKTIONEN
// ====================================================================

/**
 * @brief Initialisiert SDL2 und simuliert die Sensor-Eingangsdaten (Texturen).
 */
bool InitializeSystem(OSPREY_System *sys) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Fehler: %s\n", SDL_GetError());
        return false;
    }

    sys->Window = SDL_CreateWindow("RADIONULLOLITH - OSPREY 2.0 DETEKTION",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   800, 600, SDL_WINDOW_SHOWN);
    if (!sys->Window) {
        fprintf(stderr, "SDL_CreateWindow Fehler: %s\n", SDL_GetError());
        return false;
    }

    sys->Renderer = SDL_CreateRenderer(sys->Window, -1, SDL_RENDERER_ACCELERATED);
    if (!sys->Renderer) {
        fprintf(stderr, "SDL_CreateRenderer Fehler: %s\n", SDL_GetError());
        return false;
    }
    
    sys->CurrentTimeMs = 0;
    sys->Running = true;
    sys->Vitals.CirculatoryFailure = false;
    sys->Vitals.ThermalSignature = 0.0f;
    
    // Initialisiere drei Texturen für die Verarbeitungskanäle
    int w = 800, h = 600;
    sys->ChannelVIS = SDL_CreateTexture(sys->Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    sys->ChannelIR = SDL_CreateTexture(sys->Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    sys->ChannelFiltered = SDL_CreateTexture(sys->Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    // *Simuliere* die Eingangsdaten:
    // 1. VIS-Kanal: Hintergrund (grau) + ein Subjekt (weiß, das unsichtbar werden soll)
    SDL_SetRenderTarget(sys->Renderer, sys->ChannelVIS);
    SDL_SetRenderDrawColor(sys->Renderer, 50, 50, 50, 255); // Hintergrund (Urbanes Grau)
    SDL_RenderClear(sys->Renderer);
    SDL_SetRenderDrawColor(sys->Renderer, 200, 200, 200, 255); // Subjekt-Silouhette (Weiß)
    SDL_Rect subjectRect = {350, 250, 100, 100}; 
    SDL_RenderFillRect(sys->Renderer, &subjectRect);

    // 2. IR-Kanal: Nur das Ziel (RN-Subjekt) ist ein deutlicher Hotspot.
    SDL_SetRenderTarget(sys->Renderer, sys->ChannelIR);
    SDL_SetRenderDrawColor(sys->Renderer, 0, 0, 0, 255); 
    SDL_RenderClear(sys->Renderer);
    SDL_SetRenderDrawColor(sys->Renderer, 255, 50, 0, 255); // Rote Thermalsignatur
    SDL_RenderFillRect(sys->Renderer, &subjectRect);

    SDL_SetRenderTarget(sys->Renderer, NULL);
    
    printf("OSPREY 2.0 System Initialisiert. Starte Radionullon-Detektion...\n");
    return true;
}

/**
 * @brief Gibt SDL-Ressourcen frei.
 */
void CleanupSystem(OSPREY_System *sys) {
    if (sys->ChannelVIS) SDL_DestroyTexture(sys->ChannelVIS);
    if (sys->ChannelIR) SDL_DestroyTexture(sys->ChannelIR);
    if (sys->ChannelFiltered) SDL_DestroyTexture(sys->ChannelFiltered);
    if (sys->Renderer) SDL_DestroyRenderer(sys->Renderer);
    if (sys->Window) SDL_DestroyWindow(sys->Window);
    SDL_Quit();
}

/**
 * @brief Simuliert die Überlagerung und Filterung von VIS- und IR-Kanälen (Der Kern).
 */
void PerformMultispectralAnalysis(OSPREY_System *sys) {
    SDL_SetRenderTarget(sys->Renderer, sys->ChannelFiltered);
    SDL_SetRenderDrawColor(sys->Renderer, 0, 0, 0, 255);
    SDL_RenderClear(sys->Renderer);

    // --- A. IR-Detektion (Primärer Detektionsvektor) ---
    
    // Die Deckkraft des IR-Kanals spiegelt die physiologische Signatur wider.
    int alphaIR = (int)(255 * sys->Vitals.ThermalSignature); 
    
    SDL_SetTextureColorMod(sys->ChannelIR, 255, 0, 0); // Rot = Hitzesignatur
    SDL_SetTextureAlphaMod(sys->ChannelIR, (Uint8)alphaIR);
    SDL_RenderCopy(sys->Renderer, sys->ChannelIR, NULL, NULL);

    // --- B. VIS-Analyse (Sekundärer, zeitabhängiger Vektor) ---
    
    float visOpacity = 0.0f;
    Uint8 modR = 255, modG = 255, modB = 255; // Standard-Farbe

    if (sys->CurrentTimeMs < ONSET_TIME_MS) {
        // PHASE 1: ONSET (0 - 90s). Die Unsichtbarkeit baut sich auf.
        float onsetProgress = (float)sys->CurrentTimeMs / ONSET_TIME_MS;
        visOpacity = 1.0f - onsetProgress; // Von 1.0 (sichtbar) zu 0.0 (invisibel)
        printf("STATUS: ONSET-Phase. Optische Signatur nimmt ab (Opazität: %.2f)\n", visOpacity);
    } else if (sys->CurrentTimeMs > COLLAPSE_TIME_MS) {
        // PHASE 3: KOLLAPS (> 15 min). Der Effekt zerfällt.
        visOpacity = 1.0f;
        modR = 255; modG = 255; modB = 0; // Gelb markiert chemischen Zerfall
        printf("ALARM: RN-Effekt KOLLABIERT! VIS-Signatur voll hergestellt.\n");
    } else {
        // PHASE 2: EFFEKTIVES FENSTER. Optimale Unsichtbarkeit.
        visOpacity = 0.05f; // <5% Sichtbarkeit (Restrauschen/Artefakte)
        printf("WARNUNG: HÖCHSTE IR-ANOMALIE (%d%%). Ziel optisch neutralisiert.\n", (int)(sys->Vitals.ThermalSignature * 100));
    }
    
    // Rendere den VIS-Kanal über den IR-Kanal
    SDL_SetTextureColorMod(sys->ChannelVIS, modR, modG, modB);
    SDL_SetTextureAlphaMod(sys->ChannelVIS, (Uint8)(255 * visOpacity));
    SDL_SetTextureBlendMode(sys->ChannelVIS, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(sys->Renderer, sys->ChannelVIS, NULL, NULL);
    
    SDL_SetRenderTarget(sys->Renderer, NULL); 
}


/**
 * @brief Haupt-Detektions-Frame-Prozessor (OSPREY 2.0).
 */
void ProcessDetectionFrame(OSPREY_System *sys) {
    // 1. PHYSIOLOGISCHE ZUSTANDSANALYSE (Simuliere V-12 Bericht)
    
    // Die thermische Signatur steigt im Laufe der Zeit durch Hitzestress an.
    // Sie erreicht den Maximalwert kurz vor dem Kollaps.
    sys->Vitals.ThermalSignature = fminf(1.0f, (float)sys->CurrentTimeMs / (COLLAPSE_TIME_MS * 0.8f));
    
    if (sys->CurrentTimeMs >= FAILURE_TIME_MS) {
        sys->Vitals.CirculatoryFailure = true;
    }

    // 2. MULTISPEKTRALE ANALYSE DURCHFÜHREN
    PerformMultispectralAnalysis(sys);

    // 3. FINAL OUTPUT RENDERING
    
    SDL_RenderCopy(sys->Renderer, sys->ChannelFiltered, NULL, NULL);

    // 4. KRITISCHER STATUS-OVERLAY (Visuelle Bestätigung)
    if (sys->Vitals.CirculatoryFailure) {
         // Markiere den Bildschirm als "Fatal Error"
         SDL_SetRenderDrawColor(sys->Renderer, 150, 0, 0, 150); 
         SDL_RenderFillRect(sys->Renderer, &(SDL_Rect){0, 0, 800, 600});
         printf(">>> VORFALL V-12: KREISLAUFVERSAGEN NACH %ld MS. ZIEL NICHT REAKTIONSAKTIV. <<<\n", sys->CurrentTimeMs);
    }
    
    SDL_RenderPresent(sys->Renderer);
}

// ====================================================================
// V. MAIN LOOP
// ====================================================================

int main(int argc, char *argv[]) {
    OSPREY_System sys = {0};
    
    if (!InitializeSystem(&sys)) {
        CleanupSystem(&sys);
        return 1;
    }

    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();
    
    while (sys.Running) {
        // Event-Verarbeitung
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                sys.Running = false;
            }
        }

        // Zeit-Aktualisierung (Simuliere den Fortschritt)
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        sys.CurrentTimeMs += deltaTime;
        
        // Führe die Detektionslogik aus
        ProcessDetectionFrame(&sys);

        // Frame-Rate Begrenzung (ca. 30 FPS)
        SDL_Delay(33); 
    }

    CleanupSystem(&sys);
    return 0;
}
