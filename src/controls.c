#include "controls.h"

u32 kPressed;
u32 kDown;
u32 kReleased;

void updateControls()
{
	int error;
	VPADStatus vpad_data;
	VPADRead(0, &vpad_data, 1, &error);
	kDown = vpad_data.hold;
	kReleased = vpad_data.release;
	kPressed = vpad_data.trigger;
}

u32 isKeyPressed(u32 key){ 
	return kPressed & key; 
}
u32 isKeyDown(u32 key){ 
	return kDown & key;  
}
u32 isKeyReleased(u32 key){
	return kReleased & key;
}

