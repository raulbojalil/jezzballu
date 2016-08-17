#include "draw.h"

#include <coreinit/cache.h>
#include <coreinit/screen.h>

#include "../textures/verdana14_000000.h"
#include "../textures/verdana14_000000_data.h"

#include "../textures/verdana14_ffffff_bold.h"
#include "../textures/verdana14_ffffff_bold_data.h"

#include "../textures/verdana11_000000.h"
#include "../textures/verdana11_000000_data.h"

#include "../textures/verdana10_282828.h"
#include "../textures/verdana10_282828_data.h"

void *screenBuffer;

void flipBuffers()
{
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	
	//Flush the cache
	DCFlushRange((void *)screenBuffer + buf0_size, buf1_size);
	DCFlushRange((void *)screenBuffer, buf0_size);
	
	//Flip the buffer
	OSScreenFlipBuffersEx(0);
	OSScreenFlipBuffersEx(1);
}

void fillScreen(char r,char g,char b,char a)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenClearBufferEx(0, num);
	OSScreenClearBufferEx(1, num);
}

void clearScreen()
{
	//fillScreen(0, 0, 0, 0);
	//flipBuffers();

	for (int i = 0; i < 1920; i++)
	{
		for (int j = 0; j < 1080; j++)
		{
			drawPixel(i, j, 0, 0, 0, 1);
		}
	}
}

void drawInit()
{
}

void drawPixel(int x, int y, u8 r, u8 g, u8 b, u8 a)
{

	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenPutPixelEx(0, x * 2 + TV_DRAWING_OFFSET_X, y * 2 + TV_DRAWING_OFFSET_Y, num);
	OSScreenPutPixelEx(1, x * 2, y * 2, num);
	OSScreenPutPixelEx(0, x * 2 + 1 + TV_DRAWING_OFFSET_X, y * 2 + TV_DRAWING_OFFSET_Y, num);
	OSScreenPutPixelEx(1, x * 2 + 1, y * 2, num);
	OSScreenPutPixelEx(0, x * 2 + TV_DRAWING_OFFSET_X, y * 2 + 1 + TV_DRAWING_OFFSET_Y, num);
	OSScreenPutPixelEx(1, x * 2, y * 2 + 1, num);
	OSScreenPutPixelEx(0, x * 2 + 1 + TV_DRAWING_OFFSET_X, y * 2 + 1 + TV_DRAWING_OFFSET_Y, num);
	OSScreenPutPixelEx(1, x * 2 + 1, y * 2 + 1, num);
	//Code to write to framebuffer directly. For some reason this is only writing to one of the framebuffers when calling flipBuffers. Should provide speedup but needs investigation.
	/*
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int height = 1024;
	int width = 1280;
	char *screen = (void *)screenBuffer;
	uint32_t v=(x + y*width)*4;
	screen[v]=r;
	screen[v+1]=g;
	screen[v+2]=b;
	screen[v+3]=a;

	height = 480;
	width = 896;
	char *screen2 = (void *)screenBuffer + buf0_size;
	v=(x + y*width)*4;
	screen2[v]=r;
	screen2[v+1]=g;
	screen2[v+2]=b;
	screen2[v+3]=a;
	*/

}

void drawRect(Rect* rect, u8 R, u8 G, u8 B, u8 A)
{
	u32 bufWidth = 1920;
	u32 bufHeight = 1080;

	u32 x, y;
	for (x = rect->x*DRAWING_SCALE; x < (rect->x*DRAWING_SCALE) + (rect->w*DRAWING_SCALE); x++)
	{
		for (y = rect->y*DRAWING_SCALE; y < (rect->y*DRAWING_SCALE) + (rect->h*DRAWING_SCALE); y++)
		{
			if (x < 0 || y < 0 || x >= bufWidth || y >= bufHeight) continue;

			drawPixel(x, y, R, G, B, A);
		}
	}
}

void drawImageN(u8* imageData, u32 imageWidth, u32 imageHeight, Rect* src, Rect* dest, u8 bgR, u8 bgG, u8 bgB)
{
	u32 bufWidth = 1920;
	u32 bufHeight = 1080;

	if (imageData == NULL)
		return;

	u32 x, y;

	for (x = 0; x < src->w; x++)
	{
		for (y = 0; y < src->h; y++)
		{
			if ((src->x) + x < 0 || (src->y) + y < 0 || (src->x) + x >= imageWidth || (src->y) + y >= imageHeight) continue;
			if ((dest->x) + x < 0 || (dest->y) + y < 0 || (dest->x) + x >= bufWidth || (dest->y) + y >= bufHeight) continue;

			u32 src_pix = ((src->x) + x) * 4 + ((src->y) + y) * 4 * imageWidth;

			if (imageData[src_pix + 3] > 0)// alpha > 0
			{
				if (imageData[src_pix + 3] < 0xFF) //alpha blend
				{
					drawPixel((dest->x) + x, (dest->y) + y,
						(((imageData[src_pix + 3] + 1) * imageData[src_pix] + (256 - imageData[src_pix + 3]) * bgR) >> 8),
						(((imageData[src_pix + 3] + 1) * imageData[src_pix + 1] + (256 - imageData[src_pix + 3]) * bgG) >> 8),
						(((imageData[src_pix + 3] + 1) * imageData[src_pix + 2] + (256 - imageData[src_pix + 3]) * bgB) >> 8), 255);
				}
				else
					drawPixel((dest->x) + x, (dest->y) + y, imageData[src_pix], imageData[src_pix + 1], imageData[src_pix + 2], 255);
			}

		}
	}
}

void drawImage(u8* imageData, u32 imageWidth, u32 imageHeight, Rect* src, Rect* dest)
{
	u32 bufWidth = 1920;
	u32 bufHeight = 1080;

	if (imageData == NULL)
		return;

	u32 x, y;

	for (x = 0; x < src->w*DRAWING_SCALE; x++)
	{
		for (y = 0; y < src->h*DRAWING_SCALE; y++)
		{
			if ((src->x*DRAWING_SCALE) + x < 0 || (src->y*DRAWING_SCALE) + y < 0 || (src->x*DRAWING_SCALE) + x >= imageWidth || (src->y*DRAWING_SCALE) + y >= imageHeight) continue;
			if ((dest->x*DRAWING_SCALE) + x < 0 || (dest->y*DRAWING_SCALE) + y < 0 || (dest->x*DRAWING_SCALE) + x >= bufWidth || (dest->y*DRAWING_SCALE) + y >= bufHeight) continue;

			u32 src_pix = ((src->x*DRAWING_SCALE) + x) * 4 + ((src->y*DRAWING_SCALE) + y) * 4 * imageWidth;

			if (imageData[src_pix + 3] > 0)// alpha > 0
			{
				if (imageData[src_pix + 3] < 0xFF) //alpha blend
				{
					drawPixel((dest->x*DRAWING_SCALE) + x, (dest->y*DRAWING_SCALE) + y,
						(((imageData[src_pix + 3] + 1) * imageData[src_pix] + (256 - imageData[src_pix + 3]) * 255) >> 8),
						(((imageData[src_pix + 3] + 1) * imageData[src_pix + 1] + (256 - imageData[src_pix + 3]) * 255) >> 8),
						(((imageData[src_pix + 3] + 1) * imageData[src_pix + 2] + (256 - imageData[src_pix + 3]) * 255) >> 8), 255);
				}
				else
					drawPixel((dest->x*DRAWING_SCALE) + x, (dest->y*DRAWING_SCALE) + y, imageData[src_pix], imageData[src_pix + 1], imageData[src_pix + 2], 255);
			}

		}
	}
}

u32 getCharWidth(FontFamily fontFamily, char c)
{
	Rect* fontInfo;

	switch (fontFamily)
	{
	case FONTFAMILY_VERDANA14:
		fontInfo = verdana14_000000;
		break;
	case FONTFAMILY_VERDANA14_BOLD:
		fontInfo = verdana14_ffffff_bold;
		break;
	case FONTFAMILY_VERDANA11:
		fontInfo = verdana11_000000;
		break;
	case FONTFAMILY_VERDANA10:
		fontInfo = verdana10_282828;
		break;
	}

	return fontInfo[c].w + CHARACTER_PADDING;
}



void drawString(FontFamily fontFamily, u16 X, u16 Y, const char *str, TextAlign textAlign, u8 bgR, u8 bgG, u8 bgB)
{

	u8 offset = 0;
	Rect* fontInfo;
	u8* fontImage;
	u16 fontWidth;
	u16 fontHeight;

	switch (fontFamily)
	{
	case FONTFAMILY_VERDANA14:
		fontInfo = verdana14_000000;
		fontImage = verdana14_000000_data.pixel_data;
		fontWidth = verdana14_000000_width;
		fontHeight = verdana14_000000_height;
		break;
	case FONTFAMILY_VERDANA14_BOLD:
		fontInfo = verdana14_ffffff_bold;
		fontImage = verdana14_ffffff_bold_data.pixel_data;
		fontWidth = verdana14_ffffff_bold_width;
		fontHeight = verdana14_ffffff_bold_height;
		break;
	case FONTFAMILY_VERDANA11:
		fontInfo = verdana11_000000;
		fontImage = verdana11_000000_data.pixel_data;
		fontWidth = verdana11_000000_width;
		fontHeight = verdana11_000000_height;
		//fontImage = verdana11Image;
		break;
	case FONTFAMILY_VERDANA10:
		fontInfo = verdana10_282828;
		fontImage = verdana10_282828_data.pixel_data;
		fontWidth = verdana10_282828_width;
		fontHeight = verdana10_282828_height;
		break;

	}


	Rect dest = { X,Y,0,0 };

	if (textAlign == TEXT_ALIGN_RIGHT || textAlign == TEXT_ALIGN_CENTER)
	{
		u32 strWidth = 0;
		const char *g = str;
		while (*g) { if (*g != '\n' && *g != '\t') strWidth += fontInfo[*g].w + CHARACTER_PADDING; ++g; }

		if (textAlign == TEXT_ALIGN_RIGHT)
			dest.x -= strWidth;
		else
			dest.x -= strWidth / 2;
	}

	const char *s = str;
	while (*s) {

		if (*s == '\n') {
		}
		else if (*s == '\t') {
		}
		else {

			drawImageN(fontImage, fontWidth, fontHeight, &fontInfo[*s], &dest, bgR, bgG, bgB);
			dest.x += fontInfo[*s].w + CHARACTER_PADDING;
		}
		++s;
	}
}


void drawFinish()
{
}