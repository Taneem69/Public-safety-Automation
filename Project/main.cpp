#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

int windowWidth = 1280;
int windowHeight = 720;

// ---------- Forward declarations ----------
void displayMenu();
void mouseClick(int button, int state, int x, int y);

// ---------- Each member's project declarations ----------
void initGL_Reshoan();
void display_Reshoan();
void specialKeys_Reshoan(int key, int x, int y);
void timer_Reshoan(int value);
void keyboard_Reshoan(unsigned char key, int x, int y);
void reshape_Reshoan(int w, int h);

void display_Taneem();
void update_Taneem(int value);
void mouse_Taneem(int button, int state, int x, int y);
void initGL_Taneem();
void keyboard_Taneem(unsigned char key, int x, int y);

void day_Prince();
void update_bike1_Prince(int value);
void update_bike2_Prince(int value);
void update_bike3_Prince(int value);
void update_bike4_Prince(int value);
void button_Prince(unsigned char key, int x, int y);
void initGL_Prince();

void initGL_Tutul();
void display_Tutul();
void spe_key(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void my_keyboard(unsigned char key, int x, int y);

int currentProject = 0; // 0 = menu, 1–4 = project index

// ---------- Sign Configuration Structure ----------
struct SignConfig {
    float width;           // Base width of the sign
    float height;          // Base height of the sign
    float borderWidth;     // Border thickness
    float shadowOffset;    // Shadow displacement
    float rodThickness;    // Connecting rod thickness
    float badgeSize;       // Size of project badge
};

// Easy-to-modify sign dimensions
SignConfig signConfig = {
    0.8f,      // width
    0.25f,     // height
    0.03f,     // borderWidth
    0.015f,    // shadowOffset
    3.0f,      // rodThickness
    0.1f       // badgeSize
};

// ---------- Enhanced Button struct ----------
struct Button {
    float x, y;           // Position (top-left corner)
    const char* label;
    int id;
    // Dimensions will be taken from signConfig
};

// Positions arranged in a professional layout
Button buttons[4] = {
    {-0.95f,  0.2f, "Rickshaw on the wrong lane", 1},
    { 0.15f,  0.2f, "Wrong side car driving", 2},
    {-0.95f, -0.3f, "Traffic Light Compliance Simulation", 3},
    { 0.15f, -0.3f, "Bus Stop", 4}
};

// ---------- Draw connecting rod from pole to sign ----------
void drawConnectingRod(float signX, float signY) {
    // Determine which side of the pole the sign is on
    bool isLeft = signX < 0;

    // Calculate connection points using config values
    float poleEdgeX = isLeft ? -0.05f : 0.05f; // Edge of the pole
    float signEdgeX = isLeft ? signX + signConfig.width : signX; // Edge of the sign

    // Rod connection points
    float poleConnectY = signY - signConfig.height/2; // Connect to middle of sign vertically
    float signConnectY = signY - signConfig.height/2;

    // Draw the rod with metallic appearance
    glColor3f(0.4f, 0.4f, 0.4f);
    glLineWidth(signConfig.rodThickness);
    glBegin(GL_LINES);
    glVertex2f(poleEdgeX, poleConnectY);
    glVertex2f(signEdgeX, signConnectY);
    glEnd();

    // Draw connection joints
    glColor3f(0.3f, 0.3f, 0.3f);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(poleEdgeX, poleConnectY);
    glVertex2f(signEdgeX, signConnectY);
    glEnd();
}

// ---------- Draw rectangular traffic sign with 3D effect ----------
// ---------- Draw rectangular traffic sign with 3D effect ----------
void drawRectangularSign(const Button& b) {
    // Use config values for dimensions
    float width = signConfig.width;
    float height = signConfig.height;

    // Calculate the four corners of the rectangle
    float left = b.x;
    float right = b.x + width;
    float top = b.y;
    float bottom = b.y - height;

    // Draw connecting rod first (so it appears behind the sign)
    drawConnectingRod(b.x, b.y);

    // Shadow effect for depth - offset slightly from main shape
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(left - signConfig.shadowOffset, top - signConfig.shadowOffset);
    glVertex2f(right - signConfig.shadowOffset, top - signConfig.shadowOffset);
    glVertex2f(right - signConfig.shadowOffset, bottom - signConfig.shadowOffset);
    glVertex2f(left - signConfig.shadowOffset, bottom - signConfig.shadowOffset);
    glEnd();

    // White reflective border
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(left - signConfig.borderWidth, top + signConfig.borderWidth);
    glVertex2f(right + signConfig.borderWidth, top + signConfig.borderWidth);
    glVertex2f(right + signConfig.borderWidth, bottom - signConfig.borderWidth);
    glVertex2f(left - signConfig.borderWidth, bottom - signConfig.borderWidth);
    glEnd();

    // Gradient green background for realistic appearance
    glBegin(GL_QUADS);
    // Top - lighter green
    glColor3f(0.1f, 0.6f, 0.1f);
    glVertex2f(left, top);
    glVertex2f(right, top);
    // Bottom - darker green
    glColor3f(0.0f, 0.4f, 0.0f);
    glVertex2f(right, bottom);
    glVertex2f(left, bottom);
    glEnd();

    // Sign text with larger font and adjusted positioning
    glColor3f(1, 1, 1);
    glRasterPos2f(b.x + width / 4.0f, b.y - height / 1.7f);  // Adjusted position for larger text
    for (const char* c = b.label; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);  // Larger 24px font

    // Project number badge
    float badgeX, badgeY;
    bool isLeft = b.x < 0;
    if (isLeft) {
        // For left signs, put badge on left side
        badgeX = b.x + width * 0.1f;
    } else {
        // For right signs, put badge on right side
        badgeX = b.x + width * 0.7f;
    }
    badgeY = b.y - height * 0.15f;

    // Draw red badge square
    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(badgeX, badgeY);
    glVertex2f(badgeX + signConfig.badgeSize, badgeY);
    glVertex2f(badgeX + signConfig.badgeSize, badgeY - signConfig.badgeSize);
    glVertex2f(badgeX, badgeY - signConfig.badgeSize);
    glEnd();

    // Badge number text
    glColor3f(1, 1, 1);
    glRasterPos2f(badgeX + signConfig.badgeSize/3, badgeY - signConfig.badgeSize/1.5f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '0' + b.id);
}
// ---------- Draw realistic metallic pole with details ----------
void drawPole() {
    // Pole shadow
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-0.02f, -1.0f);
    glVertex2f(0.08f, -1.0f);
    glVertex2f(0.08f, 1.0f);
    glVertex2f(-0.02f, 1.0f);
    glEnd();

    // Main pole with metallic gradient
    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.7f, 0.7f); // Light at top
    glVertex2f(-0.05f, 1.0f);
    glVertex2f(0.05f, 1.0f);
    glColor3f(0.4f, 0.4f, 0.4f); // Darker at bottom
    glVertex2f(0.05f, -1.0f);
    glVertex2f(-0.05f, -1.0f);
    glEnd();

    // Pole details - joints/segments
    glColor3f(0.3f, 0.3f, 0.3f);
    for (float y = -0.8f; y < 0.8f; y += 0.4f) {
        glBegin(GL_QUADS);
        glVertex2f(-0.07f, y);
        glVertex2f(0.07f, y);
        glVertex2f(0.07f, y + 0.02f);
        glVertex2f(-0.07f, y + 0.02f);
        glEnd();
    }
}

// ---------- Draw professional heading with 3D effect ----------
void drawHeading() {
    // Main sign support
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-0.35f, 0.65f);
    glVertex2f(0.35f, 0.65f);
    glVertex2f(0.35f, 0.95f);
    glVertex2f(-0.35f, 0.95f);
    glEnd();

    // Yellow diamond sign with border
    glPushMatrix();
    glTranslatef(0.0f, 0.8f, 0.0f);

    // Black border
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; i++) {
        float angle = i * M_PI / 2;
        glVertex2f(0.3f * cos(angle + M_PI/4), 0.3f * sin(angle + M_PI/4));
    }
    glEnd();

    // Yellow diamond
    glColor3f(1.0f, 0.9f, 0.0f);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; i++) {
        float angle = i * M_PI / 2;
        glVertex2f(0.28f * cos(angle + M_PI/4), 0.28f * sin(angle + M_PI/4));
    }
    glEnd();
    glPopMatrix();

    // Text with shadow effect
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(-0.05f, 0.77f);
    const char* title = "Traffic PSA";
    for (const char* c = title; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

// ---------- Draw background with subtle road elements ----------
void drawBackground() {
    // Sky gradient
    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.9f, 1.0f); // Light blue at top
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glColor3f(0.96f, 0.98f, 1.0f); // Very light blue at bottom
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
}

// ---------- Enhanced Menu display ----------
void displayMenu() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();  // Professional background
    drawPole();        // Realistic central post
    drawHeading();     // Enhanced heading sign

    for (int i = 0; i < 4; i++)
        drawRectangularSign(buttons[i]); // Draw rectangular signs with rods

    // Instructions at bottom
    glColor3f(0.3f, 0.3f, 0.3f);
    glRasterPos2f(-0.25f, -0.9f);
    const char* instruction = "Click on any project to view | ESC to exit";
    for (const char* c = instruction; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);

    glutSwapBuffers();
}

// ---------- Mouse click handler (unchanged functionality) ----------
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float wx = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
        float wy = 1 - (float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2;

        for (int i = 0; i < 4; i++) {
            Button& b = buttons[i];
            // Simple bounding box check using config dimensions
            if (wx > b.x && wx < b.x + signConfig.width &&
                wy < b.y && wy > b.y - signConfig.height) {
                currentProject = i + 1;
                cout << "Launching project " << currentProject << endl;

                switch (currentProject) {
                    case 1:
                        initGL_Reshoan();
                        glutDisplayFunc(display_Reshoan);
                        reshape_Reshoan(windowWidth, windowHeight);
                        glutSpecialFunc(specialKeys_Reshoan);
                        glutKeyboardFunc(keyboard_Reshoan);
                        glutTimerFunc(16, timer_Reshoan, 0);
                        break;

                    case 2:
                        initGL_Taneem();
                        glutDisplayFunc(display_Taneem);
                        glutKeyboardFunc(keyboard_Taneem);
                        glutMouseFunc(mouse_Taneem);
                        glutTimerFunc(16, update_Taneem, 0);
                        break;

                    case 3:
                        glutDisplayFunc(day_Prince);
                        initGL_Prince();
                        glutTimerFunc(10, update_bike1_Prince, 0);
                        glutTimerFunc(10, update_bike2_Prince, 0);
                        glutTimerFunc(10, update_bike3_Prince, 0);
                        glutTimerFunc(10, update_bike4_Prince, 0);
                        glutKeyboardFunc(button_Prince);
                        glutPostRedisplay();
                        break;

                    case 4:
                        initGL_Tutul();
                        glutDisplayFunc(display_Tutul);
                        glutSpecialFunc(spe_key);
                        glutKeyboardFunc(my_keyboard);
                        glutMouseFunc(mouse);
                        break;
                }
                break;
            }
        }
    }
}

void keyboardMenu(unsigned char key, int, int) {
    if (key == 27) {   // ESC key
        exit(0);
    }
}

// ---------- Enhanced Menu init ----------
void initGL_Menu() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ---------- Entry point ----------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Traffic Public Service Announcement");
    glutFullScreen();          // ← makes it full screen immediately

    initGL_Menu();
    glutDisplayFunc(displayMenu);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboardMenu);

    glutMainLoop();
    return 0;
}
