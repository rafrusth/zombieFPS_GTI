/*  Nama File   :   ZombieVerse
    Matakuliah  :   Grafik dan Teknik Interaktif 
    Kelompok    :   6
    Anggota     :   Rafif Setya Imaduddin  24060124130115
                    Raffi Arditama         24060124120020
                    Felicia Evelina        24060124120012
                    Anggita Kirana Puspa   24060124130064
    Keyboard    :   W/A/S/D    = Gerak
                    1-4        = Mode Proyeksi
                    = / -      = Zoom In/Out
                    Mouse Kiri = Tembak
                    E		   = Reload Senjata
                    R          = Restart (saat Game Over atau Menang)
                    ESC        = Keluar
*/

#include "globals.h"
#include "render.h"
#include "aabb.h"
#include "pathfinding.h"
#include "hud.h"
#include "game.h"
#include <stdio.h>
#include <algorithm>
#include <cmath>

// ===================== DISPLAY ===================== //
void display() {
    /* === KAMUS LOKAL === */
    int i;
    float rad, eyeX, eyeY, eyeZ, dist4, hpPerc;
    
    /* === ALGORITMA === */
    // Background merah saat mati
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
            glTranslatef(-posX, -(posY + eyeOffset), -posZ);
            break;

        case 2:
            rad = camAngleY * (float) M_PI / 180.0f;
            eyeX = posX - sin(rad) * camDist;
            eyeY = posY;
            eyeZ = posZ + cos(rad) * camDist;
            gluLookAt(eyeX,eyeY,eyeZ, posX,posY,posZ, 0,1,0);
            break;
        
        case 3:
            rad = camAngleY * (float) M_PI / 180.0f;
            eyeX = posX - sin(rad) * camDist;
            eyeY = posY + camDist;
            eyeZ = posZ + cos(rad) * camDist;
            gluLookAt(eyeX,eyeY,eyeZ, posX,posY,posZ, 0,1,0);
            break;
        
        case 4:
            rad = camAngleY * (float) M_PI / 180.0f;
            dist4 = 10.0f;
            eyeX = posX - sin(rad) * dist4;
            eyeY = posY + 10.0f;
            eyeZ = posZ + cos(rad) * dist4;
            gluLookAt(eyeX,eyeY,eyeZ, posX,posY,posZ, 0,1,0);
            break;
    }

    GLfloat light_pos[] = {sunX,sunY,sunZ,1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	
	drawSky();
	
    // Matahari
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(sunX,sunY,sunZ);
        glColor3f(1,1,0);
        glutSolidSphere(3,20,20);
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
    for (i = 0;i < numTables;i++) {
        glPushMatrix();
            glTranslatef(tableX[i],tableY-0.5f,tableZ[i]);
            drawTable();
        glPopMatrix();
    }

    // Pohon
    glDisable(GL_COLOR_MATERIAL);
    for (i = 0;i < nbElmPohon;i++) {
        drawPohon(listPohon[i]);
    }

    // Zombie
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    for (i = 0; i < numZombies; i++) {
        glPushMatrix();
            glTranslatef(zomX[i],zomY,zomZ[i]);
            glRotatef(zomAngleY[i]+180.0f,0,1,0);
            glScalef(0.5f,0.5f,0.5f);
            drawZombie();
        glPopMatrix();
        
        // === HEALTH BAR ZOMBIE ===
		hpPerc = zombieHealth[i] / 100.0f;
		if (hpPerc < 0) {
            hpPerc = 0;
        }

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);

		glPushMatrix();
            glTranslatef(zomX[i], zomY + 1.4f, zomZ[i]);
            glRotatef(-camAngleY, 0, 1, 0);

            // Background abu-abu
            glColor3f(0.3f, 0.3f, 0.3f);
            glBegin(GL_QUADS);
                glVertex3f(-0.35f, 0.0f, 0);
                glVertex3f( 0.35f, 0.0f, 0);
                glVertex3f( 0.35f, 0.08f, 0);
                glVertex3f(-0.35f, 0.08f, 0);
            glEnd();
            
            // Warna HP
            if (zombieHealth[i] <= 0) {
                glColor3f(0.5f, 0.5f, 0.5f); // abu-abu mati
            } else if (hpPerc > 0.6f) {
                glColor3f(0.0f, 1.0f, 0.0f); // hijau
            } else if (hpPerc > 0.3f) {
                glColor3f(1.0f, 0.5f, 0.0f); // oranye
            } else {
                glColor3f(1.0f, 0.0f, 0.0f); // merah
            }

            glBegin(GL_QUADS);
                glVertex3f(-0.35f, 0.0f, 0.01f);
                glVertex3f(-0.35f + (0.7f * hpPerc), 0.0f, 0.01f);
                glVertex3f(-0.35f + (0.7f * hpPerc), 0.08f, 0.01f);
                glVertex3f(-0.35f, 0.08f, 0.01f);
            glEnd();

		glPopMatrix();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
    }

    // Player / Hand HUD
    if (viewMode == 1) {
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
            drawHUD();
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
    if (key == 27) {
        exit(0);
    }
	
	// Tombol E: reload ammo
	if (key == 'e' || key == 'E') {
        ammo = maxAmmo;
    }
    
    // Tombol R: restart hanya saat Game Over atau Menang
    if ((key == 'r' || key == 'R') && (isDead || isWin)) {
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
    float overlapX, overlapZ;
    float rad, fwdX, fwdZ, rightX, rightZ, speed, pastDist; 
    int t, p, pastMode;
    AABB playerBox, table, pohon;

    /* === ALGORITMA === */
    rad = camAngleY * (float) M_PI / 180.0f;
    fwdX = sin(rad);
    fwdZ = -cos(rad);
    rightX = cos(rad);
    rightZ = sin(rad);
    speed = 0.02f;

    // Collision player vs meja
    playerBox = getPlayerAABB();
    for (t = 0; t < numTables; t++) {
        table = getTableAABB(t);
        if (checkAABB(playerBox,table)) {
            overlapX = std::min(playerBox.maxX-table.minX, table.maxX-playerBox.minX);
            overlapZ = std::min(playerBox.maxZ-table.minZ, table.maxZ-playerBox.minZ);
            if (overlapX < overlapZ) {
                if (posX < tableX[t]) {
                    posX -= overlapX;
                } else {
                    posX += overlapX;
                }
            } else {
                if (posZ < tableZ[t]) {
                    posZ -= overlapZ;
                } else {
                    posZ += overlapZ;
                }
            }
        }
    }

    // Collision player vs pohon
    for (p = 0; p < nbElmPohon; p++) {
        pohon = getPohonAABB(p);
        playerBox = getPlayerAABB();
        if (checkAABB(playerBox,pohon)) {
            overlapX = std::min(playerBox.maxX-pohon.minX, pohon.maxX-playerBox.minX);
            overlapZ = std::min(playerBox.maxZ-pohon.minZ, pohon.maxZ-playerBox.minZ);
            if (overlapX < overlapZ) {                
                if (posX < listPohon[p].posX) {
                    posX -= overlapX;
                } else {
                    posX += overlapX;
                }
            } else {
                if (posZ < listPohon[p].posZ) {
                    posZ -= overlapZ;
                } else {
                    posZ += overlapZ;
                }
            }
        }
    }

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

    pastMode = viewMode;
    pastDist = camDist;
    
    if (keys['1']) {
        viewMode = 1;
    }

    if (keys['2']) {
        viewMode = 2;
    }

    if (keys['3']) {
        viewMode = 3;
    }

    if (keys['4']) {
        viewMode = 4;
    }

    if (keys['='] && camDist > 1.1f && viewMode != 1) {
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
    /* === KAMUS LOKAL === */
    int currentTime, i;

    /* === ALGORITMA === */
    updateMovement();

    currentTime = glutGet(GLUT_ELAPSED_TIME);
    for (i = 0; i < numZombies; i++) {
        if (currentTime - lastPathUpdate[i] > 500 + i * 35) {
            calculateZombiePath(i);
            lastPathUpdate[i] = currentTime;
        }
        updateZombie(i);
    }

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
    static int centerX = 640, centerY = 360;
    int dx, dy;

    /* === ALGORITMA === */
    if (x == centerX && y == centerY) {
        return; 
    }

    dx = x - centerX;
    dy = y - centerY;

    camAngleY += dx * 0.2f;
    camAngleX += dy * 0.2f;

    if (camAngleX > 85) {
        camAngleX = 85;
    }
    if (camAngleX < -85) {
        camAngleX = -85;
    }

    glutWarpPointer(centerX, centerY);
}

void rapidFireTimer(int value) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (!isShooting || ammo <= 0) {
        isShooting = false;
        return; 
    }

    shoot();
    
    glutPostRedisplay();
    glutTimerFunc(100, rapidFireTimer, 0);
}

void mouseClick(int b, int s, int x, int y) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (b == GLUT_LEFT_BUTTON) {
        if (s == GLUT_DOWN) {
            if (!isShooting) {
                isShooting = true;
                shoot();
                glutTimerFunc(100, rapidFireTimer, 0); 
            }
        } else if (s == GLUT_UP) {
            isShooting = false; 
        }
    }
}

// ===================== RESHAPE ===================== //
void reshape(int width, int height) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    if (height == 0) {
        height = 1;
    }

    winWidth = width;
    winHeight = height;

    glViewport(0,0,width,height);
    applyProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ===================== MAIN ===================== //
int main(int argc, char** argv) {
    /* === KAMUS === */
    int screenWidth, screenHeight;
    int windowWidth, windowHeight;

    /* === ALGORITMA === */
    windowWidth = 1280;
    windowHeight = 720;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth - windowWidth) / 2,(screenHeight - windowHeight) / 2);
    glutCreateWindow("ZombieVerse");
    glutWarpPointer(500,350);

    initGame();

    rightShoulder = -90;
    leftShoulder = -90;

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
