/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Opaque Segment Rendering

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern bitmap_type		lmap_black_bitmap,lmap_white_bitmap;

/* =======================================================

      Mesh-Poly Culling
      
======================================================= */

bool render_check_poly_cull(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
		// check for never cull flags

	if (mesh->flag.never_cull) return(FALSE);
	if (poly->flag.never_cull) return(FALSE);
	if (map.optimize.never_cull) return(FALSE);

		// skip polys with away facing normals
		// do dot product between normal and vector
		// from poly mid-eye point

	return(((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>map.optimize.cull_angle);
}

/* =======================================================

      Opaque Map Rendering Types
      
======================================================= */

void render_opaque_mesh_fixed(void)
{
	int						n,k,frame;
	float					glow_color;
	GLuint					gl_id,lmap_gl_id,glow_gl_id;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

		// enable arrays

	glEnableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE3);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
		// run through the draw list

	gl_texture_map_fixed_start();

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		if (!mesh->draw.has_opaque) continue;
		
			// the mesh vbo
			
		view_bind_mesh_liquid_vertex_object(&mesh->vbo);
		view_bind_mesh_liquid_index_object(&mesh->vbo);
		
		glVertexPointer(3,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)0);

		glClientActiveTexture(GL_TEXTURE3);
		glTexCoordPointer(2,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)(3*sizeof(float)));

		glClientActiveTexture(GL_TEXTURE2);
		glTexCoordPointer(2,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)(3*sizeof(float)));
		
		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)(5*sizeof(float)));

		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)(3*sizeof(float)));

		glColorPointer(4,GL_UNSIGNED_BYTE,mesh->vbo.vertex_stride,(GLvoid*)(7*sizeof(float)));

			// draw the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent polys

			if (poly->draw.transparent_on) {
				poly++;
				continue;
			}
		
				// skip culling

			if (render_check_poly_cull(mesh,poly)) {
				poly++;
				continue;
			}

				// get textures
				
			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if ((mesh->flag.hilite) || (setup.debug_on)) {
				lmap_gl_id=lmap_white_bitmap.gl_id;
			}
			else {
				if (poly->lmap_txt_idx==-1) {
					lmap_gl_id=lmap_black_bitmap.gl_id;
				}
				else {
					lmap_gl_id=map.textures[poly->lmap_txt_idx].frames[0].bitmap.gl_id;
				}
			}

			if (poly->draw.glow_on) {
				glow_gl_id=texture->frames[frame].glowmap.gl_id;
				glow_color=texture->glow.current_color;
			}
			else {
				glow_gl_id=lmap_black_bitmap.gl_id;
				glow_color=0.0f;
			}

				// substitute any back rendering

			if (!gl_back_render_get_texture(poly->camera,&gl_id,NULL)) gl_id=texture->frames[frame].bitmap.gl_id;

				// draw polygon

			gl_texture_map_fixed_set(gl_id,lmap_gl_id,glow_gl_id,glow_color,1.0f);
			glDrawElements(GL_TRIANGLE_FAN,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			
			poly++;
			view.count.mesh_poly++;
		}
		
		view_unbind_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_index_object();
	}

		// clear fixed state

	gl_texture_map_fixed_end();

	glDisableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE3);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void render_opaque_mesh_shader(void)
{
	int							n,k,mesh_idx,frame;
	float						alpha;
	bool						lighting_small;
	GLuint						gl_id;
	texture_type				*texture;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	view_glsl_light_list_type	light_list;

		// enable arrays
		
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
		// start shader

	gl_shader_draw_start();
	
		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh_idx=view.render->draw_list.items[n].idx;

		mesh=&map.mesh.meshes[mesh_idx];
		if (!mesh->draw.has_opaque) continue;
		
			// the mesh vbo
			
		view_bind_mesh_liquid_vertex_object(&mesh->vbo);
		view_bind_mesh_liquid_index_object(&mesh->vbo);

		glVertexPointer(3,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)0);

		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)(5*sizeof(float)));
	
		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2,GL_FLOAT,mesh->vbo.vertex_stride,(GLvoid*)(3*sizeof(float)));

		gl_shader_draw_reset_normal_tangent_attrib();

			// small meshes don't create light lists
			// per-poly, instead just use the mesh list
			// if the mesh list always has less than
			// max shader lights, just use that

		lighting_small=((mesh->precalc_flag.lighting_small) || (mesh->light_cache.count<=max_shader_light));
		if (lighting_small) gl_lights_build_mesh_glsl_light_list(mesh,&light_list);

			// draw the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent polys

			if (poly->draw.transparent_on) {
				poly++;
				continue;
			}
		
				// skip culling

			if (render_check_poly_cull(mesh,poly)) {
				poly++;
				continue;
			}

				// setup shader

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if (!lighting_small) gl_lights_build_poly_glsl_light_list(mesh,poly,&light_list);
			gl_shader_draw_execute(core_shader_group_map,texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,&light_list,(7*sizeof(float)),(10*sizeof(float)),mesh->vbo.vertex_stride);

				// fix texture if any back rendering

			if (gl_back_render_get_texture(poly->camera,&gl_id,&alpha)) {
				gl_shader_texture_override(gl_id,alpha);
			}

				// draw polygon
				
			glDrawElements(GL_TRIANGLE_FAN,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			
			poly++;
			view.count.mesh_poly++;
		}
		
		view_unbind_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_index_object();
	}
		
	gl_shader_draw_end();

		// disable arrays

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/* =======================================================

      Opaque Map Rendering
      
======================================================= */

void render_map_mesh_opaque(void)
{
		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// common setup
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glDisable(GL_BLEND);

		// draw the polygons

	if (view_shader_on()) {
		render_opaque_mesh_shader();
	}
	else {
		render_opaque_mesh_fixed();
	}
}

