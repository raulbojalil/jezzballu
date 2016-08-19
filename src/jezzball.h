#ifndef _JEZZBALL_H_
#define _JEZZBALL_H_

#include <wut_types.h>
#include "rect.h"
#include "draw.h"
#include "bmp.h"
#include "highscores.h"
#include <string.h>
#include "sound.h"
#include "controls.h"

#define DRAWING_OFFSET_X 164
#define DRAWING_OFFSET_Y 24

#define MAP_WIDTH 30
#define MAP_HEIGHT 24

#define GAME_SPEED 512

#define MAX_BALLS 100
#define MAX_NAME_LENGTH 12

#define TILE_CLEARED 3
#define TILE_DEFAULT 2
#define TILE_BLUE 1
#define TILE_RED 0

typedef struct
{
	s16 x, y;
	s32 vx, vy;
	u8 animFrame;
	u8 animTimer;
	s8 xdirection, ydirection;
	u8 angle, stretched;
	
} sprite;

typedef struct
{
	s16 x1, y1, x2, y2;
	s32 vx, vy;
	s8 xdirection, ydirection;
} wall;

/////////////////
////// INIT /////
/////////////////

void jezzballInit();
void initBackground();
void initLevel(u8 level);
void initBalls();
void initCursor();

////////////////////////////
//////// UPDATE ////////////
////////////////////////////

void jezzballUpdate();
u16  checkCollision(s16 x, s16 y);
void moveBalls();
void moveCursor();  
void clearEmptyRooms();
void buildWall(wall * thiswall, wall * otherwall, s16 xstep, s16 ystep, u16 tilenum, Rect* head); 
void fillWall(wall * wall, u16 tilenum, Rect* head);
u8   getMapTile(u16 x, u16 y);
void setMapTile(u16 x, u16 y, u8 tilenum);
void refreshMapTile(u16 x, u16 y);
void refreshMapTiles(u16 x, u16 y);
void checkIfWin();
void gameOver();

///////////////
//// DRAW /////
///////////////

void jezzballDraw(); 
void drawBackgroundTile(u8 tile, u16 mapX, u16 mapY);
void drawBackground();
void drawBall(sprite* ball);
void drawBalls();
void drawWallHeads();
void drawCursor();
void drawScore();

///////////////
//// MISC /////
///////////////

u32 randMinMax(u32 min, u32 max);
s32 getSquaredDistanceBetweenTwoPoints(s16 x1, s16 y1, s16 x2, s16 y2);

#endif





