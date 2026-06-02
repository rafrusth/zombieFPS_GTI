#include "globals.h"

// ===================== KAMERA ===================== //
int   viewMode   = 1;
float recoil     = 0.0f;
float camAngleX  = 0.0f;
float camAngleY  = 0.0f;
float camDist    = 3.0f;
float eyeOffset  = 0.13f;
bool  isShooting = false;
bool  keys[256]  = {false};

// ===================== PEMAIN ===================== //
float posX = 0.0f;
float posY = -0.37f;
float posZ = 5.0f;
float playerHealth = 100.0f;
bool  isDead       = false;
int   lastDamageTime = 0;
int   damageDelay    = 1000;
bool  isWin = false;

// ===================== PISTOL ===================== //
int ammo = 12;
int maxAmmo = 12;

// ===================== WINDOW ===================== //
int winWidth  = 1280;
int winHeight = 720;

// ===================== MEJA ===================== //
int   numTables = 0;
float tableX[LIMIT_TABLES];
float tableZ[LIMIT_TABLES];
float tableY      = 0.0f;
float tableW      = 0.5f;
float tableH      = 0.08f;
float tableD      = 1.0f;
float tableFootW  = 0.1f;
float tableFootH  = 0.48f;
float tableFootD  = 0.1f;

// ===================== POHON ===================== //
int   nbElmPohon = 0;
Pohon listPohon[LIMIT_POHON];

// ===================== ZOMBIE ===================== //
int   numZombies  = 0;
float zomX[LIMIT_ZOMBIES];
float zomZ[LIMIT_ZOMBIES];
float zomY        = -0.52f;
int   head        = 0;
int   rightShoulder = -90;
int   leftShoulder  = -90;
float zomSpeed    = 0.03f;
float zomAngleY[LIMIT_ZOMBIES];
float stopDistance = 0.4f;
float zombieHealth[LIMIT_ZOMBIES];

// ===================== MATAHARI ===================== //
float sunX = 0.0f;
float sunY = 15.0f;
float sunZ = -30.0f;

// ===================== TEXTURE ===================== //
GLuint textureFloor = 0;
GLuint textureTable = 0;
GLuint textureSky   = 0;
GLuint textureSkyWin= 0;
GLuint textureSkyLose= 0;

// ===================== A* ===================== //
int pathGrid[GRID_W][GRID_H];
std::vector<PathNode> zombiePath[LIMIT_ZOMBIES];
int lastPathUpdate[LIMIT_ZOMBIES];

// ===================== MINI-MAP ===================== //
float g_mapX = 0, g_mapY = 0, g_mapSize = 0, g_mapScale = 0;

