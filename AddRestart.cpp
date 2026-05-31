/*  Nama File   :   ZombieVerse.cpp
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
                    R = Restart (saat Game Over)
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
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>

// ===================== ABSTRACT DATA TYPES ===================== //
struct AABB {
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
};

struct Pohon {
    float posX, posY, posZ;
    float sizeW, sizeH, sizeD;
};

struct PathNode {
    int x;
    int z;
};

// ===================== KAMUS GLOBAL ===================== //
bool keys[256] = {false};

/* === VARIABEL KAMERA === */
int   viewMode   = 1;
float recoil     = 0.0f;
float camAngleX  = 0.0f;
float camAngleY  = 0.0f;
float camDist    = 3.0f;
float eyeOffset  = 0.13f;
bool  isShooting = false;

/* === TEXTURE === */
GLuint textureFloor;
GLuint textureTable;

/* === POSISI PEMAIN === */
float posX = 0.0f;
float posY = -0.37f;
float posZ = 5.0f;

/* === PLAYER & GAME STATE === */
float playerHealth = 100;
bool  isDead       = false;

/* === DAMAGE COOLDOWN === */
int lastDamageTime = 0;
int damageDelay    = 1000;

/* === MEJA === */
int   numTables;
const int   LIMIT_TABLES = 20;
float tableX[20];
float tableZ[20];
float tableY;
float tableW     = 0.5f;
float tableH     = 0.08f;
float tableD     = 1.0f;
float tableFootW = 0.1f;
float tableFootH = 0.48f;
float tableFootD = 0.1f;

/* === POHON === */
const int LIMIT_POHON = 20;
int   nbElmPohon = 0;
Pohon listPohon[20];

/* === ZOMBIE === */
int   numZombies;
const int   LIMIT_ZOMBIES = 20;
float zomX[20];
float zomZ[20];
float zomY        = -0.52f;
int   head        = 0;
int   rightShoulder = -90;
int   leftShoulder  = -90;
float zomSpeed    = 0.03f;
float zomAngleY[20];
float stopDistance = 0.4f;

/* === MATAHARI === */
float sunX = 0.0f;
float sunY = 15.0f;
float sunZ = -30.0f;

/* === WINDOW === */
int winWidth  = 1280;
int winHeight = 720;

/* === MINI-MAP === */
const float MAP_X           = 20.0f;
const float MAP_Y_FROM_TOP  = 220.0f;
const float MAP_SIZE        = 160.0f;
const float MAP_WORLD_RANGE = 40.0f;
float g_mapX, g_mapY, g_mapSize, g_mapScale;

/* === A* PATHFINDING === */
const int   GRID_W      = 201;
const int   GRID_H      = 201;
const float CELL_SIZE   = 0.5f;
const float WORLD_MIN_X = -50.0f;
const float WORLD_MIN_Z = -50.0f;

int pathGrid[201][201];
std::vector<PathNode> zombiePath[20];
int lastPathUpdate[20];

// ===================== HELPER POHON ===================== //
void makePohon(float x, float y, float z,
               float w, float h, float d) {
    /* === KAMUS LOKAL === */
    Pohon p;

    /* === ALGORITMA === */
    if (nbElmPohon >= LIMIT_POHON) return;
    p.posX  = x; p.posY  = y; p.posZ  = z;
    p.sizeW = w; p.sizeH = h; p.sizeD = d;
    listPohon[nbElmPohon] = p;
    nbElmPohon++;
}

// ===================== AABB ===================== //
bool checkAABB(const AABB& a, const AABB& b) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    return (a.minX <= b.maxX && a.maxX >= b.minX) &&
           (a.minY <= b.maxY && a.maxY >= b.minY) &&
           (a.minZ <= b.maxZ && a.maxZ >= b.minZ);
}

AABB getPlayerAABB() {
    /* === KAMUS LOKAL === */
    AABB box;

    /* === ALGORITMA === */
    box.minX = posX - 0.25f; box.maxX = posX + 0.25f;
    box.minY = posY;          box.maxY = posY + 0.9f;
    box.minZ = posZ - 0.25f; box.maxZ = posZ + 0.25f;
    return box;
}

AABB getTableAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;

    /* === ALGORITMA === */
    box.minX = tableX[i] - tableW * 0.45f; box.maxX = tableX[i] + tableW * 0.45f;
    box.minY = tableY;                       box.maxY = tableY + tableH;
    box.minZ = tableZ[i] - tableD * 0.45f; box.maxZ = tableZ[i] + tableD * 0.45f;
    return box;
}

AABB getPohonAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;
    Pohon p;

    /* === ALGORITMA === */
    p = listPohon[i];
    box.minX = p.posX - p.sizeW * 0.5f; box.maxX = p.posX + p.sizeW * 0.5f;
    box.minY = p.posY;                    box.maxY = p.posY + p.sizeH;
    box.minZ = p.posZ - p.sizeD * 0.5f; box.maxZ = p.posZ + p.sizeD * 0.5f;
    return box;
}

AABB getZombieAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;

    /* === ALGORITMA === */
    box.minX = zomX[i] - 0.25f; box.maxX = zomX[i] + 0.25f;
    box.minY = zomY;              box.maxY = zomY + 0.9f;
    box.minZ = zomZ[i] - 0.25f; box.maxZ = zomZ[i] + 0.25f;
    return box;
}

// ===================== MATERIAL ===================== //
void setMaterial(float r, float g, float b) {
    /* === KAMUS LOKAL === */
    GLfloat mat_ambient[]  = {r*0.3f, g*0.3f, b*0.3f, 1.0f};
    GLfloat mat_diffuse[]  = {r, g, b, 1.0f};
    GLfloat mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};

    /* === ALGORITMA === */
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialf (GL_FRONT, GL_SHININESS, 50.0f);
}

// ===================== CUBOID (warna per sisi) ===================== //
void drawCuboid(float w, float h, float d,
                float fR, float fG, float fB,
                float sR, float sG, float sB,
                float tR, float tG, float tB,
                float bR, float bG, float bB) {
    /* === KAMUS LOKAL === */
    float x = w*0.5f, y = h*0.5f, z = d*0.5f;

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

// ===================== CUBOID (dengan TexCoord) ===================== //
void drawCuboid(float w, float h, float d) {
    /* === KAMUS LOKAL === */
    float x = w*0.5f, y = h*0.5f, z = d*0.5f;

    /* === ALGORITMA === */
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glTexCoord2f(0,0); glVertex3f(-x,-y, z);
        glTexCoord2f(1,0); glVertex3f( x,-y, z);
        glTexCoord2f(1,1); glVertex3f( x, y, z);
        glTexCoord2f(0,1); glVertex3f(-x, y, z);

        glNormal3f(0,0,-1);
        glTexCoord2f(1,0); glVertex3f( x,-y,-z);
        glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
        glTexCoord2f(0,1); glVertex3f(-x, y,-z);
        glTexCoord2f(1,1); glVertex3f( x, y,-z);

        glNormal3f(-1,0,0);
        glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
        glTexCoord2f(1,0); glVertex3f(-x,-y, z);
        glTexCoord2f(1,1); glVertex3f(-x, y, z);
        glTexCoord2f(0,1); glVertex3f(-x, y,-z);

        glNormal3f(1,0,0);
        glTexCoord2f(0,0); glVertex3f(x,-y, z);
        glTexCoord2f(1,0); glVertex3f(x,-y,-z);
        glTexCoord2f(1,1); glVertex3f(x, y,-z);
        glTexCoord2f(0,1); glVertex3f(x, y, z);

        glNormal3f(0,1,0);
        glTexCoord2f(0,1); glVertex3f(-x,y, z);
        glTexCoord2f(1,1); glVertex3f( x,y, z);
        glTexCoord2f(1,0); glVertex3f( x,y,-z);
        glTexCoord2f(0,0); glVertex3f(-x,y,-z);

        glNormal3f(0,-1,0);
        glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
        glTexCoord2f(1,0); glVertex3f( x,-y,-z);
        glTexCoord2f(1,1); glVertex3f( x,-y, z);
        glTexCoord2f(0,1); glVertex3f(-x,-y, z);
    glEnd();
}

void drawFrontPatch(float px, float py, float pw, float ph,
                    float z, float r, float g, float b) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(px,   py,   z); glVertex3f(px+pw, py,   z);
        glVertex3f(px+pw,py+ph,z); glVertex3f(px,    py+ph,z);
    glEnd();
}

// ===================== ZOMBIE ===================== //
void drawHeadZombie() {
    /* === KAMUS LOKAL === */
    float w=0.5f, h=0.45f, d=0.5f, z=d*0.5f+0.001f;

    /* === ALGORITMA === */
    drawCuboid(w,h,d, 0.34f,0.52f,0.24f, 0.38f,0.56f,0.28f, 0.40f,0.58f,0.30f, 0.28f,0.42f,0.20f);
    drawFrontPatch(-0.17f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch( 0.06f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch(-0.06f,-0.07f,0.12f,0.07f,z, 0.22f,0.34f,0.16f);
    drawFrontPatch(-0.06f,-0.15f,0.12f,0.03f,z, 0.20f,0.30f,0.14f);
}

void drawBodyZombie() {
    /* === KAMUS LOKAL === */
    float w=0.5f, h=0.67f, d=0.2f, z=d*0.5f+0.001f;

    /* === ALGORITMA === */
    drawCuboid(w,h,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f, 0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
    drawFrontPatch(-0.1f, 0.27f,0.2f, 0.06f,z, 0.30f,0.32f,0.12f);
    drawFrontPatch(-0.05f,0.21f,0.1f, 0.06f,z, 0.30f,0.32f,0.12f);
}

void drawArmZombie() {
    /* === KAMUS LOKAL === */
    float w=0.25f, h=0.65f, d=0.25f, sleeveH=0.20f, armH=h-sleeveH;

    /* === ALGORITMA === */
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
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glTranslatef(0,0.05f,0);
        drawCuboid(0.25f,0.7f,0.18f, 0.34f,0.27f,0.78f, 0.30f,0.24f,0.70f, 0.37f,0.30f,0.82f, 0.30f,0.24f,0.70f);
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
        glPushMatrix(); glTranslatef(0,0,0.22f); gluDisk(q,0,0.22f,24,1); glPopMatrix();
    glPopMatrix();
    glPushMatrix(); gluDisk(q,0.20f,0.34f,24,1); glPopMatrix();
    glPushMatrix(); glTranslatef(0,0,-0.02f); gluDisk(q,0.18f,0.32f,24,1); glPopMatrix();
    glPushMatrix(); glTranslatef(0,0,-0.02f); gluCylinder(q,0.32f,0.34f,0.02f,24,1); glPopMatrix();
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
        glPushMatrix(); glTranslatef(0,0.07f,0); drawBodyZombie(); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.36f,0.32f,-0.1f); glRotatef((GLfloat)rightShoulder,1,0,0); glTranslatef(0,-0.32f,0); drawArmZombie(); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.36f,0.32f,-0.1f); glRotatef((GLfloat)leftShoulder, 1,0,0); glTranslatef(0,-0.32f,0); drawArmZombie(); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.125f,-0.65f,0); drawLegZombie(); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.125f,-0.65f,0); drawLegZombie(); glPopMatrix();
    glPopMatrix();
}

// ===================== OBSTACLE ===================== //
void drawTableFace() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureTable);
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(1.0f,1.0f,1.0f);
    drawCuboid(tableW, tableH, tableD);
    glDisable(GL_TEXTURE_2D);
}

void drawTableFoot() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureTable);
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(1.0f,1.0f,1.0f);
    drawCuboid(tableFootW, tableFootH, tableFootD);
    glDisable(GL_TEXTURE_2D);
}

void drawTable() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glPushMatrix(); drawTableFace(); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.22f,-0.2f, 0.3f); drawTableFoot(); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.22f,-0.2f,-0.3f); drawTableFoot(); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.22f,-0.2f, 0.3f); drawTableFoot(); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.22f,-0.2f,-0.3f); drawTableFoot(); glPopMatrix();
    glPopMatrix();
}

void drawBatangPohon(Pohon p) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glDisable(GL_TEXTURE_2D);
    setMaterial(0.35f,0.18f,0.08f);
    drawCuboid(p.sizeW, p.sizeH, p.sizeD);
}

void drawDaunPohon(Pohon p) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glDisable(GL_TEXTURE_2D);
    setMaterial(0.05f,0.45f,0.08f);
    glutSolidSphere(p.sizeW*2.0f, 16, 16);
}

void drawPohon(Pohon p) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glPushMatrix();
            glTranslatef(p.posX, p.posY + p.sizeH*0.5f, p.posZ);
            drawBatangPohon(p);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(p.posX, p.posY + p.sizeH + 0.45f, p.posZ);
            drawDaunPohon(p);
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
        glPushMatrix(); glTranslatef(0,0.08f, 0.2f); setMaterial(0.1f,0.1f,0.1f); drawCuboid(0.04f,0.04f,0.04f); glPopMatrix();
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

// ===================== BODY PLAYER ===================== //
void drawPlayer(float size) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glTranslatef(posX, posY, posZ);
        glScalef(size, size, size);
        glRotatef(camAngleY, 0, 1, 0);

        glPushMatrix(); glTranslatef(0,-0.2f,0);
            drawCuboid(0.5f,0.7f,0.25f, 0.20f,0.40f,0.80f, 0.15f,0.35f,0.70f, 0.25f,0.45f,0.85f, 0.10f,0.30f,0.65f);
        glPopMatrix();
        glPushMatrix(); glTranslatef(0,0.35f,0);
            drawCuboid(0.4f,0.4f,0.4f, 0.85f,0.65f,0.50f, 0.80f,0.60f,0.45f, 0.88f,0.68f,0.52f, 0.75f,0.55f,0.40f);
        glPopMatrix();
        glPushMatrix(); glTranslatef( 0.13f,-0.85f,0);
            drawCuboid(0.22f,0.55f,0.22f, 0.15f,0.15f,0.60f, 0.12f,0.12f,0.55f, 0.18f,0.18f,0.65f, 0.10f,0.10f,0.50f);
        glPopMatrix();
        glPushMatrix(); glTranslatef(-0.13f,-0.85f,0);
            drawCuboid(0.22f,0.55f,0.22f, 0.15f,0.15f,0.60f, 0.12f,0.12f,0.55f, 0.18f,0.18f,0.65f, 0.10f,0.10f,0.50f);
        glPopMatrix();
        glPushMatrix(); glTranslatef(-0.35f,-0.1f,0);
            drawCuboid(0.20f,0.60f,0.20f, 0.85f,0.65f,0.50f, 0.80f,0.60f,0.45f, 0.88f,0.68f,0.52f, 0.75f,0.55f,0.40f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.35f,0.2f,0.0f);
            glRotatef(90.0f-(recoil*100.0f),1.0f,0.0f,0.0f);
            glTranslatef(0.0f,-0.3f,0.0f);
            drawCuboid(0.20f,0.60f,0.20f, 0.85f,0.65f,0.50f, 0.80f,0.60f,0.45f, 0.88f,0.68f,0.52f, 0.75f,0.55f,0.40f);
            glPushMatrix();
                glTranslatef(0.0f,-0.35f,0.08f);
                glRotatef(-90.0f,1.0f,0.0f,0.0f);
                glScalef(0.8f,0.8f,0.8f);
                glDisable(GL_COLOR_MATERIAL);
                setMaterial(0.15f,0.15f,0.15f);
                drawPistol();
                glEnable(GL_COLOR_MATERIAL);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
}

// ===================== A* PATHFINDING ===================== //
bool worldToGrid(float wx, float wz, int &gx, int &gz) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    gx = (int)((wx - WORLD_MIN_X) / CELL_SIZE);
    gz = (int)((wz - WORLD_MIN_Z) / CELL_SIZE);
    return gx >= 0 && gx < GRID_W && gz >= 0 && gz < GRID_H;
}

void gridToWorld(int gx, int gz, float &wx, float &wz) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    wx = WORLD_MIN_X + gx * CELL_SIZE + CELL_SIZE * 0.5f;
    wz = WORLD_MIN_Z + gz * CELL_SIZE + CELL_SIZE * 0.5f;
}

bool isWalkable(int gx, int gz) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (gx < 0 || gx >= GRID_W || gz < 0 || gz >= GRID_H) return false;
    return pathGrid[gx][gz] == 0;
}

void buildPathGrid() {
    /* === KAMUS LOKAL === */
    int t, p, gx, gz;
    int minGX, maxGX, minGZ, maxGZ;
    float minX, maxX, minZ, maxZ;
    float pad;

    /* === ALGORITMA === */
    memset(pathGrid, 0, sizeof(pathGrid));

    for (t = 0; t < numTables; t++) {
        pad  = 0.55f;
        minX = tableX[t] - tableW*0.5f - pad; maxX = tableX[t] + tableW*0.5f + pad;
        minZ = tableZ[t] - tableD*0.5f - pad; maxZ = tableZ[t] + tableD*0.5f + pad;
        worldToGrid(minX, minZ, minGX, minGZ);
        worldToGrid(maxX, maxZ, maxGX, maxGZ);
        if (minGX > maxGX) std::swap(minGX, maxGX);
        if (minGZ > maxGZ) std::swap(minGZ, maxGZ);
        if (minGX < 0)       minGX = 0;
        if (maxGX >= GRID_W) maxGX = GRID_W-1;
        if (minGZ < 0)       minGZ = 0;
        if (maxGZ >= GRID_H) maxGZ = GRID_H-1;
        for (gx = minGX; gx <= maxGX; gx++)
            for (gz = minGZ; gz <= maxGZ; gz++)
                pathGrid[gx][gz] = 1;
    }

    for (p = 0; p < nbElmPohon; p++) {
        pad  = 0.55f;
        minX = listPohon[p].posX - listPohon[p].sizeW*0.5f - pad;
        maxX = listPohon[p].posX + listPohon[p].sizeW*0.5f + pad;
        minZ = listPohon[p].posZ - listPohon[p].sizeD*0.5f - pad;
        maxZ = listPohon[p].posZ + listPohon[p].sizeD*0.5f + pad;
        worldToGrid(minX, minZ, minGX, minGZ);
        worldToGrid(maxX, maxZ, maxGX, maxGZ);
        if (minGX > maxGX) std::swap(minGX, maxGX);
        if (minGZ > maxGZ) std::swap(minGZ, maxGZ);
        if (minGX < 0)       minGX = 0;
        if (maxGX >= GRID_W) maxGX = GRID_W-1;
        if (minGZ < 0)       minGZ = 0;
        if (maxGZ >= GRID_H) maxGZ = GRID_H-1;
        for (gx = minGX; gx <= maxGX; gx++)
            for (gz = minGZ; gz <= maxGZ; gz++)
                pathGrid[gx][gz] = 1;
    }
}

int heuristic(int x1, int z1, int x2, int z2) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    return abs(x1-x2) + abs(z1-z2);
}

bool findNearestWalkable(int sx, int sz, int &outX, int &outZ) {
    /* === KAMUS LOKAL === */
    int r, x, z;

    /* === ALGORITMA === */
    if (isWalkable(sx, sz)) { outX=sx; outZ=sz; return true; }
    for (r=1; r<=10; r++)
        for (x=sx-r; x<=sx+r; x++)
            for (z=sz-r; z<=sz+r; z++)
                if (isWalkable(x,z)) { outX=x; outZ=z; return true; }
    return false;
}

struct AStarItem {
    int x, z, f;
    bool operator<(const AStarItem& o) const { return f > o.f; }
};

bool findPathAStar(int startX, int startZ, int goalX, int goalZ,
                   std::vector<PathNode> &outPath) {
    /* === KAMUS LOKAL === */
    static int      gCost[201][201];
    static bool     closedSet[201][201];
    static PathNode parent[201][201];
    int dx4[4] = {1,-1,0,0};
    int dz4[4] = {0,0,1,-1};
    int x, z, dir, nx, nz, ng, k;
    AStarItem si, cur, ni;
    std::priority_queue<AStarItem> open;
    std::vector<PathNode> rev;
    PathNode c, pp;

    /* === ALGORITMA === */
    outPath.clear();
    if (!findNearestWalkable(startX, startZ, startX, startZ)) return false;
    if (!findNearestWalkable(goalX,  goalZ,  goalX,  goalZ))  return false;

    for (x=0; x<GRID_W; x++) for (z=0; z<GRID_H; z++) {
        gCost[x][z]=999999; closedSet[x][z]=false;
        parent[x][z].x=-1;  parent[x][z].z=-1;
    }
    gCost[startX][startZ]=0;
    si.x=startX; si.z=startZ; si.f=heuristic(startX,startZ,goalX,goalZ);
    open.push(si);

    while (!open.empty()) {
        cur=open.top(); open.pop();
        if (closedSet[cur.x][cur.z]) continue;
        closedSet[cur.x][cur.z]=true;

        if (cur.x==goalX && cur.z==goalZ) {
            c.x=goalX; c.z=goalZ;
            while (!(c.x==startX && c.z==startZ)) {
                rev.push_back(c);
                pp=parent[c.x][c.z];
                if (pp.x==-1||pp.z==-1) return false;
                c=pp;
            }
            for (k=(int)rev.size()-1; k>=0; k--) outPath.push_back(rev[k]);
            return true;
        }

        for (dir=0; dir<4; dir++) {
            nx=cur.x+dx4[dir]; nz=cur.z+dz4[dir];
            if (!isWalkable(nx,nz)||closedSet[nx][nz]) continue;
            ng=gCost[cur.x][cur.z]+1;
            if (ng<gCost[nx][nz]) {
                gCost[nx][nz]=ng;
                parent[nx][nz].x=cur.x; parent[nx][nz].z=cur.z;
                ni.x=nx; ni.z=nz; ni.f=ng+heuristic(nx,nz,goalX,goalZ);
                open.push(ni);
            }
        }
    }
    return false;
}

void calculateZombiePath(int i) {
    /* === KAMUS LOKAL === */
    int sx, sz, gx, gz;

    /* === ALGORITMA === */
    zombiePath[i].clear();
    if (!worldToGrid(zomX[i], zomZ[i], sx, sz)) return;
    if (!worldToGrid(posX,    posZ,    gx, gz))  return;
    findPathAStar(sx, sz, gx, gz, zombiePath[i]);
}

bool positionHitsTable(float x, float z) {
    /* === KAMUS LOKAL === */
    AABB test;
    int t, p;

    /* === ALGORITMA === */
    test.minX=x-0.25f; test.maxX=x+0.25f;
    test.minY=zomY;     test.maxY=zomY+0.9f;
    test.minZ=z-0.25f; test.maxZ=z+0.25f;
    for (t=0; t<numTables; t++)
        if (checkAABB(test, getTableAABB(t))) return true;
    for (p=0; p<nbElmPohon; p++)
        if (checkAABB(test, getPohonAABB(p))) return true;
    return false;
}

bool willHitTable(float x, float z) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    return positionHitsTable(x,z);
}

bool tryMoveZombie(int i, float dirX, float dirZ, float speedMul) {
    /* === KAMUS LOKAL === */
    float len, newX, newZ;

    /* === ALGORITMA === */
    len = sqrt(dirX*dirX + dirZ*dirZ);
    if (len <= 0.001f) return false;
    dirX/=len; dirZ/=len;
    newX = zomX[i]+dirX*zomSpeed*speedMul;
    newZ = zomZ[i]+dirZ*zomSpeed*speedMul;
    if (!willHitTable(newX,newZ)) {
        zomX[i]=newX; zomZ[i]=newZ;
        zomAngleY[i]=atan2(-dirX,-dirZ)*180.0f/3.14159265f;
        return true;
    }
    return false;
}

void doSteeringAvoidance(int i, float desiredX, float desiredZ) {
    /* === KAMUS LOKAL === */
    float rX, rZ, lX, lZ;
    float drX, drZ, dlX, dlZ;
    float brX, brZ, blX, blZ;

    /* === ALGORITMA === */
    rX= desiredZ; rZ=-desiredX;
    lX=-desiredZ; lZ= desiredX;
    drX=desiredX*0.55f+rX*0.85f; drZ=desiredZ*0.55f+rZ*0.85f;
    dlX=desiredX*0.55f+lX*0.85f; dlZ=desiredZ*0.55f+lZ*0.85f;
    brX=-desiredX*0.25f+rX;      brZ=-desiredZ*0.25f+rZ;
    blX=-desiredX*0.25f+lX;      blZ=-desiredZ*0.25f+lZ;
    if (tryMoveZombie(i,drX,drZ,0.8f))  return;
    if (tryMoveZombie(i,dlX,dlZ,0.8f))  return;
    if (tryMoveZombie(i, rX, rZ,0.65f)) return;
    if (tryMoveZombie(i, lX, lZ,0.65f)) return;
    if (tryMoveZombie(i,brX,brZ,0.45f)) return;
    if (tryMoveZombie(i,blX,blZ,0.45f)) return;
}

void updateZombie(int i) {
    /* === KAMUS LOKAL === */
    PathNode next;
    float targetX, targetZ;
    float dx, dz, dist;
    float dX, dZ, step;
    float newX, newZ;

    /* === ALGORITMA === */
    if (isDead) return;

    if (!zombiePath[i].empty()) {
        next = zombiePath[i][0];
        gridToWorld(next.x, next.z, targetX, targetZ);
    } else {
        targetX=posX; targetZ=posZ;
    }

    dx=targetX-zomX[i]; dz=targetZ-zomZ[i];
    dist=sqrt(dx*dx+dz*dz);

    if (dist < 0.12f && !zombiePath[i].empty()) {
        zombiePath[i].erase(zombiePath[i].begin()); return;
    }

    dx=posX-zomX[i]; dz=posZ-zomZ[i];
    if (sqrt(dx*dx+dz*dz) <= stopDistance) return;

    dx=targetX-zomX[i]; dz=targetZ-zomZ[i];
    dist=sqrt(dx*dx+dz*dz);
    if (dist<=0.001f) return;

    dX=dx/dist; dZ=dz/dist;
    step=(zomSpeed<dist)?zomSpeed:dist;
    newX=zomX[i]+dX*step; newZ=zomZ[i]+dZ*step;

    if (willHitTable(newX,newZ))
        doSteeringAvoidance(i,dX,dZ);
    else {
        zomX[i]=newX; zomZ[i]=newZ;
        zomAngleY[i]=atan2(-dX,-dZ)*180.0f/3.14159265f;
    }
}

// ===================== ZOMBIE ATTACK ===================== //
void zombieAttack() {
    /* === KAMUS LOKAL === */
    int currentTime, i;

    /* === ALGORITMA === */
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (isDead) return;

    AABB playerBox = getPlayerAABB();
    for (i=0; i<numZombies; i++) {
        if (checkAABB(playerBox, getZombieAABB(i))) {
            if (currentTime - lastDamageTime > damageDelay) {
                playerHealth -= 10;
                lastDamageTime = currentTime;
                if (playerHealth <= 0) { isDead=true; break; }
            }
        }
    }
}

// ===================== PROJECTION ===================== //
void applyProjection() {
    /* === KAMUS LOKAL === */
    float aspect;

    /* === ALGORITMA === */
    aspect = (float)winWidth / (float)winHeight;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (viewMode == 4)
        glOrtho(-camDist*aspect, camDist*aspect, -camDist, camDist, 0.1, 1000.0);
    else
        gluPerspective(45.0, aspect, 0.01, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

// ===================== LOAD TEXTURE BMP ===================== //
GLuint loadBMPTexture(const char* filename) {
    /* === KAMUS LOKAL === */
    FILE* file;
    unsigned char header[54];
    unsigned int dataPos, width, height, imageSize;
    unsigned char* data;
    GLuint textureID;
    unsigned int i;
    unsigned char tmp;

    /* === ALGORITMA === */
    file = fopen(filename, "rb");
    if (!file) { printf("Gagal membuka tekstur: %s\n", filename); return 0; }

    if (fread(header,1,54,file)!=54 || header[0]!='B' || header[1]!='M') {
        fclose(file); return 0;
    }
    dataPos   = *(int*)&(header[0x0A]);
    width     = *(int*)&(header[0x12]);
    height    = *(int*)&(header[0x16]);
    imageSize = *(int*)&(header[0x22]);
    if (imageSize==0) imageSize=width*height*3;
    if (dataPos==0)   dataPos=54;

    data = new unsigned char[imageSize];
    fseek(file, dataPos, SEEK_SET);
    fread(data, 1, imageSize, file);
    fclose(file);

    for (i=0; i<imageSize; i+=3) { tmp=data[i]; data[i]=data[i+2]; data[i+2]=tmp; }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
    return textureID;
}

// ===================== RESET GAME ===================== //
void resetGame() {
    /* === KAMUS LOKAL === */
    int i, gx, gz, tries;

    /* === ALGORITMA === */
    // Reset pemain
    posX=0.0f; posY=-0.37f; posZ=5.0f;
    playerHealth   = 100.0f;
    isDead         = false;
    lastDamageTime = 0;

    // Reset kamera
    camAngleX=0.0f; camAngleY=0.0f;
    camDist=3.0f;   recoil=0.0f;
    isShooting=false; viewMode=1;

    // Bersihkan key
    for (i=0; i<256; i++) keys[i]=false;

    // Spawn ulang meja
    numTables=(rand()%10)+3;
    for (i=0; i<numTables; i++) {
        tableX[i]=(float)(rand()%40)-10.0f;
        tableZ[i]=(float)(rand()%40)-20.0f;
    }

    // Rebuild grid
    buildPathGrid();

    // Spawn ulang zombie
    numZombies=5;
    for (i=0; i<numZombies; i++) {
        tries=0;
        do {
            zomX[i]=(float)(rand()%40)-20.0f;
            zomZ[i]=(float)(rand()%40)-30.0f;
            tries++;
            worldToGrid(zomX[i],zomZ[i],gx,gz);
        } while ((!worldToGrid(zomX[i],zomZ[i],gx,gz)||
                  !isWalkable(gx,gz)||
                  positionHitsTable(zomX[i],zomZ[i])) && tries<100);

        zomAngleY[i]     =0.0f;
        lastPathUpdate[i]=0;
        zombiePath[i].clear();
        calculateZombiePath(i);
    }

    glClearColor(0.53f,0.81f,0.98f,1.0f);
}

// ===================== INIT ===================== //
void init() {
    /* === KAMUS LOKAL === */
    int i, gx, gz, tries;

    /* === ALGORITMA === */
    srand(time(NULL));

    // Spawn meja
    numTables=(rand()%10)+3;
    for (i=0; i<numTables; i++) {
        tableX[i]=(float)(rand()%40)-10.0f;
        tableZ[i]=(float)(rand()%40)-20.0f;
    }

    // Pohon (posisi tetap)
    nbElmPohon=0;
    makePohon(-4.0f,-1.0f,-8.0f,  0.35f,1.8f,0.35f);
    makePohon( 3.0f,-1.0f,-12.0f, 0.35f,1.8f,0.35f);
    makePohon( 7.0f,-1.0f,-5.0f,  0.35f,1.8f,0.35f);
    makePohon(-8.0f,-1.0f,-18.0f, 0.35f,1.8f,0.35f);
    makePohon(12.0f,-1.0f,-22.0f, 0.35f,1.8f,0.35f);

    buildPathGrid();

    // Spawn zombie
    numZombies=5;
    for (i=0; i<numZombies; i++) {
        tries=0;
        do {
            zomX[i]=(float)(rand()%40)-20.0f;
            zomZ[i]=(float)(rand()%40)-30.0f;
            tries++;
            worldToGrid(zomX[i],zomZ[i],gx,gz);
        } while ((!worldToGrid(zomX[i],zomZ[i],gx,gz)||
                  !isWalkable(gx,gz)||
                  positionHitsTable(zomX[i],zomZ[i])) && tries<100);

        zomAngleY[i]     =0.0f;
        lastPathUpdate[i]=0;
        calculateZombiePath(i);
    }

    // Tekstur — sesuaikan path ke folder kamu
    textureFloor = loadBMPTexture("D:/SMSTR_4/GTI/Prak GTI/CobaZombie/lantai.bmp");
    textureTable = loadBMPTexture("D:/SMSTR_4/GTI/Prak GTI/CobaZombie/meja.bmp");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    GLfloat global_ambient[]={0.6f,0.6f,0.6f,1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    GLfloat spec[]={0.8f,0.8f,0.8f,1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    glClearColor(0.53f,0.81f,0.98f,1.0f);
    glEnable(GLUT_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ===================== RENDER TEXT ===================== //
void renderText(float x, float y, void* font, const char* string) {
    /* === KAMUS LOKAL === */
    const char* c;

    /* === ALGORITMA === */
    glRasterPos2f(x, y);
    for (c=string; *c!='\0'; c++) glutBitmapCharacter(font, *c);
}

// ===================== MINI-MAP ===================== //
void worldToMap(float wx, float wz, float &sx, float &sy) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    sx = g_mapX + g_mapSize*0.5f + (wx-posX)*g_mapScale;
    sy = g_mapY + g_mapSize*0.5f - (wz-posZ)*g_mapScale;
}

void drawMiniMap() {
    /* === KAMUS LOKAL === */
    float mapX, mapY, mapSize, mapScale, dotSize;
    float cx, cy, sx, sy, rad;
    float tipX, tipY, b1, b2;
    int i;

    /* === ALGORITMA === */
    mapX    = MAP_X;
    mapSize = MAP_SIZE;
    mapY    = (float)winHeight - MAP_Y_FROM_TOP - mapSize;
    mapScale= mapSize / MAP_WORLD_RANGE;
    dotSize = 4.0f;

    g_mapX=mapX; g_mapY=mapY; g_mapSize=mapSize; g_mapScale=mapScale;

    // Background
    glColor4f(0,0,0,0.55f);
    glBegin(GL_QUADS);
        glVertex2f(mapX,mapY); glVertex2f(mapX+mapSize,mapY);
        glVertex2f(mapX+mapSize,mapY+mapSize); glVertex2f(mapX,mapY+mapSize);
    glEnd();

    // Border
    glColor3f(0.8f,0.8f,0.8f); glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(mapX,mapY); glVertex2f(mapX+mapSize,mapY);
        glVertex2f(mapX+mapSize,mapY+mapSize); glVertex2f(mapX,mapY+mapSize);
    glEnd();

    glColor3f(1,1,1);
    renderText(mapX+2, mapY+mapSize+4, GLUT_BITMAP_8_BY_13, "MINI-MAP");

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)mapX,(int)mapY,(int)mapSize,(int)mapSize);

    // Meja (oranye)
    glColor3f(0.85f,0.50f,0.15f);
    for (i=0;i<numTables;i++) {
        worldToMap(tableX[i],tableZ[i],sx,sy);
        glBegin(GL_QUADS);
            glVertex2f(sx-dotSize*0.5f,sy-dotSize); glVertex2f(sx+dotSize*0.5f,sy-dotSize);
            glVertex2f(sx+dotSize*0.5f,sy+dotSize); glVertex2f(sx-dotSize*0.5f,sy+dotSize);
        glEnd();
    }

    // Pohon (hijau)
    glColor3f(0.05f,0.75f,0.10f);
    for (i=0;i<nbElmPohon;i++) {
        worldToMap(listPohon[i].posX,listPohon[i].posZ,sx,sy);
        glBegin(GL_QUADS);
            glVertex2f(sx-dotSize,sy-dotSize); glVertex2f(sx+dotSize,sy-dotSize);
            glVertex2f(sx+dotSize,sy+dotSize); glVertex2f(sx-dotSize,sy+dotSize);
        glEnd();
    }

    // Zombie (merah segitiga)
    for (i=0;i<numZombies;i++) {
        worldToMap(zomX[i],zomZ[i],sx,sy);
        rad=(zomAngleY[i]+90.0f)*(float)M_PI/180.0f;
        tipX=sx+cos(rad)*(dotSize+2); tipY=sy+sin(rad)*(dotSize+2);
        b1=rad+(float)M_PI*0.75f;     b2=rad-(float)M_PI*0.75f;
        glColor3f(1.0f,0.15f,0.15f);
        glBegin(GL_TRIANGLES);
            glVertex2f(tipX,tipY);
            glVertex2f(sx+cos(b1)*dotSize,sy+sin(b1)*dotSize);
            glVertex2f(sx+cos(b2)*dotSize,sy+sin(b2)*dotSize);
        glEnd();
    }

    // Player (cyan + putih)
    sx=mapX+mapSize*0.5f; sy=mapY+mapSize*0.5f;
    rad=(-camAngleY+90.0f)*(float)M_PI/180.0f;
    tipX=sx+cos(rad)*(dotSize+3); tipY=sy+sin(rad)*(dotSize+3);
    b1=rad+(float)M_PI*0.75f;     b2=rad-(float)M_PI*0.75f;
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glVertex2f(sx-2,sy-2); glVertex2f(sx+2,sy-2);
        glVertex2f(sx+2,sy+2); glVertex2f(sx-2,sy+2);
    glEnd();
    glColor3f(0,1,1);
    glBegin(GL_TRIANGLES);
        glVertex2f(tipX,tipY);
        glVertex2f(sx+cos(b1)*dotSize,sy+sin(b1)*dotSize);
        glVertex2f(sx+cos(b2)*dotSize,sy+sin(b2)*dotSize);
    glEnd();

    glDisable(GL_SCISSOR_TEST);

    // Garis silang tengah
    cx=mapX+mapSize*0.5f; cy=mapY+mapSize*0.5f;
    glColor4f(1,1,1,0.15f); glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(cx,mapY); glVertex2f(cx,mapY+mapSize);
        glVertex2f(mapX,cy); glVertex2f(mapX+mapSize,cy);
    glEnd();
}

// ===================== DISPLAY ===================== //
void display() {
    /* === KAMUS LOKAL === */
    int i;
    float rad, eyeX, eyeY, eyeZ, dist4;
    float barW, barH, bx, by, hpPerc;
    char healthText[50], zoomCam[50];

    /* === ALGORITMA === */
    if (isDead)
        glClearColor(0.40f,0.0f,0.0f,1.0f);
    else
        glClearColor(0.53f,0.81f,0.98f,1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch(viewMode) {
        case 1:
            glRotatef(camAngleX,1,0,0);
            glRotatef(camAngleY,0,1,0);
            glTranslatef(-posX,-(posY+eyeOffset),-posZ);
            break;
        case 2: {
            rad=camAngleY*(float)M_PI/180.0f;
            eyeX=posX-sin(rad)*camDist; eyeY=posY; eyeZ=posZ+cos(rad)*camDist;
            gluLookAt(eyeX,eyeY,eyeZ, posX,posY,posZ, 0,1,0);
            break;
        }
        case 3: {
            rad=camAngleY*(float)M_PI/180.0f;
            eyeX=posX-sin(rad)*camDist; eyeY=posY+camDist; eyeZ=posZ+cos(rad)*camDist;
            gluLookAt(eyeX,eyeY,eyeZ, posX,posY,posZ, 0,1,0);
            break;
        }
        case 4: {
            rad=camAngleY*(float)M_PI/180.0f; dist4=10.0f;
            eyeX=posX-sin(rad)*dist4; eyeY=posY+10.0f; eyeZ=posZ+cos(rad)*dist4;
            gluLookAt(eyeX,eyeY,eyeZ, posX,posY,posZ, 0,1,0);
            break;
        }
    }

    GLfloat light_pos[]={sunX,sunY,sunZ,1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Matahari
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(sunX,sunY,sunZ);
        glColor3f(1,1,0); glutSolidSphere(3,20,20);
        glEnable(GL_LIGHTING);
    glPopMatrix();

    // Lantai bertekstur
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureFloor);
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(1.0f,1.0f,1.0f);
    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glTexCoord2f( 0.0f, 0.0f); glVertex3f(-100,-1,-100);
        glTexCoord2f( 0.0f,50.0f); glVertex3f(-100,-1, 100);
        glTexCoord2f(50.0f,50.0f); glVertex3f( 100,-1, 100);
        glTexCoord2f(50.0f, 0.0f); glVertex3f( 100,-1,-100);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Meja
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(0.8f,0.5f,0.2f);
    for (i=0;i<numTables;i++) {
        glPushMatrix();
            glTranslatef(tableX[i],tableY-0.5f,tableZ[i]);
            drawTable();
        glPopMatrix();
    }

    // Pohon
    glDisable(GL_COLOR_MATERIAL);
    for (i=0;i<nbElmPohon;i++) drawPohon(listPohon[i]);

    // Zombie
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    for (i=0;i<numZombies;i++) {
        glPushMatrix();
            glTranslatef(zomX[i],zomY,zomZ[i]);
            glRotatef(zomAngleY[i]+180.0f,0,1,0);
            glScalef(0.5f,0.5f,0.5f);
            drawZombie();
        glPopMatrix();
    }

    // Player / Hand HUD
    if (viewMode==1) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_COLOR_MATERIAL);
        drawHandHUD(0.7f,15.0f,recoil);
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
        gluOrtho2D(0, winWidth, 0, winHeight);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            // Label mode kamera
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
            renderText(1050,625,GLUT_BITMAP_8_BY_13,"R           : Restart");
            renderText(1050,610,GLUT_BITMAP_8_BY_13,"ESC         : Keluar");

            // Health bar
            barW=300; barH=25; bx=50; by=650;
            hpPerc = playerHealth/100.0f;
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

            sprintf(healthText,"HP : %.0f%%", playerHealth);
            if      (playerHealth > 70) glColor3f(1,1,1);
            else if (playerHealth > 30) glColor3f(1,1,0);
            else                        glColor3f(1,0,0);
            renderText(bx, by-25, GLUT_BITMAP_9_BY_15, healthText);

            glColor3f(1,1,1);
            sprintf(zoomCam,"Zoom: %.1f", camDist);
            renderText(bx, by-50, GLUT_BITMAP_9_BY_15, zoomCam);

            // ===================== GAME OVER OVERLAY ===================== //
            if (isDead) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Panel gelap
                glColor4f(0.0f,0.0f,0.0f,0.65f);
                glBegin(GL_QUADS);
                    glVertex2f(390,290); glVertex2f(890,290);
                    glVertex2f(890,450); glVertex2f(390,450);
                glEnd();

                // Border merah
                glColor4f(0.85f,0.0f,0.0f,0.9f);
                glLineWidth(2.0f);
                glBegin(GL_LINE_LOOP);
                    glVertex2f(390,290); glVertex2f(890,290);
                    glVertex2f(890,450); glVertex2f(390,450);
                glEnd();

                // Teks GAME OVER
                glColor3f(1.0f,0.15f,0.15f);
                renderText(536, 400, GLUT_BITMAP_TIMES_ROMAN_24, "GAME  OVER");

                // Instruksi restart
                glColor3f(1.0f,1.0f,1.0f);
                renderText(482, 355, GLUT_BITMAP_9_BY_15, "Tekan  [R]  untuk  Restart");
                renderText(470, 320, GLUT_BITMAP_9_BY_15, "Tekan  [ESC]  untuk  Keluar");
            }

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

    // Restart hanya saat Game Over
    if ((key=='r' || key=='R') && isDead) {
        resetGame();
        return;
    }

    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    keys[key] = false;
}

// ===================== MOVEMENT ===================== //
void updateMovement() {
    /* === KAMUS LOKAL === */
    float rad, fwdX, fwdZ, rightX, rightZ, speed;
    float overlapX, overlapZ;
    int   pastMode, t, p;
    float pastDist;

    /* === ALGORITMA === */
    rad    = camAngleY*(float)M_PI/180.0f;
    fwdX   = sin(rad);  fwdZ   = -cos(rad);
    rightX = cos(rad);  rightZ =  sin(rad);
    speed  = 0.02f;

    // Collision player vs meja
    AABB playerBox = getPlayerAABB();
    for (t=0; t<numTables; t++) {
        AABB table = getTableAABB(t);
        if (checkAABB(playerBox,table)) {
            overlapX=std::min(playerBox.maxX-table.minX, table.maxX-playerBox.minX);
            overlapZ=std::min(playerBox.maxZ-table.minZ, table.maxZ-playerBox.minZ);
            if (overlapX<overlapZ) posX+=(posX<tableX[t])?-overlapX:overlapX;
            else                   posZ+=(posZ<tableZ[t])?-overlapZ:overlapZ;
        }
    }

    // Collision player vs pohon
    for (p=0; p<nbElmPohon; p++) {
        AABB pohon = getPohonAABB(p);
        playerBox  = getPlayerAABB();
        if (checkAABB(playerBox,pohon)) {
            overlapX=std::min(playerBox.maxX-pohon.minX, pohon.maxX-playerBox.minX);
            overlapZ=std::min(playerBox.maxZ-pohon.minZ, pohon.maxZ-playerBox.minZ);
            if (overlapX<overlapZ) posX+=(posX<listPohon[p].posX)?-overlapX:overlapX;
            else                   posZ+=(posZ<listPohon[p].posZ)?-overlapZ:overlapZ;
        }
    }

    if (keys['w']) { posX+=fwdX*speed;   posZ+=fwdZ*speed;   }
    if (keys['s']) { posX-=fwdX*speed;   posZ-=fwdZ*speed;   }
    if (keys['a']) { posX-=rightX*speed; posZ-=rightZ*speed; }
    if (keys['d']) { posX+=rightX*speed; posZ+=rightZ*speed; }

    pastMode=viewMode; pastDist=camDist;
    if (keys['1']) viewMode=1;
    if (keys['2']) viewMode=2;
    if (keys['3']) viewMode=3;
    if (keys['4']) viewMode=4;
    if (keys['='] && camDist>1.1f && viewMode!=1) camDist-=0.1f;
    if (keys['-'] && camDist<7.0f && viewMode!=1) camDist+=0.1f;
    if (pastMode!=viewMode || pastDist!=camDist) applyProjection();
}

// ===================== TIMER ===================== //
void timer(int v) {
    /* === KAMUS LOKAL === */
    int currentTime, i;

    /* === ALGORITMA === */
    updateMovement();
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    for (i=0; i<numZombies; i++) {
        if (currentTime - lastPathUpdate[i] > 500 + i*35) {
            calculateZombiePath(i);
            lastPathUpdate[i]=currentTime;
        }
        updateZombie(i);
    }

    zombieAttack();

    if (isShooting) {
        recoil -= 0.05f;
        if (recoil < -0.3f) recoil=0;
    } else {
        recoil=0;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ===================== MOUSE ===================== //
void mouseMove(int x, int y) {
    /* === KAMUS LOKAL === */
    int dx, dy;
    static int  lastX=500, lastY=350;
    static bool first=true;

    /* === ALGORITMA === */
    if (first) { lastX=x; lastY=y; first=false; return; }
    dx=x-lastX; dy=y-lastY;
    lastX=x; lastY=y;

    camAngleY += dx*0.2f;
    camAngleX += dy*0.2f;
    if (camAngleX >  85) camAngleX= 85;
    if (camAngleX < -85) camAngleX=-85;

    if (x<100||x>900||y<100||y>600) {
        lastX=500; lastY=350; glutWarpPointer(500,350);
    }
}

void mouseClick(int b, int s, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (b==GLUT_LEFT_BUTTON) isShooting=(s==GLUT_DOWN);
}

// ===================== RESHAPE ===================== //
void reshape(int width, int height) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (height==0) height=1;
    winWidth=width; winHeight=height;
    glViewport(0,0,width,height);
    applyProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ===================== MAIN ===================== //
int main(int argc, char** argv) {
    /* === KAMUS === */
    int screenWidth, screenHeight;
    int windowWidth=1280, windowHeight=720;

    /* === ALGORITMA === */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH|GLUT_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    screenWidth =glutGet(GLUT_SCREEN_WIDTH);
    screenHeight=glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth-windowWidth)/2,(screenHeight-windowHeight)/2);
    glutCreateWindow("Blocky Zombie FPS - Kelompok 6");
    glutWarpPointer(500,350);
    init();

    rightShoulder=-90;
    leftShoulder =-90;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMove);
    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);
    glutTimerFunc(0,timer,0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();
    return 0;
}
