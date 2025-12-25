#include <GLUT/glut.h>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <algorithm> // used for std::max

/* ========================================================================
   SECTION 1: GLOBAL SETTINGS & VARIABLES
   ======================================================================== */

// Game States
enum GameMode { MENU, PLAYING, PAUSED, GAMEOVER };
GameMode mode = MENU;

// Scores
long distanceScore = 0;
long coinScore = 0;

// World Generation
long currentSegment = 0;
float roadOffset = 0.0f;
float scrollSpeed = 0.15f;
const float segmentLength = 2.0f;
const int visibleSegments = 40; // How far we can see

// Player
float playerX = 0.0f;
float playerY = 0.5f;
int currentLane = 0; // -1 = Left, 0 = Middle, 1 = Right
float targetX = 0.0f;

// Dimensions
const float laneWidth = 2.0f;
const float roadHalfWidth = 3.3f; 
float laneSpeed = 0.3f;

// Physics (Jumping)
bool isJumping = false;
float velY = 0.0f;
const float GRAVITY = 0.025f;
const float JUMP_FORCE = 0.35f;

// Animation
float windmillAngle = 0.0f;

// Data Structures for Obstacles and Coins
struct Car { long seg; int lane; };
struct Coin { long seg; int lane; bool collected; };

std::vector<Car> cars;
std::vector<Coin> coins;


/* ========================================================================
   SECTION 2: CUSTOM DRAWING ALGORITHMS (The Core Requirement)
   ======================================================================== */

// --------------------------------------------------------
// ALGORITHM 1: DDA Line Drawing
// Used for: Road lines, House walls, Character bodies
// --------------------------------------------------------
void drawLineDDA(float x1, float y1, float z1, float x2, float y2, float z2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    // Find the number of steps needed (based on the longest axis difference)
    float steps = std::max(std::abs(dx), std::max(std::abs(dy), std::abs(dz)));

    // Prevent crashing if drawing a point (steps = 0)
    if (steps == 0) {
        glBegin(GL_POINTS);
        glVertex3f(x1, y1, z1);
        glEnd();
        return;
    }

    // Calculate increment amount per step
    float xInc = dx / steps;
    float yInc = dy / steps;
    float zInc = dz / steps;

    float x = x1;
    float y = y1;
    float z = z1;

    // Draw the points
    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex3f(x, y, z);
        x += xInc;
        y += yInc;
        z += zInc;
    }
    glEnd();
}

// --------------------------------------------------------
// ALGORITHM 2: Midpoint Circle Algorithm
// Used for: Sun, Clouds, Coins, Heads, Doorknobs
// --------------------------------------------------------
void drawMidpointCirclePoints(int radius, float pixelScale) {
    int x = 0;
    int y = radius;
    int p = 1 - radius; // Initial decision parameter

    glBegin(GL_POINTS);
    while (x <= y) {
        // We scale the x,y by 'pixelScale' to convert integer math to 3D world size
        float fx = (float)x * pixelScale;
        float fy = (float)y * pixelScale;

        // Plot 8 Octants
        glVertex3f(fx, fy, 0);  
        glVertex3f(fy, fx, 0);
        glVertex3f(-fx, fy, 0); 
        glVertex3f(-fy, fx, 0);
        glVertex3f(-fx, -fy, 0);
        glVertex3f(-fy, -fx, 0);
        glVertex3f(fx, -fy, 0); 
        glVertex3f(fy, -fx, 0);

        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * x - 2 * y + 1;
        }
    }
    glEnd();
}

// Helper: Calls the Midpoint algorithm repeatedly to fill a circle
void drawFilledMidpointCircle(int radius, float scale) {
    for (int r = 0; r <= radius; r++) {
        drawMidpointCirclePoints(r, scale);
    }
}


/* ========================================================================
   SECTION 3: DRAWING GAME OBJECTS (Using the Algorithms)
   ======================================================================== */

// --- 1. Cartoon House (DDA Lines + Midpoint Circle Doorknob) ---
void drawCartoonHouse(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0, z);
    glScalef(2.5f, 2.5f, 2.5f); // Size of the house
    glLineWidth(3.0f); // Thicker lines for cartoon style

    // A. Walls (White) -> Using DDA
    glColor3f(1.0f, 1.0f, 1.0f);
    drawLineDDA(-1, 0, 0, -1, 1, 0); // Left Wall
    drawLineDDA(1, 0, 0, 1, 1, 0);   // Right Wall
    drawLineDDA(-1, 0, 0, 1, 0, 0);  // Floor
    drawLineDDA(-1, 1, 0, 1, 1, 0);  // Ceiling

    // B. Roof (Red) -> Using DDA
    glColor3f(1.0f, 0.0f, 0.0f);
    drawLineDDA(-1.2f, 1, 0, 0, 1.8f, 0); // Left Slope
    drawLineDDA(1.2f, 1, 0, 0, 1.8f, 0);  // Right Slope
    drawLineDDA(-1.2f, 1, 0, 1.2f, 1, 0); // Roof Base

    // C. Door (Brown) -> Using DDA
    glColor3f(0.6f, 0.3f, 0.1f);
    drawLineDDA(-0.3f, 0, 0, -0.3f, 0.6f, 0); // Door Left
    drawLineDDA(0.3f, 0, 0, 0.3f, 0.6f, 0);   // Door Right
    drawLineDDA(-0.3f, 0.6f, 0, 0.3f, 0.6f, 0); // Door Top

    // D. Doorknob (Yellow) -> Using Midpoint Circle
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.15f, 0.3f, 0.05f); // Position knob on door
    drawFilledMidpointCircle(3, 0.02f);
    glPopMatrix();

    glLineWidth(1.0f); 
    glPopMatrix();
}

// --- 2. Medium Cartoon Character (DDA Limbs + Midpoint Head) ---
void drawCartoonCharacter(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.7f, z); 
    glScalef(1.5f, 1.5f, 1.5f); // "Medium" size scaling
    
    // Simple bobbing animation
    float bob = sin(distanceScore * 0.1f + x) * 0.05f;
    glTranslatef(0, bob, 0);

    glLineWidth(3.0f); // Thick stick figure lines

    // A. Head (Skin color) -> Using Midpoint Circle
    glColor3f(1.0f, 0.8f, 0.6f); 
    glPushMatrix();
    glTranslatef(0, 0.6f, 0);
    drawFilledMidpointCircle(8, 0.02f);
    glPopMatrix();

    // B. Body (Green Shirt) -> Using DDA
    glColor3f(0.0f, 0.8f, 0.0f); 
    drawLineDDA(0, 0.6f, 0, 0, 0.2f, 0);

    // C. Arms (Green Sleeves) -> Using DDA
    float wave = std::abs(sin(distanceScore * 0.2f + z)) * 0.3f;
    drawLineDDA(0, 0.5f, 0, -0.3f, 0.4f, 0);       // Left arm
    drawLineDDA(0, 0.5f, 0, 0.3f, 0.4f + wave, 0); // Right arm (Waving)

    // D. Legs (Blue Pants) -> Using DDA
    glColor3f(0.0f, 0.0f, 0.8f); 
    drawLineDDA(0, 0.2f, 0, -0.2f, -0.5f, 0); // Left Leg
    drawLineDDA(0, 0.2f, 0, 0.2f, -0.5f, 0);  // Right Leg

    glLineWidth(1.0f);
    glPopMatrix();
}

// --- 3. Coin (Midpoint Circle) ---
void drawCoin(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.9f, z);
    glRotatef((float)(distanceScore % 360) * 4.0f, 0, 1, 0); // Spin animation

    glColor3f(1.0f, 0.85f, 0.0f); // Gold
    glPointSize(2.0f); // Thicker points
    
    // Draw 3 layers to give it thickness
    drawFilledMidpointCircle(12, 0.02f); // Center
    
    glPushMatrix(); 
    glTranslatef(0, 0, 0.05f); 
    drawFilledMidpointCircle(12, 0.02f); // Front
    glPopMatrix();
    
    glPushMatrix(); 
    glTranslatef(0, 0, -0.05f); 
    drawFilledMidpointCircle(12, 0.02f); // Back
    glPopMatrix();

    glPointSize(1.0f);
    glPopMatrix();
}

// --- 4. Cloud Helper (Midpoint Circles) ---
void drawCloud2D(float cx, float cy, float scale) {
    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glScalef(scale, scale * 0.6f, 1.0f); 

    glColor4f(1.0f, 1.0f, 1.0f, 0.8f); // Transparent White

    // Draw 4 overlapping circles to make a cloud shape
    glPushMatrix(); glTranslatef(-30, -10, 0); drawFilledMidpointCircle(25, 1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(  0,   0, 0); drawFilledMidpointCircle(35, 1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 30, -10, 0); drawFilledMidpointCircle(25, 1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 15,  15, 0); drawFilledMidpointCircle(20, 1.0f); glPopMatrix();
    
    glPopMatrix();
}

// --- 5. Attractive Background (Gradient + Clouds + Solid Sun) ---
void drawAttractiveBackground() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Switch to 2D view
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600); // 0,0 is bottom-left. 800,600 is top-right.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // A. Gradient Sky (Quad)
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.4f, 0.8f); // Deep Blue Top
    glVertex2f(0, 600); 
    glVertex2f(800, 600);
    glColor3f(0.6f, 0.8f, 1.0f); // Light Blue Bottom
    glVertex2f(800, 0); 
    glVertex2f(0, 0);
    glEnd();

    // B. Clouds (Transparent)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawCloud2D(150, 500, 1.2f);
    drawCloud2D(600, 450, 1.5f);
    drawCloud2D(350, 550, 1.0f);
    
    // C. Solid Sun (Upper Right Corner) -> Midpoint Circle
    glPushMatrix();
    glTranslatef(700, 520, 0); // Move to Upper Right (x=700, y=520)

    // Solid Yellow Sun
    glColor3f(1.0f, 1.0f, 0.0f); // Pure Yellow
    drawFilledMidpointCircle(40, 1.0f); // Radius 40
    
    glPopMatrix();

    glDisable(GL_BLEND);

    // Restore 3D view
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}


/* ========================================================================
   SECTION 4: 3D MODELS (Using GLUT for Solids)
   ======================================================================== */

void drawTree(float x, float z) {
    // Trunk
    glColor3f(0.55f, 0.27f, 0.07f);
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, 0.25f, 0.25f, 1.5f, 8, 1);
    gluDeleteQuadric(q);
    glPopMatrix();

    // Leaves
    glColor3f(0.1f, 0.7f, 0.1f);
    glPushMatrix();
    glTranslatef(x, 1.5f, z);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(1.0f, 2.3f, 10, 2);
    glPopMatrix();
}

void drawWindmill(float x, float z) {
    // Pole
    glColor3f(0.85f, 0.85f, 0.85f);
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, 0.5f, 0.25f, 5.0f, 12, 1);
    gluDeleteQuadric(q);
    glPopMatrix();

    // Blades
    glPushMatrix();
    glTranslatef(x, 5.0f, z);
    glRotatef(windmillAngle, 0, 0, 1);
    glColor3f(0.8f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(90.0f * i, 0, 0, 1);
        glTranslatef(0, 1.5f, 0);
        glScalef(0.4f, 3.0f, 0.1f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawRobot() {
    float runAnim = 0.0f;
    if (mode == PLAYING) runAnim = sin(distanceScore * 0.2f) * 30.0f;

    glPushMatrix();
    glTranslatef(playerX, playerY + 0.6f, 0.0f);
    glScalef(0.65f, 0.65f, 0.65f);

    // Torso
    glColor3f(0.2f, 0.2f, 0.8f);
    glPushMatrix(); glScalef(0.6f, 0.8f, 0.4f); glutSolidCube(1.0f); glPopMatrix();

    // Head
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix(); glTranslatef(0.0f, 0.7f, 0.0f); glutSolidSphere(0.35f, 12, 12); glPopMatrix();

    // Arms
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix(); glTranslatef(0.4f, 0.2f, 0.0f); glRotatef(runAnim, 1, 0, 0); glTranslatef(0.0f, -0.35f, 0.0f); glScalef(0.15f, 0.7f, 0.15f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.4f, 0.2f, 0.0f); glRotatef(-runAnim, 1, 0, 0); glTranslatef(0.0f, -0.35f, 0.0f); glScalef(0.15f, 0.7f, 0.15f); glutSolidCube(1.0f); glPopMatrix();

    // Legs
    glColor3f(0.2f, 0.2f, 0.6f);
    glPushMatrix(); glTranslatef(0.15f, -0.55f, 0.0f); glRotatef(-runAnim, 1, 0, 0); glTranslatef(0.0f, -0.45f, 0.0f); glScalef(0.2f, 0.9f, 0.2f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.15f, -0.55f, 0.0f); glRotatef(runAnim, 1, 0, 0); glTranslatef(0.0f, -0.45f, 0.0f); glScalef(0.2f, 0.9f, 0.2f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

void drawCar(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.35f, z);
    
    // Body
    glColor3f(0.85f, 0.1f, 0.1f); 
    glPushMatrix(); glScalef(1.4f, 0.6f, 2.0f); glutSolidCube(1.0f); glPopMatrix();
    
    // Top
    glColor3f(0.75f, 0.05f, 0.05f); 
    glPushMatrix(); glTranslatef(0.0f, 0.45f, -0.2f); glScalef(1.0f, 0.45f, 1.0f); glutSolidCube(1.0f); glPopMatrix();
    
    // Wheels
    glColor3f(0.1f, 0.1f, 0.1f);
    for (int sx = -1; sx <= 1; sx += 2) {
        for (int sz = -1; sz <= 1; sz += 2) {
            glPushMatrix(); glTranslatef(0.55f * sx, -0.35f, 0.75f * sz); glutSolidTorus(0.05f, 0.13f, 10, 16); glPopMatrix();
        }
    }
    glPopMatrix();
}


/* ========================================================================
   SECTION 5: WORLD GENERATION & LOGIC
   ======================================================================== */

// Simple random number generator using a hash function (determinstic based on segment)
static inline uint32_t hash32(uint32_t x) {
    x ^= x >> 16; x *= 0x7feb352dU; x ^= x >> 15;
    x *= 0x846ca68bU; x ^= x >> 16;
    return x;
}

// Helpers
float laneX(int lane) { return lane * laneWidth; }
float segmentZ(long seg) {
    long i = seg - currentSegment;
    return -(i * segmentLength + segmentLength * 0.5f);
}

// Draw 2D Text
void drawText(const char* s, float x, float y, float r, float g, float b) {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    while (*s) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s++);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// Calculate difficulty based on distance
int carChance() {
    int t = (int)(distanceScore / 1000);   
    int c = 5 + t * 5;                     
    if (c > 28) c = 28;                    
    return c;
}

// Remove old objects, keep vector size small
void cleanupOld() {
    long minSeg = currentSegment - 6;
    std::vector<Car> nc;
    for (auto &c : cars) if (c.seg >= minSeg) nc.push_back(c);
    cars.swap(nc);
    std::vector<Coin> nco;
    for (auto &c : coins) if (c.seg >= minSeg) nco.push_back(c);
    coins.swap(nco);
}

// Generate cars and coins
void spawn(long seg) {
    uint32_t h = hash32((uint32_t)seg ^ 0xA53C9E11U);
    int safeLane = (int)(h % 3) - 1; 

    // Spawn Cars
    for (int lane = -1; lane <= 1; lane++) {
        if (lane == safeLane) continue;
        uint32_t r = hash32(h ^ (lane + 7) * 123u);
        if ((int)(r % 100) < carChance()) cars.push_back({ seg, lane });
    }
    // Spawn Coins
    for (int lane = -1; lane <= 1; lane++) {
        uint32_t r = hash32(h ^ (lane + 9) * 999u);
        if ((int)(r % 100) < 30) {
            bool blocked = false;
            for (auto &cc : cars) {
                if (cc.seg == seg && cc.lane == lane) { blocked = true; break; }
            }
            if (!blocked) coins.push_back({ seg, lane, false });
        }
    }
}

// Reset everything
void resetGame() {
    mode = PLAYING;
    distanceScore = 0;
    coinScore = 0;
    currentSegment = 0;
    roadOffset = 0.0f;
    playerX = 0.0f;
    playerY = 0.5f;
    currentLane = 0;
    targetX = 0.0f;
    isJumping = false;
    velY = 0.0f;
    windmillAngle = 0.0f;
    cars.clear();
    coins.clear();
    // Fill initial world
    for (long s = 5; s < visibleSegments + 60; s++) spawn(s);
}

// Main Draw World Loop
void drawWorld() {
    glPushMatrix();
    glTranslatef(0, 0, roadOffset);

    // Draw visible road segments
    for (int i = -1; i < visibleSegments; i++) {
        long seg = currentSegment + i;
        float zn = -i * segmentLength;
        float zf = -(i + 1) * segmentLength;
        float zm = (zn + zf) * 0.5f;

        // 1. Highway Road (Grey)
        glColor3f(0.25f, 0.25f, 0.25f);
        glBegin(GL_QUADS);
        glVertex3f(-roadHalfWidth, 0, zn); glVertex3f( roadHalfWidth, 0, zn);
        glVertex3f( roadHalfWidth, 0, zf); glVertex3f(-roadHalfWidth, 0, zf);
        glEnd();

        // 2. Dashed Lines (Using DDA Algorithm)
        if (i % 2 == 0) {
            glDisable(GL_LIGHTING);
            glColor3f(1, 1, 0); // Yellow
            drawLineDDA(-1.0f, 0.02f, zn, -1.0f, 0.02f, zf);
            drawLineDDA(1.0f, 0.02f, zn, 1.0f, 0.02f, zf);
            glEnable(GL_LIGHTING);
        }

        // 3. Grass (Green)
        glColor3f(0.1f, 0.6f, 0.1f);
        glBegin(GL_QUADS);
        // Left grass
        glVertex3f(-50.0f, -0.1f, zn); glVertex3f(-roadHalfWidth, -0.1f, zn);
        glVertex3f(-roadHalfWidth, -0.1f, zf); glVertex3f(-50.0f, -0.1f, zf);
        // Right grass
        glVertex3f(roadHalfWidth, -0.1f, zn); glVertex3f(50.0f, -0.1f, zn);
        glVertex3f(50.0f, -0.1f, zf); glVertex3f(roadHalfWidth, -0.1f, zf);
        glEnd();

        // 4. Scenery Generation
        uint32_t h = hash32((uint32_t)seg ^ 0x9e3779b9U);
        
        // A. Trees
        if ((h % 10) > 6) drawTree( roadHalfWidth + 3.0f + (h % 5), zm);
        if (((h >> 4) % 10) > 7) drawTree(-(roadHalfWidth + 3.0f + ((h >> 8) % 5)), zm);
        
        // B. Windmill (occasional)
        if (seg % 25 == 0) drawWindmill(roadHalfWidth + 9.0f, zm);

        // C. Houses (in the grass)
        if (seg % 17 == 0) drawCartoonHouse(-(roadHalfWidth + 12.0f), zm);
        if (seg % 23 == 0) drawCartoonHouse(roadHalfWidth + 12.0f, zm);
        
        // D. Medium Cartoon Characters (in the grass, closer to road)
        if (seg % 11 == 0) drawCartoonCharacter(-(roadHalfWidth + 6.0f), zm);
        if (seg % 13 == 0) drawCartoonCharacter(roadHalfWidth + 6.0f, zm);
    }

    // Draw Dynamic Objects
    for (auto &c : cars) {
        float z = segmentZ(c.seg);
        if (z > -160 && z < 10) drawCar(laneX(c.lane), z);
    }
    for (auto &cn : coins) {
        if (cn.collected) continue;
        float z = segmentZ(cn.seg);
        if (z > -160 && z < 10) drawCoin(laneX(cn.lane), z);
    }

    glPopMatrix();
}

// Logic to check if player hit something
void checkCollisions() {
    for (auto &c : cars) {
        if (c.lane != currentLane) continue;
        float z = segmentZ(c.seg) + roadOffset;
        if (z > -0.8f && z < 0.8f && playerY <= 0.75f) { mode = GAMEOVER; return; }
    }
    for (auto &cn : coins) {
        if (cn.collected) continue;
        if (cn.lane != currentLane) continue;
        float z = segmentZ(cn.seg) + roadOffset;
        if (z > -0.8f && z < 0.8f) { cn.collected = true; coinScore++; }
    }
}

// Main Display Loop
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera
    gluLookAt(0.0, 4.0, 6.0, 0.0, 0.0, -8.0, 0.0, 1.0, 0.0);
    
    // Light
    GLfloat lightPos[] = { 30.0f, 60.0f, 30.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Draw Background First
    drawAttractiveBackground();

    // Game Mode UI
    if (mode == MENU) {
        drawText("STREET RUNNER -2D Game", 220, 420, 1, 1, 1);
        drawText("Press ENTER to Start Game", 260, 340, 1, 1, 1);
        drawText("Controls: A/D move, SPACE jump, P pause", 210, 270, 1, 1, 1);
        // drawText("Developer - Mahfujur Rhamna ", 210, 270, 1, 1, 1);
    } else {
        drawWorld();
        drawRobot();

        char s1[64], s2[64];
        std::snprintf(s1, sizeof(s1), "Distance: %ld", distanceScore);
        std::snprintf(s2, sizeof(s2), "Coins: %ld", coinScore);
        drawText(s1, 10, 570, 1, 1, 1);
        drawText(s2, 10, 540, 1, 1, 0);

        if (mode == PAUSED) {
            drawText("PAUSED", 360, 380, 1, 1, 1);
            drawText("Press P to Continue", 300, 330, 1, 1, 1);
            drawText("Press R to Restart",  300, 290, 1, 1, 1);
        }
        if (mode == GAMEOVER) {
            drawText("GAME OVER - Press R to Restart", 240, 320, 1, 0, 0);
        }
    }
    glutSwapBuffers();
}

// Game Loop / Timer
void update(int) {
    if (mode == PLAYING) {
        distanceScore++;
        windmillAngle += 2.0f;
        roadOffset += scrollSpeed;
        
        // Infinite scrolling logic
        if (roadOffset > segmentLength) {
            roadOffset -= segmentLength;
            currentSegment++;
            spawn(currentSegment + visibleSegments + 40);
            cleanupOld();
        }

        // Jump Physics
        if (isJumping) {
            playerY += velY;
            velY -= GRAVITY;
            if (playerY <= 0.5f) { playerY = 0.5f; isJumping = false; velY = 0.0f; }
        }

        // Lane Moving Physics (Smooth)
        targetX = currentLane * laneWidth;
        if (playerX < targetX) { 
            playerX += laneSpeed; 
            if (playerX > targetX) playerX = targetX; 
        } else if (playerX > targetX) { 
            playerX -= laneSpeed; 
            if (playerX < targetX) playerX = targetX; 
        }

        checkCollisions();
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Keyboard Input
void keys(unsigned char k, int, int) {
    if (mode == MENU) {
        if (k == 13) resetGame(); // Enter
        if (k == 27) std::exit(0); // Esc
        return;
    }
    if (mode == GAMEOVER) {
        if (k == 'r' || k == 'R') resetGame();
        return;
    }
    if (k == 'p' || k == 'P') { mode = (mode == PLAYING) ? PAUSED : PLAYING; return; }
    if (mode == PAUSED) {
        if (k == 'r' || k == 'R') resetGame();
        return;
    }
    if (mode == PLAYING) {
        if ((k == 'a' || k == 'A') && currentLane > -1) currentLane--;
        if ((k == 'd' || k == 'D') && currentLane < 1) currentLane++;
        if (k == ' ' && !isJumping) { isJumping = true; velY = JUMP_FORCE; }
    }
}

// Window Resize
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

// OpenGL Setup
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat amb[] = {0.4f,0.4f,0.4f,1};
    GLfloat dif[] = {0.8f,0.8f,0.8f,1};
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Street Runner - Cartoon Edition");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keys);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}