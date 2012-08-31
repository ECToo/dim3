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
model_type						model;
file_path_setup_type			file_path_setup;
iface_type						iface;
app_state_type					state;

tool_palette_tool_type			map_tool_palette_setup[map_tool_count]=map_tool_palette_def;
tool_palette_type				map_tool_palette;

extern list_palette_type		file_palette,
								project_palette,map_palette,model_palette;

/* =======================================================

      Initialize and Shutdown
	        
======================================================= */

void main_wind_initialize(void)
{
		// initializations
		
	text_initialize();

//	tool_palette_initialize();
	tool_palette_initialize(&map_tool_palette,"Editor",map_tool_count,map_tool_palette_setup);
//	tool_palette_initialize();

	list_palette_initialize("Editor");
	file_palette_initialize();
	project_palette_initialize();
	map_palette_initialize();
//	model_palette_initialize();		// supergumba -- model

	tool_tip_initialize();

	view_initialize();
   
        // misc setup

	state.mode=app_mode_project;
        
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
	
	project_palette_shutdown();
	map_palette_shutdown();
//	model_palette_shutdown();		// supergumba -- model

	file_palette_shutdown();
	list_palette_shutdown();
	
//	tool_palette_shutdown(&project_tool_palette);		// supergumba -- model
	tool_palette_shutdown(&map_tool_palette);
//	tool_palette_shutdown(&model_tool_palette);		// supergumba -- model
	
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

void main_wind_draw_project(void)
{
//	project_tool_palette_set_state();
//	tool_palette_draw(&project_tool_palette);		// supergumba -- model
	project_palette_draw();
}

void main_wind_draw_map(void)
{
	if (state.map.map_open) {
		if (state.map.texture_edit_idx==-1) {
			view_draw();
		}
		else {
			texture_edit_draw();
		}
	}
	
	map_tool_palette_set_state();
	tool_palette_draw(&map_tool_palette);

	texture_palette_draw(map.textures);
	
	map_palette_draw();
}

void main_wind_draw_model(void)
{
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

	switch (state.mode) {
		case app_mode_project:
			main_wind_draw_project();
			break;
		case app_mode_map:
			main_wind_draw_map();
			break;
		case app_mode_model:
			main_wind_draw_model();
			break;
	}

		// all views have file palette
		// and tool tips

	file_palette_draw();
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

void main_wind_click_project(d3pnt *pnt,bool double_click)
{
	d3rect				tbox;

		// tool palette

	tool_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
	//	project_tool_palette_set_state();
	//	idx=tool_palette_click(&project_tool_palette,pnt);		// supergumba -- model
	//	if (idx!=-1) project_tool_palette_click(idx);
		return;
	}

		// property palette
		
	list_palette_total_box(&project_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		project_palette_click(pnt,double_click);
		return;
	}
}

void main_wind_click_map(d3pnt *pnt,bool double_click)
{
	int					idx;
	d3rect				tbox;
	
		// tool palette

	tool_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		map_tool_palette_set_state();
		idx=tool_palette_click(&map_tool_palette,pnt);
		if (idx!=-1) map_tool_palette_click(idx);
		return;
	}

		// texture palette
		
	texture_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		texture_palette_click(map.textures,pnt,double_click);
		return;
	}

		// property palette
		
	list_palette_total_box(&map_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		map_palette_click(pnt,double_click);
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

void main_wind_click_model(d3pnt *pnt,bool double_click)
{
}

void main_wind_click(d3pnt *pnt,bool double_click)
{
	d3rect				tbox;

		// file palettes
	
	list_palette_total_box(&file_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		file_palette_click(pnt,double_click);
		return;
	}

		// other clicks

	switch (state.mode) {
		case app_mode_project:
			main_wind_click_project(pnt,double_click);
			break;
		case app_mode_map:
			main_wind_click_map(pnt,double_click);
			break;
		case app_mode_model:
			main_wind_click_model(pnt,double_click);
			break;
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

	list_palette_total_box(&map_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		map_palette_scroll_wheel(pnt,delta);
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
	switch (state.mode) {

		case app_mode_project:
		//	tool_palette_mouse_move(&project_tool_palette,pnt);	// supergumba -- model
			break;

		case app_mode_map:
			tool_palette_mouse_move(&map_tool_palette,pnt);
			break;

		case app_mode_model:
		//	tool_palette_mouse_move(&model_tool_palette,pnt);	// supergumba -- model
			break;
	}
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














