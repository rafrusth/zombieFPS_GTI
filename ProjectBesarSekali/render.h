#ifndef RENDER_H
#define RENDER_H

#include "globals.h"

void setMaterial(float r, float g, float b);
void drawCuboid(float w, float h, float d,
                float fR, float fG, float fB,
                float sR, float sG, float sB,
                float tR, float tG, float tB,
                float bR, float bG, float bB);
void drawCuboid(float w, float h, float d);
void drawFrontPatch(float px, float py, float pw, float ph,
                    float z, float r, float g, float b);
void drawPistol();
void drawHandHUD(float xPos, float rotation, float currentRecoil);
void drawZombie();
void drawPlayer(float size);
void drawTable();
void drawPohon(Pohon p);
void applyProjection();
GLuint loadBMPTexture(const char* filename);
void renderText(float x, float y, void* font, const char* string);

#endif
