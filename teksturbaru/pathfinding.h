#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "globals.h"
#include "aabb.h"
#include <vector>

bool worldToGrid(float wx, float wz, int &gx, int &gz);
void gridToWorld(int gx, int gz, float &wx, float &wz);
bool isWalkable(int gx, int gz);
void buildPathGrid();
bool positionHitsObjects(float x, float z);
void calculateZombiePath(int i);
void updateZombie(int i);

#endif
