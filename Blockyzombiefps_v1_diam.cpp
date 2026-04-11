/*  Nama File   :   BlockyZombieFPS.cpp
    Matakuliah  :   Grafik dan Teknik Interaktif 
    Kelompok    :   6
    Anggota     :   Rafif Setya Imaduddin  24060124130115
                    Raffi Arditama         24060124120020
                    Felicia Evelina        24060124120012
                    Anggita Kirana Puspa   24060124130064
    Keyboard    :   w = Jalan ke Depan
                    a = Jalan ke Kiri
                    d = Jalan ke Kanan
                    s = Jalan ke Belakang
                    1 = 1-Point Perspective (FPS Default)
                    2 = 2-Point Perspective (Third Person Samping)
                    3 = 3-Point Perspective (Third Person Atas)
                    4 = Orthographic
                    ESC = Keluar
                    Mouse Left Click = Menembak
*/

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
float camDist = 3.0f;
float eyeOffset = 0.13f;
bool isShooting = false;

/* === POSISI PEMAIN === */
float posX = 0.0f;
float posY = -0.37f;   // tinggi mata pemain dari lantai (lantai di Y=-1)
float posZ = 5.0f;

/* === PLAYER & GAME STATE === */
float playerHealth = 100;
bool isDead = false;

/* === DAMAGE COOLDOWN === */
int lastDamageTime = 0;
int damageDelay = 1000;

/* === POSISI ZOMBIE === */
// Lantai Y = -1.0
// Zombie scale 0.5x, titik bawah sepatu di Y = -0.72 (sebelum scale)
// Setelah scale 0.5: -0.72 * 0.5 = -0.36 → zomY = -1.0 + 0.36 = -0.64
float zomX = 0.0f;
float zomY = -0.52f;
float zomZ = 3.0f;

/* === POSE ZOMBIE === */
static int head = 0;
static int rightShoulder = -90;
static int leftShoulder  = -90;

/* === GERAK & ARAH ZOMBIE === */
float zomSpeed = 0.01f;
float zomAngleY = 0.0f;
float stopDistance = 0.5f;

/* === POSISI MATAHARI === */
float sunX =   0.0f;
float sunY =  15.0f;
float sunZ = -30.0f;

/* === WINDOW SIZE === */
int winWidth  = 1280;
int winHeight =  720;

// ===================== MATERIAL ===================== //
void setMaterial(float r, float g, float b) {
    GLfloat mat_ambient[] = {r * 0.3f, g * 0.3f, b * 0.3f, 1.0f};
    GLfloat mat_diffuse[] = {r, g, b, 1.0f};
    GLfloat mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialf (GL_FRONT, GL_SHININESS, 50.0f);
}

// ===================== CUBOID ===================== //
void drawCuboid(float w, float h, float d,
                float fR, float fG, float fB,
                float sR, float sG, float sB,
                float tR, float tG, float tB,
                float bR, float bG, float bB) {
    float x = w*0.5f, y = h*0.5f, z = d*0.5f;
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);  glColor3f(fR,fG,fB);
        glVertex3f(-x,-y,z); glVertex3f(x,-y,z); glVertex3f(x,y,z); glVertex3f(-x,y,z);
        glNormal3f(0,0,-1); glColor3f(fR*.8f,fG*.8f,fB*.8f);
        glVertex3f(x,-y,-z); glVertex3f(-x,-y,-z); glVertex3f(-x,y,-z); glVertex3f(x,y,-z);
        glNormal3f(-1,0,0); glColor3f(sR,sG,sB);
        glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z); glVertex3f(-x,y,z); glVertex3f(-x,y,-z);
        glNormal3f(1,0,0);  glColor3f(sR,sG,sB);
        glVertex3f(x,-y,z); glVertex3f(x,-y,-z); glVertex3f(x,y,-z); glVertex3f(x,y,z);
        glNormal3f(0,1,0);  glColor3f(tR,tG,tB);
        glVertex3f(-x,y,z); glVertex3f(x,y,z); glVertex3f(x,y,-z); glVertex3f(-x,y,-z);
        glNormal3f(0,-1,0); glColor3f(bR,bG,bB);
        glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z); glVertex3f(x,-y,z); glVertex3f(-x,-y,z);
    glEnd();
}

void drawCuboid(float w, float h, float d) {
    float x = w*0.5f, y = h*0.5f, z = d*0.5f;
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);  glVertex3f(-x,-y,z); glVertex3f(x,-y,z); glVertex3f(x,y,z); glVertex3f(-x,y,z);
        glNormal3f(0,0,-1); glVertex3f(x,-y,-z); glVertex3f(-x,-y,-z); glVertex3f(-x,y,-z); glVertex3f(x,y,-z);
        glNormal3f(-1,0,0); glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z); glVertex3f(-x,y,z); glVertex3f(-x,y,-z);
        glNormal3f(1,0,0);  glVertex3f(x,-y,z); glVertex3f(x,-y,-z); glVertex3f(x,y,-z); glVertex3f(x,y,z);
        glNormal3f(0,1,0);  glVertex3f(-x,y,z); glVertex3f(x,y,z); glVertex3f(x,y,-z); glVertex3f(-x,y,-z);
        glNormal3f(0,-1,0); glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z); glVertex3f(x,-y,z); glVertex3f(-x,-y,z);
    glEnd();
}

void drawFrontPatch(float px, float py, float pw, float ph, float z, float r, float g, float b) {
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(px,py,z); glVertex3f(px+pw,py,z);
        glVertex3f(px+pw,py+ph,z); glVertex3f(px,py+ph,z);
    glEnd();
}

// ===================== BAGIAN ZOMBIE ===================== //
void drawHeadZombie() {
    float w=0.5f, h=0.45f, d=0.5f, z=d*0.5f+0.001f;
    drawCuboid(w,h,d, 0.34f,0.52f,0.24f, 0.38f,0.56f,0.28f, 0.40f,0.58f,0.30f, 0.28f,0.42f,0.20f);
    drawFrontPatch(-0.17f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch( 0.06f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch(-0.06f,-0.07f,0.12f,0.07f,z, 0.22f,0.34f,0.16f);
    drawFrontPatch(-0.06f,-0.15f,0.12f,0.03f,z, 0.20f,0.30f,0.14f);
}

void drawBodyZombie() {
    float w=0.5f, h=0.67f, d=0.25f, z=d*0.5f+0.001f;
    drawCuboid(w,h,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f, 0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
    drawFrontPatch(-0.1f, 0.27f,0.2f, 0.06f,z, 0.30f,0.32f,0.12f);
    drawFrontPatch(-0.05f,0.21f,0.1f, 0.06f,z, 0.30f,0.32f,0.12f);
}

void drawArmZombie() {
    float w=0.25f, h=0.65f, d=0.25f, sleeveH=0.20f, armH=h-sleeveH;
    glPushMatrix();
        glTranslatef(0,(h-sleeveH)*0.5f,0);
        drawCuboid(w,sleeveH,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f, 0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0,-(sleeveH*0.5f),0);
        drawCuboid(w,armH,d, 0.46f,0.60f,0.35f, 0.42f,0.56f,0.32f, 0.48f,0.62f,0.37f, 0.38f,0.50f,0.28f);
    glPopMatrix();
}

void drawLegZombie() {
    float w=0.25f, h=0.7f, d=0.18f;
    glPushMatrix();
        glTranslatef(0,0.05f,0);
        drawCuboid(w,h,d, 0.34f,0.27f,0.78f, 0.30f,0.24f,0.70f, 0.37f,0.30f,0.82f, 0.30f,0.24f,0.70f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0,-0.30f,0.04f);
        drawCuboid(0.27f,0.12f,0.28f, 0.45f,0.45f,0.45f, 0.40f,0.40f,0.40f, 0.50f,0.50f,0.50f, 0.30f,0.30f,0.30f);
    glPopMatrix();
}

void drawBucketHat() {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    setMaterial(0.85f,0.85f,0.85f);
    glPushMatrix(); gluCylinder(q,0.22f,0.20f,0.22f,24,4);
        glPushMatrix(); glTranslatef(0,0,0.22f); gluDisk(q,0,0.22f,24,1); glPopMatrix();
    glPopMatrix();
    glPushMatrix(); gluDisk(q,0.20f,0.34f,24,1); glPopMatrix();
    glPushMatrix(); glTranslatef(0,0,-0.02f); gluDisk(q,0.18f,0.32f,24,1); glPopMatrix();
    glPushMatrix(); glTranslatef(0,0,-0.02f); gluCylinder(q,0.32f,0.34f,0.02f,24,1); glPopMatrix();
    gluDeleteQuadric(q);
}

void drawZombie() {
    glPushMatrix();
        glPushMatrix();
            glTranslatef(0,0.3f,0); glRotatef((GLfloat)head,0,0,1);
            glTranslatef(0,0.3f,0); glScalef(0.9f,0.9f,0.9f);
            glShadeModel(GL_FLAT); drawHeadZombie();
            glPushMatrix();
                glTranslatef(0,0.25f,0); glRotatef(-90,1,0,0);
                glShadeModel(GL_SMOOTH); drawBucketHat(); glShadeModel(GL_FLAT);
            glPopMatrix();
        glPopMatrix();
        glPushMatrix(); glTranslatef(0,0.07f,0); drawBodyZombie(); glPopMatrix();
        glPushMatrix(); glTranslatef(0.36f,0.32f,-0.1f);
            glRotatef((GLfloat)rightShoulder,1,0,0); glTranslatef(0,-0.32f,0); drawArmZombie();
        glPopMatrix();
        glPushMatrix(); glTranslatef(-0.36f,0.32f,-0.1f);
            glRotatef((GLfloat)leftShoulder,1,0,0); glTranslatef(0,-0.32f,0); drawArmZombie();
        glPopMatrix();
        glPushMatrix(); glTranslatef( 0.125f,-0.65f,0); drawLegZombie(); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.125f,-0.65f,0); drawLegZombie(); glPopMatrix();
    glPopMatrix();
}

// ===================== BODY PLAYER (untuk mode 2 & 3) ===================== //
// Tubuh player sederhana agar terlihat di third-person view
// Digambar di posisi (posX, posY, posZ) dengan rotasi menghadap arah kamera
void drawPlayer(float size) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glTranslatef(posX, posY, posZ);
        glScalef(size, size, size);
        glRotatef(camAngleY, 0, 1, 0); // menghadap arah pandang pemain

        // Badan
        glPushMatrix();
            glTranslatef(0, -0.2f, 0);
            drawCuboid(0.5f, 0.7f, 0.25f,
                0.20f, 0.40f, 0.80f,   // front: biru
                0.15f, 0.35f, 0.70f,   // side
                0.25f, 0.45f, 0.85f,   // top
                0.10f, 0.30f, 0.65f);  // bottom
        glPopMatrix();

        // Kepala
        glPushMatrix();
            glTranslatef(0, 0.35f, 0);
            drawCuboid(0.4f, 0.4f, 0.4f,
                0.85f, 0.65f, 0.50f,   // front: warna kulit
                0.80f, 0.60f, 0.45f,
                0.88f, 0.68f, 0.52f,
                0.75f, 0.55f, 0.40f);
        glPopMatrix();

        // Kaki kanan
        glPushMatrix();
            glTranslatef(0.13f, -0.85f, 0);
            drawCuboid(0.22f, 0.55f, 0.22f,
                0.15f, 0.15f, 0.60f,
                0.12f, 0.12f, 0.55f,
                0.18f, 0.18f, 0.65f,
                0.10f, 0.10f, 0.50f);
        glPopMatrix();

        // Kaki kiri
        glPushMatrix();
            glTranslatef(-0.13f, -0.85f, 0);
            drawCuboid(0.22f, 0.55f, 0.22f,
                0.15f, 0.15f, 0.60f,
                0.12f, 0.12f, 0.55f,
                0.18f, 0.18f, 0.65f,
                0.10f, 0.10f, 0.50f);
        glPopMatrix();

        // Lengan kanan
        glPushMatrix();
            glTranslatef(0.35f, -0.1f, 0);
            drawCuboid(0.20f, 0.60f, 0.20f,
                0.85f, 0.65f, 0.50f,
                0.80f, 0.60f, 0.45f,
                0.88f, 0.68f, 0.52f,
                0.75f, 0.55f, 0.40f);
        glPopMatrix();

        // Lengan kiri
        glPushMatrix();
            glTranslatef(-0.35f, -0.1f, 0);
            drawCuboid(0.20f, 0.60f, 0.20f,
                0.85f, 0.65f, 0.50f,
                0.80f, 0.60f, 0.45f,
                0.88f, 0.68f, 0.52f,
                0.75f, 0.55f, 0.40f);
        glPopMatrix();
    glPopMatrix();
}

void zombieAttack() {
    float dx = posX-zomX, dy = posY-zomY, dz = posZ-zomZ;
    float dist = sqrt(dx*dx + dy*dy + dz*dz);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (!isDead) {
        if (dist <= stopDistance) {
            if (currentTime - lastDamageTime > damageDelay) {
                playerHealth -= 10;
                lastDamageTime = currentTime;
                if (playerHealth <= 0) isDead = true;
            }
        }
    } else {
        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    }
}

// ===================== PISTOL ===================== //
void drawPistol() {
    setMaterial(0.15f,0.15f,0.15f); drawCuboid(0.08f,0.35f,0.12f);
    glPushMatrix(); glTranslatef(0,-0.18f,0); setMaterial(0.1f,0.1f,0.1f); drawCuboid(0.09f,0.04f,0.14f); glPopMatrix();
    glPushMatrix();
        glTranslatef(0,0.20f,-0.15f); setMaterial(0.3f,0.3f,0.3f); drawCuboid(0.1f,0.12f,0.5f);
        glPushMatrix(); glTranslatef(0,0.08f,0.2f); setMaterial(0.1f,0.1f,0.1f); drawCuboid(0.04f,0.04f,0.04f); glPopMatrix();
        glPushMatrix(); glTranslatef(0,0.07f,-0.22f); drawCuboid(0.02f,0.03f,0.03f); glPopMatrix();
        glPushMatrix(); glTranslatef(0,-0.08f,0.05f);
            drawCuboid(0.03f,0.12f,0.03f); glTranslatef(0,-0.05f,0.05f); drawCuboid(0.03f,0.03f,0.1f);
        glPopMatrix();
    glPopMatrix();
}

// ===================== HAND HUD ===================== //
void drawHand(float xPos, float rotation, float currentRecoil) {
    glPushMatrix();
        glLoadIdentity();
        glTranslatef(xPos, -0.6f + currentRecoil, -1.5f);
        glRotatef(-1,1,0,0); glRotatef(rotation,0,1,0);
        setMaterial(0.8f,0.6f,0.4f); drawCuboid(0.3f,0.3f,0.8f);
        if (xPos > 0.0f) {
            glPushMatrix();
                glTranslatef(0,0.15f,-0.35f); glRotatef(1,1,0,0); drawPistol();
            glPopMatrix();
        }
    glPopMatrix();
}

// ===================== INIT ===================== //
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    GLfloat global_ambient[] = {0.6f, 0.6f, 0.6f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    GLfloat spec[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
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
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // -------------------------------------------------------
    // SETUP KAMERA berdasarkan viewMode
    //
    // Mode 1 - 1-Point Perspective (FPS):
    //   Satu titik hilang ke depan. Sudut pandang orang pertama.
    //
    // Mode 2 - 2-Point Perspective (Third Person Samping):
    //   Kamera dari samping-belakang pemain. Dua titik hilang
    //   di kiri & kanan horizon. Body player terlihat.
    //
    // Mode 3 - 3-Point Perspective (Third Person Atas):
    //   Kamera dari atas-belakang pemain. Tiga titik hilang.
    //   Body player terlihat dari sudut tinggi.
    //
    // Mode 4 - Orthographic:
    //   Kamera atas-belakang seperti mode 3 tapi tanpa distorsi.
    //   Body player terlihat, ukuran objek seragam (tidak mengecil
    //   saat jauh). Bisa bergerak mengikuti posisi pemain.
    // -------------------------------------------------------
    switch(viewMode) {
        case 1:
            // FPS: rotasi di titik asal lalu geser ke posisi pemain
            glRotatef(camAngleX, 1, 0, 0);
            glRotatef(camAngleY, 0, 1, 0);
            glTranslatef(-posX, -(posY + eyeOffset), -posZ);
            break;

        case 2: {
            // Third person samping: kamera 3 unit di belakang & 2 unit di atas pemain
            float rad = camAngleY * M_PI / 180.0f;
            float eyeX = posX - sin(rad) * camDist;
            float eyeY = posY;
            float eyeZ = posZ + cos(rad) * camDist;
            gluLookAt(eyeX, eyeY, eyeZ,
                        posX, posY, posZ,
                        0, 1, 0);
            break;
        }

        case 3: {
            // Third person atas: kamera 4 unit di belakang & 4 unit di atas
            float rad = camAngleY * M_PI / 180.0f;
            float eyeX = posX - sin(rad) * camDist;
            float eyeY = posY + camDist;
            float eyeZ = posZ + cos(rad) * camDist;
            gluLookAt(eyeX, eyeY, eyeZ,
                        posX, posY, posZ,
                        0, 1, 0);
            break;
        }

        case 4: {
            // Orthographic: kamera atas-belakang mengikuti pemain
            // Projection matrix sudah glOrtho di applyProjection()
            float rad = camAngleY * M_PI / 180.0f;
            float camDist = 10.0f;
            float eyeX = posX - sin(rad) * camDist;
            float eyeY = posY + 10.0f;
            float eyeZ = posZ + cos(rad) * camDist;
            gluLookAt(eyeX, eyeY, eyeZ,
                        posX, posY, posZ,
                        0, 1, 0);
            break;
        }
    }

    GLfloat light_pos[] = {sunX, sunY, sunZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Matahari
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(sunX, sunY, sunZ);
        glColor3f(1,1,0); glutSolidSphere(3,20,20);
        glEnable(GL_LIGHTING);
    glPopMatrix();

    // Lantai hijau
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(0.2f, 0.7f, 0.2f);
    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glVertex3f(-100,-1,-100); glVertex3f(-100,-1,100);
        glVertex3f(100,-1,100);   glVertex3f(100,-1,-100);
    glEnd();

    // Zombie
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glPushMatrix();
        glTranslatef(zomX, zomY, zomZ);
        glRotatef(zomAngleY + 180.0f, 0, 1, 0);
        glScalef(0.5f, 0.5f, 0.5f);  // scale 0.5 agar setara tinggi player
        drawZombie();
    glPopMatrix();

    // Body player hanya di mode 2, 3, 4 (third person)
    if (viewMode != 1) {
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        drawPlayer(0.5f);
    }

    // HUD tangan hanya di mode 1 (FPS)
    if (viewMode == 1) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_COLOR_MATERIAL);
        drawHand(0.7f, 15.0f, recoil);
    }

    // ===================== HUD 2D ===================== //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 1280, 0, 720);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            // Label mode aktif
            glColor3f(1,1,1);
            switch(viewMode) {
                case 1: renderText(50,690,GLUT_BITMAP_9_BY_15,"[1] 1-Point Perspective (FPS)"); break;
                case 2: renderText(50,690,GLUT_BITMAP_9_BY_15,"[2] 2-Point Perspective (3rd Person)"); break;
                case 3: renderText(50,690,GLUT_BITMAP_9_BY_15,"[3] 3-Point Perspective (3rd Person Atas)"); break;
                case 4: renderText(50,690,GLUT_BITMAP_9_BY_15,"[4] Orthographic"); break;
            }

            // Panduan kontrol
            glColor3f(0.9f,0.9f,0.9f);
            renderText(1050,700,GLUT_BITMAP_8_BY_13,"W/A/S/D     : Gerak");
            renderText(1050,685,GLUT_BITMAP_8_BY_13,"Mouse       : Lihat");
            renderText(1050,670,GLUT_BITMAP_8_BY_13,"Left-Click  : Tembak");
            renderText(1050,625,GLUT_BITMAP_8_BY_13,"=/-         : Zoom-In/Out");
            renderText(1050,655,GLUT_BITMAP_8_BY_13,"1/2/3/4     : Proyeksi");
            renderText(1050,640,GLUT_BITMAP_8_BY_13,"ESC         : Keluar");

            // Health bar
            float barW=300, barH=25, bx=50, by=650;
            float hpPerc = playerHealth/100.0f;
            if (hpPerc < 0) hpPerc = 0;

            glColor3f(0.1f,0.1f,0.1f);
            glBegin(GL_QUADS);
                glVertex2f(bx,by); glVertex2f(bx+barW,by);
                glVertex2f(bx+barW,by+barH); glVertex2f(bx,by+barH);
            glEnd();

            if      (playerHealth > 70) glColor3f(0,1,0);
            else if (playerHealth > 30) glColor3f(1,1,0);
            else                        glColor3f(1,0,0);
            glBegin(GL_QUADS);
                glVertex2f(bx,by); glVertex2f(bx+barW*hpPerc,by);
                glVertex2f(bx+barW*hpPerc,by+barH); glVertex2f(bx,by+barH);
            glEnd();

            glColor3f(1,1,1); glLineWidth(2);
            glBegin(GL_LINE_LOOP);
                glVertex2f(bx,by); glVertex2f(bx+barW,by);
                glVertex2f(bx+barW,by+barH); glVertex2f(bx,by+barH);
            glEnd();

            char healthText[50];
            sprintf(healthText, "HP : %.0f%%", playerHealth);
            if      (playerHealth > 70) glColor3f(1,1,1);
            else if (playerHealth > 30) glColor3f(1,1,0);
            else                        glColor3f(1,0,0);
            renderText(bx, by-25, GLUT_BITMAP_9_BY_15, healthText);

            glColor3f(1.0f, 1.0f, 1.0f);
            char zoomCam[50];
            sprintf(zoomCam, "Zoom Percentage: %0.1f%%", camDist);
            renderText(bx, by-50, GLUT_BITMAP_9_BY_15, zoomCam);

            if (isDead) {
                glColor3f(1,1,1);
                glLineWidth(5.0f);
                renderText(580, 360, GLUT_BITMAP_9_BY_15, "Game Over");
            }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}

void applyProjection() {
    float aspect = (float)winWidth / (float)winHeight;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (viewMode == 4) {
        // Orthographic: Objek tidak mengecil saat jauh.
        // Ganti angka 6.0 dengan variabel 'zoom' jika ingin bisa zoom in/out
        glOrtho(-camDist * aspect, camDist * aspect, -camDist, camDist, 0.1, 1000.0);
    } else {
        // Perspective: Mode 1, 2, 3
        gluPerspective(45.0, aspect, 0.01, 200.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

// ===================== KEYBOARD ===================== //
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    keys[key] = true;
}
void keyboardUp(unsigned char key, int x, int y) { keys[key] = false; }

void updateMovement() {
    float rad = camAngleY * M_PI / 180.0f;
    float fwdX=sin(rad), fwdZ=-cos(rad), rightX=cos(rad), rightZ=sin(rad), speed=0.02f;
    if (keys['w']) { posX+=fwdX*speed;  posZ+=fwdZ*speed;  }
    if (keys['s']) { posX-=fwdX*speed;  posZ-=fwdZ*speed;  }
    if (keys['a']) { posX-=rightX*speed; posZ-=rightZ*speed; }
    if (keys['d']) { posX+=rightX*speed; posZ+=rightZ*speed; }

    int pastMode = viewMode;
    float pastDist = camDist;
    if (keys['1']) viewMode = 1;
    if (keys['2']) viewMode = 2;
    if (keys['3']) viewMode = 3;
    if (keys['4']) viewMode = 4;
    if (keys['='] && camDist > 1.1f && (viewMode != 1)) {
        camDist -= 0.1f;
    }
    if (keys['-'] && camDist < 7.0f && viewMode != 1) {
        camDist += 0.1f;
    }

    if (pastMode != viewMode || pastDist != camDist) {
        applyProjection();
    }
}

// ===================== TIMER ===================== //
void timer(int v) {
    updateMovement();
    zombieAttack();
    if (isShooting) { recoil -= 0.05f; if (recoil < -0.3f) recoil = 0; }
    else recoil = 0;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ===================== MOUSE ===================== //
void mouseMove(int x, int y) {
    int cx=500, cy=350, dx, dy;
    static int lastX=500, lastY=350;
    static bool first=true;
    if (first) { lastX=x; lastY=y; first=false; return; }
    dx=x-lastX; dy=y-lastY; lastX=x; lastY=y;
    camAngleY += dx*0.2f; camAngleX += dy*0.2f;
    if (camAngleX> 85) camAngleX= 85;
    if (camAngleX<-85) camAngleX=-85;
    if (x<100||x>900||y<100||y>600) { lastX=cx; lastY=cy; glutWarpPointer(cx,cy); }
}

void mouseClick(int b, int s, int x, int y) {
    if (b == GLUT_LEFT_BUTTON) isShooting = (s == GLUT_DOWN);
}

// ===================== PROYEKSI ===================== //
void reshape(int width, int height) {
    if (height == 0) height = 1;
    winWidth=width; winHeight=height;
    glViewport(0, 0, width, height);
    applyProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ===================== MAIN ===================== //
int main(int argc, char** argv) {
    int screenWidth, screenHeight, windowWidth=1280, windowHeight=720;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    screenWidth=glutGet(GLUT_SCREEN_WIDTH);
    screenHeight=glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth-windowWidth)/2, (screenHeight-windowHeight)/2);
    glutCreateWindow("Blocky Zombie FPS");
    glutWarpPointer(500, 350);
    init();
    rightShoulder=-90; leftShoulder=-90;
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMove);
    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);
    glutTimerFunc(0, timer, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();
    return 0;
}
