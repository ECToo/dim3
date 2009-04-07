/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Console Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "consoles.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"

extern int						console_mode,console_count;
extern console_line_type		console_line[max_console_line];

extern view_type				view;
extern server_type				server;
extern hud_type					hud;
extern setup_type				setup;
extern network_setup_type		net_setup;
extern render_info_type			render_info;

/* =======================================================

      Console View Offsets
      
======================================================= */

int console_y_offset(void)
{
	int			y;
	
	switch (console_mode) {
		
		case console_mode_close:
			return(0);
			
		case console_mode_fps:
			y=gl_text_get_char_height(hud.font.text_size_small);
			return((y*setup.screen.y_sz)/hud.scale_y);
			
		case console_mode_open:
			y=gl_text_get_char_height(hud.font.text_size_small);
			y=(max_console_line*y)+2;
			return((y*setup.screen.y_sz)/hud.scale_y);
		
	}
	
	return(0);
}

/* =======================================================

      Console Input
      
======================================================= */
	
void console_input(void)
{
	if (input_action_get_state_single(nc_console)) console_next_mode();
}

/* =======================================================

      Draw Console
      
======================================================= */

void console_draw_fps(void)
{
	int					y,lx,rx,ty,by,y_add;
	char				fps_txt[64],lat_txt[32];
	d3col				col;
	
		// setup view
				
	gl_2D_view_interface();
	
		// setup fonts
	
    y_add=gl_text_get_char_height(hud.font.text_size_small);
    
		// background

	lx=0;
	rx=hud.scale_x;
	ty=hud.scale_y-y_add;
	by=hud.scale_y;

	y=ty+(y_add+1);

	col.r=col.g=col.b=0.75f;
	view_draw_next_vertex_object_2D_color_quad(lx,ty,&col,rx,ty,&col,rx,by,&col,lx,by,&col,1.0f);
	
	gl_text_start(hud.font.text_size_small);
	col.r=col.g=col.b=0.0f;
	
		// version
		
	gl_text_draw(5,y,dim3_version,tx_left,FALSE,&col,1.0f);
	
		// fps & latency
	
	if (view.fps.total==0) {
		strcpy(fps_txt,"--.--");
	}
	else {
		if ((setup.lock_fps_refresh) && (view.fps.total>render_info.monitor_refresh_rate)) {
			sprintf(fps_txt,"%d.00",render_info.monitor_refresh_rate);
		}
		else {
			sprintf(fps_txt,"%.2f",view.fps.total);
		}
	}

	if (net_setup.client.joined) {
		if (net_setup.client.latency>=100) {
			strcat(fps_txt," [??]");
		}
		else {
			sprintf(lat_txt,"[%d]",net_setup.client.latency);
			strcat(fps_txt,lat_txt);
		}
	}

	gl_text_draw((hud.scale_x-5),y,fps_txt,tx_right,FALSE,&col,1.0f);

	gl_text_end();
}
	
void console_draw_open(void)
{
	int					i,y,lx,rx,ty,ty2,by,y_add;
	d3col				col;
	console_line_type	*cline;
	
		// setup view
				
	gl_2D_view_interface();
	
		// setup fonts
	
    y_add=gl_text_get_char_height(hud.font.text_size_small);
    
		// console background
		
	lx=0;
	rx=hud.scale_x;
	ty=hud.scale_y-((max_console_line*y_add)+2);
	ty2=ty+2;
	by=hud.scale_y;
	
	y=ty2+1;

	col.r=col.g=col.b=0.75f;
	view_draw_next_vertex_object_2D_color_quad(lx,ty,&col,rx,ty,&col,rx,ty2,&col,lx,ty2,&col,1.0f);

	col.r=col.g=col.b=0.05f;
	view_draw_next_vertex_object_2D_color_quad(lx,ty2,&col,rx,ty2,&col,rx,by,&col,lx,by,&col,1.0f);

		// console lines
		
	gl_text_start(hud.font.text_size_small);
	
	cline=console_line;
	
	for (i=0;i!=console_count;i++) {
		y+=y_add;
		gl_text_draw(5,y,cline->txt,tx_left,FALSE,&cline->color,1.0f);
		cline++;
	}

	gl_text_end();
}

void console_draw(void)
{
	switch (console_mode) {
	
		case console_mode_close:
			return;
			
		case console_mode_fps:
			console_draw_fps();
			return;
			
		case console_mode_open:
			console_draw_open();
			return;
			
	}
}
	
