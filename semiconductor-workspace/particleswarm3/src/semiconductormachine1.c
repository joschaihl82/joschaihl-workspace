/*
 * river_pso_glut.c
 *
 * Einfacher Flusssimulator:
 * - GLUT + GLEW + GLSL
 * - Partikel-Schwarm (PSO-artig) auf einem 2D-Geschwindigkeitsfeld
 * - Geometrie-Idee aus LBM-Beispiel:
 *     dicker Hauptfluss links, zwei dünne Seitenarme rechts
 * - speed_factor: schmale Kanäle -> höhere Geschwindigkeit
 *
 * Build (Linux, Beispiel):
 *   gcc river_pso_glut.c -lGL -lGLEW -lglut -lm -o river_pso_glut
 */

#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NX  200
#define NY  100
#define NUM_PARTICLES 20000

typedef struct {
    float x, y;
    float vx, vy;
} Particle;

static Particle *particles = NULL;
static int windowWidth  = 1280;
static int windowHeight = 720;

static unsigned char obstacle[NX*NY];   // 1 = Wand, 0 = Fluid
static float speed_factor[NX*NY];       // schmaler Kanal -> >1
static float ux_field[NX*NY];
static float uy_field[NX*NY];

static GLuint particleVBO = 0;
static GLuint shaderProgram = 0;

static float globalTime = 0.0f;
static float dt = 0.016f;

// ---------------- Hilfsfunktionen ----------------

static inline int IDX(int x, int y) { return y * NX + x; }

static void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

// ---------------- Shader ----------------

static const char *vsSrc =
"#version 120\n"
"attribute vec2 aPos;\n"
"void main() {\n"
"    gl_PointSize = 2.0;\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}\n";

static const char *fsSrc =
"#version 120\n"
"void main() {\n"
"    // einfaches Wasserblau\n"
"    gl_FragColor = vec4(0.1, 0.5, 0.9, 1.0);\n"
"}\n";

static void checkCompile(GLuint obj, GLenum type) {
    GLint status = 0;
    if (type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER) {
        glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
        if (!status) {
            char log[1024];
            glGetShaderInfoLog(obj, sizeof(log), NULL, log);
            fprintf(stderr, "Shader compile error:\n%s\n", log);
            die("shader compile failed");
        }
    } else if (type == GL_PROGRAM) {
        glGetProgramiv(obj, GL_LINK_STATUS, &status);
        if (!status) {
            char log[1024];
            glGetProgramInfoLog(obj, sizeof(log), NULL, log);
            fprintf(stderr, "Program link error:\n%s\n", log);
            die("program link failed");
        }
    }
}

static GLuint createShaderProgram(void) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs, 1, &vsSrc, NULL);
    glShaderSource(fs, 1, &fsSrc, NULL);
    glCompileShader(vs);
    checkCompile(vs, GL_VERTEX_SHADER);
    glCompileShader(fs);
    checkCompile(fs, GL_FRAGMENT_SHADER);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glBindAttribLocation(prog, 0, "aPos");
    glLinkProgram(prog);
    checkCompile(prog, GL_PROGRAM);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// ---------------- Geometrie: dicker Fluss + dünne Arme ----------------

static void init_geometry(void) {
    memset(obstacle, 1, sizeof(obstacle)); // alles Wand

    int mid = NY / 2;
    int big_radius = NY / 6;      // dicker Hauptfluss
    int narrow_radius = NY / 12;  // dünne Arme

    // dicker horizontaler Kanal links
    for (int y = mid - big_radius; y <= mid + big_radius; ++y) {
        for (int x = 5; x < NX/2; ++x) {
            obstacle[IDX(x,y)] = 0;
        }
    }

    // zwei schmale Arme rechts
    int branch_x = NX / 2;
    int upper_center = mid - (big_radius + narrow_radius);
    int lower_center = mid + (big_radius + narrow_radius);

    for (int x = branch_x; x < NX - 5; ++x) {
        for (int y = 0; y < NY; ++y) {
            if (abs(y - upper_center) <= narrow_radius) obstacle[IDX(x,y)] = 0;
            if (abs(y - lower_center) <= narrow_radius) obstacle[IDX(x,y)] = 0;
        }
    }

    // Y-Split-Region verbinden
    for (int x = NX/2 - 4; x < NX/2 + 8; ++x) {
        for (int y = mid - big_radius; y <= mid + big_radius; ++y) {
            obstacle[IDX(x,y)] = 0;
        }
    }

    // speed_factor: schmale Kanäle -> schneller
    for (int y = 0; y < NY; ++y) {
        for (int x = 0; x < NX; ++x) {
            int id = IDX(x,y);
            if (obstacle[id]) {
                speed_factor[id] = 0.0f;
                continue;
            }
            int up = y, down = y;
            while (up > 0 && !obstacle[IDX(x,up-1)])   up--;
            while (down < NY-1 && !obstacle[IDX(x,down+1)]) down++;
            int local_width = down - up + 1;

            float base_wide = (float)(NY/6);
            float base_narrow = (float)(NY/12);
            float factor = 1.0f + (base_wide - (float)local_width) / base_narrow;
            if (factor < 1.0f) factor = 1.0f;
            if (factor > 3.0f) factor = 3.0f;
            speed_factor[id] = factor;
        }
    }
}

// ---------------- Geschwindigkeitsfeld ----------------

/*
 * Wir definieren ein analytisches Feld:
 * - Grundrichtung: nach rechts
 * - Inlet-Amplitude sinusförmig (wie im LBM-Code)
 * - Magnitude skaliert mit speed_factor
 * - leichte Krümmung / Wirbel für "Wellen"
 */
static void update_velocity_field(float t) {
    float inlet_base = 0.04f;
    float inlet = inlet_base * (0.5f + 0.5f * sinf(t)); // 0..inlet_base

    for (int y = 0; y < NY; ++y) {
        for (int x = 0; x < NX; ++x) {
            int id = IDX(x,y);
            if (obstacle[id]) {
                ux_field[id] = 0.0f;
                uy_field[id] = 0.0f;
                continue;
            }

            float fx = (float)x / (float)(NX - 1); // 0..1
            float fy = (float)y / (float)(NY - 1);

            // Grundrichtung: nach rechts
            float vx = 1.0f;
            float vy = 0.0f;

            // leichte Meander / Wirbel
            vx += 0.3f * sinf(6.0f * fy + 2.0f * fx + t * 0.5f);
            vy += 0.3f * cosf(5.0f * fx + 3.0f * fy + t * 0.4f);

            // Normalisieren
            float len = sqrtf(vx*vx + vy*vy);
            if (len < 1e-5f) len = 1e-5f;
            vx /= len;
            vy /= len;

            // Inlet-Puls: links stärker, rechts schwächer
            float inlet_profile = inlet * (1.0f - 0.3f * fx);

            // speed_factor: schmale Kanäle -> schneller
            float sf = speed_factor[id];
            float mag = inlet_profile * sf * 10.0f; // etwas verstärken

            ux_field[id] = vx * mag;
            uy_field[id] = vy * mag;
        }
    }
}

// ---------------- Partikel ----------------

static void respawn_particle(int i) {
    // in der Nähe des linken Hauptflusses spawnen
    int tries = 0;
    while (tries < 50) {
        int x = 8 + rand() % 20;
        int y = NY/2 - (NY/6) + rand() % (NY/3);
        if (x < 0 || x >= NX || y < 0 || y >= NY) { tries++; continue; }
        if (obstacle[IDX(x,y)]) { tries++; continue; }

        particles[i].x = (float)x + (float)rand() / (float)RAND_MAX;
        particles[i].y = (float)y + (float)rand() / (float)RAND_MAX;
        particles[i].vx = 0.0f;
        particles[i].vy = 0.0f;
        return;
    }
    // Fallback
    particles[i].x = 5.0f;
    particles[i].y = (float)(NY/2);
    particles[i].vx = particles[i].vy = 0.0f;
}

static void init_particles(void) {
    particles = (Particle*)malloc(sizeof(Particle) * NUM_PARTICLES);
    if (!particles) die("particles malloc failed");
    srand((unsigned)time(NULL));
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        respawn_particle(i);
    }
}

/*
 * PSO-artiges Update:
 * - Partikel "wollen" der lokalen Flussgeschwindigkeit folgen
 * - Velocity wird an ux_field/uy_field angezogen
 * - kleine Rauschkomponente für Wellen
 */
static void update_particles(float dt) {
    const float attraction = 4.0f;
    const float damping    = 0.3f;
    const float noiseAmp   = 0.5f;

    for (int i = 0; i < NUM_PARTICLES; ++i) {
        Particle *p = &particles[i];

        int ix = (int)floorf(p->x);
        int iy = (int)floorf(p->y);

        if (ix < 1 || ix >= NX-1 || iy < 1 || iy >= NY-1 ||
            obstacle[IDX(ix,iy)]) {
            respawn_particle(i);
            continue;
        }

        // bilineare Interpolation des Geschwindigkeitsfeldes
        float fx = p->x - ix;
        float fy = p->y - iy;
        float ux = 0.0f, uy = 0.0f;
        for (int dy = 0; dy <= 1; ++dy) {
            for (int dx = 0; dx <= 1; ++dx) {
                int sx = ix + dx;
                int sy = iy + dy;
                float wgt = (dx ? fx : (1.0f - fx)) * (dy ? fy : (1.0f - fy));
                ux += ux_field[IDX(sx,sy)] * wgt;
                uy += uy_field[IDX(sx,sy)] * wgt;
            }
        }

        // PSO-artig: Velocity Richtung (ux,uy)
        p->vx += attraction * (ux - p->vx) * dt;
        p->vy += attraction * (uy - p->vy) * dt;

        // Dämpfung
        p->vx *= (1.0f - damping * dt);
        p->vy *= (1.0f - damping * dt);

        // Rauschen
        float nx = ((float)rand() / (float)RAND_MAX - 0.5f) * noiseAmp;
        float ny = ((float)rand() / (float)RAND_MAX - 0.5f) * noiseAmp;
        p->vx += nx * dt;
        p->vy += ny * dt;

        // Positionsupdate
        p->x += p->vx * dt;
        p->y += p->vy * dt;

        // außerhalb / in Wand -> respawn
        if (p->x < 1.0f || p->x >= (float)(NX-1) ||
            p->y < 1.0f || p->y >= (float)(NY-1) ||
            obstacle[IDX((int)p->x,(int)p->y)]) {
            respawn_particle(i);
        }
    }
}

// ---------------- OpenGL / GLUT ----------------

static void upload_particles(void) {
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    float *data = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (data) {
        for (int i = 0; i < NUM_PARTICLES; ++i) {
            // in Normalized Device Coordinates umrechnen: x in [0,NX] -> [-1,1]
            float nx = (particles[i].x / (float)(NX-1)) * 2.0f - 1.0f;
            float ny = (particles[i].y / (float)(NY-1)) * 2.0f - 1.0f;
            data[2*i + 0] = nx;
            data[2*i + 1] = ny;
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glutSwapBuffers();
}

static void idle(void) {
    globalTime += dt;
    update_velocity_field(globalTime);
    update_particles(dt);
    upload_particles();
    glutPostRedisplay();
}

static void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

static void initGL(void) {
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        die("glewInit failed");
    }
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glDisable(GL_DEPTH_TEST);

    shaderProgram = createShaderProgram();

    glGenBuffers(1, &particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 2 * sizeof(float),
                 NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ---------------- main ----------------

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Flusssimulator (GLUT + PSO + Branches)");

    initGL();
    init_geometry();
    init_particles();
    update_velocity_field(0.0f);
    upload_particles();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
