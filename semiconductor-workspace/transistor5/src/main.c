/* main.c
 * Start, GLUT/GLEW Setup, Callbacks
 */

#include "app.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("CAD - modular");

    /* GLEW initialisieren (nach Fenster) */
    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        fprintf(stderr, "ERROR: glewInit failed: %s\n", glewGetErrorString(glewErr));
    } else {
        const GLubyte *glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
        const GLubyte *vendor = glGetString(GL_VENDOR);
        const GLubyte *renderer = glGetString(GL_RENDERER);
        const GLubyte *version = glGetString(GL_VERSION);
        fprintf(stderr, "GL_VENDOR:   %s\n", vendor ? (const char*)vendor : "NULL");
        fprintf(stderr, "GL_RENDERER: %s\n", renderer ? (const char*)renderer : "NULL");
        fprintf(stderr, "GL_VERSION:  %s\n", version ? (const char*)version : "NULL");
        fprintf(stderr, "GLSL:        %s\n", glsl ? (const char*)glsl : "NULL");
    }

    /* Render-Subsystem initialisieren */
    if (!render_init(1200, 800)) {
        fprintf(stderr, "Warning: render_init reported failure (CPU fallback may be used).\n");
    }

    /* UI initialisieren */
    ui_init();

    /* GLUT callbacks registrieren */
    glutDisplayFunc(ui_on_display);
    glutReshapeFunc(ui_on_reshape);
    glutMouseFunc(ui_on_mouse);
    glutMotionFunc(ui_on_motion);
    glutPassiveMotionFunc(ui_on_passive_motion);
    glutKeyboardFunc(ui_on_keyboard);

    /* Fenster auf Bildschirmgröße setzen, wenn möglich */
    int screen_w = glutGet(GLUT_SCREEN_WIDTH);
    int screen_h = glutGet(GLUT_SCREEN_HEIGHT);
    if (screen_w > 0 && screen_h > 0) {
        glutReshapeWindow(screen_w, screen_h);
        glutPositionWindow(0, 0);
        fprintf(stderr, "Window resized to screen: %d x %d\n", screen_w, screen_h);
    } else {
        fprintf(stderr, "Note: GLUT_SCREEN_WIDTH/HEIGHT returned 0; skipping maximize.\n");
    }

    glutMainLoop();

    /* never reached normally */
    render_shutdown();
    return 0;
}
