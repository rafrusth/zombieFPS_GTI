#include "aabb.h"

bool checkAABB(const AABB& a, const AABB& b) {
    return  (a.minX <= b.maxX && a.maxX >= b.minX) &&
            (a.minY <= b.maxY && a.maxY >= b.minY) &&
            (a.minZ <= b.maxZ && a.maxZ >= b.minZ);
}

AABB getPlayerAABB() {
    /* === KAMUS LOKAL === */
    AABB box;

    /* === ALGORITMA === */
    box.minX = posX - 0.25f;
    box.maxX = posX + 0.25f;
    box.minY = posY;
    box.maxY = posY + 0.9f;
    box.minZ = posZ - 0.25f;
    box.maxZ = posZ + 0.25f;
    
    return box;
}

AABB getTableAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;
    
    /* === ALGORITMA === */
    box.minX = tableX[i] - tableW * 0.45f;
    box.maxX = tableX[i] + tableW * 0.45f;
    box.minY = tableY;
    box.maxY = tableY + tableH;
    box.minZ = tableZ[i] - tableD * 0.45f;
    box.maxZ = tableZ[i] + tableD * 0.45f;
    
    return box;
}

AABB getPohonAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;
    Pohon p;
    
    /* === ALGORITMA === */
    p = listPohon[i];

    box.minX = p.posX - p.sizeW * 0.5f;
    box.maxX = p.posX + p.sizeW * 0.5f;
    box.minY = p.posY;                   
    box.maxY = p.posY + p.sizeH;
    box.minZ = p.posZ - p.sizeD * 0.5f;
    box.maxZ = p.posZ + p.sizeD * 0.5f;
    
    return box;
}

AABB getZombieAABB(int i) {
    /* === KAMUS LOKAL === */
    AABB box;
    
    /* === ALGORITMA === */
    box.minX = zomX[i] - 0.25f;
    box.maxX = zomX[i] + 0.25f;
    box.minY = zomY;             
    box.maxY = zomY + 0.9f;
    box.minZ = zomZ[i] - 0.25f;
    box.maxZ = zomZ[i] + 0.25f;
    
    return box;
}


