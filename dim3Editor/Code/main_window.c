/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Window Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

map_type						map;
file_path_setup_type			file_path_setup;
iface_type						iface;
app_state_type					state;

extern list_palette_type		file_palette,property_palette;

/* =======================================================

      Initialize and Shutdown
	        
======================================================= */

void main_wind_initialize(void)
{
		// initializations
		
	text_initialize();
	
	tool_palette_initialize("Editor");
	list_palette_initialize("Editor");
	file_palette_initialize();
	property_palette_initialize();

	tool_tip_initialize();

	view_initialize();
   
        // misc setup
        
	state.map.vertex_mode=vertex_mode_none;
	state.map.drag_mode=drag_mode_mesh;
	state.map.grid_mode=grid_mode_small;
	state.map.handle_mode=handle_mode_rotate;
	
	state.map.free_look=TRUE;
	state.map.select_add=FALSE;
	
	state.map.drag_handle_idx=-1;
	
		// update view
	
	menu_update_view();
}

void main_wind_shutdown(void)
{
	view_shutdown();
	
	property_palette_shutdown();
	file_palette_shutdown();
	list_palette_shutdown();
	
	tool_palette_shutdown();
	
	text_shutdown();
}

/* =======================================================

      GL Setup
      
======================================================= */

void main_wind_gl_setup(void)
{
		// setup

	glClearColor(1.0f,1.0f,1.0f,0.0f);

	glEnable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
	glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	
	glEnable(GL_DEPTH_TEST);

 	glEnableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      Drawing
      
======================================================= */

void main_wind_clear_viewport(void)
{
	int				wid,high;
	d3rect			wbox;

	os_get_window_box(&wbox);

	wid=wbox.rx-wbox.lx;
	high=wbox.by-wbox.ty;

	glScissor(0,0,wid,high);
	glViewport(0,0,wid,high);
}

void main_wind_draw_no_swap(void)
{
		// clear draw buffer

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	main_wind_clear_viewport();

	glClearColor(0.75f,0.75f,0.75f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// draw window

	if (state.map.map_open) {
		if (state.map.texture_edit_idx==-1) {
			view_draw();
		}
		else {
			texture_edit_draw();
		}
	}
	
	tool_palette_draw();
	texture_palette_draw(map.textures);
	
	file_palette_draw();
	property_palette_draw();

	tool_tip_draw();
}

void main_wind_draw(void)
{
	main_wind_draw_no_swap();
	os_swap_gl_buffer();
}

/* =======================================================

      Clicking
      
======================================================= */

void main_wind_click(d3pnt *pnt,bool double_click)
{
	d3rect				tbox;
	
		// tool palette

	tool_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		tool_palette_click(pnt);
		return;
	}

		// texture palette
		
	texture_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		texture_palette_click(map.textures,pnt,double_click);
		return;
	}
	
		// file palettes
	
	list_palette_total_box(&file_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		file_palette_click(pnt,double_click);
		return;
	}

		// property palette
		
	list_palette_total_box(&property_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		property_palette_click(pnt,double_click);
		return;
	}

		// view clicks

	if (!state.map.map_open) return;

	if (state.map.texture_edit_idx==-1) {
		view_click(pnt,double_click);
	}
	else {
		texture_edit_click(pnt,double_click);
	}
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(d3pnt *pnt,int delta)
{
	d3rect				tbox;
	
		// scroll wheel in file palette

	list_palette_total_box(&file_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		file_palette_scroll_wheel(pnt,delta);
		return;
	}

		// scroll wheel in item, property, or alt property palette

	list_palette_total_box(&property_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		property_palette_scroll_wheel(pnt,delta);
		return;
	}

		// scroll wheel in view

	if (state.map.texture_edit_idx==-1) {
		view_scroll_wheel(pnt,delta);
	}
	else {
		texture_edit_scroll_wheel(delta);
	}
}

/* =======================================================

      Mouse Movement
      
======================================================= */

void main_wind_mouse_move(d3pnt *pnt)
{
	if (!state.map.map_open) return;

	tool_palette_mouse_move(pnt);
}

/* =======================================================

      Cursors
      
======================================================= */

bool main_wind_cursor(void)
{
	d3pnt			pnt;
	
	os_get_cursor(&pnt);

	if (state.map.texture_edit_idx==-1) return(view_cursor(&pnt));
	return(texture_edit_cursor());
}

/* =======================================================

      Keys
      
======================================================= */

void main_wind_key(char ch)
{
	editor_view_type		*view;
	
		// special movement keys
		
	if ((ch=='w') || (ch=='W')) {
		view=view_get_current_view();
		view_key_forward_movement(view,-1);
		return;
	}

	if ((ch=='s') || (ch=='S')) {
		view=view_get_current_view();
		view_key_forward_movement(view,1);
		return;
	}
	
	if ((ch=='a') || (ch=='A')) {
		view=view_get_current_view();
		view_key_side_movement(view,-1);
		return;
	}

	if ((ch=='d') || (ch=='D')) {
		view=view_get_current_view();
		view_key_side_movement(view,1);
		return;
	}

	if ((ch=='q') || (ch=='Q')) {
		view=view_get_current_view();
		view_key_vert_movement(view,-1);
		return;
	}
	
	if ((ch=='e') || (ch=='E')) {
		view=view_get_current_view();
		view_key_vert_movement(view,1);
		return;
	}
	
		// panel keys
		
	view_key(ch);
}

/* =======================================================

      Resize Setup
      
======================================================= */

void main_wind_resize(void)
{
}














