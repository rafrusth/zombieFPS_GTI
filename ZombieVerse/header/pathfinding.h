#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "globals.h"
#include "aabb.h"
#include <vector>

struct AStarItem {
    int x, z, f;

    bool operator<(const AStarItem &o) const {
        return f > o.f;
    }
};

bool worldToGrid(float wx, float wz, int &gx, int &gz);
void gridToWorld(int gx, int gz, float &wx, float &wz);
bool isWalkable(int gx, int gz);

static int _minGX, _maxGX, _minGZ, _maxGZ;
static void markObstacle(float cx, float cz, float hw, float hd);
static int heuristic(int x1, int z1, int x2, int z2);
static bool findNearestWalkable(int sx, int sz, int &outX, int &outZ);
static bool findPathAStar(int startX, int startZ, int goalX, int goalZ, std::vector<PathNode> &outPath);
static void doSteeringAvoidance(int i, float desiredX, float desiredZ);
static bool tryMoveZombie(int i, float dirX, float dirZ, float speedMul);

void buildPathGrid();
bool positionHitsObjects(float x, float z);
void calculateZombiePath(int i);
void updateZombie(int i);

#endif