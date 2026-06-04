#include "render.h"
#include <stdio.h>

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

// ===================== CUBOID (warna per sisi) ===================== //
void drawCuboid(float w, float h, float d,
                float fR, float fG, float fB,
                float sR, float sG, float sB,
                float tR, float tG, float tB,
                float bR, float bG, float bB) {
    /* === KAMUS LOKAL === */
    float x, y, z;

    /* === ALGORITMA === */
    x = w * 0.5f;
    y = h * 0.5f;
    z = d * 0.5f;
    glBegin(GL_QUADS);
        glNormal3f(0,0,1); glColor3f(fR,fG,fB);
        glVertex3f(-x,-y,z); glVertex3f(x,-y,z); glVertex3f(x,y,z); glVertex3f(-x,y,z);
        glNormal3f(0,0,-1); glColor3f(fR * 0.8f,fG * 0.8f,fB * 0.8f);
        glVertex3f(x,-y,-z); glVertex3f(-x,-y,-z); glVertex3f(-x,y,-z); glVertex3f(x,y,-z);
        glNormal3f(-1,0,0); glColor3f(sR,sG,sB);
        glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z); glVertex3f(-x,y,z); glVertex3f(-x,y,-z);
        glNormal3f(1,0,0); glColor3f(sR,sG,sB);
        glVertex3f(x,-y,z); glVertex3f(x,-y,-z); glVertex3f(x,y,-z); glVertex3f(x,y,z);
        glNormal3f(0,1,0); glColor3f(tR,tG,tB);
        glVertex3f(-x,y,z); glVertex3f(x,y,z); glVertex3f(x,y,-z); glVertex3f(-x,y,-z);
        glNormal3f(0,-1,0); glColor3f(bR,bG,bB);
        glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z); glVertex3f(x,-y,z); glVertex3f(-x,-y,z);
    glEnd();
}

// ===================== CUBOID (dengan TexCoord) ===================== //
void drawCuboid(float w, float h, float d) {
    /* === KAMUS LOKAL === */
    float x, y, z;

    /* === ALGORITMA === */
    x = w * 0.5f;
    y = h * 0.5f;
    z = d * 0.5f;
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glTexCoord2f(0,0); glVertex3f(-x,-y,z);
        glTexCoord2f(1,0); glVertex3f(x,-y,z);
        glTexCoord2f(1,1); glVertex3f(x,y,z);
        glTexCoord2f(0,1); glVertex3f(-x,y,z);

        glNormal3f(0,0,-1);
        glTexCoord2f(1,0); glVertex3f( x,-y,-z);
        glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
        glTexCoord2f(0,1); glVertex3f(-x,y,-z);
        glTexCoord2f(1,1); glVertex3f( x,y,-z);

        glNormal3f(-1,0,0);
        glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
        glTexCoord2f(1,0); glVertex3f(-x,-y,z);
        glTexCoord2f(1,1); glVertex3f(-x,y,z);
        glTexCoord2f(0,1); glVertex3f(-x,y,-z);

        glNormal3f(1,0,0);
        glTexCoord2f(0,0); glVertex3f(x,-y,z);
        glTexCoord2f(1,0); glVertex3f(x,-y,-z);
        glTexCoord2f(1,1); glVertex3f(x,y,-z);
        glTexCoord2f(0,1); glVertex3f(x,y,z);

        glNormal3f(0,1,0);
        glTexCoord2f(0,1); glVertex3f(-x,y,z);
        glTexCoord2f(1,1); glVertex3f(x,y,z);
        glTexCoord2f(1,0); glVertex3f(x,y,-z);
        glTexCoord2f(0,0); glVertex3f(-x,y,-z);

        glNormal3f(0,-1,0);
        glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
        glTexCoord2f(1,0); glVertex3f(x,-y,-z);
        glTexCoord2f(1,1); glVertex3f(x,-y,z);
        glTexCoord2f(0,1); glVertex3f(-x,-y,z);
    glEnd();
}

void drawFrontPatch(float px, float py, float pw, float ph,
                    float z, float r, float g, float b) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(px, py, z); glVertex3f(px + pw, py, z);
        glVertex3f(px + pw,py + ph,z); glVertex3f(px, py + ph,z);
    glEnd();
}

// ===================== ZOMBIE ===================== //
static void drawHeadZombie() {
    /* === KAMUS LOKAL === */
    float w, h, d, z;

    /* === ALGORITMA === */
    w = 0.5f;
    h = 0.45f;
    d = 0.5f;
    z = d * 0.5f + 0.001f;
    drawCuboid(w,h,d, 0.34f,0.52f,0.24f, 0.38f,0.56f,0.28f, 0.40f,0.58f,0.30f, 0.28f,0.42f,0.20f);
    drawFrontPatch(-0.17f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch(0.06f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
    drawFrontPatch(-0.06f,-0.07f,0.12f,0.07f,z, 0.22f,0.34f,0.16f);
    drawFrontPatch(-0.06f,-0.15f,0.12f,0.03f,z, 0.20f,0.30f,0.14f);
}

static void drawBodyZombie() {
    /* === KAMUS LOKAL === */
    float w, h, d, z;

    /* === ALGORITMA === */
    w = 0.5f;
    h = 0.67f;
    d = 0.2f;
    z = d * 0.5f + 0.001f;
    drawCuboid(w,h,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f, 0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
    drawFrontPatch(-0.1f,0.27f,0.2f,0.06f,z, 0.30f,0.32f,0.12f);
    drawFrontPatch(-0.05f,0.21f,0.1f,0.06f,z, 0.30f,0.32f,0.12f);
}

static void drawArmZombie() {
    /* === KAMUS LOKAL === */
    float w, h, d;
    float sleeveH, armH;

    /* === ALGORITMA === */
    w = 0.25f;
    h = 0.65f;
    d = 0.25f;
    sleeveH = 0.20f;
    armH = h - sleeveH;

    glPushMatrix();
        glTranslatef(0,(h-sleeveH) * 0.5f,0);
        drawCuboid(w,sleeveH,d,0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f, 0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0,-(sleeveH * 0.5f),0);
        drawCuboid(w,armH,d,0.46f,0.60f,0.35f, 0.42f,0.56f,0.32f, 0.48f,0.62f,0.37f, 0.38f,0.50f,0.28f);
    glPopMatrix();
}

static void drawLegZombie() {
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

static void drawBucketHat() {
    /* === KAMUS LOKAL === */
    GLUquadric *q;

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
                glTranslatef(0,0.25f,0);
                glRotatef(-90,1,0,0);
                glShadeModel(GL_SMOOTH);
                drawBucketHat();
                glShadeModel(GL_FLAT);
            glPopMatrix();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,0.07f,0);
            drawBodyZombie();
        glPopMatrix();
        glPushMatrix();
            glTranslatef( 0.36f,0.32f,-0.1f);
            glRotatef((GLfloat)rightShoulder,1,0,0);
            glTranslatef(0,-0.32f,0);
            drawArmZombie();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-0.36f,0.32f,-0.1f);
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

// ===================== MEJA ===================== //
static void drawTableFace() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureTable);
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(1.0f,1.0f,1.0f);
    drawCuboid(tableW, tableH, tableD);
    glDisable(GL_TEXTURE_2D);
}

static void drawTableFoot() {
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
        glPushMatrix();
            drawTableFace();
        glPopMatrix();
        glPushMatrix(); 
            glTranslatef(0.22f,-0.2f,0.3f);
            drawTableFoot();
        glPopMatrix();
        glPushMatrix(); 
            glTranslatef(-0.22f,-0.2f,-0.3f);
            drawTableFoot();
        glPopMatrix();
        glPushMatrix(); 
            glTranslatef(-0.22f,-0.2f,0.3f);
            drawTableFoot();
        glPopMatrix();
        glPushMatrix(); 
            glTranslatef(0.22f,-0.2f,-0.3f);
            drawTableFoot();
        glPopMatrix();
    glPopMatrix();
}

// ===================== POHON ===================== //
void drawPohon(Pohon p) {
    /* === KAMUS LOKAL === */
    GLUquadric *qTrunk;
    GLUquadric *q;
    float radius;

    /* === ALGORITMA === */
    glPushMatrix();
        glPushMatrix();
            glTranslatef(p.posX, p.posY, p.posZ);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureTable);
            glDisable(GL_COLOR_MATERIAL);
            setMaterial(1.0f, 1.0f, 1.0f);
            qTrunk = gluNewQuadric();
            gluQuadricTexture(qTrunk, GL_TRUE);       
            gluQuadricNormals(qTrunk, GLU_SMOOTH);
            radius = p.sizeW * 0.5f;
            gluCylinder(qTrunk, radius, radius, p.sizeH, 16, 4);
            gluDeleteQuadric(qTrunk);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(p.posX, p.posY + p.sizeH + 0.45f, p.posZ);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureFloor);
            glDisable(GL_COLOR_MATERIAL);
            setMaterial(1.0f, 1.0f, 1.0f);
            q = gluNewQuadric();
            gluQuadricTexture(q, GL_TRUE);
            gluQuadricNormals(q, GLU_SMOOTH);
            gluSphere(q, p.sizeW * 2.0f, 16, 16);
            gluDeleteQuadric(q);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    glPopMatrix();
}

// ===================== LANGIT ===================== //
void drawSky() {
    /* === KAMUS LOKAL === */
    GLuint activeSky;
    GLUquadric *q;

    /* === ALGORITMA === */
    activeSky = textureSky;

    if (isWin) {
        activeSky = textureSkyWin;
    } else if (isDead) {
        activeSky = textureSkyLose;
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, activeSky);

    glColor3f(1,1,1);

    glPushMatrix();
        glTranslatef(posX, posY, posZ);

        q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE);
        gluQuadricNormals(q, GLU_SMOOTH);
        gluQuadricOrientation(q, GLU_INSIDE);

        gluSphere(q, 80.0f, 48, 24);

        gluDeleteQuadric(q);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ===================== PISTOL ===================== //
void drawPistol() {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    setMaterial(0.15f,0.15f,0.15f);
    drawCuboid(0.08f,0.35f,0.12f);
    glPushMatrix();
        glTranslatef(0,-0.18f,0);
        setMaterial(0.1f,0.1f,0.1f);
        drawCuboid(0.09f,0.04f,0.14f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0,0.20f,-0.15f);
        setMaterial(0.3f,0.3f,0.3f);
        drawCuboid(0.1f,0.12f,0.5f);
        glPushMatrix();
            glTranslatef(0,0.08f, 0.2f);
            setMaterial(0.1f,0.1f,0.1f);
            drawCuboid(0.04f,0.04f,0.04f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,0.07f,-0.22f);
            drawCuboid(0.02f,0.03f,0.03f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,-0.08f,0.05f);
            drawCuboid(0.03f,0.12f,0.03f);
            glTranslatef(0,-0.05f,0.05f);
            drawCuboid(0.03f,0.03f,0.1f);
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

// ===================== PLAYER ===================== //
void drawPlayer(float size) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    glPushMatrix();
        glTranslatef(posX, posY, posZ);
        glScalef(size, size, size);
        glRotatef(camAngleY, 0, 1, 0);

        glPushMatrix();
            glTranslatef(0,-0.2f,0);
            drawCuboid(0.5f,0.7f,0.25f, 0.20f,0.40f,0.80f, 0.15f,0.35f,0.70f, 0.25f,0.45f,0.85f, 0.10f,0.30f,0.65f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0,0.35f,0);
            drawCuboid(0.4f,0.4f,0.4f, 0.85f,0.65f,0.50f, 0.80f,0.60f,0.45f, 0.88f,0.68f,0.52f, 0.75f,0.55f,0.40f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef( 0.13f,-0.85f,0);
            drawCuboid(0.22f,0.55f,0.22f, 0.15f,0.15f,0.60f, 0.12f,0.12f,0.55f, 0.18f,0.18f,0.65f, 0.10f,0.10f,0.50f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-0.13f,-0.85f,0);
            drawCuboid(0.22f,0.55f,0.22f, 0.15f,0.15f,0.60f, 0.12f,0.12f,0.55f, 0.18f,0.18f,0.65f, 0.10f,0.10f,0.50f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-0.35f,-0.1f,0);
            drawCuboid(0.20f,0.60f,0.20f, 0.85f,0.65f,0.50f, 0.80f,0.60f,0.45f, 0.88f,0.68f,0.52f, 0.75f,0.55f,0.40f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.35f,0.2f,0.0f);
            glRotatef(90.0f - (recoil * 100.0f),1.0f,0.0f,0.0f);
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

// ===================== PROJECTION ===================== //
void applyProjection() {
    /* === KAMUS LOKAL === */
    float aspect;

    /* === ALGORITMA === */
    aspect = (float) winWidth / (float) winHeight;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (viewMode == 4) {
        glOrtho(-camDist * aspect, camDist * aspect, -camDist, camDist, 0.1, 1000.0);
    } else {
        gluPerspective(45.0, aspect, 0.01, 200.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

// ===================== LOAD TEXTURE ===================== //
GLuint loadBMPTexture(const char* filename) {
    /* === KAMUS LOKAL === */
    GLuint id;
    FILE *file;
    unsigned int dataPos, width, height, imageSize, i;
    unsigned char header[54]; 
    unsigned char *data;
    unsigned char tmp;

    /* === ALGORITMA === */
    file = fopen(filename, "rb");
    if (!file) {
        printf("Gagal membuka tekstur: %s\n", filename);
        return 0;
    }

    if (fread(header,1,54,file) != 54 || header[0] != 'B' || header[1] != 'M') {
        fclose(file);
        return 0;
    }

    dataPos = *(int*) &(header[0x0A]);
    width = *(int*) &(header[0x12]);
    height = *(int*) &(header[0x16]);
    imageSize = *(int*) &(header[0x22]);

    if (imageSize == 0) {
        imageSize = width * height * 3;
    }
    if (dataPos == 0) {
        dataPos = 54;
    }

    data = new unsigned char[imageSize];
    fseek(file, dataPos, SEEK_SET);
    fread(data, 1, imageSize, file);
    fclose(file);

    for (i = 0; i < imageSize; i += 3) {
        tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;

    return id;
}

// ===================== RENDER TEXT ===================== //
void renderText(float x, float y, void* font, const char* string) {
    /* === KAMUS LOKAL === */
    const char *c;

    /* === ALGORITMA === */
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}
