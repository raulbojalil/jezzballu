#ifndef DRAW_H
#define DRAW_H
#include <coreinit/screen.h>
#include <wut_types.h>
#include "rect.h"
#include "bmp.h"
#include <stdlib.h>

#define DRAWING_SCALE 1
#define TV_DRAWING_OFFSET_X 200
#define TV_DRAWING_OFFSET_Y 120
#define CHARACTER_PADDING 1

void *screenBuffer;

typedef enum {
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_CENTER = 1,
	TEXT_ALIGN_RIGHT = 2
} TextAlign;

typedef enum {
	FONTFAMILY_VERDANA14 = 0,
	FONTFAMILY_VERDANA14_BOLD = 1,
	FONTFAMILY_VERDANA11 = 2,
	FONTFAMILY_VERDANA10 = 3,
} FontFamily;


//Function declarations for my graphics library
void flipBuffers();
void fillScreen(char r, char g, char b, char a);

void drawInit();

void clearScreen();
void drawPixel(int x, int y, u8 r, u8 g, u8 b, u8 a);
void drawRect(Rect* rect, u8 R, u8 G, u8 B, u8 A);
void drawImage(u8* imageData, u32 imageWidth, u32 imageHeight, Rect* src, Rect* dest);
u32 getCharWidth(FontFamily fontFamily, char c);
void drawString(FontFamily fontFamily, u16 X, u16 Y, const char *str, TextAlign textAlign, u8 bgR, u8 bgG, u8 bgB);

void drawFinish();


#endif /* DRAW_H */
