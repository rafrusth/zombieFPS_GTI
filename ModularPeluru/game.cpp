#include "game.h"
#include "aabb.h"
#include "render.h"
#include "pathfinding.h"
#include <cstdlib>
#include <ctime>

// ===================== HELPER POHON ===================== //
// Fungsi biasa (bukan lambda) supaya kompatibel Dev-C++ / MinGW lama
static void makePohon(float x, float y, float z,
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

// ===================== INIT ===================== //
void initGame() {
    int i;
    srand(time(NULL));

    // Spawn meja
    numTables = (rand() % 10) + 3;
    for (i=0; i<numTables; i++) {
        tableX[i] = (rand()%40) - 10.0f;
        tableZ[i] = (rand()%40) - 20.0f;
    }

    // Spawn pohon (posisi tetap)
    nbElmPohon = 0;
    makePohon(-4.0f,-1.0f,-8.0f,  0.35f,1.8f,0.35f);
    makePohon( 3.0f,-1.0f,-12.0f, 0.35f,1.8f,0.35f);
    makePohon( 7.0f,-1.0f,-5.0f,  0.35f,1.8f,0.35f);
    makePohon(-8.0f,-1.0f,-18.0f, 0.35f,1.8f,0.35f);
    makePohon(12.0f,-1.0f,-22.0f, 0.35f,1.8f,0.35f);

    buildPathGrid();

    // Spawn zombie di cell yang bisa dilewati
    numZombies = 5;
    for (i=0; i<numZombies; i++) {
        int gx, gz, tries=0;
        do {
            zomX[i] = (rand()%40) - 20.0f;
            zomZ[i] = (rand()%40) - 30.0f;
            tries++;
            worldToGrid(zomX[i], zomZ[i], gx, gz);
        } while ((!worldToGrid(zomX[i],zomZ[i],gx,gz) ||
                  !isWalkable(gx,gz) ||
                  positionHitsTable(zomX[i],zomZ[i])) && tries < 100);

        zomAngleY[i]     = 0.0f;
        lastPathUpdate[i] = 0;
        zombieHealth[i] = 100.0f;
        calculateZombiePath(i);
    }

    // Load tekstur
    textureFloor = loadBMPTexture("lantai.bmp");
    textureTable = loadBMPTexture("meja.bmp");
    textureSky = loadBMPTexture("langit.bmp");
	textureSkyWin = loadBMPTexture("langitsenja.bmp");
	textureSkyLose = loadBMPTexture("langitmerah.bmp");

    // OpenGL state
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

// ===================== RESET GAME (RESTART) ===================== //
void resetGame() {
    int i;

    // Reset status pemain
    posX=0.0f; posY=-0.37f; posZ=5.0f;
    playerHealth  = 100.0f;
    isDead        = false;
    lastDamageTime= 0;
    isWin         = false;
    ammo          = maxAmmo;

    // Reset kamera
    camAngleX=0.0f; camAngleY=0.0f;
    camDist=3.0f; recoil=0.0f;
    isShooting=false; viewMode=1;

    // Bersihkan semua key yang masih tertekan
    for (i=0; i<256; i++) keys[i]=false;

    // Spawn ulang meja
    numTables = (rand()%10)+3;
    for (i=0; i<numTables; i++) {
        tableX[i]=(rand()%40)-10.0f;
        tableZ[i]=(rand()%40)-20.0f;
    }

    // Rebuild path grid dulu sebelum spawn zombie
    buildPathGrid();

    // Spawn ulang zombie
    numZombies = 5;
    for (i=0; i<numZombies; i++) {
        int gx, gz, tries=0;
        do {
            zomX[i]=(rand()%40)-20.0f;
            zomZ[i]=(rand()%40)-30.0f;
            tries++;
            worldToGrid(zomX[i],zomZ[i],gx,gz);
        } while ((!worldToGrid(zomX[i],zomZ[i],gx,gz) ||
                  !isWalkable(gx,gz) ||
                  positionHitsTable(zomX[i],zomZ[i])) && tries<100);

        zomAngleY[i]     = 0.0f;
        lastPathUpdate[i] = 0;
        zombieHealth[i] = 100.0f;
        zombiePath[i].clear();
        calculateZombiePath(i);
    }

    // Reset warna background ke langit biru
    glClearColor(0.53f,0.81f,0.98f,1.0f);
}

// ===================== ZOMBIE ATTACK ===================== //
void zombieAttack() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (isDead) return;

    AABB playerBox = getPlayerAABB();
    for (int i=0; i<numZombies; i++) {
        if (checkAABB(playerBox, getZombieAABB(i))) {
            if (currentTime - lastDamageTime > damageDelay) {
                playerHealth -= 10;
                lastDamageTime = currentTime;
                if (playerHealth <= 0) { isDead=true; break; }
            }
        }
    }
}

// ===================== SHOOT PISTOL ===================== //
void shoot() {
    if (isDead || isWin) return;
    if (ammo <= 0) return;

    ammo--;

    recoil = -0.3f;
    isShooting = true;

    float rad = camAngleY * 3.14159265f / 180.0f;
    float dirX = sin(rad);
    float dirZ = -cos(rad);

    for (int i = 0; i < numZombies; i++) {

        if (zombieHealth[i] <= 0)
            continue;

        float dx = zomX[i] - posX;
        float dz = zomZ[i] - posZ;

        float dist = sqrt(dx*dx + dz*dz);

        if (dist > 10.0f)
            continue;

        float dot =
            (dx * dirX + dz * dirZ) / dist;

        if (dot > 0.95f) {

            zombieHealth[i] -= 50.0f;

            if (zombieHealth[i] <= 0) {

                for (int j = i; j < numZombies - 1; j++) {
                    zomX[j] = zomX[j+1];
                    zomZ[j] = zomZ[j+1];
                    zomAngleY[j] = zomAngleY[j+1];
                    zombieHealth[j] = zombieHealth[j+1];
                    zombiePath[j] = zombiePath[j+1];
                    lastPathUpdate[j] = lastPathUpdate[j+1];
                }

                numZombies--;
                if (numZombies <= 0) {
    				isWin = true;
				}
            }

            break;
        }
    }
}

