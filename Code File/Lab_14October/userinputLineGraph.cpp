// #include <windows.h> // Not needed on Mac
#include <GLUT/glut.h>
#include <stdlib.h>
#include <math.h>
#include<cmath>
#include <iostream>

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void dda(int x0 , int y0 , int xl, int yl){
    int dx = xl-x0;
    int dy = yl-y0;
    int step;
    if(abs(dx) > abs(dy)){
        step = abs(dx);
    }
    else {
        step = abs(dy);
    }
    float x_inc = (float)  dx/step;
    float y_inc = (float)  dy/step;

    float x = x0;
    float y = y0;
    glBegin(GL_POINTS);
    for(int i=0; i<step; i++){
        glVertex2f(x/500.0,y/500.0);
        x += x_inc;
        y += y_inc;
    }
    glEnd();
    glFlush();
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
int x0, y0, xl, yl;
    std::cout << "Enter x0 y0 xl yl: ";
    std::cin >> x0 >> y0 >> xl >> yl;
    // cin >> x0 >> y0 >> xl >> yl;
    dda(x0 , y0 , xl , yl);
    glFlush();


    // glBegin(GL_QUADS);
    //     glVertex2f(-0.5, -0.5);
    //     glVertex2f(0.5, -0.5);
    //     glVertex2f(0.5, 0.5);
    //     glVertex2f(-0.5,0.5);

    // glEnd();
    // glFlush();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 400);
    glutInitWindowPosition(100, 200);
    glutCreateWindow("SAJJAD HOSSAIN JIM");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}