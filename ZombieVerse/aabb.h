#ifndef AABB_H
#define AABB_H

#include "globals.h"

bool checkAABB(const AABB& a, const AABB& b);
AABB getPlayerAABB();
AABB getTableAABB(int i);
AABB getPohonAABB(int i);
AABB getZombieAABB(int i);

#endif