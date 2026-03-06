// Modified main.cpp - adds car, shared mouse resizing, separate controls & jam logic
#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#ifdef _WIN32
#endif
#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>
using namespace std;
extern void displayMenu();
extern void mouseClick(int button, int state, int x, int y);
extern void keyboardMenu(unsigned char key, int x, int y);


// ---- World settings ----
static float worldHalfW = 640.0f;   // width half
static float worldHalfH = 360.0f;   // height half
static float chainOffset = 0.0f; // normalized [0,1) along the chain path
static float wR = 1.125f;

// Two lanes: evenly divided
static float laneY[2] = { 0.0f, -240.0f }; // [0]=slow, [1]=fast

bool laneChangeAllowed = true;


// ---- Rickshaw state ----
float rickshawScale = 55.0f;  // initial scale (visual)
static int rickshawLane = 0;
static float rickshawX = -250.0f;
static float rickshawY = laneY[0];
static float rickshawTargetY = laneY[0];
static float rickshawWheelAngle = 0.0f;
static float rickshawSpeed = 40.0f; // units per second (slow)

// ---- Car state ----
float carScale = 4000.0f;
static int carLane = 1;
static float carX = 50.0f;
static float carY = laneY[1];
static float carTargetY = laneY[1];
static float carWheelAngle = 0.0f;
static float carSpeed = 280.0f; // units per second (fast)

// ---- Global animation state ----
static bool  paused = false;

// Utility: clamp
template<typename T>
T clamp(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }

// --------- Simple drawing helpers ----------
void drawCircle(float cx, float cy, float r, int segments = 48, bool filled = true) {
    float step = 2.0f * 3.1415926535f / segments;
    if (filled) glBegin(GL_TRIANGLE_FAN);
    else        glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float a = i * step;
        glVertex2f(cx + r * std::cos(a), cy + r * std::sin(a));
    }
    glEnd();
}
// Draws a semicircular or partial mudguard with thickness and color
void drawMudguard(float cx, float cy, float innerR, float outerR,
                  float startDeg, float endDeg,
                  float rColor, float gColor, float bColor,
                  int segments = 48) {
    float startRad = startDeg * 3.1415926535f / 180.0f;
    float endRad   = endDeg   * 3.1415926535f / 180.0f;
    float step = (endRad - startRad) / segments;

    glColor3f(rColor, gColor, bColor);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float a = startRad + i * step;
        glVertex2f(cx + outerR * cos(a), cy + outerR * sin(a)); // outer edge
        glVertex2f(cx + innerR * cos(a), cy + innerR * sin(a)); // inner edge
    }
    glEnd();
}

void geoPoint(const string &geoStr) {
    float x, y;

    // Find the '(' and ')'
    size_t start = geoStr.find('(');
    size_t end   = geoStr.find(')');

    if (start != string::npos && end != string::npos && end > start) {
        string coords = geoStr.substr(start + 1, end - start - 1); // extract "x,y"

        // Replace comma with space (old-style loop for pre-C++11)
        for (size_t i = 0; i < coords.size(); i++) {
            if (coords[i] == ',') coords[i] = ' ';
        }

        stringstream ss(coords);
        if (ss >> x >> y) {
            glVertex2f(x, y);
        } else {
            cerr << "Error: Could not parse numbers from -> " << geoStr << endl;
        }
    } else {
        cerr << "Error: Invalid format -> " << geoStr << endl;
    }
}



void drawPedalSystem(float pivotX, float pivotY, float crankLength, float crankAngleDeg) {
    // ===== PEDAL END POSITION =====
    float rad = crankAngleDeg * 3.1415926535f / 180.0f;
    float pedalX = pivotX + cos(rad) * crankLength;
    float pedalY = pivotY + sin(rad) * crankLength;

    // ---- Crank ----
    glLineWidth(4.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
        glVertex2f(pivotX, pivotY);
        glVertex2f(pedalX, pedalY);
    glEnd();

    // ---- Pedal ----
    float pedalW = 0.6f, pedalH = 0.08f;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(pedalX - pedalW/2, pedalY - pedalH/2);
        glVertex2f(pedalX + pedalW/2, pedalY - pedalH/2);
        glVertex2f(pedalX + pedalW/2, pedalY + pedalH/2);
        glVertex2f(pedalX - pedalW/2, pedalY + pedalH/2);
    glEnd();

    // ===== LEG SYSTEM =====
    float hipX = 0.24682f, hipY = 2.18015f;  // fixed hip
    float thighLen = 1.4f, legLen = 1.1f;

    float ankleX = pedalX, ankleY = pedalY;

    // --- IK solve for knee ---
    float dx = ankleX - hipX;
    float dy = ankleY - hipY;
    float dist = sqrt(dx*dx + dy*dy);

    if (dist > thighLen + legLen) dist = thighLen + legLen;
    if (dist < fabs(thighLen - legLen)) dist = fabs(thighLen - legLen);

    float a = acosf((thighLen*thighLen + dist*dist - legLen*legLen) / (2*thighLen*dist));
    float b = atan2f(dy, dx);

    // Flip so knee bends correctly
    float kneeX = hipX + thighLen * cos(b + a);
    float kneeY = hipY + thighLen * sin(b + a);


    // =========================
    // DRAW LEG (anchored knee → ankle)
    // =========================
    struct Vec2 { float x, y; };
    Vec2 legPoints[6] = {
        {1.28789f,-0.22081f}, // foot-ankle
        {1.37f,-0.22f},       // ankle pivot
        {1.44331f,-0.21294f},
        {1.21248f,0.92661f},
        {1.08f,0.88f},        // knee pivot
        {0.94934f,0.83308f}
    };
    Vec2 kneePivot = legPoints[4];       // P
    Vec2 anklePivotOrig = legPoints[1];  // R

    float legOrigDist = sqrt((anklePivotOrig.x - kneePivot.x)*(anklePivotOrig.x - kneePivot.x) +
                             (anklePivotOrig.y - kneePivot.y)*(anklePivotOrig.y - kneePivot.y));
    float legCurrDist = sqrt((ankleX - kneeX)*(ankleX - kneeX) + (ankleY - kneeY)*(ankleY - kneeY));
    float legAngle = atan2f(ankleY - kneeY, ankleX - kneeX); // direction from knee → ankle
    float origAngle = atan2f(anklePivotOrig.y - kneePivot.y, anklePivotOrig.x - kneePivot.x);

    glBegin(GL_POLYGON);
    glColor3f(0.8f, 0.5f, 0.2f);
    for (int i = 0; i < 6; i++) {
        Vec2 v = legPoints[i];
        if (i == 4) glVertex2f(kneeX, kneeY);       // knee moves
        else if (i == 1) glVertex2f(ankleX, ankleY); // ankle moves
        else {
            float relX = v.x - kneePivot.x;
            float relY = v.y - kneePivot.y;

            // rotate polygon according to angle change
            float newX = cos(legAngle - origAngle) * relX - sin(legAngle - origAngle) * relY;
            float newY = sin(legAngle - origAngle) * relX + cos(legAngle - origAngle) * relY;

            // scale to match current knee→ankle distance
            newX *= legCurrDist / legOrigDist;
            newY *= legCurrDist / legOrigDist;

            // translate to current knee
            newX += kneeX;
            newY += kneeY;

            glVertex2f(newX, newY);
        }
    }
    glEnd();
    // =========================
    // DRAW FOOT (exact polygon)
    // =========================
    glPushMatrix();

    // Compute offset from polygon's ankle pivot R to pedal
    float R_x = 1.37f, R_y = -0.22f; // ankle pivot in polygon coordinates
    float ankleOffsetY = 0.215f;      // shift up a bit
    float ankleOffsetX = -0.05f;
    float offsetX = pedalX - R_x + ankleOffsetX;
    float offsetY = pedalY - R_y + ankleOffsetY;

    // Translate entire foot polygon so that R aligns slightly above pedal
    glTranslatef(offsetX, offsetY, 0.0f);

    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("H=$point(1.27312,-0.3271)");
        geoPoint("G=$point(1.30709,-0.40183)");
        geoPoint("C=$point(1.4,-0.4)");
        geoPoint("D=$point(1.8,-0.4)");
        geoPoint("E=$point(1.77924,-0.29993)");
        geoPoint("F=$point(1.44331,-0.21294)");
        geoPoint("R=$point(1.37, -0.22)"); // ankle pivot
        geoPoint("(1.28789, -0.22081)");
    glEnd();
    //sandles
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.0f, 0.0f);
        geoPoint("G=$point(1.2, -0.5)");
        geoPoint("D=$point(1.77437, -0.50079)");
        geoPoint("V_{3}=$point(1.79,-0.33)");;
        geoPoint("H=$point(1.17904, -0.33743)");
    glEnd();
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.0f, 0.0f);
        geoPoint("W_{3}=$point(1.49, -0.35)");
        geoPoint("B_{4}=$point(1.64, -0.35)");
        geoPoint("A_{4}=$point(1.67, -0.27)");
        geoPoint("B_{4}=$point(1.55, -0.24)");
    glEnd();


    glPopMatrix();

    // =========================
    // DRAW THIGH (anchored hip → knee)
    // =========================
    Vec2 thighPoints[6] = {
        {0.94934f,0.83308f}, // K
        {1.08f,0.88f},       // P
        {1.21248f,0.92661f}, // J
        {0.72997f,2.29486f}, // M
        {0.24682f,2.18015f}, // N hip pivot
        {-0.23687f,2.04651f} // L
    };
    Vec2 hipPivot = thighPoints[4];  // N
    Vec2 kneePivotOrig = thighPoints[1]; // P

    float thighOrigDist = sqrt((kneePivotOrig.x - hipPivot.x)*(kneePivotOrig.x - hipPivot.x) +
                               (kneePivotOrig.y - hipPivot.y)*(kneePivotOrig.y - hipPivot.y));
    float thighCurrDist = sqrt((kneeX - hipX)*(kneeX - hipX) + (kneeY - hipY)*(kneeY - hipY));
    float thighAngle = atan2f(kneeY - hipY, kneeX - hipX);
    float thighOrigAngle = atan2f(kneePivotOrig.y - hipPivot.y, kneePivotOrig.x - hipPivot.x);

    glBegin(GL_POLYGON);
    glColor3f(0.8f, 0.5f, 0.2f);
    for (int i = 0; i < 6; i++) {
        Vec2 v = thighPoints[i];
        if (i == 4) glVertex2f(hipX, hipY);      // hip fixed
        else if (i == 1) glVertex2f(kneeX, kneeY); // knee moves
        else {
            float relX = v.x - hipPivot.x;
            float relY = v.y - hipPivot.y;

            float newX = cos(thighAngle - thighOrigAngle) * relX - sin(thighAngle - thighOrigAngle) * relY;
            float newY = sin(thighAngle - thighOrigAngle) * relX + cos(thighAngle - thighOrigAngle) * relY;

            // scale to match current hip→knee distance
            newX *= thighCurrDist / thighOrigDist;
            newY *= thighCurrDist / thighOrigDist;

            // translate to hip
            newX += hipX;
            newY += hipY;

            glVertex2f(newX, newY);
        }
    }
    glEnd();

    // =========================
    // DEBUG CIRCLES (remove later)
    // =========================
    glColor3f(0.8f, 0.5f, 0.2f);
    drawCircle(hipX, hipY, 0.05f, 16, true);   // Hip
    drawCircle(kneeX, kneeY, 0.15f, 16, true); // Knee
    glColor3f(0.5f, 0.0f, 0.0f);
    drawCircle(ankleX, ankleY, 0.05f, 16, true); // Ankle
}

void drawChainDotsContinuous(float rearX, float rearY, float rearR,
                             float gearX, float gearY, float gearR,
                             int numDots)
{
    glPointSize(4.0f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_POINTS);

    float rearArcLen    = 3.14159265f * rearR; // top half
    float gearArcLen    = 3.14159265f * gearR; // top half
    float topLineLen    = sqrt((gearX - gearR - (rearX + rearR))*(gearX - gearR - (rearX + rearR)) +
                               (gearY + gearR - rearY)*(gearY + gearR - rearY));
    float bottomLineLen = sqrt((gearX + gearR - (rearX - rearR))*(gearX + gearR - (rearX - rearR)) +
                               (gearY - gearR - rearY)*(gearY - rearR - rearY));

    float totalLen = rearArcLen + topLineLen + gearArcLen + bottomLineLen;

    for (int i = 0; i < numDots; ++i) {
        float t = fmod((i / (float)numDots) + chainOffset, 1.0f); // use chainOffset from timer
        float dist = t * totalLen;
        float x, y;

        if (dist < rearArcLen) {
            float angle = 3.14159265f/15.0f - (dist / rearArcLen) * 3.14159265f;
            x = rearX + cos(angle) * rearR;
            y = rearY + sin(angle) * rearR;
        }
        else if (dist < rearArcLen + topLineLen) {
            float lineT = (dist - rearArcLen) / topLineLen;
            x = rearX + rearR + (gearX - gearR - (rearX + rearR)) * lineT;
            y = rearY + (gearY + gearR - rearY) * lineT;
        }
        else if (dist < rearArcLen + topLineLen + gearArcLen) {
            float angle = 3.14159265f/2.0f - ((dist - rearArcLen - topLineLen) / gearArcLen) * 3.14159265f;
            x = gearX + cos(angle) * gearR;
            y = gearY + sin(angle) * gearR;
        }
        else {
            float lineT = (dist - rearArcLen - topLineLen - gearArcLen) / bottomLineLen;
            x = gearX - gearR + ((rearX - rearR) - (gearX - gearR)) * lineT;
            y = gearY - gearR + ((rearY - rearR) - (gearY - gearR)) * lineT;
        }

        glVertex2f(x, y);
    }

    glEnd();
}





void drawWheel(float x, float y, float r, int spokes, float angleDeg,
               bool fillTire, float tireR, float tireG, float tireB) {
    // ----- Tire -----
    glColor3f(tireR, tireG, tireB);

    if (fillTire) {
        // Filled donut from outer radius r to inner radius (0.85r)
        glBegin(GL_TRIANGLE_STRIP);
        int segments = 64;
        for (int i = 0; i <= segments; ++i) {
            float a = 2.0f * 3.1415926535f * i / segments;
            float cx = cos(a), cy = sin(a);

            glVertex2f(x + cx * r,        y + cy * r);        // outer edge
            glVertex2f(x + cx * (r*0.85), y + cy * (r*0.85)); // inner edge
        }
        glEnd();
    } else {
        // Just outline tire
        glLineWidth(3.0f);
        drawCircle(x, y, r, 64, false);
    }

    // ----- Rim -----
    glLineWidth(1.5f);
    glColor3f(0.25f, 0.25f, 0.25f);
    drawCircle(x, y, r * 0.85f, 64, false);

    // ----- Hub -----
    glColor3f(0.6f, 0.6f, 0.6f);
    drawCircle(x, y, r * 0.2f, 24, true);

    // ----- Spokes -----
    glColor3f(0.75f, 0.75f, 0.75f);
    glBegin(GL_LINES);
    float step = 360.0f / spokes;
    for (int i = 0; i < spokes; ++i) {
        float a = (angleDeg + i * step) * 3.1415926535f / 180.0f;
        float sx = x + cos(a) * (r * 0.07f);
        float sy = y + sin(a) * (r * 0.07f);
        float ex = x + cos(a) * (r * 0.85f);
        float ey = y + sin(a) * (r * 0.85f);
        glVertex2f(sx, sy);
        glVertex2f(ex, ey);
    }
    glEnd();
}
// Draw a car-style wheel (thicker tire, simple rim + few spokes)
void drawCarWheel(float x, float y, float r, float angleDeg) {
    // ----- Tire -----
    glColor3f(0.05f, 0.05f, 0.05f); // black tire
    glBegin(GL_TRIANGLE_STRIP);
    int segments = 64;
    for (int i = 0; i <= segments; ++i) {
        float a = 2.0f * 3.1415926535f * i / segments;
        float cx = cos(a), cy = sin(a);

        glVertex2f(x + cx * r,        y + cy * r);        // outer edge
        glVertex2f(x + cx * (r*0.65), y + cy * (r*0.65)); // inner edge
    }
    glEnd();

    // ----- Rim -----
    glColor3f(0.6f, 0.6f, 0.6f); // silver-gray
    drawCircle(x, y, r*0.65f, 48, true);

    // ----- Hub -----
    glColor3f(0.3f, 0.3f, 0.3f); // dark gray
    drawCircle(x, y, r*0.2f, 24, true);

    // ----- Spokes (4 or 5 thick bars) -----
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(4.0f);
    int spokes = 40;
    float step = 360.0f / spokes;
    glBegin(GL_LINES);
    for (int i = 0; i < spokes; ++i) {
        float a = (angleDeg + i * step) * 3.1415926535f / 180.0f;
        float sx = x + cos(a) * (r*0.2f);
        float sy = y + sin(a) * (r*0.2f);
        float ex = x + cos(a) * (r*0.65f);
        float ey = y + sin(a) * (r*0.65f);
        glVertex2f(sx, sy);
        glVertex2f(ex, ey);
    }
    glEnd();
}



// ---- Car Wheels + Mudguards ----
void drawCarWheelsAndGuards() {
    float wheelR = 0.75f;

    // Left car wheel
    drawCarWheel(-2.78921f, 0.12918f, wheelR, carWheelAngle);

    // Right car wheel
    drawCarWheel(3.19633f, 0.13206f, wheelR, carWheelAngle);

 // Mudguards: semicircles above wheels
    float innerR = wheelR;
    float outerR = wheelR * 1.175f;

    drawMudguard(-2.78921f, 0.12918f,
                 innerR, outerR,
                 -165.0f, -375.0f,    // top half
                 0.0f, 0.0f, 0.0f); // black

    drawMudguard(3.19633f, 0.13206f,
                 innerR, outerR,
                 -165.0f, -370.0f,    // top half
                 0.0f, 0.0f, 0.0f); // black;

}


// ---------- Rickshaw ----------
void drawRickshaw(float x, float y, float scale, float wheelSpinDeg) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);


    drawWheel(3.54232, 0.04381, wR, 14, wheelSpinDeg, true, 0.0f, 0.0f, 0.0f);   // right rear

    // Front wheel mudguard, shorter length
    drawMudguard(3.54232, 0.04381, wR, wR + 0.15f, 180.0f, 90.0f, 0.25f, 0.41f, 0.88f, 32);

    //back
    drawPedalSystem(1.1454,  -0.09165, 0.5f, rickshawWheelAngle + 180.0f);

    //BAR-1
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("K=$point(-2.44521,2.74716)");
        geoPoint("Z=$point(-1.95497,1.60414)");
        geoPoint("E_{1}=$point(-1.92917,1.74347)");
        geoPoint("B_{1}=$point(-2.34974,2.7678)");
    glEnd();
    //BAR-2
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("E_{1}=$point(-1.92917,1.74347)");
        geoPoint("F_{1}=$point(-1.83112,1.85958)");
        geoPoint("C_{1}=$point(-1.74598,2.82715)");
        geoPoint("M=$point(-1.8337,2.83747)");
    glEnd();
    //BAR-3
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("F_{1}=$point(-1.83112,1.85958)");
        geoPoint("A_{1}=$point(-1.84661,1.5964)");
        geoPoint("Q=$point(-1.38475,2.68266)");
        geoPoint("D_{1}=$point(-1.45958,2.73942)");
    glEnd();
    //BAR-Joint
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("Z=$point(-1.95497,1.60414)");
        geoPoint("A_{1}=$point(-1.84661,1.5964)");
        geoPoint("F_{1}=$point(-1.83112,1.85958)");
        geoPoint("E_{1}=$point(-1.92917,1.74347)");
    glEnd();

    //seat
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.0f, 0.5f);
        geoPoint("D=$point(-2.87873,1.49092)");
        geoPoint("P_{1}=$point(-1.74689,1.3231)");
        geoPoint("O_{1}=$point(-1.74689,1.45244)");
        geoPoint("N_{1}=$point(-1.75481,1.51842)");
        geoPoint("M_{1}=$point(-1.79176,1.56857)");
        geoPoint("A_{1}=$point(-1.84661,1.5964)");
        geoPoint("L_{1}=$point(-1.90526,1.61345)");
        geoPoint("Z=$point(-1.95497,1.60414)");
        geoPoint("K_{1}=$point(-2.72615,1.61345)");
        geoPoint("J_{1}=$point(-2.72861,2.4745)");
        geoPoint("I_{1}=$point(-2.7339,2.56704)");
        geoPoint("H_{1}=$point(-2.76827,2.60934)");
        geoPoint("G_{1}=$point(-2.82908,2.649)");
        geoPoint("E=$point(-2.89465,2.53413)");
    glEnd();
    //BAR-Joint
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("Z=$point(-1.95497,1.60414)");
        geoPoint("A_{1}=$point(-1.84661,1.5964)");
        geoPoint("F_{1}=$point(-1.83112,1.85958)");
        geoPoint("E_{1}=$point(-1.92917,1.74347)");
    glEnd();
    //HOOD-1
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("C=$point(-3.07781,1.4511)");
        geoPoint("D=$point(-2.87873,1.49092)");
        geoPoint("E=$point(-2.89465,2.53413)");
        geoPoint("F=$point(-3.00614,4.31795)");
        geoPoint("G=$point(-3.49191,4.07904)");
        geoPoint("H=$point(-4.08121,3.9357)");
        geoPoint("I=$point(-3.46006,2.70933)");
    glEnd();
    //HOOD-2
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("E=$point(-2.89465,2.53413)");
        geoPoint("J=$point(-2.6876,2.73322)");
        geoPoint("K=$point(-2.44521,2.74716)");
        geoPoint("F=$point(-3.00614,4.31795)");
    glEnd();
    //HOOD-3
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("K=$point(-2.44521,2.74716)");
        geoPoint("L=$point(-2.09831,2.89249)");
        geoPoint("M=$point(-1.8337,2.83747)");
        geoPoint("N=$point(-1.69217,3.66494)");
        geoPoint("O=$point(-1.80366,4.50907)");
        geoPoint("P=$point(-2.38499,4.35776)");
        geoPoint("F=$point(-3.00614,4.31795)");
    glEnd();

    //HOOD-4

    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("M=$point(-1.8337,2.83747)");
        geoPoint("Q=$point(-1.38475,2.68266)");
        geoPoint("R=$point(-1.07102,3.03583)");
        geoPoint("S=$point(-0.81619,3.47382)");
        geoPoint("T=$point(-0.6171,4.09497)");
        geoPoint("U=$point(-0.60118,4.23831)");
        geoPoint("V=$point(-0.97546,4.2622)");
        geoPoint("W=$point(-1.46123,4.37369)");
        geoPoint("O=$point(-1.80366,4.50907)");
        geoPoint("N=$point(-1.69217,3.66494)");
    glEnd();
    //hood line
    glBegin(GL_LINE_STRIP);
        glColor3f(0.8f, 0.4f, 0.0f); // dark orange
        geoPoint("T_{1}=$point(-2.99822,4.32949)");
        geoPoint("U_{1}=$point(-2.68525,3.78553)");
        geoPoint("V_{1}=$point(-2.58093,3.49492)");
        geoPoint("W_{1}=$point(-2.43189,3.04037)");
        geoPoint("Z_{1}=$point(-2.40954,2.73485)");
    glEnd();
    glBegin(GL_LINE_STRIP);
        glColor3f(0.8f, 0.4f, 0.0f); // dark orange
        geoPoint("A_{2}=$point(-1.80596,4.49343)");
        geoPoint("B_{2}=$point(-1.7389,4.26243)");
        geoPoint("C_{2}=$point(-1.70164,3.94946)");
        geoPoint("D_{2}=$point(-1.68674,3.65885)");
        geoPoint("E_{2}=$point(-1.70164,3.4204)");
        geoPoint("Q_{2}=$point(-1.7538,3.09998)");
        geoPoint("R_{2}=$point(-1.79106,2.84663)");
    glEnd();


    //support bar 1_1
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("R_{1}=$point(-2.93359,1.12109)");
        geoPoint("U_{1}=$point(-2.93681,0.65605)");
        geoPoint("T_{1}=$point(-2.90286,0.65443)");
        geoPoint("S_{1}=$point(-2.9005,1.12661)");
    glEnd();
    //support bar 1_2
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("C_{2}=$point(-2.16841,0.49957)");
        geoPoint("D_{2}=$point(-2.16488,1.00308)");
        geoPoint("E_{2}=$point(-2.20693,1.02235)");
        geoPoint("F_{2}=$point(-2.2,0.5)");
    glEnd();
    //support bar 1_3
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("U_{1}=$point(-2.93681,0.65605)");
        geoPoint("V_{1}=$point(-2.97077,0.63503)");
        geoPoint("W_{1}=$point(-2.96915,0.57844)");
        geoPoint("Z_{1}=$point(-2.92388,0.54448)");
        geoPoint("A_{2}=$point(-0.85926,0.12923)");
        geoPoint("B_{2}=$point(-0.81538,0.21824)");
        geoPoint("C_{2}=$point(-2.16841,0.49957)");
        geoPoint("F_{2}=$point(-2.2,0.5)");
        geoPoint("T_{1}=$point(-2.90286,0.65443)");
    glEnd();
    //support bar 2_1
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("N_{2}=$point(-2.84647,0.3113)");
        geoPoint("G_{2}=$point(-3.03394,0.30528)");
        geoPoint("H_{2}=$point(-3.08817,0.28325)");
        geoPoint("I_{2}=$point(-3.10342,0.23749)");
        geoPoint("J_{2}=$point(-3.04071,0.17986)");
        geoPoint("K_{2}=$point(-2.93394,0.16969)");
        geoPoint("L_{2}=$point(-0.8,0)");
        geoPoint("M_{2}=$point(-0.78701,0.11988)");
    glEnd();
    //support bar 2_2
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("T_{2}=$point(-0.70479,0.21753)");
        geoPoint("M_{2}=$point(-0.78701,0.11988)");
        geoPoint("L_{2}=$point(-0.8,0)");
        geoPoint("O_{2}=$point(1.07663,-0.13532)");
        geoPoint("P_{2}=$point(1.04037,0.01249)");
        geoPoint("Q_{2}=$point(-0.46495,0.08141)");
        geoPoint("U_{2}=$point(-0.51312,0.11237)");
        geoPoint("R_{2}=$point(-0.54127,0.14857)");
        geoPoint("S_{2}=$point(-0.55438,0.19053)");
    glEnd();
    //support bar 2_3
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("T_{2}=$point(-0.70479,0.21753)");
        geoPoint("S_{2}=$point(-0.55438,0.19053)");
        geoPoint("F_{3}=$point(-0.4,0.4)");
        geoPoint("E_{3}=$point(-0.2,0.6)");
        geoPoint("D_{3}=$point(0.02758,0.79178)");
        geoPoint("C_{3}=$point(0.2868,0.96929)");
        geoPoint("B_{3}=$point(0.58018,1.1136)");
        geoPoint("A_{3}=$point(0.54038,1.22851)");
        geoPoint("Z_{2}=$point(0.22339,1.06054)");
        geoPoint("W_{2}=$point(-0.07104,0.84532)");
        geoPoint("V_{2}=$point(-0.36406,0.59455)");
    glEnd();

    //support bar 2_4
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("O_{2}=$point(1.07663,-0.13532)");
        geoPoint("G_{3}=$point(1.29363,-0.12702)");
        geoPoint("H_{3}=$point(1.5,-0.1)");
        geoPoint("V_{3}=$point(1.45389,0.03558)");
        geoPoint("W_{3}=$point(1.31565,0.01278)");
        geoPoint("C_{4}=$point(1.2,0)");
        geoPoint("P_{2}=$point(1.04037,0.01249)");
    glEnd();
    //support bar 2_5
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("H_{3}=$point(1.5,-0.1)");
        geoPoint("I_{3}=$point(1.61401,-0.01757)");
        geoPoint("K_{3}=$point(1.66774,0.04014)");
        geoPoint("J_{3}=$point(1.73341,0.09387)");
        geoPoint("T_{3}=$point(1.61082,0.16601)");
        geoPoint("U_{3}=$point(1.52115,0.0743)");
        geoPoint("V_{3}=$point(1.45389,0.03558)");
    glEnd();
    //support bar 2_6
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("H_{3}=$point(1.5,-0.1)");
        geoPoint("I_{3}=$point(1.61401,-0.01757)");
        geoPoint("K_{3}=$point(1.66774,0.04014)");
        geoPoint("J_{3}=$point(1.73341,0.09387)");
        geoPoint("L_{3}=$point(1.83887,0.23117)");
        geoPoint("M_{3}=$point(2.31065,0.82264)");
        geoPoint("N_{3}=$point(2.65888,1.24105)");
        geoPoint("Z_{3}=$point(2.69807,1.26775)");
        geoPoint("O_{3}=$point(2.58426,1.4072)");
        geoPoint("P_{3}=$point(2.58498,1.34805)");
        geoPoint("Q_{3}=$point(2.49531,1.22781)");
        geoPoint("R_{3}=$point(2.34246,1.03624)");
        geoPoint("S_{3}=$point(1.82685,0.41873)");
        geoPoint("T_{3}=$point(1.61082,0.16601)");
        geoPoint("U_{3}=$point(1.52115,0.0743)");
        geoPoint("V_{3}=$point(1.45389,0.03558)");
    glEnd();
    //support bar 2_7
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("P_{2}=$point(1.04037,0.01249)");
        geoPoint("C_{4}=$point(1.2,0)");
        geoPoint("B_{4}=$point(0.68916,1.16313)");
        geoPoint("A_{4}=$point(0.64756,1.26972)");
        geoPoint("A_{3}=$point(0.54038,1.22851)");
        geoPoint("B_{3}=$point(0.58018,1.1136)");
    glEnd();
    //support bar 2_8
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("B_{4}=$point(0.68916,1.16313)");
        geoPoint("B_{5}=$point(0.85626,1.20241)");
        geoPoint("L_{4}=$point(1.07787,1.23935)");
        geoPoint("F_{5}=$point(1.05843,1.34626)");
        geoPoint("Z_{4}=$point(0.92819,1.33071)");
        geoPoint("A_{5}=$point(0.8,1.3)");
        geoPoint("A_{4}=$point(0.64756,1.26972)");
    glEnd();
    //support bar 2_9
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("L_{4}=$point(1.07787,1.23935)");
        geoPoint("C_{5}=$point(1.20034,1.25101)");
        geoPoint("W_{4}=$point(1.19062,1.35209)");
        geoPoint("E_{5}=$point(1.34613,1.27239)");
        geoPoint("M_{4}=$point(1.47249,1.27822)");
        geoPoint("N_{4}=$point(1.6,1.3)");
        geoPoint("U_{4}=$point(1.57358,1.40847)");
        geoPoint("V_{4}=$point(1.47638,1.38514)");
        geoPoint("D_{5}=$point(1.33642,1.37153)");
        geoPoint("W_{4}=$point(1.19062,1.35209)");
        geoPoint("F_{5}=$point(1.05843,1.34626)");
    glEnd();
    //support bar 2_10
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("L_{4}=$point(1.07787,1.23935)");
        geoPoint("C_{5}=$point(1.20034,1.25101)");
        geoPoint("W_{4}=$point(1.19062,1.35209)");
        geoPoint("E_{5}=$point(1.34613,1.27239)");
        geoPoint("M_{4}=$point(1.47249,1.27822)");
        geoPoint("N_{4}=$point(1.6,1.3)");
        geoPoint("U_{4}=$point(1.57358,1.40847)");
        geoPoint("V_{4}=$point(1.47638,1.38514)");
        geoPoint("D_{5}=$point(1.33642,1.37153)");
        geoPoint("W_{4}=$point(1.19062,1.35209)");
        geoPoint("F_{5}=$point(1.05843,1.34626)");
    glEnd();
    //support bar 2_11
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("N_{4}=$point(1.6,1.3)");
        geoPoint("O_{4}=$point(1.81074,1.37542)");
        geoPoint("G_{5}=$point(1.97403,1.45124)");
        geoPoint("P_{4}=$point(2.14898,1.50567)");
        geoPoint("R_{4}=$point(2.1,1.6)");
        geoPoint("S_{4}=$point(1.9196,1.53871)");
        geoPoint("T_{4}=$point(1.78158,1.48623)");
        geoPoint("U_{4}=$point(1.57358,1.40847)");
    glEnd();
    //support bar 2_12
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("P_{4}=$point(2.14898,1.50567)");
        geoPoint("Q_{4}=$point(2.25767,1.52279)");
        geoPoint("H_{5}=$point(2.41304,1.54723)");
        geoPoint("E_{4}=$point(2.52326,1.54948)");
        geoPoint("G_{4}=$point(2.53994,1.69622)");
        geoPoint("I_{5}=$point(2.4078,1.65895)");
        geoPoint("F_{4}=$point(2.23673,1.62753)");
        geoPoint("R_{4}=$point(2.1,1.6)");
    glEnd();
    //support bar 2_13
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("E_{4}=$point(2.52326,1.54948)");
        geoPoint("D_{4}=$point(2.56886,1.49294)");
        geoPoint("O_{3}=$point(2.58426,1.4072)");
        geoPoint("P_{3}=$point(2.58498,1.34805)");
        geoPoint("N_{3}=$point(2.65888,1.24105)");
        geoPoint("Z_{3}=$point(2.69807,1.26775)");
        geoPoint("K_{4}=$point(2.73977,1.25939)");
        geoPoint("J_{4}=$point(2.83679,1.28291)");
        geoPoint("I_{4}=$point(2.85069,1.33008)");
        geoPoint("H_{4}=$point(2.72516,1.72636)");
        geoPoint("M_{5}=$point(2.70583,1.81325)");
        geoPoint("L_{5}=$point(2.6431,1.85002)");
        geoPoint("K_{5}=$point(2.55498,1.8326)");
        geoPoint("J_{5}=$point(2.53601,1.78608)");
        geoPoint("G_{4}=$point(2.53994,1.69622)");
    glEnd();
    //support bar 2_14
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("E_{4}=$point(2.52326,1.54948)");
        geoPoint("D_{4}=$point(2.56886,1.49294)");
        geoPoint("O_{3}=$point(2.58426,1.4072)");
        geoPoint("P_{3}=$point(2.58498,1.34805)");
        geoPoint("N_{3}=$point(2.65888,1.24105)");
        geoPoint("Z_{3}=$point(2.69807,1.26775)");
        geoPoint("K_{4}=$point(2.73977,1.25939)");
        geoPoint("J_{4}=$point(2.83679,1.28291)");
        geoPoint("I_{4}=$point(2.85069,1.33008)");
        geoPoint("H_{4}=$point(2.72516,1.72636)");
        geoPoint("M_{5}=$point(2.70583,1.81325)");
        geoPoint("L_{5}=$point(2.6431,1.85002)");
        geoPoint("K_{5}=$point(2.55498,1.8326)");
        geoPoint("J_{5}=$point(2.53601,1.78608)");
        geoPoint("G_{4}=$point(2.53994,1.69622)");
    glEnd();
    //support bar 2_15
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("E_{4}=$point(2.52326,1.54948)");
        geoPoint("D_{4}=$point(2.56886,1.49294)");
        geoPoint("O_{3}=$point(2.58426,1.4072)");
        geoPoint("P_{3}=$point(2.58498,1.34805)");
        geoPoint("N_{3}=$point(2.65888,1.24105)");
        geoPoint("Z_{3}=$point(2.69807,1.26775)");
        geoPoint("K_{4}=$point(2.73977,1.25939)");
        geoPoint("J_{4}=$point(2.83679,1.28291)");
        geoPoint("I_{4}=$point(2.85069,1.33008)");
        geoPoint("H_{4}=$point(2.72516,1.72636)");
        geoPoint("M_{5}=$point(2.70583,1.81325)");
        geoPoint("L_{5}=$point(2.6431,1.85002)");
        geoPoint("K_{5}=$point(2.55498,1.8326)");
        geoPoint("J_{5}=$point(2.53601,1.78608)");
        geoPoint("G_{4}=$point(2.53994,1.69622)");
    glEnd();
    //support bar 2_16
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("K_{4}=$point(2.73977,1.25939)");
        geoPoint("N_{5}=$point(2.74741,1.21974)");
        geoPoint("O_{5}=$point(2.91984,0.77331)");
        geoPoint("P_{5}=$point(3,0.6)");
        geoPoint("D_{6}=$point(3.07363,0.64617)");
        geoPoint("S_{5}=$point(3,0.8) ");
        geoPoint("T_{5}=$point(2.84448,1.24224) ");
        geoPoint("J_{4}=$point(2.82667,1.28988)");
    glEnd();
    //support bar 2_17
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("P_{5}=$point(3,0.6)");
        geoPoint("U_{5}=$point(3.12397,0.42318)");
        geoPoint("V_{5}=$point(3.28084,0.23779) ");
        geoPoint("B_{6}=$point(3.33928,0.30067)");
        geoPoint("C_{6}=$point(3.2078,0.46587)");
        geoPoint("D_{6}=$point(3.07363,0.64617)");
    glEnd();
    //support bar 2_18
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("V_{5}=$point(3.28084,0.23779) ");
        geoPoint("W_{5}=$point(3.46908,0.04385)");
        geoPoint("Q_{5}=$point(3.49877,0.00296)");
        geoPoint("A_{6}=$point(3.58502,0.07832) ");
        geoPoint("B_{6}=$point(3.33928,0.30067)");
    glEnd();
    //handle bar 1_1
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("K_{5}=$point(2.55498,1.8326)");
        geoPoint("L_{5}=$point(2.6431,1.85002)");
        geoPoint("Z_{5}=$point(2.6,2)");
        geoPoint("R_{5}=$point(2.51969,1.98521)");
    glEnd();
    //handle bar 1_2
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("R_{5}=$point(2.51969,1.98521)");
        geoPoint("Z_{5}=$point(2.6,2)");
        geoPoint("E_{6}=$point(2.60658,2.05493)");
        geoPoint("F_{6}=$point(2.51862,2.05279)");
    glEnd();
    //handle bar 1_3
    glBegin(GL_TRIANGLES);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("F_{6}=$point(2.51862,2.05279)");
        geoPoint("E_{6}=$point(2.60658,2.05493)");
        geoPoint("G_{6}=$point(2.5272,2.11715)");
    glEnd();
    //handle bar 1_4
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("G_{6}=$point(2.5272,2.11715)");
        geoPoint("E_{6}=$point(2.60658,2.05493)");
        geoPoint("H_{6}=$point(2.63017,2.11179)");
        geoPoint("I_{6}=$point(2.5701,2.16864)");
    glEnd();
    //handle bar 1_5
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("H_{6}=$point(2.63017,2.11179)");
        geoPoint("J_{6}=$point(2.69346,2.14718)");
        geoPoint("M_{6}=$point(2.74732,2.1749)");
        geoPoint("N_{6}=$point(2.8,2.2)");
        geoPoint("O_{6}=$point(2.80073,2.2448)");
        geoPoint("P_{6}=$point(2.76385,2.27836)");
        geoPoint("L_{6}=$point(2.70097,2.2566)");
        geoPoint("K_{6}=$point(2.63876,2.22656)");
        geoPoint("I_{6}=$point(2.5701,2.16864)");
    glEnd();
    //handle bar 2_1
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("L_{6}=$point(2.70097,2.2566)");
        geoPoint("P_{6}=$point(2.76385,2.27836)");
        geoPoint("R_{6}=$point(2.69275,2.44107)");
        geoPoint("Q_{6}=$point(2.6385,2.40538)");
    glEnd();
    //handle bar 2_2
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("Q_{6}=$point(2.6385,2.40538)");
        geoPoint("R_{6}=$point(2.69275,2.44107)");
        geoPoint("B_{7}=$point(2.67002,2.47144)");
        geoPoint("T_{6}=$point(2.64635,2.48605)");
        geoPoint("U_{6}=$point(2.61137,2.43394)");
    glEnd();
    //handle bar 2_3
    glBegin(GL_TRIANGLES);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("U_{6}=$point(2.61137,2.43394)");
        geoPoint("T_{6}=$point(2.64635,2.48605)");
        geoPoint("S_{6}=$point(2.6,2.5)");
    glEnd();
    //handle bar 2_4
    glBegin(GL_TRIANGLES);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("U_{6}=$point(2.61137,2.43394)");
        geoPoint("S_{6}=$point(2.6,2.5)");
        geoPoint("V_{6}=$point(2.55,2.45)");
    glEnd();
    //handle bar 2_5
    glBegin(GL_TRIANGLES);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("V_{6}=$point(2.55,2.45)");
        geoPoint("S_{6}=$point(2.6,2.5)");
        geoPoint("W_{6}=$point(2.54856,2.51317)");
    glEnd();
    //handle bar 2_6
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("V_{6}=$point(2.55,2.45)");
        geoPoint("W_{6}=$point(2.54856,2.51317)");
        geoPoint("Z_{6}=$point(2.45647,2.5146)");
        geoPoint("A_{7}=$point(2.45861,2.45107)");
    glEnd();
    //handle bar 3_1
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("A_{7}=$point(2.45861,2.45107)");
        geoPoint("L_{7}=$point(2.44266,2.43726)");
        geoPoint("C_{7}=$point(2.41951,2.41902) ");
        geoPoint("D_{7}=$point(2.10198,2.42327)");
        geoPoint("E_{7}=$point(2.07488,2.43933)");
        geoPoint("F_{7}=$point(2.05481,2.4614)");
        geoPoint("G_{7}=$point(2.0528,2.50958)");
        geoPoint("H_{7}=$point(2.07689,2.53266)");
        geoPoint("I_{7}=$point(2.1,2.55)");
        geoPoint("J_{7}=$point(2.41249,2.54528)");
        geoPoint("K_{7}=$point(2.43143,2.52914)");
        geoPoint("Z_{6}=$point(2.45647,2.5146)");
    glEnd();
    //handle bar 3_2
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.5f, 0.5f);
        geoPoint("F_{7}=$point(2.05481,2.4614)");
        geoPoint("G_{7}=$point(2.0528,2.50958)");
        geoPoint("N_{7}=$point(2.00918,2.508)");
        geoPoint("M_{7}=$point(2.01173,2.46851)");
    glEnd();
    //seat 1
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("A_{3}=$point(0.54038,1.22851)");
        geoPoint("A_{4}=$point(0.64756,1.26972)");
        geoPoint("P_{7}=$point(0.50444,1.6268)");
        geoPoint("O_{7}=$point(0.39181,1.58389)");
    glEnd();
    //seat 2
    glBegin(GL_POLYGON);
        glColor3f(0.5f, 0.0f, 0.5f);
        geoPoint("O_{7}=$point(0.39181,1.58389)");
        geoPoint("P_{7}=$point(0.50444,1.6268)");
        geoPoint("N_{8}=$point(0.60574,1.66209)");
        geoPoint("M_{8}=$point(0.67382,1.68653)");
        geoPoint("L_{8}=$point(0.77158,1.72493)");
        geoPoint("K_{8}=$point(0.84315,1.77032)");
        geoPoint("J_{8}=$point(0.85712,1.82095)");
        geoPoint("I_{8}=$point(0.8449,1.87332)");
        geoPoint("H_{8}=$point(0.82046,1.8995)");
        geoPoint("G_{8}=$point(0.75412,1.9414)");
        geoPoint("F_{8}=$point(0.66859,1.96234)");
        geoPoint("E_{8}=$point(0.57083,1.9606)");
        geoPoint("D_{8}=$point(0.45562,1.96234)");
        geoPoint("C_{8}=$point(0.25367,1.95221)");
        geoPoint("B_{8}=$point(-0.0747,1.94496)");
        geoPoint("O_{8}=$point(-0.12849,1.93855)");
        geoPoint("A_{8}=$point(-0.15528,1.89856)");
        geoPoint("Z_{7}=$point(-0.16243,1.85029)");
        geoPoint("W_{7}=$point(-0.1374,1.77698)");
        geoPoint("V_{7}=$point(-0.11416,1.74301)");
        geoPoint("P_{8}=$point(-0.05933,1.71186)");
        geoPoint("U_{7}=$point(-0.00689,1.66971)");
        geoPoint("T_{7}=$point(0.05211,1.63574)");
        geoPoint("S_{7}=$point(0.16832,1.59641)");
        geoPoint("R_{7}=$point(0.24341,1.58032)");
        geoPoint("Q_{7}=$point(0.32744,1.57317)");
    glEnd();

    drawChainDotsContinuous(-2.71359, 0.31228, 0.2f, // rear wheel
                        1.1454, -0.09165, 0.2f,   // gear
                        75);                        // number of dots




    drawWheel(-2.75192, 0.20551, wR, 14, wheelSpinDeg, true, 0.0f, 0.0f, 0.0f);   // left rear

    //mudguard
    // Rear wheel mudguard
    drawMudguard(-2.75192, 0.20551, wR, wR + 0.2f, 180.0f, 0.0f, 0.25f, 0.41f, 0.88f, 32);


    float gR =  0.31918254953747;

    drawWheel(1.1454,  -0.09165, gR, 4, wheelSpinDeg, true, 0.5f, 0.5f, 0.5f);//gear

    // crankshaft
    drawPedalSystem(1.1454,  -0.09165, 0.5f, rickshawWheelAngle);
    //torso
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f,  1.0f);
        geoPoint("N_{3}=$point(-0.27765,1.91279) ");
        geoPoint("O_{3}=$point(1.09559, 2.08182)");
        geoPoint("F_{1}=$point(1.24291,2.89574)");
        geoPoint("E_{1}=$point(1.36615,3.19008)");
        geoPoint("D_{1}=$point(1.37827,3.29908)");
        geoPoint("C_{1}=$point(1.37221,3.42019)");
        geoPoint("B_{1}=$point(1.29955,3.60791)");
        geoPoint("A_{1}=$point(1.13605,3.63213)");
        geoPoint("Z=$point(0.93017,3.58368)");
        geoPoint("W=$point(0.36701,3.19008)");
        geoPoint("V=$point(-0.15375,2.72987)");
        geoPoint("U=$point(-0.29908,2.36655)");
    glEnd();

    //tank cut  out
    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("E_{3}=$point(0.7,3.3)");
        geoPoint("F_{3}=$point(0.70962,3.14531)");
        geoPoint("G_{3}=$point(0.82884,3.01662)");
        geoPoint("H_{3}=$point(0.88183,2.99013)");
        geoPoint("I_{3}=$point(1,3)");
        geoPoint("J_{3}=$point(1.0673,3.01473)");
        geoPoint("K_{3}=$point(1.32278,3.27021)");
        geoPoint("L_{3}=$point(1.32278,3.3989)");
        geoPoint("M_{3}=$point(1.30764,3.44054)");
        geoPoint("A_{3}=$point(1.22742,3.52724)");
        geoPoint("B_{3}=$point(1.07521,3.55035)");
        geoPoint("C_{3}=$point(0.96784,3.52724)");
        geoPoint("D_{3}=$point(0.80203,3.43483)");
    glEnd();

    //arm_1
    glBegin(GL_QUADS);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("O_{2}=$point(0.81454,3.28539)");
        geoPoint("M_{2}=$point(1.36791,2.56503)");
        geoPoint("N_{2}=$point(1.44976,2.76149)");
        geoPoint("P_{2}=$point(1.24021,3.43601)");
    glEnd();
    //arm_2
    glBegin(GL_QUADS);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("M_{2}=$point(1.36791,2.56503)");
        geoPoint("J_{2}=$point(2.04897,2.49627)");
        geoPoint("I_{2}=$point(2.12755,2.62397)");
        geoPoint("N_{2}=$point(1.44976,2.76149)");
    glEnd();
    //hand
    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("J_{2}=$point(2.04897,2.49627)");
        geoPoint("K_{2}=$point(2.11773,2.40131)");
        geoPoint("L_{2}=$point(2.37313,2.38494)");
        geoPoint("G_{2}=$point(2.44189,2.48972)");
        geoPoint("H_{2}=$point(2.35676,2.61087)");
        geoPoint("I_{2}=$point(2.12755,2.62397)");
    glEnd();
    //lungi
    glBegin(GL_POLYGON);
        glColor3f(0.1f, 0.5f, 0.2f);
        geoPoint("N_{3}=$point(-0.27765,1.91279)");
        geoPoint("P_{3}=$point(0.28659, 0.881)");
        geoPoint("Q_{3}=$point(0.65173, 0.72385)");
        geoPoint("R_{3}=$point(1.40851, 1.0309)");
        geoPoint("U_{3}=$point(1.6279, 1.16514))");
        geoPoint("T_{3}=$point(1.75614, 1.39775)");
        geoPoint("S_{3}=$point(1.51159, 1.71387)");
        geoPoint("O_{3}=$point(1.09559, 2.08182)");
    glEnd();
    //neck
    glBegin(GL_QUADS);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("B_{1}=$point(1.29955,3.60791)");
        geoPoint("C_{1}=$point(1.37221,3.42019)");
        geoPoint("H_{1}=$point(1.53571,3.57157)");
        geoPoint("G_{1}=$point(1.46304,3.7714)");
    glEnd();
    //hair 1
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("G_{1}=$point(1.46304,3.7714)");
        geoPoint("M_{4}=$point(1.57664, 3.97755)");
        geoPoint("C=$point(1.64507,3.98666)");
        geoPoint("P_{4}=$point(1.6516, 4.1487)");
        geoPoint("S_{4}=$point(1.63, 4.35)");
        geoPoint("K_{1}=$point(1.47011, 4.33588)");
        geoPoint("J_{1}=$point(1.34264, 4.17861)");
        geoPoint("F_{2}=$point(1.36063, 3.90003)");
    glEnd();
    //hair 2
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("P_{4}=$point(1.6516, 4.1487)");
        geoPoint("Q_{4}=$point(1.71243,4.15011)");
        geoPoint("N_{1}=$point(2.01181, 4.08359)");
        geoPoint("M_{1}=$point(2.10891, 4.21252)");
        geoPoint("L_{1}=$point(1.76478, 4.3574)");
        geoPoint("S_{4}=$point(1.63, 4.35)");
    glEnd();
    //ear
    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("I_{4}=$point(1.51648,3.9265)");
        geoPoint("C_{4}=$point(1.56186, 3.89084)");
        geoPoint("J_{4}=$point(1.59373, 3.91402)");
        geoPoint("M_{4}=$point(1.57664, 3.97755)");
        geoPoint("L_{4}=$point(1.5724,4.01008)");
        geoPoint("K_{4}=$point(1.567,4.06235)");
        geoPoint("D_{4}=$point(1.5423,4.09379)");
        geoPoint("E_{4}=$point(1.50301,4.08369)");
        geoPoint("F_{4}=$point(1.47606,4.03878)");
        geoPoint("G_{4}=$point(1.4783,3.98825)");
        geoPoint("H_{4}=$point(1.52097,3.95569)");
    glEnd();
    //face-1
    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("G_{1}=$point(1.46304,3.7714)");
        geoPoint("H_{1}=$point(1.53571,3.57157)");
        geoPoint("I_{1}=$point(1.67498, 3.43835)");
        geoPoint("P_{1}=$point(1.77761, 3.46175)");
        geoPoint("Q_{1}=$point(1.82794, 3.51532)");
        geoPoint("O_{1}=$point(1.96247, 3.68564)");
        geoPoint("R_{1}=$point(1.93063, 3.82413)");
    glEnd();
    //face-2
    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("C=$point(1.64507,3.98666)");
        geoPoint("R_{1}=$point(1.93063, 3.82413)");
        geoPoint("S_{1}=$point(2.00704, 4.02469)");
        geoPoint("N_{1}=$point(2.01181, 4.08359)");
        geoPoint("Q_{4}=$point(1.71243,4.15011)");
        geoPoint("P_{4}=$point(1.6516, 4.1487)");
    glEnd();
    //face-3
    glBegin(GL_POLYGON);
        glColor3f(0.8f, 0.5f, 0.2f);
        geoPoint("G_{1}=$point(1.46304,3.7714)");
        geoPoint("R_{1}=$point(1.93063, 3.82413)");
        geoPoint("C=$point(1.64507,3.98666)");
        geoPoint("M_{4}=$point(1.57664, 3.97755)");
    glEnd();




    glPopMatrix();
}

// Simple placeholder car drawing (replace with your full drawCar if you have)
void drawCar(float x, float y, float scale, float wheelSpinDeg) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale/50.0f, scale/50.0f, 1.0f);

   //back-1
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("C=$point(-5.02276,0.99137)");
        geoPoint("D=$point(-5.04243,0.93042)");
        geoPoint("E=$point(-5.1,0.9)");
        geoPoint("F=$point(-5.13483,0.80459)");
        geoPoint("G=$point(-5.14663,0.57653)");
        geoPoint("H=$point(-5.13483,0.52148)");
        geoPoint("I=$point(-5.09354,0.4448)");
        geoPoint("J=$point(-5.03653,0.37599)");
        geoPoint("K=$point(-5.01293,0.3229)");
        geoPoint("L=$point(-5.05422,0.201)");
        geoPoint("M=$point(-4.53125,0.05158)");
        geoPoint("N=$point(-4.54894,0.9953)");
    glEnd();

    //back light-1
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        geoPoint("C=$point(-5.02276,0.99137)");
        geoPoint("N=$point(-4.54894,0.9953)");
        geoPoint("U_{2}=$point(-4.61779,1.06562)");
        geoPoint("P_{2}=$point(-4.99167,1.07262)");
    glEnd();
    //back light-2
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        geoPoint("N=$point(-4.55,1)");
        geoPoint("Q=$point(-4.56568,1.06898)");
        geoPoint("U_{2}=$point(-4.61779,1.06562)");
    glEnd();
    //back light-3
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        geoPoint("U_{2}=$point(-4.61779,1.06562)");
        geoPoint("Q=$point(-4.56568,1.06898)");
        geoPoint("R=$point(-4.72455,1.24047)");
        geoPoint("T_{2}=$point(-4.74805,1.20539)");
    glEnd();
    //back light-4
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        geoPoint("T_{2}=$point(-4.74805,1.20539)");
        geoPoint("R=$point(-4.72455,1.24047)");
        geoPoint("R_{1}=$point(-4.76388,1.26142)");
    glEnd();
    //back light-5
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        geoPoint("S_{2}=$point(-4.97705,1.22001)");
        geoPoint("T_{2}=$point(-4.74805,1.20539)");
        geoPoint("R_{1}=$point(-4.76388,1.26142)");
        geoPoint("Q_{1}=$point(-5.00994,1.27117)");
    glEnd();

    //actual back light
    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("P_{2}=$point(-4.99167,1.07262)");
        geoPoint("U_{2}=$point(-4.61779,1.06562)");
        geoPoint("T_{2}=$point(-4.74805,1.20539)");
        geoPoint("S_{2}=$point(-4.97705,1.22001)");
        geoPoint("R_{2}=$point(-5.01725,1.17616)");
        geoPoint("Q_{2}=$point(-5.02212,1.10185)");
    glEnd();

    //back-2
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Q_{1}=$point(-5.00994,1.27117)");
        geoPoint("R_{1}=$point(-4.76388,1.26142)");
        geoPoint("O_{1}=$point(-4.76225,1.52233)");
        geoPoint("T=$point(-4.91829,1.5187)");
    glEnd();

    //back-3
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("O_{1}=$point(-4.76225,1.52233)");
        geoPoint("T=$point(-4.91829,1.5187)");
        geoPoint("S_{1}=$point(-4.87193,1.56765)");
    glEnd();

    //back-4
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Q=$point(-4.56568,1.06898)");
        geoPoint("N=$point(-4.55,1)");
        geoPoint("N_{1}=$point(-4.41283,1.53998)");
    glEnd();
    //back-5
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("R=$point(-4.72455,1.24047)");
        geoPoint("Q=$point(-4.56568,1.06898)");
        geoPoint("N_{1}=$point(-4.41283,1.53998)");
    glEnd();
    //back-6
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("R=$point(-4.72455,1.24047)");
        geoPoint("N_{1}=$point(-4.41283,1.53998)");
        geoPoint("O_{1}=$point(-4.76225,1.52233)");
        geoPoint("R_{1}=$point(-4.76388,1.26142)");
    glEnd();

    //fender-1
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("T_{1}=$point(-4.92998,1.63053)");
        geoPoint("S_{1}=$point(-4.87193,1.56765)");
        geoPoint("V_{1}=$point(-4.83485,1.71599)");
        geoPoint("G_{2}=$point(-4.88529,1.7061)");
        geoPoint("U_{1}=$point(-4.9461,1.69019)");
    glEnd();

    //fender-2
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("S_{1}=$point(-4.87193,1.56765)");
        geoPoint("O_{1}=$point(-4.76225,1.52233)");
        geoPoint("W_{1}=$point(-4.70264,1.72566)");
        geoPoint("V_{1}=$point(-4.83485,1.71599)");
    glEnd();
    //fender-3
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("E_{2}=$point(-4.59945,1.6789)");
        geoPoint("Z_{1}=$point(-4.57043,1.72083)");
        geoPoint("W_{1}=$point(-4.70264,1.72566)");
        geoPoint("P_{1}=$point(-4.72, 1.68)");
    glEnd();
    //fender-4
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("P_{1}=$point(-4.72, 1.68)");
        geoPoint("O_{1}=$point(-4.76225,1.52233)");
        geoPoint("F_{2}=$point(-4.49636,1.52562)");
        geoPoint("D_{2}=$point(-4.5,1.6)");
        geoPoint("E_{2}=$point(-4.59945,1.6789)");
    glEnd();
    //fender-5
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("D_{2}=$point(-4.5,1.6)");
        geoPoint("F_{2}=$point(-4.49636,1.52562)");
        geoPoint("N_{1}=$point(-4.41283,1.53998)");
        geoPoint("H_{2}=$point(-4.43953,1.58473)");
    glEnd();
    //fender in-1
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("E_{2}=$point(-4.59945,1.6789)");
        geoPoint("D_{2}=$point(-4.5,1.6)");
        geoPoint("A_{2}=$point(-4.41564,1.70148)");
        geoPoint("Z_{1}=$point(-4.57043,1.72083)");
    glEnd();
    //fender in-2
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("D_{2}=$point(-4.5,1.6)");
        geoPoint("H_{2}=$point(-4.43953,1.58473)");
        geoPoint("B_{2}=$point(-4.29794,1.67407)");
        geoPoint("A_{2}=$point(-4.41564,1.70148)");
    glEnd();
    //fender in-3
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{2}=$point(-4.43953,1.58473)");
        geoPoint("I_{2}=$point(-4.15979,1.60581)");
        geoPoint("C_{2}=$point(-4.15122,1.63537)");
        geoPoint("B_{2}=$point(-4.29794,1.67407)");
    glEnd();
    //fender in-4
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("I_{2}=$point(-4.15979,1.60581)");
        geoPoint("J_{2}=$point(-4.10077,1.61479)");
        geoPoint("C_{2}=$point(-4.15122,1.63537)");
    glEnd();
    //back-7
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("N_{1}=$point(-4.41283,1.53998)  ");
        geoPoint("I_{2}=$point(-4.15979,1.60581)");
        geoPoint("H_{2}=$point(-4.43953,1.58473)");
    glEnd();
    //back-8
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("N=$point(-4.55,1)");
        geoPoint("M=$point(-4.53125,0.05158)");
        geoPoint("O=$point(-4,0)");
        geoPoint("P=$point(-3.88466,0.08789)");
        geoPoint("L_{2}=$point(-3.2636,1.6574)");
        geoPoint("K_{2}=$point(-3.55287,1.6572)");
        geoPoint("J_{2}=$point(-4.10077,1.61479)");
        geoPoint("I_{2}=$point(-4.15979,1.60581)");
        geoPoint("N_{1}=$point(-4.41283,1.53998)");
    glEnd();
    //back wind shield-1
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("K_{2}=$point(-3.55287,1.6572)");
        geoPoint("L_{2}=$point(-3.2636,1.6574)");
        geoPoint("M_{2}=$point(-3.03868,1.6574)");
        geoPoint("N_{2}=$point(-2.91777,1.68918)");
        geoPoint("O_{2}=$point(-2.70411,1.79149)");
        geoPoint("V_{2}=$point(-2.51152,1.89983)");
        geoPoint("W_{2}=$point(-2.343,1.99612)");
        geoPoint("Z_{2}=$point(-2.16245,2.10145)");
        geoPoint("D_{3}=$point(-2.16006,2.22424)");
        geoPoint("E_{3}=$point(-2.21247,2.24073)");
    glEnd();
    //back wind shield-2
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Z_{2}=$point(-2.16245,2.10145)");
        geoPoint("A_{3}=$point(-2.03519,2.16441)");
        geoPoint("P_{3}=$point(-2.02095,2.19193)");
        geoPoint("B_{3}=$point(-2.04107,2.21341)");
        geoPoint("C_{3}=$point(-2.0875,2.21497)");
    glEnd();
    //back wind shield-3
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Z_{2}=$point(-2.16245,2.10145)");
        geoPoint("C_{3}=$point(-2.0875,2.21497)");
        geoPoint("D_{3}=$point(-2.16006,2.22424)");
    glEnd();

    //back tire back flap-1
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("O=$point(-4,0)");
        geoPoint("R_{3}=$point(-3.76117,-0.04746)");
        geoPoint("O_{3}=$point(-3.6044155385006333,0.08607220550752162)");
        geoPoint("P=$point(-3.88466,0.08789)");
    glEnd();
    //back tire back flap-2
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("R_{3}=$point(-3.76117,-0.04746)");
        geoPoint("S=$point(-3.6,-0.2)");
        geoPoint("O_{3}=$point(-3.6044155385006333,0.08607220550752162)");
    glEnd();
    //back 9
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("P=$point(-3.88466,0.08789)");
        geoPoint("O_{3}=$point(-3.6044155385006333,0.08607220550752162)");
        geoPoint("M_{2}=$point(-3.03868,1.6574)");
        geoPoint("L_{2}=$point(-3.2636,1.6574)");
    glEnd();
    //back 10
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("O_{3}=$point(-3.6044155385006333,0.08607220550752162)");
        geoPoint("Q_{3}=$point(-1.95196,0.08155)");
        geoPoint("F_{3}=$point(-2.3801827320273716,1.5712207195797614)");
        geoPoint("G_{3}=$point(-2.4,1.6)");
        geoPoint("M_{2}=$point(-3.03868,1.6574)");
    glEnd();

    //rear upper-1
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("M_{2}=$point(-3.03868,1.6574)");
        geoPoint("G_{3}=$point(-2.4,1.6)");
        geoPoint("H_{3}=$point(-2.37678,1.65085)");
        geoPoint("N_{2}=$point(-2.91777,1.68918)");
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("N_{2}=$point(-2.91777,1.68918)");
        geoPoint("H_{3}=$point(-2.37678,1.65085)");
        geoPoint("O_{2}=$point(-2.70411,1.79149)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("O_{2}=$point(-2.70411,1.79149)");
        geoPoint("H_{3}=$point(-2.37678,1.65085)");
        geoPoint("V_{2}=$point(-2.51152,1.89983)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("V_{2}=$point(-2.51152,1.89983)");
        geoPoint("H_{3}=$point(-2.37678,1.65085)");
        geoPoint("I_{3}=$point(-2.31036,1.73387)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("V_{2}=$point(-2.51152,1.89983)");
        geoPoint("I_{3}=$point(-2.31036,1.73387)");
        geoPoint("W_{2}=$point(-2.343,1.99612)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("W_{2}=$point(-2.343,1.99612)");
        geoPoint("I_{3}=$point(-2.31036,1.73387)");
        geoPoint("J_{3}=$point(-2.21224,1.82444)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("W_{2}=$point(-2.343,1.99612)");
        geoPoint("J_{3}=$point(-2.21224,1.82444)");
        geoPoint("Z_{2}=$point(-2.16245,2.10145)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Z_{2}=$point(-2.16245,2.10145)");
        geoPoint("J_{3}=$point(-2.21224,1.82444)");
        geoPoint("L_{3}=$point(-2.05326,1.96718)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Z_{2}=$point(-2.16245,2.10145)");
        geoPoint("L_{3}=$point(-2.05326,1.96718)");
        geoPoint("A_{3}=$point(-2.03519,2.16441)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("A_{3}=$point(-2.03519,2.16441)");
        geoPoint("L_{3}=$point(-2.05326,1.96718)");
        geoPoint("N_{3}=$point(-1.8616,2.09402)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("A_{3}=$point(-2.03519,2.16441)");
        geoPoint("N_{3}=$point(-1.8616,2.09402)");
        geoPoint("K_{3}=$point(-1.73871,2.16862)");
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("A_{3}=$point(-2.03519,2.16441)");
        geoPoint("K_{3}=$point(-1.73871,2.16862)");
        geoPoint("P_{3}=$point(-2.02095,2.19193)");
    glEnd();
    //top-1
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("E_{3}=$point(-2.21247,2.24073)");
        geoPoint("D_{3}=$point(-2.16006,2.22424)");
        geoPoint("C_{3}=$point(-2.0875,2.21497)");
        geoPoint("B_{3}=$point(-2.04107,2.21341)");
        geoPoint("M_{3}=$point(-2.10195,2.2719)");
    glEnd();
    //top-2
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("M_{3}=$point(-2.10195,2.2719)");
        geoPoint("B_{3}=$point(-2.04107,2.21341)");
        geoPoint("P_{3}=$point(-2.02095,2.19193)");
        geoPoint("K_{3}=$point(-1.73871,2.16862)");
        geoPoint("S_{3}=$point(-1.99698,2.2889)");
    glEnd();
    //top-3
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("S_{3}=$point(-1.99698,2.2889)");
        geoPoint("K_{3}=$point(-1.73871,2.16862)");
        geoPoint("T_{3}=$point(-1.87326,2.31053)");
    glEnd();
    //top-4
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("T_{3}=$point(-1.87326,2.31053)");
        geoPoint("K_{3}=$point(-1.73871,2.16862)");
        geoPoint("O_{4}=$point(-1.6086,2.23023)");
        geoPoint("U_{3}=$point(-1.66324,2.33074)");
    glEnd();
    //top-5
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("O_{4}=$point(-1.6086,2.23023)");
        geoPoint("Q_{4}=$point(-1.54313,2.25485)");
        geoPoint("U_{3}=$point(-1.66324,2.33074)");
    glEnd();
    //top-6
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("U_{3}=$point(-1.66324,2.33074)");
        geoPoint("Q_{4}=$point(-1.54313,2.25485)");
        geoPoint("P_{4}=$point(-1.48346,2.26623)");
        geoPoint("V_{3}=$point(-1.45647,2.34509)");
    glEnd();
    //top-7
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("V_{3}=$point(-1.45647,2.34509)");
        geoPoint("P_{4}=$point(-1.48346,2.26623)");
        geoPoint("R_{4}=$point(-1.32917,2.27823)");
        geoPoint("W_{3}=$point(-1.19555,2.36406)");
    glEnd();
    //top-8
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("W_{3}=$point(-1.19555,2.36406)");
        geoPoint("R_{4}=$point(-1.32917,2.27823)");
        geoPoint("S_{4}=$point(-1.00344,2.2857)");
    glEnd();
    //top-9
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("W_{3}=$point(-1.19555,2.36406)");
        geoPoint("S_{4}=$point(-1.00344,2.2857)");
        geoPoint("Z_{3}=$point(-0.80168,2.37766)");
    glEnd();
    //top-10
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("S_{4}=$point(-1.00344,2.2857)");
        geoPoint("T_{4}=$point(-0.62248,2.29231)");
        geoPoint("Z_{3}=$point(-0.80168,2.37766)");
    glEnd();
    //top-11
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("T_{4}=$point(-0.62248,2.29231)");
        geoPoint("A_{4}=$point(-0.60227,2.3797)");
        geoPoint("Z_{3}=$point(-0.80168,2.37766)");
    glEnd();
    //top-12
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("T_{4}=$point(-0.62248,2.29231)");
        geoPoint("B_{4}=$point(-0.39878,2.37224)");
        geoPoint("A_{4}=$point(-0.60227,2.3797)");
    glEnd();
    //top-13
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("T_{4}=$point(-0.62248,2.29231)");
        geoPoint("U_{4}=$point(-0.2965,2.2857)");
        geoPoint("B_{4}=$point(-0.39878,2.37224)");
    glEnd();
    //top-14
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("U_{4}=$point(-0.2965,2.2857)");
        geoPoint("C_{4}=$point(-0.20114,2.36353)");
        geoPoint("B_{4}=$point(-0.39878,2.37224)");
    glEnd();
    //top-15
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("U_{4}=$point(-0.2965,2.2857)");
        geoPoint("V_{4}=$point(-0.02451,2.27671)");
        geoPoint("D_{4}=$point(-0.01103,2.34976)");
        geoPoint("C_{4}=$point(-0.20114,2.36353)");
    glEnd();
    //top-16
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("V_{4}=$point(-0.02451,2.27671)");
        geoPoint("W_{4}=$point(0.18842,2.26386)");
        geoPoint("E_{4}=$point(0.18521,2.33105)");
        geoPoint("D_{4}=$point(-0.01103,2.34976)");
    glEnd();
    //top-17
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("W_{4}=$point(0.18842,2.26386)");
        geoPoint("Z_{4}=$point(0.32217,2.24258)");
        geoPoint("F_{4}=$point(0.36903,2.31243)");
        geoPoint("E_{4}=$point(0.18521,2.33105)");
    glEnd();
    //top-18
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Z_{4}=$point(0.32217,2.24258)");
        geoPoint("A_{5}=$point(0.44833,2.20762)");
        geoPoint("G_{4}=$point(0.50101,2.28644)");
        geoPoint("F_{4}=$point(0.36903,2.31243)");
    glEnd();
    //top-19
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("A_{5}=$point(0.44833,2.20762)");
        geoPoint("B_{5}=$point(0.5737,2.1663)");
        geoPoint("J_{4}=$point(0.59434,2.2264)");
        geoPoint("G_{4}=$point(0.50101,2.28644)");
    glEnd();
    //top-20
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("G_{4}=$point(0.50101,2.28644)");
        geoPoint("J_{4}=$point(0.59434,2.2264)");
        geoPoint("I_{4}=$point(0.60449,2.2434)");
    glEnd();
    //top-21
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("G_{4}=$point(0.50101,2.28644)");
        geoPoint("I_{4}=$point(0.60449,2.2434)");
        geoPoint("H_{4}=$point(0.63659,2.257)");
    glEnd();
    //front-1
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("J_{4}=$point(0.59434,2.2264)");
        geoPoint("B_{5}=$point(0.5737,2.1663)");
        geoPoint("K_{4}=$point(0.61674,2.20341)");
    glEnd();
    //front-2
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("B_{5}=$point(0.5737,2.1663)");
        geoPoint("L_{4}=$point(0.66095,2.1816)");
        geoPoint("K_{4}=$point(0.61674,2.20341)");
    glEnd();
    //front-3
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("B_{5}=$point(0.5737,2.1663)");
        geoPoint("C_{5}=$point(0.68654,2.11592)");
        geoPoint("M_{4}=$point(0.71333,2.15332)");
        geoPoint("L_{4}=$point(0.66095,2.1816)");
    glEnd();
    //front-4
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("C_{5}=$point(0.68654,2.11592)");
        geoPoint("D_{5}=$point(0.8014,2.05547)");
        geoPoint("M_{4}=$point(0.71333,2.15332)");
    glEnd();
    //front-5
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("D_{5}=$point(0.8014,2.05547)");
        geoPoint("P_{5}=$point(1.12251,1.92411)");
        geoPoint("M_{4}=$point(0.71333,2.15332)");
    glEnd();
    //front-6
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("D_{5}=$point(0.8014,2.05547)");
        geoPoint("E_{5}=$point(1.19031,1.82575)");
        geoPoint("P_{5}=$point(1.12251,1.92411)");
    glEnd();
    //front-7
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("E_{5}=$point(1.19031,1.82575)");
        geoPoint("Q_{5}=$point(1.43564,1.73464)");
        geoPoint("P_{5}=$point(1.12251,1.92411)");
    glEnd();
    //front-8
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("E_{5}=$point(1.19031,1.82575)");
        geoPoint("F_{5}=$point(1.44018,1.66052)");
        geoPoint("Q_{5}=$point(1.43564,1.73464)");
    glEnd();
    //front-9
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("F_{5}=$point(1.44018,1.66052)");
        geoPoint("N_{4}=$point(1.84377,1.47202)");
        geoPoint("Q_{5}=$point(1.43564,1.73464)");
    glEnd();
    //front-10
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("F_{5}=$point(1.44018,1.66052)");
        geoPoint("G_{5}=$point(1.69535,1.44211)");
        geoPoint("N_{4}=$point(1.84377,1.47202)");
    glEnd();
    //front-11
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("G_{5}=$point(1.69535,1.44211)");
        geoPoint("S_{5}=$point(1.85892,1.45157) ");
        geoPoint("N_{4}=$point(1.84377,1.47202)");
    glEnd();
    //front wind shield-1
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("J_{4}=$point(0.59434,2.2264)");
        geoPoint("K_{4}=$point(0.61674,2.20341)");
        geoPoint("H_{4}=$point(0.63659,2.257)");
        geoPoint("I_{4}=$point(0.60449,2.2434)");
    glEnd();
    //front wind shield-2
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("K_{4}=$point(0.61674,2.20341)");
        geoPoint("L_{4}=$point(0.66095,2.1816)");
        geoPoint("D_{6}=$point(0.8,2.2)");
        geoPoint("H_{4}=$point(0.63659,2.257)");
     glEnd();
     //front wind shield-3
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("L_{4}=$point(0.66095,2.1816)");
        geoPoint("M_{4}=$point(0.71333,2.15332)");
        geoPoint("D_{6}=$point(0.8,2.2)");
     glEnd();
     //front wind shield-4
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("M_{4}=$point(0.71333,2.15332)");
        geoPoint("C_{6}=$point(1.03764,2.08992)");
        geoPoint("D_{6}=$point(0.8,2.2)");
     glEnd();
     //front wind shield-5
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("M_{4}=$point(0.71333,2.15332)");
        geoPoint("P_{5}=$point(1.12251,1.92411)");
        geoPoint("C_{6}=$point(1.03764,2.08992)");
     glEnd();
     //front wind shield-6
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("P_{5}=$point(1.12251,1.92411)");
        geoPoint("B_{6}=$point(1.5009,1.8349)");
        geoPoint("C_{6}=$point(1.03764,2.08992)");
     glEnd();
     //front wind shield-7
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("P_{5}=$point(1.12251,1.92411)");
        geoPoint("Q_{5}=$point(1.43564,1.73464)");
        geoPoint("B_{6}=$point(1.5009,1.8349)");
     glEnd();
     //front wind shield-8
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Q_{5}=$point(1.43564,1.73464)");
        geoPoint("A_{6}=$point(2.05616,1.50344)");
        geoPoint("B_{6}=$point(1.5009,1.8349)");
     glEnd();
    //front wind shield-9
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Q_{5}=$point(1.43564,1.73464)");
        geoPoint("N_{4}=$point(1.84377,1.47202)");
        geoPoint("A_{6}=$point(2.05616,1.50344)");
     glEnd();
     //front wind shield-10
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("N_{4}=$point(1.84377,1.47202)");
        geoPoint("R_{5}=$point(1.86499,1.46376)");
        geoPoint("T_{5}=$point(1.90382,1.4654)");
        geoPoint("U_{5}=$point(1.94261,1.46723)");
        geoPoint("V_{5}=$point(1.98544,1.4759)");
        geoPoint("W_{5}=$point(2.01435,1.48063)");
        geoPoint("Z_{5}=$point(2.03215,1.49)");
        geoPoint("A_{6}=$point(2.05616,1.50344)");
     glEnd();
     //window-1
     glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("F_{3}=$point(-2.3801827320273716,1.5712207195797614)");
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("H_{3}=$point(-2.37678,1.65085)");
        geoPoint("G_{3}=$point(-2.4,1.6)");
    glEnd();
    //window-2
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("I_{3}=$point(-2.31036,1.73387)");
        geoPoint("H_{3}=$point(-2.37678,1.65085)");
    glEnd();
    //window-3
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("J_{3}=$point(-2.21224,1.82444)");
        geoPoint("I_{3}=$point(-2.31036,1.73387)");
    glEnd();
    //window-4
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("L_{3}=$point(-2.05326,1.96718)");
        geoPoint("J_{3}=$point(-2.21224,1.82444)");
    glEnd();
    //window-5
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("N_{3}=$point(-1.8616,2.09402)");
        geoPoint("L_{3}=$point(-2.05326,1.96718)");
    glEnd();
    //window-6
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("K_{3}=$point(-1.73871,2.16862)");
        geoPoint("N_{3}=$point(-1.8616,2.09402)");
    glEnd();
    //window-7
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("O_{4}=$point(-1.6086,2.23023)");
        geoPoint("K_{3}=$point(-1.73871,2.16862)");
    glEnd();
    //window-8
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("Q_{4}=$point(-1.54313,2.25485)");
        geoPoint("O_{4}=$point(-1.6086,2.23023)");
    glEnd();
    //window-9
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("P_{4}=$point(-1.48346,2.26623)");
        geoPoint("Q_{4}=$point(-1.54313,2.25485)");
    glEnd();
    //window-10
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("R_{4}=$point(-1.32917,2.27823)");
        geoPoint("P_{4}=$point(-1.48346,2.26623)");
    glEnd();
    //window-11
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("S_{4}=$point(-1.00344,2.2857)");
        geoPoint("R_{4}=$point(-1.32917,2.27823)");
    glEnd();
    //window-12
     glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("T_{4}=$point(-0.62248,2.29231)");
        geoPoint("S_{4}=$point(-1.00344,2.2857)");
    glEnd();
    //window-13
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("I_{5}=$point(-0.46814,1.49184)");
        geoPoint("T_{4}=$point(-0.62248,2.29231)");
    glEnd();
    //window-14
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("I_{5}=$point(-0.46814,1.49184)");
        geoPoint("J_{5}=$point(0.19191,1.4735)");
        geoPoint("U_{4}=$point(-0.2965,2.2857)");
        geoPoint("T_{4}=$point(-0.62248,2.29231)");
    glEnd();
    //window-15
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("J_{5}=$point(0.19191,1.4735)");
        geoPoint("B_{5}=$point(0.5737,2.1663)");
        geoPoint("A_{5}=$point(0.44833,2.20762)");
        geoPoint("Z_{4}=$point(0.32217,2.24258)");
        geoPoint("W_{4}=$point(0.18842,2.26386)");
        geoPoint("V_{4}=$point(-0.02451,2.27671)");
        geoPoint("U_{4}=$point(-0.2965,2.2857)");
    glEnd();
    //window-16
    glBegin(GL_TRIANGLE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("G_{5}=$point(1.69535,1.44211)");
        geoPoint("F_{5}=$point(1.44018,1.66052)");
        geoPoint("L_{5}=$point(1.28893,1.45211)");
        geoPoint("F_{5}=$point(1.44018,1.66052)");
        geoPoint("E_{5}=$point(1.19031,1.82575)");
        geoPoint("L_{5}=$point(1.28893,1.45211)");
        geoPoint("K_{5}=$point(0.7939,1.45211)");
        geoPoint("E_{5}=$point(1.19031,1.82575)");
        geoPoint("D_{5}=$point(0.8014,2.05547)");
        geoPoint("K_{5}=$point(0.7939,1.45211)");
        geoPoint("D_{5}=$point(0.8014,2.05547)");
        geoPoint("C_{5}=$point(0.68654,2.11592)");
    glEnd();
    //window-17
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("J_{5}=$point(0.19191,1.4735)");
        geoPoint("K_{5}=$point(0.7939,1.45211)");
        geoPoint("C_{5}=$point(0.68654,2.11592)");
        geoPoint("B_{5}=$point(0.5737,2.1663)");
    glEnd();
    //body
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Q_{3}=$point(-1.95196,0.08155)");
        geoPoint("I_{5}=$point(-0.46814,1.49184)");
        geoPoint("H_{5}=$point(-1.40626,1.53462)");
        geoPoint("F_{3}=$point(-2.3801827320273716,1.5712207195797614)");
    glEnd();
    //body-1
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Q_{3}=$point(-1.95196,0.08155)");
        geoPoint("U=$point(2.18916,0.11667)");
        geoPoint("J_{5}=$point(0.19191,1.4735)");
        geoPoint("I_{5}=$point(-0.46814,1.49184)");
    glEnd();
    //body-2
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("U=$point(2.18916,0.11667)");
        geoPoint("G_{5}=$point(1.69535,1.44211)");
        geoPoint("L_{5}=$point(1.28893,1.45211)");
        geoPoint("K_{5}=$point(0.7939,1.45211)");
        geoPoint("J_{5}=$point(0.19191,1.4735)");
    glEnd();
    //body-3
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("U=$point(2.18916,0.11667)");
        geoPoint("V=$point(2.4,0.1)");
        geoPoint("S_{5}=$point(1.85892,1.45157)");
        geoPoint("G_{5}=$point(1.69535,1.44211)");
    glEnd();
    //lower side bumper 1
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Q_{3}=$point(-1.95196,0.08155)");
        geoPoint("M_{5}=$point(-1.92319,-0.1978)");
        geoPoint("N_{5}=$point(-1.71607,-0.15579)");
    glEnd();
    //lower side bumper 2
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Q_{3}=$point(-1.95196,0.08155)");
        geoPoint("N_{5}=$point(-1.71607,-0.15579)");
        geoPoint("W=$point(2.17429,-0.13702)");
        geoPoint("U=$point(2.18916,0.11667)");
    glEnd();
    //lower side bumper 3
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("W=$point(2.17429,-0.13702)");
        geoPoint("B_{1}=$point(2.39675,-0.12501)");
        geoPoint("V=$point(2.4,0.1)");
        geoPoint("U=$point(2.18916,0.11667)");
    glEnd();
    //lower side bumper 4
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("W=$point(2.17429,-0.13702)");
        geoPoint("Z=$point(2.29552,-0.24868)");
        geoPoint("A_{1}=$point(2.37528,-0.25187)");
        geoPoint("B_{1}=$point(2.39675,-0.12501)");
    glEnd();
    //lower side bumper 5
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("B_{1}=$point(2.39675,-0.12501)");
        geoPoint("O_{5}=$point(3.97921,-0.01359)");
        geoPoint("E_{6}=$point(3.99701,0.08181)");
        geoPoint("V=$point(2.4,0.1)");
    glEnd();
    //lower side bumper 6
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("F_{6}=$point(3.86,-0.02)");
        geoPoint("C_{1}=$point(3.95449,-0.05407)");
        geoPoint("D_{1}=$point(4.4,0)");
        geoPoint("O_{5}=$point(3.97921,-0.01359)");
    glEnd();

    //front hood-1
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("N_{4}=$point(1.84377,1.47202)");
        geoPoint("S_{5}=$point(1.85892,1.45157)");
        geoPoint("R_{5}=$point(1.86499,1.46376)");
    glEnd();
    //front hood-2
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("V=$point(2.4,0.1)");
        geoPoint("A_{6}=$point(2.05616,1.50344)");
        geoPoint("Z_{5}=$point(2.03215,1.49)");
        geoPoint("W_{5}=$point(2.01435,1.48063)");
        geoPoint("V_{5}=$point(1.98544,1.4759)");
        geoPoint("U_{5}=$point(1.94261,1.46723)");
        geoPoint("T_{5}=$point(1.90382,1.4654)");
        geoPoint("R_{5}=$point(1.86499,1.46376)");
        geoPoint("S_{5}=$point(1.85892,1.45157)");
    glEnd();
    //front hood-3
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("V=$point(2.4,0.1)");
        geoPoint("G_{6}=$point(2.45276,1.47459)");
        geoPoint("A_{6}=$point(2.05616,1.50344)");
    glEnd();
    //front hood-4
    glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("V=$point(2.4,0.1)");
        geoPoint("E_{6}=$point(3.99701,0.08181)");
        geoPoint("V_{6}=$point(4.09346,0.92912)");
        geoPoint("U_{6}=$point(4.0911,0.95748)");
        geoPoint("H_{6}=$point(2.92422,1.42564)");
        geoPoint("G_{6}=$point(2.45276,1.47459)");
    glEnd();
    //front hood-5
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("O_{5}=$point(3.97921,-0.01359)");
        geoPoint("D_{1}=$point(4.4,0)");
        geoPoint("E_{6}=$point(3.99701,0.08181)");
    glEnd();
    //front hood-6
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("E_{6}=$point(3.99701,0.08181)");
        geoPoint("D_{1}=$point(4.4,0)");
        geoPoint("E_{1}=$point(4.67869,0.02249)");
        geoPoint("F_{1}=$point(4.75336,0.1005)");
        geoPoint("G_{1}=$point(4.70882,0.21716)");
        geoPoint("H_{1}=$point(4.75761,0.3826)");
        geoPoint("I_{1}=$point(4.8276,0.44624)");
        geoPoint("J_{1}=$point(4.81275,0.70713)");
        geoPoint("K_{1}=$point(4.72342,0.74581)");
        geoPoint("A_{7}=$point(4.48097,0.75714)");
        geoPoint("Z_{6}=$point(4.25086,0.76157)");
        geoPoint("V_{6}=$point(4.09346,0.92912)");
    glEnd();
    //front hood-7
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("H_{6}=$point(2.92422,1.42564)");
        geoPoint("U_{6}=$point(4.0911,0.95748)");
        geoPoint("T_{6}=$point(4.097,0.98112)");
        geoPoint("I_{6}=$point(3.45365,1.35351)");
    glEnd();
    //front hood-8
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("T_{6}=$point(4.097,0.98112)");
        geoPoint("S_{6}=$point(4.12655,0.98821)");
        geoPoint("J_{6}=$point(3.74864,1.30069)");
        geoPoint("I_{6}=$point(3.45365,1.35351)");
    glEnd();
    //front hood-9
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("S_{6}=$point(4.12655,0.98821)");
        geoPoint("R_{6}=$point(4.2,1)");
        geoPoint("K_{6}=$point(4.04749,1.23629)");
        geoPoint("J_{6}=$point(3.74864,1.30069)");
    glEnd();
    //front hood-10
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("S_{6}=$point(4.12655,0.98821)");
        geoPoint("R_{6}=$point(4.2,1)");
        geoPoint("K_{6}=$point(4.04749,1.23629)");
        geoPoint("J_{6}=$point(3.74864,1.30069)");
    glEnd();
    //front hood-11
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("R_{6}=$point(4.2,1)");
        geoPoint("Q_{6}=$point(4.31207,1.00714)");
        geoPoint("L_{6}=$point(4.26003,1.17574)");
        geoPoint("K_{6}=$point(4.04749,1.23629)");
    glEnd();
    //front hood-12
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("Q_{6}=$point(4.31207,1.00714)");
        geoPoint("P_{6}=$point(4.63672,1.00284)");
        geoPoint("O_{6}=$point(4.56532,1.05079)");
        geoPoint("N_{6}=$point(4.48545,1.08815)");
        geoPoint("M_{6}=$point(4.38756,1.13581)");
        geoPoint("L_{6}=$point(4.26003,1.17574)");
    glEnd();
    //front light-1
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.75f, 0.75f, 0.75f); // Light gray
        geoPoint("A_{7}=$point(4.48097,0.75714)");
        geoPoint("K_{1}=$point(4.72342,0.74581)");
        geoPoint("B_{7}=$point(4.71883,0.8158)");
        geoPoint("C_{7}=$point(4.7,0.9)");
        geoPoint("D_{7}=$point(4.67007,0.96794)");
        geoPoint("P_{6}=$point(4.63672,1.00284)");
        geoPoint("Q_{6}=$point(4.31207,1.00714)");
    glEnd();
    //front light-2
    glBegin(GL_QUADS);
        glColor3f(0.75f, 0.75f, 0.75f); // Light gray
        geoPoint("A_{7}=$point(4.48097,0.75714)");
        geoPoint("Q_{6}=$point(4.31207,1.00714)");
        geoPoint("R_{6}=$point(4.2,1)");
        geoPoint("Z_{6}=$point(4.25086,0.76157)");
    glEnd();
    //front light-3
    glBegin(GL_TRIANGLES);
        glColor3f(0.75f, 0.75f, 0.75f); // Light gray
        geoPoint("Z_{6}=$point(4.25086,0.76157)");
        geoPoint("U_{6}=$point(4.0911,0.95748)");
        geoPoint("V_{6}=$point(4.09346,0.92912)");
    glEnd();
    //front light-4
    glBegin(GL_TRIANGLES);
        glColor3f(0.75f, 0.75f, 0.75f); // Light gray
        geoPoint("Z_{6}=$point(4.25086,0.76157)");
        geoPoint("T_{6}=$point(4.097,0.98112)");
        geoPoint("U_{6}=$point(4.0911,0.95748)");
    glEnd();
    //front light-5
    glBegin(GL_TRIANGLES);
        glColor3f(0.75f, 0.75f, 0.75f); // Light gray
        geoPoint("Z_{6}=$point(4.25086,0.76157)");
        geoPoint("S_{6}=$point(4.12655,0.98821)");
        geoPoint("T_{6}=$point(4.097,0.98112)");
    glEnd();
    //front light-6
    glBegin(GL_TRIANGLES);
        glColor3f(0.75f, 0.75f, 0.75f); // Light gray
        geoPoint("Z_{6}=$point(4.25086,0.76157)");
        geoPoint("R_{6}=$point(4.2,1)");
        geoPoint("S_{6}=$point(4.12655,0.98821)");
    glEnd();
    glLineWidth(1.0f);   // makes lines 5 pixels thick

    //cosmetics
    //back-1
    glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("P=$point(-4.52185,0.05462)");
        geoPoint("Q=$point(-4.53389,0.09402)");
        geoPoint("R=$point(-4.54265,0.12357)");
        geoPoint("S=$point(-4.54593,0.18049)");
        geoPoint("T=$point(-4.54374,0.21661)");
        geoPoint("U=$point(-4.53279,0.30089)");
        geoPoint("V=$point(-4.54375,0.39425)");
        geoPoint("W=$point(-4.54572,0.70724)");
        geoPoint("Z=$point(-4.54375,1.00252)");
    glEnd();

    //back-2
    glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("C=$point(-5.00848,1.28657)");
        geoPoint("D=$point(-4.97575,1.34604)");
        geoPoint("E=$point(-4.93571,1.41175)");
        geoPoint("F=$point(-4.89566,1.47746)");
        geoPoint("G=$point(-4.86486,1.51237)");
        geoPoint("H=$point(-4.77142,1.50929)");
        geoPoint("F_{5}=$point(-4.45153,1.51995)");
        geoPoint("G_{5}=$point(-4.42685,1.52574)");
        geoPoint("I=$point(-4.41115,1.54229) ");
        geoPoint("J=$point(-3.87931,1.56199)");
        geoPoint("K=$point(-3.65169,1.57731)");
        geoPoint("L=$point(-3.50724,1.59263)");
        geoPoint("M=$point(-3.39044,1.61255)");
        geoPoint("N=$point(-3.35271,1.62727)");
        geoPoint("O=$point(-3.32153,1.659)");
    glEnd();
    //oil
    glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("N_{1}=$point(-3.57643,1.42531)");
        geoPoint("M_{1}=$point(-3.57694,1.23332)");
        geoPoint("L_{1}=$point(-3.56838,1.20636)");
        geoPoint("K_{1}=$point(-3.54998,1.19138)");
        geoPoint("J_{1}=$point(-3.5243,1.1847)");
        geoPoint("I_{1}=$point(-3.24124,1.18613)");
        geoPoint("H_{1}=$point(-3.2147,1.18869)");
        geoPoint("G_{1}=$point(-3.19583,1.20052)");
        geoPoint("F_{1}=$point(-3.18929,1.22313) ");
        geoPoint("E_{1}=$point(-3.19,1.44)");
        geoPoint("D_{1}=$point(-3.19308,1.45037)");
        geoPoint("C_{1}=$point(-3.21445,1.47051)");
        geoPoint("B_{1}=$point(-3.23468,1.478) ");
        geoPoint("A_{1}=$point(-3.52713,1.47709)");
        geoPoint("P_{1}=$point(-3.5517,1.47243)");
        geoPoint("O_{1}=$point(-3.56997,1.45249)");
        geoPoint("N_{1}=$point(-3.57643,1.42531)");
    glEnd();
    //backdoor
    glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("H_{4}=$point(-2.34782,1.57894)");
        geoPoint("I_{4}=$point(-2.46498,1.50665)");
        geoPoint("J_{4}=$point(-2.53383,1.4523)");
        geoPoint("K_{4}=$point(-2.56645,1.407)");
        geoPoint("L_{4}=$point(-2.59363,1.33271)");
        geoPoint("M_{4}=$point(-2.61537,1.27291)");
        geoPoint("N_{4}=$point(-2.63893,1.18413)");
        geoPoint("O_{4}=$point(-2.64255,1.05186)");
        geoPoint("P_{4}=$point(-2.61537,1.00475)");
        geoPoint("Q_{4}=$point(-2.58819,0.97576)");
        geoPoint("R_{4}=$point(-2.53927,0.94677)");
        geoPoint("S_{4}=$point(-2.46136,0.90872)");
        geoPoint("T_{4}=$point(-2.37438,0.87067)");
        geoPoint("U_{4}=$point(-2.26929,0.80725)");
        geoPoint("V_{4}=$point(-2.16782,0.73115)");
        geoPoint("W_{4}=$point(-2.09535,0.6623)");
        geoPoint("Z_{4}=$point(-2.03374,0.58801)");
        geoPoint("A_{5}=$point(-1.96851,0.47748)");
        geoPoint("B_{5}=$point(-1.92684,0.39051)");
        geoPoint("C_{5}=$point(-1.89604,0.27092)");
        geoPoint("D_{5}=$point(-1.87973,0.19663)");
        geoPoint("E_{5}=$point(-1.88159,0.06548)");
    glEnd();
    //door handle back-1
    glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("G_{6}=$point(-2.45,1.3)");
        geoPoint("H_{6}=$point(-2.47522,1.27937)");
        geoPoint("I_{6}=$point(-2.49501,1.25738)");
        geoPoint("J_{6}=$point(-2.49428,1.17602)");
        geoPoint("U_{5}=$point(-2.47023,1.15369)");
        geoPoint("V_{5}=$point(-2.4459,1.14157)");
        geoPoint("W_{5}=$point(-2.42117,1.1311)");
        geoPoint("Z_{5}=$point(-2.11425,1.1311)");
        geoPoint("A_{6}=$point(-2.07795,1.15109)");
        geoPoint("B_{6}=$point(-2.07062,1.17308)");
        geoPoint("C_{6}=$point(-2.07355,1.25005)");
        geoPoint("D_{6}=$point(-2.10434,1.28156)");
        geoPoint("E_{6}=$point(-2.12926,1.29256)");
        geoPoint("F_{6}=$point(-2.15,1.3)");
        geoPoint("G_{6}=$point(-2.45,1.3)");
    glEnd();
    //door handle back-2
    glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("P_{6}=$point(-2.37334,1.2757)");
        geoPoint("Q_{6}=$point(-2.39679,1.25957)");
        geoPoint("K_{6}=$point(-2.39669,1.22011)");
        geoPoint("L_{6}=$point(-2.12119,1.21853)");
        geoPoint("M_{6}=$point(-2.11826,1.24931)");
        geoPoint("N_{6}=$point(-2.14612,1.26837)");
        geoPoint("O_{6}=$point(-2.16224,1.27423)");
    glEnd();
    //door divide
    glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("W_{2}=$point(-0.45126,0.03519)");
        geoPoint("Z_{2}=$point(-0.42218,0.2092)");
        geoPoint("A_{3}=$point(-0.38906,0.37089)");
        geoPoint("B_{3}=$point(-0.37542,0.53648)");
        geoPoint("C_{3}=$point(-0.37153,0.86181)");
        geoPoint("D_{3}=$point(-0.4027,1.23195)");
        geoPoint("E_{3}=$point(-0.45679,1.54279)");
    glEnd();
    //door handle front-1
    glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("V_{6}=$point(-0.22398,1.08977)");
        geoPoint("W_{6}=$point(0.0982,1.088)");
        geoPoint("Z_{6}=$point(0.12661,1.10486)");
        geoPoint("A_{7}=$point(0.13829,1.11822)");
        geoPoint("B_{7}=$point(0.1388,1.19001)");
        geoPoint("C_{7}=$point(0.12353,1.21343)");
        geoPoint("D_{7}=$point(0.09603,1.2394)");
        geoPoint("E_{7}=$point(0.08004,1.25125) ");
        geoPoint("F_{7}=$point(-0.21789,1.25312)");
        geoPoint("G_{7}=$point(-0.26,1.23758)");
        geoPoint("H_{7}=$point(-0.27296,1.22462)");
        geoPoint("R_{6}=$point(-0.28079,1.20427)");
        geoPoint("S_{6}=$point(-0.28167,1.14036)");
        geoPoint("T_{6}=$point(-0.26925,1.11373)");
        geoPoint("U_{6}=$point(-0.2444,1.09243)");
        geoPoint("V_{6}=$point(-0.22398,1.08977)");
    glEnd();
    //door handle front-2
    glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("I_{7}=$point(-0.18671,1.17397)");
        geoPoint("J_{7}=$point(0.09732,1.17297)");
        geoPoint("K_{7}=$point(0.0933,1.19881)");
        geoPoint("L_{7}=$point(0.08177,1.21577)");
        geoPoint("M_{7}=$point(0.07159,1.21984)");
        geoPoint("N_{7}=$point(0.06034,1.23079)");
        geoPoint("O_{7}=$point(-0.17193,1.23138)");
        geoPoint("P_{7}=$point(-0.18687,1.20763)");
        geoPoint("I_{7}=$point(-0.18671,1.17397)");
    glEnd();
    //front door divide
    glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Q_{7}=$point(1.611,1.44835)");
        geoPoint("V_{2}=$point(1.71056,1.44766)");
        geoPoint("U_{2}=$point(1.83169,1.36471)");
        geoPoint("T_{2}=$point(1.9,1.3)");
        geoPoint("S_{2}=$point(1.91882,1.25581)");
        geoPoint("R_{2}=$point(1.94931,1.15562)");
        geoPoint("Q_{2}=$point(1.9798,0.99008)");
        geoPoint("P_{2}=$point(2,0.8)");
        geoPoint("O_{2}=$point(1.99128,0.59929)");
        geoPoint("N_{2}=$point(1.98421,0.50281)");
        geoPoint("M_{2}=$point(1.95697,0.37027)");
        geoPoint("L_{2}=$point(1.91354,0.23021)");
        geoPoint("K_{2}=$point(1.8667,0.10544)");
    glEnd();
    //hood divide
    glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("U_{3}=$point(1.98312,1.49486)");
        geoPoint("V_{3}=$point(2.04461,1.46063)");
        geoPoint("W_{3}=$point(2.09862,1.44215)");
        geoPoint("Z_{3}=$point(2.18532,1.42936)");
        geoPoint("A_{4}=$point(2.58327,1.40662)");
        geoPoint("B_{4}=$point(3.37207,1.31993)");
        geoPoint("C_{4}=$point(4.0557,1.19485)");
        geoPoint("D_{4}=$point(4.4,1.1)");
        geoPoint("E_{4}=$point(4.50482,1.04704)");
        geoPoint("F_{4}=$point(4.52183,1.02505)");
        geoPoint("G_{4}=$point(4.53223,1.00426)");
    glEnd();
    //front light divide
    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Q_{1}=$point(4.3101,1.00586)");
        geoPoint("R_{1}=$point(4.48091,0.76225)");
    glEnd();
    //front signal light
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.5f, 0.0f);
        geoPoint("U_{1}=$point(4.1365,0.94239)");
        geoPoint("V_{1}=$point(4.25877,0.80518)");
        geoPoint("S_{1}=$point(4.37731,0.80612)");
        geoPoint("T_{1}=$point(4.2653,0.94239)");
    glEnd();
    //line from light to front tire
    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        geoPoint("Z_{1}=$point(4.2541,0.76505)");
        geoPoint("W_{1}=$point(3.72301,0.77252)");
    glEnd();
    //flood light
    glBegin(GL_POLYGON);
        glColor3f(0.75f, 0.75f, 0.75f); // Light gray
        geoPoint("H_{2}=$point(4.43524,0.3755)");
        geoPoint("G_{2}=$point(4.41978,0.3636)");
        geoPoint("F_{2}=$point(4.40312,0.33446)");
        geoPoint("E_{2}=$point(4.39777,0.28926)");
        geoPoint("D_{2}=$point(4.40191,0.22274)");
        geoPoint("C_{2}=$point(4.42582,0.20665)");
        geoPoint("B_{2}=$point(4.58098,0.20616)");
        geoPoint("A_{2}=$point(4.70744,0.21264)");
        geoPoint("J_{2}=$point(4.75282,0.36301)");
        geoPoint("I_{2}=$point(4.74806,0.37728)");
    glEnd();
    //side mirror
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 1.0f);
        geoPoint("G_{3}=$point(1.27436,1.46409)");
        geoPoint("F_{3}=$point(1.58503,1.47144)");
        geoPoint("T_{3}=$point(1.41246,1.60164)");
        geoPoint("S_{3}=$point(1.3926,1.64534)");
        geoPoint("R_{3}=$point(1.38307,1.68109)");
        geoPoint("Q_{3}=$point(1.36082,1.71049)");
        geoPoint("P_{3}=$point(1.3211,1.72876)");
        geoPoint("O_{3}=$point(1.23472,1.74933)");
        geoPoint("N_{3}=$point(1.07967,1.74802)");
        geoPoint("M_{3}=$point(1.06388,1.73263)");
        geoPoint("L_{3}=$point(1.05491,1.70735)");
        geoPoint("K_{3}=$point(1.05899,1.5777)");
        geoPoint("J_{3}=$point(1.07693,1.53693)");
        geoPoint("I_{3}=$point(1.10099,1.51285)");
        geoPoint("H_{3}=$point(1.17503,1.48215)");
    glEnd();

    drawCarWheelsAndGuards();



    glPopMatrix();
}
// Helper function to draw building windows in a grid pattern
void drawBuildingWindows(float startX, float startY, float windowWidth, float windowHeight, int cols, int rows) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            // Randomly skip some windows to create a more natural look
            if (rand() % 10 > 1) { // 80% chance to draw a window
                float x = startX + col * (windowWidth + 5);
                float y = startY + row * (windowHeight + 8);
                glVertex2f(x, y);
                glVertex2f(x + windowWidth, y);
                glVertex2f(x + windowWidth, y + windowHeight);
                glVertex2f(x, y + windowHeight);
            }
        }
    }
}

// Helper function to draw rooftop structures
void drawRooftopStructure(float centerX, float baseY, float width, float height) {
    float left = centerX - width/2;
    float right = centerX + width/2;

    glVertex2f(left, baseY);
    glVertex2f(right, baseY);
    glVertex2f(right, baseY + height);
    glVertex2f(left, baseY + height);
}


// --------- Scene ----------
void drawGround() {
    float skyY = worldHalfH / 3.0f; // Horizon height

    // --- Sky Gradient ---
    glBegin(GL_QUADS);
    glColor3f(0.55f, 0.80f, 0.98f); // top (blue)
    glVertex2f(-worldHalfW, worldHalfH);
    glVertex2f(worldHalfW, worldHalfH);
    glColor3f(0.82f, 0.93f, 0.99f); // near horizon (lighter blue)
    glVertex2f(worldHalfW, skyY + 100);
    glVertex2f(-worldHalfW, skyY + 100);
    glEnd();

    // --- Detailed City Skyline (Far Background) ---

    // Left Skyline Cluster - More buildings with varied heights and widths
    glBegin(GL_QUADS);
    glColor3f(0.35f, 0.35f, 0.38f); // Main building color

    // Left cluster - expanded with more buildings
    glVertex2f(-680, skyY + 100); glVertex2f(-630, skyY + 100);
    glVertex2f(-630, skyY + 280); glVertex2f(-680, skyY + 280);

    glVertex2f(-620, skyY + 100); glVertex2f(-580, skyY + 100);
    glVertex2f(-580, skyY + 220); glVertex2f(-620, skyY + 220);

    glVertex2f(-570, skyY + 100); glVertex2f(-530, skyY + 100);
    glVertex2f(-530, skyY + 190); glVertex2f(-570, skyY + 190);

    glVertex2f(-520, skyY + 100); glVertex2f(-480, skyY + 100);
    glVertex2f(-480, skyY + 260); glVertex2f(-520, skyY + 260);

    glVertex2f(-470, skyY + 100); glVertex2f(-430, skyY + 100);
    glVertex2f(-430, skyY + 310); glVertex2f(-470, skyY + 310);

    // Left-center cluster
    glVertex2f(-400, skyY + 100); glVertex2f(-360, skyY + 100);
    glVertex2f(-360, skyY + 240); glVertex2f(-400, skyY + 240);

    glVertex2f(-350, skyY + 100); glVertex2f(-310, skyY + 100);
    glVertex2f(-310, skyY + 190); glVertex2f(-350, skyY + 190);

    glVertex2f(-300, skyY + 100); glVertex2f(-260, skyY + 100);
    glVertex2f(-260, skyY + 270); glVertex2f(-300, skyY + 270);

    // Center cluster - more dense
    glVertex2f(-240, skyY + 100); glVertex2f(-200, skyY + 100);
    glVertex2f(-200, skyY + 320); glVertex2f(-240, skyY + 320);

    glVertex2f(-190, skyY + 100); glVertex2f(-150, skyY + 100);
    glVertex2f(-150, skyY + 270); glVertex2f(-190, skyY + 270);

    glVertex2f(-140, skyY + 100); glVertex2f(-100, skyY + 100);
    glVertex2f(-100, skyY + 290); glVertex2f(-140, skyY + 290);

    glVertex2f(-90, skyY + 100); glVertex2f(-50, skyY + 100);
    glVertex2f(-50, skyY + 250); glVertex2f(-90, skyY + 250);

    glVertex2f(-40, skyY + 100); glVertex2f(0, skyY + 100);
    glVertex2f(0, skyY + 330); glVertex2f(-40, skyY + 330);

    glVertex2f(10, skyY + 100); glVertex2f(50, skyY + 100);
    glVertex2f(50, skyY + 260); glVertex2f(10, skyY + 260);

    glVertex2f(60, skyY + 100); glVertex2f(100, skyY + 100);
    glVertex2f(100, skyY + 280); glVertex2f(60, skyY + 280);

    glVertex2f(110, skyY + 100); glVertex2f(150, skyY + 100);
    glVertex2f(150, skyY + 210); glVertex2f(110, skyY + 210);

    // Right-center cluster
    glVertex2f(160, skyY + 100); glVertex2f(200, skyY + 100);
    glVertex2f(200, skyY + 300); glVertex2f(160, skyY + 300);

    glVertex2f(210, skyY + 100); glVertex2f(250, skyY + 100);
    glVertex2f(250, skyY + 240); glVertex2f(210, skyY + 240);

    glVertex2f(260, skyY + 100); glVertex2f(300, skyY + 100);
    glVertex2f(300, skyY + 270); glVertex2f(260, skyY + 270);

    glVertex2f(310, skyY + 100); glVertex2f(350, skyY + 100);
    glVertex2f(350, skyY + 190); glVertex2f(310, skyY + 190);

    // Right cluster - expanded
    glVertex2f(360, skyY + 100); glVertex2f(400, skyY + 100);
    glVertex2f(400, skyY + 310); glVertex2f(360, skyY + 310);

    glVertex2f(410, skyY + 100); glVertex2f(450, skyY + 100);
    glVertex2f(450, skyY + 250); glVertex2f(410, skyY + 250);

    glVertex2f(460, skyY + 100); glVertex2f(500, skyY + 100);
    glVertex2f(500, skyY + 280); glVertex2f(460, skyY + 280);

    glVertex2f(510, skyY + 100); glVertex2f(550, skyY + 100);
    glVertex2f(550, skyY + 220); glVertex2f(510, skyY + 220);

    glVertex2f(560, skyY + 100); glVertex2f(600, skyY + 100);
    glVertex2f(600, skyY + 340); glVertex2f(560, skyY + 340);
    glEnd();

    // --- Building Details (Windows, Highlights, etc.) ---

    // Windows - left cluster
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.9f, 0.5f); // Yellow window color
    for (int i = 0; i < 4; i++) {
        // Left tall building windows
        glVertex2f(-655, skyY + 120 + i*40); glVertex2f(-635, skyY + 120 + i*40);
        glVertex2f(-635, skyY + 140 + i*40); glVertex2f(-655, skyY + 140 + i*40);
    }

    // Center building windows
    for (int i = 0; i < 6; i++) {
        glVertex2f(-25, skyY + 120 + i*35); glVertex2f(-5, skyY + 120 + i*35);
        glVertex2f(-5, skyY + 140 + i*35); glVertex2f(-25, skyY + 140 + i*35);
    }

    // Right tall building windows
    for (int i = 0; i < 5; i++) {
        glVertex2f(575, skyY + 120 + i*45); glVertex2f(595, skyY + 120 + i*45);
        glVertex2f(595, skyY + 140 + i*45); glVertex2f(575, skyY + 140 + i*45);
    }
    glEnd();

    // --- Sunlight Reflection on Buildings ---
    glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.65f); // Lighter reflection color

    // Multiple reflection strips across different buildings
    glVertex2f(-450, skyY + 180); glVertex2f(-435, skyY + 180);
    glVertex2f(-435, skyY + 300); glVertex2f(-450, skyY + 300);

    glVertex2f(-280, skyY + 150); glVertex2f(-265, skyY + 150);
    glVertex2f(-265, skyY + 260); glVertex2f(-280, skyY + 260);

    glVertex2f(-120, skyY + 200); glVertex2f(-105, skyY + 200);
    glVertex2f(-105, skyY + 280); glVertex2f(-120, skyY + 280);

    glVertex2f(80, skyY + 160); glVertex2f(95, skyY + 160);
    glVertex2f(95, skyY + 270); glVertex2f(80, skyY + 270);

    glVertex2f(220, skyY + 190); glVertex2f(235, skyY + 190);
    glVertex2f(235, skyY + 290); glVertex2f(220, skyY + 290);

    glVertex2f(380, skyY + 170); glVertex2f(395, skyY + 170);
    glVertex2f(395, skyY + 300); glVertex2f(380, skyY + 300);

    glVertex2f(520, skyY + 140); glVertex2f(535, skyY + 140);
    glVertex2f(535, skyY + 210); glVertex2f(520, skyY + 210);
    glEnd();

    // --- Distant Small Buildings (Very Far Background) ---
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.32f); // Even darker for distant buildings

    // Scattered small buildings behind main skyline
    for (int i = 0; i < 8; i++) {
        float x = -700 + i * 180;
        float height = 120 + (i * 23) % 80;
        glVertex2f(x, skyY + 100); glVertex2f(x + 25, skyY + 100);
        glVertex2f(x + 25, skyY + 100 + height); glVertex2f(x, skyY + 100 + height);
    }
    glEnd();

    // --- Curb / Railing (Transition Between Road and Skyline) ---
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.4f, 0.4f); // dark base
    glVertex2f(-worldHalfW, skyY + 10);
    glVertex2f(worldHalfW, skyY + 10);
    glVertex2f(worldHalfW, skyY + 40);
    glVertex2f(-worldHalfW, skyY + 40);
    glEnd();

    // Railing top stripe
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.75f, 0.75f); // light gray top
    glVertex2f(-worldHalfW, skyY + 40);
    glVertex2f(worldHalfW, skyY + 40);
    glVertex2f(worldHalfW, skyY + 45);
    glVertex2f(-worldHalfW, skyY + 45);
    glEnd();

    // --- Road (Foreground) ---
    glColor3f(0.92f, 0.92f, 0.92f);
    glBegin(GL_QUADS);
    glVertex2f(-worldHalfW, -worldHalfH);
    glVertex2f(worldHalfW, -worldHalfH);
    glVertex2f(worldHalfW, skyY + 10);
    glVertex2f(-worldHalfW, skyY + 10);
    glEnd();

    // --- Lane Divider ---
    glLineWidth(3.0f);
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINES);
    glVertex2f(-worldHalfW, -120);
    glVertex2f(worldHalfW, -120);
    glEnd();

    // --- Lane Speed Text ---
    glColor3f(0, 0, 0);
    void *font = GLUT_BITMAP_HELVETICA_18;

    glRasterPos2f(-600, laneY[0] - 10);
    const char *lane1 = "Speed Limit: 40 km/h (slow lane)";
    for (const char* p = lane1; *p; ++p) glutBitmapCharacter(font, *p);

    glRasterPos2f(-600, laneY[1] - 10);
    const char *lane2 = "Speed Limit: 80 km/h (fast lane)";
    for (const char* p = lane2; *p; ++p) glutBitmapCharacter(font, *p);
}


void display_Reshoan() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawGround();

    // Draw car and rickshaw (order so closer object can overlay)
    drawRickshaw(rickshawX, rickshawY, rickshawScale, rickshawWheelAngle);
    drawCar(carX, carY, carScale, carWheelAngle);

    // HUD text
    glColor3f(0, 0, 0);
    glRasterPos2f(-worldHalfW + 10, worldHalfH - 20);
    char buf[256];
    std::snprintf(buf, sizeof(buf), "Rickshaw speed: %.0f km/h  |  Car speed: %.0f km/h  |  %s",
                  rickshawSpeed, carSpeed, paused ? "Paused" : "Running");
    for (const char* p = buf; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);

    // small legend for controls
    glRasterPos2f(-worldHalfW + 10, worldHalfH - 40);
    const char *legend = "Rickshaw: A/D move, W/S speed, Q/E lane.  Car: Arrow keys move, i/k speed, z/x lane.  Mouse L/R = shrink/enlarge both.";
    for (const char* p = legend; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);

    glutSwapBuffers();
}
void reshape_Reshoan(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-worldHalfW, worldHalfW, -worldHalfH, worldHalfH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void timer_Reshoan(int value) {
    static int prev = 0;
    int now = glutGet(GLUT_ELAPSED_TIME); // ms
    float dt = (prev == 0) ? 0.016f : (now - prev) / 1000.0f;
    prev = now;

    if (!paused) {
        // compute wheel angular velocity for each vehicle
        // Rickshaw movement
        rickshawX += rickshawSpeed * dt;
        if (rickshawX > worldHalfW + 160.0f) rickshawX = -worldHalfW - 160.0f;
        if (rickshawX < -worldHalfW - 160.0f) rickshawX = worldHalfW + 160.0f;

        float rwheelRadius = 28.0f;
        float romega = (rickshawSpeed / rwheelRadius) * 180.0f / 3.1415926535f;
        rickshawWheelAngle = std::fmod(rickshawWheelAngle - romega * dt, 360.0f);

        // Car movement, but check for jam (if rickshaw is ahead in same lane and close)
        bool jam = false;
        float jamThreshold = 650.0f; // horizontal proximity threshold for jam
        if (carLane == rickshawLane) {
            // if rickshaw is in front (x greater than car) and within threshold -> jam
            if ((rickshawX > carX) && (fabs(rickshawX - carX) < jamThreshold)) {
                jam = true;
            }
            // also if car is in front and close and moving slower, it still causes no jam for car
        }

        float carEffectiveSpeed = jam ? min(carSpeed, rickshawSpeed) : carSpeed;
        // Move car
        carX += carEffectiveSpeed * dt;
        if (carX > worldHalfW + 160.0f) carX = -worldHalfW - 160.0f;
        if (carX < -worldHalfW - 160.0f) carX = worldHalfW + 160.0f;

        float cwheelRadius = 28.0f;
        float comega = (carEffectiveSpeed / cwheelRadius) * 180.0f / 3.1415926535f;
        carWheelAngle = std::fmod(carWheelAngle - comega * dt, 360.0f);

        // chain offset (visual)
        chainOffset += (romega * dt) / 360.0f;
        chainOffset = fmod(chainOffset, 1.0f);
    }

    // Smooth lane switching (both vehicles)
    float laneSpeed = 200.0f; // units per second (adjust smoothness)
    if (fabs(rickshawY - rickshawTargetY) > 1.0f) {
        if (rickshawY < rickshawTargetY) rickshawY += laneSpeed * dt;
        else if (rickshawY > rickshawTargetY) rickshawY -= laneSpeed * dt;
    } else {
        rickshawY = rickshawTargetY;
    }

    if (fabs(carY - carTargetY) > 1.0f) {
        if (carY < carTargetY) carY += laneSpeed * dt;
        else if (carY > carTargetY) carY -= laneSpeed * dt;
    } else {
        carY = carTargetY;
    }
    float sideThresholdX = 120.0f; // horizontal distance threshold
    float sideThresholdY = 50.0f;  // vertical distance threshold (lane width)

    if (fabs(rickshawX - carX) < sideThresholdX && fabs(rickshawY - carY) < sideThresholdY) {
        laneChangeAllowed = false;  // block lane change
    } else {
        laneChangeAllowed = true;   // allow lane change
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer_Reshoan, 0); // ~60 FPS
}

void keyboard_Reshoan(unsigned char key, int, int) {
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
        case ' ': paused = !paused; break;
        case 'r': case 'R':
            // reset both vehicles
            rickshawX = -250.0f; rickshawWheelAngle = 0.0f; rickshawSpeed = 40.0f; paused = false;
            rickshawLane = 1; rickshawY = laneY[rickshawLane]; rickshawTargetY = rickshawY;
            carX = 50.0f; carWheelAngle = 0.0f; carSpeed = 80.0f;
            carLane = 2; carY = laneY[carLane]; carTargetY = carY;
            break;

        // Rickshaw controls (existing)
        case 'a': case 'A': rickshawX -= 10.0f; break;
        case 'd': case 'D': rickshawX += 10.0f; break;
        case 'w': case 'W': rickshawSpeed = clamp(rickshawSpeed + 20.0f, 0.0f, 400.0f); break;
        case 's': case 'S': rickshawSpeed = clamp(rickshawSpeed - 20.0f, 0.0f, 400.0f); break;

        // Rickshaw lane switching with safety check
        case 'q': case 'Q': // move UP a lane (index lower)
            if (rickshawLane > 0) {
                int desired = rickshawLane - 1;
                // if desired lane is occupied by car, only allow change if rickshaw is entirely before or after car
                if (desired == carLane) {
                    float safeGap = 140.0f;
                    if ( (rickshawX + safeGap < carX) || (rickshawX - safeGap > carX) ) {
                        rickshawLane = desired;
                        rickshawTargetY = laneY[rickshawLane];
                    } else {
                        // deny lane change (unsafe)
                        // optionally you could flash or give feedback; we just ignore the request
                    }
                } else {
                    rickshawLane = desired;
                    rickshawTargetY = laneY[rickshawLane];
                }
            }
            break;

        case 'e': case 'E': // move DOWN a lane (index higher)
            if (rickshawLane < 2) {
                int desired = rickshawLane + 1;
                if (desired == carLane) {
                    float safeGap = 140.0f;
                    if ( (rickshawX + safeGap < carX) || (rickshawX - safeGap > carX) ) {
                        rickshawLane = desired;
                        rickshawTargetY = laneY[rickshawLane];
                    } else {
                        // deny change
                    }
                } else {
                    rickshawLane = desired;
                    rickshawTargetY = laneY[rickshawLane];
                }
            }
            break;

        // Car controls (different keys)
        case 'i': case 'I': carSpeed = clamp(carSpeed + 20.0f, 0.0f, 400.0f); break;
        case 'k': case 'K': carSpeed = clamp(carSpeed - 20.0f, 0.0f, 400.0f); break;

        // Car lane switching (z/x) - no extra safety: car can switch lanes freely
        case 'z': case 'Z':
            if (carLane > 0) { carLane--; carTargetY = laneY[carLane]; }
            break;
        case 'x': case 'X':
            if (carLane < 2) { carLane++; carTargetY = laneY[carLane]; }
            break;

        default:
            break;
    }
}

// Special (arrow keys) for car left/right
void specialKeys_Reshoan(int key, int, int) {
    switch(key) {
        case GLUT_KEY_LEFT: carX -= 10.0f; break;
        case GLUT_KEY_RIGHT: carX += 10.0f; break;
        case GLUT_KEY_UP: carSpeed = clamp(carSpeed + 10.0f, 0.0f, 400.0f); break;
        case GLUT_KEY_DOWN: carSpeed = clamp(carSpeed - 10.0f, 0.0f, 400.0f); break;
    }
    glutPostRedisplay();
}

void initGL_Reshoan() {
    glClearColor(0.96f, 0.98f, 1.0f, 1.0f); // light background
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
