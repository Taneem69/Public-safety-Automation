#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
extern void displayMenu();
extern void mouseClick(int button, int state, int x, int y);
extern void keyboardMenu(unsigned char key, int x, int y);

float carPosition = -140.0f; // Initial car position (left side)
float car2Position = 140.0f; // Second car position (right side)
bool collision = false;
bool resetting = false;
float fireSize = 0.0f;
float fireAlpha = 0.0f;
int collisionCount = 0;
int scenario = 0; // 0 = not selected, 1 = "to do", 2 = "not to do"
std::string message = "";

// Cloud structure and variables
struct Cloud {
    float x;
    float y;
    float speed;
    float size;
};

std::vector<Cloud> clouds;
const int NUM_CLOUDS = 5;

void initClouds_Taneem() {
    srand(time(0));
    for (int i = 0; i < NUM_CLOUDS; i++) {
        Cloud cloud;
        cloud.x = rand() % 140 - 70; // Random x position between -70 and 70
        cloud.y = rand() % 30 + 50;  // Random y position between 50 and 80
        cloud.speed = (rand() % 10 + 5) * 0.01f; // Random speed between 0.05 and 0.15
        cloud.size = rand() % 5 + 8; // Random size between 8 and 12
        clouds.push_back(cloud);
    }
}

void updateClouds_Taneem() {
    for (unsigned int i = 0; i < clouds.size(); i++) {
        clouds[i].x += clouds[i].speed;
        // If cloud moves off screen, reset it to the left
        if (clouds[i].x > 90) {
            clouds[i].x = -90;
            clouds[i].y = rand() % 30 + 50;
        }
    }
}

void drawCircle_Taneem(float cx, float cy, float r, int num_segments);
void drawCircle_Taneem(float cx, float cy, float r) {
    drawCircle_Taneem(cx, cy, r, 100);
}

void drawCloud_Taneem(float x, float y, float size) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 0.8f); // White with some transparency

    // Draw a fluffy cloud using multiple circles
    drawCircle_Taneem(x, y, size);
    drawCircle_Taneem(x + size * 0.7f, y + size * 0.3f, size * 0.8f);
    drawCircle_Taneem(x + size * 1.2f, y, size * 0.7f);
    drawCircle_Taneem(x - size * 0.7f, y + size * 0.3f, size * 0.8f);
    drawCircle_Taneem(x - size * 1.2f, y, size * 0.7f);

    glDisable(GL_BLEND);
}

void drawText_Taneem(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    for (unsigned int i = 0; i < text.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawCircle_Taneem(float cx, float cy, float r, int num_segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawTree_Taneem(float x, float y) {
    // Tree trunk
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 1.5f, y);
        glVertex2f(x - 1.5f, y + 10.0f);
        glVertex2f(x + 1.5f, y + 10.0f);
        glVertex2f(x + 1.5f, y);
    glEnd();

    // Tree foliage
    glColor3f(0.0f, 0.5f, 0.0f);
    drawCircle_Taneem(x, y + 15.0f, 5.0f);
    drawCircle_Taneem(x - 3.0f, y + 12.0f, 4.0f);
    drawCircle_Taneem(x + 3.0f, y + 12.0f, 4.0f);
}

void drawBench_Taneem(float x, float y) {
    // Bench seat
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 5.0f, y);
        glVertex2f(x - 5.0f, y + 1.0f);
        glVertex2f(x + 5.0f, y + 1.0f);
        glVertex2f(x + 5.0f, y);
    glEnd();

    // Bench back
    glBegin(GL_POLYGON);
        glVertex2f(x - 5.0f, y + 1.0f);
        glVertex2f(x - 5.0f, y + 3.0f);
        glVertex2f(x + 5.0f, y + 3.0f);
        glVertex2f(x + 5.0f, y + 1.0f);
    glEnd();

    // Bench legs
    glBegin(GL_POLYGON);
        glVertex2f(x - 4.5f, y);
        glVertex2f(x - 4.5f, y - 2.0f);
        glVertex2f(x - 3.5f, y - 2.0f);
        glVertex2f(x - 3.5f, y);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(x + 4.5f, y);
        glVertex2f(x + 4.5f, y - 2.0f);
        glVertex2f(x + 3.5f, y - 2.0f);
        glVertex2f(x + 3.5f, y);
    glEnd();
}

void drawFireHydrant_Taneem(float x, float y) {

}

void drawCar_Taneem() {
    // Car body
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        glVertex2f(18.58,13.71);
        glVertex2f(17.58,3.21);
        glVertex2f(63.45,3.21);
        glVertex2f(62.22,13.81);
    glEnd();

    // Car top
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        glVertex2f(34.03,20.27);
        glVertex2f(31.46,13.81);
        glVertex2f(49.80,13.81);
        glVertex2f(48.38,20.17);
    glEnd();

    // Left window
    glBegin(GL_POLYGON);
        glColor3f(0.7f, 0.9f, 1.0f); // Light blue window
        glVertex2f(34.4261173744973,18.8964706198953);
        glVertex2f(32.5698974707934,13.7561693481);
        glVertex2f(39.9947770856088,13.8989554945388);
        glVertex2f(40.1375632320476,19.1106498395535);
    glEnd();

    // Right window
    glBegin(GL_POLYGON);
        glColor3f(0.7f, 0.9f, 1.0f); // Light blue window
        glVertex2f(41.2798524035577,18.8964706198953);
        glVertex2f(41.2488457974861,13.8534372086272);
        glVertex2f(48.9189112380313,13.8989554945388);
        glVertex2f(47.8480151397406,19.0392567663341);
    glEnd();

    // Window frames
    glLineWidth(2.0);
    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(32.5698974707934,13.7561693481);
        glVertex2f(32.0714543114186,6.7779079788174);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(39.9947770856088,13.8989554945388);
        glVertex2f(40.0507519527142,6.6314988477844);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(40.0507519527142,6.6314988477844);
        glVertex2f(32.0714543114186,6.7779079788174);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(41.2488457974861,13.8534372086272);
        glVertex2f(41.1488204354613,6.7047034133009);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(41.1488204354613,6.7047034133009);
        glVertex2f(47.0783902422956,6.7047034133009);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(47.0783902422956,6.7047034133009);
        glVertex2f(49.0549135112404,9.5596814684434);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(49.0549135112404,9.5596814684434);
        glVertex2f(48.9189112380313,13.8989554945388);
    glEnd();

    // Wheels
    glColor3f(0.0f, 0.0f, 0.0f); // Black wheels
    drawCircle_Taneem(27.6530626854438f,5.3167161137791f,6.2284090867737);

    glColor3f(1.0f, 1.0f, 1.0f); // White hubcap
    drawCircle_Taneem(27.6530626854438f,5.3167161137791f,4.7914728005964f);

    glColor3f(0.0f, 0.0f, 0.0f); // Black center
    drawCircle_Taneem(27.6530626854438f,5.3167161137791f,2.2284090867737);

    glColor3f(0.0f, 0.0f, 0.0f); // Black wheels
    drawCircle_Taneem(52.0038293588435,5.4858186601222,6.2284090867737);

    glColor3f(1.0f, 1.0f, 1.0f); // White hubcap
    drawCircle_Taneem(52.0038293588435,5.4858186601222,4.7914728005964f);

    glColor3f(0.0f, 0.0f, 0.0f); // Black center
    drawCircle_Taneem(52.0038293588435,5.4858186601222,2.2284090867737);

    // Headlights
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow headlight
    drawCircle_Taneem(18.58,11.71,1.9989728842302);

    // Taillight
    glColor3f(1.0f, 1.0f, 1.0f); // White taillight
    drawCircle_Taneem(62.22,11.81,1.9989728842302);
}

void drawCar2_Taneem() {
    // Draw the second car (blue) - flipped horizontally
    glPushMatrix();
    glScalef(-1.0f, 1.0f, 1.0f); // Flip horizontally

    // Car body
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f); // Blue
        glVertex2f(18.58,13.71);
        glVertex2f(17.58,3.21);
        glVertex2f(63.45,3.21);
        glVertex2f(62.22,13.81);
    glEnd();

    // Car top
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f); // Blue
        glVertex2f(34.03,20.27);
        glVertex2f(31.46,13.81);
        glVertex2f(49.80,13.81);
        glVertex2f(48.38,20.17);
    glEnd();

    // Left window
    glBegin(GL_POLYGON);
        glColor3f(0.7f, 0.9f, 1.0f); // Light blue window
        glVertex2f(34.4261173744973,18.8964706198953);
        glVertex2f(32.5698974707934,13.7561693481);
        glVertex2f(39.9947770856088,13.8989554945388);
        glVertex2f(40.1375632320476,19.1106498395535);
    glEnd();

    // Right window
    glBegin(GL_POLYGON);
        glColor3f(0.7f, 0.9f, 1.0f); // Light blue window
        glVertex2f(41.2798524035577,18.8964706198953);
        glVertex2f(41.2488457974861,13.8534372086272);
        glVertex2f(48.9189112380313,13.8989554945388);
        glVertex2f(47.8480151397406,19.0392567663341);
    glEnd();

    // Window frames
    glLineWidth(2.0);
    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(32.5698974707934,13.7561693481);
        glVertex2f(32.0714543114186,6.7779079788174);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(39.9947770856088,13.8989554945388);
        glVertex2f(40.0507519527142,6.6314988477844);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(40.0507519527142,6.6314988477844);
        glVertex2f(32.0714543114186,6.7779079788174);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(41.2488457974861,13.8534372086272);
        glVertex2f(41.1488204354613,6.7047034133009);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(41.1488204354613,6.7047034133009);
        glVertex2f(47.0783902422956,6.7047034133009);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(47.0783902422956,6.7047034133009);
        glVertex2f(49.0549135112404,9.5596814684434);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(49.0549135112404,9.5596814684434);
        glVertex2f(48.9189112380313,13.8989554945388);
    glEnd();

    // Wheels
    glColor3f(0.0f, 0.0f, 0.0f); // Black wheels
    drawCircle_Taneem(27.6530626854438f,5.3167161137791f,6.2284090867737);

    glColor3f(1.0f, 1.0f, 1.0f); // White hubcap
    drawCircle_Taneem(27.6530626854438f,5.3167161137791f,4.7914728005964f);

    glColor3f(0.0f, 0.0f, 0.0f); // Black center
    drawCircle_Taneem(27.6530626854438f,5.3167161137791f,2.2284090867737);

    glColor3f(0.0f, 0.0f, 0.0f); // Black wheels
    drawCircle_Taneem(52.0038293588435,5.4858186601222,6.2284090867737);

    glColor3f(1.0f, 1.0f, 1.0f); // White hubcap
    drawCircle_Taneem(52.0038293588435,5.4858186601222,4.7914728005964f);

    glColor3f(0.0f, 0.0f, 0.0f); // Black center
    drawCircle_Taneem(52.0038293588435,5.4858186601222,2.2284090867737);

    // Headlights (now on the right side after flipping)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow headlight
    drawCircle_Taneem(18.58,11.71,1.9989728842302);

    // Taillight (now on the left side after flipping)
    glColor3f(1.0f, 1.0f, 1.0f); // White taillight
    drawCircle_Taneem(62.22,11.81,1.9989728842302);

    glPopMatrix();
}

void drawFire_Taneem() {
    if (collision && !resetting && scenario == 2) {
        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw fire effect at the front of both cars
        glColor4f(1.0f, 0.5f, 0.0f, fireAlpha); // Orange with alpha
        drawCircle_Taneem(0, 10, fireSize);

        glColor4f(1.0f, 0.0f, 0.0f, fireAlpha * 0.7f); // Red with alpha
        drawCircle_Taneem(0, 12, fireSize * 0.7f);

        glColor4f(1.0f, 1.0f, 0.0f, fireAlpha * 0.5f); // Yellow with alpha
        drawCircle_Taneem(0, 14, fireSize * 0.5f);

        glDisable(GL_BLEND);
    }
}

void drawBuilding_Taneem(float x, float y, float width, float height, int windowsX, int windowsY, std::string name) {
    // Building base
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(x, y);
        glVertex2f(x, y + height);
        glVertex2f(x + width, y + height);
        glVertex2f(x + width, y);
    glEnd();

    // Building outline
    glLineWidth(2.0f);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x, y + height);
        glVertex2f(x + width, y + height);
        glVertex2f(x + width, y);
    glEnd();

    // Windows
    float windowWidth = width / (windowsX * 2 + 1);
    float windowHeight = height / (windowsY * 2 + 1);

    glColor3f(0.8f, 0.9f, 1.0f);
    for (int i = 0; i < windowsX; i++) {
        for (int j = 0; j < windowsY; j++) {
            float winX = x + (i * 2 + 1) * windowWidth;
            float winY = y + (j * 2 + 1) * windowHeight;

            glBegin(GL_POLYGON);
                glVertex2f(winX, winY);
                glVertex2f(winX, winY + windowHeight);
                glVertex2f(winX + windowWidth, winY + windowHeight);
                glVertex2f(winX + windowWidth, winY);
            glEnd();

            // Window frame
            glLineWidth(1.0f);
            glColor3f(0.2f, 0.2f, 0.2f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(winX, winY);
                glVertex2f(winX, winY + windowHeight);
                glVertex2f(winX + windowWidth, winY + windowHeight);
                glVertex2f(winX + windowWidth, winY);
            glEnd();

            glColor3f(0.8f, 0.9f, 1.0f);
        }
    }

    // Building name
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText_Taneem(x + width/2 - 10, y + height + 2, name);
}

void drawRoad_Taneem() {
    // Main road surface
    glBegin(GL_POLYGON);
        glColor3f(0.3f, 0.3f, 0.3f); // Dark gray
        glVertex2f(-70,6);
        glVertex2f(-70,-13.289345378585);
        glVertex2f(70,-13.289345378585);
        glVertex2f(70,6);
    glEnd();

    // Road markings (stationary)
    glColor3f(1.0f, 1.0f, 1.0f); // White markings
    for (int i = -8; i < 8; i++) {
        glBegin(GL_POLYGON);
            glVertex2f(i * 20.0f, -3.5f);
            glVertex2f(i * 20.0f - 8.0f, -3.5f);
            glVertex2f(i * 20.0f - 8.0f, -2.5f);
            glVertex2f(i * 20.0f, -2.5f);
        glEnd();
    }

    // Road shoulders
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.4f, 0.2f); // Brown shoulder
        glVertex2f(-70,6);
        glVertex2f(-70,-13.289345378585);
        glVertex2f(-70,-13.289345378585);
        glVertex2f(-70,6);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.4f, 0.2f); // Brown shoulder
        glVertex2f(70,6);
        glVertex2f(70,-13.289345378585);
        glVertex2f(70,-13.289345378585);
        glVertex2f(70,6);
    glEnd();

    // Road edge lines
    glLineWidth(2.0);
    glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow edge line
        glVertex2f(-70,6);
        glVertex2f(70,6);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow edge line
        glVertex2f(-70,-13.289345378585);
        glVertex2f(70,-13.289345378585);
    glEnd();

    // Footpaths
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_POLYGON);
        glVertex2f(-70, 6);
        glVertex2f(-70, 10);
        glVertex2f(70, 10);
        glVertex2f(70, 6);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(-70, -13.289345378585);
        glVertex2f(-70, -30);
        glVertex2f(70, -30);
        glVertex2f(70, -13.289345378585);
    glEnd();
}

void drawScene_Taneem() {
    // Draw buildings on the right side with names
    drawBuilding_Taneem(40, 10, 25, 40, 4, 6, "BFC");
    drawBuilding_Taneem(10, 10, 25, 35, 4, 5, "KFC");
    drawBuilding_Taneem(-20, 10, 25, 45, 4, 7, "MALL");
    drawBuilding_Taneem(-50, 10, 15, 30, 3, 5, "HOTEL");

    // Draw trees on the right side
    drawTree_Taneem(45, 10);
    drawTree_Taneem(15, 10);
    drawTree_Taneem(-15, 10);
    drawTree_Taneem(-45, 10);

    // Draw benches on the right side
    drawBench_Taneem(35, 10);
    drawBench_Taneem(5, 10);
    drawBench_Taneem(-25, 10);
    drawBench_Taneem(-55, 10);

    // Draw fire hydrants on the right side
    drawFireHydrant_Taneem(50, 10);
    drawFireHydrant_Taneem(20, 10);
    drawFireHydrant_Taneem(-10, 10);
    drawFireHydrant_Taneem(-40, 10);
}

void drawButtons_Taneem() {
    // Draw "To Do" button
    if (scenario == 1) {
        glColor3f(0.0f, 0.8f, 0.0f); // Green when selected
    } else {
        glColor3f(0.7f, 0.7f, 0.7f); // Gray when not selected
    }
    glBegin(GL_POLYGON);
        glVertex2f(-60, -50);
        glVertex2f(-60, -40);
        glVertex2f(-20, -40);
        glVertex2f(-20, -50);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText_Taneem(-47, -47, "TO DO");

    // Draw "Not To Do" button
    if (scenario == 2) {
        glColor3f(0.8f, 0.0f, 0.0f); // Red when selected
    } else {
        glColor3f(0.7f, 0.7f, 0.7f); // Gray when not selected
    }
    glBegin(GL_POLYGON);
        glVertex2f(20, -50);
        glVertex2f(20, -40);
        glVertex2f(60, -40);
        glVertex2f(60, -50);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText_Taneem(30, -46, "NOT TO DO");

    // Draw message
    if (!message.empty()) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText_Taneem(-65, -35, message.c_str());
    }
}

void display_Taneem() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Draw clouds first (in the background)
    for (unsigned int i = 0; i < clouds.size(); i++) {
        drawCloud_Taneem(clouds[i].x, clouds[i].y, clouds[i].size);
    }

    // Draw road and footpaths
    drawRoad_Taneem();

    // Draw buildings, trees, benches, and fire hydrants (all on right side)
    drawScene_Taneem();

    // Draw cars based on scenario
    if (scenario == 1) {
        // "To Do" scenario - cars moving in same direction
        glPushMatrix();
        glTranslatef(carPosition, 0, 0);
        drawCar_Taneem();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(carPosition - 80.0f, 0, 0); // Second car follows first car
        drawCar2_Taneem();
        glPopMatrix();
    } else if (scenario == 2) {
        // "Not To Do" scenario - cars moving in opposite directions
        glPushMatrix();
        glTranslatef(carPosition, 0, 0);
        drawCar_Taneem();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(car2Position, 0, 0);
        drawCar2_Taneem();
        glPopMatrix();

        // Draw fire if collision occurred
        drawFire_Taneem();
    }

    // Draw buttons
    drawButtons_Taneem();

    glutSwapBuffers();
}

void update_Taneem(int value) {
    if (scenario > 0) {
        if (scenario == 1) {
            // "To Do" scenario - cars moving in same direction
            if (!collision && !resetting) {
                carPosition += 0.5f;

                if (carPosition > 140.0f) {
                    // Reset cars to start again
                    carPosition = -140.0f;
                }
            }
        } else if (scenario == 2) {
            // "Not To Do" scenario - cars moving in opposite directions
            if (!collision && !resetting) {
                carPosition += 0.5f;
                car2Position -= 0.5f;

                // Check for collision
                if ((carPosition + 62.22) >= (car2Position - 62.22)) {
                    collision = true;
                    float collisionPoint = (carPosition + car2Position) / 2.0f;
                    carPosition = collisionPoint - 62.22f;
                    car2Position = collisionPoint + 62.22f;
                    collisionCount++;
                }
            }
            else if (collision && !resetting) {
                // Animate fire after collision
                fireSize += 0.5f;
                fireAlpha += 0.02f;
                if (fireAlpha > 0.8f) fireAlpha = 0.8f;
                if (fireSize > 20.0f) {
                    resetting = true;
                }
            }
            else if (resetting) {
                // Gradually fade out fire
                fireAlpha -= 0.02f;
                if (fireAlpha < 0.0f) fireAlpha = 0.0f;

                // Shrink fire
                fireSize -= 0.5f;
                if (fireSize < 0.0f) fireSize = 0.0f;

                // Once fire fully gone, reset cars
                if (fireSize <= 0.0f && fireAlpha <= 0.0f) {
                    resetting = false;
                    collision = false;
                    carPosition = -140.0f;
                    car2Position = 140.0f;
                }
            }
        }
    }

    // Update cloud positions
    updateClouds_Taneem();

    glutPostRedisplay();
    glutTimerFunc(16, update_Taneem, 0);
}

void mouse_Taneem(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Get current window dimensions
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // Convert mouse coordinates to OpenGL coordinates
        float glX = (x / (float)windowWidth) * 140 - 70;
        float glY = 70 - (y / (float)windowHeight) * 140;

        // Check if "To Do" button is clicked
        if (glX >= -60 && glX <= -20 && glY >= -50 && glY <= -40) {
            scenario = 1;
            message = "Drive in one direction to avoid accident";
            // Reset car positions
            carPosition = -140.0f;
            car2Position = 140.0f;
            collision = false;
            resetting = false;
            fireSize = 0.0f;
            fireAlpha = 0.0f;
        }
        // Check if "Not To Do" button is clicked
        else if (glX >= 20 && glX <= 60 && glY >= -50 && glY <= -40) {
            scenario = 2;
            message = "Driving in opposite direction results in accident";
            // Reset car positions
            carPosition = -140.0f;
            car2Position = 140.0f;
            collision = false;
            resetting = false;
            fireSize = 0.0f;
            fireAlpha = 0.0f;
        }
    }
}
void keyboard_Taneem(unsigned char key, int, int) {
    if (key == 27) { // ESC key
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
    }
}




void initGL_Taneem() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Sky blue background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-70,70,-70,70); // 2D Orthographic projection
    glMatrixMode(GL_MODELVIEW);
}

