#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(3.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void dda(int x0, int y0, int x1, int y1, float r, float g, float b) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    float x_inc = (float)dx / steps;
    float y_inc = (float)dy / steps;

    float x = x0;
    float y = y0;

    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex2f(x / 500.0, y / 500.0);
        x += x_inc;
        y += y_inc;
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    int x, y;
    cout << "Enter endpoint coordinate (x y): ";
    cin >> x >> y;

    dda(0, 0, x, y, 1.0, 0.0, 0.0);
    dda(0, y, x, 0, 0.0, 1.0, 0.0);

    dda(-500, 0, 500, 0, 0.3, 0.3, 1.0);
    dda(0, -500, 0, 500, 0.3, 0.3, 1.0);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 200);
    glutCreateWindow("S.M. Sajjad Hossain JIM — DDA Cross Positive Quadrant");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}