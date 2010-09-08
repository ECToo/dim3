/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Vertex Objects

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#include "video.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							cur_vbo_cache_idx,cur_vbo_cache_index_idx;
GLuint						vbo_map,vbo_map_index,
							vbo_liquid,vbo_liquid_index,
							vbo_sky,
							vbo_cache[view_vertex_object_count],
							vbo_cache_index[view_vertex_object_count];

/* =======================================================

      Create and Dispose Vertex Objects
      
======================================================= */

void view_create_vertex_objects(void)
{
		// map, liquid and sky vbo

	glGenBuffersARB(1,&vbo_map);
	glGenBuffersARB(1,&vbo_map_index);

	glGenBuffersARB(1,&vbo_liquid);
	glGenBuffersARB(1,&vbo_liquid_index);

	glGenBuffersARB(1,&vbo_sky);

		// misc vbos

	glGenBuffersARB(view_vertex_object_count,vbo_cache);
	glGenBuffersARB(view_vertex_object_count,vbo_cache_index);

		// start at first misc vbo

	cur_vbo_cache_idx=0;
	cur_vbo_cache_index_idx=0;
}

void view_dispose_vertex_objects(void)
{
	glDeleteBuffersARB(1,&vbo_map);
	glDeleteBuffersARB(1,&vbo_map_index);

	glDeleteBuffersARB(1,&vbo_liquid);
	glDeleteBuffersARB(1,&vbo_liquid_index);

	glDeleteBuffersARB(1,&vbo_sky);

	glDeleteBuffersARB(view_vertex_object_count,vbo_cache);
	glDeleteBuffersARB(view_vertex_object_count,vbo_cache_index);
}

/* =======================================================

      Map VBOs
      
======================================================= */

void view_init_map_vertex_object(int sz)
{
		// create map geometery buffer

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_map);

	sz*=sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_DYNAMIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

inline float* view_bind_map_map_vertex_object(void)
{
	float		*vertex_ptr;

		// bind to map specific VBO

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_map);

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(vertex_ptr);
}

inline void view_bind_map_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_map);
}

inline void view_unmap_map_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_map_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

void view_init_map_index_object(int sz)
{
		// create map index buffer

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_map_index);

	sz*=sizeof(unsigned int);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,sz,NULL,GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

inline unsigned int* view_bind_map_map_index_object(void)
{
	unsigned int		*index_ptr;

		// bind to map specific VBO

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_map_index);

		// map pointer

	index_ptr=(unsigned int*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (index_ptr==NULL) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(index_ptr);
}

inline void view_bind_map_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_map_index);
}

inline void view_unmap_map_index_object(void)
{
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
}

inline void view_unbind_map_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Liquid VBOs
      
======================================================= */

inline float* view_bind_map_liquid_vertex_object(int sz)
{
	float		*vertex_ptr;

		// bind to liquid specific VBO

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_liquid);

		// resize VBO

	sz*=sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(vertex_ptr);
}

inline void view_unmap_liquid_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_liquid_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

inline unsigned int* view_bind_map_liquid_index_object(int sz)
{
	unsigned int		*index_ptr;

		// bind to liquid specific VBO

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_liquid_index);

		// resize VBO

	sz*=sizeof(unsigned int);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);

		// map pointer

	index_ptr=(unsigned int*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (index_ptr==NULL) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(index_ptr);
}

inline void view_unmap_liquid_index_object(void)
{
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
}

inline void view_unbind_liquid_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Sky VBOs
      
======================================================= */

inline float* view_bind_map_sky_vertex_object(int sz)
{
	float		*vertex_ptr;

		// bind to sky specific VBO

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_sky);

		// resize VBO

	sz*=sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STATIC_DRAW_ARB);

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(vertex_ptr);
}

inline void view_bind_sky_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_sky);
}

inline void view_unmap_sky_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_sky_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Model, Particle, Etc VBOs
      
======================================================= */

float* view_bind_map_next_vertex_object(int sz)
{
	float			*vertex_ptr;

		// bind it

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_cache[cur_vbo_cache_idx]);

		// supergumba -- this checks to see if the buffer is in
		// use and stalls.  The OpenGL drivers should be smart
		// enough to not require this but it's here just in case

	// vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	// glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

		// change size of buffer

	sz*=sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

		// get next object

	cur_vbo_cache_idx++;
	if (cur_vbo_cache_idx==view_vertex_object_count) cur_vbo_cache_idx=0;

	return(vertex_ptr);
}

inline void view_unmap_current_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_current_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

unsigned short* view_bind_map_next_index_object(int sz)
{
	unsigned short		*index_ptr;

		// bind it

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_cache_index[cur_vbo_cache_index_idx]);

		// supergumba -- this checks to see if the buffer is in
		// use and stalls.  The OpenGL drivers should be smart
		// enough to not require this but it's here just in case

	// index_ptr=(unsigned short*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	// glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);

		// change size of buffer

	sz*=sizeof(unsigned short);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);

		// map pointer

	index_ptr=(unsigned short*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (index_ptr==NULL) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

		// get next object

	cur_vbo_cache_index_idx++;
	if (cur_vbo_cache_index_idx==view_vertex_object_count) cur_vbo_cache_index_idx=0;

	return(index_ptr);
}

inline void view_unmap_current_index_object(void)
{
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
}

inline void view_unbind_current_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Screen Tint
      
======================================================= */

void view_draw_next_vertex_object_2D_tint_screen(void)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*2);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=0.0f;
	*vertex_ptr++=0.0f;

	*vertex_ptr++=(float)setup.screen.x_sz;
	*vertex_ptr++=0.0f;

	*vertex_ptr++=(float)setup.screen.x_sz;
	*vertex_ptr++=(float)setup.screen.y_sz;

	*vertex_ptr++=0.0f;
	*vertex_ptr++=(float)setup.screen.y_sz;

  	view_unmap_current_vertex_object();

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

/* =======================================================

      2D Fills
      
======================================================= */

void view_draw_next_vertex_object_2D_color_poly(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha)
{
	float			*vertex_ptr,*col_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+4));
	if (vertex_ptr==NULL) return;

	col_ptr=vertex_ptr+(4*2);

		// get the vertexes

	*vertex_ptr++=(float)x0;
	*vertex_ptr++=(float)y0;

	*col_ptr++=col0->r;
	*col_ptr++=col0->g;
	*col_ptr++=col0->b;
	*col_ptr++=alpha;

	*vertex_ptr++=(float)x1;
	*vertex_ptr++=(float)y1;

	*col_ptr++=col1->r;
	*col_ptr++=col1->g;
	*col_ptr++=col1->b;
	*col_ptr++=alpha;

	*vertex_ptr++=(float)x2;
	*vertex_ptr++=(float)y2;

	*col_ptr++=col2->r;
	*col_ptr++=col2->g;
	*col_ptr++=col2->b;
	*col_ptr++=alpha;

	*vertex_ptr++=(float)x3;
	*vertex_ptr++=(float)y3;

	*col_ptr++=col3->r;
	*col_ptr++=col3->g;
	*col_ptr++=col3->b;
	*col_ptr++=alpha;

  	view_unmap_current_vertex_object();

		// setup draw
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// draw the polygon

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

 	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
		
		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_color_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_draw_next_vertex_object_2D_color_poly(lft,top,col,rgt,top,col,rgt,bot,col,lft,bot,col,alpha);
}

void view_draw_next_vertex_object_2D_color_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*2);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	switch (dir) {

		case 0:
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)bot;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)bot;
			*vertex_ptr++=((float)(lft+rgt))*0.5f;
			*vertex_ptr++=(float)top;
			break;

		case 1:
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=((float)(top+bot))*0.5f;
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)bot;
			break;

		case 2:
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=((float)(lft+rgt))*0.5f;
			*vertex_ptr++=(float)bot;
			break;

		case 3:
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=((float)(top+bot))*0.5f;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)bot;
			break;
	}

  	view_unmap_current_vertex_object();

		// setup draw
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the trig
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_POLYGON,0,3);

	glDisableClientState(GL_VERTEX_ARRAY);
	
		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

/* =======================================================

      2D Lines
      
======================================================= */

void view_draw_next_vertex_object_2D_line(d3col *col,float alpha,int x0,int y0,int x1,int y1)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(2*2);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=(float)x0;
	*vertex_ptr++=(float)y0;

	*vertex_ptr++=(float)x1;
	*vertex_ptr++=(float)y1;

  	view_unmap_current_vertex_object();

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_LINES,0,2);

	glDisableClientState(GL_VERTEX_ARRAY);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_line_poly(d3col *col,float alpha,int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*2);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=(float)x0;
	*vertex_ptr++=(float)y0;

	*vertex_ptr++=(float)x1;
	*vertex_ptr++=(float)y1;

	*vertex_ptr++=(float)x2;
	*vertex_ptr++=(float)y2;

	*vertex_ptr++=(float)x3;
	*vertex_ptr++=(float)y3;

  	view_unmap_current_vertex_object();

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_LINE_LOOP,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_line_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_draw_next_vertex_object_2D_line_poly(col,alpha,lft,top,rgt,top,rgt,bot,lft,bot);
}

void view_draw_next_vertex_object_2D_line_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*2);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	switch (dir) {

		case 0:
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)bot;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)bot;
			*vertex_ptr++=((float)(lft+rgt))*0.5f;
			*vertex_ptr++=(float)top;
			break;

		case 1:
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=((float)(top+bot))*0.5f;
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)bot;
			break;

		case 2:
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=((float)(lft+rgt))*0.5f;
			*vertex_ptr++=(float)bot;
			break;

		case 3:
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)top;
			*vertex_ptr++=(float)lft;
			*vertex_ptr++=((float)(top+bot))*0.5f;
			*vertex_ptr++=(float)rgt;
			*vertex_ptr++=(float)bot;
			break;
	}

  	view_unmap_current_vertex_object();

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_LINE_LOOP,0,3);

	glDisableClientState(GL_VERTEX_ARRAY);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

/* =======================================================

      3D Lines
      
======================================================= */

void view_draw_next_vertex_object_3D_line(d3col *col,float alpha,int x0,int y0,int z0,int x1,int y1,int z1)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(2*3);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=(float)x0;
	*vertex_ptr++=(float)y0;
	*vertex_ptr++=(float)z0;

	*vertex_ptr++=(float)x1;
	*vertex_ptr++=(float)y1;
	*vertex_ptr++=(float)z1;

  	view_unmap_current_vertex_object();

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_LINES,0,2);

	glDisableClientState(GL_VERTEX_ARRAY);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_3D_line_cube(d3col *col,float alpha,int *px,int *py,int *pz)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(24*3);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=(float)px[0];
	*vertex_ptr++=(float)py[0];
	*vertex_ptr++=(float)pz[0];

	*vertex_ptr++=(float)px[1];
	*vertex_ptr++=(float)py[1];
	*vertex_ptr++=(float)pz[1];

	*vertex_ptr++=(float)px[1];
	*vertex_ptr++=(float)py[1];
	*vertex_ptr++=(float)pz[1];

	*vertex_ptr++=(float)px[2];
	*vertex_ptr++=(float)py[2];
	*vertex_ptr++=(float)pz[2];

	*vertex_ptr++=(float)px[2];
	*vertex_ptr++=(float)py[2];
	*vertex_ptr++=(float)pz[2];

	*vertex_ptr++=(float)px[3];
	*vertex_ptr++=(float)py[3];
	*vertex_ptr++=(float)pz[3];

	*vertex_ptr++=(float)px[3];
	*vertex_ptr++=(float)py[3];
	*vertex_ptr++=(float)pz[3];

	*vertex_ptr++=(float)px[0];
	*vertex_ptr++=(float)py[0];
	*vertex_ptr++=(float)pz[0];

	*vertex_ptr++=(float)px[4];
	*vertex_ptr++=(float)py[4];
	*vertex_ptr++=(float)pz[4];

	*vertex_ptr++=(float)px[5];
	*vertex_ptr++=(float)py[5];
	*vertex_ptr++=(float)pz[5];

	*vertex_ptr++=(float)px[5];
	*vertex_ptr++=(float)py[5];
	*vertex_ptr++=(float)pz[5];

	*vertex_ptr++=(float)px[6];
	*vertex_ptr++=(float)py[6];
	*vertex_ptr++=(float)pz[6];

	*vertex_ptr++=(float)px[6];
	*vertex_ptr++=(float)py[6];
	*vertex_ptr++=(float)pz[6];

	*vertex_ptr++=(float)px[7];
	*vertex_ptr++=(float)py[7];
	*vertex_ptr++=(float)pz[7];

	*vertex_ptr++=(float)px[7];
	*vertex_ptr++=(float)py[7];
	*vertex_ptr++=(float)pz[7];

	*vertex_ptr++=(float)px[4];
	*vertex_ptr++=(float)py[4];
	*vertex_ptr++=(float)pz[4];

	*vertex_ptr++=(float)px[0];
	*vertex_ptr++=(float)py[0];
	*vertex_ptr++=(float)pz[0];

	*vertex_ptr++=(float)px[4];
	*vertex_ptr++=(float)py[4];
	*vertex_ptr++=(float)pz[4];

	*vertex_ptr++=(float)px[1];
	*vertex_ptr++=(float)py[1];
	*vertex_ptr++=(float)pz[1];

	*vertex_ptr++=(float)px[5];
	*vertex_ptr++=(float)py[5];
	*vertex_ptr++=(float)pz[5];

	*vertex_ptr++=(float)px[2];
	*vertex_ptr++=(float)py[2];
	*vertex_ptr++=(float)pz[2];

	*vertex_ptr++=(float)px[6];
	*vertex_ptr++=(float)py[6];
	*vertex_ptr++=(float)pz[6];

	*vertex_ptr++=(float)px[3];
	*vertex_ptr++=(float)py[3];
	*vertex_ptr++=(float)pz[3];

	*vertex_ptr++=(float)px[7];
	*vertex_ptr++=(float)py[7];
	*vertex_ptr++=(float)pz[7];

  	view_unmap_current_vertex_object();

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_LINES,0,24);

	glDisableClientState(GL_VERTEX_ARRAY);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

/* =======================================================

      Textured Fills
      
======================================================= */

void view_draw_next_vertex_object_2D_texture_quad(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float gx,float gx2,float gy,float gy2)
{
	float			*vertex_ptr,*uv_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(4*2);

		// get the vertexes

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)top;

	*uv_ptr++=gx;
	*uv_ptr++=gy;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)top;

	*uv_ptr++=gx2;
	*uv_ptr++=gy;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=gx2;
	*uv_ptr++=gy2;

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=gx;
	*uv_ptr++=gy2;

  	view_unmap_current_vertex_object();

		// setup texture draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	if (col==NULL) {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}
	else {
		glColor4f(col->r,col->g,col->b,alpha);
	}
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	gl_texture_bind(0,gl_id);

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// finish texture draw
	
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_texture_quad_rectangle(GLuint gl_id,float alpha,int lft,int rgt,int top,int bot,int pixel_wid,int pixel_high)
{
	float				*vertex_ptr,*uv_ptr;
	
		// create the vertexes and uv

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(4*2);

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)top;

	*uv_ptr++=0.0f;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)top;

	*uv_ptr++=(float)pixel_wid;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=(float)pixel_wid;
	*uv_ptr++=(float)pixel_high;

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=0.0f;
	*uv_ptr++=(float)pixel_high;

  	view_unmap_current_vertex_object();

		// setup texture draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(1.0f,0.0f,1.0f,alpha);
	
	gl_texture_clear(0);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,gl_id);

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// finish texture draw
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// unbind the vbo

	view_unbind_current_vertex_object();
}
