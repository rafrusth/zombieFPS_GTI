#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>

// ===================== KAMUS GLOBAL ===================== //
bool keys[256] = {false};

/* === VARIABEL KAMERA === */
int viewMode = 1;
float recoil = 0.0f;
float camAngleX = 0.0f;
float camAngleY = 0.0f;
bool isShooting = false;
bool ignoreNextWarp = false;

/* === POSISI KAMERA / PEMAIN === */
float posX = 0.0f;
float posY = 0.6f;
float posZ = 5.0f;

/* === PLAYERT & GAME STATE === */
float playerHealth = 100;
bool isDead = false;

/* === DAMAGE COOLDOWN === */
int lastDamageTime = 0;
int damageDelay = 1000; // ms

/* === POSISI ZOMBIE === */
float zomX = 0;
float zomY = 0.5f;
float zomZ = 3;

/* === POSE ZOMBIE === */
static int head = 0;
static int rightShoulder = -90;
static int leftShoulder = -90;

/* === GERAK & ARAH ZOMBIE === */
float zomSpeed = 0.01f;
float zomAngleY = 0.0f;
float stopDistance = 0.22f;

/* === POSISI MATAHARI === */
float sunX = 0.0f;
float sunY = 15.0f;
float sunZ = -30.0f;

// ===================== MATERIAL ===================== //
void setMaterial(float r, float g, float b) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    GLfloat mat_ambient[] = {r * 0.3f, g * 0.3f, b * 0.3f, 1.0f};
    GLfloat mat_diffuse[] = {r, g, b, 1.0f};
    GLfloat mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
}

// ===================== CUBOID ===================== //
void drawCuboid(float w, float h, float d,
                float fR, float fG, float fB,
                float sR, float sG, float sB,
                float tR, float tG, float tB,
                float bR, float bG, float bB) {
    /* === KAMUS LOKAL === */
    float z,y,x;

    /* === ALGORITMA === */
    x = w * 0.5f;
    y = h * 0.5f;
    z = d * 0.5f;

    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glColor3f(fR, fG, fB);
        glVertex3f(-x,-y,z);
        glVertex3f(x,-y,z);
        glVertex3f(x,y,z);
        glVertex3f(-x,y,z);

        glNormal3f(0,0,-1);
        glColor3f(fR * 0.8f, fG * 0.8f, fB * 0.8f);
        glVertex3f(x,-y,-z);
        glVertex3f(-x,-y,-z);
        glVertex3f(-x,y,-z); 
        glVertex3f(x,y,-z);

        glNormal3f(-1,0,0);
        glColor3f(sR, sG, sB);
        glVertex3f(-x,-y,-z);
        glVertex3f(-x,-y,z);
        glVertex3f(-x,y,z);
        glVertex3f(-x,y,-z);

        glNormal3f(1,0,0);
        glColor3f(sR, sG, sB);
        glVertex3f(x,-y,z);
        glVertex3f(x,-y,-z);
        glVertex3f(x,y,-z);
        glVertex3f(x,y,z);

        glNormal3f(0,1,0);
        glColor3f(tR, tG, tB);
        glVertex3f(-x,y,z);
        glVertex3f(x,y,z);
        glVertex3f(x,y,-z);
        glVertex3f(-x,y,-z);

        glNormal3f(0,-1,0);
        glColor3f(bR, bG, bB);
        glVertex3f(-x,-y,-z);
        glVertex3f(x,-y,-z);
        glVertex3f(x,-y,z);
        glVertex3f(-x,-y,z);
    glEnd();
}

void drawCuboid(float w, float h, float d) {
    /* === KAMUS LOKAL === */
    float z,y,x;

    /* === ALGORITMA === */
    x = w * 0.5f;
    y = h * 0.5f;
    z = d * 0.5f;

    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(-x,-y,z);
        glVertex3f(x,-y,z);
        glVertex3f(x,y,z);
        glVertex3f(-x,y,z);

        glNormal3f(0,0,-1);
        glVertex3f(x,-y,-z);
        glVertex3f(-x,-y,-z);
        glVertex3f(-x,y,-z);
        glVertex3f(x,y,-z);

        glNormal3f(-1,0,0);
        glVertex3f(-x,-y,-z);
        glVertex3f(-x,-y,z);
        glVertex3f(-x,y,z);
        glVertex3f(-x,y,-z);

        glNormal3f(1,0,0);
        glVertex3f(x,-y,z);
        glVertex3f(x,-y,-z);
        glVertex3f(x,y,-z);
        glVertex3f(x,y,z);

        glNormal3f(0,1,0);
        glVertex3f(-x,y,z);
        glVertex3f(x,y,z);
        glVertex3f(x,y,-z);
        glVertex3f(-x,y,-z);

        glNormal3f(0,-1,0);
        glVertex3f(-x,-y,-z);
        glVertex3f(x,-y,-z);
        glVertex3f(x,-y,z);
        glVertex3f(-x,-y,z);
    glEnd();
}

void drawFrontPatch(float px, float py, float pw, float ph, float z, float r, float g, float b) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glColor3f(r, g, b);

    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(px, py, z);
        glVertex3f(px + pw, py, z);
        glVertex3f(px + pw, py + ph, z);
        glVertex3f(px, py + ph, z);
    glEnd();
}

// ===================== BAGIAN ZOMBIE ===================== //
void drawHeadZombie() {
    /* === KAMUS LOKAL === */
    float w,h,d,z;

    /* === ALGORITMA === */
    w = 0.5f;
    h = 0.45f;
    d = 0.5f;
    z = d * 0.5f + 0.001f;

    drawCuboid(w, h, d,
                0.34f, 0.52f, 0.24f,
                0.38f, 0.56f, 0.28f,
                0.40f, 0.58f, 0.30f,
                0.28f, 0.42f, 0.20f);

    drawFrontPatch(-0.17f, -0.01f, 0.12f, 0.07f, z, 0.05f, 0.05f, 0.05f);
    drawFrontPatch( 0.06f, -0.01f, 0.12f, 0.07f, z, 0.05f, 0.05f, 0.05f);
    drawFrontPatch(-0.06f,-0.07f, 0.12f, 0.07f, z, 0.22f, 0.34f, 0.16f);
    drawFrontPatch(-0.06f,-0.15f, 0.12f, 0.03f, z, 0.20f, 0.30f, 0.14f);
}

void drawBodyZombie() {
    /* === KAMUS LOKAL === */
    float w,h,d,z;

    /* === ALGORITMA === */
    w = 0.5f;
    h = 0.67f;
    d = 0.25f;
    z = d * 0.5f + 0.001f;

    drawCuboid(w, h, d,
                0.08f, 0.78f, 0.82f,
                0.06f, 0.68f, 0.72f,
                0.12f, 0.82f, 0.86f,
                0.05f, 0.55f, 0.60f);

    drawFrontPatch(-0.1f, 0.27f, 0.2f, 0.06f, z, 0.30f, 0.32f, 0.12f);
    drawFrontPatch(-0.05f, 0.21f, 0.1f, 0.06f, z, 0.30f, 0.32f, 0.12f);
}

void drawArmZombie() {
    /* === KAMUS LOKAL === */
    float w,h,d;
    float armH;
    float sleeveH;

    /* === ALGORITMA === */
    w = 0.25f;
    h = 0.65f;
    d = 0.25f;
    sleeveH = 0.20f;
    armH = h - sleeveH;

    glPushMatrix();
        glTranslatef(0.0f, (h - sleeveH) * 0.5f, 0.0f);
        drawCuboid(w, sleeveH, d,
                    0.08f, 0.78f, 0.82f,
                    0.06f, 0.68f, 0.72f,
                    0.12f, 0.82f, 0.86f,
                    0.05f, 0.55f, 0.60f);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, -(sleeveH * 0.5f), 0.0f);
        drawCuboid(w, armH, d,
                    0.46f, 0.60f, 0.35f,
                    0.42f, 0.56f, 0.32f,
                    0.48f, 0.62f, 0.37f,
                    0.38f, 0.50f, 0.28f);
    glPopMatrix();
}

void drawLegZombie() {
    /* === KAMUS LOKAL === */
    float w,h,d;

    /* === ALGORITMA === */
    w = 0.25f;
    h = 0.7f;
    d = 0.18f;

    glPushMatrix();
        glTranslatef(0.0f, 0.05f, 0.0f);
        drawCuboid(w, h, d,
                    0.34f, 0.27f, 0.78f,
                    0.30f, 0.24f, 0.70f,
                    0.37f, 0.30f, 0.82f,
                    0.30f, 0.24f, 0.70f);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, -0.30f, 0.04f);
        drawCuboid(0.27f, 0.12f, 0.28f,
                    0.45f, 0.45f, 0.45f,
                    0.40f, 0.40f, 0.40f,
                    0.50f, 0.50f, 0.50f,
                    0.30f, 0.30f, 0.30f);
    glPopMatrix();
}

void drawBucketHat() {
    /* === KAMUS LOKAL === */
    GLUquadric* quad;

    /* === ALGORITMA === */
    quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    setMaterial(0.85f, 0.85f, 0.85f);

    // Crown
    glPushMatrix();
        gluCylinder(quad, 0.22f, 0.20f, 0.22f, 24, 4);

        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.22f);
            gluDisk(quad, 0.0f, 0.22f, 24, 1);
        glPopMatrix();
    glPopMatrix();

    // Brim atas
    glPushMatrix();
        gluDisk(quad, 0.20f, 0.34f, 24, 1); 
    glPopMatrix();

    // Brim bawah
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -0.02f);
        gluDisk(quad, 0.18f, 0.32f, 24, 1);
    glPopMatrix();

    // Sisi brim
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -0.02f);
        gluCylinder(quad, 0.32f, 0.34f, 0.02f, 24, 1);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void drawZombie() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glPushMatrix();
            // Kepala
            glTranslatef(0.0f, 0.3f, 0.0f);
            glRotatef((GLfloat)head, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.0f, 0.3f, 0.0f);
            glScalef(0.9f, 0.9f, 0.9f);

            glShadeModel(GL_FLAT);
            drawHeadZombie();

            // Topi
            glPushMatrix();
                glTranslatef(0.0f, 0.25f, 0.0f);
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                glShadeModel(GL_SMOOTH);
                drawBucketHat();
                glShadeModel(GL_FLAT);
            glPopMatrix();
        glPopMatrix();

        // Badan
        glPushMatrix();
            glShadeModel(GL_FLAT);
            glTranslatef(0, 0.07f, 0);
            drawBodyZombie();
        glPopMatrix();

        // Tangan Kanan
        glPushMatrix();
            glTranslatef(0.36f, 0.32f, -0.1f);
            glRotatef((GLfloat)rightShoulder, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.32f, 0.0f);
            drawArmZombie();
        glPopMatrix();

        // Tangan Kiri
        glPushMatrix();
            glTranslatef(-0.36f, 0.32f, -0.1f);
            glRotatef((GLfloat)leftShoulder, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.32f, 0.0f);
            drawArmZombie();
        glPopMatrix();

        // Kaki Kanan
        glPushMatrix();
            glTranslatef(0.125f, -0.65f, 0.0f);
            drawLegZombie();
        glPopMatrix();

        // Kaki Kiri
        glPushMatrix();
            glTranslatef(-0.125f, -0.65f, 0.0f);
            drawLegZombie();
        glPopMatrix();
    glPopMatrix();
}

void zombieAttack() {
    /* === KAMUS LOKAL === */
    float dx, dy, dz;
    float dist;
    int currentTime;

    /* === ALGORITMA === */
    if (!isDead) {
        // Kalkulasi jarak
        dx = posX - zomX;
        dy = posY - zomY;
        dz = posZ - zomZ;
        dist = sqrt((dx*dx) + (dy*dy) + (dz*dz));

        // Logika menyerang
        currentTime = glutGet(GLUT_ELAPSED_TIME);
        if (dist <= 0.25f) {
            if (currentTime - lastDamageTime > damageDelay) {
                playerHealth -= 10;
                lastDamageTime = currentTime;

                // State player meninggal 
                if (playerHealth <= 0) {
                    isDead = true;
                }
            }
        }
    } else { // isDead == true
        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    }
}

void updateZombie() {
    /* === KAMUS LOKAL === */
    float dx, dz;
    float dist;
    float desiredX, desiredZ;

    /* === ALGORITMA === */
    if (isDead) {
        return;
    }

    // Arah dari zombie ke player (cukup bidang XZ)
    dx = posX - zomX;
    dz = posZ - zomZ;
    dist = sqrt(dx * dx + dz * dz);

    // Kalau sudah sangat dekat, berhenti
    if (dist <= stopDistance) {
        return;
    }

    // Normalisasi arah
    desiredX = dx / dist;
    desiredZ = dz / dist;

    // Gerakkan zombie ke arah player
    zomX += desiredX * zomSpeed;
    zomZ += desiredZ * zomSpeed;

    // Rotasi zombie agar menghadap player
    zomAngleY = atan2(-dx, -dz) * 180.0f / M_PI;
}

// ===================== PISTOL ===================== //
void drawPistol() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    setMaterial(0.15f, 0.15f, 0.15f);
    drawCuboid(0.08f, 0.35f, 0.12f);

    glPushMatrix();
        glTranslatef(0.0f, -0.18f, 0.0f);
        setMaterial(0.1f, 0.1f, 0.1f);
        drawCuboid(0.09f, 0.04f, 0.14f);
    glPopMatrix();

    glPushMatrix();
            glTranslatef(0.0f, 0.20f, -0.15f);
            setMaterial(0.3f, 0.3f, 0.3f);
            drawCuboid(0.1f, 0.12f, 0.5f);

        glPushMatrix();
            glTranslatef(0.0f, 0.08f, 0.2f);
            setMaterial(0.1f, 0.1f, 0.1f);
            drawCuboid(0.04f, 0.04f, 0.04f);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, 0.07f, -0.22f);
            drawCuboid(0.02f, 0.03f, 0.03f);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, -0.08f, 0.05f);
            drawCuboid(0.03f, 0.12f, 0.03f);
            glTranslatef(0.0f, -0.05f, 0.05f);
            drawCuboid(0.03f, 0.03f, 0.1f);
        glPopMatrix();
    glPopMatrix();
}

// ===================== HAND HUD ===================== //
void drawHand(float xPos, float rotation, float currentRecoil) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glLoadIdentity();
        glTranslatef(xPos, -0.6f + currentRecoil, -1.5f);
        glRotatef(-1, 1, 0, 0);
        glRotatef(rotation, 0, 1, 0);

        setMaterial(0.8f, 0.6f, 0.4f);
        drawCuboid(0.3f, 0.3f, 0.8f);

        if (xPos > 0.0f) {
            glPushMatrix();
                glTranslatef(0.0f, 0.15f, -0.35f);
                glRotatef(1, 1, 0, 0);
                drawPistol();
            glPopMatrix();
        }
    glPopMatrix();
}

// ===================== INIT ===================== //
void init() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat global_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    GLfloat spec[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

    // Anti Aliasing
    glEnable(GLUT_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ===================== DISPLAY ===================== //
void renderText(float x, float y, void* font, const char* string) {
    /* === KAMUS LOKAL === */
    const char* c;

    /* === ALGORITMA === */
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void display() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch(viewMode) {
        case 1:
            glRotatef(camAngleX, 1, 0, 0);
            glRotatef(camAngleY, 0, 1, 0);
            glTranslatef(-posX, -posY, -posZ);
            break;

        case 2: 
            gluLookAt(8.0f + posX, posY, 8.0f + posZ,
                    0.0f, 0.0f, -8.0f,
                    0.0f, 1.0f, 0.0f);
            glRotatef(camAngleY, 0, 1, 0);
            break;

        case 3:
            gluLookAt(8.0f + posX, 8.0f + posY, 8.0f + posZ,
                    0.0f, 0.0f, -8.0f,
                    0.0f, 1.0f, 0.0f);
            glRotatef(camAngleX, 1, 0, 0);
            glRotatef(camAngleY, 0, 1, 0);
            break;
    }

    GLfloat light_pos[] = { sunX, sunY, sunZ, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Matahari
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(sunX, sunY, sunZ);
        glColor3f(1.0f, 1.0f, 0.0f);
        glutSolidSphere(3.0f, 20, 20);
        glEnable(GL_LIGHTING);
    glPopMatrix();

    // Lantai
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(0.99f, 0.99f, 0.99f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-100, -1, -100);
        glVertex3f(-100, -1, 100);
        glVertex3f(100, -1, 100);
        glVertex3f(100, -1, -100);
    glEnd();

    // Zombie
    glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
    glPushMatrix();
    	glTranslatef(zomX, zomY, zomZ);
    	glRotatef(zomAngleY + 180.0f, 0.0f, 1.0f, 0.0f);
    	glScalef(0.2f, 0.2f, 0.2f);
    	drawZombie();
	glPopMatrix();

    // HUD tangan hanya untuk First Person
    if (viewMode == 1) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_COLOR_MATERIAL);
        drawHand(0.7f, 15.0f, recoil);
    }

    // Game Over
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 1280, 0, 720);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            char healthText[50];
            sprintf(healthText, "Health: %.0f", playerHealth);

            if (playerHealth >= 70) {
                glColor3f(0.0f, 1.0f, 0.0f);
            } else if (playerHealth > 30 && playerHealth < 70) {
                glColor3f(1.0f, 1.0f, 0.0f);
            }  else {
                glColor3f(1.0f, 0.0f ,0.0f);
            }

            renderText(50, 680, GLUT_BITMAP_9_BY_15, healthText);

            if (isDead) {
                glColor3f(1.0f, 1.0f, 1.0f);
                renderText(580, 360, GLUT_BITMAP_9_BY_15, "Game Over");
            }
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

// ===================== KEYBOARD ===================== //
void keyboard(unsigned char key, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (key == 27) {
        exit(0);
    }
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    keys[key] = false;
}

void updateMovement() {
    /* === KAMUS LOKAL === */
    float fwdX, fwdZ;
    float rightZ, rightX;
    float rad, speed;

    /* === ALGORITMA === */
    rad = camAngleY * M_PI / 180.0f;
    fwdX = sin(rad);
    fwdZ = -cos(rad);
    rightX = cos(rad);
    rightZ = sin(rad);
    speed = 0.02f;

    // Movement
    if (keys['w']) { 
        posX += fwdX * speed;
        posZ += fwdZ * speed;
    }
    if (keys['s']) {
        posX -= fwdX * speed;
        posZ -= fwdZ * speed;
    }
    if (keys['a']) {
        posX -= rightX * speed;
        posZ -= rightZ * speed;
    }
    if (keys['d']) {
        posX += rightX * speed;
        posZ += rightZ * speed;
    }

    // View
    if (keys['1']) {
        viewMode = 1;
    }
    if (keys['2']) {
        viewMode = 2;
    }
    if (keys['3']) {
        viewMode = 3;
    }
    // Tambah Orthographic nanti // 
}

// ===================== TIMER ===================== //
void timer(int v) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    updateMovement();
    updateZombie();
    zombieAttack();
    if (isShooting) {
        recoil -= 0.05f;

        if (recoil < -0.3f) {
            recoil = 0;
        }
    } else {
        recoil = 0;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ===================== MOUSE ===================== //
void mouseMove(int x, int y) {
    /* === KAMUS LOKAL === */
    int cx,cy,dx,dy;
    static int lastX = 500;
    static int lastY = 350;
    static bool first = true;

    /* === ALGORITMA === */
    cx = 500;
    cy = 350;
    
    if (first) {
        lastX = x;
        lastY = y;
        first = false;
    } else {
        dx = x - lastX;
        dy = y - lastY;
        lastX = x;
        lastY = y;
        camAngleY += dx * 0.2f;
        camAngleX += dy * 0.2f;

        if (camAngleX >  85.0f) {
            camAngleX =  85.0f;
        }
        if (camAngleX < -85.0f) {
            camAngleX = -85.0f;
        }

        // Warp hanya saat deket di ujung, ngga setiap frame
        if (x < 100 || x > 900 || y < 100 || y > 600) {
            lastX = cx;
            lastY = cy;
            glutWarpPointer(cx, cy);
        }
    }
}

void mouseClick(int b, int s, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (b == GLUT_LEFT_BUTTON) {
        isShooting = (s == GLUT_DOWN);
    }
}

// ===================== RESHAPE ===================== //
void reshape(int width, int height) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (height == 0) {
        height = 1;
    }
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat) width / (GLfloat) height, 0.01, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ===================== MAIN ===================== //
int main(int argc, char** argv) {
    /* === KAMUS === */
    int screenWidth, screenHeight;
    int windowWidth, windowHeight;

    /* === ALGORITMA === */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  | GLUT_MULTISAMPLE);

    windowWidth = 1280;
    windowHeight = 720;
    glutInitWindowSize(windowWidth, windowHeight);
    screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
    glutCreateWindow("Minecraft FPS");

    glutWarpPointer(500, 350);
    init();
    rightShoulder = -90;
    leftShoulder = -90;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMove);
    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);
    glutTimerFunc(0, timer, 0);
    glutKeyboardUpFunc(keyboardUp);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();
    return 0;
}
