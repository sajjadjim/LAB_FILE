#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

void init(void){
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glPointSize(5.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0,800,0,800);
}

struct Point {
    float x, y;
};

Point rotatePoint(float px, float py, float cx, float cy, float angle){
    float rad = angle * M_PI / 180.0;
    float cosA = cos(rad);
    float sinA = sin(rad);
    float x_new = cx + (px - cx) * cosA - (py - cy) * sinA;
    float y_new = cy + (px - cx) * sinA + (py - cy) * cosA;
    return {x_new, y_new};
}

void drawRotatedQuad(float angle, float tx, float ty, float r, float g, float b){
    Point p1 = rotatePoint(150 + tx, 200 + ty, 250 + tx, 225 + ty, angle);
    Point p2 = rotatePoint(350 + tx, 200 + ty, 250 + tx, 225 + ty, angle);
    Point p3 = rotatePoint(350 + tx, 250 + ty, 250 + tx, 225 + ty, angle);
    Point p4 = rotatePoint(150 + tx, 250 + ty, 250 + tx, 225 + ty, angle);

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
        glVertex2f(p4.x, p4.y);
    glEnd();

    cout << "Rotated Quad Points (angle " << angle << "°, translation " << tx << "," << ty << "):" << endl;
    cout << "P1: (" << p1.x << ", " << p1.y << ")" << endl;
    cout << "P2: (" << p2.x << ", " << p2.y << ")" << endl;
    cout << "P3: (" << p3.x << ", " << p3.y << ")" << endl;
    cout << "P4: (" << p4.x << ", " << p4.y << ")" << endl;
    cout << "--------------------------------" << endl;
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    drawRotatedQuad(30, 0, 0, 1.0, 0.0, 0.0);
    drawRotatedQuad(30, 200, 200, 1.0, 1.0, 0.0);
    glFlush();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(300,0);
    glutCreateWindow("Rotation 30 Degree");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}