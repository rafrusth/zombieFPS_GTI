//#define _USE_MATH_DEFINES
//#include <GL/glut.h>
//#include <math.h>
//#include <stdio.h>
//#include <iostream>
//#include <string>
//#include <vector>
//#include <queue>
//#include <algorithm>
//#include <cfloat>
// 
//// ===================== KAMUS GLOBAL ===================== //
//bool keys[256] = {false};
// 
///* === VARIABEL KAMERA === */
//int viewMode = 1;
//float recoil = 0.0f;
//float camAngleX = 0.0f;
//float camAngleY = 0.0f;
//bool isShooting = false;
//bool ignoreNextWarp = false;
// 
///* === POSISI KAMERA / PEMAIN === */
//float posX = 0.0f;
//float posY = 0.6f;
//float posZ = 5.0f;
// 
///* === PLAYER & GAME STATE === */
//float playerHealth = 100;
//bool isDead = false;
// 
///* === DAMAGE COOLDOWN === */
//int lastDamageTime = 0;
//int damageDelay = 1000; // ms
// 
///* === POSISI ZOMBIE === */
//float zomX = 0;
//float zomY = 0.5f;
//float zomZ = -5.0f;
//float zombieFacing = 0.0f;
// 
///* === POSE ZOMBIE === */
//static int head = 0;
//static int rightShoulder = -90;
//static int leftShoulder = -90;
// 
///* === POSISI MATAHARI === */
//float sunX = 0.0f;
//float sunY = 15.0f;
//float sunZ = -30.0f;
// 
//// ===================== A* PATHFINDING ===================== //
// 
///* === KONFIGURASI GRID === */
//const float CELL_SIZE   = 0.5f;
//const int   GRID_W      = 120;
//const int   GRID_H      = 120;
//const float GRID_ORIG_X = -30.0f;
//const float GRID_ORIG_Z = -30.0f;
// 
///* === STRUKTUR OBSTACLE (AABB) === */
//struct Obstacle {
//    float minX, maxX, minZ, maxZ;
//};
// 
///* === DATA OBSTACLE / GEDUNG === */
//// 4 gedung: {minX, maxX, minZ, maxZ}
//std::vector<Obstacle> obstacles;
// 
//void initObstacles() {
//    Obstacle o;
// 
//    o.minX=-8.0f; o.maxX=-4.0f; o.minZ=-8.0f; o.maxZ=-4.0f; obstacles.push_back(o); // Gedung 1
//    o.minX= 4.0f; o.maxX= 8.0f; o.minZ=-8.0f; o.maxZ=-4.0f; obstacles.push_back(o); // Gedung 2
//    o.minX=-8.0f; o.maxX=-4.0f; o.minZ= 4.0f; o.maxZ= 8.0f; obstacles.push_back(o); // Gedung 3
//    o.minX= 4.0f; o.maxX= 8.0f; o.minZ= 4.0f; o.maxZ= 8.0f; obstacles.push_back(o); // Gedung 4
//}
// 
///* === STRUKTUR NODE A* === */
//struct Node {
//    int x, z;
//    float g, f;
//    bool operator>(const Node& o) const { return f > o.f; }
//};
// 
//struct Vec2 { float x, z; };
// 
///* === KONVERSI DUNIA <-> GRID === */
//int worldToGridX(float wx) {
//    return (int)((wx - GRID_ORIG_X) / CELL_SIZE);
//}
//int worldToGridZ(float wz) {
//    return (int)((wz - GRID_ORIG_Z) / CELL_SIZE);
//}
//float gridToWorldX(int gx) {
//    return GRID_ORIG_X + gx * CELL_SIZE + CELL_SIZE * 0.5f;
//}
//float gridToWorldZ(int gz) {
//    return GRID_ORIG_Z + gz * CELL_SIZE + CELL_SIZE * 0.5f;
//}
// 
///* === CEK WALKABLE (tidak menabrak obstacle) === */
//bool isWalkable(int gx, int gz) {
//    if (gx < 0 || gx >= GRID_W || gz < 0 || gz >= GRID_H) return false;
// 
//    float wx = gridToWorldX(gx);
//    float wz = gridToWorldZ(gz);
//    float margin = 0.3f; // buffer agar zombie tidak terlalu mepet gedung
// 
//    for (int i = 0; i < (int)obstacles.size(); i++) {
//        if (wx > obstacles[i].minX - margin && wx < obstacles[i].maxX + margin &&
//            wz > obstacles[i].minZ - margin && wz < obstacles[i].maxZ + margin) {
//            return false;
//        }
//    }
//    return true;
//}
// 
///* === HEURISTIK EUCLIDEAN === */
//float heuristic(int ax, int az, int bx, int bz) {
//    float dx = (float)(ax - bx);
//    float dz = (float)(az - bz);
//    return sqrt(dx*dx + dz*dz);
//}
// 
///* === ALGORITMA A* === */
//std::vector<Vec2> findPath(float startX, float startZ,
//                            float goalX,  float goalZ) {
//    /* === KAMUS LOKAL === */
//    int sx, sz, gx, gz;
//    int dx[] = {1,-1,0,0, 1,1,-1,-1};
//    int dz[] = {0,0,1,-1, 1,-1,1,-1};
//    float dc[] = {1,1,1,1, 1.414f,1.414f,1.414f,1.414f};
// 
//    /* === ALGORITMA === */
//    sx = worldToGridX(startX); sz = worldToGridZ(startZ);
//    gx = worldToGridX(goalX);  gz = worldToGridZ(goalZ);
// 
//    // Clamp ke dalam grid
//    sx = std::max(0, std::min(GRID_W-1, sx));
//    sz = std::max(0, std::min(GRID_H-1, sz));
//    gx = std::max(0, std::min(GRID_W-1, gx));
//    gz = std::max(0, std::min(GRID_H-1, gz));
// 
//    // gScore & parent map (flat array lebih cepat dari vector 2D)
//    std::vector<float> gScore(GRID_W * GRID_H, FLT_MAX);
//    std::vector<int>   parent(GRID_W * GRID_H, -1);
// 
//    std::priority_queue<Node, std::vector<Node>, std::greater<Node> > open;
// 
//    gScore[sx * GRID_H + sz] = 0.0f;
//    Node startNode; startNode.x=sx; startNode.z=sz; startNode.g=0.0f; startNode.f=heuristic(sx,sz,gx,gz);
//    open.push(startNode);
// 
//    bool found = false;
// 
//    while (!open.empty()) {
//        Node cur = open.top(); open.pop();
// 
//        if (cur.x == gx && cur.z == gz) { found = true; break; }
//        if (cur.g > gScore[cur.x * GRID_H + cur.z]) continue;
// 
//        for (int i = 0; i < 8; i++) {
//            int nx = cur.x + dx[i];
//            int nz = cur.z + dz[i];
//            if (!isWalkable(nx, nz)) continue;
// 
//            float ng = gScore[cur.x * GRID_H + cur.z] + dc[i];
//            if (ng < gScore[nx * GRID_H + nz]) {
//                gScore[nx * GRID_H + nz] = ng;
//                parent[nx * GRID_H + nz] = cur.x * GRID_H + cur.z;
//                float f = ng + heuristic(nx, nz, gx, gz);
//                Node nb; nb.x=nx; nb.z=nz; nb.g=ng; nb.f=f;
//                open.push(nb);
//            }
//        }
//    }
// 
//    // Rekonstruksi path
//    std::vector<Vec2> path;
//    if (!found) return path;
// 
//    int cur = gx * GRID_H + gz;
//    int start = sx * GRID_H + sz;
//    while (cur != start && parent[cur] != -1) {
//        int cx = cur / GRID_H;
//        int cz = cur % GRID_H;
//        Vec2 wp; wp.x = gridToWorldX(cx); wp.z = gridToWorldZ(cz);
//        path.push_back(wp);
//        cur = parent[cur];
//    }
//    std::reverse(path.begin(), path.end());
//    return path;
//}
// 
///* === STATE PATH ZOMBIE === */
//std::vector<Vec2> zombiePath;
//int   pathIndex         = 0;
//float zombieSpeed       = 0.025f;
//int   lastPathUpdate    = 0;
//int   pathUpdateInterval = 600; // ms
// 
//// ===================== MATERIAL ===================== //
//void setMaterial(float r, float g, float b) {
//    GLfloat mat_ambient[] = {r * 0.3f, g * 0.3f, b * 0.3f, 1.0f};
//    GLfloat mat_diffuse[] = {r, g, b, 1.0f};
//    GLfloat mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};
//    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
//}
// 
//// ===================== CUBOID ===================== //
//void drawCuboid(float w, float h, float d,
//                float fR, float fG, float fB,
//                float sR, float sG, float sB,
//                float tR, float tG, float tB,
//                float bR, float bG, float bB) {
//    float x = w * 0.5f;
//    float y = h * 0.5f;
//    float z = d * 0.5f;
// 
//    glBegin(GL_QUADS);
//        glNormal3f(0,0,1);
//        glColor3f(fR, fG, fB);
//        glVertex3f(-x,-y,z); glVertex3f(x,-y,z);
//        glVertex3f(x,y,z);   glVertex3f(-x,y,z);
// 
//        glNormal3f(0,0,-1);
//        glColor3f(fR*0.8f, fG*0.8f, fB*0.8f);
//        glVertex3f(x,-y,-z);  glVertex3f(-x,-y,-z);
//        glVertex3f(-x,y,-z);  glVertex3f(x,y,-z);
// 
//        glNormal3f(-1,0,0);
//        glColor3f(sR, sG, sB);
//        glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z);
//        glVertex3f(-x,y,z);   glVertex3f(-x,y,-z);
// 
//        glNormal3f(1,0,0);
//        glColor3f(sR, sG, sB);
//        glVertex3f(x,-y,z);  glVertex3f(x,-y,-z);
//        glVertex3f(x,y,-z);  glVertex3f(x,y,z);
// 
//        glNormal3f(0,1,0);
//        glColor3f(tR, tG, tB);
//        glVertex3f(-x,y,z);  glVertex3f(x,y,z);
//        glVertex3f(x,y,-z);  glVertex3f(-x,y,-z);
// 
//        glNormal3f(0,-1,0);
//        glColor3f(bR, bG, bB);
//        glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z);
//        glVertex3f(x,-y,z);   glVertex3f(-x,-y,z);
//    glEnd();
//}
// 
//void drawCuboid(float w, float h, float d) {
//    float x = w * 0.5f;
//    float y = h * 0.5f;
//    float z = d * 0.5f;
// 
//    glBegin(GL_QUADS);
//        glNormal3f(0,0,1);
//        glVertex3f(-x,-y,z); glVertex3f(x,-y,z);
//        glVertex3f(x,y,z);   glVertex3f(-x,y,z);
// 
//        glNormal3f(0,0,-1);
//        glVertex3f(x,-y,-z);  glVertex3f(-x,-y,-z);
//        glVertex3f(-x,y,-z);  glVertex3f(x,y,-z);
// 
//        glNormal3f(-1,0,0);
//        glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z);
//        glVertex3f(-x,y,z);   glVertex3f(-x,y,-z);
// 
//        glNormal3f(1,0,0);
//        glVertex3f(x,-y,z);  glVertex3f(x,-y,-z);
//        glVertex3f(x,y,-z);  glVertex3f(x,y,z);
// 
//        glNormal3f(0,1,0);
//        glVertex3f(-x,y,z);  glVertex3f(x,y,z);
//        glVertex3f(x,y,-z);  glVertex3f(-x,y,-z);
// 
//        glNormal3f(0,-1,0);
//        glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z);
//        glVertex3f(x,-y,z);   glVertex3f(-x,-y,z);
//    glEnd();
//}
// 
//void drawFrontPatch(float px, float py, float pw, float ph,
//                    float z, float r, float g, float b) {
//    glColor3f(r, g, b);
//    glBegin(GL_QUADS);
//        glNormal3f(0,0,1);
//        glVertex3f(px,      py,      z);
//        glVertex3f(px + pw, py,      z);
//        glVertex3f(px + pw, py + ph, z);
//        glVertex3f(px,      py + ph, z);
//    glEnd();
//}
// 
//// ===================== BAGIAN ZOMBIE ===================== //
//void drawHeadZombie() {
//    float w=0.5f, h=0.45f, d=0.5f;
//    float z = d * 0.5f + 0.001f;
//    drawCuboid(w,h,d, 0.34f,0.52f,0.24f, 0.38f,0.56f,0.28f,
//               0.40f,0.58f,0.30f, 0.28f,0.42f,0.20f);
//    drawFrontPatch(-0.17f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
//    drawFrontPatch( 0.06f,-0.01f,0.12f,0.07f,z, 0.05f,0.05f,0.05f);
//    drawFrontPatch(-0.06f,-0.07f,0.12f,0.07f,z, 0.22f,0.34f,0.16f);
//    drawFrontPatch(-0.06f,-0.15f,0.12f,0.03f,z, 0.20f,0.30f,0.14f);
//}
// 
//void drawBodyZombie() {
//    float w=0.5f, h=0.67f, d=0.25f;
//    float z = d * 0.5f + 0.001f;
//    drawCuboid(w,h,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f,
//               0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
//    drawFrontPatch(-0.1f,0.27f,0.2f,0.06f,z, 0.30f,0.32f,0.12f);
//    drawFrontPatch(-0.05f,0.21f,0.1f,0.06f,z, 0.30f,0.32f,0.12f);
//}
// 
//void drawArmZombie() {
//    float w=0.25f, h=0.65f, d=0.25f;
//    float sleeveH=0.20f, armH=h-sleeveH;
//    glPushMatrix();
//        glTranslatef(0.0f,(h-sleeveH)*0.5f,0.0f);
//        drawCuboid(w,sleeveH,d, 0.08f,0.78f,0.82f, 0.06f,0.68f,0.72f,
//                   0.12f,0.82f,0.86f, 0.05f,0.55f,0.60f);
//    glPopMatrix();
//    glPushMatrix();
//        glTranslatef(0.0f,-(sleeveH*0.5f),0.0f);
//        drawCuboid(w,armH,d, 0.46f,0.60f,0.35f, 0.42f,0.56f,0.32f,
//                   0.48f,0.62f,0.37f, 0.38f,0.50f,0.28f);
//    glPopMatrix();
//}
// 
//void drawLegZombie() {
//    float w=0.25f, h=0.7f, d=0.18f;
//    glPushMatrix();
//        glTranslatef(0.0f,0.05f,0.0f);
//        drawCuboid(w,h,d, 0.34f,0.27f,0.78f, 0.30f,0.24f,0.70f,
//                   0.37f,0.30f,0.82f, 0.30f,0.24f,0.70f);
//    glPopMatrix();
//    glPushMatrix();
//        glTranslatef(0.0f,-0.30f,0.04f);
//        drawCuboid(0.27f,0.12f,0.28f, 0.45f,0.45f,0.45f, 0.40f,0.40f,0.40f,
//                   0.50f,0.50f,0.50f, 0.30f,0.30f,0.30f);
//    glPopMatrix();
//}
// 
//void drawBucketHat() {
//    GLUquadric* quad = gluNewQuadric();
//    gluQuadricNormals(quad, GLU_SMOOTH);
//    setMaterial(0.85f,0.85f,0.85f);
//    glPushMatrix();
//        gluCylinder(quad,0.22f,0.20f,0.22f,24,4);
//        glPushMatrix();
//            glTranslatef(0.0f,0.0f,0.22f);
//            gluDisk(quad,0.0f,0.22f,24,1);
//        glPopMatrix();
//    glPopMatrix();
//    glPushMatrix();
//        gluDisk(quad,0.20f,0.34f,24,1);
//    glPopMatrix();
//    glPushMatrix();
//        glTranslatef(0.0f,0.0f,-0.02f);
//        gluDisk(quad,0.18f,0.32f,24,1);
//    glPopMatrix();
//    glPushMatrix();
//        glTranslatef(0.0f,0.0f,-0.02f);
//        gluCylinder(quad,0.32f,0.34f,0.02f,24,1);
//    glPopMatrix();
//    gluDeleteQuadric(quad);
//}
// 
//void drawZombie() {
//    glPushMatrix();
//        glPushMatrix();
//            glTranslatef(0.0f,0.3f,0.0f);
//            glRotatef((GLfloat)head,0.0f,0.0f,1.0f);
//            glTranslatef(0.0f,0.3f,0.0f);
//            glScalef(0.9f,0.9f,0.9f);
//            glShadeModel(GL_FLAT);
//            drawHeadZombie();
//            glPushMatrix();
//                glTranslatef(0.0f,0.25f,0.0f);
//                glRotatef(-90.0f,1.0f,0.0f,0.0f);
//                glShadeModel(GL_SMOOTH);
//                drawBucketHat();
//                glShadeModel(GL_FLAT);
//            glPopMatrix();
//        glPopMatrix();
// 
//        glPushMatrix();
//            glShadeModel(GL_FLAT);
//            glTranslatef(0,0.07f,0);
//            drawBodyZombie();
//        glPopMatrix();
// 
//        glPushMatrix();
//            glTranslatef(0.36f,0.32f,-0.1f);
//            glRotatef((GLfloat)rightShoulder,1.0f,0.0f,0.0f);
//            glTranslatef(0.0f,-0.32f,0.0f);
//            drawArmZombie();
//        glPopMatrix();
// 
//        glPushMatrix();
//            glTranslatef(-0.36f,0.32f,-0.1f);
//            glRotatef((GLfloat)leftShoulder,1.0f,0.0f,0.0f);
//            glTranslatef(0.0f,-0.32f,0.0f);
//            drawArmZombie();
//        glPopMatrix();
// 
//        glPushMatrix();
//            glTranslatef(0.125f,-0.65f,0.0f);
//            drawLegZombie();
//        glPopMatrix();
// 
//        glPushMatrix();
//            glTranslatef(-0.125f,-0.65f,0.0f);
//            drawLegZombie();
//        glPopMatrix();
//    glPopMatrix();
//}
// 
//// ===================== UPDATE ZOMBIE (A*) ===================== //
//void updateZombie() {
//    /* === KAMUS LOKAL === */
//    int now;
//    float dx, dz, dist;
//    float fdx, fdz;
//    static float walkCycle = 0.0f;
// 
//    /* === ALGORITMA === */
//    if (isDead) return;
// 
//    now = glutGet(GLUT_ELAPSED_TIME);
// 
//    // Recalculate path A* setiap interval
//    if (now - lastPathUpdate > pathUpdateInterval) {
//        zombiePath   = findPath(zomX, zomZ, posX, posZ);
//        pathIndex    = 0;
//        lastPathUpdate = now;
//    }
// 
//    // Ikuti waypoint A*
//    if (!zombiePath.empty() && pathIndex < (int)zombiePath.size()) {
//        Vec2 target = zombiePath[pathIndex];
//        dx = target.x - zomX;
//        dz = target.z - zomZ;
//        dist = sqrt(dx*dx + dz*dz);
// 
//        if (dist < CELL_SIZE * 0.5f) {
//            pathIndex++;
//        } else {
//            zomX += (dx / dist) * zombieSpeed;
//            zomZ += (dz / dist) * zombieSpeed;
//        }
//    }
// 
//    // Hitung jarak ke pemain
//    fdx = posX - zomX;
//    fdz = posZ - zomZ;
//    float distToPlayer = sqrt(fdx*fdx + fdz*fdz);
// 
//    // Animasi berjalan
//    if (distToPlayer > 0.3f) {
//        walkCycle += 0.05f;
//        rightShoulder = (int)(-90 + 60.0f * sin(walkCycle));
//        leftShoulder  = (int)(-90 - 60.0f * sin(walkCycle));
//        head          = (int)(10.0f * sin(walkCycle * 0.5f));
//    }
// 
//    // Rotasi zombie menghadap pemain
//    if (fabs(fdx) > 0.001f || fabs(fdz) > 0.001f) {
//        zombieFacing = atan2(fdx, fdz) * 180.0f / (float)M_PI;
//    }
//}
// 
//// ===================== ZOMBIE ATTACK ===================== //
//void zombieAttack() {
//    /* === KAMUS LOKAL === */
//    float dx, dy, dz, dist;
//    int currentTime;
// 
//    /* === ALGORITMA === */
//    if (!isDead) {
//        dx = posX - zomX;
//        dy = posY - zomY;
//        dz = posZ - zomZ;
//        dist = sqrt((dx*dx) + (dy*dy) + (dz*dz));
// 
//        currentTime = glutGet(GLUT_ELAPSED_TIME);
//        if (dist <= 0.4f) {
//            if (currentTime - lastDamageTime > damageDelay) {
//                playerHealth -= 10;
//                lastDamageTime = currentTime;
//                if (playerHealth <= 0) {
//                    isDead = true;
//                }
//            }
//        }
//    } else {
//        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
//    }
//}
// 
//// ===================== GEDUNG / OBSTACLE ===================== //
//void drawBuilding(float minX, float maxX, float minZ, float maxZ,
//                  float height,
//                  float wallR, float wallG, float wallB,
//                  float roofR, float roofG, float roofB) {
//    /* === KAMUS LOKAL === */
//    float cx, cz, w, d, scaleY;
// 
//    /* === ALGORITMA === */
//    cx     = (minX + maxX) * 0.5f;
//    cz     = (minZ + maxZ) * 0.5f;
//    w      = maxX - minX;
//    d      = maxZ - minZ;
//    scaleY = height;
// 
//    // Dinding utama gedung
//    glColor3f(wallR, wallG, wallB);
//    glPushMatrix();
//        glTranslatef(cx, height * 0.5f - 1.0f, cz);
//        glScalef(w, scaleY, d);
//        glutSolidCube(1.0f);
//    glPopMatrix();
// 
//    // Atap gedung (sedikit lebih lebar, lebih tipis)
//    glColor3f(roofR, roofG, roofB);
//    glPushMatrix();
//        glTranslatef(cx, height - 1.0f, cz);
//        glScalef(w + 0.3f, 0.3f, d + 0.3f);
//        glutSolidCube(1.0f);
//    glPopMatrix();
//}
// 
//void drawAllBuildings() {
//    /* === KAMUS LOKAL === */
// 
//    /* === ALGORITMA === */
//    // Gedung 1 - abu tua
//    drawBuilding(-8,-4, -8,-4, 5.0f,
//                 0.55f,0.55f,0.58f, 0.40f,0.40f,0.43f);
// 
//    // Gedung 2 - coklat bata
//    drawBuilding(4,8, -8,-4, 6.0f,
//                 0.72f,0.42f,0.28f, 0.55f,0.30f,0.18f);
// 
//    // Gedung 3 - hijau tua
//    drawBuilding(-8,-4, 4,8, 4.5f,
//                 0.30f,0.50f,0.35f, 0.22f,0.38f,0.26f);
// 
//    // Gedung 4 - biru gelap
//    drawBuilding(4,8, 4,8, 7.0f,
//                 0.25f,0.35f,0.60f, 0.18f,0.25f,0.48f);
//}
// 
//// ===================== PISTOL ===================== //
//void drawPistol() {
//    setMaterial(0.15f,0.15f,0.15f);
//    drawCuboid(0.08f,0.35f,0.12f);
// 
//    glPushMatrix();
//        glTranslatef(0.0f,-0.18f,0.0f);
//        setMaterial(0.1f,0.1f,0.1f);
//        drawCuboid(0.09f,0.04f,0.14f);
//    glPopMatrix();
// 
//    glPushMatrix();
//        glTranslatef(0.0f,0.20f,-0.15f);
//        setMaterial(0.3f,0.3f,0.3f);
//        drawCuboid(0.1f,0.12f,0.5f);
//        glPushMatrix();
//            glTranslatef(0.0f,0.08f,0.2f);
//            setMaterial(0.1f,0.1f,0.1f);
//            drawCuboid(0.04f,0.04f,0.04f);
//        glPopMatrix();
//        glPushMatrix();
//            glTranslatef(0.0f,0.07f,-0.22f);
//            drawCuboid(0.02f,0.03f,0.03f);
//        glPopMatrix();
//        glPushMatrix();
//            glTranslatef(0.0f,-0.08f,0.05f);
//            drawCuboid(0.03f,0.12f,0.03f);
//            glTranslatef(0.0f,-0.05f,0.05f);
//            drawCuboid(0.03f,0.03f,0.1f);
//        glPopMatrix();
//    glPopMatrix();
//}
// 
//// ===================== HAND HUD ===================== //
//void drawHand(float xPos, float rotation, float currentRecoil) {
//    glPushMatrix();
//        glLoadIdentity();
//        glTranslatef(xPos, -0.6f + currentRecoil, -1.5f);
//        glRotatef(-1, 1, 0, 0);
//        glRotatef(rotation, 0, 1, 0);
//        setMaterial(0.8f,0.6f,0.4f);
//        drawCuboid(0.3f,0.3f,0.8f);
//        if (xPos > 0.0f) {
//            glPushMatrix();
//                glTranslatef(0.0f,0.15f,-0.35f);
//                glRotatef(1,1,0,0);
//                drawPistol();
//            glPopMatrix();
//        }
//    glPopMatrix();
//}
// 
//// ===================== INIT ===================== //
//void init() {
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_NORMALIZE);
//    glEnable(GL_COLOR_MATERIAL);
//    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
// 
//    GLfloat global_ambient[] = {0.3f,0.3f,0.3f,1.0f};
//    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
// 
//    GLfloat spec[] = {0.8f,0.8f,0.8f,1.0f};
//    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
// 
//    glClearColor(0.5f,0.8f,1.0f,1.0f);
// 
//    glEnable(GLUT_MULTISAMPLE);
//    glEnable(GL_LINE_SMOOTH);
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}
// 
//// ===================== DISPLAY ===================== //
//void renderText(float x, float y, void* font, const char* string) {
//    const char* c;
//    glRasterPos2f(x, y);
//    for (c = string; *c != '\0'; c++) {
//        glutBitmapCharacter(font, *c);
//    }
//}
// 
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
// 
//    switch(viewMode) {
//        case 1:
//            glRotatef(camAngleX,1,0,0);
//            glRotatef(camAngleY,0,1,0);
//            glTranslatef(-posX,-posY,-posZ);
//            break;
//        case 2:
//            gluLookAt(8.0f+posX,posY,8.0f+posZ, 0,0,-8, 0,1,0);
//            glRotatef(camAngleY,0,1,0);
//            break;
//        case 3:
//            gluLookAt(8.0f+posX,8.0f+posY,8.0f+posZ, 0,0,-8, 0,1,0);
//            glRotatef(camAngleX,1,0,0);
//            glRotatef(camAngleY,0,1,0);
//            break;
//    }
// 
//    GLfloat light_pos[] = {sunX,sunY,sunZ,1.0f};
//    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
// 
//    // Matahari
//    glPushMatrix();
//        glDisable(GL_LIGHTING);
//        glTranslatef(sunX,sunY,sunZ);
//        glColor3f(1.0f,1.0f,0.0f);
//        glutSolidSphere(3.0f,20,20);
//        glEnable(GL_LIGHTING);
//    glPopMatrix();
// 
//    // Lantai
//    glDisable(GL_COLOR_MATERIAL);
//    setMaterial(0.55f,0.70f,0.40f); // warna rumput
//    glBegin(GL_QUADS);
//        glNormal3f(0,1,0);
//        glVertex3f(-100,-1,-100);
//        glVertex3f(-100,-1, 100);
//        glVertex3f( 100,-1, 100);
//        glVertex3f( 100,-1,-100);
//    glEnd();
// 
//    // Gedung
//    glEnable(GL_COLOR_MATERIAL);
//    drawAllBuildings();
// 
//    // Zombie — dengan rotasi menghadap pemain
//    glPushMatrix();
//        glTranslatef(zomX, zomY, zomZ);
//        glRotatef(zombieFacing, 0, 1, 0); // rotasi menghadap pemain
//        glScalef(0.2f, 0.2f, 0.2f);
//        drawZombie();
//    glPopMatrix();
// 
//    // HUD tangan hanya untuk First Person
//    if (viewMode == 1) {
//        glClear(GL_DEPTH_BUFFER_BIT);
//        glDisable(GL_COLOR_MATERIAL);
//        drawHand(0.7f, 15.0f, recoil);
//    }
// 
//    // HUD overlay (health, game over)
//    glDisable(GL_LIGHTING);
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//        glLoadIdentity();
//        gluOrtho2D(0, 1280, 0, 720);
//        glMatrixMode(GL_MODELVIEW);
//        glPushMatrix();
//            glLoadIdentity();
// 
//            // Health bar background
//            glColor3f(0.2f,0.2f,0.2f);
//            glBegin(GL_QUADS);
//                glVertex2f(48,672); glVertex2f(248,672);
//                glVertex2f(248,692); glVertex2f(48,692);
//            glEnd();
// 
//            // Health bar isi
//            float barW = 200.0f * (playerHealth / 100.0f);
//            if (playerHealth >= 70)      glColor3f(0.0f,1.0f,0.0f);
//            else if (playerHealth > 30)  glColor3f(1.0f,1.0f,0.0f);
//            else                         glColor3f(1.0f,0.0f,0.0f);
//            glBegin(GL_QUADS);
//                glVertex2f(48,672); glVertex2f(48+barW,672);
//                glVertex2f(48+barW,692); glVertex2f(48,692);
//            glEnd();
// 
//            // Teks health
//            char healthText[50];
//            sprintf(healthText, "Health: %.0f", playerHealth);
//            glColor3f(1.0f,1.0f,1.0f);
//            renderText(50, 696, GLUT_BITMAP_9_BY_15, healthText);
// 
//            // Crosshair
//            glColor3f(1.0f,1.0f,1.0f);
//            glLineWidth(1.5f);
//            glBegin(GL_LINES);
//                glVertex2f(635,360); glVertex2f(645,360);
//                glVertex2f(640,355); glVertex2f(640,365);
//            glEnd();
// 
//            if (isDead) {
//                // Layar merah semi-transparan
//                glColor4f(0.5f,0.0f,0.0f,0.5f);
//                glBegin(GL_QUADS);
//                    glVertex2f(0,0); glVertex2f(1280,0);
//                    glVertex2f(1280,720); glVertex2f(0,720);
//                glEnd();
//                glColor3f(1.0f,1.0f,1.0f);
//                renderText(565, 370, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
//                renderText(530, 340, GLUT_BITMAP_9_BY_15, "Press ESC to exit");
//            }
// 
//        glPopMatrix();
//        glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
//    glEnable(GL_LIGHTING);
// 
//    glutSwapBuffers();
//}
// 
//// ===================== KEYBOARD ===================== //
//void keyboard(unsigned char key, int x, int y) {
//    if (key == 27) exit(0);
//    keys[key] = true;
//}
// 
//void keyboardUp(unsigned char key, int x, int y) {
//    keys[key] = false;
//}
// 
//void updateMovement() {
//    /* === KAMUS LOKAL === */
//    float fwdX, fwdZ, rightX, rightZ, rad, speed;
// 
//    /* === ALGORITMA === */
//    rad    = camAngleY * M_PI / 180.0f;
//    fwdX   =  sin(rad);
//    fwdZ   = -cos(rad);
//    rightX =  cos(rad);
//    rightZ =  sin(rad);
//    speed  = 0.05f;
// 
//    if (keys['w']) { posX += fwdX  * speed; posZ += fwdZ  * speed; }
//    if (keys['s']) { posX -= fwdX  * speed; posZ -= fwdZ  * speed; }
//    if (keys['a']) { posX -= rightX * speed; posZ -= rightZ * speed; }
//    if (keys['d']) { posX += rightX * speed; posZ += rightZ * speed; }
// 
//    if (keys['1']) viewMode = 1;
//    if (keys['2']) viewMode = 2;
//    if (keys['3']) viewMode = 3;
//}
// 
//// ===================== TIMER ===================== //
//void timer(int v) {
//    updateMovement();
//    updateZombie();
//    zombieAttack();
// 
//    if (isShooting) {
//        recoil -= 0.05f;
//        if (recoil < -0.3f) recoil = 0;
//    } else {
//        recoil = 0;
//    }
//    glutPostRedisplay();
//    glutTimerFunc(16, timer, 0);
//}
// 
//// ===================== MOUSE ===================== //
//void mouseMove(int x, int y) {
//    /* === KAMUS LOKAL === */
//    int cx = 500, cy = 350;
//    static int lastX = 500, lastY = 350;
//    static bool first = true;
// 
//    /* === ALGORITMA === */
//    if (first) { lastX = x; lastY = y; first = false; return; }
// 
//    int dx = x - lastX;
//    int dy = y - lastY;
//    lastX = x; lastY = y;
// 
//    camAngleY += dx * 0.2f;
//    camAngleX += dy * 0.2f;
//    if (camAngleX >  85.0f) camAngleX =  85.0f;
//    if (camAngleX < -85.0f) camAngleX = -85.0f;
// 
//    if (x < 100 || x > 900 || y < 100 || y > 600) {
//        lastX = cx; lastY = cy;
//        glutWarpPointer(cx, cy);
//    }
//}
// 
//void mouseClick(int b, int s, int x, int y) {
//    if (b == GLUT_LEFT_BUTTON) isShooting = (s == GLUT_DOWN);
//}
// 
//// ===================== RESHAPE ===================== //
//void reshape(int width, int height) {
//    if (height == 0) height = 1;
//    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.01, 200.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//}
// 
//// ===================== MAIN ===================== //
//int main(int argc, char** argv) {
//    /* === KAMUS === */
//    int screenWidth, screenHeight;
//    int windowWidth = 1280, windowHeight = 720;
// 
//    /* === ALGORITMA === */
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
//    glutInitWindowSize(windowWidth, windowHeight);
//    screenWidth  = glutGet(GLUT_SCREEN_WIDTH);
//    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
//    glutInitWindowPosition((screenWidth  - windowWidth)  / 2,
//                           (screenHeight - windowHeight) / 2);
//    glutCreateWindow("Zombie FPS - A* Pathfinding");
// 
//    glutWarpPointer(500, 350);
//    initObstacles();
//    init();
//    rightShoulder = -90;
//    leftShoulder  = -90;
// 
//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//    glutKeyboardFunc(keyboard);
//    glutKeyboardUpFunc(keyboardUp);
//    glutPassiveMotionFunc(mouseMove);
//    glutMotionFunc(mouseMove);
//    glutMouseFunc(mouseClick);
//    glutTimerFunc(0, timer, 0);
//    glutSetCursor(GLUT_CURSOR_NONE);
// 
//    glutMainLoop();
//    return 0;
//}
