#ifndef GLOBALS_H
#define GLOBALS_H

#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <math.h>
#include <vector>

// ===================== STRUCT ===================== //
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

// ===================== KAMERA ===================== //
extern int   viewMode;
extern float recoil;
extern float camAngleX;
extern float camAngleY;
extern float camDist;
extern float eyeOffset;
extern bool  isShooting;
extern bool  keys[256];

// ===================== POSISI PEMAIN ===================== //
extern float posX, posY, posZ;
extern float playerHealth;
extern bool  isDead;
extern int   lastDamageTime;
extern int   damageDelay;
extern bool  isWin;

// ===================== PISTOL ===================== //
extern int ammo;
extern int maxAmmo;

// ===================== WINDOW ===================== //
extern int winWidth, winHeight;

// ===================== MEJA ===================== //
extern int   numTables;
const  int   LIMIT_TABLES = 20;
extern float tableX[LIMIT_TABLES];
extern float tableZ[LIMIT_TABLES];
extern float tableY;
extern float tableW, tableH, tableD;
extern float tableFootW, tableFootH, tableFootD;

// ===================== POHON ===================== //
const  int   LIMIT_POHON = 20;
extern int   nbElmPohon;
extern Pohon listPohon[LIMIT_POHON];

// ===================== ZOMBIE ===================== //
extern int   numZombies;
const  int   LIMIT_ZOMBIES = 20;
extern float zomX[LIMIT_ZOMBIES];
extern float zomZ[LIMIT_ZOMBIES];
extern float zomY;
extern int   head;
extern int   rightShoulder;
extern int   leftShoulder;
extern float zomSpeed;
extern float zomAngleY[LIMIT_ZOMBIES];
extern float stopDistance;
extern float zombieHealth[LIMIT_ZOMBIES];

// ===================== MATAHARI ===================== //
extern float sunX, sunY, sunZ;

// ===================== TEXTURE ===================== //
extern GLuint textureFloor;
extern GLuint textureTable;
extern GLuint textureSky;
extern GLuint textureSkyWin;
extern GLuint textureSkyLose;

// ===================== A* PATHFINDING ===================== //
const int   GRID_W      = 201;
const int   GRID_H      = 201;
const float CELL_SIZE   = 0.5f;
const float WORLD_MIN_X = -50.0f;
const float WORLD_MIN_Z = -50.0f;

extern int pathGrid[GRID_W][GRID_H];
extern std::vector<PathNode> zombiePath[LIMIT_ZOMBIES];
extern int lastPathUpdate[LIMIT_ZOMBIES];

// ===================== MINI-MAP ===================== //
const float MAP_X            = 20.0f;
const float MAP_Y_FROM_TOP   = 220.0f;
const float MAP_SIZE         = 160.0f;
const float MAP_WORLD_RANGE  = 40.0f;

extern float g_mapX, g_mapY, g_mapSize, g_mapScale;

#endif

