/*
================================================================================
                               LAB REPORT
================================================================================

Experiment Name: 2D Animation of a Moving Object with Sound
Course Code:     [Insert Code]
Submitted By:    [Your Name]
ID:              [Your ID]
Date:            [Current Date]

--------------------------------------------------------------------------------
1. OBJECTIVE
--------------------------------------------------------------------------------
The objective of this experiment is to design a 2D graphical object (a Boat) 
using OpenGL primitives and animate its movement across the screen using 
geometric translation. Additionally, the project integrates audio to play 
background sound, demonstrating multimedia handling in a C++ graphics environment.

--------------------------------------------------------------------------------
2. THEORY
--------------------------------------------------------------------------------
A. Geometric Translation:
   Translation shifts an object from one position to another. If a point is at 
   P(x, y), its new position P'(x', y') after translation by (tx, ty) is:
       x' = x + tx
       y' = y + ty
   In this experiment, we increment 'tx' continuously to simulate horizontal 
   movement.

B. Animation Loop:
   Motion is achieved by the "Clear-Update-Draw" cycle:
   1. Clear the screen.
   2. Update the object's position variable.
   3. Draw the object at the new position.
   4. Swap the display buffers (Double Buffering).

C. Audio Integration (macOS):
   We use the Unix system command `afplay` to play audio files. The ampersand 
   (&) is used to run the audio in a background thread so it does not freeze 
   the graphics animation.

--------------------------------------------------------------------------------
3. ALGORITHM
--------------------------------------------------------------------------------
1. Initialize the boat's starting X-position to the left edge of the screen.
2. Execute the system command `afplay boat_sound.wav &` to start audio.
3. In the display function:
   a. Clear the background (Blue color for sky/water).
   b. Draw the boat relative to the current X-position.
   c. Draw static elements (like water waves or sun).
   d. Swap buffers.
4. In the timer function:
   a. Increase the X-position by a speed factor.
   b. If X-position > Screen Width, reset to Left Edge (Looping).
   c. Request redisplay.
5. Repeat step 4 every 16ms (~60 FPS).

================================================================================
                               END OF REPORT
================================================================================
*/

#include <GLUT/glut.h>  // Mac-specific GLUT header
#include <math.h>
#include <cstdlib>      // For system() command

// Global Variables
float boatX = -350.0f;  // Start position (Left side)
float speed = 2.5f;     // Movement speed

// --- Helper Function to Draw a Circle (for the Sun) ---
void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// --- Function to Draw the Boat Object ---
void drawBoat(float x, float y) {
    // 1. Boat Hull (Trapezoid - Brown)
    glColor3f(0.6, 0.3, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(x - 60, y);       // Bottom Left
        glVertex2f(x + 60, y);       // Bottom Right
        glVertex2f(x + 90, y + 40);  // Top Right
        glVertex2f(x - 90, y + 40);  // Top Left
    glEnd();

    // 2. Boat Cabin (Rectangle - White)
    glColor3f(0.9, 0.9, 0.9);
    glBegin(GL_QUADS);
        glVertex2f(x - 40, y + 40);
        glVertex2f(x + 40, y + 40);
        glVertex2f(x + 40, y + 80);
        glVertex2f(x - 40, y + 80);
    glEnd();

    // 3. Windows (Blue Squares)
    glColor3f(0.3, 0.7, 1.0);
    glBegin(GL_QUADS);
        // Window 1
        glVertex2f(x - 30, y + 50);
        glVertex2f(x - 10, y + 50);
        glVertex2f(x - 10, y + 70);
        glVertex2f(x - 30, y + 70);
        // Window 2
        glVertex2f(x + 10, y + 50);
        glVertex2f(x + 30, y + 50);
        glVertex2f(x + 30, y + 70);
        glVertex2f(x + 10, y + 70);
    glEnd();

    // 4. Chimney/Exhaust (Dark Grey)
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
        glVertex2f(x - 10, y + 80);
        glVertex2f(x + 10, y + 80);
        glVertex2f(x + 10, y + 100);
        glVertex2f(x - 10, y + 100);
    glEnd();
}

// --- Main Display Function ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // 1. Draw Sky (Light Blue Background set in init)
    
    // 2. Draw Sun (Yellow) - Static
    glColor3f(1.0, 1.0, 0.0);
    drawCircle(200, 200, 40, 30);

    // 3. Draw Water (Deep Blue Rect at bottom) - Static
    glColor3f(0.0, 0.4, 0.8);
    glBegin(GL_QUADS);
        glVertex2f(-350, -350);
        glVertex2f( 350, -350);
        glVertex2f( 350, -100);
        glVertex2f(-350, -100);
    glEnd();

    // 4. Draw Moving Boat
    // Placing it nicely on top of the water line (-100)
    drawBoat(boatX, -100); 

    glutSwapBuffers();
}

// --- Animation Timer ---
void timer(int) {
    boatX += speed; // Move right

    // Loop logic: If boat leaves right side, reappear on left
    if (boatX > 400) {
        boatX = -400;
    }

    glutPostRedisplay();
    glutTimerFunc(200, timer, 0); // ~60 FPS
}

void init() {
    glClearColor(0.5, 0.8, 1.0, 1.0); // Sky Blue Background
    gluOrtho2D(-350, 350, -350, 350);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutCreateWindow("2D Boat Animation with Sound");

    init();

    // --- AUDIO COMMAND (MAC SPECIFIC) ---
    // Uses standard macOS 'afplay' command. 
    // The '&' ensures it runs in background.
    system("afplay sound.wav &");

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}