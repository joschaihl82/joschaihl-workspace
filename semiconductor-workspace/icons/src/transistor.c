/*
 * demo.c
 * Kompilieren (Linux): gcc demo.c -o demo -lGL -lGLU -lglut -lm
 * Kompilieren (macOS): clang -Wno-deprecated demo.c -o demo -framework OpenGL -framework GLUT
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>

#define PI 3.14159265359

// --- Hilfsfunktionen für Geometrie ---

// Setzt Farbe via Hex (z.B. 0xFF0000)
void setColorHex(int hex) {
    float r = ((hex >> 16) & 0xFF) / 255.0f;
    float g = ((hex >> 8) & 0xFF) / 255.0f;
    float b = ((hex) & 0xFF) / 255.0f;
    glColor3f(r, g, b);
}

// Zeichnet ein rotierbares Rechteck
void drawRectRotated(float x, float y, float w, float h, float angleDeg) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(angleDeg, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2f(-w/2, -h/2);
    glVertex2f( w/2, -h/2);
    glVertex2f( w/2,  h/2);
    glVertex2f(-w/2,  h/2);
    glEnd();
    glPopMatrix();
}

// Zeichnet einen Kreisbogen oder Ring
// startAngle/endAngle in Grad
void drawArc(float cx, float cy, float r, float thickness, float startAngle, float endAngle) {
    int segments = 40;
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float angDeg = startAngle + t * (endAngle - startAngle);
        float angRad = angDeg * PI / 180.0f;

        float c = cosf(angRad);
        float s = sinf(angRad);

        glVertex2f(cx + (r - thickness/2) * c, cy + (r - thickness/2) * s);
        glVertex2f(cx + (r + thickness/2) * c, cy + (r + thickness/2) * s);
    }
    glEnd();
}

// Zeichnet einen abgerundeten Hintergrund-Button (wie im Bild)
void drawRoundedBox(float x, float y, float size, float radius) {
    int segments = 20;
    float half = size / 2.0f;
    float r = radius;

    // Wir nutzen ein Polygon für die Füllung (vereinfacht)
    // In echtem Production-Code würde man Triangle-Fans für die Ecken nehmen

    glColor3f(0.96f, 0.96f, 0.96f); // Ganz helles Grau (Fast Weiß)

    glBegin(GL_POLYGON);
    // Ecke oben rechts
    for(int i=0; i<=segments; i++) {
        float theta = 0 + (PI/2.0f * i / segments);
        glVertex2f(x + half - r + r*cosf(theta), y + half - r + r*sinf(theta));
    }
    // Ecke oben links
    for(int i=0; i<=segments; i++) {
        float theta = PI/2.0f + (PI/2.0f * i / segments);
        glVertex2f(x - half + r + r*cosf(theta), y + half - r + r*sinf(theta));
    }
    // Ecke unten links
    for(int i=0; i<=segments; i++) {
        float theta = PI + (PI/2.0f * i / segments);
        glVertex2f(x - half + r + r*cosf(theta), y - half + r + r*sinf(theta));
    }
    // Ecke unten rechts
    for(int i=0; i<=segments; i++) {
        float theta = 3.0f*PI/2.0f + (PI/2.0f * i / segments);
        glVertex2f(x + half - r + r*cosf(theta), y - half + r + r*sinf(theta));
    }
    glEnd();
}

// --- ICON IMPLEMENTIERUNGEN ---

// 1. Move (Verschieben)
void iconMove(float size) {
    setColorHex(0x000000);
    float thick = size * 0.08f;
    float len = size * 0.7f;

    // Kreuz
    drawRectRotated(0, 0, len, thick, 0);
    drawRectRotated(0, 0, len, thick, 90);

    // Pfeilspitzen (Dreiecke)
    float tipSize = size * 0.15f;
    float offset = len/2 + tipSize/4;

    // Wir rotieren einfach das Koordinatensystem 4 mal für die 4 Pfeile
    for(int i=0; i<4; i++) {
        glPushMatrix();
        glRotatef(90 * i, 0, 0, 1);
        glTranslatef(len/2, 0, 0);

        glBegin(GL_TRIANGLES);
        glVertex2f(tipSize, 0);
        glVertex2f(0, -tipSize*0.7f);
        glVertex2f(0,  tipSize*0.7f);
        glEnd();

        glPopMatrix();
    }

    // Kleine Ecken-Pfeile (diagonal) wie im Bild
    float cornerOff = size * 0.35f;
    float cornerS = size * 0.12f;
    for(int i=0; i<4; i++) {
        glPushMatrix();
        glRotatef(45 + 90*i, 0, 0, 1);
        glTranslatef(cornerOff, 0, 0);
        glBegin(GL_LINES); // Nur kleine Striche oder Pfeile
        // Vereinfacht als Pfeil nach außen
        glVertex2f(0, 0); glVertex2f(-cornerS, cornerS/2);
        glVertex2f(0, 0); glVertex2f(-cornerS, -cornerS/2);
        glEnd();
        glPopMatrix();
    }
}

// 2. & 3. Zoom (Lupe)
void iconZoom(float size, int mode) { // mode 1 = Plus, 0 = Minus
    setColorHex(0x000000);
    float r = size * 0.32f;
    float thick = size * 0.08f;

    // Glas-Ring
    drawArc(0, 0, r, thick, 0, 360);

    // Griff
    float handleL = size * 0.35f;
    float handleW = thick;
    // Position Griff: 45 Grad nach unten rechts
    float diag = r;
    float hX = diag * cosf(-45 * PI/180);
    float hY = diag * sinf(-45 * PI/180);

    glPushMatrix();
    glTranslatef(hX, hY, 0);
    glRotatef(-45, 0, 0, 1);
    // Griff zeichnen (Offset damit er am Ring startet)
    drawRectRotated(handleL/2, 0, handleL, handleW, 0);
    glPopMatrix();

    // Symbol (+ oder -)
    float symLen = r * 1.0f;
    drawRectRotated(0, 0, symLen, thick*0.8f, 0); // Minus
    if (mode == 1) {
        drawRectRotated(0, 0, symLen, thick*0.8f, 90); // Plus
    }
}

// 4. Refresh (Pfeil im Kreis)
void iconRefresh(float size) {
    setColorHex(0x000000);
    float r = size * 0.35f;
    float thick = size * 0.07f;

    // Bogen (Nicht ganz geschlossen)
    // Zeichnen wir von -30 Grad bis 240 Grad (270 Grad Umfang)
    // Wir drehen das ganze Icon etwas, damit die Öffnung rechts ist
    glPushMatrix();
    glRotatef(-45, 0, 0, 1);

    drawArc(0, 0, r, thick, 0, 270);

    // Pfeilspitze am Ende (bei 270 Grad)
    float tipX = r * cosf(270 * PI/180);
    float tipY = r * sinf(270 * PI/180);
    float tipSize = size * 0.18f;

    glPushMatrix();
    glTranslatef(tipX, tipY, 0);
    // Spitze passend drehen
    glRotatef(180, 0, 0, 1);

    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0);
    glVertex2f(-tipSize, -tipSize/2);
    glVertex2f(-tipSize/3, 0); // Einbuchtung für schärferen Pfeil
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0);
    glVertex2f(-tipSize/3, 0);
    glVertex2f(-tipSize, tipSize/2);
    glEnd();

    glPopMatrix();
    glPopMatrix();
}

// 5. Grid (Gitter)
void iconGrid(float size) {
    // Blaues Gitter
    setColorHex(0x0088CC);
    float len = size * 0.7f;
    float thick = size * 0.05f;
    float gap = size * 0.25f;

    // Vertikal
    drawRectRotated(-gap, 0, thick, len, 0);
    drawRectRotated(0,    0, thick, len, 0);
    drawRectRotated(gap,  0, thick, len, 0);

    // Horizontal
    drawRectRotated(0, -gap, len, thick, 0);
    drawRectRotated(0, 0,    len, thick, 0);
    drawRectRotated(0, gap,  len, thick, 0);
}

// 6. Magnet
void iconMagnet(float size) {
    setColorHex(0x000000);
    float w = size * 0.3f;
    float h = size * 0.35f; // Höhe des geraden Teils
    float thick = size * 0.14f;

    // Linker Balken
    drawRectRotated(-w + thick/2, h/2, thick, h, 0);
    // Rechter Balken
    drawRectRotated(w - thick/2, h/2, thick, h, 0);

    // Bogen unten
    drawArc(0, 0, w - thick/2, thick, 180, 360);

    // Die weißen Trennlinien an den Polen
    glColor3f(1, 1, 1); // Weiß
    // Trennung oben
    float cutH = size * 0.04f;
    drawRectRotated(-w + thick/2, h * 0.6f, thick+2, cutH, 0);
    drawRectRotated( w - thick/2, h * 0.6f, thick+2, cutH, 0);
}


// --- MAIN LOOP ---

void display() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // Weißer Hintergrund
    glClear(GL_COLOR_BUFFER_BIT);

    // Anti-Aliasing aktivieren für schöne Vektoren
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Wir wollen 6 Icons nebeneinander zeichnen
    // Gesamtbreite der Szene ist etwa von -1.0 bis 1.0
    float startX = -0.8f;
    float gap = 0.32f;
    float iconSize = 0.25f; // Größe des Hintergrund-Buttons
    float symbolScale = 0.6f; // Größe des Symbols relativ zum Button

    // Array von Funktionszeigern oder Switch wäre elegant,
    // hier explizit für Lesbarkeit:

    // 1. Move
    glPushMatrix();
    glTranslatef(startX + gap*0, 0, 0);
    drawRoundedBox(0, 0, iconSize, iconSize*0.2f);
    iconMove(iconSize * symbolScale);
    glPopMatrix();

    // 2. Zoom In
    glPushMatrix();
    glTranslatef(startX + gap*1, 0, 0);
    drawRoundedBox(0, 0, iconSize, iconSize*0.2f);
    iconZoom(iconSize * symbolScale, 1);
    glPopMatrix();

    // 3. Zoom Out
    glPushMatrix();
    glTranslatef(startX + gap*2, 0, 0);
    drawRoundedBox(0, 0, iconSize, iconSize*0.2f);
    iconZoom(iconSize * symbolScale, 0);
    glPopMatrix();

    // 4. Refresh
    glPushMatrix();
    glTranslatef(startX + gap*3, 0, 0);
    drawRoundedBox(0, 0, iconSize, iconSize*0.2f);
    iconRefresh(iconSize * symbolScale);
    glPopMatrix();

    // 5. Grid
    glPushMatrix();
    glTranslatef(startX + gap*4, 0, 0);
    drawRoundedBox(0, 0, iconSize, iconSize*0.2f);
    iconGrid(iconSize * symbolScale);
    glPopMatrix();

    // 6. Magnet
    glPushMatrix();
    glTranslatef(startX + gap*5, 0, 0);
    drawRoundedBox(0, 0, iconSize, iconSize*0.2f);
    iconMagnet(iconSize * symbolScale);
    glPopMatrix();

    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Seitenverhältnis korrigieren damit Icons nicht verzerrt sind
    float aspect = (float)w / (float)h;
    if (w >= h) {
        // Breitbild: x anpassen
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // Hochformat: y anpassen
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_MULTISAMPLE); // Multisample für AA wenn möglich
    glutInitWindowSize(1000, 300); // Fenstergröße passend für die Leiste
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Icon Vektor Demo (OpenGL/C)");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
