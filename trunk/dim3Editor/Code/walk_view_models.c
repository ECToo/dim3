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
	
	models=valloc(sizeof(model_type)*max_model);
	
	model_setup(&file_path_setup,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);
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
				if (model_open(&models[nmodel],spot->display_model,TRUE,FALSE,FALSE)) nmodel++;
			}
		}
		
		spot++;
	}
	
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
	
		if (scenery->model_name[0]!=0x0) {
		
			idx=walk_view_models_find_model(scenery->model_name);
			
			if ((idx==-1) && (nmodel<max_model)) {
				if (model_open(&models[nmodel],scenery->model_name,TRUE,FALSE,FALSE)) nmodel++;
			}
		}
		
		scenery++;
	}
}

/* =======================================================

      Draw Model
      
======================================================= */

void walk_view_model_draw_material(model_type *model,model_draw_setup *draw_setup,texture_type *texture,model_material_type *material)
{
	register int					k,trig_count,bitmap_gl_id;
    register model_trig_type		*trig;

	trig_count=material->trig_count;
	if (trig_count==0) return;

		// setup texture

	bitmap_gl_id=texture->bitmaps[0].gl_id;
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

bool walk_view_model_draw(d3pnt *cpt,d3pnt *pnt,d3ang *ang,char *name)
{
	int								idx,x,y,z,n;
	model_type						*model;
	model_draw_setup				draw_setup;
	model_mesh_type					*mesh;
    register texture_type			*texture;
	register model_material_type	*material;
	
	if (name[0]==0x0) return(FALSE);
	
	idx=walk_view_models_find_model(name);
	if (idx==-1) return(FALSE);
	
	model=&models[idx];

		// translated drawing point
		
	x=pnt->x-cpt->x;
	y=pnt->y-cpt->y;
	z=pnt->z-cpt->z;
	
		// build model vertex list
		
	model_clear_draw_setup(model,&draw_setup);
	
    draw_setup.poses[0].idx_1=draw_setup.poses[0].idx_2=-1;
    draw_setup.poses[0].factor=0;
	draw_setup.poses[0].acceleration=0;
	memmove(&draw_setup.ang,ang,sizeof(d3ang));
	
	model_create_draw_bones(model,&draw_setup);
	model_create_draw_vertexes(model,0,&draw_setup);

	model_translate_draw_vertex(model,0,x,y,z);

		// draw triangles

	mesh=&model->meshes[0];
	
	glEnable(GL_TEXTURE_2D);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,mesh->draw.gl_vertex_array);
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
		if (texture->bitmaps[0].alpha_mode!=alpha_mode_transparent) walk_view_model_draw_material(model,&draw_setup,texture,material);
		texture++;
		material++;
	}
	
		// run through the transparent textures

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	
	texture=model->textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		if (texture->bitmaps[0].alpha_mode==alpha_mode_transparent) walk_view_model_draw_material(model,&draw_setup,texture,material);
		texture++;
		material++;
	}
	
	glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
    
	glUnlockArraysEXT();
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
	
	return(TRUE);
}


/* =======================================================

      Get Model Click Size
      
======================================================= */

bool walk_view_model_click_select_size(d3pnt *cpt,char *name,d3pnt *pnt,d3ang *ang,int *px,int *pz,int *ty,int *by)
{
	int						idx,x,y,z,wid_x,wid_z,high;
	model_type				*model;
	
		// get position
		
	x=pnt->x-cpt->x;
	y=pnt->y-cpt->y;
	z=cpt->z-pnt->z;

		// default size
		
    wid_x=wid_z=map_enlarge*3;
    high=map_enlarge*4;

		// if there is a model, then get size
		
	if (name!=NULL) {
		if (name[0]!=0x0) {
	
			idx=walk_view_models_find_model(name);
			if (idx!=-1) {
	
				model=&models[idx];
		
				wid_x=model->view_box.size.x>>1;
				wid_z=model->view_box.size.z>>1;
				high=model->view_box.size.y;
			}
		}
	}
	
		// get polygons
	
	px[0]=px[3]=x-wid_x;
	px[1]=px[2]=x+wid_x;
		
	pz[0]=pz[1]=z-wid_z;
	pz[2]=pz[3]=z+wid_z;
	
	if (ang!=NULL) rotate_2D_polygon(4,px,pz,x,z,-ang->y);

	*ty=y-high;
	*by=y;
	
	return(TRUE);
}

/* =======================================================

      Draw Model Select
      
======================================================= */

bool walk_view_model_draw_select(d3pnt *cpt,d3pnt *pnt,d3ang *ang,char *name)
{
	int				px[4],pz[4],ty,by;
	
		// get polygons
		
	if (!walk_view_model_click_select_size(cpt,name,pnt,ang,px,pz,&ty,&by)) return(FALSE);

		// draw selection
		
    glLineWidth(3);
	glColor4f(1,0,0,1);
    
	glBegin(GL_LINE_LOOP);
	glVertex3i(px[0],ty,pz[0]);
	glVertex3i(px[1],ty,pz[1]);
	glVertex3i(px[2],ty,pz[2]);
	glVertex3i(px[3],ty,pz[3]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3i(px[0],by,pz[0]);
	glVertex3i(px[1],by,pz[1]);
	glVertex3i(px[2],by,pz[2]);
	glVertex3i(px[3],by,pz[3]);
	glEnd();
	
	glBegin(GL_LINES);
 	glVertex3i(px[0],ty,pz[0]);
	glVertex3i(px[0],by,pz[0]);
	glVertex3i(px[1],ty,pz[1]);
	glVertex3i(px[1],by,pz[1]);
	glVertex3i(px[2],ty,pz[2]);
	glVertex3i(px[2],by,pz[2]);
	glVertex3i(px[3],ty,pz[3]);
	glVertex3i(px[3],by,pz[3]);
	glEnd();
    
    glLineWidth(1);
	
	return(TRUE);
}

