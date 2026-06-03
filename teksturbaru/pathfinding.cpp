#include "pathfinding.h"
#include <queue>
#include <cstring>
#include <algorithm>
#include <cmath>

// ===================== KONVERSI KOORDINAT ===================== //
bool worldToGrid(float wx, float wz, int &gx, int &gz) {
    gx = (int)((wx - WORLD_MIN_X) / CELL_SIZE);
    gz = (int)((wz - WORLD_MIN_Z) / CELL_SIZE);
    return gx >= 0 && gx < GRID_W && gz >= 0 && gz < GRID_H;
}

void gridToWorld(int gx, int gz, float &wx, float &wz) {
    wx = WORLD_MIN_X + gx * CELL_SIZE + CELL_SIZE * 0.5f;
    wz = WORLD_MIN_Z + gz * CELL_SIZE + CELL_SIZE * 0.5f;
}

bool isWalkable(int gx, int gz) {
    if (gx < 0 || gx >= GRID_W || gz < 0 || gz >= GRID_H) return false;
    return pathGrid[gx][gz] == 0;
}

// ===================== MARK OBSTACLE (ganti lambda) ===================== //
static int   _minGX, _maxGX, _minGZ, _maxGZ;

static void markObstacle(float cx, float cz, float hw, float hd) {
    int gx, gz;
    float pad  = 0.55f;
    float minX = cx - hw - pad, maxX = cx + hw + pad;
    float minZ = cz - hd - pad, maxZ = cz + hd + pad;
    worldToGrid(minX, minZ, _minGX, _minGZ);
    worldToGrid(maxX, maxZ, _maxGX, _maxGZ);
    if (_minGX > _maxGX) std::swap(_minGX, _maxGX);
    if (_minGZ > _maxGZ) std::swap(_minGZ, _maxGZ);
    if (_minGX < 0)        _minGX = 0;
    if (_maxGX >= GRID_W)  _maxGX = GRID_W - 1;
    if (_minGZ < 0)        _minGZ = 0;
    if (_maxGZ >= GRID_H)  _maxGZ = GRID_H - 1;
    for (gx = _minGX; gx <= _maxGX; gx++)
        for (gz = _minGZ; gz <= _maxGZ; gz++)
            pathGrid[gx][gz] = 1;
}

// ===================== BUILD GRID OBSTACLE ===================== //
void buildPathGrid() {
    int t, p;
    memset(pathGrid, 0, sizeof(pathGrid));

    for (t = 0; t < numTables; t++)
        markObstacle(tableX[t], tableZ[t], tableW * 0.5f, tableD * 0.5f);

    for (p = 0; p < nbElmPohon; p++)
        markObstacle(listPohon[p].posX, listPohon[p].posZ,
                     listPohon[p].sizeW * 0.5f, listPohon[p].sizeD * 0.5f);
}

// ===================== HELPER A* ===================== //
static int heuristic(int x1, int z1, int x2, int z2) {
    return abs(x1 - x2) + abs(z1 - z2);
}

static bool findNearestWalkable(int sx, int sz, int &outX, int &outZ) {
    int r, x, z;
    if (isWalkable(sx, sz)) { outX = sx; outZ = sz; return true; }
    for (r = 1; r <= 10; r++)
        for (x = sx - r; x <= sx + r; x++)
            for (z = sz - r; z <= sz + r; z++)
                if (isWalkable(x, z)) { outX = x; outZ = z; return true; }
    return false;
}

struct AStarItem {
    int x, z, f;
    bool operator<(const AStarItem &o) const { return f > o.f; }
};

// ===================== A* ===================== //
static bool findPathAStar(int startX, int startZ, int goalX, int goalZ,
                          std::vector<PathNode> &outPath) {
    static int      gCost[GRID_W][GRID_H];
    static bool     closedSet[GRID_W][GRID_H];
    static PathNode parent[GRID_W][GRID_H];

    int dx4[4] = {1, -1, 0, 0};
    int dz4[4] = {0,  0, 1, -1};
    int x, z, dir, k;

    outPath.clear();
    if (!findNearestWalkable(startX, startZ, startX, startZ)) return false;
    if (!findNearestWalkable(goalX,  goalZ,  goalX,  goalZ))  return false;

    for (x = 0; x < GRID_W; x++)
        for (z = 0; z < GRID_H; z++) {
            gCost[x][z]     = 999999;
            closedSet[x][z] = false;
            parent[x][z].x  = -1;
            parent[x][z].z  = -1;
        }

    gCost[startX][startZ] = 0;
    std::priority_queue<AStarItem> open;
    AStarItem si;
    si.x = startX; si.z = startZ;
    si.f = heuristic(startX, startZ, goalX, goalZ);
    open.push(si);

    while (!open.empty()) {
        AStarItem cur = open.top(); open.pop();
        if (closedSet[cur.x][cur.z]) continue;
        closedSet[cur.x][cur.z] = true;

        if (cur.x == goalX && cur.z == goalZ) {
            PathNode c;
            c.x = goalX; c.z = goalZ;
            std::vector<PathNode> rev;
            while (!(c.x == startX && c.z == startZ)) {
                rev.push_back(c);
                PathNode p = parent[c.x][c.z];
                if (p.x == -1 || p.z == -1) return false;
                c = p;
            }
            for (k = (int)rev.size() - 1; k >= 0; k--)
                outPath.push_back(rev[k]);
            return true;
        }

        for (dir = 0; dir < 4; dir++) {
            int nx = cur.x + dx4[dir];
            int nz = cur.z + dz4[dir];
            if (!isWalkable(nx, nz) || closedSet[nx][nz]) continue;
            int ng = gCost[cur.x][cur.z] + 1;
            if (ng < gCost[nx][nz]) {
                gCost[nx][nz]     = ng;
                parent[nx][nz].x  = cur.x;
                parent[nx][nz].z  = cur.z;
                AStarItem ni;
                ni.x = nx; ni.z = nz;
                ni.f = ng + heuristic(nx, nz, goalX, goalZ);
                open.push(ni);
            }
        }
    }
    return false;
}

void calculateZombiePath(int i) {
    int sx, sz, gx, gz;
    zombiePath[i].clear();
    if (!worldToGrid(zomX[i], zomZ[i], sx, sz)) return;
    if (!worldToGrid(posX,    posZ,    gx, gz))  return;
    findPathAStar(sx, sz, gx, gz, zombiePath[i]);
}

// ===================== COLLISION CHECK ===================== //
bool positionHitsTable(float x, float z) {
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

bool willHitTable(float x, float z) { return positionHitsTable(x, z); }

// ===================== STEERING ===================== //
static bool tryMoveZombie(int i, float dirX, float dirZ, float speedMul) {
    float len = sqrt(dirX * dirX + dirZ * dirZ);
    if (len <= 0.001f) return false;
    dirX /= len; dirZ /= len;
    float nx = zomX[i] + dirX * zomSpeed * speedMul;
    float nz = zomZ[i] + dirZ * zomSpeed * speedMul;
    if (!willHitTable(nx, nz)) {
        zomX[i]      = nx; zomZ[i] = nz;
        zomAngleY[i] = atan2(-dirX, -dirZ) * 180.0f / 3.14159265f;
        return true;
    }
    return false;
}

static void doSteeringAvoidance(int i, float desiredX, float desiredZ) {
    float rX =  desiredZ, rZ = -desiredX;
    float lX = -desiredZ, lZ =  desiredX;
    float drX = desiredX * 0.55f + rX * 0.85f, drZ = desiredZ * 0.55f + rZ * 0.85f;
    float dlX = desiredX * 0.55f + lX * 0.85f, dlZ = desiredZ * 0.55f + lZ * 0.85f;
    float brX = -desiredX * 0.25f + rX,         brZ = -desiredZ * 0.25f + rZ;
    float blX = -desiredX * 0.25f + lX,         blZ = -desiredZ * 0.25f + lZ;
    if (tryMoveZombie(i, drX, drZ, 0.8f))  return;
    if (tryMoveZombie(i, dlX, dlZ, 0.8f))  return;
    if (tryMoveZombie(i,  rX,  rZ, 0.65f)) return;
    if (tryMoveZombie(i,  lX,  lZ, 0.65f)) return;
    if (tryMoveZombie(i, brX, brZ, 0.45f)) return;
    if (tryMoveZombie(i, blX, blZ, 0.45f)) return;
}

// ===================== UPDATE ZOMBIE ===================== //
void updateZombie(int i) {
    float targetX, targetZ, dx, dz, dist, dpx, dpz, dX, dZ, step, nx, nz;
    if (isDead) return;

    if (!zombiePath[i].empty()) {
        PathNode next = zombiePath[i][0];
        gridToWorld(next.x, next.z, targetX, targetZ);
    } else {
        targetX = posX; targetZ = posZ;
    }

    dx   = targetX - zomX[i];
    dz   = targetZ - zomZ[i];
    dist = sqrt(dx * dx + dz * dz);

    if (dist < 0.12f && !zombiePath[i].empty()) {
        zombiePath[i].erase(zombiePath[i].begin());
        return;
    }

    dpx = posX - zomX[i];
    dpz = posZ - zomZ[i];
    if (sqrt(dpx * dpx + dpz * dpz) <= stopDistance) return;

    dx   = targetX - zomX[i];
    dz   = targetZ - zomZ[i];
    dist = sqrt(dx * dx + dz * dz);
    if (dist <= 0.001f) return;

    dX   = dx / dist;
    dZ   = dz / dist;
    step = (zomSpeed < dist) ? zomSpeed : dist;
    nx   = zomX[i] + dX * step;
    nz   = zomZ[i] + dZ * step;

    if (willHitTable(nx, nz))
        doSteeringAvoidance(i, dX, dZ);
    else {
        zomX[i]      = nx; zomZ[i] = nz;
        zomAngleY[i] = atan2(-dX, -dZ) * 180.0f / 3.14159265f;
    }
}


