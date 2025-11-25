#include <GLUT/glut.h> 
#include <cmath>
#include <iostream>


using namespace std;

// FIX: Renamed global variables to avoid conflict with math.h functions (like y1)
float rect_x1, rect_x2, rect_y1, rect_y2; 
float tx, ty;
float movex = 0, movey = 0;

void init (void){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION); // Setup projection matrix
    gluOrtho2D(-100, 100, -100, 100);
    glMatrixMode(GL_MODELVIEW); // Switch back to modelview matrix
}

void rec(float x1_r, float y1_r, float x2_r, float y2_r)
{
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glVertex2f(x1_r, y1_r);
    glVertex2f(x2_r, y1_r);
    glVertex2f(x2_r, y2_r);
    glVertex2f(x1_r, y2_r);
    glEnd();
}

void display (void){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    // Apply the translation transformation
    glTranslatef(movex, movey, 0); 
    // Use the renamed global variables
    rec(rect_x1, rect_y1, rect_x2, rect_y2); 

    glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'a': 
        case 'A': // Move Left
            movex = movex - tx;
            break; // FIX: Added break
        case 'd':
        case 'D': // Move Right
            movex = movex + tx;
            break; // FIX: Added break
        case 'w':
        case 'W': // Move Up
            movey = movey + ty;
            break; // FIX: Added break
        case 's':
        case 'S': // Move Down
            movey = movey - ty;
            break; // FIX: Added break
    }
    glutPostRedisplay();
}

int main( int argc, char** argv){
    cout << "Enter rectangle corner 1 (x1 y1): ";
    // Use the renamed variables
    cin >> rect_x1 >> rect_y1; 
    
    cout << "Enter rectangle corner 2 (x2 y2): ";
    // Use the renamed variables
    cin >> rect_x2 >> rect_y2; 
    
    cout << "Translation unit (tx ty): ";
    // FIX: Correctly reading both tx and ty
    cin >> tx >> ty; 

    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 400);
    glutInitWindowPosition( 100, 200);
    glutCreateWindow("2D Rectangle Translation");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}