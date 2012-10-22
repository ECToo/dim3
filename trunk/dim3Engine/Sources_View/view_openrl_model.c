/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Models

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
#include "objects.h"

#ifdef D3_OPENRL

#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rl_scene_id;

extern int view_openrl_create_material_from_texture(char *sub_path,texture_type *texture,texture_frame_type *frame);

/* =======================================================

      OpenRL Single Model Mesh
      
======================================================= */

void view_openrl_model_setup_single_model(model_draw *draw,bool hidden,bool no_ray_trace_block,bool no_light_block)
{
	int					n,k,mesh_id,uv_count;
	unsigned long		flags;
	float				*uv;
	short				*vk,*ray_polys;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
	draw->openrl_mesh_id=-1;

		// get the model
		
	if ((draw->model_idx==-1) || (!draw->on)) return;

	mdl=server.model_list.models[draw->model_idx];
	mesh=&mdl->meshes[0];

		// get mesh flags

	flags=0;
	if (no_ray_trace_block) flags|=RL_MESH_FLAG_NON_RAY_TRACE_BLOCKING;
	if (no_light_block) flags|=RL_MESH_FLAG_NON_LIGHT_BLOCKING;
		
		// add the mesh

	mesh_id=rlSceneMeshAdd(view_rl_scene_id,flags);
	if (mesh_id<0) return;
	
	if (hidden) rlSceneMeshSetHidden(view_rl_scene_id,mesh_id,TRUE);
	
		// we set the UVs and polys at the beginning
		// and only change the vertexes and normals
		// while rendering

		// the UVs

	uv_count=0;
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		uv_count+=poly->ptsz;
		poly++;
	}

	rlSceneMeshSetUV(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,uv_count,NULL);
	rlSceneMeshMapUVPointer(view_rl_scene_id,mesh_id,(void**)&uv);

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		for (k=0;k!=poly->ptsz;k++) {
			*uv++=poly->gx[k];
			*uv++=poly->gy[k];
		}
		poly++;
	}

	rlSceneMeshUnMapUVPointer(view_rl_scene_id,mesh_id);

		// polygons

	ray_polys=(short*)malloc((mesh->npoly*(2+(4*8)))*sizeof(short));		// supergumba -- this will work but chews up a lot of memory
	vk=ray_polys;

	uv_count=0;
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		*vk++=poly->ptsz;
		*vk++=(short)mdl->textures[poly->txt_idx].frames[0].bitmap.rl_material_id;

		for (k=0;k!=poly->ptsz;k++) {
			*vk++=(short)poly->v[k];	// vertex
			*vk++=uv_count;				// uv, each vertex has unique uv count
			*vk++=(short)poly->v[k];	// normal are parallel to vertexes
			*vk++=(short)poly->v[k];	// tangents are parallel to vertexes
			uv_count++;
		}

		poly++;
	}

	rlSceneMeshSetPoly(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT,mesh->npoly,ray_polys);
	free(ray_polys);

		// set the draw's mesh id
		
	draw->openrl_mesh_id=mesh_id;
}

/* =======================================================

      OpenRL Map Models
      
======================================================= */

void view_openrl_map_model_mesh_start(void)
{
	int					n,k;
	char				sub_path[1024];
	bool				no_ray_trace_block;
	obj_type			*obj;
	weapon_type			*weap;
	model_type			*mdl;
	texture_type		*texture;
	texture_frame_type	*frame;
	
		// model materials

	for (k=0;k!=max_model_list;k++) {

		mdl=server.model_list.models[k];
		if (mdl==NULL) continue;

		sprintf(sub_path,"Models/%s/Textures",mdl->name);

		for (n=0;n!=max_model_texture;n++) {
			texture=&mdl->textures[n];
			
			frame=&texture->frames[0];
			if (frame->name[0]==0x0) continue;
			
			frame->bitmap.rl_material_id=view_openrl_create_material_from_texture(sub_path,texture,frame);
		}
	}
		
		// object models
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		no_ray_trace_block=((map.camera.mode==cv_fpp) && (obj->idx==camera.obj_idx));
		view_openrl_model_setup_single_model(&obj->draw,TRUE,no_ray_trace_block,FALSE);
	}

		// player weapon models

	obj=server.obj_list.objs[server.player_obj_idx];

	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap!=NULL) view_openrl_model_setup_single_model(&weap->draw,TRUE,FALSE,TRUE);
	}
}

void view_openrl_map_model_mesh_stop(void)
{
	int					n,k;
	obj_type			*obj;
	weapon_type			*weap;
	model_type			*mdl;
	texture_type		*texture;
	texture_frame_type	*frame;
	
		// delete object models
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->draw.openrl_mesh_id!=-1) rlSceneMeshDelete(view_rl_scene_id,obj->draw.openrl_mesh_id);
	}

		// delete player weapon models

	obj=server.obj_list.objs[server.player_obj_idx];

	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;
		
		if (weap->draw.openrl_mesh_id!=-1) rlSceneMeshDelete(view_rl_scene_id,weap->draw.openrl_mesh_id);
	}

		// delete materials
		// we do this last as we can't
		// delete attached materials

	for (k=0;k!=max_model_list;k++) {

		mdl=server.model_list.models[k];
		if (mdl==NULL) continue;

		for (n=0;n!=max_model_texture;n++) {
			texture=&mdl->textures[n];
			
			frame=&texture->frames[0];
			if (frame->name[0]==0x0) continue;

			if (frame->bitmap.rl_material_id!=-1) rlMaterialDelete(frame->bitmap.rl_material_id);
		}
	}	
}

void view_openrl_map_model_update(void)
{
	int					n;
	obj_type			*obj;
	weapon_type			*weap;
	model_draw			*draw;
	model_type			*mdl;
	model_mesh_type		*mesh;

	for (n=0;n!=max_obj_list;n++) {
	
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		draw=&obj->draw;
		if (draw->openrl_mesh_id==-1) continue;
		
		if (obj->hidden) {
			rlSceneMeshSetHidden(view_rl_scene_id,draw->openrl_mesh_id,TRUE);
			continue;
		}
		
			// get the openrl mesh
			// and model
			
		rlSceneMeshSetHidden(view_rl_scene_id,draw->openrl_mesh_id,FALSE);
	
		mdl=server.model_list.models[draw->model_idx];
		mesh=&mdl->meshes[0];
			
			// the UVs and Polys are
			// already set, we just need to
			// update the vertexes, normals, and tangents

		rlSceneMeshSetVertex(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_vertex_array);
		rlSceneMeshSetNormal(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_normal_array);
		rlSceneMeshSetTangent(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_TANGENT_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_tangent_array);
	}

		// player weapon models

	obj=server.obj_list.objs[server.player_obj_idx];

	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;

		draw=&weap->draw;

			// not current weapons are hidden

		if (n!=obj->held_weapon.current_idx) {
			rlSceneMeshSetHidden(view_rl_scene_id,draw->openrl_mesh_id,TRUE);
			continue;
		}

			// current weapon

		rlSceneMeshSetHidden(view_rl_scene_id,draw->openrl_mesh_id,FALSE);

		mdl=server.model_list.models[draw->model_idx];
		mesh=&mdl->meshes[0];

		rlSceneMeshSetVertex(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_vertex_array);
		rlSceneMeshSetNormal(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_normal_array);
		rlSceneMeshSetTangent(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_TANGENT_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_tangent_array);
	}
}

/* =======================================================

      OpenRL Projectile Models
      
======================================================= */

void view_openrl_projectile_model_setup(proj_type *proj)
{
	view_openrl_model_setup_single_model(&proj->draw,FALSE,FALSE,FALSE);
}

void view_openrl_projectile_model_close(proj_type *proj)
{
	if (proj->draw.openrl_mesh_id!=-1) rlSceneMeshDelete(view_rl_scene_id,proj->draw.openrl_mesh_id);
}

void view_openrl_projectile_model_update(void)
{
	int					n;
	proj_type			*proj;
	model_draw			*draw;
	model_type			*mdl;
	model_mesh_type		*mesh;

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;

		draw=&proj->draw;
		if (draw->openrl_mesh_id==-1) continue;

			// projectile drawings share
			// vertex lists

		render_model_build_vertex_lists(&proj->draw,TRUE);

			// rebuild the mesh

		mdl=server.model_list.models[draw->model_idx];
		mesh=&mdl->meshes[0];

		rlSceneMeshSetVertex(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_vertex_array);
		rlSceneMeshSetNormal(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_normal_array);
		rlSceneMeshSetTangent(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_TANGENT_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_tangent_array);
	}
}

#endif