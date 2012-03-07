/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Mouse Input Routines

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern iface_type			iface;
extern setup_type			setup;

int							mouse_tick;
d3pnt						mouse_motion,mouse_gui_pnt;
bool						mouse_button_state[input_max_mouse_button];

#ifndef D3_SDL_1_3
int							mouse_wheel_up_count,mouse_wheel_down_count;
bool						mouse_wheel_up_reset,mouse_wheel_down_reset;
#endif

/* =======================================================

      Mouse Initialize
      
======================================================= */

void input_mouse_initialize(void)
{
#ifdef D3_SDL_1_3
	SDL_SetRelativeMouseMode(TRUE);
#else
	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);
#endif
}

void input_mouse_shutdown(void)
{
#ifdef D3_SDL_1_3
	SDL_SetRelativeMouseMode(FALSE);
#else
	SDL_WM_GrabInput(SDL_GRAB_OFF);
#endif
}

/* =======================================================

      Mouse Clear
      
======================================================= */

void input_mouse_clear(void)
{
	int				n;
	
	mouse_tick=-1;

	mouse_motion.x=0;
	mouse_motion.y=0;
	
	for (n=0;n!=input_max_mouse_button;n++) {
		mouse_button_state[n]=FALSE;
	}

	#ifndef D3_SDL_1_3
		mouse_wheel_up_count=0;
		mouse_wheel_down_count=0;
		mouse_wheel_up_reset=FALSE;
		mouse_wheel_down_reset=FALSE;
	#endif
}

/* =======================================================

      Mouse Pausing
      
======================================================= */

void input_mouse_pause(void)
{
#ifdef D3_SDL_1_3
	SDL_SetRelativeMouseMode(FALSE);
#else
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);
#endif
}

void input_mouse_resume(void)
{
		// turn back off cursor
		
#ifdef D3_SDL_1_3
	SDL_SetRelativeMouseMode(TRUE);
#else
	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);
#endif

		// pump out any changes

	input_event_pump();
	input_mouse_clear();
}

/* =======================================================

      Mouse Events
      
======================================================= */

void input_event_mouse_button(int button,bool down)
{
	switch (button) {
		case 1:
			mouse_button_state[input_mouse_button_left]=down;
			break;
		case 2:
			mouse_button_state[input_mouse_button_middle]=down;
			break;
		case 3:
			mouse_button_state[input_mouse_button_right]=down;
			break;
	}

#ifndef D3_SDL_1_3
	if (down) {
		if (button==SDL_BUTTON_WHEELUP) {
			mouse_button_state[input_mouse_button_wheel_up]=TRUE;
			return;
		}
		if (button==SDL_BUTTON_WHEELDOWN) {
			mouse_button_state[input_mouse_button_wheel_down]=TRUE;
			return;
		}
	}
#endif
}

void input_event_mouse_motion(int x,int y)
{
	mouse_motion.x+=x;
	mouse_motion.y+=y;
}

#ifdef D3_SDL_1_3
void input_event_mouse_wheel(int y)
{
	if (y>0) mouse_button_state[input_mouse_button_wheel_up]=TRUE;
	if (y<0) mouse_button_state[input_mouse_button_wheel_down]=TRUE;
}
#endif

/* =======================================================

      Game Mouse Input
      
======================================================= */

void input_get_mouse_movement(float *x,float *y)
{	
	int			tick;
	float		fx,fy,f_tick;
	
		// get movement and clear
		
	fx=(float)mouse_motion.x;
	mouse_motion.x=0;

	fy=(float)mouse_motion.y;
	mouse_motion.y=0;

		// calculate time change
		
	tick=game_time_get_raw();

	if (mouse_tick!=-1) {
		f_tick=(float)input_tick_rate/(float)(tick-mouse_tick);
		fx*=f_tick;
		fy*=f_tick;
	}

	mouse_tick=tick;

		// calculate speed and acceleration

	fx*=setup.mouse.speed;
	*x=fx+(fx*setup.mouse.acceleration);

	fy*=setup.mouse.speed;
	*y=fy+(fy*setup.mouse.acceleration);
}

bool input_get_mouse_button(int button)
{
	return(mouse_button_state[button]);
}

void input_mouse_wheel_reset(void)
{
	mouse_button_state[input_mouse_button_wheel_up]=FALSE;
	mouse_button_state[input_mouse_button_wheel_down]=FALSE;
}

/* =======================================================

      Mouse GUI Input
      
======================================================= */

void input_mouse_gui_set_position(int x,int y)
{
		// clear mouse
		
	input_mouse_clear();
	
		// center to window
		
	mouse_gui_pnt.x=iface.scale_x>>1;
	mouse_gui_pnt.y=iface.scale_y>>1;
}

void input_mouse_gui_get_position(int *x,int *y)
{
	mouse_gui_pnt.x+=mouse_motion.x;
	mouse_motion.x=0;

	if (mouse_gui_pnt.x<0) mouse_gui_pnt.x=0;
	if (mouse_gui_pnt.x>=iface.scale_x) mouse_gui_pnt.x=iface.scale_x-1;
	
	mouse_gui_pnt.y+=mouse_motion.y;
	mouse_motion.y=0;

	if (mouse_gui_pnt.y<0) mouse_gui_pnt.y=0;
	if (mouse_gui_pnt.y>=iface.scale_y) mouse_gui_pnt.y=iface.scale_y-1;
	
	*x=mouse_gui_pnt.x;
	*y=mouse_gui_pnt.y;
}

void input_mouse_gui_get_hilite_position(int *x,int *y)
{
	input_mouse_gui_get_position(x,y);
}

bool input_mouse_gui_is_click_down(void)
{
	return(input_get_mouse_button(input_mouse_button_left));
}
