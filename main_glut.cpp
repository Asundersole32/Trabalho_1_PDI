// main_glut.cpp
// Demonstração de cores, transparência e blending em OpenGL/GLUT
// Compilar:  g++ main_glut.cpp -o glut_demo -lGL -lGLU -lglut -lm

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Garante M_PI em qualquer compilador (MinGW, MSVC, etc.)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------- Estado global (controlado por teclado) ----------
float alpha1 = 0.5f;          // alpha do quadrado vermelho
float alpha2 = 0.5f;          // alpha do quadrado azul
int   blendOn = 1;            // liga/desliga GL_BLEND
int   drawOrder = 0;          // 0 = vermelho primeiro, 1 = azul primeiro
int   currentObj = 0;         // qual alpha o 'a'/'z' controla (0 = vermelho, 1 = azul)

// ---------- Inicialização ----------
void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // alpha blending padrão
}

// ---------- Primitivas coloridas ----------
void drawSquare(float x, float y, float s, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(x,     y);
        glVertex2f(x + s, y);
        glVertex2f(x + s, y + s);
        glVertex2f(x,     y + s);
    glEnd();
}

void drawTriangle(float x, float y, float s, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_TRIANGLES);
        glVertex2f(x,         y);
        glVertex2f(x + s,     y);
        glVertex2f(x + s/2.f, y + s);
    glEnd();
}

void drawCircle(float cx, float cy, float rad, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= 100; ++i) {
            float ang = 2.0f * (float)M_PI * i / 100.0f;
            glVertex2f(cx + rad * cosf(ang), cy + rad * sinf(ang));
        }
    glEnd();
}

// Quadrados transparentes que se sobrepõem
void drawTranspRed() {
    glColor4f(1.0f, 0.0f, 0.0f, alpha1);
    glBegin(GL_QUADS);
        glVertex2f(-0.7f, -0.8f);
        glVertex2f( 0.1f, -0.8f);
        glVertex2f( 0.1f,  0.0f);
        glVertex2f(-0.7f,  0.0f);
    glEnd();
}

void drawTranspBlue() {
    glColor4f(0.0f, 0.0f, 1.0f, alpha2);
    glBegin(GL_QUADS);
        glVertex2f(-0.3f, -0.4f);
        glVertex2f( 0.5f, -0.4f);
        glVertex2f( 0.5f,  0.4f);
        glVertex2f(-0.3f,  0.4f);
    glEnd();
}

// ---------- Display ----------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (blendOn) glEnable(GL_BLEND);
    else         glDisable(GL_BLEND);

    // -------- Objetos opacos (RGB puro, diferentes intensidades) --------
    drawSquare(-0.95f, 0.4f, 0.30f, 1.0f, 0.0f, 0.0f, 1.0f);  // vermelho puro
    drawTriangle(-0.55f, 0.4f, 0.30f, 0.0f, 1.0f, 0.0f, 1.0f); // verde puro
    drawCircle(0.0f, 0.55f, 0.15f, 0.0f, 0.0f, 1.0f, 1.0f);   // azul puro
    drawSquare(0.35f, 0.4f, 0.30f, 1.0f, 1.0f, 0.0f, 1.0f);   // amarelo (R+G)
    drawSquare(0.70f, 0.4f, 0.30f, 0.4f, 0.4f, 0.4f, 1.0f);   // cinza (intensidades iguais)

    // -------- Objetos transparentes: ordem de desenho alterável --------
    if (drawOrder == 0) { drawTranspRed(); drawTranspBlue(); }
    else                { drawTranspBlue(); drawTranspRed(); }

    glutSwapBuffers();
}

// ---------- Teclado ----------
void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'b': case 'B':
            blendOn = !blendOn;
            printf("Blend: %s\n", blendOn ? "ON" : "OFF");
            break;
        case 'r': case 'R':
            drawOrder = !drawOrder;
            printf("Ordem: %s\n", drawOrder == 0 ?
                   "Vermelho -> Azul" : "Azul -> Vermelho");
            break;
        case '1': currentObj = 0; printf("Controlando alpha1 (vermelho)\n"); break;
        case '2': currentObj = 1; printf("Controlando alpha2 (azul)\n");     break;
        case 'a': case 'A':
            if (currentObj == 0) alpha1 = fminf(1.0f, alpha1 + 0.1f);
            else                 alpha2 = fminf(1.0f, alpha2 + 0.1f);
            printf("alpha1=%.2f  alpha2=%.2f\n", alpha1, alpha2);
            break;
        case 'z': case 'Z':
            if (currentObj == 0) alpha1 = fmaxf(0.0f, alpha1 - 0.1f);
            else                 alpha2 = fmaxf(0.0f, alpha2 - 0.1f);
            printf("alpha1=%.2f  alpha2=%.2f\n", alpha1, alpha2);
            break;
        case 27: exit(0); // ESC
    }
    glutPostRedisplay();
}

// ---------- Reshape ----------
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h) gluOrtho2D(-1.2, 1.2, -1.2 * h / w, 1.2 * h / w);
    else        gluOrtho2D(-1.2 * w / h, 1.2 * w / h, -1.2, 1.2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(900, 650);
    glutCreateWindow("OpenGL - Cores, Transparencia e Blending");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    printf("Teclas:\n"
           "  b = liga/desliga blending\n"
           "  r = alterna ordem de desenho\n"
           "  1/2 = escolhe alpha a controlar\n"
           "  a/z = aumenta/diminui alpha\n"
           "  ESC = sair\n");
    glutMainLoop();
    return 0;
}