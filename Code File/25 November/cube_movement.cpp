#include <GLUT/glut.h> 
#include <cmath>
#include <iostream>
using namespace std;

float X1, Y1, Z1, X2, Y2, Z2; 
float move_x = 0.0f;
float move_y = 0.0f;
float move_z = 0.0f;
const float STEP = 1.0f; 

void init(){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void rec(float x1, float y1, float z1, float x2, float y2, float z2){
    glBegin(GL_QUADS);
    
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y2, z1);
    glVertex3f(x1, y2, z1);

    glColor3f(0.0f, 0.0f, 1.0f); 
    glVertex3f(x1, y1, z2);
    glVertex3f(x2, y1, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z2);

    glColor3f(0.0f, 1.0f, 0.0f); 
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y2, z1);
    glVertex3f(x1, y2, z2);
    glVertex3f(x1, y1, z2);

    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x2, y2, z1);

    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(x1, y2, z1);
    glVertex3f(x2, y2, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z2);

    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x1, y1, z2);
    
    glEnd();
}

void display(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    

    gluLookAt(15.0, 15.0, 15.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);


    glTranslatef(move_x, move_y, move_z);
    
    rec(X1, Y1, Z1, X2, Y2, Z2);
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'a':
        case 'A':
            move_x -= STEP;
            break;
        case 'd':
        case 'D':
            move_x += STEP;
            break;
        case 'w': 
        case 'W': 
            move_z -= STEP;
            break;
        case 's': 
        case 'S':
            move_z += STEP;
            break;
        case 32: 
            move_y += STEP;
            break;
        case 27:
            move_y -= STEP;
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_L:
            move_x -= STEP;
            break;
        case GLUT_KEY_R:
            move_x += STEP;
            break;
        case GLUT_KEY_UP:
            move_y += STEP;
            break;
        case GLUT_KEY_DOWN:
            move_y -= STEP;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv){

    cout << "Enter corner 1 (x1 y1 z1): ";
    cin >> X1 >> Y1 >> Z1;
    cout << "Enter corner 2 (x2 y2 z2): ";
    cin >> X2 >> Y2 >> Z2;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("3D Cube Translation (WASD & Arrows)");
    
    init();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    glutMainLoop();
    return 0;
}