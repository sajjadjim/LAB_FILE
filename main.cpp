#include <GLUT/glut.h> 
#include <cmath>
#include <iostream>
using namespace std;

float X1, Y1, X2, Y2; 
float dx, dy; 

const float L = -90.0f;
const float R = 90.0f;
const float B = -90.0f;
const float T = 90.0f;

void init(){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-100, 100, -100, 100);
    glMatrixMode(GL_MODELVIEW);
}

void draw_rectangle(float x1, float y1, float x2, float y2){
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void draw_boundary(){
    glBegin(GL_LINE_LOOP); 
    glColor3f(0.0f, 1.0f, 0.0f); 
    glVertex2f(L, B);
    glVertex2f(R, B);
    glVertex2f(R, T);
    glVertex2f(L, T);
    glEnd();
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    draw_boundary();
    draw_rectangle(X1, Y1, X2, Y2); 
    
    glFlush();
}

void timer(int){
    
    float wdh = X2 - X1;
    float hght = Y2 - Y1;

    if (X1 + dx < L || X2 + dx > R) {
        dx = -dx; 
        if (X1 < L) { 
            X1 = L; 
            X2 = L + wdh; 
        }
        if (X2 > R) { 
            X2 = R; 
            X1 = R - wdh;
         }
    }

    if (Y1 + dy < B || Y2 + dy > T) {
        dy = -dy; 
        if (Y1 < B) { 
            Y1 = B; 
            Y2 = B + hght; 
        }
        if (Y2 > T) {
             Y2 = T; 
             Y1 = T - hght; 
            }
    }
    
    X1 += dx;
    X2 += dx;
    Y1 += dy;
    Y2 += dy;

    glutPostRedisplay();
    glutTimerFunc(10, timer, 0); 
}

int main(int argc, char** argv){

    // cout << "Enter inner rectangle corner 1 (x1 y1, e.g., -50 0): ";
    cout << "Enter inner rectangle corner 1 : ";
    cin >> X1 >> Y1;
    // cout << "Enter inner rectangle corner 2 (x2 y2, e.g., -40 10): ";
    cout << "Enter inner rectangle corner 2 : ";
    cin >> X2 >> Y2;
    // cout << "Enter diagonal speed (dx, dy, e.g., 2 1): "; 
    cout << "Enter diagonal speed : "; 
    cin >> dx >> dy; 

    if (dx == 0.0f && dy == 0.0f) {
        cout << "Warning: Speed set to (0, 0). Setting default speed to (1, 1).\n";
        dx = 1.0f;
        dy = 1.0f;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("2D Diagonal Bouncing Rectangle");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(16, timer, 0); 
    glutMainLoop();
    return 0;
}