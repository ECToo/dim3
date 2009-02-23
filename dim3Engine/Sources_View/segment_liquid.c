/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Segment Liquid Tide and Breakup Routines

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
#include "video.h"
#include "lights.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

extern bool					dim3_debug;

extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);

/* =======================================================

      Initialize Liquid GL Lists
      
======================================================= */

inline int liquid_render_liquid_get_tide_split(map_liquid_type *liq)
{
	int				tide_split;

	tide_split=liq->rgt-liq->lft;
	if ((liq->bot-liq->top)>tide_split) tide_split=liq->bot-liq->top;
	
	if (liq->tide.flat) return(tide_split);
	
	return(tide_split/liq->tide.division);
}

inline int liquid_render_liquid_get_max_vertex(map_liquid_type *liq)
{
	int				tide_split,x_sz,z_sz;
	
	tide_split=liquid_render_liquid_get_tide_split(liq);

	x_sz=((liq->rgt-liq->lft)/tide_split)+4;		// possible extra edges on side because of griding
	z_sz=((liq->bot-liq->top)/tide_split)+4;

	return(x_sz*z_sz);
}

void liquid_gl_list_init(void)
{
	int					n,v_sz,sz;
	map_liquid_type		*liq;

	sz=0;

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {
		v_sz=liquid_render_liquid_get_max_vertex(liq);
		if (v_sz>sz) sz=v_sz;
		liq++;
	}

	view_init_liquid_vertex_object(v_sz*(3+2+3));
	view_init_liquid_index_object(v_sz*4);
}

/* =======================================================

      Setup Liquid Vertexes
      
======================================================= */

void liquid_render_liquid_create_vertex(int tick,map_liquid_type *liq,int v_sz)
{
	int				x,y,z,k,x_add,z_add,x_sz,z_sz,
					v_cnt,tide_split,tide_split_half,
					tide_high,tide_rate;
	float			fy,fgx,fgy,normal[3],f_intensity,x_txtoff,y_txtoff,
					f_break,f_time,f_tick,sn,
					f_tide_split_half,f_tide_high;
	bool			x_break,z_break;
	float			*vertex_ptr,*vl,*uv,*cl;
	
	y=liq->y;
	fy=(float)y;

		// setup vbo

	vertex_ptr=view_bind_map_liquid_vertex_object();
	if (vertex_ptr==NULL) return;
	
	vl=vertex_ptr;
	uv=vertex_ptr+(v_sz*3);
	cl=vertex_ptr+(v_sz*(3+2));

		// reduce lighting to liquid space

	map_calculate_light_reduce_liquid(liq);

		// setup tiding

	tide_split=liquid_render_liquid_get_tide_split(liq);
	
	tide_high=liq->tide.high;
	if (tide_high<1) tide_high=1;
	
	tide_rate=liq->tide.rate;
	if (tide_rate<1) tide_rate=1;

	tide_split_half=tide_split<<2;
	f_tide_split_half=(float)tide_split_half;
	
	f_tide_high=(float)tide_high;

	f_time=(float)(tick%tide_rate);		// get rate between 0..1
	f_time=f_time/(float)tide_rate;
	
		// liquid texture movement
		
	f_tick=(float)tick*0.001f;
	
	x_txtoff=f_tick*liq->x_shift;
	k=(int)x_txtoff;
	x_txtoff=liq->x_txtoff+(x_txtoff-(float)k);
	
	y_txtoff=f_tick*liq->y_shift;
	k=(int)y_txtoff;
	y_txtoff=liq->y_txtoff+(y_txtoff-(float)k);

		// create vertexes from tide splits

	fgx=(float)(liq->rgt-liq->lft);
	fgy=(float)(liq->bot-liq->top);
		
	z=liq->top;
	z_add=tide_split-(z%tide_split);
	z_break=FALSE;

	v_cnt=0;

	z_sz=0;
	
	while (TRUE) {

		x=liq->lft;
		x_add=tide_split-(x%tide_split);
		x_break=FALSE;

		x_sz=0;
		
		while (TRUE) {

				// color

			if (dim3_debug) {
				*cl=*(cl+1)=*(cl+2)=1.0f;
			}
			else {
				map_calculate_light_color_normal((double)x,(double)y,(double)z,cl,normal,&f_intensity);
			}
			
			cl+=3;

				// setup tide Y

			if (liq->tide.direction==liquid_direction_vertical) {
				f_break=(float)(z%tide_split_half);
			}
			else {
				f_break=(float)(x%tide_split_half);
			}
				
			f_break=f_break/f_tide_split_half;
		   
			sn=(float)sin((TRIG_PI*2.0f)*(f_break+f_time));

				// vertex and uvs

			*vl++=(float)x;
			*vl++=fy-(f_tide_high*sn);
			*vl++=(float)z;

			*uv++=x_txtoff+((liq->x_txtfact*(float)(x-liq->lft))/fgx);
			*uv++=y_txtoff+((liq->y_txtfact*(float)(z-liq->top))/fgy);

			v_cnt++;
			
			x_sz++;

			if (x_break) break;
			
			x+=x_add;
			x_add=tide_split;
			
			if (x>=liq->rgt) {
				x=liq->rgt;
				x_break=TRUE;
			}
		}

		z_sz++;
		
		if (z_break) break;
		
		z+=z_add;
		z_add=tide_split;
		
		if (z>=liq->bot) {
			z=liq->bot;
			z_break=TRUE;
		}
	}

	view_unmap_liquid_vertex_object();

		// setup split sizes

	liq->draw.v_cnt=v_cnt;

	liq->draw.x_sz=x_sz;
	liq->draw.z_sz=z_sz;
}

int liquid_render_liquid_create_quads(map_liquid_type *liq)
{
	int				x,z,x_sz,z_sz,quad_cnt,
					tz,bz,tz_add,top_row,bot_row,
					lx,rx,lx_add,tide_split;
	unsigned int	*index_ptr;

		// drawing the quads only draws to the edges

	x_sz=liq->draw.x_sz-1;
	if (x_sz<=0) return(0);

	z_sz=liq->draw.z_sz-1;
	if (z_sz<=0) return(0);

	tide_split=liquid_render_liquid_get_tide_split(liq);

		// setup index vbo

	index_ptr=view_bind_map_liquid_index_object();
	if (index_ptr==NULL) return(0);

		// create the draw indexes

	quad_cnt=0;
		
	tz=liq->top;
	tz_add=tide_split-(tz%tide_split);
	top_row=0;
	
	for (z=0;z!=z_sz;z++) {
		bz=tz+tz_add;
		tz_add=tide_split;
		if (bz>=liq->bot) bz=liq->bot;
		
		lx=liq->lft;
		lx_add=tide_split-(lx%tide_split);
		bot_row=top_row+(x_sz+1);
		
		for (x=0;x!=x_sz;x++) {
			rx=lx+lx_add;
			lx_add=tide_split;
			if (rx>=liq->rgt) rx=liq->rgt;
			
			*index_ptr++=(unsigned int)(top_row+x);
			*index_ptr++=(unsigned int)(top_row+(x+1));
			*index_ptr++=(unsigned int)(bot_row+(x+1));
			*index_ptr++=(unsigned int)(bot_row+x);

			quad_cnt++;

			lx=rx;
		}
	
		tz=bz;
		top_row=bot_row;
	}

	view_unmap_liquid_index_object();

	return(quad_cnt);
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render_liquid(int tick,map_liquid_type *liq)
{
	int						v_sz,quad_cnt,frame;
	float					col[3],normal[3],f_intensity;
	d3pnt					mid;
	texture_type			*texture;

		// liquid in view?

	if (!boundbox_inview(liq->lft,liq->top,liq->rgt,liq->bot,liq->y,liq->y)) return;

		// create vertexes

	v_sz=liquid_render_liquid_get_max_vertex(liq);

	liquid_render_liquid_create_vertex(tick,liq,v_sz);

		// create quads

	quad_cnt=liquid_render_liquid_create_quads(liq);
	if (quad_cnt==0) {
		view_unbind_liquid_vertex_object();
		return;
	}
	
		// setup drawing

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((v_sz*3)*sizeof(float)));
	
//	glEnableClientState(GL_COLOR_ARRAY);	// supergumba
//	glColorPointer(3,GL_FLOAT,0,(void*)((v_sz*(3+2))*sizeof(float)));

		// setup texture

	texture=&map.textures[liq->txt_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;
			
	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

		// start shader or regular texture

		// supergumba -- separate these so shader program start isn't called continually
/* supergumba
	if (texture->shader.on) {
		gl_shader_start();

		mid.x=(liq->lft+liq->rgt)>>1;
		mid.y=liq->y;
		mid.z=(liq->top+liq->bot)>>1;

		gl_shader_execute(texture,frame,&mid,0,1.0f);		// supergumba -- fix

		map_calculate_light_reduce_liquid(liq);
		map_calculate_light_color_normal((double)mid.x,(double)mid.y,(double)mid.z,col,normal,&f_intensity);

		glNormal3f(normal[0],normal[1],normal[2]);
	}
*/
//	else {
		gl_texture_transparent_start();
		gl_texture_transparent_set(texture->bitmaps[frame].gl_id,liq->alpha);
//	}

		// draw the quads

	glDrawElements(GL_QUADS,(quad_cnt*4),GL_UNSIGNED_INT,(GLvoid*)0);

		// end texture
/* supergumba
	if (texture->shader.on) {
		gl_shader_end();
	}
	*/
//	else {
		gl_texture_transparent_end();
	//}

		// end drawing

	glDisableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

		// unmap VBOs

	view_unbind_liquid_index_object();
	view_unbind_liquid_vertex_object();
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render(int tick)
{
	int					n;
	map_liquid_type		*liq;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.pnt,&view.camera.ang);
	gl_setup_project();
	
		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
		// draw liquids
		
		// supergumba -- need to sort, obscure, etc
		
	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {
		liquid_render_liquid(tick,liq);
		liq++;
	}
}

