#include <GLUT/glut.h>
#include <iostream>
using namespace std;

float shx = 0.5f; // shear factor along X-axis
float shy = 0.0f; // no shear along Y-axis for now

void init(void){
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glPointSize(5.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 800, 0, 800);
}

// Apply shearing to a point (x,y)
void shearPoint(float x, float y, float &xs, float &ys)
{
    xs = x + shx * y;
    ys = y + shy * x;
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    // First square (sheared)
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    float xs, ys;
    shearPoint(150, 200, xs, ys); glVertex2f(xs, ys);
    shearPoint(350, 200, xs, ys); glVertex2f(xs, ys);
    shearPoint(350, 250, xs, ys); glVertex2f(xs, ys);
    shearPoint(150, 250, xs, ys); glVertex2f(xs, ys);
    glEnd();

    // Second square translated and sheared
    int tx = 200, ty = 200;
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_QUADS);
    shearPoint(150 + tx, 200 + ty, xs, ys); glVertex2f(xs, ys);
    shearPoint(350 + tx, 200 + ty, xs, ys); glVertex2f(xs, ys);
    shearPoint(350 + tx, 250 + ty, xs, ys); glVertex2f(xs, ys);
    shearPoint(150 + tx, 250 + ty, xs, ys); glVertex2f(xs, ys);
    glEnd();

    glFlush();
}

int main(int argc, char** argv){

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(300,0);
    glutCreateWindow("Shearing");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}