#include "program.h"

#include <coreinit/core.h>
#include <coreinit/debug.h>
#include <coreinit/internal.h>
#include <coreinit/screen.h>
#include <coreinit/foreground.h>
#include <proc_ui/procui.h>
#include <vpad/input.h>
#include "memory.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "i18n.h"
#include "bmp.h"
#include "rect.h"
#include "draw.h"
#include "jezzball.h"
#include "controls.h"

char log_buf[0x400];

bool isAppRunning = true;
bool initialized = false;

void screenInit()
{
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	__os_snprintf(log_buf, 0x400, "Screen sizes %x, %x\n", buf0_size, buf1_size);
	OSReport(log_buf);
	
	//Set the buffer area.
	screenBuffer = MEM1_alloc(buf0_size + buf1_size, 0x40);
	__os_snprintf(log_buf, 0x400, "Allocated screen buffers at %x\n", screenBuffer);
	OSReport(log_buf);

    OSScreenSetBufferEx(0, screenBuffer);
    OSScreenSetBufferEx(1, (screenBuffer + buf0_size));
    OSReport("Set screen buffers\n");

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);
    
    //Clear both framebuffers.
	for (int ii = 0; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
}

void screenDeinit()
{
    for(int ii = 0; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
    
    MEM1_free(screenBuffer);
}

void SaveCallback()
{
   OSSavesDone_ReadyToRelease(); // Required
}

bool AppRunning()
{
   if(!OSIsMainCore())
   {
      ProcUISubProcessMessages(true);
   }
   else
   {
      ProcUIStatus status = ProcUIProcessMessages(true);
    
      if(status == PROCUI_STATUS_EXITING)
      {
          // Being closed, deinit things and prepare to exit
          isAppRunning = false;
          
          if(initialized)
          {
              initialized = false;
              screenDeinit();
              memoryRelease();
          }
          
          ProcUIShutdown();
      }
      else if(status == PROCUI_STATUS_RELEASE_FOREGROUND)
      {
          // Free up MEM1 to next foreground app, etc.
          initialized = false;
          
          screenDeinit();
          memoryRelease();
          ProcUIDrawDoneRelease();
      }
      else if(status == PROCUI_STATUS_IN_FOREGROUND)
      {
         // Reallocate MEM1, reinit screen, etc.
         if(!initialized)
         {
            initialized = true;
            
            memoryInitialize();
            screenInit();
         }
      }
   }

   return isAppRunning;
}

int main(int argc, char **argv)
{	
    OSScreenInit();
    OSReport("Screen initted\n");
    
    ProcUIInit(&SaveCallback);

	drawInit();
	jezzballInit();

	while (AppRunning())
	{
	    if(!initialized) continue;
	 
		updateControls();
		jezzballUpdate();
		jezzballDraw();
		flipBuffers();
	}

	drawFinish();
	return 0;
}
