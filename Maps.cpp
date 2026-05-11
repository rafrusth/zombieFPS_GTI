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
                    = / - = Zoom In / Out (mode 2, 3, 4)
                    ESC = Keluar
                    Mouse Left Click = Menembak
*/

#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>   
#include <cstdlib> 

// ===================== ABSTRACT DATA TYPES ===================== //
// Fisika kolision (Kotak based)
struct AABB {
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
};

// ===================== KAMUS GLOBAL ===================== //
bool keys[256] = {false};

/* === VARIABEL KAMERA === */
int viewMode    = 1;
float recoil    = 0.0f;
float camAngleX = 0.0f;
float camAngleY = 0.0f;
float camDist   = 3.0f;
float eyeOffset = 0.13f;
bool isShooting = false;

/* === POSISI PEMAIN === */
float posX = 0.0f;
float posY = -0.37f;
float posZ = 5.0f;

/* === PLAYER & GAME STATE === */
float playerHealth = 100;
bool isDead = false;

/* === DAMAGE COOLDOWN === */
int lastDamageTime = 0;
int damageDelay = 1000;

/* === POSISI OBSTACLE === */
// Meja
int numTables;
const int LIMIT_TABLES = 20;
float tableX[LIMIT_TABLES];
float tableZ[LIMIT_TABLES];
float tableY;

// Ukuran meja
float tableW = 0.5f;
float tableH = 0.08f; 
float tableD = 1.0f;

// Ukuran kaki meja
float tableFootW = 0.1f;
float tableFootH = 0.48f; 
float tableFootD = 0.1f;

/* === POSISI ZOMBIE === */
int numZombies;                 
const int LIMIT_ZOMBIES = 20;
float zomX[LIMIT_ZOMBIES];
float zomZ[LIMIT_ZOMBIES];
float zomY = -0.52f;

/* === POSE ZOMBIE === */
static int head = 0;
static int rightShoulder = -90;
static int leftShoulder = -90;

/* === GERAK & ARAH ZOMBIE === */
float zomSpeed = 0.02f;
float zomAngleY[LIMIT_ZOMBIES];
float stopDistance = 0.4f;

/* === POSISI MATAHARI === */
float sunX = 0.0f;
float sunY = 15.0f;
float sunZ = -30.0f;

/* === WINDOW SIZE === */
int winWidth  = 1280;
int winHeight = 720;

// ===================== MINI-MAP CONFIG ===================== //
// Posisi dan ukuran kotak mini-map di layar (kiri atas)
// Untuk nambah jangkauan world yang ditampilkan ? naikkan MAP_WORLD_RANGE
// Untuk perbesar kotak visual ? naikkan MAP_SIZE
const float MAP_X           = 20.0f;   // jarak dari tepi kiri layar (px)
const float MAP_Y_FROM_TOP  = 220.0f;  // geser ke bawah supaya tidak nutupin HP bar
const float MAP_SIZE        = 160.0f;  // lebar & tinggi kotak (px)
const float MAP_WORLD_RANGE = 40.0f;   // jangkauan dunia yang dicakup map (-range..+range)
// Untuk tiap tipe objek baru ? tambahkan warna & ukuran dot-nya di drawMiniMap()

// ===================== AABB ===================== //
bool checkAABB(const AABB& a, const AABB& b) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    return  (a.minX <= b.maxX && a.maxX >= b.minX) &&
            (a.minY <= b.maxY && a.maxY >= b.minY) &&
            (a.minZ <= b.maxZ && a.maxZ >= b.minZ);
}

AABB getPlayerAABB() {
    /* === KAMUS LOKAL === */
    AABB box;

    /* === ALGORITMA === */
    box.minX = posX - 0.25f;
    box.maxX = posX + 0.25f;
    box.minY = posY;
    box.maxY = posY + 0.9f;
    box.minZ = posZ - 0.25f;
    box.maxZ = posZ + 0.25f;
    return box;
}

AABB getTableAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;

    /* === ALGORITMA === */
    box.minX = tableX[i] - tableW * 0.45f;
    box.maxX = tableX[i] + tableW * 0.45f;
    box.minY = tableY;
    box.maxY = tableY + tableH;
    box.minZ = tableZ[i] - tableD * 0.45f;
    box.maxZ = tableZ[i] + tableD * 0.45f;
    return box;
}

AABB getZombieAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;

    /* === ALGORITMA === */
    box.minX = zomX[i] - 0.25f;
    box.maxX = zomX[i] + 0.25f;
    box.minY = zomY;
    box.maxY = zomY + 0.9f;
    box.minZ = zomZ[i] - 0.25f;
    box.maxZ = zomZ[i] + 0.25f;
    return box;
}

// ===================== MATERIAL ===================== //
void setMaterial(float r, float g, float b) {
    /* === KAMUS LOKAL === */
    GLfloat mat_ambient[] = {r * 0.3f, g * 0.3f, b * 0.3f, 1.0f};
    GLfloat mat_diffuse[] = {r, g, b, 1.0f};
    GLfloat mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};

    /* === ALGORITMA === */
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf (GL_FRONT, GL_SHININESS, 50.0f);
}

// ===================== CUBOID ===================== //
void drawCuboid(float w, float h, float d,
                float fR, float fG, float fB,
                float sR, float sG, float sB,
                float tR, float tG, float tB,
                float bR, float bG, float bB) {
    /* === KAMUS LOKAL === */
    float x = w * 0.5f;
    float y = h * 0.5f;
    float z = d * 0.5f;

    /* === ALGORITMA === */
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
    /* === KAMUS LOKAL === */
    float x = w * 0.5f;
    float y = h * 0.5f;
    float z = d * 0.5f;

    /* === ALGORITMA === */
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
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(px,py,z); glVertex3f(px+pw,py,z);
        glVertex3f(px+pw,py+ph,z); glVertex3f(px,py+ph,z);
    glEnd();
}

// ===================== ZOMBIE ===================== //
void drawHeadZombie() {
    /* === KAMUS LOKAL === */
    float w = 0.5f;
    float h = 0.45f;
    float d = 0.5f;
    float z = d * 0.5f + 0.001f;

    /* === ALGORITMA === */
    drawCuboid(w,h,d, 0.34f,0.52f,0.24f, 0.38f,0.56f,0.28f, 0.40f,0.58f,0.30f, 0.28f,0.42f,0.20f);
    drawFrontPatch(-0.17f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch( 0.06f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch(-0.06f,-0.07f,0.12f,0.07f,z, 0.22f,0.34f,0.16f);
    drawFrontPatch(-0.06f,-0.15f,0.12f,0.03f,z, 0.20f,0.30f,0.14f);
}

void drawBodyZombie() {
    /* === KAMUS LOKAL === */
    float w = 0.5f;
    float h = 0.67f;
    float d = 0.2f;
    float z = d * 0.5f + 0.001f;

    /* === ALGORITMA === */
    drawCuboid(w,h,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f, 0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
    drawFrontPatch(-0.1f, 0.27f,0.2f, 0.06f,z, 0.30f,0.32f,0.12f);
    drawFrontPatch(-0.05f,0.21f,0.1f, 0.06f,z, 0.30f,0.32f,0.12f);
}

void drawArmZombie() {
    /* === KAMUS LOKAL === */
    float w = 0.25f;
    float h = 0.65f;
    float d = 0.25f;
    float sleeveH = 0.20f;
    float armH = h - sleeveH;

    /* === ALGORITMA === */
    glPushMatrix();
        glTranslatef(0,(h - sleeveH) * 0.5f,0);
        drawCuboid(w,sleeveH,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f, 0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0,-(sleeveH * 0.5f),0);
        drawCuboid(w,armH,d, 0.46f,0.60f,0.35f, 0.42f,0.56f,0.32f, 0.48f,0.62f,0.37f, 0.38f,0.50f,0.28f);
    glPopMatrix();
}

void drawLegZombie() {
    /* === KAMUS LOKAL === */
    float w = 0.25f;
    float h = 0.7f;
    float d = 0.18f;

    /* === ALGORITMA === */
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
    /* === KAMUS LOKAL === */
    GLUquadric* q;

    /* === ALGORITMA === */
    q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    setMaterial(0.85f,0.85f,0.85f);
    glPushMatrix();
        gluCylinder(q,0.22f,0.20f,0.22f,24,4);
        glPushMatrix(); 
            glTranslatef(0,0,0.22f);
            gluDisk(q,0,0.22f,24,1);
        glPopMatrix();
    glPopMatrix();
    glPushMatrix();
        gluDisk(q,0.20f,0.34f,24,1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0,0,-0.02f);
        gluDisk(q,0.18f,0.32f,24,1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0,0,-0.02f);
        gluCylinder(q,0.32f,0.34f,0.02f,24,1);
    glPopMatrix();
    gluDeleteQuadric(q);
}

void drawZombie() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glPushMatrix();
            glTranslatef(0,0.3f,0);
            glRotatef((GLfloat)head,0,0,1);
            glTranslatef(0,0.3f,0);
            glScalef(0.9f,0.9f,0.9f);
            glShadeModel(GL_FLAT);
            drawHeadZombie();
            glPushMatrix();
                glTranslatef(0,0.25f,0); glRotatef(-90,1,0,0);
                glShadeModel(GL_SMOOTH); drawBucketHat(); glShadeModel(GL_FLAT);
            glPopMatrix();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,0.07f,0);
            drawBodyZombie();
        glPopMatrix();
        glPushMatrix(); glTranslatef(0.36f,0.32f,-0.1f);
            glRotatef((GLfloat)rightShoulder,1,0,0);
            glTranslatef(0,-0.32f,0);
            drawArmZombie();
        glPopMatrix();
        glPushMatrix(); glTranslatef(-0.36f,0.32f,-0.1f);
            glRotatef((GLfloat)leftShoulder,1,0,0);
            glTranslatef(0,-0.32f,0);
            drawArmZombie();
        glPopMatrix();
        glPushMatrix();
            glTranslatef( 0.125f,-0.65f,0);
            drawLegZombie();
        glPopMatrix();
            glPushMatrix();
            glTranslatef(-0.125f,-0.65f,0);
            drawLegZombie();
        glPopMatrix();
    glPopMatrix();
}

// ===================== OBSTACLE ===================== //
void drawTableFace() {
	/* === KAMUS LOKAL === */
	
	/* === ALGORITMA=== */
    setMaterial(0.15f,0.15f,0.15f);
    drawCuboid(tableW, tableH, tableD);
}

void drawTableFoot() {
	/* === KAMUS LOKAL === */
	
	/* === ALGORITMA=== */
    setMaterial(0.15f,0.15f,0.15f);
    drawCuboid(tableFootW, tableFootH, tableFootD);
}

void drawTable() {
	/* === KAMUS LOKAL === */
	
	/* === ALGORITMA=== */
	glPushMatrix();
        glPushMatrix();
            drawTableFace();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.22f, -0.2f, 0.3f);
            drawTableFoot();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-0.22f, -0.2f, -0.3f);
            drawTableFoot();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-0.22f, -0.2f, 0.3f);
            drawTableFoot();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.22f, -0.2f, -0.3f);
            drawTableFoot();
        glPopMatrix();
    glPopMatrix();
}

// ===================== PISTOL ===================== //
void drawPistol() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
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
void drawHandHUD(float xPos, float rotation, float currentRecoil) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glLoadIdentity(); 
        glTranslatef(xPos, -0.6f + currentRecoil, -1.5f);
        glRotatef(-1,1,0,0);
        glRotatef(rotation,0,1,0);
        setMaterial(0.8f,0.6f,0.4f);
        drawCuboid(0.3f,0.3f,0.8f);
        if (xPos > 0.0f) {
            glPushMatrix();
                glTranslatef(0,0.15f,-0.35f);
                glRotatef(1,1,0,0);
                drawPistol();
            glPopMatrix();
        }
    glPopMatrix();
}

// ===================== BODY PLAYER + PISTOL ===================== //
void drawPlayer(float size) {
    /* === KAMUS LOKAL === */
    float rad;

    /* === ALGORITMA === */
    glPushMatrix();
        glTranslatef(posX, posY, posZ);
        glScalef(size, size, size);
        glRotatef(camAngleY, 0, 1, 0); 

        // Badan
        glPushMatrix();
            glTranslatef(0, -0.2f, 0);
            drawCuboid(0.5f, 0.7f, 0.25f,
                0.20f, 0.40f, 0.80f,
                0.15f, 0.35f, 0.70f,
                0.25f, 0.45f, 0.85f,
                0.10f, 0.30f, 0.65f);
        glPopMatrix();

        // Kepala
        glPushMatrix();
            glTranslatef(0, 0.35f, 0);
            drawCuboid(0.4f, 0.4f, 0.4f,
                0.85f, 0.65f, 0.50f,
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

        // Lengan kiri
        glPushMatrix();
            glTranslatef(-0.35f, -0.1f, 0);
            drawCuboid(0.20f, 0.60f, 0.20f,
                0.85f, 0.65f, 0.50f,
                0.80f, 0.60f, 0.45f,
                0.88f, 0.68f, 0.52f,
                0.75f, 0.55f, 0.40f);
        glPopMatrix();

        // Lengan kanan + pistol
        glPushMatrix();
            glTranslatef(0.35f, 0.2f, 0.0f);
            glRotatef(90.0f - (recoil * 100.0f), 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.3f, 0.0f);
            drawCuboid(0.20f, 0.60f, 0.20f,
                0.85f, 0.65f, 0.50f,
                0.80f, 0.60f, 0.45f,
                0.88f, 0.68f, 0.52f,
                0.75f, 0.55f, 0.40f);
            glPushMatrix();
                glTranslatef(0.0f, -0.35f, 0.08f);                
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                glScalef(0.8f, 0.8f, 0.8f);
                glDisable(GL_COLOR_MATERIAL);    
                setMaterial(0.15f, 0.15f, 0.15f); 
                drawPistol();
                glEnable(GL_COLOR_MATERIAL);
            glPopMatrix();
        glPopMatrix();

    glPopMatrix();
}

void zombieAttack() {
    /* === KAMUS LOKAL === */
    float dx, dz, dist;
    int currentTime, i;

    /* === ALGORITMA === */
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    if (!isDead) {
        AABB playerBox = getPlayerAABB();
        for (int i = 0; i < numZombies; i++) {
            AABB zombieBox = getZombieAABB(i);
            if (checkAABB(playerBox, zombieBox)) {
                if (currentTime - lastDamageTime > damageDelay) {
                    playerHealth -= 10;
                    lastDamageTime = currentTime;
                    if (playerHealth <= 0) {
                        isDead = true;
                        break; 
                    }
                }
            }
        }
    } else {
        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    }
}

void updateZombie(int i) {
    /* === KAMUS LOKAL === */
    float dx, dz;
    float dist;
    float desiredx, desiredz;

    /* === ALGORITMA === */
    if (isDead) {
        return;
    }

    dx = posX - zomX[i];
    dz = posZ - zomZ[i];
    dist = dx * dx + dz * dz;

    if (dist <= stopDistance * stopDistance) {
        return;
    }
    desiredx = dx / dist;
    desiredz = dz / dist;
    zomX[i] += desiredx * zomSpeed;
    zomZ[i] += desiredz * zomSpeed;
    zomAngleY[i] = atan2(-dx, -dz) * 180.0f / M_PI;
}

// ===================== PROJECTION HELPER ===================== //
void applyProjection() {
    /* === KAMUS LOKAL === */
    float aspect;

    /* === ALGORITMA === */
    aspect = (float)winWidth / (float)winHeight;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (viewMode == 4) {
        glOrtho(-camDist * aspect, camDist * aspect, -camDist, camDist, 0.1, 1000.0);
    } else {
        gluPerspective(45.0, aspect, 0.01, 200.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

// ===================== INIT ===================== //
void init() {
    /* === KAMUS LOKAL === */
    int i;

    /* === ALGORITMA === */
    srand(time(NULL)); 
    numZombies = (rand() % 10) + 3; 
    for (i = 0; i < numZombies; i++) {
        zomX[i] = (rand() % 40) - 20.0f;
        zomZ[i] = (rand() % 40) - 30.0f;
        zomAngleY[i] = 0.0f;
    }

    numTables = (rand() % 10) + 3;
    for (i = 0; i < numTables; i++) {
        tableX[i] = (rand() % 40) - 10.0f;
        tableZ[i] = (rand() % 40) - 20.0f;
    }

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
    // GL_POLYGON_SMOOTH dinonaktifkan — kalau aktif bareng GL_BLEND akan
    // menghasilkan garis artifact di setiap edge polygon (garis putus-putus horizontal)
    // Anti-aliasing sudah ditangani GLUT_MULTISAMPLE di atas
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ===================== DISPLAY ===================== //
void renderText(float x, float y, void* font, const char* string) {
    /* === KAMUS LOKAL === */
    const char* c;

    /* === ALGORITMA === */
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// ===================== MINI-MAP HUD ===================== //
/*
 * Cara menambah objek baru ke mini-map:
 * 1. Buat array posisi global (misal: barrelX[], barrelZ[])
 * 2. Di dalam drawMiniMap(), copy blok "--- Meja ---" dan sesuaikan:
 *    - Ganti tableX/tableZ dengan array baru
 *    - Ganti numTables dengan jumlah objek baru
 *    - Ganti warna glColor3f sesuai jenis objek
 *    - Sesuaikan dotSize jika perlu
 * Tidak perlu ubah apapun selain isi fungsi ini.
 *
 * Cara memperbesar jangkauan map:
 *    Ubah konstanta MAP_WORLD_RANGE di atas (default 40 = range -20..+20 unit dunia)
 *
 * Legenda warna default:
 *    Oranye  = Meja / Rintangan statis
 *    Merah   = Zombie (segitiga ? arah gerak)
 *    Cyan    = Player (segitiga ? arah kamera)
 */

// Helper worldToMap — tidak bisa lambda karena kompiler C++98/MinGW lama
// Pakai variabel global map sementara yang diset sebelum dipanggil
static float g_mapX, g_mapY, g_mapSize, g_mapScale;

void worldToMap(float wx, float wz, float &sx, float &sy) {
    sx = g_mapX + g_mapSize * 0.5f + (wx - posX) * g_mapScale;
    sy = g_mapY + g_mapSize * 0.5f - (wz - posZ) * g_mapScale;
}

void drawMiniMap() {
    /* === KAMUS LOKAL === */
    float mapX, mapY, mapSize, mapScale;
    float dotSize;
    int i;

    /* === ALGORITMA === */
    // Hitung posisi Y map dari atas layar ? koordinat Ortho2D (Y dari bawah)
    mapX    = MAP_X;
    mapSize = MAP_SIZE;
    mapY    = (float)winHeight - MAP_Y_FROM_TOP - mapSize;
    mapScale = mapSize / MAP_WORLD_RANGE;

    // Set variabel global helper sebelum memanggil worldToMap()
    g_mapX = mapX; g_mapY = mapY; g_mapSize = mapSize; g_mapScale = mapScale;

    // --- Background map (hitam semi-transparan) ---
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(mapX,          mapY);
        glVertex2f(mapX + mapSize, mapY);
        glVertex2f(mapX + mapSize, mapY + mapSize);
        glVertex2f(mapX,          mapY + mapSize);
    glEnd();

    // --- Garis border map ---
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(mapX,          mapY);
        glVertex2f(mapX + mapSize, mapY);
        glVertex2f(mapX + mapSize, mapY + mapSize);
        glVertex2f(mapX,          mapY + mapSize);
    glEnd();

    // --- Label ---
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(mapX + 2, mapY + mapSize + 4, GLUT_BITMAP_8_BY_13, "MINI-MAP");

    // --- Scissor test: potong dot yang keluar dari kotak map ---
    glEnable(GL_SCISSOR_TEST);
    glScissor((int)mapX, (int)mapY, (int)mapSize, (int)mapSize);

    dotSize = 4.0f;

    // ----------------------------------------------------------------
    // RINTANGAN STATIS — Meja (oranye/coklat)
    // Untuk tambah tipe rintangan baru: duplikasi blok ini, ganti array & warna
    // ----------------------------------------------------------------
    glColor3f(0.85f, 0.50f, 0.15f);
    for (i = 0; i < numTables; i++) {
        float sx, sy;
        worldToMap(tableX[i], tableZ[i], sx, sy);
        // Gambar sebagai persegi panjang kecil (proporsional sama meja 0.5 x 1.0)
        glBegin(GL_QUADS);
            glVertex2f(sx - dotSize * 0.5f, sy - dotSize);
            glVertex2f(sx + dotSize * 0.5f, sy - dotSize);
            glVertex2f(sx + dotSize * 0.5f, sy + dotSize);
            glVertex2f(sx - dotSize * 0.5f, sy + dotSize);
        glEnd();
    }

    // ----------------------------------------------------------------
    // ZOMBIE (merah) — segitiga menghadap arah gerak zombie
    // Untuk tambah tipe musuh baru: duplikasi blok ini, ganti array & warna
    // ----------------------------------------------------------------
    for (i = 0; i < numZombies; i++) {
        float sx, sy;
        worldToMap(zomX[i], zomZ[i], sx, sy);

        // Arah hadap zombie di map (zomAngleY: 0 = ke -Z, 90 = ke -X, dll.)
        // +90 offset karena konvensi sumbu kamera vs. sumbu map
        float rad     = (zomAngleY[i] + 90.0f) * (float)M_PI / 180.0f;
        float tipX    = sx + cosf(rad) * (dotSize + 2);
        float tipY    = sy + sinf(rad) * (dotSize + 2);
        float bAng1   = rad + (float)M_PI * 0.75f;
        float bAng2   = rad - (float)M_PI * 0.75f;

        glColor3f(1.0f, 0.15f, 0.15f);
        glBegin(GL_TRIANGLES);
            glVertex2f(tipX, tipY);
            glVertex2f(sx + cosf(bAng1) * dotSize, sy + sinf(bAng1) * dotSize);
            glVertex2f(sx + cosf(bAng2) * dotSize, sy + sinf(bAng2) * dotSize);
        glEnd();
    }

    // ----------------------------------------------------------------
    // PLAYER (cyan) — selalu di tengah map, segitiga menghadap arah kamera
    // ----------------------------------------------------------------
    {
        float sx = mapX + mapSize * 0.5f;
        float sy = mapY + mapSize * 0.5f;

        // camAngleY: 0 = lihat ke -Z, bertambah CW saat geser kanan
        // Di map: 0 = atas (Z-), jadi flip sign & offset 90
        float rad   = (-camAngleY + 90.0f) * (float)M_PI / 180.0f;
        float tipX  = sx + cosf(rad) * (dotSize + 3);
        float tipY  = sy + sinf(rad) * (dotSize + 3);
        float bAng1 = rad + (float)M_PI * 0.75f;
        float bAng2 = rad - (float)M_PI * 0.75f;

        // Titik pusat putih
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2f(sx - 2, sy - 2);
            glVertex2f(sx + 2, sy - 2);
            glVertex2f(sx + 2, sy + 2);
            glVertex2f(sx - 2, sy + 2);
        glEnd();

        // Segitiga cyan arah pandang
        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(tipX, tipY);
            glVertex2f(sx + cosf(bAng1) * dotSize, sy + sinf(bAng1) * dotSize);
            glVertex2f(sx + cosf(bAng2) * dotSize, sy + sinf(bAng2) * dotSize);
        glEnd();
    }

    glDisable(GL_SCISSOR_TEST);

    // --- Garis tengah (crosshair map) sebagai referensi pusat ---
    glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
    glLineWidth(1.0f);
    float cx = mapX + mapSize * 0.5f;
    float cy = mapY + mapSize * 0.5f;
    glBegin(GL_LINES);
        glVertex2f(cx, mapY);        glVertex2f(cx, mapY + mapSize);
        glVertex2f(mapX, cy);        glVertex2f(mapX + mapSize, cy);
    glEnd();
}

void display() {
    /* === KAMUS LOKAL === */
    int i;
    float rad;
    float eyeX, eyeY, eyeZ;
    float dist4;

    /* === ALGORITMA === */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch(viewMode) {
        case 1:
            glRotatef(camAngleX, 1, 0, 0);
            glRotatef(camAngleY, 0, 1, 0);
            glTranslatef(-posX, -(posY + eyeOffset), -posZ);
            break;

        case 2: {
            rad = camAngleY * M_PI / 180.0f;
            eyeX = posX - sin(rad) * camDist;
            eyeY = posY;
            eyeZ = posZ + cos(rad) * camDist;
            gluLookAt(eyeX, eyeY, eyeZ, posX, posY, posZ, 0, 1, 0);
            break;
        }

        case 3: {
            rad = camAngleY * M_PI / 180.0f;
            eyeX = posX - sin(rad) * camDist;
            eyeY = posY + camDist;
            eyeZ = posZ + cos(rad) * camDist;
            gluLookAt(eyeX, eyeY, eyeZ, posX, posY, posZ, 0, 1, 0);
            break;
        }

        case 4: {
            rad = camAngleY * M_PI / 180.0f;
            dist4 = 10.0f;
            eyeX = posX - sin(rad) * dist4;
            eyeY = posY + 10.0f;
            eyeZ = posZ + cos(rad) * dist4;
            gluLookAt(eyeX, eyeY, eyeZ, posX, posY, posZ, 0, 1, 0);
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

    // Meja
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(0.8f, 0.5f, 0.2f);
    for (i = 0; i < numTables; i++) {
        glPushMatrix();
            glTranslatef(tableX[i], tableY - 0.5f, tableZ[i]);
            drawTable();
        glPopMatrix();
    }

    // Zombie
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	for (i = 0; i < numZombies; i++) {
        glPushMatrix();
            glTranslatef(zomX[i], zomY, zomZ[i]);
            glRotatef(zomAngleY[i] + 180.0f, 0, 1, 0);
            glScalef(0.5f, 0.5f, 0.5f);
            drawZombie(); 
        glPopMatrix();
	}

    if (viewMode == 1) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_COLOR_MATERIAL);
        drawHandHUD(0.7f, 15.0f, recoil);
    } else {
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        drawPlayer(0.5f);
    }

    // ===================== HUD 2D ===================== //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, winWidth, 0, winHeight);  // pakai winWidth/winHeight biar responsive
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
            renderText(1050,655,GLUT_BITMAP_8_BY_13,"1/2/3/4     : Proyeksi");
            renderText(1050,640,GLUT_BITMAP_8_BY_13,"=/-         : Zoom-In/Out");
            renderText(1050,625,GLUT_BITMAP_8_BY_13,"ESC         : Keluar");

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
            sprintf(zoomCam, "Zoom: %.1f", camDist);
            renderText(bx, by-50, GLUT_BITMAP_9_BY_15, zoomCam);

            if (isDead) {
                glColor3f(1,1,1);
                renderText(580, 360, GLUT_BITMAP_9_BY_15, "Game Over");
            }

            // ===================== MINI-MAP ===================== //
            // Selalu digambar terakhir supaya tidak tertimpa elemen HUD lain
            drawMiniMap();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}

// ===================== KEYBOARD ===================== //
void keyboard(unsigned char key, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (key == 27) exit(0);
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    keys[key] = false;
}

void updateMovement() {
    /* === KAMUS LOKAL === */
    float rad, fwdX, fwdZ, rightX, rightZ, speed;
    int pastMode;
    float pastDist;
    float overlapX, overlapZ;
    int t, z;

    /* === ALGORITMA === */
    rad = camAngleY * M_PI / 180.0f;
    fwdX = sin(rad);
    fwdZ = -cos(rad);
    rightX = cos(rad);
    rightZ = sin(rad);
    speed = 0.02f;

    AABB playerBox = getPlayerAABB();
    for (t = 0; t < numTables; t++) {
        AABB table = getTableAABB(t);
        if (checkAABB(playerBox, table)) {
            overlapX = std::min(playerBox.maxX - table.minX, table.maxX - playerBox.minX);
            overlapZ = std::min(playerBox.maxZ - table.minZ, table.maxZ - playerBox.minZ);
            if (overlapX < overlapZ)
                posX += (posX < tableX[t]) ? -overlapX : overlapX;
            else
                posZ += (posZ < tableZ[t]) ? -overlapZ : overlapZ;
        }
    }

    for (z = 0; z < numZombies; z++) {
        AABB zombieBox = getZombieAABB(z);
        for (t = 0; t < numTables; t++) {
            AABB table = getTableAABB(t);
            if (checkAABB(zombieBox, table)) {
                overlapX = std::min(zombieBox.maxX - table.minX, table.maxX - zombieBox.minX);
                overlapZ = std::min(zombieBox.maxZ - table.minZ, table.maxZ - zombieBox.minZ);
                if (overlapX < overlapZ)
                    zomX[z] += (zomX[z] < tableX[t]) ? -overlapX : overlapX;
                else
                    zomZ[z] += (zomZ[z] < tableZ[t]) ? -overlapZ : overlapZ;
            }
    }
}    

    if (keys['w']) { posX += fwdX  * speed; posZ += fwdZ  * speed; }
    if (keys['s']) { posX -= fwdX  * speed; posZ -= fwdZ  * speed; }
    if (keys['a']) { posX -= rightX * speed; posZ -= rightZ * speed; }
    if (keys['d']) { posX += rightX * speed; posZ += rightZ * speed; }

    pastMode = viewMode;
    pastDist = camDist;

    if (keys['1']) viewMode = 1;
    if (keys['2']) viewMode = 2;
    if (keys['3']) viewMode = 3;
    if (keys['4']) viewMode = 4;

    if (keys['='] && camDist > 1.1f && viewMode != 1) camDist -= 0.1f;
    if (keys['-'] && camDist < 7.0f && viewMode != 1) camDist += 0.1f;

    if (pastMode != viewMode || pastDist != camDist) applyProjection();
}

// ===================== TIMER ===================== //
void timer(int v) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    updateMovement();
    for (int i = 0; i < numZombies; i++) {
        updateZombie(i); 
    }
    zombieAttack();
    if (isShooting) {
        recoil -= 0.05f;
        if (recoil < -0.3f) {
            recoil = 0;
        }
    }
    else {
        recoil = 0;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ===================== MOUSE ===================== //
void mouseMove(int x, int y) {
    /* === KAMUS LOKAL === */
    int cx, cy, dx, dy;
    static int lastX = 500;
    static int lastY = 350;
    static bool first = true;

    /* === ALGORITMA === */
    cx = 500;
    cy = 350;
    if (first) {
        lastX = x; lastY = y; first = false;
        return; 
    }
    dx = x-lastX;
    dy = y-lastY;
    lastX = x;
    lastY = y;

    camAngleY += dx * 0.2f;
    camAngleX += dy * 0.2f;
    if (camAngleX >  85) camAngleX =  85;
    if (camAngleX < -85) camAngleX = -85;

    if (x < 100 || x > 900 || y < 100 || y > 600) {
        lastX=cx;
        lastY=cy;
        glutWarpPointer(cx,cy);
    }
}

void mouseClick(int b, int s, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (b == GLUT_LEFT_BUTTON) isShooting = (s == GLUT_DOWN);
}

// ===================== RESHAPE ===================== //
void reshape(int width, int height) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (height == 0) height = 1;
    winWidth = width; winHeight = height;
    glViewport(0, 0, width, height);
    applyProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ===================== MAIN ===================== //
int main(int argc, char** argv) {
    /* === KAMUS === */
    int screenWidth, screenHeight;
    int windowWidth = 1280;
    int windowHeight = 720;

    /* === ALGORITMA === */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth-windowWidth)/2, (screenHeight-windowHeight)/2);
    glutCreateWindow("Blocky Zombie FPS");
    glutWarpPointer(500, 350);
    init();

    rightShoulder = -90;
    leftShoulder = -90;

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
