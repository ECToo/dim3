/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Model Routines

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
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern map_type					map;

extern file_path_setup_type		file_path_setup;

int								nmodel;
model_type						*models;

/* =======================================================

      Start and Close Models
	        
======================================================= */

void walk_view_models_start(void)
{
	nmodel=0;
	
	models=malloc(sizeof(model_type)*max_model);
	
	model_setup(&file_path_setup,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE);
}

void walk_view_models_close(void)
{
	int					n;
	model_type			*model;
	
	if (models!=NULL) {
	
		model=models;
	
		for (n=0;n!=nmodel;n++) {
			model_close(model);
			model++;
		}
		
		free(models);
	}
	
	nmodel=0;
}

/* =======================================================

      Reload Models Set
	        
======================================================= */

int walk_view_models_find_model(char *name)
{
	int			n;
	model_type	*model;
	
	model=models;
	
	for (n=0;n!=nmodel;n++) {
		if (strcmp(model->name,name)==0) return(n);
		model++;
	}
	
	return(-1);
}

void walk_view_models_reset(void)
{
	int					n,idx;
	bool				used[max_model];
	spot_type			*spot;
	map_scenery_type	*scenery;
	
	if (models==NULL) return;
	
		// find models loaded but not in spots/scenery
	
	for (n=0;n!=nmodel;n++) {
		used[n]=FALSE;
	}
	
	spot=map.spots;
	
	for (n=0;n!=map.nspot;n++) {
		if (spot->display_model[0]!=0x0) {
			idx=walk_view_models_find_model(spot->display_model);
			if (idx!=-1) used[idx]=TRUE;
		}
		spot++;
	}
	
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
		if (scenery->model_name[0]!=0x0) {
			idx=walk_view_models_find_model(scenery->model_name);
			if (idx!=-1) used[idx]=TRUE;
		}
		scenery++;
	}
	
		// remove models loaded but not in spots/scenery
		
	n=0;
	
	while (n<nmodel) {
		if (used[n]) {
			n++;
			continue;
		}
		
		model_close(&models[n]);
	
		if (n<(nmodel-1)) {
			memmove(&models[n],&models[n+1],(sizeof(model_type)*((nmodel-n)-1)));
			memmove(&used[n],&used[n+1],(sizeof(bool)*((nmodel-n)-1)));
		}
		
		nmodel--;
		if (nmodel==0) break;
	}
	
		// add in models in spots/scenery but not loaded
		
	spot=map.spots;
	
	for (n=0;n!=map.nspot;n++) {
	
		if (spot->display_model[0]!=0x0) {
		
			idx=walk_view_models_find_model(spot->display_model);
			
			if ((idx==-1) && (nmodel<max_model)) {
				if (model_open(&models[nmodel],spot->display_model,TRUE)) {
					nmodel++;
				}
			}
		}
		
		spot++;
	}
	
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
	
		if (scenery->model_name[0]!=0x0) {
		
			idx=walk_view_models_find_model(scenery->model_name);
			
			if ((idx==-1) && (nmodel<max_model)) {
				if (model_open(&models[nmodel],scenery->model_name,TRUE)) {
					nmodel++;
				}
			}
		}
		
		scenery++;
	}
}

/* =======================================================

      Model Y size
      
======================================================= */

void walk_view_get_model_size(char *model_name,d3pnt *size)
{
	int					idx;

	size->x=size->y=size->z=view_sprite_size;

	if (model_name[0]==0x0) return;
	
	idx=walk_view_models_find_model(model_name);
	if (idx==-1) return;
	
	memmove(size,&models[idx].view_box.size,sizeof(d3pnt));
}

/* =======================================================

      Draw Model
      
======================================================= */

void walk_view_model_draw_material(model_type *model,texture_type *texture,model_material_type *material,int frame)
{
	int					k,trig_count,bitmap_gl_id;
    model_trig_type		*trig;

	trig_count=material->trig_count;
	if (trig_count==0) return;

		// setup texture

	bitmap_gl_id=texture->frames[frame].bitmap.gl_id;
	glBindTexture(GL_TEXTURE_2D,bitmap_gl_id);
	
		// triangles
		
	glBegin(GL_TRIANGLES);
	
	trig=&model->meshes[0].trigs[material->trig_start];

	for (k=0;k!=trig_count;k++) {
		glTexCoord2f(trig->gx[0],trig->gy[0]);
		glMultiTexCoord2f(GL_TEXTURE1,trig->gx[0],trig->gy[0]);
		glArrayElement(trig->v[0]);
		glTexCoord2f(trig->gx[1],trig->gy[1]);
		glMultiTexCoord2f(GL_TEXTURE1,trig->gx[1],trig->gy[1]);
		glArrayElement(trig->v[1]);
		glTexCoord2f(trig->gx[2],trig->gy[2]);
		glMultiTexCoord2f(GL_TEXTURE1,trig->gx[2],trig->gy[2]);
		glArrayElement(trig->v[2]);
		trig++;
	}
	
	glEnd();
}

bool walk_view_model_draw(d3pnt *pnt,d3ang *ang,char *name,short *texture_frame,int frame_count)
{
	int								idx,n,frame;
	model_type						*model;
	model_draw_setup				draw_setup;
	model_mesh_type					*mesh;
    texture_type					*texture;
	model_material_type				*material;

	if (name[0]==0x0) return(FALSE);
	
	idx=walk_view_models_find_model(name);
	if (idx==-1) return(FALSE);
	
	model=&models[idx];

		// build model vertex list
		
	model_draw_setup_initialize(model,&draw_setup,FALSE);
	model_draw_setup_clear(model,&draw_setup);
	
    draw_setup.poses[0].idx_1=draw_setup.poses[0].idx_2=-1;
    draw_setup.poses[0].factor=0;
	draw_setup.poses[0].acceleration=0;
	
	memmove(&draw_setup.ang,ang,sizeof(d3ang));
	draw_setup.ang.y=ang->y;
	
	model_create_draw_bones(model,&draw_setup);
	model_create_draw_vertexes(model,0,&draw_setup);

	model_translate_draw_vertex(model,0,pnt->x,pnt->y,pnt->z,&draw_setup);

		// draw triangles

	mesh=&model->meshes[0];
	
	glEnable(GL_TEXTURE_2D);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,draw_setup.mesh_arrays[0].gl_vertex_array);
	glLockArraysEXT(0,mesh->nvertex);	
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);

	glColor4f(1,1,1,1);
    
        // run through the opaque textures
	
	glDisable(GL_BLEND);
	
    texture=model->textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		frame=0;
		if (n<frame_count) frame=(int)texture_frame[n];
		if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) walk_view_model_draw_material(model,texture,material,frame);
		texture++;
		material++;
	}
	
		// run through the transparent textures

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	
	texture=model->textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		frame=0;
		if (n<frame_count) frame=(int)texture_frame[n];
		if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) walk_view_model_draw_material(model,texture,material,frame);
		texture++;
		material++;
	}
	
	glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
    
	glUnlockArraysEXT();
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
	
	model_draw_setup_shutdown(model,&draw_setup);
	
	return(TRUE);
}

/* =======================================================

      Get Model Click Size
      
======================================================= */

void walk_view_model_cube_vertexes(char *name,d3pnt *pnt,d3ang *ang,d3pnt *v_pnts)
{
	int						n,idx,cx,cy,cz,wid_x,wid_z,high,
							px[8],py[8],pz[8];
	float					fx,fy,fz;
	matrix_type				mat;
	model_type				*model;
	
		// default size
		
    wid_x=wid_z=view_sprite_size;
	high=view_sprite_size*2;
	
	cx=cy=cz=0;

		// if there is a model, then get size

	if (name!=NULL) {
		if (name[0]!=0x0) {
	
			idx=walk_view_models_find_model(name);
			if (idx!=-1) {
	
				model=&models[idx];
		
				wid_x=model->view_box.size.x>>1;
				wid_z=model->view_box.size.z>>1;
				high=model->view_box.size.y;
				
				cx=model->center.x;
				cy=model->center.y;
				cz=model->center.z;
			}
		}
	}

		// get polygons
	
	px[0]=px[3]=px[6]=px[7]=-wid_x;
	px[1]=px[2]=px[4]=px[5]=wid_x;
	
	py[0]=py[1]=py[2]=py[3]=-high;
	py[4]=py[5]=py[6]=py[7]=0;
		
	pz[0]=pz[1]=pz[4]=pz[7]=-wid_z;
	pz[2]=pz[3]=pz[5]=pz[6]=wid_z;
	
		// any rotations
	
	if (ang!=NULL) {
		matrix_rotate_xyz(&mat,ang->x,ang->y,ang->z);
		
		for (n=0;n!=8;n++) {
			fx=(float)(px[n]-cx);
			fy=(float)(py[n]-cy);
			fz=(float)(pz[n]-cz);
			
			matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			
			px[n]=((int)fx)+cx;
			py[n]=((int)fy)+cy;
			pz[n]=((int)fz)+cz;
		}
	}
	
	for (n=0;n!=8;n++) {
		v_pnts[n].x=px[n]+pnt->x;
		v_pnts[n].y=py[n]+pnt->y;
		v_pnts[n].z=pz[n]+pnt->z;
	}
}

