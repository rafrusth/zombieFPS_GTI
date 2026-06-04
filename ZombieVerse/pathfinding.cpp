#include "pathfinding.h"
#include <queue>
#include <cstring>
#include <algorithm>
#include <cmath>

// ===================== KONVERSI KOORDINAT ===================== //
bool worldToGrid(float wx, float wz, int &gx, int &gz) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    gx = (int) ((wx - WORLD_MIN_X) / CELL_SIZE);
    gz = (int) ((wz - WORLD_MIN_Z) / CELL_SIZE);
    return gx >= 0 && gx < GRID_W && gz >= 0 && gz < GRID_H;
}

void gridToWorld(int gx, int gz, float &wx, float &wz) {
    wx = WORLD_MIN_X + gx * CELL_SIZE + CELL_SIZE * 0.5f;
    wz = WORLD_MIN_Z + gz * CELL_SIZE + CELL_SIZE * 0.5f;
}

bool isWalkable(int gx, int gz) {
    /* === KAMUS LOKAL === */
    
    /* === ALGORITMA === */
    if (gx < 0 || gx >= GRID_W || gz < 0 || gz >= GRID_H) {
        return false;
    }

    return pathGrid[gx][gz] == 0;
}

// ===================== MARK OBSTACLE ===================== //
static void markObstacle(float cx, float cz, float hw, float hd) {
    /* === KAMUS LOKAL === */
    int gx, gz;
    float pad, minX, minZ, maxX, maxZ;

    /* === ALGORITMA === */
    pad = 0.55f;
    minX = cx - hw - pad;
    maxX = cx + hw + pad;
    minZ = cz - hd - pad;
    maxZ = cz + hd + pad;

    worldToGrid(minX, minZ, _minGX, _minGZ);
    worldToGrid(maxX, maxZ, _maxGX, _maxGZ);

    if (_minGX > _maxGX) {
        std::swap(_minGX, _maxGX);
    }
    if (_minGZ > _maxGZ) {
        std::swap(_minGZ, _maxGZ);
    }
    if (_minGX < 0) {
        _minGX = 0;
    }
    if (_maxGX >= GRID_W) {
        _maxGX = GRID_W - 1;
    }
    if (_minGZ < 0) {
        _minGZ = 0;
    }
    if (_maxGZ >= GRID_H) {
        _maxGZ = GRID_H - 1;
    }
    for (gx = _minGX; gx <= _maxGX; gx++) {
        for (gz = _minGZ; gz <= _maxGZ; gz++) {
            pathGrid[gx][gz] = 1;
        }
    }
}

// ===================== BUILD GRID OBSTACLE ===================== //
void buildPathGrid() {
    /* === KAMUS LOKAL === */
    int t, p;

    /* === ALGORITMA === */
    memset(pathGrid, 0, sizeof(pathGrid));

    for (t = 0; t < numTables; t++) {
        markObstacle(tableX[t], tableZ[t], tableW * 0.5f, tableD * 0.5f);
    }

    for (p = 0; p < nbElmPohon; p++) {
        markObstacle(listPohon[p].posX, listPohon[p].posZ,
                     listPohon[p].sizeW * 0.5f, listPohon[p].sizeD * 0.5f);
    }
}

// ===================== HELPER A* ===================== //
static int heuristic(int x1, int z1, int x2, int z2) {
    /* === KAMUS LOKAL === */

    /* === ALGORITMA === */
    return abs(x1 - x2) + abs(z1 - z2);
}

static bool findNearestWalkable(int sx, int sz, int &outX, int &outZ) {
    /* === KAMUS LOKAL === */
    int r, x, z;

    /* === ALGORITMA === */
    if (isWalkable(sx, sz)) {
        outX = sx;
        outZ = sz;

        return true;
    }

    for (r = 1; r <= 10; r++) {
        for (x = sx - r; x <= sx + r; x++) {
            for (z = sz - r; z <= sz + r; z++) {
                if (isWalkable(x, z)) {
                    outX = x;
                    outZ = z;

                    return true;
                }
            }
        }
    }
    
    return false;
}


// ===================== A* ===================== //
static bool findPathAStar(int startX, int startZ, int goalX, int goalZ, std::vector<PathNode> &outPath) {
    /* === KAMUS LOKAL === */
    static int gCost[GRID_W][GRID_H];
    static bool closedSet[GRID_W][GRID_H];
    static PathNode parent[GRID_W][GRID_H];
    
    PathNode c, p;
    AStarItem si, ni, cur;
    std::vector<PathNode> rev;
    std::priority_queue<AStarItem> open;

    int dx4[4] = {1, -1, 0, 0};
    int dz4[4] = {0, 0, 1, -1};
    int x, z, dir, k, nx, ng, nz;
    
    /* === ALGORITMA === */
    outPath.clear();
    if (!findNearestWalkable(startX, startZ, startX, startZ) 
        ||
        !findNearestWalkable(goalX,  goalZ,  goalX,  goalZ)) {
            return false;
        }

    for (x = 0; x < GRID_W; x++) {
        for (z = 0; z < GRID_H; z++) {
            gCost[x][z] = 999999;
            closedSet[x][z] = false;
            parent[x][z].x = -1;
            parent[x][z].z = -1;
        }
    }

    gCost[startX][startZ] = 0;
    si.x = startX;
    si.z = startZ;
    si.f = heuristic(startX, startZ, goalX, goalZ);
    open.push(si);

    while (!open.empty()) {
        cur = open.top();
        open.pop();

        if (closedSet[cur.x][cur.z]) {
            continue;
        }
        
        closedSet[cur.x][cur.z] = true;

        if (cur.x == goalX && cur.z == goalZ) {
            c.x = goalX;
            c.z = goalZ;

            while (!(c.x == startX && c.z == startZ)) {
                rev.push_back(c);
                p = parent[c.x][c.z];
                if (p.x == -1 || p.z == -1) {
                    return false;
                }
                c = p;
            }

            for (k = (int) rev.size() - 1; k >= 0; k--) {
                outPath.push_back(rev[k]);
            }
            
            return true;
        }

        for (dir = 0; dir < 4; dir++) {
            nx = cur.x + dx4[dir];
            nz = cur.z + dz4[dir];

            if (!isWalkable(nx, nz) || closedSet[nx][nz]) {
                continue;
            }
            
            ng = gCost[cur.x][cur.z] + 1;

            if (ng < gCost[nx][nz]) {
                gCost[nx][nz] = ng;
                parent[nx][nz].x = cur.x;
                parent[nx][nz].z = cur.z;
                ni.x = nx;
                ni.z = nz;
                ni.f = ng + heuristic(nx, nz, goalX, goalZ);
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
    if (!worldToGrid(zomX[i], zomZ[i], sx, sz)
        ||
        !worldToGrid(posX, posZ, gx, gz)) {
            return;
        }
    findPathAStar(sx, sz, gx, gz, zombiePath[i]);
}

// ===================== COLLISION CHECK ===================== //
bool positionHitsObjects(float x, float z) {
    /* === KAMUS LOKAL === */
    AABB test;
    int t, p;

    /* === ALGORITMA === */
    test.minX = x - 0.25f; 
    test.maxX = x + 0.25f;
    test.minY = zomY;      
    test.maxY = zomY + 0.9f;
    test.minZ = z - 0.25f;
    test.maxZ = z + 0.25f;

    for (t = 0; t < numTables; t++) {
        if (checkAABB(test, getTableAABB(t))) {
            return true;
        }
    }

    for (p = 0; p < nbElmPohon; p++) {
        if (checkAABB(test, getPohonAABB(p))) {
            return true;
        }
    }

    return false;
}

// ===================== STEERING ===================== //
static bool tryMoveZombie(int i, float dirX, float dirZ, float speedMul) {
    /* === KAMUS LOKAL === */
    float len, nx, nz;

    /* === ALGORITMA === */
    len = sqrt(dirX * dirX + dirZ * dirZ);
    if (len <= 0.001f) {
        return false;
    }

    dirX /= len;
    dirZ /= len;
    nx = zomX[i] + dirX * zomSpeed * speedMul;
    nz = zomZ[i] + dirZ * zomSpeed * speedMul;

    if (!positionHitsObjects(nx, nz)) {
        zomX[i] = nx;
        zomZ[i] = nz;
        zomAngleY[i] = atan2(-dirX, -dirZ) * 180.0f / 3.14159265f;
        return true;
    }

    return false;
}

static void doSteeringAvoidance(int i, float desiredX, float desiredZ) {
    /* === KAMUS LOKAL === */
    float rX, rZ, lX, lZ, drX, drZ, dlX, dlZ, brX, brZ, blX, blZ;

    /* === ALGORITMA === */
    rX = desiredZ;
    rZ = -desiredX;
    lX = -desiredZ;
    lZ = desiredX;
    drX = desiredX * 0.55f + rX * 0.85f;
    drZ = desiredZ * 0.55f + rZ * 0.85f;
    dlX = desiredX * 0.55f + lX * 0.85f;
    dlZ = desiredZ * 0.55f + lZ * 0.85f;
    brX = -desiredX * 0.25f + rX;
    brZ = -desiredZ * 0.25f + rZ;
    blX = -desiredX * 0.25f + lX;
    blZ = -desiredZ * 0.25f + lZ;

    if (tryMoveZombie(i, drX, drZ, 0.8f)
        ||
        tryMoveZombie(i, dlX, dlZ, 0.8f)
        ||
        tryMoveZombie(i, rX, rZ, 0.65f)
        ||
        tryMoveZombie(i, lX, lZ, 0.65f)
        ||
        tryMoveZombie(i, brX, brZ, 0.45f)
        ||
        tryMoveZombie(i, blX, blZ, 0.45f)) {
            return;
        }
}

// ===================== UPDATE ZOMBIE ===================== //
void updateZombie(int i) {
    /* === KAMUS LOKAL === */
    float targetX, targetZ, dx, dz, dist, dpx, dpz, dX, dZ, step, nx, nz;
    PathNode next;

    /* === ALGORITMA === */
    if (isDead) {
        return;
    }

    if (!zombiePath[i].empty()) {
        next = zombiePath[i][0];
        gridToWorld(next.x, next.z, targetX, targetZ);
    } else {
        targetX = posX;
        targetZ = posZ;
    }

    dx = targetX - zomX[i];
    dz = targetZ - zomZ[i];
    dist = sqrt(dx * dx + dz * dz);

    if (dist < 0.12f && !zombiePath[i].empty()) {
        zombiePath[i].erase(zombiePath[i].begin());
        return;
    }

    dpx = posX - zomX[i];
    dpz = posZ - zomZ[i];
    if (sqrt(dpx * dpx + dpz * dpz) <= stopDistance) {
        return;
    }

    dx = targetX - zomX[i];
    dz = targetZ - zomZ[i];
    dist = sqrt(dx * dx + dz * dz);
    if (dist <= 0.001f) {
        return;
    }

    dX = dx / dist;
    dZ = dz / dist;

    if (zomSpeed < dist) {
        step = zomSpeed;
    } else {
        step = dist;
    }

    nx = zomX[i] + dX * step;
    nz = zomZ[i] + dZ * step;

    if (positionHitsObjects(nx, nz)) {
        doSteeringAvoidance(i, dX, dZ);
    } else {
        zomX[i] = nx;
        zomZ[i] = nz;
        zomAngleY[i] = atan2(-dX, -dZ) * 180.0f / 3.14159265f;
    }
}