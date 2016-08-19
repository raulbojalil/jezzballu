#include "jezzball.h"
//#include "sprites_bmp.h"
//#include "scoreboard_bmp.h"
#include "../textures/scoreboard_data.h"
#include "../textures/sprites_data.h"

u8 map[MAP_WIDTH * MAP_HEIGHT];
u8 mapDraw[MAP_WIDTH * MAP_HEIGHT];
u16 collision[MAP_WIDTH * MAP_HEIGHT];

u8 columns[MAP_WIDTH];
u8 rows[MAP_HEIGHT];

sprite balls[MAX_BALLS];

wall blueWall, redWall;
sprite cursor;

u8 level = 1;
u8 lives = 0;
u8 ballCount = 0;

s32 levelScore = 0;
s32 totalScore = 0;
u32 bonus = 0;

u16 timeForLevel;
s16 timeLeft;
u32 counter = 0;

u8 complete = 0;
u8 gameover = 0;
u8 levelComplete = 0;

u16 tilesToClear;

//BmpFile* spritesImage = (BmpFile*)sprites_bmp;
//BmpFile* scoreboardImage = (BmpFile*)scoreboard_bmp;


u32 lfsr = 0;
u32 bit;

Rect ballPos = {0,0,8,8};
Rect ballSrc = {0,6*8,8,8};
Rect ballAnimFrames[10] = {{0,6*8,8,8}, 
						   {0,7*8,8,8},
						   {0,8*8,8,8},
						   {0,9*8,8,8},
						   {0,10*8,8,8},
						   {0,11*8,8,8},
						   {0,12*8,8,8},
						   {0,13*8,8,8},
						   {0,14*8,8,8},
						   {0,15*8,8,8},
					      };

Rect redWallHead = {0,0,8,8};
Rect blueWallHead = {0,0,8,8};

u16 onTouchScreenPressedX = 0;
u16 onTouchScreenPressedY = 0;

u8 cls = 0;
u8 taps = 0;
u32 touchTime = 0;
u32 testTouch = 0;

char playerName[MAX_NAME_LENGTH+1];
s16 inputHighscore = -1;

u8 highscoreCount = 0;
Highscore highscores[50];


void numToString(char str[], int num)
{
	if (num == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		return;
	}
	int i, rem, len = 0, n;

	n = num;
	while (n != 0)
	{
		len++;
		n /= 10;
	}
	for (i = 0; i < len; i++)
	{
		rem = num % 10;
		num = num / 10;
		str[len - (i + 1)] = rem + '0';
	}
	str[len] = '\0';
}

void jezzballInit(void)
{    
	//keyboardInit();
	lfsr = 123;
	level = 1;
	initLevel(level);
}
void initBackground(){

	u32 i=0;

	for(i = 0; i < (MAP_WIDTH * MAP_HEIGHT); i++)
	{
		map[i] = TILE_DEFAULT;
		mapDraw[i] = map[i];
	}

}
void initLevel(u8 level)
{
	
	u8 i, j;

    ballCount = level + 1;
    lives = ballCount;
    complete = 0;
    timeForLevel = 1300;

    for (i=0; i<level; i++)
	{
      timeForLevel += (i&1) * 100 + 200;
	}

    timeLeft = timeForLevel;
    levelScore = 0;
    levelComplete = 0;
    bonus = 0;
    counter = 0;

	initBackground();
    
	tilesToClear = 0;

    for (i=0; i<MAP_WIDTH; i++)
    {
		for (j=0; j<MAP_HEIGHT; j++)
        {
			if (map[j*MAP_WIDTH+i] == TILE_CLEARED)
            {
				collision[j*MAP_WIDTH+i] = TILE_CLEARED;
            }
			else
            {
				collision[j*MAP_WIDTH+i] = TILE_DEFAULT;
				tilesToClear++;
            }
        }
    }

	initBalls();
	initCursor();

	/*Rect clear = { 0, 0, SCREEN_BOTTOM_WIDTH, SCREEN_HEIGHT };
	clearRect(SCREEN_BOTTOM, &clear, 0,0,0,0);

	drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 17, i18nLevel(), TEXT_ALIGN_LEFT);  
	drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 63, i18nCompleted(), TEXT_ALIGN_LEFT); 
	drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 109, i18nLives(), TEXT_ALIGN_LEFT); 
	drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 155, i18nScore(), TEXT_ALIGN_LEFT); 
	drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 201, i18nTimeLeft(), TEXT_ALIGN_LEFT); */
	
}

void initBalls(void)
{
	u8 i;

	for (i=0; i<ballCount; i++)
	{
		u8 x, y;
		
		balls[i].xdirection = randMinMax(1,100) > 50 ? -1 : 1;
		balls[i].ydirection = randMinMax(1,100) > 50 ? -1 : 1;
		
		x = randMinMax(1, MAP_WIDTH-2);
	    y = randMinMax(1, MAP_HEIGHT-2);

		while (collision[y*MAP_WIDTH+x] == TILE_CLEARED)
		{
			x = randMinMax(1, MAP_WIDTH-2);
			y = randMinMax(1, MAP_HEIGHT-2);
		}

		balls[i].x = x * 8;
		balls[i].y = y * 8;
		balls[i].vx = balls[i].x << 8;
		balls[i].vy = balls[i].y << 8;
	}
}

void initCursor(void)
{
	cursor.x = (MAP_WIDTH*8)/2;
	cursor.y = (MAP_HEIGHT*8)/2;;
	cursor.angle = 0;

	fillWall(&blueWall, TILE_DEFAULT, &blueWallHead);
	fillWall(&redWall, TILE_DEFAULT, &redWallHead);
}

void jezzballUpdate(void)
{
	counter+=2;

	if (!gameover && !levelComplete && (inputHighscore == -1))
    {
		moveBalls();
		moveCursor();

		buildWall(&blueWall, &redWall, cursor.angle%2 ? 0 : -1,
			cursor.angle%2 ? 1 : 0, TILE_BLUE, &blueWallHead);
		
		buildWall(&redWall, &blueWall, cursor.angle%2 ? 0 : 1,
			cursor.angle%2 ? -1 : 0, TILE_RED, &redWallHead);
			
		

		timeLeft = timeForLevel - (u32) (((double) ((counter * GAME_SPEED)>>8)) / 6.);
		if (timeLeft <= 0)
		{
			gameOver();
			timeLeft = 0;
		}
	}

	else if (!gameover && inputHighscore != -1)
    {
		/*u8 ok = keyboardUpdate();
		if(ok)
		{
			getKeyboardInputText(playerName);
			Highscore newHighscore;
			newHighscore.score = totalScore;
			newHighscore.level = level;
			strcpy(newHighscore.player, playerName);
			saveHighscore(newHighscore);
			gameover = 1;
			loadHighscores(highscores, &highscoreCount);

			Rect clear = { 0, 0, SCREEN_BOTTOM_WIDTH, SCREEN_HEIGHT };
			clearRect(SCREEN_BOTTOM, &clear, 0,0,0,0);

			drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 17, i18nLevel(), TEXT_ALIGN_LEFT);  
			drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 63, i18nCompleted(), TEXT_ALIGN_LEFT); 
			drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 109, i18nLives(), TEXT_ALIGN_LEFT); 
			drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 155, i18nScore(), TEXT_ALIGN_LEFT); 
			drawString(SCREEN_BOTTOM, FONTFAMILY_VERDANA14_BOLD, 50, 201, i18nTimeLeft(), TEXT_ALIGN_LEFT); 
		}*/
    }
	else if(gameover && (isKeyPressed(CONTROLS_CONTINUE)))
	{
		
	    gameover = 0;
		inputHighscore = -1;
		totalScore = 0;
		level = 1;
		cls = 2;
		initLevel(level);
	}
    else if (levelComplete && (isKeyPressed(CONTROLS_CONTINUE)))
	{
		levelComplete = 0;
		totalScore += levelScore;
		cls = 2;
		initLevel(++level);
    }
}

u16 checkCollision(s16 x, s16 y)
{
	s8 x2, y2;

	x2 = x>>3;
	y2 = y>>3;

	if (x2 <= -1 || y2 <= -1 || x2 >= MAP_WIDTH || y2 >= MAP_HEIGHT)
	  return TILE_CLEARED;
	else
	  return collision[y2*MAP_WIDTH+x2];
}

void moveBalls(void)
{
	s8 i;
	u8 bounceSoundPlayed = 0, deadSoundPlayed = 0;

	for (i=0; i<ballCount; i++)
    {
		u16 left = TILE_DEFAULT, top = TILE_DEFAULT, right = TILE_DEFAULT, bottom = TILE_DEFAULT, check;

		// left
		check = checkCollision((balls[i].x - 1), (balls[i].y + 4));
		if (check != TILE_DEFAULT) left = check;

		// top left
		check = checkCollision((balls[i].x + 1), (balls[i].y + 1));
		if (check != TILE_DEFAULT) top = left = check;

		// top
		check = checkCollision((balls[i].x + 4), (balls[i].y - 1));
		if (check != TILE_DEFAULT) top = check;

		// right top
		check = checkCollision((balls[i].x + 7), (balls[i].y + 1));
		if (check != TILE_DEFAULT) right = top = check;

		// right
		check = checkCollision((balls[i].x + 8), (balls[i].y + 4));
		if (check != TILE_DEFAULT) right = check;

		// bottom right
		check = checkCollision((balls[i].x + 7), (balls[i].y + 7));
		if (check != TILE_DEFAULT) bottom = right = check;

		// bottom
		check = checkCollision((balls[i].x + 4), (balls[i].y + 8));
		if (check != TILE_DEFAULT) bottom = check;

		// left bottom
		check = checkCollision((balls[i].x + 1), (balls[i].y + 7));
		if (check != TILE_DEFAULT) left = bottom = check;

		if (top == TILE_CLEARED || bottom == TILE_CLEARED || right == TILE_CLEARED || left == TILE_CLEARED)
		{
			if (!bounceSoundPlayed)
			{
				//playSound(bounceSound);
				bounceSoundPlayed = 1;
			}
		}
        
		if (top == TILE_BLUE || bottom == TILE_BLUE || right == TILE_BLUE || left == TILE_BLUE)
		{
			lives --;
			if (lives == 0)
			{
				gameOver();
			}
			fillWall(&blueWall, TILE_DEFAULT, &blueWallHead);
            
			if (!deadSoundPlayed)
			{
				//playSound(deadSound);
				deadSoundPlayed = 1;
			}
		}
		if (top == TILE_RED || bottom == TILE_RED || right == TILE_RED || left == TILE_RED)
		{
			lives --;
			if (lives == 0)
			{
				gameOver();
			}
			fillWall(&redWall, TILE_DEFAULT, &redWallHead);
            
			if (!deadSoundPlayed)
			{
				//playSound(deadSound);
				deadSoundPlayed = 1;
			}
		}
        
		if (!(top == TILE_CLEARED && bottom == TILE_CLEARED) && !(right == TILE_CLEARED && left == TILE_CLEARED))
		{
			if (right == TILE_CLEARED)
				balls[i].xdirection = -1;
			else if (left == TILE_CLEARED)
				balls[i].xdirection = 1;

			if (bottom == TILE_CLEARED)
				balls[i].ydirection = -1;
			else if (top == TILE_CLEARED)
				balls[i].ydirection = 1;

			balls[i].vx += balls[i].xdirection * GAME_SPEED;
			balls[i].vy += balls[i].ydirection * GAME_SPEED;
			balls[i].x = balls[i].vx >> 8;
			balls[i].y = balls[i].vy >> 8;
		}
		else
		{
			// stop the ball
			u8 modulo;
			balls[i].xdirection = balls[i].ydirection = 0;

			modulo = balls[i].x & 7;
			if (modulo <= 4)
			{
				balls[i].x -= modulo;
				balls[i].vx = balls[i].x << 8;
			}
			else
			{
				balls[i].x = balls[i].x - modulo + 8;
				balls[i].vx = balls[i].x << 8;
			}

			modulo = balls[i].y & 7;
			if (modulo <= 4)
			{
				balls[i].y -= modulo;
				balls[i].vy = balls[i].y << 8;
			}
			else
			{
				balls[i].y = balls[i].y - modulo + 8;
				balls[i].vy = balls[i].y << 8;
			}
		}

		// balls collision
		u8 j;
		for (j=0; j<i; j++)
		{ 
			// Test collisions for all balls with a smaller number...
			if (getSquaredDistanceBetweenTwoPoints(balls[i].x, balls[i].y, balls[j].x, balls[j].y) < 8*8)
			{
				balls[i].xdirection = balls[i].x - balls[j].x > 0 ? 1 : -1;
				balls[i].ydirection = balls[i].y - balls[j].y > 0 ? 1 : -1;
				balls[j].xdirection = -(balls[i].xdirection);
				balls[j].ydirection = -(balls[i].ydirection);
                
				if (!bounceSoundPlayed)
				{
					//playSound(bounceSound);
					bounceSoundPlayed = 1;
				}
			}
		}
        
    }
}

void moveCursor(void)
{
    s8 x, y;
  
/*#ifdef TOUCH_SCREEN_CONTROLS_ENABLED

	touchTime++;

	if(isKeyPressed(KEY_TOUCH))
	{
		testTouch = 0;

		if(touchTime > 3)
			taps=0;

		touchTime = 0;
		touchPosition touchPos;
		hidTouchRead(&touchPos);
		onTouchScreenPressedX = touchPos.px;
		onTouchScreenPressedY = touchPos.py;
	}
	else if(isKeyReleased(KEY_TOUCH))
	{
		if(touchTime <= 3)
			taps++;

		if(taps == 2)
		{
			cursor.angle++;
			cursor.angle &= 1;
			taps = 0;
		}

		touchTime = 0;
	}

	if(isKeyDown(KEY_TOUCH))
	{
		testTouch++;

		touchPosition touchPos;
		hidTouchRead(&touchPos);
		cursor.x += (((s32)touchPos.px - (s32)onTouchScreenPressedX));
		cursor.y += (((s32)touchPos.py - (s32)onTouchScreenPressedY));
		onTouchScreenPressedX = touchPos.px;
		onTouchScreenPressedY = touchPos.py;
	}

#endif*/


	if(isKeyPressed(CONTROLS_RIGHT | CONTROLS_LEFT | CONTROLS_DOWN | CONTROLS_UP))
    {
		cursor.x += ( (isKeyPressed(CONTROLS_RIGHT) ? 1 : 0) - (isKeyPressed(CONTROLS_LEFT) ? 1 : 0) ) * 8;
		cursor.y += ( (isKeyPressed(CONTROLS_DOWN) ? 1 : 0) - (isKeyPressed(CONTROLS_UP) ? 1 : 0) ) * 8 ;
    }
    else
    {
		cursor.x += ( (isKeyDown(CONTROLS_RIGHT) ? 1 : 0) - (isKeyDown(CONTROLS_LEFT) ? 1 : 0) ) * 8;
		cursor.y += ( (isKeyDown(CONTROLS_DOWN) ? 1 : 0) - (isKeyDown(CONTROLS_UP) ? 1 : 0) ) * 8;
    }
    
    x = (cursor.x)>>3;
    y = (cursor.y)>>3;
   
	if (x < ((!cursor.angle) * -1)) { x = ((!cursor.angle) * -1); cursor.x = x<<3; }
    else if (x > (MAP_WIDTH-1)) {  x = (MAP_WIDTH-1); cursor.x = x<<3; }

    if (y < (cursor.angle * -1))
    {
		y = (cursor.angle * -1);
		cursor.y = y<<3;
    }
    else if (y > MAP_HEIGHT-1)
    {
		y = MAP_HEIGHT-1;
		cursor.y = y<<3;
    }

    if (isKeyPressed(CONTROLS_DIRECTION))
    {
		cursor.angle++;
		cursor.angle &= 1;
    }

    if (isKeyPressed(CONTROLS_VERTICAL))
		cursor.angle = 1;

	if (isKeyPressed(CONTROLS_HORIZONTAL))
		cursor.angle = 0; 
}

void clearEmptyRooms(void)
{
	u8 cleared[MAP_WIDTH][MAP_HEIGHT];
    
	u16 tilesComplete;
	u8 change = 1;
	s8 x, y, i;

	for (y=0; y<MAP_HEIGHT; y++)
	{
		rows[y] = 0;
		for (x=0; x<MAP_WIDTH; x++)
		{
			rows[x] = 0;
			cleared[x][y] = 1;
		}
	}

	for (i=0; i<ballCount; i++)
	{
		x = (balls[i].x + 4)>>3;
		y = (balls[i].y + 4)>>3;
		cleared[x][y] = 0;
	}

	while (change)
	{
		change = 0;
		for (y=0; y<MAP_HEIGHT; y++)
		{
			for (x=0; x<MAP_WIDTH; x++)
			{
				if (!cleared[x][y])
				{
					if (cleared[x][y+1] && (y+1) < MAP_HEIGHT && collision[(y+1)*MAP_WIDTH+(x)] != TILE_CLEARED)
					{
						cleared[x][y+1] = 0;
						change = 1;
					}
					if (cleared[x+1][y] && (x+1) < MAP_WIDTH && collision[(y)*MAP_WIDTH+(x+1)] != TILE_CLEARED)
					{
						cleared[x+1][y] = 0;
						change = 1;
					}
				}
			}
		}

		for (y=(MAP_HEIGHT-1); y>=0; y--)
		{
			for (x=(MAP_WIDTH-1); x>=0; x--)
			{
				if (!cleared[x][y])
				{
					if (cleared[x][y-1] && (y-1) >= 0 && collision[(y-1)*MAP_WIDTH+(x)] != TILE_CLEARED)
					{
						cleared[x][y-1] = 0;
						change = 1;
					}
					if (cleared[x-1][y] && (x-1) >= 0 && collision[(y)*MAP_WIDTH+(x-1)] != TILE_CLEARED)
					{
						cleared[x-1][y] = 0;
						change = 1;
					}
				}
			}
		}
	}

	tilesComplete = 0;

	for (y=0; y<MAP_HEIGHT; y++)
	{
		for (x=0; x<MAP_WIDTH; x++)
		{
			if (cleared[x][y])
			{
				setMapTile(x, y, TILE_CLEARED);
				collision[y*MAP_WIDTH+x] = TILE_CLEARED;

				if (map[y*MAP_WIDTH+x] != TILE_CLEARED) tilesComplete++;

				columns[x]++;
				rows[y]++;
			}
			else if (collision[y*MAP_WIDTH+x] == TILE_CLEARED)
			{
				if (map[y*MAP_WIDTH+x] != TILE_CLEARED) tilesComplete++;

				columns[x]++;
				rows[y]++;
			}
		}
	}

	complete = (u8) ((tilesComplete*100)/tilesToClear + .5);
	levelScore = tilesComplete * (30 + 5 * level);
	checkIfWin();
}

void buildWall(wall * thiswall, wall * otherwall, s16 xstep, s16 ystep, u16 tilenum, Rect* head)
{
	
    s8 x, y;
    if (complete < 75 && (isKeyPressed(CONTROLS_BUILD_WALL)))
     {
        x = ((cursor.x)>>3) + (xstep>0 ? 1 : 0);
        y = ((cursor.y)>>3) + (ystep>0 ? 1 : 0);

        if (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT && thiswall->xdirection == 0 && thiswall->ydirection == 0 && collision[y*MAP_WIDTH+x] == TILE_DEFAULT
            && !((otherwall->x1 != otherwall->x2 || otherwall->y1 != otherwall->y2) && (otherwall->xdirection == -1 || otherwall->ydirection == -1) && y*MAP_WIDTH+x == (otherwall->y2>>3)*MAP_WIDTH+(otherwall->x2>>3))
            && !((otherwall->x1 != otherwall->x2 || otherwall->y1 != otherwall->y2) && (otherwall->xdirection == 1 || otherwall->ydirection == 1) && y*MAP_WIDTH+x == ((otherwall->y2>>3)+otherwall->ydirection)*MAP_WIDTH+((otherwall->x2>>3)+otherwall->xdirection))
        )
          {
            thiswall->xdirection = xstep;
            thiswall->ydirection = ystep;

            thiswall->x1 = thiswall->x2 = (x<<3);
            thiswall->y1 = thiswall->y2 = (y<<3);
            thiswall->vx = thiswall->x2 << 8;
            thiswall->vy = thiswall->y2 << 8;

            setMapTile(x, y, tilenum);
            collision[y*MAP_WIDTH+x] = tilenum;
            
			head->x = thiswall->x2;
			head->y = thiswall->y2;
          }
      }
    else if (thiswall->xdirection != 0 || thiswall->ydirection != 0)
      {
        s8 xnext, ynext, otherxnext, otherynext;
        
        x = xnext = ((thiswall->x2 + 4)>>3);
        y = ynext = ((thiswall->y2 + 4)>>3);
        if (thiswall->xdirection != 0)
          xnext += thiswall->xdirection > 0 ? 1 : -1;
        if (thiswall->ydirection != 0)
          ynext += thiswall->ydirection > 0 ? 1 : -1;
        otherxnext = ((otherwall->x2 + 4 + otherwall->xdirection)>>3);
        otherynext = ((otherwall->y2 + 4 + otherwall->ydirection)>>3);
        
        if ( (thiswall->xdirection > 0 && thiswall->x2 == ((MAP_WIDTH-1)*8))
            || (thiswall->xdirection < 0 && thiswall->x2 == 0)
            || (thiswall->ydirection > 0 && thiswall->y2 == ((MAP_HEIGHT-1)*8))
            || (thiswall->ydirection < 0 && thiswall->y2 == 0)
            || (collision[ynext*MAP_WIDTH+xnext] == TILE_CLEARED && (thiswall->x2 & 7) == 0 && (thiswall->y2 & 7) == 0))
          {
            fillWall(thiswall, TILE_CLEARED, head);
            clearEmptyRooms();
          }
        else if (
               (collision[ynext*MAP_WIDTH+xnext] != TILE_DEFAULT && (thiswall->x2 & 7) == 0 && (thiswall->y2 & 7) == 0)
               || (thiswall->ydirection == 1 && otherwall->y2 == thiswall->y2 + 8 && otherwall->x2 < thiswall->x2 + 8 && otherwall->x2 > thiswall->x2 - 8)
               || (thiswall->ydirection == -1 && otherwall->y2 == thiswall->y2 - 8 && otherwall->x2 < thiswall->x2 + 8 && otherwall->x2 > thiswall->x2 - 8)
               || (thiswall->xdirection == 1 && otherwall->x2 == thiswall->x2 + 8 && otherwall->y2 < thiswall->y2 + 8 && otherwall->y2 > thiswall->y2 - 8)
               || (thiswall->xdirection == -1 && otherwall->x2 == thiswall->x2 - 8 && otherwall->y2 < thiswall->y2 + 8 && otherwall->y2 > thiswall->y2 - 8)
            )
          { 
			// blue and red collision
            // wait
          }
        else
          {
            thiswall->vx += thiswall->xdirection * GAME_SPEED;
            thiswall->vy += thiswall->ydirection * GAME_SPEED;
            thiswall->x2 = thiswall->vx >> 8;
            thiswall->y2 = thiswall->vy >> 8;

            if ((thiswall->x2 & 7) == 0 && (thiswall->y2 & 7) == 0)
            {
              x = ((thiswall->x2 + 4)>>3);
              y = ((thiswall->y2 + 4)>>3);
              
              setMapTile(x, y, tilenum);
              collision[y*MAP_WIDTH+x] = tilenum;
            }
          }
        
		head->x = thiswall->x2;
		head->y = thiswall->y2;
      }
}

void fillWall(wall * wall, u16 tilenum, Rect* head)
{
	s8 x, y, start, end, step;

	if (wall->xdirection != 0)
	{
		start = ((wall->x1)>>3);
		step = wall->xdirection;
		end = ((wall->x2)>>3) + step;
		y = (wall->y1)>>3;
		
		for (x=start; x!=end; x+=step)
		{
			setMapTile(x, y, tilenum == TILE_DEFAULT ? map[y*MAP_WIDTH+x] : tilenum);
			collision[y*MAP_WIDTH+x] = tilenum;
		}
	}
	else if (wall->ydirection != 0)
	{
		start = ((wall->y1)>>3);
		step = wall->ydirection;
		end = ((wall->y2)>>3) + step;
		x = (wall->x1)>>3;
		
		for (y=start; y!=end; y+=step)
		{
			setMapTile(x, y, tilenum == TILE_DEFAULT ? map[y*MAP_WIDTH+x] : tilenum);
			collision[y*MAP_WIDTH+x] = tilenum;
			if (tilenum == TILE_CLEARED)
			{
				columns[x]++;
				rows[y]++;
			}
		}
	}

	wall->x1 = wall->x2 = wall->y1 = wall->y2 = -8;
	wall->vx = wall->vy = -8 << 3;
	wall->xdirection = wall->ydirection = 0;

	head->x = 0;
	head->y = 0;

	checkIfWin();
}

u8 getMapTile(u16 x, u16 y)
{
	return mapDraw[y * MAP_WIDTH + x];
}
void setMapTile(u16 x, u16 y, u8 tilenum)
{
	if(y * MAP_WIDTH + x < MAP_WIDTH * MAP_HEIGHT)
	{
		mapDraw[y * MAP_WIDTH + x] = tilenum;
		//drawBackgroundTile(mapDraw[y * MAP_WIDTH + x], x, y);
	}
}
void checkIfWin(void)
{
	if (complete >= 75 && blueWall.xdirection == 0 && blueWall.ydirection == 0 && redWall.xdirection == 0 && redWall.ydirection == 0)
    {
		s8 x, y;
        
		for (y=0; y<MAP_HEIGHT; y++)
			if (rows[y] == MAP_WIDTH)
			bonus += (30 + 5 * level) * 32;
		for (x=0; x<MAP_WIDTH; x++)
			if (columns[x] == MAP_HEIGHT)
			bonus += (30 + 5 * level) * 24;
        
		bonus += (timeLeft * lives * level * GAME_SPEED) >> 8;
		bonus += (((complete - 79) * level * GAME_SPEED) >> 8) * 100;
		bonus += (((complete - 89) * level * GAME_SPEED) >> 8) * 200;
        
		levelScore += bonus;
		levelComplete = 1;
    }
}
void gameOver()	
{
	totalScore += levelScore;
	levelScore = 0;
	inputHighscore = isHighscore(totalScore);
	gameover = (inputHighscore == -1);
	if(gameover == 1) loadHighscores(highscores, &highscoreCount);
}

void jezzballDraw()
{
	if (cls > 0)
	{
		clearScreen();
		cls--;
	}
	drawBackground();
	drawBalls();
	
	if(!gameover)
	{
		drawScore();
		drawWallHeads();
		drawCursor();
	}

	if(!gameover && inputHighscore != -1)
	{
		/*Rect box = { 80, 80, 240, 70 }; 
		Rect box2 = { 75, 75, 250, 80 }; 
		
		
		drawRect(&box2, 0x84, 0x84, 0x84, 0xFF);
		drawRect(&box, 0xFE, 0xFE, 0xFE, 0xFF);

		drawString(FONTFAMILY_VERDANA10, 200, 93, i18nNewHighscore(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
		drawString(FONTFAMILY_VERDANA10, 200, 117, i18nEnterYourName(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);

		keyboardDraw();*/
	}
	else if(levelComplete)
	{
		Rect box = { 80, 80, 240, 70 }; 
		Rect box2 = { 75, 75, 250, 80 }; 
		drawRect(&box2, 0x84, 0x84, 0x84, 0xFF);
		drawRect(&box, 0xFE, 0xFE, 0xFE, 0xFF);
		//drawRect3D(&box, 0x67, 0x8f, 0xab, 0);
		
		//char levelCompleteText[50];
		//sprintf(levelCompleteText, i18nLevelCompleted(), level);
		//drawString(FONTFAMILY_VERDANA10, 200, 93, levelCompleteText, TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
		drawString(FONTFAMILY_VERDANA10, 200, 93, i18nLevelCompleted(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
		drawString(FONTFAMILY_VERDANA10, 200, 117, i18nPressAToContinue(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
	}
	else if(gameover)
	{
		drawScore();

		Rect box = { 50, 25, 290, 190 }; 
		Rect box2 = { 45, 20, 300, 200 }; 
		drawRect(&box2, 0x84, 0x84, 0x84, 0xFF);
		drawRect(&box, 0xFE, 0xFE, 0xFE, 0xFF);

		drawString(FONTFAMILY_VERDANA11, 200, 182, i18nGameOver(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
		drawString(FONTFAMILY_VERDANA11, 200, 195, i18nPressAToTryAgain(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);

		drawString(FONTFAMILY_VERDANA11, 120, 25, i18nPlayer(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
		drawString(FONTFAMILY_VERDANA11, 220, 25, i18nLevel(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
		drawString(FONTFAMILY_VERDANA11, 290, 25, i18nScore(), TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);

	
		/*char playerText[15];
		char levelText[5];
		char scoreText[15];

		u8 i=0;
		for(i=0; i < highscoreCount; i++)
		{
			sprintf(playerText, "%s", highscores[i].player);
			sprintf(levelText, "%d", highscores[i].level);
			sprintf(scoreText, "%d", highscores[i].score);

			drawString(FONTFAMILY_VERDANA10, 120, 32 + ((i+1)*13), playerText, TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
			drawString(FONTFAMILY_VERDANA10, 220, 32 + ((i+1)*13), levelText, TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);
			drawString(FONTFAMILY_VERDANA10, 290, 32 + ((i+1)*13), scoreText, TEXT_ALIGN_CENTER, 0xFE, 0xFE, 0xFE);

			if(inputHighscore == i)
			{
				Rect highlighter = { 58, 33 + ((i+1)*13), 273, 13 }; 
				drawRect(&highlighter, 0x80, 0x80, 0x80, 0x78);
			}
		}*/
	}

}

void drawBackgroundTile(u8 tile, u16 mapX, u16 mapY)
{
	Rect dest = {(mapX*8)+DRAWING_OFFSET_X,(mapY*8)+ DRAWING_OFFSET_Y,8,8};

	if (tile == TILE_CLEARED)
		drawRect(&dest,0x52,0x52,0x52, 0xFF);	
	else
	{
		Rect src = {0,0 + (tile*8),8,8};
		drawImage(sprites_data.pixel_data, sprites_data.width, sprites_data.height, &src, &dest); //TILE_DEFAULT, TILE_BLUE, TILE_RED
	}
}

void drawBackground() {

	u16 i,j;
	for(i=0; i < MAP_WIDTH; i++)
	{
		for(j=0; j < MAP_HEIGHT; j++) 
		{
			drawBackgroundTile(mapDraw[j * MAP_WIDTH + i], i, j);
		}
	}

}
void drawBall(sprite* ball) {

	ball->animTimer ++;
	if(ball->animTimer > 3)
	{
		ball->animTimer = 0;
		ball->animFrame ++;
		ball->animFrame %= 9;
	}
	
	ballPos.x = ball->x + DRAWING_OFFSET_X;
	ballPos.y = ball->y + DRAWING_OFFSET_Y;

	drawImage(sprites_data.pixel_data, sprites_data.width, sprites_data.height,  &ballAnimFrames[ball->animFrame], &ballPos);
}

void drawBalls()
{
	u8 i=0;
	for (i=0; i<ballCount; i++)
	{
		drawBall(&balls[i]);
	}
}
void drawCursor()
{
	Rect dest = {(((cursor.x>>3)-1)<<3)+ 8 + DRAWING_OFFSET_X,(((cursor.y>>3)-1)<<3)+8 + DRAWING_OFFSET_Y,8,16};
	Rect src = {0,15*8,16,8};
	
	if (cursor.angle)
	{
		src.y = 16*8;
		src.w = 8;
		src.h = 16;
	}
	
	drawImage(sprites_data.pixel_data, sprites_data.width, sprites_data.height, &src, &dest);
}
void drawWallHeads()
{
	Rect redSrc = {0,3*8,8,8};
	Rect blueSrc = {0,4*8,8,8};

	if(blueWallHead.x > 0 && blueWallHead.y > 0 && blueWallHead.x < MAP_WIDTH*8 && blueWallHead.y < MAP_HEIGHT*8)
	{
		Rect blueWallHeadDraw = { blueWallHead.x + DRAWING_OFFSET_X, blueWallHead.y + DRAWING_OFFSET_Y, blueWallHead.w, blueWallHead.h };
		drawImage(sprites_data.pixel_data, sprites_data.width, sprites_data.height, &blueSrc, &blueWallHeadDraw);
	}

	if(redWallHead.x > 0 && redWallHead.y > 0 && redWallHead.x < MAP_WIDTH*8 && redWallHead.y < MAP_HEIGHT*8)
	{
		Rect redWallHeadDraw = { redWallHead.x + DRAWING_OFFSET_X, redWallHead.y + DRAWING_OFFSET_Y, redWallHead.w, redWallHead.h };
		drawImage(sprites_data.pixel_data, sprites_data.width, sprites_data.height, &redSrc, &redWallHeadDraw);
	}
}
void drawScore()
{	
	Rect bg = { 0,0,144 / 2,480 / 2 };
	//drawRect(&bg, 0xFF,0xFF,0xFF,0xFF);

	char scoreText[50];
	char livesText[10];
	char levelText[10];
	char completedText[50];
	char timeText[50];
	//char debugText[50];

	numToString(levelText, level);
	numToString(livesText, lives);
	numToString(completedText, complete);
	numToString(scoreText, totalScore + levelScore);
	numToString(timeText, timeLeft);

	/*sprintf(levelText, "%d", level);
	sprintf(livesText, "%d", lives);
	sprintf(completedText, "%d", complete);
	sprintf(scoreText, "%d", totalScore + levelScore);
	sprintf(timeText, "%d", timeLeft);*/


	drawString(FONTFAMILY_VERDANA14_BOLD, 5, 0, i18nLevel(), TEXT_ALIGN_LEFT, 0, 0, 0);
	drawString(FONTFAMILY_VERDANA14_BOLD, 5, 40, i18nCompleted(), TEXT_ALIGN_LEFT, 0, 0, 0);
	drawString(FONTFAMILY_VERDANA14_BOLD, 5, 80, i18nLives(), TEXT_ALIGN_LEFT, 0, 0, 0);
	drawString(FONTFAMILY_VERDANA14_BOLD, 5, 120, i18nScore(), TEXT_ALIGN_LEFT, 0, 0, 0);
	drawString(FONTFAMILY_VERDANA14_BOLD, 5, 160, i18nTimeLeft(), TEXT_ALIGN_LEFT, 0, 0, 0);

	bg.w = 120;
	bg.h = 20;
	bg.x = 10;

	bg.y = 20; drawRect(&bg, 0xFF, 0xFF, 0xFF, 0xFF);
	drawString(FONTFAMILY_VERDANA14, 130, 20, levelText, TEXT_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF);
	bg.y = 60; drawRect(&bg, 0xFF, 0xFF, 0xFF, 0xFF);
	drawString(FONTFAMILY_VERDANA14, 130, 60, completedText, TEXT_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF);
	bg.y = 100; drawRect(&bg, 0xFF, 0xFF, 0xFF, 0xFF);
	drawString(FONTFAMILY_VERDANA14, 130, 100, livesText, TEXT_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF);
	bg.y = 140; drawRect(&bg, 0xFF, 0xFF, 0xFF, 0xFF);
	drawString(FONTFAMILY_VERDANA14, 130, 140, scoreText, TEXT_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF);
	bg.y = 180; drawRect(&bg, 0xFF, 0xFF, 0xFF, 0xFF);
	drawString(FONTFAMILY_VERDANA14, 130, 180, timeText, TEXT_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF);

}

u32 randMinMax(u32 min, u32 max)
{
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr = (lfsr >> 1) | (bit << 15);
	return lfsr % max + min;
}
s32 getSquaredDistanceBetweenTwoPoints(s16 x1, s16 y1, s16 x2, s16 y2)
{
	return ((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1));  
}