#include <windows.h>
#include <iostream>
#include <GL/glut.h>
#include <math.h>
using namespace std;

#define PI 3.14159265358979323846

GLfloat x, y, radius, twicePi; // for circle
int triangleAmount;

GLfloat position_c1 = 1.6f;
//bike 1

extern void displayMenu();
extern void mouseClick(int button, int state, int x, int y);
extern void keyboardMenu(unsigned char key, int x, int y);
float position_b1 = 0.0f;
float speed_b1 = 0.01f;
float fallOffset = 0.0f;
bool hasHelmet = true;
bool accidentTriggered = false;


float position_b2 = 2.4f;
float speed_b2 = 0.01f;


GLfloat position_b3 = -0.9f; // for bike 3
GLfloat speed_b3 = 0.01f;    // for bike 3

// Bike 4 (replacing car 4)
GLfloat position_b4 = 0.9f; // for bike 4
GLfloat speed_b4 = 0.01f;   // for bike 4
//int cnt = 0;                // Added to match update logic
//int flag = 0;               // Added to match light logic

bool policeActive = false;
bool lightBlink = false;
int blinkCounter = 0;
bool showOverspeeding = false;

int cnt = 0, flag = 0, r = 0;

char *c;
/* Handler for window-repaint event. Call back when the window first appears and
whenever the window needs to be re-painted. */



void trafficPolice_Prince() {
    glPushMatrix();
    glTranslatef(0.0f, -0.6f, 0.0f); // Position near road

    // --- Legs ---
    glColor3ub(0, 0, 128); // Navy blue pants
    glBegin(GL_POLYGON); // Left leg
        glVertex2f(-0.035f, 0.0f);
        glVertex2f(-0.015f, 0.0f);
        glVertex2f(-0.012f, 0.15f);
        glVertex2f(-0.038f, 0.15f);
    glEnd();

    glBegin(GL_POLYGON); // Right leg
        glVertex2f(0.015f, 0.0f);
        glVertex2f(0.035f, 0.0f);
        glVertex2f(0.038f, 0.15f);
        glVertex2f(0.012f, 0.15f);
    glEnd();

    // --- Shoes ---
    glColor3ub(0, 0, 0); // Black shoes
    glBegin(GL_POLYGON);
        glVertex2f(-0.035f, 0.0f); glVertex2f(-0.015f, 0.0f);
        glVertex2f(-0.015f, -0.02f); glVertex2f(-0.035f, -0.02f);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(0.015f, 0.0f); glVertex2f(0.035f, 0.0f);
        glVertex2f(0.035f, -0.02f); glVertex2f(0.015f, -0.02f);
    glEnd();

    // --- Body (Slimmer) ---
    glColor3ub(0, 100, 100); // Deep teal green uniform
    glBegin(GL_POLYGON);
        glVertex2f(-0.045f, 0.15f);
        glVertex2f(0.045f, 0.15f);
        glVertex2f(0.035f, 0.32f);
        glVertex2f(-0.035f, 0.32f);
    glEnd();

    // --- Belt ---
    glColor3ub(0, 0, 0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.045f, 0.15f);
        glVertex2f(0.045f, 0.15f);
        glVertex2f(0.045f, 0.155f);
        glVertex2f(-0.045f, 0.155f);
    glEnd();

    // --- Badge ---
    glColor3ub(255, 215, 0); // Gold
    glBegin(GL_POLYGON);
        glVertex2f(0.025f, 0.28f);
        glVertex2f(0.035f, 0.28f);
        glVertex2f(0.035f, 0.29f);
        glVertex2f(0.025f, 0.29f);
    glEnd();

    // --- Arms (Adjusted Width) ---
    glColor3ub(0, 100, 100); // Deep teal green
    glBegin(GL_POLYGON); // Left arm
        glVertex2f(-0.035f, 0.32f);
        glVertex2f(-0.06f, 0.32f);
        glVertex2f(-0.065f, 0.27f);
        glVertex2f(-0.06f, 0.22f);
        glVertex2f(-0.035f, 0.22f);
    glEnd();

    glBegin(GL_POLYGON); // Right arm
        glVertex2f(0.035f, 0.32f);
        glVertex2f(0.06f, 0.32f);
        glVertex2f(0.065f, 0.37f);
        glVertex2f(0.06f, 0.42f);
        glVertex2f(0.035f, 0.42f);
    glEnd();

    // --- Gloved Hands ---
    glColor3ub(50, 50, 50); // Dark gray gloves

    // Left glove palm
    glBegin(GL_POLYGON);
        glVertex2f(-0.06f, 0.22f);
        glVertex2f(-0.035f, 0.22f);
        glVertex2f(-0.035f, 0.24f);
        glVertex2f(-0.06f, 0.24f);
    glEnd();

    // Left glove fingers
    for (float i = -0.058f; i <= -0.037f; i += 0.004f) {
        glBegin(GL_POLYGON);
        glVertex2f(i - 0.002f, 0.24f);
        glVertex2f(i + 0.002f, 0.24f);
        glVertex2f(i + 0.002f, 0.26f);
        glVertex2f(i - 0.002f, 0.26f);
        glEnd();
    }

    // Right glove palm
    glBegin(GL_POLYGON);
        glVertex2f(0.035f, 0.42f);
        glVertex2f(0.06f, 0.42f);
        glVertex2f(0.06f, 0.44f);
        glVertex2f(0.035f, 0.44f);
    glEnd();

    // Right glove fingers
    for (float i = 0.037f; i <= 0.058f; i += 0.004f) {
        glBegin(GL_POLYGON);
        glVertex2f(i - 0.002f, 0.44f);
        glVertex2f(i + 0.002f, 0.44f);
        glVertex2f(i + 0.002f, 0.46f);
        glVertex2f(i - 0.002f, 0.46f);
        glEnd();
    }

    // --- Glove Stitching ---
    glColor3ub(80, 80, 80);
    glBegin(GL_LINES);
        glVertex2f(-0.06f, 0.23f); glVertex2f(-0.035f, 0.23f);
        glVertex2f(0.035f, 0.43f); glVertex2f(0.06f, 0.43f);
    glEnd();

    // --- Sticklight ---
    if (blinkCounter % 20 < 10)
        glColor3ub(255, 0, 0);
    else
        glColor3ub(255, 255, 0);

    glBegin(GL_POLYGON);
        glVertex2f(0.06f, 0.46f);
        glVertex2f(0.07f, 0.46f);
        glVertex2f(0.07f, 0.58f);
        glVertex2f(0.06f, 0.58f);
    glEnd();

    // --- Whistle ---
    glColor3ub(255, 0, 0);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.34f);
        glVertex2f(0.01f, 0.34f);
        glVertex2f(0.01f, 0.345f);
        glVertex2f(0.0f, 0.345f);
    glEnd();

    // --- Mouth ---
    glColor3ub(150, 75, 0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.01f, 0.365f);
        glVertex2f(0.01f, 0.365f);
        glVertex2f(0.01f, 0.362f);
        glVertex2f(-0.01f, 0.362f);
    glEnd();

    // --- Neck ---
    glColor3ub(255, 224, 189);
    glBegin(GL_POLYGON);
        glVertex2f(-0.015f, 0.32f);
        glVertex2f(0.015f, 0.32f);
        glVertex2f(0.015f, 0.34f);
        glVertex2f(-0.015f, 0.34f);
    glEnd();

    // --- Head ---
    glColor3ub(255, 224, 189);
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(0.0f + 0.03f * cos(rad), 0.37f + 0.03f * sin(rad));
    }
    glEnd();

    // --- Eyes ---
    glColor3ub(0, 0, 0);
    glBegin(GL_POINTS);
        glVertex2f(-0.01f, 0.375f);
        glVertex2f(0.01f, 0.375f);
    glEnd();

    // --- Nose ---
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.375f);
        glVertex2f(0.0f, 0.37f);
    glEnd();

    // --- Ears ---
    glBegin(GL_POLYGON);
        glVertex2f(-0.03f, 0.37f);
        glVertex2f(-0.035f, 0.37f);
        glVertex2f(-0.035f, 0.375f);
        glVertex2f(-0.03f, 0.375f);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(0.03f, 0.37f);
        glVertex2f(0.035f, 0.37f);
        glVertex2f(0.035f, 0.375f);
        glVertex2f(0.03f, 0.375f);
    glEnd();

    // --- Sunglasses ---
    glColor3ub(0, 0, 0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.015f, 0.38f);
        glVertex2f(0.015f, 0.38f);
        glVertex2f(0.015f, 0.385f);
        glVertex2f(-0.015f, 0.385f);
    glEnd();

    // --- Hat (Adjusted to Fit Head) ---
    glColor3ub(0, 0, 128); // Navy blue
    glBegin(GL_POLYGON); // Brim
        glVertex2f(-0.04f, 0.395f);
        glVertex2f(0.04f, 0.395f);
        glVertex2f(0.035f, 0.405f);
        glVertex2f(-0.035f, 0.405f);
    glEnd();

    glBegin(GL_POLYGON); // Dome
        glVertex2f(-0.03f, 0.405f);
        glVertex2f(0.03f, 0.405f);
        glVertex2f(0.025f, 0.42f);
        glVertex2f(-0.025f, 0.42f);
    glEnd();

    // --- Hat Logo ---
    glColor3ub(255, 215, 0); // Gold
    glBegin(GL_POLYGON);
        glVertex2f(-0.007f, 0.405f);
        glVertex2f(0.007f, 0.405f);
        glVertex2f(0.007f, 0.41f);
        glVertex2f(-0.007f, 0.41f);
    glEnd();

    glPopMatrix();
}



  void shop_Prince() {
  // Chelox shop - modern curved roof & arched shelter
// body
glBegin(GL_POLYGON);
    glColor3ub(255, 182, 193); // vibrant pink
    glVertex2f(-1.95f, 0.5f);
    glVertex2f(-1.95f, 0.75f);
    glVertex2f(-1.55f, 0.75f);
    glVertex2f(-1.55f, 0.5f);
glEnd();

// curved roof - split into 3 polygons for subtle wave
// left part of roof
glBegin(GL_POLYGON);
    glColor3ub(255, 105, 180); // hot pink
    glVertex2f(-1.97f, 0.75f); // bottom left
    glVertex2f(-1.95f, 0.85f); // mid-left top
    glVertex2f(-1.87f, 0.87f); // slightly higher left-mid
    glVertex2f(-1.85f, 0.75f); // bottom mid-left
glEnd();

// center part of roof
glBegin(GL_POLYGON);
    glColor3ub(255, 105, 180); // hot pink
    glVertex2f(-1.85f, 0.75f); // bottom left
    glVertex2f(-1.87f, 0.87f); // top left
    glVertex2f(-1.63f, 0.87f); // top right
    glVertex2f(-1.65f, 0.75f); // bottom right
glEnd();

// right part of roof
glBegin(GL_POLYGON);
    glColor3ub(255, 105, 180); // hot pink
    glVertex2f(-1.65f, 0.75f); // bottom left
    glVertex2f(-1.63f, 0.87f); // top left
    glVertex2f(-1.55f, 0.85f); // top right
    glVertex2f(-1.53f, 0.75f); // bottom right
glEnd();

// curved shelter - split into two triangles for arch effect
// left triangle
glBegin(GL_TRIANGLES);
    glColor3ub(255, 165, 0); // vibrant orange
    glVertex2f(-1.96f, 0.75f); // left bottom
    glVertex2f(-1.75f, 0.92f); // top peak slightly higher
    glVertex2f(-1.75f, 0.75f); // center bottom
glEnd();

// right triangle
glBegin(GL_TRIANGLES);
    glColor3ub(255, 165, 0); // vibrant orange
    glVertex2f(-1.75f, 0.75f); // center bottom
    glVertex2f(-1.75f, 0.92f); // top peak
    glVertex2f(-1.54f, 0.75f); // right bottom
glEnd();

// door
glBegin(GL_POLYGON);
    glColor3ub(0, 255, 255); // cyan
    glVertex2f(-1.9f, 0.5f);
    glVertex2f(-1.9f, 0.62f);
    glColor3ub(0, 128, 128); // teal gradient
    glVertex2f(-1.82f, 0.62f);
    glVertex2f(-1.82f, 0.5f);
glEnd();

// window
glBegin(GL_POLYGON);
    glColor3ub(173, 216, 230); // light blue
    glVertex2f(-1.77f, 0.55f);
    glVertex2f(-1.77f, 0.62f);
    glColor3ub(0, 191, 255); // deep sky blue gradient
    glVertex2f(-1.6f, 0.62f);
    glVertex2f(-1.6f, 0.55f);
glEnd();

// outline lines for Chelox shop
glBegin(GL_LINES);
    glColor3ub(139, 0, 0); // dark red
    glVertex2f(-1.52f, 0.5f); glVertex2f(-1.98f, 0.5f);
    glVertex2f(-1.52f, 0.75f); glVertex2f(-1.98f, 0.75f);

    // door outline
    glVertex2f(-1.9f, 0.5f); glVertex2f(-1.9f, 0.62f);
    glVertex2f(-1.9f, 0.62f); glVertex2f(-1.82f, 0.62f);
    glVertex2f(-1.82f, 0.62f); glVertex2f(-1.82f, 0.5f);
    glVertex2f(-1.82f, 0.5f); glVertex2f(-1.9f, 0.5f);

    // window outline
    glVertex2f(-1.77f, 0.55f); glVertex2f(-1.77f, 0.62f);
    glVertex2f(-1.77f, 0.62f); glVertex2f(-1.6f, 0.62f);
    glVertex2f(-1.6f, 0.62f); glVertex2f(-1.6f, 0.55f);
    glVertex2f(-1.6f, 0.55f); glVertex2f(-1.77f, 0.55f);
glEnd();

// shop name Chillox
glColor3f(1.0, 1.0, 0.0); // yellow
const char* c = "Chillox ";
glRasterPos2f(-1.85 , 0.78);
for(int i = 0; c[i] !='\0'; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c[i]);
// ------------------------
// Right shop start - Waffle House (Pepsi version)
// ------------------------

// Shop body
glBegin(GL_POLYGON); // body
    glColor3ub(255, 215, 0); // gold yellow (unchanged)
    glVertex2f(-0.75f, 0.5f);
    glVertex2f(-0.75f, 0.8f);
    glVertex2f(-0.2f, 0.8f);
    glVertex2f(-0.2f, 0.5f);
glEnd();

// Roof
glBegin(GL_POLYGON); // roof
    glColor3ub(255, 69, 0); // red-orange
    glVertex2f(-0.75f, 0.85f);
    glVertex2f(-0.75f, 0.8f);
    glVertex2f(-0.2f, 0.8f);
    glVertex2f(-0.2f, 0.85f);
glEnd();

// Roof accent
glBegin(GL_POLYGON); // accent
    glColor3ub(255, 140, 0); // dark orange
    glVertex2f(-0.3f, 0.85f);
    glVertex2f(-0.35f, 0.9f);
    glVertex2f(-0.6f, 0.9f);
    glVertex2f(-0.66f, 0.85f);
glEnd();

// Inside area
glBegin(GL_POLYGON);
    glColor3ub(205, 133, 63); // peru brown
    glVertex2f(-0.73f, 0.6f);
    glVertex2f(-0.73f, 0.75f);
    glVertex2f(-0.22f, 0.75f);
    glVertex2f(-0.22f, 0.6f);
glEnd();

// ------------------------
// Products Section - Pepsi Cold Drinks
// ------------------------

// French fries box 1 (unchanged)
glBegin(GL_POLYGON);
    glColor3ub(255, 0, 0); // red box
    glVertex2f(-0.7f, 0.6f);
    glVertex2f(-0.7f, 0.65f);
    glVertex2f(-0.64f, 0.65f);
    glVertex2f(-0.64f, 0.6f);
glEnd();
glBegin(GL_LINES); // fries sticks
    glColor3ub(255, 215, 0); // yellow fries
    glVertex2f(-0.695f, 0.65f); glVertex2f(-0.695f, 0.675f);
    glVertex2f(-0.685f, 0.65f); glVertex2f(-0.685f, 0.675f);
    glVertex2f(-0.675f, 0.65f); glVertex2f(-0.675f, 0.675f);
glEnd();

// Cold drink box 1 - Pepsi
glBegin(GL_POLYGON);
    glColor3ub(0, 75, 147); // Pepsi blue
    glVertex2f(-0.62f, 0.6f);
    glVertex2f(-0.62f, 0.66f);
    glVertex2f(-0.57f, 0.66f);
    glVertex2f(-0.57f, 0.6f);
glEnd();

// Add Pepsi logo circle (simple approximation)
int num_segments = 50;
float cx = -0.595f, cy = 0.63f, r = 0.015f;
glBegin(GL_TRIANGLE_FAN);
    glColor3ub(255, 255, 255); // white circle
    glVertex2f(cx, cy);
    for(int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
glEnd();

// Straw
glBegin(GL_LINES);
    glColor3ub(255, 0, 0); // red straw (Pepsi theme)
    glVertex2f(-0.595f, 0.66f); glVertex2f(-0.595f, 0.68f);
glEnd();

// Cold drink box 2 - Pepsi
glBegin(GL_POLYGON);
    glColor3ub(0, 75, 147); // Pepsi blue
    glVertex2f(-0.55f, 0.6f);
    glVertex2f(-0.55f, 0.66f);
    glVertex2f(-0.5f, 0.66f);
    glVertex2f(-0.5f, 0.6f);
glEnd();

// Pepsi logo
cx = -0.525f; cy = 0.63f; r = 0.015f;
glBegin(GL_TRIANGLE_FAN);
    glColor3ub(255, 255, 255); // white circle
    glVertex2f(cx, cy);
    for(int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
glEnd();

// Straw
glBegin(GL_LINES);
    glColor3ub(255, 0, 0); // red straw
    glVertex2f(-0.525f, 0.66f); glVertex2f(-0.525f, 0.68f);
glEnd();

// Card payment machine (unchanged)
glBegin(GL_POLYGON);
    glColor3ub(105, 105, 105); // grey body
    glVertex2f(-0.48f, 0.57f);
    glVertex2f(-0.48f, 0.62f);
    glVertex2f(-0.44f, 0.62f);
    glVertex2f(-0.44f, 0.57f);
glEnd();
glBegin(GL_POLYGON); // screen
    glColor3ub(0, 255, 255);
    glVertex2f(-0.475f, 0.595f);
    glVertex2f(-0.475f, 0.62f);
    glVertex2f(-0.445f, 0.62f);
    glVertex2f(-0.445f, 0.595f);
glEnd();
glBegin(GL_POLYGON); // swiper slot
    glColor3ub(0, 0, 0);
    glVertex2f(-0.475f, 0.57f);
    glVertex2f(-0.475f, 0.575f);
    glVertex2f(-0.445f, 0.575f);
    glVertex2f(-0.445f, 0.57f);
glEnd();

// Table (unchanged)
glBegin(GL_POLYGON);
    glColor3ub(160, 82, 45);
    glVertex2f(-0.73f, 0.57f);
    glVertex2f(-0.73f, 0.6f);
    glVertex2f(-0.22f, 0.6f);
    glVertex2f(-0.22f, 0.57f);
glEnd();

// Outline lines (unchanged)
glBegin(GL_LINES);
    glColor3ub(128, 0, 0);
    glVertex2f(-0.2f, 0.5f); glVertex2f(-0.75f, 0.5f);
    glVertex2f(-0.2f, 0.8f); glVertex2f(-0.75f, 0.8f);
    glVertex2f(-0.75f, 0.5f); glVertex2f(-0.75f, 0.8f);
    glVertex2f(-0.2f, 0.5f); glVertex2f(-0.2f, 0.8f);
glEnd();

// Shop name
glColor3f(0.0, 0.0, 1.0); // blue text for Pepsi theme
const char* w = "Waffle House";
glRasterPos2f(-0.63 , 0.82);
for(int i = 0; w[i] !='\0'; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, w[i]);



 }



void house_Prince() {
   // left house start
glBegin(GL_POLYGON); // body
    glColor3ub(210, 180, 140); // light brown
    glVertex2f(-1.4f, 0.5f);
    glVertex2f(-1.4f, 1.0f);
    glVertex2f(-0.9f, 1.0f);
    glVertex2f(-0.9f, 0.5f);
glEnd();

glBegin(GL_POLYGON); // bottom-window 1
    glColor3ub(173, 216, 230); // light blue
    glVertex2f(-1.35f, 0.55f);
    glVertex2f(-1.35f, 0.65f);
    glColor3ub(0, 191, 255); // deep sky blue
    glVertex2f(-1.25f, 0.65f);
    glVertex2f(-1.25f, 0.55f);
glEnd();

// bottom-window 2 removed

glBegin(GL_POLYGON); // top-window 1
    glColor3ub(173, 216, 230);
    glVertex2f(-1.35f, 0.85f);
    glVertex2f(-1.35f, 0.95f);
    glColor3ub(0, 191, 255);
    glVertex2f(-1.25f, 0.95f);
    glVertex2f(-1.25f, 0.85f);
glEnd();

// top-window 2
glBegin(GL_POLYGON);
    glColor3ub(173, 216, 230);
    glVertex2f(-1.15f, 0.85f);
    glVertex2f(-1.15f, 0.95f);
    glColor3ub(0, 191, 255);
    glVertex2f(-1.05f, 0.95f);
    glVertex2f(-1.05f, 0.85f);
glEnd();

// door
// modern door
glBegin(GL_POLYGON);
    glColor3ub(60, 179, 113); // medium sea green - modern & elegant
    glVertex2f(-1.2f, 0.5f);
    glVertex2f(-1.2f, 0.65f);
    glVertex2f(-1.1f, 0.65f);
    glVertex2f(-1.1f, 0.5f);
glEnd();

// door panels (vertical lines for modern look)
glBegin(GL_LINES);
    glColor3ub(34, 139, 34); // darker green for panels
    glVertex2f(-1.17f, 0.5f); glVertex2f(-1.17f, 0.65f);
    glVertex2f(-1.14f, 0.5f); glVertex2f(-1.14f, 0.65f);
glEnd();

// sleek handle
glLineWidth(3.0);
glBegin(GL_LINES);
    glColor3ub(255, 215, 0); // golden handle
    glVertex2f(-1.12f, 0.575f); glVertex2f(-1.13f, 0.575f);
glEnd();

glBegin(GL_LINES); // outline all windows, door, body
    glColor3ub(105, 105, 105); // dim gray lines

    // bottom-window 1
    glVertex2f(-1.35f, 0.55f); glVertex2f(-1.35f, 0.65f);
    glVertex2f(-1.35f, 0.65f); glVertex2f(-1.25f, 0.65f);
    glVertex2f(-1.25f, 0.65f); glVertex2f(-1.25f, 0.55f);
    glVertex2f(-1.25f, 0.55f); glVertex2f(-1.35f, 0.55f);
    glVertex2f(-1.25f, 0.6f); glVertex2f(-1.35f, 0.6f);

    // top-window 1
    glVertex2f(-1.35f, 0.85f); glVertex2f(-1.35f, 0.95f);
    glVertex2f(-1.35f, 0.95f); glVertex2f(-1.25f, 0.95f);
    glVertex2f(-1.25f, 0.95f); glVertex2f(-1.25f, 0.85f);
    glVertex2f(-1.25f, 0.85f); glVertex2f(-1.35f, 0.85f);
    glVertex2f(-1.25f, 0.9f); glVertex2f(-1.35f, 0.9f);

    // top-window 2
    glVertex2f(-1.15f, 0.85f); glVertex2f(-1.15f, 0.95f);
    glVertex2f(-1.15f, 0.95f); glVertex2f(-1.05f, 0.95f);
    glVertex2f(-1.05f, 0.95f); glVertex2f(-1.05f, 0.85f);
    glVertex2f(-1.05f, 0.85f); glVertex2f(-1.15f, 0.85f);
    glVertex2f(-1.15f, 0.9f); glVertex2f(-1.05f, 0.9f);

    // door
    glVertex2f(-1.2f, 0.5f); glVertex2f(-1.2f, 0.65f);
    glVertex2f(-1.2f, 0.65f); glVertex2f(-1.1f, 0.65f);
    glVertex2f(-1.1f, 0.65f); glVertex2f(-1.1f, 0.5f);
    glVertex2f(-1.1f, 0.5f); glVertex2f(-1.2f, 0.5f);

    // body
    glVertex2f(-1.4f, 0.5f); glVertex2f(-1.4f, 1.0f);
    glVertex2f(-1.4f, 1.0f); glVertex2f(-0.9f, 1.0f);
    glVertex2f(-0.9f, 1.0f); glVertex2f(-0.9f, 0.5f);
glEnd();

glBegin(GL_POLYGON); // shelter
    glColor3ub(176, 196, 222); // light steel blue
    glVertex2f(-1.4f, 0.8f);
    glVertex2f(-1.45f, 0.75f);
    glVertex2f(-1.45f, 0.7f);
    glVertex2f(-0.85f, 0.7f);
    glVertex2f(-0.85f, 0.75f);
    glVertex2f(-0.9f, 0.8f);
glEnd();

glBegin(GL_LINES); // shelter outline improved
    glColor3ub(25, 25, 112); // dark slate blue, more realistic for roof

    // left side slope
    glVertex2f(-1.4f, 0.8f); glVertex2f(-1.45f, 0.75f);

    // left vertical
    glVertex2f(-1.45f, 0.75f); glVertex2f(-1.45f, 0.7f);

    // bottom horizontal
    glVertex2f(-1.45f, 0.7f); glVertex2f(-0.85f, 0.7f);

    // right vertical
    glVertex2f(-0.85f, 0.7f); glVertex2f(-0.85f, 0.75f);

    // right slope
    glVertex2f(-0.85f, 0.75f); glVertex2f(-0.9f, 0.8f);

    // top horizontal
    glVertex2f(-0.9f, 0.8f); glVertex2f(-1.4f, 0.8f);

    // add diagonal shadow line for 3D effect
    glVertex2f(-1.45f, 0.75f); glVertex2f(-0.9f, 0.8f);

    glVertex2f(-1.4f, 0.8f); glVertex2f(-0.85f, 0.75f);
glEnd();


// Right house design with slightly modified door

// House Body
glBegin(GL_POLYGON);
    glColor3ub(255, 153, 51); // vibrant orange
    glVertex2f(-0.15f, 0.5f);
    glVertex2f(-0.15f, 0.8f);
    glVertex2f(0.35f, 0.8f);
    glVertex2f(0.35f, 0.5f);
glEnd();

// Store Room
glBegin(GL_POLYGON);
    glColor3ub(255, 204, 102); // lighter orange
    glVertex2f(0.05f, 0.8f);
    glVertex2f(0.05f, 0.99f);
    glVertex2f(0.35f, 0.99f);
    glVertex2f(0.35f, 0.8f);
glEnd();

// Window 1
glBegin(GL_POLYGON);
    glColor3ub(102, 255, 255); // cyan
    glVertex2f(0.05f, 0.6f);
    glVertex2f(0.05f, 0.7f);
    glColor3ub(51, 204, 255);
    glVertex2f(0.15f, 0.7f);
    glVertex2f(0.15f, 0.6f);
glEnd();

// Window 2
glBegin(GL_POLYGON);
    glColor3ub(102, 255, 255);
    glVertex2f(0.2f, 0.6f);
    glVertex2f(0.2f, 0.7f);
    glColor3ub(51, 204, 255);
    glVertex2f(0.3f, 0.7f);
    glVertex2f(0.3f, 0.6f);
glEnd();

// Store Room Window
glBegin(GL_POLYGON);
    glColor3ub(102, 255, 255);
    glVertex2f(0.15f, 0.85f);
    glVertex2f(0.15f, 0.95f);
    glColor3ub(51, 204, 255);
    glVertex2f(0.25f, 0.95f);
    glVertex2f(0.25f, 0.85f);
glEnd();

// Door (slightly redesigned double-panel door)
glBegin(GL_POLYGON); // left panel
    glColor3ub(102, 51, 0); // brown
    glVertex2f(-0.1f, 0.5f);
    glVertex2f(-0.1f, 0.65f);
    glVertex2f(-0.03f, 0.65f);
    glVertex2f(-0.03f, 0.5f);
glEnd();

glBegin(GL_POLYGON); // right panel
    glColor3ub(102, 51, 0);
    glVertex2f(-0.03f, 0.5f);
    glVertex2f(-0.03f, 0.65f);
    glVertex2f(0.0f, 0.65f);
    glVertex2f(0.0f, 0.5f);
glEnd();

// Door handles
glPointSize(5.0);
glBegin(GL_POINTS);
    glColor3ub(255, 215, 0); // golden
    glVertex2f(-0.08f, 0.575f); // left handle
    glVertex2f(-0.015f, 0.575f); // right handle
glEnd();

// Door Shelter (triangular)
glBegin(GL_POLYGON);
    glColor3ub(255, 102, 0); // vibrant orange
    glVertex2f(0.02f, 0.65f);
    glVertex2f(-0.05f, 0.7f);
    glVertex2f(-0.12f, 0.65f);
glEnd();

// Borders using lines
glBegin(GL_LINES);
    glColor3ub(255, 255, 255);

    // Store Room Window Border
    glVertex2f(0.15f, 0.85f); glVertex2f(0.15f, 0.95f);
    glVertex2f(0.15f, 0.95f); glVertex2f(0.25f, 0.95f);
    glVertex2f(0.25f, 0.95f); glVertex2f(0.25f, 0.85f);
    glVertex2f(0.25f, 0.85f); glVertex2f(0.15f, 0.85f);

    // Window 1 Border
    glVertex2f(0.05f, 0.6f); glVertex2f(0.05f, 0.7f);
    glVertex2f(0.05f, 0.7f); glVertex2f(0.15f, 0.7f);
    glVertex2f(0.15f, 0.7f); glVertex2f(0.15f, 0.6f);
    glVertex2f(0.15f, 0.6f); glVertex2f(0.05f, 0.6f);

    // Window 2 Border
    glVertex2f(0.2f, 0.6f); glVertex2f(0.2f, 0.7f);
    glVertex2f(0.2f, 0.7f); glVertex2f(0.3f, 0.7f);
    glVertex2f(0.3f, 0.7f); glVertex2f(0.3f, 0.6f);
    glVertex2f(0.3f, 0.6f); glVertex2f(0.2f, 0.6f);

    // Door Border
    glColor3ub(153, 102, 51);
    glVertex2f(-0.1f, 0.5f); glVertex2f(-0.1f, 0.65f);
    glVertex2f(-0.1f, 0.65f); glVertex2f(0.0f, 0.65f);
    glVertex2f(0.0f, 0.65f); glVertex2f(0.0f, 0.5f);
    glVertex2f(0.0f, 0.5f); glVertex2f(-0.1f, 0.5f);

    // Door Shelter Border
    glVertex2f(0.02f, 0.65f); glVertex2f(-0.05f, 0.7f);
    glVertex2f(-0.05f, 0.7f); glVertex2f(-0.12f, 0.65f);
    glVertex2f(-0.12f, 0.65f); glVertex2f(0.02f, 0.65f);

glEnd();


}



void fence_Prince() {
    glBegin(GL_LINES);
        // Horizontal rails
        glColor3ub(160, 82, 45); // brown color instead of red
        glVertex2f(-2.0f, 0.6f); // left
        glVertex2f(0.39f, 0.6f);

        glVertex2f(-2.0f, 0.53f);
        glVertex2f(0.39f, 0.53f);

        glVertex2f(2.0f, 0.6f); // right
        glVertex2f(1.42f, 0.6f);

        glVertex2f(2.0f, 0.53f);
        glVertex2f(1.42f, 0.53f);

        // Vertical posts
        glColor3ub(139, 69, 19); // darker brown for posts
        glVertex2f(-1.98f, 0.5f);
        glVertex2f(-1.98f, 0.63f);

        glVertex2f(-1.5f, 0.5f);
        glVertex2f(-1.5f, 0.63f);

        glVertex2f(-1.45f, 0.5f);
        glVertex2f(-1.45f, 0.63f);

        glVertex2f(-0.4f, 0.5f);
        glVertex2f(-0.4f, 0.63f);

        glVertex2f(-0.85f, 0.5f);
        glVertex2f(-0.85f, 0.63f);

        glVertex2f(-0.8f, 0.5f);
        glVertex2f(-0.8f, 0.63f);

        glVertex2f(-0.17f, 0.5f);
        glVertex2f(-0.17f, 0.63f);

        glVertex2f(0.37f, 0.5f);
        glVertex2f(0.37f, 0.63f);

        glVertex2f(1.95f, 0.5f);
        glVertex2f(1.95f, 0.63f);

        glVertex2f(1.9f, 0.5f);
        glVertex2f(1.9f, 0.63f);

        glVertex2f(1.85f, 0.5f);
        glVertex2f(1.85f, 0.63f);

        glVertex2f(1.8f, 0.5f);
        glVertex2f(1.8f, 0.63f);

        glVertex2f(1.75f, 0.5f);
        glVertex2f(1.75f, 0.63f);

        glVertex2f(1.7f, 0.5f);
        glVertex2f(1.7f, 0.63f);

        glVertex2f(1.65f, 0.5f);
        glVertex2f(1.65f, 0.63f);

        glVertex2f(1.6f, 0.5f);
        glVertex2f(1.6f, 0.63f);

        glVertex2f(1.55f, 0.5f);
        glVertex2f(1.55f, 0.63f);

        glVertex2f(1.5f, 0.5f);
        glVertex2f(1.5f, 0.63f);

        glVertex2f(1.45f, 0.5f);
        glVertex2f(1.45f, 0.63f);
    glEnd();
}


void road_footpath_Prince() {
    // road
    glBegin(GL_POLYGON);
        glColor3ub(95, 96, 91);
        glVertex2f(-2.0f, 0.5f);
        glVertex2f(2.0f, 0.5f);
        glVertex2f(2.0f, -0.5f);
        glVertex2f(-2.0f, -0.5f);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(95,96,91);
        glVertex2f(0.4f, 1.0f);
        glVertex2f(1.4f, 1.0f);
        glVertex2f(1.4f, -1.0f);
        glVertex2f(0.4f, -1.0f);
    glEnd();

    // footpath
    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(-2.0f, 0.5f);
        glVertex2f(-2.0f, 0.3f);
        glVertex2f(0.6f, 0.3f);
        glVertex2f(0.6f, 0.5f);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(-2.0f, -0.5f);
        glVertex2f(-2.0f, -0.3f);
        glVertex2f(0.6f, -0.3f);
        glVertex2f(0.6f, -0.5f);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(2.0f, -0.5f);
        glVertex2f(2.0f, -0.3f);
        glVertex2f(1.2f, -0.3f);
        glVertex2f(1.2f, -0.5f);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(2.0f, 0.5f);
        glVertex2f(2.0f, 0.3f);
        glVertex2f(1.2f, 0.3f);
        glVertex2f(1.2f, 0.5f);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(0.4f, -1.0f);
        glVertex2f(0.6f, -1.0f);
        glVertex2f(0.6f, -0.3f);
        glVertex2f(0.4f, -0.3f);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(1.2f, -1.0f);
        glVertex2f(1.4f, -1.0f);
        glVertex2f(1.4f, -0.3f);
        glVertex2f(1.2f, -0.3f);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(1.2f, 1.0f);
        glVertex2f(1.4f, 1.0f);
        glVertex2f(1.4f, 0.3f);
        glVertex2f(1.2f, 0.3f);
    glEnd();
    glBegin(GL_POLYGON);
        glColor3ub(176, 191, 189);
        glVertex2f(0.4f, 1.0f);
        glVertex2f(0.6f, 1.0f);
        glVertex2f(0.6f, 0.3f);
        glVertex2f(0.4f, 0.3f);
    glEnd();

    // divider
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2f(-2.0f, 0.0f);
        glVertex2f(-1.8f, 0.0f);

        glVertex2f(-1.7f, 0.0f);
        glVertex2f(-1.5f, 0.0f);

        glVertex2f(-1.4f, 0.0f);
        glVertex2f(-1.2f, 0.0f);

        glVertex2f(-1.1f, 0.0f);
        glVertex2f(-0.9f, 0.0f);

        glVertex2f(-0.8f, 0.0f);
        glVertex2f(-0.6f, 0.0f);

        glVertex2f(-0.5f, 0.0f);
        glVertex2f(-0.3f, 0.0f);

        glVertex2f(-0.2f, 0.0f);
        glVertex2f(0.0f, 0.0f);

        glVertex2f(1.7f, 0.0f);
        glVertex2f(1.9f, 0.0f);

        glVertex2f(0.9f, 0.8f);
        glVertex2f(0.9f, 0.97f);

        glVertex2f(0.9f, -0.97f);
        glVertex2f(0.9f, -0.8f);
    glEnd();

    // zebra-crossing start
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2f(0.2f, 0.3f);
        glVertex2f(0.2f, -0.3f);

        glVertex2f(0.4f, -0.3f);
        glVertex2f(0.4f, 0.3f);

        glVertex2f(0.6f, 0.5f);
        glVertex2f(1.2f, 0.5f);

        glVertex2f(0.6f, 0.7f);
        glVertex2f(1.2f, 0.7f);

        glVertex2f(1.4f, 0.3f);
        glVertex2f(1.4f, -0.3f);

        glVertex2f(1.6f, 0.3f);
        glVertex2f(1.6f, -0.3f);

        glVertex2f(0.6f, -0.5f);
        glVertex2f(1.2f, -0.5f);

        glVertex2f(0.6f, -0.7f);
        glVertex2f(1.2f, -0.7f);
    glEnd();

    glLineWidth(10.0f);
    glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        // left cross
        glVertex2f(0.2f, 0.2f);
        glVertex2f(0.4f, 0.2f);

        glVertex2f(0.2f, 0.1f);
        glVertex2f(0.4f, 0.1f);

        glVertex2f(0.2f, 0.0f);
        glVertex2f(0.4f, 0.0f);

        glVertex2f(0.2f, -0.1f);
        glVertex2f(0.4f, -0.1f);

        glVertex2f(0.2f, -0.2f);
        glVertex2f(0.4f, -0.2f);

        // right cross
        glVertex2f(1.4f, 0.2f);
        glVertex2f(1.6f, 0.2f);

        glVertex2f(1.4f, 0.1f);
        glVertex2f(1.6f, 0.1f);

        glVertex2f(1.4f, 0.0f);
        glVertex2f(1.6f, 0.0f);

        glVertex2f(1.4f, -0.1f);
        glVertex2f(1.6f, -0.1f);

        glVertex2f(1.4f, -0.2f);
        glVertex2f(1.6f, -0.2f);

        // bottom cross
        glVertex2f(0.7f, -0.5f);
        glVertex2f(0.7f, -0.7f);

        glVertex2f(0.8f, -0.5f);
        glVertex2f(0.8f, -0.7f);

        glVertex2f(0.9f, -0.5f);
        glVertex2f(0.9f, -0.7f);

        glVertex2f(1.0f, -0.5f);
        glVertex2f(1.0f, -0.7f);

        glVertex2f(1.1f, -0.5f);
        glVertex2f(1.1f, -0.7f);

        // top cross
        glVertex2f(0.7f, 0.5f);
        glVertex2f(0.7f, 0.7f);

        glVertex2f(0.8f, 0.5f);
        glVertex2f(0.8f, 0.7f);

        glVertex2f(0.9f, 0.5f);
        glVertex2f(0.9f, 0.7f);

        glVertex2f(1.0f, 0.5f);
        glVertex2f(1.0f, 0.7f);

        glVertex2f(1.1f, 0.5f);
        glVertex2f(1.1f, 0.7f);
    glEnd();
    // zebra-crossing end

    // road border
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glColor3ub(102, 102, 102);
        glVertex2f(-2.0f, 0.5f);
        glVertex2f(0.4f, 0.5f);

        glVertex2f(0.4f, 0.5f);
        glVertex2f(0.4f, 1.0f);

        glVertex2f(1.4f, 1.0f);
        glVertex2f(1.4f, 0.5f);

        glVertex2f(1.4f, 0.5f);
        glVertex2f(2.0f, 0.5f);

        glVertex2f(2.0f, -0.5f);
        glVertex2f(1.4f, -0.5f);

        glVertex2f(1.4f, -0.5f);
        glVertex2f(1.4f, -1.0f);

        glVertex2f(0.4f, -1.0f);
        glVertex2f(0.4f, -0.5f);

        glVertex2f(0.4f, -0.5f);
        glVertex2f(-2.0f, -0.5f);

        glColor3ub(230, 230, 230);
        glVertex2f(-2.0f, 0.3f);
        glVertex2f(0.6f, 0.3f);

        glVertex2f(0.6f, 0.3f);
        glVertex2f(0.6f, 1.0f);

        glVertex2f(1.2f, 1.0f);
        glVertex2f(1.2f, 0.3f);

        glVertex2f(1.2f, 0.3f);
        glVertex2f(2.0f, 0.3f);

        glVertex2f(2.0f, -0.3f);
        glVertex2f(1.2f, -0.3f);

        glVertex2f(1.2f, -0.3f);
        glVertex2f(1.2f, -1.0f);

        glVertex2f(0.6f, -1.0f);
        glVertex2f(0.6f, -0.3f);

        glVertex2f(0.6f, -0.3f);
        glVertex2f(-2.0f, -0.3f);
    glEnd();
}

void traffic_light1_Prince() {
    float x, y, radius, twicePi;
    int triangleAmount;

    // --- Stand (Modern Dark Gray) ---
    glBegin(GL_POLYGON);
        glColor3ub(60, 60, 60); // Dark gray stand
        glVertex2f(0.45f, 0.38f);
        glVertex2f(0.45f, 0.42f);
        glVertex2f(0.55f, 0.42f);
        glVertex2f(0.55f, 0.38f);
    glEnd();

    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glColor3ub(80, 80, 80); // slightly lighter pole line
        glVertex2f(0.55f, 0.4f);
        glVertex2f(0.7f, 0.4f);

        glVertex2f(0.7f, 0.4f);
        glVertex2f(0.7f, 0.15f);
    glEnd();

    // --- Light Housing (Sleek Style) ---
    glBegin(GL_POLYGON);
        glColor3ub(40, 40, 40); // dark gray housing
        glVertex2f(0.65f, 0.15f);
        glVertex2f(0.75f, 0.15f);
        glVertex2f(0.75f, -0.05f);
        glVertex2f(0.65f, -0.05f);
    glEnd();

    // Border lines for housing
    glLineWidth(2.0f);
    glColor3ub(200, 200, 200); // light gray border
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.65f, 0.15f);
        glVertex2f(0.75f, 0.15f);
        glVertex2f(0.75f, -0.05f);
        glVertex2f(0.65f, -0.05f);
    glEnd();

    // Side small design lines for style
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(0.65f, 0.1f); glVertex2f(0.6f, 0.1f);
        glVertex2f(0.65f, 0.05f); glVertex2f(0.62f, 0.05f);
        glVertex2f(0.65f, 0.0f); glVertex2f(0.63f, 0.0f);

        glVertex2f(0.75f, 0.1f); glVertex2f(0.8f, 0.1f);
        glVertex2f(0.75f, 0.05f); glVertex2f(0.78f, 0.05f);
        glVertex2f(0.75f, 0.0f); glVertex2f(0.77f, 0.0f);
    glEnd();

    // --- Lights ---
    triangleAmount = 50;
    twicePi = 2.0f * PI;

    // Red light
    x = 0.7f; y = 0.1f; radius = 0.02f;
    glColor3ub(220, 0, 0); // brighter red
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                x + (radius * cos(i * twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();

    // Yellow light
    x = 0.7f; y = 0.05f; radius = 0.02f;
    glColor3ub(255, 215, 0); // golden yellow
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                x + (radius * cos(i * twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();

    // Green light
    x = 0.7f; y = 0.0f; radius = 0.02f;
    glColor3ub(0, 180, 0); // brighter green
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                x + (radius * cos(i * twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();

    // Optional subtle light glow effect (like reflection)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Red glow
    x = 0.7f; y = 0.1f; radius = 0.025f;
    glColor4ub(255, 100, 100, 50);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++)
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
    glEnd();

    // Yellow glow
    x = 0.7f; y = 0.05f; radius = 0.025f;
    glColor4ub(255, 255, 100, 50);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++)
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
    glEnd();

    // Green glow
    x = 0.7f; y = 0.0f; radius = 0.025f;
    glColor4ub(100, 255, 100, 50);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++)
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
    glEnd();

    glDisable(GL_BLEND);
}

void traffic_light2_Prince() {
    float x, y, radius, twicePi;
    int triangleAmount;

    // --- Stand (Modern Dark Gray) ---
    glBegin(GL_POLYGON);
        glColor3ub(60, 60, 60); // Dark gray stand
        glVertex2f(1.28f, -0.35f);
        glVertex2f(1.28f, -0.45f);
        glVertex2f(1.32f, -0.45f);
        glVertex2f(1.32f, -0.35f);
    glEnd();

    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glColor3ub(80, 80, 80); // slightly lighter pole line
        glVertex2f(1.3f, -0.35f);
        glVertex2f(1.3f, -0.2f);

        glVertex2f(1.3f, -0.2f);
        glVertex2f(1.0f, -0.2f);
    glEnd();

    // --- Light Housing (Sleek Style) ---
    glBegin(GL_POLYGON);
        glColor3ub(40, 40, 40); // dark gray housing
        glVertex2f(0.9f, -0.1f);
        glVertex2f(1.0f, -0.1f);
        glVertex2f(1.0f, -0.3f);
        glVertex2f(0.9f, -0.3f);
    glEnd();

    // Border lines for housing
    glLineWidth(2.0f);
    glColor3ub(200, 200, 200); // light gray border
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.9f, -0.1f);
        glVertex2f(1.0f, -0.1f);
        glVertex2f(1.0f, -0.3f);
        glVertex2f(0.9f, -0.3f);
    glEnd();

    // Side small design lines for style
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(0.9f, -0.15f); glVertex2f(0.85f, -0.15f);
        glVertex2f(0.9f, -0.2f);  glVertex2f(0.87f, -0.2f);
        glVertex2f(0.9f, -0.25f); glVertex2f(0.88f, -0.25f);

        glVertex2f(1.0f, -0.15f); glVertex2f(1.05f, -0.15f);
        glVertex2f(1.0f, -0.2f);  glVertex2f(1.03f, -0.2f);
        glVertex2f(1.0f, -0.25f); glVertex2f(1.02f, -0.25f);
    glEnd();

    // --- Lights ---
    triangleAmount = 50;
    twicePi = 2.0f * PI;

    // Red light
    x = 0.95f; y = -0.15f; radius = 0.02f;
    glColor3ub(220, 0, 0); // brighter red
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++) {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
        }
    glEnd();

    // Yellow light
    x = 0.95f; y = -0.2f; radius = 0.02f;
    glColor3ub(255, 215, 0); // golden yellow
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++) {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
        }
    glEnd();

    // Green light
    x = 0.95f; y = -0.25f; radius = 0.02f;
    glColor3ub(0, 180, 0); // brighter green
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++) {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
        }
    glEnd();

    // --- Optional glow effect ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Red glow
    x = 0.95f; y = -0.15f; radius = 0.025f;
    glColor4ub(255, 100, 100, 50);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++)
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
    glEnd();

    // Yellow glow
    x = 0.95f; y = -0.2f; radius = 0.025f;
    glColor4ub(255, 255, 100, 50);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++)
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
    glEnd();

    // Green glow
    x = 0.95f; y = -0.25f; radius = 0.025f;
    glColor4ub(100, 255, 100, 50);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= triangleAmount; i++)
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                       y + (radius * sin(i * twicePi / triangleAmount)));
    glEnd();

    glDisable(GL_BLEND);
}


void bike1_Prince() {
  glPushMatrix();
glTranslatef(position_b1, fallOffset, 0.0f);

float wheelRadius = 0.06f;
float rimRadius = 0.04f;
int spokeCount = 16;

// 🛞 Rear Wheel
glColor3ub(0, 0,0); // Jet Black
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 2) {
    float rad = angle * PI / 180.0f;
    glVertex2f(-0.18f + wheelRadius * cos(rad), 0.05f + wheelRadius * sin(rad));
}
glEnd();

glColor3ub(255, 255, 255); // Pure White Rim
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 2) {
    float rad = angle * PI / 180.0f;
    glVertex2f(-0.18f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
}
glEnd();

glColor3ub(255, 0, 0); // Bright Red Spokes
for (float angle = 0; angle < 360; angle += 360 / spokeCount) {
    float rad = angle * PI / 180.0f;
    glBegin(GL_LINES);
    glVertex2f(-0.18f, 0.05f);
    glVertex2f(-0.18f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
    glEnd();
}

// 🛞 Front Wheel
glColor3ub(0, 0, 0); // Jet Black
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 2) {
    float rad = angle * PI / 180.0f;
    glVertex2f(0.12f + wheelRadius * cos(rad), 0.05f + wheelRadius * sin(rad));
}
glEnd();

glColor3ub(255, 255, 255); // Pure White Rim
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 2) {
    float rad = angle * PI / 180.0f;
    glVertex2f(0.12f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
}
glEnd();

glColor3ub(255, 0, 0); // Bright Red Spokes
for (float angle = 0; angle < 360; angle += 360 / spokeCount) {
    float rad = angle * PI / 180.0f;
    glBegin(GL_LINES);
    glVertex2f(0.12f, 0.05f);
    glVertex2f(0.12f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
    glEnd();
}

// 🧱 Steering Column
glColor3ub(80, 80, 80); // Dark Steel
glBegin(GL_POLYGON);
glVertex2f(-0.025f, 0.13f);
glVertex2f(0.025f, 0.13f);
glVertex2f(0.03f, 0.18f);
glVertex2f(-0.03f, 0.18f);
glEnd();

// 🪛 Spring Fork
glColor3ub(0, 255, 80); // Lime Green
glBegin(GL_LINES);
glVertex2f(0.025f, 0.18f);
glVertex2f(0.12f, 0.05f);
glVertex2f(-0.025f, 0.18f);
glVertex2f(0.12f, 0.05f);
glEnd();

// 🕹️ Handlebar
glColor3ub(0, 0, 0); // Jet Black
glBegin(GL_LINES);
glVertex2f(-0.06f, 0.20f);
glVertex2f(0.06f, 0.20f);
glEnd();

// ✋ Clutch Lever
glColor3ub(255, 255, 255); // White
glBegin(GL_LINES);
glVertex2f(-0.06f, 0.20f);
glVertex2f(-0.08f, 0.21f);
glEnd();
// 💡 Headlight (rounded, layered design)
glColor3ub(80, 80, 80); // Outer casing (dark steel)
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 5) {
    float rad = angle * PI / 180.0f;
    glVertex2f(0.0f + 0.02f * cos(rad), 0.145f + 0.02f * sin(rad));
}
glEnd();

glColor3ub(255, 255, 150); // Inner bulb (soft yellow glow)
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 5) {
    float rad = angle * PI / 180.0f;
    glVertex2f(0.0f + 0.012f * cos(rad), 0.145f + 0.012f * sin(rad));
}
glEnd();

// 🛢️ Fuel Tank
glColor3ub(255, 0, 40); // Scarlet Red
glBegin(GL_POLYGON);
glVertex2f(-0.08f, 0.13f);
glVertex2f(0.06f, 0.13f);
glVertex2f(0.07f, 0.145f);
glVertex2f(0.04f, 0.165f);
glVertex2f(-0.04f, 0.165f);
glVertex2f(-0.07f, 0.145f);
glEnd();

// ⛽ Fuel Meter
glColor3ub(255, 255, 255); // White Ring
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180.0f;
    glVertex2f(0.01f + 0.005f * cos(rad), 0.155f + 0.005f * sin(rad));
}
glEnd();
glColor3ub(255, 0, 0); // Red Needle
glBegin(GL_LINES);
glVertex2f(0.01f, 0.155f);
glVertex2f(0.015f, 0.158f);
glEnd();

// 🪑 Seat
glColor3ub(0, 0, 128); // Navy Blue
glBegin(GL_POLYGON);
glVertex2f(-0.14f, 0.115f);
glVertex2f(-0.015f, 0.115f);
glVertex2f(-0.005f, 0.125f);
glVertex2f(-0.015f, 0.14f);
glVertex2f(-0.11f, 0.145f);
glVertex2f(-0.14f, 0.13f);
glEnd();

// ⚙️ Engine Block
glColor3ub(60, 0, 255); // Electric Indigo
glBegin(GL_POLYGON);
glVertex2f(-0.05f, 0.08f);
glVertex2f(0.03f, 0.08f);
glVertex2f(0.03f, 0.12f);
glVertex2f(-0.05f, 0.12f);
glEnd();

// 🔗 Chain
glColor3ub(255, 204, 0); // Golden Yellow
glBegin(GL_LINES);
glVertex2f(0.03f, 0.08f);
glVertex2f(-0.18f, 0.05f);
glEnd();

// 🦶 Starter Pedal
glColor3ub(255, 255, 255); // White
glBegin(GL_LINES);
glVertex2f(-0.15f, 0.09f);
glVertex2f(-0.17f, 0.11f);
glEnd();

// 🔊 Silencer
glColor3ub(160, 0, 255); // Neon Violet
glBegin(GL_POLYGON);
glVertex2f(-0.10f, 0.07f);
glVertex2f(0.06f, 0.07f);
glVertex2f(0.06f, 0.09f);
glVertex2f(-0.10f, 0.09f);
glEnd();

// 🦶 Foot Pegs
glColor3ub(0, 255, 80); // Lime Green
glBegin(GL_LINES);
glVertex2f(-0.05f, 0.10f);
glVertex2f(-0.07f, 0.10f);
glVertex2f(0.05f, 0.10f);
glVertex2f(0.07f, 0.10f);
glEnd();

// 🧭 Speedometer Outer Ring
glColor3ub(255, 255, 255); // White Ring
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 5) {
    float rad = angle * PI / 180.0f;
    glVertex2f(0.0f + 0.015f * cos(rad), 0.20f + 0.015f * sin(rad));
}
glEnd();

// 🧭 Speedometer Inner Dial
glColor3ub(0, 0, 0); // Black Dial
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 5) {
    float rad = angle * PI / 180.0f;
    glVertex2f(0.0f + 0.010f * cos(rad), 0.20f + 0.010f * sin(rad));
}
glEnd();

// 🧭 Speedometer Needle
glColor3ub(0, 255, 0); // Bright Green Needle
glBegin(GL_LINES);
glVertex2f(0.0f, 0.20f);
glVertex2f(0.008f, 0.208f);
glEnd();

    // Rider Torso (aligned with seat)

// 🧍 Rider Torso (seated on seat)
glColor3ub(200, 0, 0); // red shirt
glBegin(GL_POLYGON);
glVertex2f(-0.11f, 0.145f);
glVertex2f(-0.07f, 0.145f);
glVertex2f(-0.07f, 0.185f);
glVertex2f(-0.11f, 0.185f);
glEnd();

// 🧠 Neck (angled forward)
glColor3ub(255, 224, 189); // skin tone
glBegin(GL_POLYGON);
glVertex2f(-0.09f, 0.185f);
glVertex2f(-0.085f, 0.185f);
glVertex2f(-0.082f, 0.195f);
glVertex2f(-0.087f, 0.195f);
glEnd();

// 🧑 Head (tilted forward, oval shape)
glColor3ub(255, 224, 189); // skin tone
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 5) {
    float rad = angle * PI / 180.0f;
    glVertex2f(-0.085f + 0.015f * cos(rad), 0.205f + 0.018f * sin(rad));
}
glEnd();

// 👁️ Eyes (centered forward)
glColor3ub(0, 0, 0); // black eyes
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180.0f;
    glVertex2f(-0.085f + 0.003f * cos(rad), 0.208f + 0.003f * sin(rad)); // left eye
}
glEnd();

glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180.0f;
    glVertex2f(-0.078f + 0.003f * cos(rad), 0.208f + 0.003f * sin(rad)); // right eye
}
glEnd();

// 👂 Ears (side profile)
glColor3ub(255, 224, 189); // skin tone
glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180.0f;
    glVertex2f(-0.098f + 0.004f * cos(rad), 0.205f + 0.004f * sin(rad)); // left ear
}
glEnd();

glBegin(GL_POLYGON);
for (float angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180.0f;
    glVertex2f(-0.072f + 0.004f * cos(rad), 0.205f + 0.004f * sin(rad)); // right ear
}
glEnd();

// 🪖 Helmet (angled forward)
if (hasHelmet) {
    glColor3ub(0, 128, 0); // green helmet shell
    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(-0.085f + 0.017f * cos(rad), 0.205f + 0.018f * sin(rad));
    }
    glEnd();

    glColor3ub(200, 255, 255); // light cyan visor
    glBegin(GL_POLYGON);
    for (float angle = 200; angle < 340; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(-0.085f + 0.012f * cos(rad), 0.205f + 0.012f * sin(rad));
    }
    glEnd();
}

// 🖐️ Arms (reaching handlebar)
glColor3ub(0, 0, 0); // black sleeves
glBegin(GL_LINES);
glVertex2f(-0.11f, 0.18f);
glVertex2f(-0.13f, 0.20f); // left arm
glVertex2f(-0.07f, 0.18f);
glVertex2f(-0.03f, 0.20f); // right arm
glEnd();

// 🦵 Legs (bent toward foot pegs)
glColor3ub(0, 100, 100); // teal pants
glBegin(GL_LINES);
glVertex2f(-0.10f, 0.145f);
glVertex2f(-0.12f, 0.11f); // left thigh
glVertex2f(-0.12f, 0.11f);
glVertex2f(-0.11f, 0.09f); // left shin

glVertex2f(-0.08f, 0.145f);
glVertex2f(-0.06f, 0.11f); // right thigh
glVertex2f(-0.06f, 0.11f);
glVertex2f(-0.07f, 0.09f); // right shin
glEnd();

// 👟 Shoes
glColor3ub(255, 140, 0); // orange shoes
glBegin(GL_POLYGON);
glVertex2f(-0.11f, 0.09f);
glVertex2f(-0.105f, 0.09f);
glVertex2f(-0.105f, 0.085f);
glVertex2f(-0.11f, 0.085f);
glEnd();

glBegin(GL_POLYGON);
glVertex2f(-0.07f, 0.09f);
glVertex2f(-0.075f, 0.09f);
glVertex2f(-0.075f, 0.085f);
glVertex2f(-0.07f, 0.085f);
glEnd();

glPopMatrix();
}

 void update_bike_accident_Prince(int value);




void bike2_Prince() {
    glPushMatrix();
    glTranslatef(position_b2, fallOffset - 0.10f, 0.0f); // Move bike2 slightly down by 0.10f
    glScalef(0.95f, 0.95f, 1.0f); // Slightly reduce size
    float x, y, radius, twicePi;
    int triangleAmount = 50;
    twicePi = 2.0f * PI;
    float wheelRadius = 0.06f;
    float rimRadius = 0.04f;
    int spokeCount = 16;
    // --- Front Wheel (LEFT side - direction of movement) ---
    glColor3ub(0, 0, 0); // Black wheel
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 2) {
        float rad = angle * PI / 180.0f;
        glVertex2f(-0.18f + wheelRadius * cos(rad), 0.05f + wheelRadius * sin(rad));
    }
    glEnd();
    glColor3ub(255, 255, 255); // White rim
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 2) {
        float rad = angle * PI / 180.0f;
        glVertex2f(-0.18f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
    }
    glEnd();
    glColor3ub(255, 0, 0); // Red spokes
    for(float angle = 0; angle < 360; angle += 360 / spokeCount) {
        float rad = angle * PI / 180.0f;
        glBegin(GL_LINES);
        glVertex2f(-0.18f, 0.05f);
        glVertex2f(-0.18f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
        glEnd();
    }
    // --- Rear Wheel (RIGHT side) ---
    glColor3ub(0, 0, 0);
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 2) {
        float rad = angle * PI / 180.0f;
        glVertex2f(0.12f + wheelRadius * cos(rad), 0.05f + wheelRadius * sin(rad));
    }
    glEnd();
    glColor3ub(255, 255, 255);
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 2) {
        float rad = angle * PI / 180.0f;
        glVertex2f(0.12f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
    }
    glEnd();
    glColor3ub(255, 0, 0);
    for(float angle = 0; angle < 360; angle += 360 / spokeCount) {
        float rad = angle * PI / 180.0f;
        glBegin(GL_LINES);
        glVertex2f(0.12f, 0.05f);
        glVertex2f(0.12f + rimRadius * cos(rad), 0.05f + rimRadius * sin(rad));
        glEnd();
    }
    // --- Main Frame (connecting wheels) ---
    glColor3ub(80, 80, 80);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.18f, 0.05f);
    glVertex2f(0.12f, 0.05f);
    glVertex2f(-0.02f, 0.05f);
    glVertex2f(-0.02f, 0.115f);
    glVertex2f(0.12f, 0.05f);
    glVertex2f(-0.02f, 0.115f);
    glEnd();
    glLineWidth(1.0f);
    // --- Steering Column ---
    glColor3ub(80, 80, 80);
    glBegin(GL_POLYGON);
    glVertex2f(-0.025f, 0.13f);
    glVertex2f(0.025f, 0.13f);
    glVertex2f(0.03f, 0.18f);
    glVertex2f(-0.03f, 0.18f);
    glEnd();
    // --- Front Fork (improved) ---
    glColor3ub(0, 255, 80);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(-0.025f, 0.18f);
    glVertex2f(-0.18f, 0.05f);
    glVertex2f(0.025f, 0.18f);
    glVertex2f(-0.18f, 0.05f);
    glEnd();
    glLineWidth(1.0f);
    // --- Handlebar (improved with slight increase) ---
    glColor3ub(0, 0, 0);
    glLineWidth(3.5f); // Slightly thicker for realism
    glBegin(GL_LINES);
    glVertex2f(-0.10f, 0.20f); // Extended left
    glVertex2f(0.08f, 0.20f); // Extended right
    glEnd();
    glLineWidth(1.0f);
    // --- Brake Lever (left side) ---
    glColor3ub(255, 255, 255);
    glBegin(GL_LINES);
    glVertex2f(-0.10f, 0.20f);
    glVertex2f(-0.12f, 0.21f);
    glEnd();
    // --- Throttle (right side) ---
    glColor3ub(200, 200, 200);
    glBegin(GL_LINES);
    glVertex2f(0.08f, 0.20f);
    glVertex2f(0.10f, 0.195f);
    glEnd();
    // --- Headlight (round shape) ---
    glColor3ub(80, 80, 80); // Housing
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(-0.02f + 0.025f * cos(rad), 0.145f + 0.025f * sin(rad));
    }
    glEnd();
    glColor3ub(255, 255, 150); // Bright lens
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(-0.02f + 0.020f * cos(rad), 0.145f + 0.020f * sin(rad));
    }
    glEnd();
    // --- Speedometer (round shape) ---
    glColor3ub(0, 0, 0); // Black housing
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(0.02f + 0.020f * cos(rad), 0.145f + 0.020f * sin(rad));
    }
    glEnd();
    glColor3ub(255, 255, 255); // White face
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(0.02f + 0.015f * cos(rad), 0.145f + 0.015f * sin(rad));
    }
    glEnd();
    // --- Fuel Tank (adjusted for forward direction) ---
    glColor3ub(255, 51, 0); // Vibrant red
    glBegin(GL_POLYGON);
    glVertex2f(-0.08f, 0.13f);
    glVertex2f(0.06f, 0.13f);
    glVertex2f(0.05f, 0.145f);
    glVertex2f(0.02f, 0.165f);
    glVertex2f(-0.06f, 0.165f);
    glVertex2f(-0.09f, 0.145f);
    glEnd();
    // --- Tank Cap ---
    glColor3ub(150, 150, 150);
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 10) {
        float rad = angle * PI / 180.0f;
        glVertex2f(-0.01f + 0.008f * cos(rad), 0.158f + 0.008f * sin(rad));
    }
    glEnd();
    // --- Seat (adjusted for forward direction, rider sitting) ---
    glColor3ub(0, 0, 128); // Dark blue
    glBegin(GL_POLYGON);
    glVertex2f(-0.10f, 0.12f); // Shifted back
    glVertex2f(0.00f, 0.12f);
    glVertex2f(0.01f, 0.14f);
    glVertex2f(-0.09f, 0.14f);
    glEnd();
    // --- Engine Box Cover (sky blue) ---
    glColor3ub(135, 206, 235); // Sky blue
    glBegin(GL_POLYGON);
    glVertex2f(-0.08f, 0.05f);
    glVertex2f(0.02f, 0.05f);
    glVertex2f(0.02f, 0.09f);
    glVertex2f(-0.08f, 0.09f);
    glEnd();
    // --- Rider Body (improved structure) ---
    glColor3ub(0, 100, 150); // Blue jacket
    glBegin(GL_POLYGON);
    glVertex2f(-0.05f, 0.12f); // Adjusted to sit on seat
    glVertex2f(0.05f, 0.12f);
    glVertex2f(0.04f, 0.17f);
    glVertex2f(-0.04f, 0.17f);
    glEnd();
    // --- Rider Head (facing forward) ---
    glColor3ub(255, 204, 153); // Skin tone
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180.0f;
        glVertex2f(0.0f + 0.018f * cos(rad), 0.18f + 0.020f * sin(rad));
    }
    glEnd();
    // --- Rider Arms (improved, adjusted to handlebar) ---
    glColor3ub(0, 100, 150); // Blue sleeves
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(-0.04f, 0.15f); // Adjusted for forward position
    glVertex2f(-0.10f, 0.20f);
    glVertex2f(0.04f, 0.15f);
    glVertex2f(0.08f, 0.20f);
    glEnd();
    glLineWidth(1.0f);
    // --- Rider Legs (improved with thickness, forward position) ---
    glColor3ub(50, 50, 100); // Dark blue pants
    glBegin(GL_POLYGON); // Left leg
    glVertex2f(-0.03f, 0.12f);
    glVertex2f(-0.02f, 0.12f);
    glVertex2f(-0.08f, 0.08f);
    glVertex2f(-0.09f, 0.08f);
    glEnd();
    glBegin(GL_POLYGON); // Right leg
    glVertex2f(0.02f, 0.12f);
    glVertex2f(0.03f, 0.12f);
    glVertex2f(0.07f, 0.08f);
    glVertex2f(0.08f, 0.08f);
    glEnd();
    // --- Rider Feet/Boots ---
    glColor3ub(139, 69, 19); // Brown
    glBegin(GL_POLYGON);
    glVertex2f(-0.09f, 0.08f); glVertex2f(-0.08f, 0.08f);
    glVertex2f(-0.08f, 0.06f); glVertex2f(-0.09f, 0.06f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.08f, 0.08f); glVertex2f(0.07f, 0.08f);
    glVertex2f(0.07f, 0.06f); glVertex2f(0.08f, 0.06f);
    glEnd();
    // --- Exhaust Pipe ---
    glColor3ub(120, 120, 120);
    glBegin(GL_POLYGON);
    glVertex2f(0.08f, 0.07f);
    glVertex2f(0.15f, 0.07f);
    glVertex2f(0.15f, 0.08f);
    glVertex2f(0.08f, 0.08f);
    glEnd();
    // --- Exhaust End ---
    glColor3ub(80, 80, 80);
    glBegin(GL_POLYGON);
    for(float angle = 0; angle < 360; angle += 10) {
        float rad = angle * PI / 180.0f;
        glVertex2f(0.15f + 0.008f * cos(rad), 0.075f + 0.008f * sin(rad));
    }
    glEnd();
    // --- Helmet (if worn, red) ---
    if(hasHelmet) {
        glColor3ub(255, 0, 0); // Red
        glBegin(GL_POLYGON);
        for(float angle = 0; angle < 360; angle += 5) {
            float rad = angle * PI / 180.0f;
            glVertex2f(0.0f + 0.020f * cos(rad), 0.18f + 0.022f * sin(rad));
        }
        glEnd();
        glColor3ub(0, 0, 0); // Black visor
        glBegin(GL_POLYGON);
        for(float angle = 180; angle < 360; angle += 5) {
            float rad = angle * PI / 180.0f;
            glVertex2f(0.0f + 0.015f * cos(rad), 0.18f + 0.015f * sin(rad));
        }
        glEnd();
    }
    glPopMatrix();
}


void bike3_Prince() {
    glPushMatrix();
    glTranslatef(0.8, position_b3, 0.0f); // Slightly more left on road

    // Rear wheel (detailed, reduced size, black tire, silver rim, gray spokes)
    glBegin(GL_POLYGON); // Outer tire (black rubber)
        glColor3ub(0, 0, 0); // Black
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.035f * cos(angle), -0.12f + 0.035f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Rim (silver)
        glColor3ub(192, 192, 192); // Silver
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.03f * cos(angle), -0.12f + 0.03f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Hub (gray)
        glColor3ub(128, 128, 128); // Gray
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.008f * cos(angle), -0.12f + 0.008f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Spokes (gray, more for detail)
        glColor3ub(128, 128, 128);
        for (int i = 0; i < 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f, -0.12f);
            glVertex2f(0.0f + 0.03f * cos(angle), -0.12f + 0.03f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Tire treads (dark gray, more lines)
        glColor3ub(50, 50, 50);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.03f * cos(angle), -0.12f + 0.03f * sin(angle));
            glVertex2f(0.0f + 0.035f * cos(angle), -0.12f + 0.035f * sin(angle));
        }
    glEnd();

    // Front wheel (same as rear, reduced size)
    glBegin(GL_POLYGON); // Outer tire
        glColor3ub(0, 0, 0);
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.035f * cos(angle), 0.12f + 0.035f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Rim
        glColor3ub(192, 192, 192);
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.03f * cos(angle), 0.12f + 0.03f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Hub
        glColor3ub(128, 128, 128);
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.008f * cos(angle), 0.12f + 0.008f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Spokes
        glColor3ub(128, 128, 128);
        for (int i = 0; i < 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f, 0.12f);
            glVertex2f(0.0f + 0.03f * cos(angle), 0.12f + 0.03f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Tire treads
        glColor3ub(50, 50, 50);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.03f * cos(angle), 0.12f + 0.03f * sin(angle));
            glVertex2f(0.0f + 0.035f * cos(angle), 0.12f + 0.035f * sin(angle));
        }
    glEnd();

    // Front fender (black)
    glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0); // Black
        glVertex2f(-0.04f, 0.12f);
        glVertex2f(0.04f, 0.12f);
        glVertex2f(0.04f, 0.15f);
        glVertex2f(0.03f, 0.18f);
        glVertex2f(-0.03f, 0.18f);
        glVertex2f(-0.04f, 0.15f);
    glEnd();

    // Front forks (silver)
    glBegin(GL_POLYGON); // Left fork
        glColor3ub(192, 192, 192); // Silver
        glVertex2f(-0.02f, 0.12f);
        glVertex2f(-0.01f, 0.12f);
        glVertex2f(-0.01f, 0.2f);
        glVertex2f(-0.02f, 0.2f);
    glEnd();
    glBegin(GL_POLYGON); // Right fork
        glColor3ub(192, 192, 192);
        glVertex2f(0.01f, 0.12f);
        glVertex2f(0.02f, 0.12f);
        glVertex2f(0.02f, 0.2f);
        glVertex2f(0.01f, 0.2f);
    glEnd();

    // Frame (red)
    glBegin(GL_POLYGON);
        glColor3ub(255, 0, 0); // Red
        glVertex2f(-0.03f, -0.11f);
        glVertex2f(0.03f, -0.11f);
        glVertex2f(0.035f, -0.06f);
        glVertex2f(0.035f, 0.06f);
        glVertex2f(0.0f, 0.11f);
        glVertex2f(-0.035f, 0.06f);
        glVertex2f(-0.035f, -0.06f);
    glEnd();

    // Fuel tank (red, improved shape)
    glBegin(GL_POLYGON);
        glColor3ub(255, 0, 0); // Red
        glVertex2f(-0.04f, -0.03f);
        glVertex2f(0.04f, -0.03f);
        glVertex2f(0.045f, 0.0f);
        glVertex2f(0.045f, 0.03f);
        glVertex2f(0.0f, 0.05f);
        glVertex2f(-0.045f, 0.03f);
        glVertex2f(-0.045f, 0.0f);
    glEnd();

    // Seat (black)
    glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0); // Black
        glVertex2f(-0.03f, -0.09f);
        glVertex2f(0.03f, -0.09f);
        glVertex2f(0.035f, -0.03f);
        glVertex2f(0.0f, -0.01f);
        glVertex2f(-0.035f, -0.03f);
    glEnd();

    // Engine block (gray)
    glBegin(GL_POLYGON);
        glColor3ub(100, 100, 100); // Gray
        glVertex2f(-0.03f, -0.03f);
        glVertex2f(0.03f, -0.03f);
        glVertex2f(0.03f, 0.03f);
        glVertex2f(-0.03f, 0.03f);
    glEnd();
    glBegin(GL_LINES); // Cooling fins
        glColor3ub(80, 80, 80);
        glVertex2f(-0.035f, -0.02f);
        glVertex2f(-0.03f, -0.02f);
        glVertex2f(-0.035f, 0.02f);
        glVertex2f(-0.03f, 0.02f);
        glVertex2f(0.03f, -0.02f);
        glVertex2f(0.035f, -0.02f);
        glVertex2f(0.03f, 0.02f);
        glVertex2f(0.035f, 0.02f);
    glEnd();

    // Cylinders (gray)
    glBegin(GL_POLYGON); // Left cylinder
        glColor3ub(100, 100, 100);
        glVertex2f(-0.04f, -0.03f);
        glVertex2f(-0.03f, -0.03f);
        glVertex2f(-0.035f, 0.0f);
        glVertex2f(-0.045f, 0.0f);
    glEnd();
    glBegin(GL_LINES); // Fins
        glColor3ub(80, 80, 80);
        glVertex2f(-0.04f, -0.02f);
        glVertex2f(-0.035f, -0.02f);
        glVertex2f(-0.04f, 0.0f);
        glVertex2f(-0.035f, 0.0f);
    glEnd();
    glBegin(GL_POLYGON); // Right cylinder
        glColor3ub(100, 100, 100);
        glVertex2f(0.03f, -0.03f);
        glVertex2f(0.04f, -0.03f);
        glVertex2f(0.045f, 0.0f);
        glVertex2f(0.035f, 0.0f);
    glEnd();
    glBegin(GL_LINES); // Fins
        glColor3ub(80, 80, 80);
        glVertex2f(0.035f, -0.02f);
        glVertex2f(0.04f, -0.02f);
        glVertex2f(0.035f, 0.0f);
        glVertex2f(0.04f, 0.0f);
    glEnd();

    // Pistons (silver)
    glBegin(GL_LINES);
        glColor3ub(192, 192, 192);
        glVertex2f(-0.037f, -0.03f);
        glVertex2f(-0.037f, 0.0f);
        glVertex2f(0.037f, -0.03f);
        glVertex2f(0.037f, 0.0f);
    glEnd();

    // Clutch (silver)
    glBegin(GL_POLYGON);
        glColor3ub(192, 192, 192); // Silver
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.04f + 0.018f * cos(angle), -0.03f + 0.018f * sin(angle));
        }
    glEnd();
    glBegin(GL_POINTS); // Bolts
        glPointSize(2.0f);
        glColor3ub(0, 0, 0);
        glVertex2f(0.04f + 0.01f, -0.03f);
        glVertex2f(0.04f - 0.01f, -0.03f);
        glVertex2f(0.04f, -0.03f + 0.01f);
    glEnd();

    // Handlebars (silver)
    glBegin(GL_POLYGON);
        glColor3ub(192, 192, 192); // Silver
        glVertex2f(-0.04f, 0.11f);
        glVertex2f(0.04f, 0.11f);
        glVertex2f(0.04f, 0.13f);
        glVertex2f(-0.04f, 0.13f);
    glEnd();
    glBegin(GL_LINES); // Grips (black)
        glColor3ub(0, 0, 0);
        glVertex2f(-0.04f, 0.12f);
        glVertex2f(-0.05f, 0.12f);
        glVertex2f(0.04f, 0.12f);
        glVertex2f(0.05f, 0.12f);
    glEnd();

    // Speedometer (black)
    glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0); // Black
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.015f * cos(angle), 0.12f + 0.015f * sin(angle));
        }
    glEnd();

    // Headlight (white, more realistic)
    glBegin(GL_POLYGON);
        glColor3ub(242, 242, 242); // White
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.02f * cos(angle), 0.14f + 0.02f * sin(angle));
        }
    glEnd();
    if (flag != 0) {
        glBegin(GL_POLYGON); // Headlight beam
            glColor3ub(255, 255, 204); // Yellow
            glVertex2f(-0.02f, 0.14f);
            glVertex2f(0.02f, 0.14f);
            glVertex2f(0.04f, 0.3f);
            glVertex2f(-0.04f, 0.3f);
        glEnd();
    }

    // Chain (dark gray, more details)
    glBegin(GL_LINES);
        glColor3ub(80, 80, 80); // Dark gray
        glVertex2f(0.04f, -0.09f);
        glVertex2f(0.02f, -0.12f);
        glVertex2f(0.04f, -0.08f);
        glVertex2f(0.02f, -0.11f);
        glVertex2f(-0.02f, -0.12f);
        glVertex2f(-0.04f, -0.09f);
    glEnd();
    glBegin(GL_QUADS); // Chain links
        glColor3ub(60, 60, 60);
        glVertex2f(0.035f, -0.085f);
        glVertex2f(0.04f, -0.085f);
        glVertex2f(0.04f, -0.095f);
        glVertex2f(0.035f, -0.095f);
        glVertex2f(0.025f, -0.115f);
        glVertex2f(0.03f, -0.115f);
        glVertex2f(0.03f, -0.125f);
        glVertex2f(0.025f, -0.125f);
    glEnd();

    // Rider (improved, better head and red helmet)
    // Torso
    glBegin(GL_POLYGON);
        glColor3ub(0, 0, 255); // Blue jacket
        glVertex2f(-0.03f, -0.03f);
        glVertex2f(0.03f, -0.03f);
        glVertex2f(0.035f, 0.03f);
        glVertex2f(-0.035f, 0.03f);
    glEnd();

    // Arms
    glBegin(GL_POLYGON); // Left arm
        glColor3ub(0, 0, 255);
        glVertex2f(-0.03f, 0.03f);
        glVertex2f(-0.02f, 0.03f);
        glVertex2f(-0.04f, 0.09f);
        glVertex2f(-0.05f, 0.09f);
    glEnd();
    glBegin(GL_POLYGON); // Right arm
        glColor3ub(0, 0, 255);
        glVertex2f(0.02f, 0.03f);
        glVertex2f(0.03f, 0.03f);
        glVertex2f(0.05f, 0.09f);
        glVertex2f(0.04f, 0.09f);
    glEnd();

    // Legs
    glBegin(GL_POLYGON); // Left leg
        glColor3ub(0, 0, 0); // Black pants
        glVertex2f(-0.03f, -0.03f);
        glVertex2f(-0.02f, -0.03f);
        glVertex2f(-0.03f, -0.09f);
        glVertex2f(-0.04f, -0.09f);
    glEnd();
    glBegin(GL_POLYGON); // Right leg
        glColor3ub(0, 0, 0);
        glVertex2f(0.02f, -0.03f);
        glVertex2f(0.03f, -0.03f);
        glVertex2f(0.04f, -0.09f);
        glVertex2f(0.03f, -0.09f);
    glEnd();

    // Boots
    glBegin(GL_POLYGON); // Left boot
        glColor3ub(139, 69, 19); // Brown
        glVertex2f(-0.04f, -0.09f);
        glVertex2f(-0.03f, -0.09f);
        glVertex2f(-0.03f, -0.1f);
        glVertex2f(-0.04f, -0.1f);
    glEnd();
    glBegin(GL_POLYGON); // Right boot
        glColor3ub(139, 69, 19);
        glVertex2f(0.03f, -0.09f);
        glVertex2f(0.04f, -0.09f);
        glVertex2f(0.04f, -0.1f);
        glVertex2f(0.03f, -0.1f);
    glEnd();

    // Head
    glBegin(GL_POLYGON);
        glColor3ub(255, 204, 153); // Skin
        for (int i = 0; i <= 360; i += 5) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.02f * cos(angle), 0.04f + 0.02f * sin(angle));
        }
    glEnd();

    // Helmet (red)
    if (hasHelmet) {
        glBegin(GL_POLYGON);
            glColor3ub(255, 0, 0); // Red
            for (int i = 0; i <= 360; i += 5) {
                float angle = i * PI / 180.0f;
                glVertex2f(0.0f + 0.025f * cos(angle), 0.04f + 0.025f * sin(angle));
            }
        glEnd();
        glBegin(GL_POLYGON); // Visor (black)
            glColor3ub(0, 0, 0);
            glVertex2f(-0.02f, 0.04f);
            glVertex2f(0.02f, 0.04f);
            glVertex2f(0.025f, 0.06f);
            glVertex2f(-0.025f, 0.06f);
        glEnd();
    }

    glPopMatrix();
}



void bike4_Prince() {
    glPushMatrix();
    glTranslatef(1.0f, position_b4 - 0.02f, 0.0f); // Adjusted position slightly to the right
    // Front wheel (now at bottom, detailed, reduced size, black tire, silver rim, gray spokes)
    glBegin(GL_POLYGON); // Outer tire (black rubber)
        glColor3ub(0, 0, 0); // Black
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.035f * cos(angle), -0.12f + 0.035f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Rim (silver)
        glColor3ub(192, 192, 192); // Silver
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.03f * cos(angle), -0.12f + 0.03f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Hub (gray)
        glColor3ub(128, 128, 128); // Gray
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.008f * cos(angle), -0.12f + 0.008f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Spokes (gray, more for detail)
        glColor3ub(128, 128, 128);
        for (int i = 0; i < 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f, -0.12f);
            glVertex2f(0.0f + 0.03f * cos(angle), -0.12f + 0.03f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Tire treads (dark gray, more lines)
        glColor3ub(50, 50, 50);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.03f * cos(angle), -0.12f + 0.03f * sin(angle));
            glVertex2f(0.0f + 0.035f * cos(angle), -0.12f + 0.035f * sin(angle));
        }
    glEnd();
    // Front brake disc (dark silver)
    glBegin(GL_POLYGON);
        glColor3ub(169, 169, 169);
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.02f + 0.02f * cos(angle), -0.12f + 0.02f * sin(angle));
        }
    glEnd();
    // Front fender (black, now at bottom)
    glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0); // Black
        glVertex2f(-0.04f, -0.12f);
        glVertex2f(0.04f, -0.12f);
        glVertex2f(0.04f, -0.15f);
        glVertex2f(0.03f, -0.18f);
        glVertex2f(-0.03f, -0.18f);
        glVertex2f(-0.04f, -0.15f);
    glEnd();
    // Front forks (adjusted to white-like color, slightly repositioned)
    glBegin(GL_POLYGON); // Left fork
        glColor3ub(220, 220, 220); // Lighter silver, closer to white
        glVertex2f(-0.02f, -0.12f);
        glVertex2f(-0.01f, -0.12f);
        glVertex2f(-0.01f, -0.20f);
        glVertex2f(-0.02f, -0.20f);
    glEnd();
    glBegin(GL_POLYGON); // Right fork
        glColor3ub(220, 220, 220); // Lighter silver, closer to white
        glVertex2f(0.01f, -0.12f);
        glVertex2f(0.02f, -0.12f);
        glVertex2f(0.02f, -0.20f);
        glVertex2f(0.01f, -0.20f);
    glEnd();
    // Fuel tank (red, improved shape, now at bottom)
    glBegin(GL_POLYGON);
        glColor3ub(255, 0, 0); // Red
        glVertex2f(-0.04f, -0.03f);
        glVertex2f(0.04f, -0.03f);
        glVertex2f(0.045f, 0.0f);
        glVertex2f(0.045f, 0.03f);
        glVertex2f(0.0f, 0.05f);
        glVertex2f(-0.045f, 0.03f);
        glVertex2f(-0.045f, 0.0f);
    glEnd();
    // Seat (black, now at bottom)
    glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0); // Black
        glVertex2f(-0.03f, -0.09f);
        glVertex2f(0.03f, -0.09f);
        glVertex2f(0.035f, -0.03f);
        glVertex2f(0.0f, -0.01f);
        glVertex2f(-0.035f, -0.03f);
    glEnd();
    // Engine block (gray, now at bottom)
    glBegin(GL_POLYGON);
        glColor3ub(100, 100, 100); // Gray
        glVertex2f(-0.03f, -0.03f);
        glVertex2f(0.03f, -0.03f);
        glVertex2f(0.03f, 0.03f);
        glVertex2f(-0.03f, 0.03f);
    glEnd();
    glBegin(GL_LINES); // Cooling fins
        glColor3ub(80, 80, 80);
        glVertex2f(-0.035f, -0.02f);
        glVertex2f(-0.03f, -0.02f);
        glVertex2f(-0.035f, 0.02f);
        glVertex2f(-0.03f, 0.02f);
        glVertex2f(0.03f, -0.02f);
        glVertex2f(0.035f, -0.02f);
        glVertex2f(0.03f, 0.02f);
        glVertex2f(0.035f, 0.02f);
    glEnd();
    // Cylinders (gray, now at bottom)
    glBegin(GL_POLYGON); // Left cylinder
        glColor3ub(100, 100, 100);
        glVertex2f(-0.04f, -0.03f);
        glVertex2f(-0.03f, -0.03f);
        glVertex2f(-0.035f, 0.0f);
        glVertex2f(-0.045f, 0.0f);
    glEnd();
    glBegin(GL_LINES); // Fins
        glColor3ub(80, 80, 80);
        glVertex2f(-0.04f, -0.02f);
        glVertex2f(-0.035f, -0.02f);
        glVertex2f(-0.04f, 0.0f);
        glVertex2f(-0.035f, 0.0f);
    glEnd();
    glBegin(GL_POLYGON); // Right cylinder
        glColor3ub(100, 100, 100);
        glVertex2f(0.03f, -0.03f);
        glVertex2f(0.04f, -0.03f);
        glVertex2f(0.045f, 0.0f);
        glVertex2f(0.035f, 0.0f);
    glEnd();
    glBegin(GL_LINES); // Fins
        glColor3ub(80, 80, 80);
        glVertex2f(0.035f, -0.02f);
        glVertex2f(0.04f, -0.02f);
        glVertex2f(0.035f, 0.0f);
        glVertex2f(0.04f, 0.0f);
    glEnd();
    // Pistons (silver, now at bottom)
    glBegin(GL_LINES);
        glColor3ub(192, 192, 192);
        glVertex2f(-0.037f, -0.03f);
        glVertex2f(-0.037f, 0.0f);
        glVertex2f(0.037f, -0.03f);
        glVertex2f(0.037f, 0.0f);
    glEnd();
    // Clutch (silver, now at bottom)
    glBegin(GL_POLYGON);
        glColor3ub(192, 192, 192); // Silver
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.04f + 0.018f * cos(angle), -0.03f + 0.018f * sin(angle));
        }
    glEnd();
    glBegin(GL_POINTS); // Bolts
        glPointSize(2.0f);
        glColor3ub(0, 0, 0);
        glVertex2f(0.04f + 0.01f, -0.03f);
        glVertex2f(0.04f - 0.01f, -0.03f);
        glVertex2f(0.04f, -0.03f + 0.01f);
    glEnd();
    // Chain (dark gray, more details, now at bottom)
    glBegin(GL_LINES);
        glColor3ub(80, 80, 80); // Dark gray
        glVertex2f(0.04f, -0.09f);
        glVertex2f(0.02f, -0.12f);
        glVertex2f(0.04f, -0.08f);
        glVertex2f(0.02f, -0.11f);
        glVertex2f(-0.02f, -0.12f);
        glVertex2f(-0.04f, -0.09f);
    glEnd();
    glBegin(GL_QUADS); // Chain links
        glColor3ub(60, 60, 60);
        glVertex2f(0.035f, -0.085f);
        glVertex2f(0.04f, -0.085f);
        glVertex2f(0.04f, -0.095f);
        glVertex2f(0.035f, -0.095f);
        glVertex2f(0.025f, -0.115f);
        glVertex2f(0.03f, -0.115f);
        glVertex2f(0.03f, -0.125f);
        glVertex2f(0.025f, -0.125f);
    glEnd();
    // Rear wheel (now at top, detailed, reduced size, black tire, silver rim, gray spokes)
    glBegin(GL_POLYGON); // Outer tire (black rubber)
        glColor3ub(0, 0, 0); // Black
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.04f * cos(angle), 0.12f + 0.04f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Rim (silver)
        glColor3ub(192, 192, 192); // Silver
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.035f * cos(angle), 0.12f + 0.035f * sin(angle));
        }
    glEnd();
    glBegin(GL_POLYGON); // Hub (gray)
        glColor3ub(128, 128, 128); // Gray
        for (int i = 0; i <= 360; i += 2) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.008f * cos(angle), 0.12f + 0.008f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Spokes (gray, more for detail)
        glColor3ub(128, 128, 128);
        for (int i = 0; i < 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f, 0.12f);
            glVertex2f(0.0f + 0.035f * cos(angle), 0.12f + 0.035f * sin(angle));
        }
    glEnd();
    glBegin(GL_LINES); // Tire treads (dark gray, more lines)
        glColor3ub(50, 50, 50);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.035f * cos(angle), 0.12f + 0.035f * sin(angle));
            glVertex2f(0.0f + 0.04f * cos(angle), 0.12f + 0.04f * sin(angle));
        }
    glEnd();
    // Rear brake disc (dark silver)
    glBegin(GL_POLYGON);
        glColor3ub(169, 169, 169);
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * PI / 180.0f;
            glVertex2f(-0.02f + 0.02f * cos(angle), 0.12f + 0.02f * sin(angle));
        }
    glEnd();
    // Frame (red, adjusted for new orientation)
    glBegin(GL_POLYGON);
        glColor3ub(255, 0, 0); // Red
        glVertex2f(-0.03f, -0.11f);
        glVertex2f(0.03f, -0.11f);
        glVertex2f(0.035f, -0.06f);
        glVertex2f(0.035f, 0.06f);
        glVertex2f(0.0f, 0.11f);
        glVertex2f(-0.035f, 0.06f);
        glVertex2f(-0.035f, -0.06f);
    glEnd();
    // Rider (adjusted for new front-down orientation, slightly back position)
    // Torso
    glBegin(GL_POLYGON);
        glColor3ub(0, 0, 255); // Blue jacket
        glVertex2f(-0.03f, 0.02f);
        glVertex2f(0.03f, 0.02f);
        glVertex2f(0.035f, -0.04f);
        glVertex2f(-0.035f, -0.04f);
    glEnd();
    // Arms
    glBegin(GL_POLYGON); // Left arm
        glColor3ub(0, 0, 255);
        glVertex2f(-0.03f, -0.04f);
        glVertex2f(-0.02f, -0.04f);
        glVertex2f(-0.04f, -0.10f);
        glVertex2f(-0.05f, -0.10f);
    glEnd();
    glBegin(GL_POLYGON); // Right arm
        glColor3ub(0, 0, 255);
        glVertex2f(0.02f, -0.04f);
        glVertex2f(0.03f, -0.04f);
        glVertex2f(0.05f, -0.10f);
        glVertex2f(0.04f, -0.10f);
    glEnd();
    // Legs
    glBegin(GL_POLYGON); // Left leg
        glColor3ub(0, 0, 0); // Black pants
        glVertex2f(-0.03f, 0.02f);
        glVertex2f(-0.02f, 0.02f);
        glVertex2f(-0.03f, 0.08f);
        glVertex2f(-0.04f, 0.08f);
    glEnd();
    glBegin(GL_POLYGON); // Right leg
        glColor3ub(0, 0, 0);
        glVertex2f(0.02f, 0.02f);
        glVertex2f(0.03f, 0.02f);
        glVertex2f(0.04f, 0.08f);
        glVertex2f(0.03f, 0.08f);
    glEnd();
    // Boots
    glBegin(GL_POLYGON); // Left boot
        glColor3ub(139, 69, 19); // Brown
        glVertex2f(-0.04f, 0.08f);
        glVertex2f(-0.03f, 0.08f);
        glVertex2f(-0.03f, 0.09f);
        glVertex2f(-0.04f, 0.09f);
    glEnd();
    glBegin(GL_POLYGON); // Right boot
        glColor3ub(139, 69, 19);
        glVertex2f(0.03f, 0.08f);
        glVertex2f(0.04f, 0.08f);
        glVertex2f(0.04f, 0.09f);
        glVertex2f(0.03f, 0.09f);
    glEnd();
    // Head
    glBegin(GL_POLYGON);
        glColor3ub(255, 204, 153); // Skin
        for (int i = 0; i <= 360; i += 5) {
            float angle = i * PI / 180.0f;
            glVertex2f(0.0f + 0.02f * cos(angle), -0.05f + 0.02f * sin(angle));
        }
    glEnd();
    // Helmet (red)
    if (hasHelmet) {
        glBegin(GL_POLYGON);
            glColor3ub(255, 0, 0); // Red
            for (int i = 0; i <= 360; i += 5) {
                float angle = i * PI / 180.0f;
                glVertex2f(0.0f + 0.025f * cos(angle), -0.05f + 0.025f * sin(angle));
            }
        glEnd();
        glBegin(GL_POLYGON); // Visor (black)
            glColor3ub(0, 0, 0);
            glVertex2f(-0.02f, -0.05f);
            glVertex2f(0.02f, -0.05f);
            glVertex2f(0.025f, -0.07f);
            glVertex2f(-0.025f, -0.07f);
        glEnd();
    }
    glPopMatrix();
}




void redgreen1_Prince () {
    x=0.7f; y=0.1f; radius =.02f; // red
	triangleAmount = 50;
	twicePi = 2.0f * PI;
	glColor3ub(255, 51, 51);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(int i = 0; i <= triangleAmount;i++) {
			glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)),
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();

	x=0.95f; y=-0.25f; radius =.02f; // green
	triangleAmount = 50;
	twicePi = 2.0f * PI;
	glColor3ub(0, 255, 0);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(int i = 0; i <= triangleAmount;i++) {
			glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)),
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}

void redgreen2_Prince () {
    x=0.7f; y=0.0f; radius =.02f; // green
	triangleAmount = 50;
	twicePi = 2.0f * PI;
	glColor3ub(0, 255, 0);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(int i = 0; i <= triangleAmount;i++) {
			glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)),
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();

	x=0.95f; y=-0.15f; radius =.02f; // red
	triangleAmount = 50;
	twicePi = 2.0f * PI;
	glColor3ub(255, 51, 51);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(int i = 0; i <= triangleAmount;i++) {
			glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)),
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}



void day_Prince() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // --- Background ---
    glBegin(GL_POLYGON);
        glColor3ub(135, 206, 235);  // LightSkyBlue
        glVertex2f(2.0f, 1.0f);
        glVertex2f(2.0f, -1.0f);
        glVertex2f(-2.0f, -1.0f);
        glVertex2f(-2.0f, 1.0f);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3ub(135, 206, 235);
        glVertex2f(0.35f, -0.5f);
        glVertex2f(1.9f, -0.5f);
        glVertex2f(1.9f, -1.0f);
        glVertex2f(0.35f, -1.0f);
    glEnd();

    // --- Scene Objects ---
    trafficPolice_Prince();
    road_footpath_Prince();
    bike1_Prince();
    bike2_Prince();
    bike3_Prince();
    bike4_Prince();
    traffic_light1_Prince();
    traffic_light2_Prince();
    fence_Prince();
    shop_Prince();
    house_Prince();

    // --- Shop Open Sign ---
    glBegin(GL_POLYGON);
        glColor3ub(255, 255, 204);
        glVertex2f(-1.72f, 0.57f);
        glVertex2f(-1.72f, 0.6f);
        glVertex2f(-1.65f, 0.6f);
        glVertex2f(-1.65f, 0.57f);
    glEnd();

    glColor3ub(255, 100, 10);
    c = "closed";
    glRasterPos2f(-1.715 , 0.58);
    for(int i = 0; c[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c[i]);

    // --- Traffic Police ---
    trafficPolice_Prince();

    // --- Overspeeding Warning ---
    if (showOverspeeding) {
        glColor3ub(255, 10, 4);
        const char* warning = " Dangerous ";
        glRasterPos2f(-0.2f, 0.4f);
        for (int i = 0; warning[i] != '\0'; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, warning[i]);
    }

    // --- Signal Logic ---
    if(cnt == 0)
        redgreen2_Prince();
    else
        redgreen1_Prince();

    glutSwapBuffers();
}




void update_bike1_Prince(int value) {
    if (cnt == 0) {
        speed_b1 = 0.01f;
        if (position_b1 > 2.7f)
            position_b1 = -2.7f;
        position_b1 += speed_b1;
    } else {
        if (position_b1 > 0.0f) {
            speed_b1 = 0.0f;
            position_b1 = 0.0f;
        }
        position_b1 += speed_b1;
    }

    glutPostRedisplay();
    glutTimerFunc(10, update_bike1_Prince, 0);
}



void update_bike2_Prince(int value) {
    if (cnt == 0 && !accidentTriggered) {
        speed_b2 = 0.01f;

        // Reset position if bike goes off screen
        if (position_b2 < -2.7f)
            position_b2 = 2.7f;

        position_b2 -= speed_b2;

        // Check for collision with bike1
        if (!hasHelmet && abs(position_b1 - position_b2) < 0.1f) {
            accidentTriggered = true;
            glutTimerFunc(30, update_bike_accident_Prince, 0);
        }
    } else {
        // Stop bike2 if cnt != 0
        if (position_b2 < 1.8f) {
            speed_b2 = 0.0f;
            position_b2 = 1.8f;
        }
        position_b2 -= speed_b2;
    }

    glutPostRedisplay();
    glutTimerFunc(10, update_bike2_Prince, 0);
}

void update_bike_accident_Prince(int value) {
    if (!hasHelmet && accidentTriggered) {
        fallOffset -= 0.01f; // rider falls down
        if (fallOffset < -0.3f) {
            fallOffset = -0.3f; // limit fall
        }
        glutPostRedisplay();
        glutTimerFunc(30, update_bike_accident_Prince, 0);
    }
}

void update_bike3_Prince(int value) {
    if (cnt == 0) {
        if(position_b3 > -0.9) { // Changed from position_c3
            speed_b3 = 0.0f;     // Changed from speed_c3
            position_b3 = -0.9;  // Changed from position_c3
        }
        position_b3 += speed_b3; // Changed from position_c3 and speed_c3
    } else {
        speed_b3 = 0.01f;        // Changed from speed_c3
        if(position_b3 > 1.7)    // Changed from position_c3
            position_b3 = -1.7f; // Changed from position_c3

        position_b3 += speed_b3; // Changed from position_c3 and speed_c3
    }

    glutPostRedisplay();
    glutTimerFunc(10, update_bike3_Prince, 0);
}

void update_bike4_Prince(int value) {
    if(cnt == 0) {
        if(position_b4 < 0.9) {
            speed_b4 = 0.0f;
            position_b4 = 0.9;
        }
        position_b4 -= speed_b4;
    }

    else {
        speed_b4 = 0.01f;
        if(position_b4 < -1.7)
            position_b4 = 1.7f;

        position_b4 -= speed_b4;
    }

    glutPostRedisplay();
    glutTimerFunc(10, update_bike4_Prince, 0);
}



 void updatePolice_Prince(int value) {
    if (policeActive) {
        blinkCounter++;
        lightBlink = (blinkCounter % 20 < 10); // Toggle every few frames
        showOverspeeding = true;

        // Optional: play whistle sound (platform-dependent)
        // PlaySound("whistle.wav", NULL, SND_ASYNC); // Windows only
    } else {
        showOverspeeding = false;
    }

    glutPostRedisplay();
    glutTimerFunc(100, updatePolice_Prince, 0);
}

void button_Prince(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            extern void displayMenu();
        extern void mouseClick(int, int, int, int);

        // Reset menu OpenGL state
        glClearColor(0.96f, 0.98f, 1.0f, 1.0f); // Menu background
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Set menu callbacks
        glutDisplayFunc(displayMenu);
        glutMouseFunc(mouseClick);
        glutKeyboardFunc(keyboardMenu);
        glutSpecialFunc(NULL);

        // Force redraw
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers();

            break; // Esc
        case 'r':
            cnt++;  // some counter increment
            break;

        case 'g':
            cnt = 0;  // reset counter
            break;



       case 'h': // toggle helmet
            hasHelmet = !hasHelmet;
            break;

        case 'a': // trigger accident
            accidentTriggered = true;
            glutTimerFunc(30, update_bike_accident_Prince, 0);
            break;

        case 'x': // optional reset
            accidentTriggered = false;
            fallOffset = 0.0f;
            break;


       case 'o':
       case 'O':
       policeActive = !policeActive;
       if (policeActive)
        glutTimerFunc(100, updatePolice_Prince, 0);
        break;



    }
}



void initGL_Prince() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to black and opaque
    gluOrtho2D(-2, 2, -1, 1); // Set range of axis of display (left, right, bottom, top)
}
// Main function: GLUT runs as a console application starting at main()

