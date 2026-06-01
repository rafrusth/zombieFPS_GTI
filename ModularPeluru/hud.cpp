#include "hud.h"
#include "render.h"
#include <stdio.h>
#include <cmath>

// ===================== HELPER WORLD -> MAP ===================== //
static void worldToMap(float wx, float wz, float &sx, float &sy) {
    sx = g_mapX + g_mapSize*0.5f + (wx-posX)*g_mapScale;
    sy = g_mapY + g_mapSize*0.5f - (wz-posZ)*g_mapScale;
}

// ===================== MINI-MAP ===================== //
void drawMiniMap() {
    float mapX    = MAP_X;
    float mapSize = MAP_SIZE;
    float mapY    = (float)winHeight - MAP_Y_FROM_TOP - mapSize;
    float mapScale= mapSize / MAP_WORLD_RANGE;
    float dotSize = 4.0f;
    int i;

    g_mapX=mapX; g_mapY=mapY; g_mapSize=mapSize; g_mapScale=mapScale;

    // Background
    glColor4f(0,0,0,0.55f);
    glBegin(GL_QUADS);
        glVertex2f(mapX,mapY); glVertex2f(mapX+mapSize,mapY);
        glVertex2f(mapX+mapSize,mapY+mapSize); glVertex2f(mapX,mapY+mapSize);
    glEnd();

    // Border
    glColor3f(0.8f,0.8f,0.8f); glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(mapX,mapY); glVertex2f(mapX+mapSize,mapY);
        glVertex2f(mapX+mapSize,mapY+mapSize); glVertex2f(mapX,mapY+mapSize);
    glEnd();

    glColor3f(1,1,1);
    renderText(mapX+2, mapY+mapSize+4, GLUT_BITMAP_8_BY_13, "MINI-MAP");

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)mapX,(int)mapY,(int)mapSize,(int)mapSize);

    // Meja (oranye)
    glColor3f(0.85f,0.50f,0.15f);
    for (i=0;i<numTables;i++){
        float sx,sy; worldToMap(tableX[i],tableZ[i],sx,sy);
        glBegin(GL_QUADS);
            glVertex2f(sx-dotSize*0.5f,sy-dotSize); glVertex2f(sx+dotSize*0.5f,sy-dotSize);
            glVertex2f(sx+dotSize*0.5f,sy+dotSize); glVertex2f(sx-dotSize*0.5f,sy+dotSize);
        glEnd();
    }

    // Pohon (hijau)
    glColor3f(0.05f,0.75f,0.10f);
    for (i=0;i<nbElmPohon;i++){
        float sx,sy; worldToMap(listPohon[i].posX,listPohon[i].posZ,sx,sy);
        glBegin(GL_QUADS);
            glVertex2f(sx-dotSize,sy-dotSize); glVertex2f(sx+dotSize,sy-dotSize);
            glVertex2f(sx+dotSize,sy+dotSize); glVertex2f(sx-dotSize,sy+dotSize);
        glEnd();
    }

    // Zombie (merah segitiga)
    for (i=0;i<numZombies;i++){
        float sx,sy; worldToMap(zomX[i],zomZ[i],sx,sy);
        float rad=(zomAngleY[i]+90.0f)*(float)M_PI/180.0f;
        float tipX=sx+cosf(rad)*(dotSize+2), tipY=sy+sinf(rad)*(dotSize+2);
        float b1=rad+(float)M_PI*0.75f, b2=rad-(float)M_PI*0.75f;
        glColor3f(1.0f,0.15f,0.15f);
        glBegin(GL_TRIANGLES);
            glVertex2f(tipX,tipY);
            glVertex2f(sx+cosf(b1)*dotSize,sy+sinf(b1)*dotSize);
            glVertex2f(sx+cosf(b2)*dotSize,sy+sinf(b2)*dotSize);
        glEnd();
    }

    // Player (cyan + putih)
    {
        float sx=mapX+mapSize*0.5f, sy=mapY+mapSize*0.5f;
        float rad=(-camAngleY+90.0f)*(float)M_PI/180.0f;
        float tipX=sx+cosf(rad)*(dotSize+3), tipY=sy+sinf(rad)*(dotSize+3);
        float b1=rad+(float)M_PI*0.75f, b2=rad-(float)M_PI*0.75f;

        glColor3f(1,1,1);
        glBegin(GL_QUADS);
            glVertex2f(sx-2,sy-2); glVertex2f(sx+2,sy-2);
            glVertex2f(sx+2,sy+2); glVertex2f(sx-2,sy+2);
        glEnd();
        glColor3f(0,1,1);
        glBegin(GL_TRIANGLES);
            glVertex2f(tipX,tipY);
            glVertex2f(sx+cosf(b1)*dotSize,sy+sinf(b1)*dotSize);
            glVertex2f(sx+cosf(b2)*dotSize,sy+sinf(b2)*dotSize);
        glEnd();
    }

    glDisable(GL_SCISSOR_TEST);

    // Garis silang tengah
    glColor4f(1,1,1,0.15f); glLineWidth(1.0f);
    float cx=mapX+mapSize*0.5f, cy=mapY+mapSize*0.5f;
    glBegin(GL_LINES);
        glVertex2f(cx,mapY); glVertex2f(cx,mapY+mapSize);
        glVertex2f(mapX,cy); glVertex2f(mapX+mapSize,cy);
    glEnd();
}

// ===================== HUD UTAMA ===================== //
void drawHUD() {
    // Label mode kamera
    glColor3f(1,1,1);
    switch(viewMode) {
        case 1: renderText(50,690,GLUT_BITMAP_9_BY_15,"[1] 1-Point Perspective (FPS)"); break;
        case 2: renderText(50,690,GLUT_BITMAP_9_BY_15,"[2] 2-Point Perspective (3rd Person)"); break;
        case 3: renderText(50,690,GLUT_BITMAP_9_BY_15,"[3] 3-Point Perspective (3rd Person Atas)"); break;
        case 4: renderText(50,690,GLUT_BITMAP_9_BY_15,"[4] Orthographic"); break;
    }

    // Panduan kontrol
    glColor3f(0.9f,0.9f,0.9f);
    renderText(1050,700,GLUT_BITMAP_8_BY_13,"W/A/S/D     : Gerak");
    renderText(1050,685,GLUT_BITMAP_8_BY_13,"Mouse       : Lihat");
    renderText(1050,670,GLUT_BITMAP_8_BY_13,"Left-Click  : Tembak");
    renderText(1050,655,GLUT_BITMAP_8_BY_13,"E           : Reload");
    renderText(1050,640,GLUT_BITMAP_8_BY_13,"1/2/3/4     : Proyeksi");
    renderText(1050,625,GLUT_BITMAP_8_BY_13,"=/-         : Zoom-In/Out");
    renderText(1050,610,GLUT_BITMAP_8_BY_13,"R           : Restart");
    renderText(1050,595,GLUT_BITMAP_8_BY_13,"ESC         : Keluar");

    // === HEALTH BAR ===
    float barW=300, barH=25, bx=50, by=650;
    float hpPerc = playerHealth/100.0f;
    if (hpPerc < 0) hpPerc = 0;

    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_QUADS);
        glVertex2f(bx,by); glVertex2f(bx+barW,by);
        glVertex2f(bx+barW,by+barH); glVertex2f(bx,by+barH);
    glEnd();

    if      (playerHealth > 70) glColor3f(0,1,0);
    else if (playerHealth > 30) glColor3f(1,1,0);
    else                        glColor3f(1,0,0);
    glBegin(GL_QUADS);
        glVertex2f(bx,by); glVertex2f(bx+barW*hpPerc,by);
        glVertex2f(bx+barW*hpPerc,by+barH); glVertex2f(bx,by+barH);
    glEnd();

    glColor3f(1,1,1); glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(bx,by); glVertex2f(bx+barW,by);
        glVertex2f(bx+barW,by+barH); glVertex2f(bx,by+barH);
    glEnd();

    char healthText[50];
    sprintf(healthText,"HP : %.0f%%", playerHealth);
    if      (playerHealth > 70) glColor3f(1,1,1);
    else if (playerHealth > 30) glColor3f(1,1,0);
    else                        glColor3f(1,0,0);
    renderText(bx, by-25, GLUT_BITMAP_9_BY_15, healthText);

    glColor3f(1,1,1);
    char zoomCam[50];
    sprintf(zoomCam,"Zoom: %.1f", camDist);
    renderText(bx, by-50, GLUT_BITMAP_9_BY_15, zoomCam);

	// === DISPLAY AMMO ===
	char ammoText[50];
	sprintf(ammoText, "Ammo : %d / %d", ammo, maxAmmo);

	glColor3f(1,1,1);
	renderText(bx, by-75, GLUT_BITMAP_9_BY_15, ammoText);
	
	// === CROSSHAIR / BIDIKAN ===
	float cx = winWidth / 2;
	float cy = winHeight / 2;

	glColor3f(1, 1, 1);
	glLineWidth(2);

	glBegin(GL_LINES);
    	glVertex2f(cx - 10, cy);
    	glVertex2f(cx + 10, cy);

    	glVertex2f(cx, cy - 10);
    	glVertex2f(cx, cy + 10);
	glEnd();
	
    // === GAME OVER OVERLAY ===================== //
    if (isDead) {
        // Panel gelap semi-transparan
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
        glBegin(GL_QUADS);
            glVertex2f(390,290); glVertex2f(890,290);
            glVertex2f(890,450); glVertex2f(390,450);
        glEnd();

        // Garis border merah
        glColor4f(0.85f,0.0f,0.0f,0.9f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(390,290); glVertex2f(890,290);
            glVertex2f(890,450); glVertex2f(390,450);
        glEnd();

        // Teks "GAME OVER"
        glColor3f(1.0f, 0.15f, 0.15f);
        renderText(536, 400, GLUT_BITMAP_TIMES_ROMAN_24, "GAME  OVER");

        // Instruksi
        glColor3f(1.0f,1.0f,1.0f);
        renderText(482, 355, GLUT_BITMAP_9_BY_15, "Tekan  [R]  untuk  Restart");
        renderText(470, 320, GLUT_BITMAP_9_BY_15, "Tekan  [ESC]  untuk  Keluar");
    }
    
	// === WIN OVERLAY ===================== //
	if (isWin) {
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    	glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    	glBegin(GL_QUADS);
        	glVertex2f(390,290); glVertex2f(890,290);
        	glVertex2f(890,450); glVertex2f(390,450);
    	glEnd();

    	glColor4f(0.0f,0.8f,0.2f,0.9f);
    	glLineWidth(2.0f);
    	glBegin(GL_LINE_LOOP);
        	glVertex2f(390,290); glVertex2f(890,290);
        	glVertex2f(890,450); glVertex2f(390,450);
    	glEnd();

    	glColor3f(0.2f, 1.0f, 0.2f);
    	renderText(555, 400, GLUT_BITMAP_TIMES_ROMAN_24, "YOU  WIN");

    	glColor3f(1.0f,1.0f,1.0f);
    	renderText(482, 355, GLUT_BITMAP_9_BY_15, "Tekan  [R]  untuk  Restart");
    	renderText(470, 320, GLUT_BITMAP_9_BY_15, "Tekan  [ESC]  untuk  Keluar");
	}
    drawMiniMap();
}

