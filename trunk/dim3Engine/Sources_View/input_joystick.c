/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Joystick Input Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "inputs.h"

extern setup_type			setup;

SDL_Joystick				*input_joystick;

/* =======================================================

      Joystick Initialize
      
======================================================= */

bool input_joystick_initialize(void)
{
	input_joystick=NULL;

		// any joysticks?
		
	if (SDL_NumJoysticks()==0) return(FALSE);

	input_joystick=SDL_JoystickOpen(0);
	if (input_joystick==NULL) return(FALSE);

		// start up events

	SDL_JoystickEventState(SDL_ENABLE);
	
	return(TRUE);
}

void input_joystick_shutdown(void)
{
	if (input_joystick==NULL) return;

	SDL_JoystickEventState(SDL_IGNORE);
	SDL_JoystickClose(input_joystick);

	input_joystick=NULL;
}

/* =======================================================

      Check If JoyStick Input OK
      
======================================================= */

inline bool input_check_joystick_ok(void)
{
	return(input_joystick!=NULL);
}

/* =======================================================

      Game JoyStick Input
      
======================================================= */

inline float input_get_joystick_axis(int axis)
{
	float				f;
	
		// get joystick to be between 1 and 20
		
	f=(float)SDL_JoystickGetAxis(input_joystick,axis);
	if ((f>-8192) && (f<8192)) return(0.0f);
	
	f/=(32768.0f/20.0f);
	
	f*=setup.joystick.speed;
	return(f+(f*setup.joystick.acceleration));
}

inline bool input_get_joystick_axis_as_button_min(int axis)
{
	return(SDL_JoystickGetAxis(input_joystick,axis)<-8192);
}

inline bool input_get_joystick_axis_as_button_max(int axis)
{
	return(SDL_JoystickGetAxis(input_joystick,axis)>8192);
}

bool input_get_joystick_button(int button_idx)
{
		// regular joystick input buttons
		
	if ((button_idx>=input_joystick_button_1) && (button_idx<=input_joystick_button_16)) {
		return(SDL_JoystickGetButton(input_joystick,(button_idx-input_joystick_button_1))!=0);
	}

		// these inputs are hard left/right/up/down on an axis
		// we determine this by a movement greater then halfway
	
	switch (button_idx) {

		case input_joystick_button_left:
			return(SDL_JoystickGetAxis(input_joystick,0)<-8192);

		case input_joystick_button_right:
			return(SDL_JoystickGetAxis(input_joystick,0)>8192);

		case input_joystick_button_up:
			return(SDL_JoystickGetAxis(input_joystick,1)<-8192);

		case input_joystick_button_down:
			return(SDL_JoystickGetAxis(input_joystick,1)>8192);

	}
	
	return(FALSE);
}
