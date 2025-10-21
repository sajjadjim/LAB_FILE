#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

int x_s = 0, y_s = 0, r = 100;  

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(3.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-200.0, 200.0, -200.0, 200.0);
}

void plot(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x_s + x, y_s + y);
    glVertex2i(x_s - x, y_s + y);
    glVertex2i(x_s + x, y_s - y);
    glVertex2i(x_s - x, y_s - y);
    glVertex2i(x_s + y, y_s + x);
    glVertex2i(x_s - y, y_s + x);
    glVertex2i(x_s + y, y_s - x);
    glVertex2i(x_s - y, y_s - x);
    glEnd();

    cout << "Points: "
         << "(" << x_s + x << "," << y_s + y << "), "
         << "(" << x_s - x << "," << y_s + y << "), "
         << "(" << x_s + x << "," << y_s - y << "), "
         << "(" << x_s - x << "," << y_s - y << "), "
         << "(" << x_s + y << "," << y_s + x << "), "
         << "(" << x_s - y << "," << y_s + x << "), "
         << "(" << x_s + y << "," << y_s - x << "), "
         << "(" << x_s - y << "," << y_s - x << ")"
         << endl;
}

void circle() {
    int x = 0;
    int y = r;
    int p = 1 - r;

    cout << "Midpoint Circle Drawing Algorithm" << endl;
    cout << "Radius: " << r << endl;
    cout << "Center: (" << x_s << ", " << y_s << ")" << endl;
    cout << "---------------------------------------------" << endl;

    while (x <= y) {
        plot(x, y);
        x++;
        if (p < 0) {
            p = p + 2 * x + 1;
        } else {
            y--;
            p = p + 2 * (x - y) + 1;
        }
    }
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    circle();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Midpoint Circle Drawing Algorithm — S.M. Sajjad Hossain JIM");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}