/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shadow Drawing

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

#include "consoles.h"
#include "models.h"
#include "lights.h"
#include "video.h"
#include "physics.h"

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

int						shadow_stencil_poly_vertex_count,shadow_replace_idx;
float					*shadow_poly_stencil_vbo;
d3pnt					*shadow_spt,*shadow_hpt;
d3vct					*shadow_vct;
poly_pointer_type		*shadow_poly_ptrs;
view_light_spot_type	shadow_above_lspot;

shadow_render_type		shadow_renders[max_shadow_render];

/* =======================================================

      Initialize and Shutdown Shadows
      
======================================================= */

bool shadow_initialize(void)
{
	int					n;
	shadow_render_type	*shad;

		// memory for ray tracing lists
		
	shadow_spt=(d3pnt*)malloc(sizeof(d3pnt)*shadow_max_ray_trace_vertexes);
	if (shadow_spt==NULL) return(FALSE);

	shadow_hpt=(d3pnt*)malloc(sizeof(d3pnt)*shadow_max_ray_trace_vertexes);
	if (shadow_hpt==NULL) return(FALSE);
	
	shadow_vct=(d3vct*)malloc(sizeof(d3vct)*shadow_max_ray_trace_vertexes);
	if (shadow_vct==NULL) return(FALSE);
	
		// memory for stencil vbo and poly ptrs
	
	shadow_poly_ptrs=(poly_pointer_type*)malloc(shadow_max_polys*sizeof(poly_pointer_type));
	if (shadow_poly_ptrs==NULL) return(FALSE);

	shadow_poly_stencil_vbo=(float*)malloc(((8*shadow_max_polys)*3)*sizeof(float));
	if (shadow_poly_stencil_vbo==NULL) return(FALSE);

		// shadow renders

	shad=shadow_renders;

	for (n=0;n!=max_shadow_render;n++) {
		shad->indexes=(unsigned short*)malloc(sizeof(unsigned short)*(shadow_max_trigs*3));
		if (shad->indexes==NULL) return(FALSE);

		shad->vp=(float*)malloc(sizeof(float)*(3*shadow_max_vertexes));
		if (shad->vp==NULL) return(FALSE);

		shad->cp=(float*)malloc(sizeof(float)*(4*shadow_max_vertexes));
		if (shad->cp==NULL) return(FALSE);

		shad->polys=(shadow_render_poly_type*)malloc(sizeof(shadow_render_poly_type)*shadow_max_polys);
		if (shad->polys==NULL) return(FALSE);

		shad->cur_item.idx=-1;

		shad++;
	}

	shadow_replace_idx=0;

	return(TRUE);
}

void shadow_shutdown(void)
{
	int					n;
	shadow_render_type	*shad;

		// shadow renders

	shad=shadow_renders;

	for (n=0;n!=max_shadow_render;n++) {
		free(shad->indexes);
		free(shad->vp);
		free(shad->cp);
		free(shad->polys);

		shad++;
	}

		// shadow ray trace lists

	free(shadow_poly_ptrs);
	free(shadow_poly_stencil_vbo);

	free(shadow_spt);
	free(shadow_vct);
	free(shadow_hpt);
}

/* =======================================================

      Find A Shadow Renderer
      
======================================================= */

void shadow_render_update(shadow_render_type *shad,d3pnt *pnt,view_light_spot_type *lspot)
{
	memmove(&shad->cur_item.pnt,pnt,sizeof(d3pnt));
	memmove(&shad->cur_light.pnt,&lspot->pnt,sizeof(d3pnt));
	memmove(&shad->cur_light.col,&lspot->col,sizeof(d3col));
	shad->cur_light.intensity=lspot->intensity;
}

shadow_render_type* shadow_find_existing_render(int item_type,int item_idx,d3pnt *pnt,view_light_spot_type *lspot,bool *shadow_changed)
{
	int					n;
	shadow_render_type	*shad;

		// find if we already setup a shadow render
		// for this object in this lighting

	shad=shadow_renders;

	for (n=0;n!=max_shadow_render;n++) {

		if ((shad->cur_item.type==item_type) && (shad->cur_item.idx==item_idx)) {

				// has position changed?

			if ((shad->cur_item.pnt.x!=pnt->x) || (shad->cur_item.pnt.y!=pnt->y) || (shad->cur_item.pnt.z!=pnt->z)) {
				*shadow_changed=TRUE;
				shadow_render_update(shad,pnt,lspot);
				return(shad);
			}

				// has lighting changed?

			if ((shad->cur_light.intensity!=lspot->intensity) || (shad->cur_light.pnt.x!=lspot->pnt.x) || (shad->cur_light.pnt.y!=lspot->pnt.y) || (shad->cur_light.pnt.z!=lspot->pnt.z) || (shad->cur_light.col.r!=lspot->col.r) || (shad->cur_light.col.g!=lspot->col.g) || (shad->cur_light.col.b!=lspot->col.b)) {
				*shadow_changed=TRUE;
				shadow_render_update(shad,pnt,lspot);
				return(shad);
			}

				// nothings changed, re-use

			*shadow_changed=FALSE;

			return(shad);
		}

		shad++;
	}

	return(NULL);
}

shadow_render_type* shadow_create_new_render(int item_type,int item_idx,d3pnt *pnt,view_light_spot_type *lspot)
{
	int					n,idx;
	shadow_render_type	*shad;

		// find a free item first

	idx=-1;
	shad=shadow_renders;

	for (n=0;n!=max_shadow_render;n++) {
		if (shad->cur_item.idx==-1) {
			idx=n;
			break;
		}
		shad++;
	}

		// if there are no open spots, then
		// repurpose renders in round robin fashion

	if (idx==-1) {
		idx=shadow_replace_idx;

		shadow_replace_idx++;
		if (shadow_replace_idx>=max_shadow_render) shadow_replace_idx=0;
	}

		// setup items

	shad=&shadow_renders[idx];

	shad->cur_item.type=item_type;
	shad->cur_item.idx=item_idx;

	shadow_render_update(shad,pnt,lspot);

	return(shad);
}

void shadow_render_free(shadow_render_type *shad)
{
	shad->cur_item.idx=-1;
}

/* =======================================================

      Shadow Light
      
======================================================= */

view_light_spot_type* shadow_get_light_spot(d3pnt *pnt,int high)
{
	view_light_spot_type			*lspot;

		// get closest light

	lspot=gl_light_find_closest_light(pnt->x,pnt->y,pnt->z);

		// if no light, get light directly above

	if (lspot==NULL) {
		lspot=&shadow_above_lspot;
		memmove(&lspot->pnt,pnt,sizeof(d3pnt));
		lspot->pnt.y-=map_enlarge*100;
		lspot->intensity=(map_enlarge*100)+high;
	}

	return(lspot);
}

/* =======================================================

      Shadow Draw Volumes
      
======================================================= */

bool shadow_get_volume(d3pnt *pnt,int high,int *px,int *py,int *pz)
{
	int							n;
	float						f_dist;
	bool						hits[8];
	d3pnt						spt[8],ept[8],hpt[8],*sp,*ep;
	d3vct						ray_move;
	view_light_spot_type		*lspot;
	ray_trace_contact_type		base_contact;

		// get light and draw distance

	lspot=shadow_get_light_spot(pnt,high);
	f_dist=(float)lspot->intensity;

		// ray trace bounding box
		
	sp=spt;
	ep=ept;

	for (n=0;n!=8;n++) {
		sp->x=px[n];
		sp->y=py[n];
		sp->z=pz[n];

		vector_create(&ray_move,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,sp->x,sp->y,sp->z);
				
		ep->x=lspot->pnt.x-(int)(ray_move.x*f_dist);
		ep->y=lspot->pnt.y-(int)(ray_move.y*f_dist);
		ep->z=lspot->pnt.z-(int)(ray_move.z*f_dist);
		
		sp++;
		ep++;
	}

	base_contact.obj.on=FALSE;
	base_contact.obj.ignore_uid=-1;

	base_contact.proj.on=FALSE;
	base_contact.proj.ignore_uid=-1;

	base_contact.hit_mode=poly_ray_trace_hit_mode_all;
	base_contact.origin=poly_ray_trace_origin_object;

	ray_trace_map_by_point_array_no_contact(8,spt,ept,hpt,hits,&base_contact);

		// set the volume

	for (n=0;n!=8;n++) {
		px[n]=hpt[n].x;
		py[n]=hpt[n].y;
		pz[n]=hpt[n].z;
	}

	return(TRUE);
}

inline bool shadow_get_volume_model(model_type *mdl,model_draw *draw,int *px,int *py,int *pz)
{
	model_get_view_complex_bounding_box(mdl,&draw->pnt,&draw->setup.ang,px,py,pz);
	return(shadow_get_volume(&draw->pnt,draw->size.y,px,py,pz));
}

inline bool shadow_get_volume_mesh(map_mesh_type *mesh,int *px,int *py,int *pz)
{
	px[0]=px[3]=px[4]=px[7]=mesh->box.min.x;
	px[1]=px[2]=px[5]=px[6]=mesh->box.max.x;
	py[0]=py[1]=py[2]=py[3]=mesh->box.min.y;
	py[4]=py[5]=py[6]=py[7]=mesh->box.max.y;
	pz[0]=pz[1]=pz[4]=pz[5]=mesh->box.min.z;
	pz[2]=pz[3]=pz[6]=pz[7]=mesh->box.max.z;

	return(shadow_get_volume(&mesh->box.mid,(mesh->box.max.y-mesh->box.min.y),px,py,pz));
}

/* =======================================================

      Find Polygons that Shadows Cross
      
======================================================= */

int shadow_build_poly_set(int *px,int *py,int *pz,int skip_mesh_idx)
{
	int					n,k,cnt;
	d3pnt				min,max;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// get shadow volume for collision check
		
	min.x=max.x=px[0];
	min.y=max.y=py[0];
	min.z=max.z=pz[0];
	
	for (n=1;n!=8;n++) {
		if (px[n]<min.x) min.x=px[n];
		if (px[n]>max.x) max.x=px[n];
		if (py[n]<min.y) min.y=py[n];
		if (py[n]>max.y) max.y=py[n];
		if (pz[n]<min.z) min.z=pz[n];
		if (pz[n]>max.z) max.z=pz[n];
	}

		// add in some slop for shadows
		// that directly collide with a polygon
		// and don't pass through it

	min.x-=map_enlarge;
	max.x+=map_enlarge;
	min.y-=map_enlarge;
	max.y+=map_enlarge;
	min.z-=map_enlarge;
	max.z+=map_enlarge;
	
		// check meshes
		
	cnt=0;
	
	for (n=0;n!=view.render->draw_list.count;n++) {
	
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;
		if (view.render->draw_list.items[n].idx==skip_mesh_idx) continue;

			// find mesh colliding with shadow

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];

		if ((mesh->box.max.x<min.x) || (mesh->box.min.x>max.x)) continue;
		if ((mesh->box.max.z<min.z) || (mesh->box.min.z>max.z)) continue;
		if ((mesh->box.max.y<min.y) || (mesh->box.min.y>max.y)) continue;		// check Y last as X/Z are usually better eliminations
		
			// find polys colliding with shadow
			
		for (k=0;k!=mesh->npoly;k++) {
			poly=&mesh->polys[k];
			
				// skip all transparent polys
				
			if (poly->draw.transparent_on) continue;

				// check if within shadow volume
				
			if ((poly->box.max.x<min.x) || (poly->box.min.x>max.x)) continue;
			if ((poly->box.max.z<min.z) || (poly->box.min.z>max.z)) continue;
			if ((poly->box.max.y<min.y) || (poly->box.min.y>max.y)) continue;		// check Y last as X/Z are usually better eliminations
			
			shadow_poly_ptrs[cnt].mesh_idx=view.render->draw_list.items[n].idx;
			shadow_poly_ptrs[cnt].poly_idx=k;
			cnt++;
			
				// can only shadow for max stencil count
				
			if (cnt==shadow_max_polys) return(cnt);
		}
	}
	
	return(cnt);
}

inline int shadow_build_poly_set_model(model_type *mdl,model_draw *draw)
{
	int				px[8],py[8],pz[8];

	if (!shadow_get_volume_model(mdl,draw,px,py,pz)) return(0);
	return(shadow_build_poly_set(px,py,pz,-1));	
}

inline int shadow_build_poly_set_mesh(int mesh_idx)
{
	int				px[8],py[8],pz[8];

	if (!shadow_get_volume_mesh(&map.mesh.meshes[mesh_idx],px,py,pz)) return(0);
	return(shadow_build_poly_set(px,py,pz,mesh_idx));	
}

/* =======================================================

      Stencil Shadows Polygons
      
======================================================= */

void shadow_stencil_mesh_poly(shadow_render_type *shadow_render)
{
	int						n,k,vertex_count;
	float					*vp,*vertex_ptr;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	shadow_render_poly_type	*shadow_poly;

		// setup vbo memory

	vertex_count=0;
	vp=shadow_poly_stencil_vbo;

	shadow_poly=shadow_render->polys;

	for (k=0;k!=shadow_render->npoly;k++) {
		
		mesh=&map.mesh.meshes[shadow_poly->ptr.mesh_idx];
		poly=&mesh->polys[shadow_poly->ptr.poly_idx];
		
		for (n=0;n!=poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];
			*vp++=(float)pt->x;
			*vp++=(float)pt->y;
			*vp++=(float)pt->z;
		}

		vertex_count+=poly->ptsz;
		shadow_poly++;
	}

	shadow_stencil_poly_vertex_count=vertex_count;

		// setup vertex ptr
		
	vertex_ptr=view_bind_map_next_vertex_object(shadow_stencil_poly_vertex_count*3);
	if (vertex_ptr==NULL) return;
	
	memmove(vertex_ptr,shadow_poly_stencil_vbo,((shadow_stencil_poly_vertex_count*3)*sizeof(float)));
	
	view_unmap_current_vertex_object();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);
		
		// stencil the polygon
		
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	
	vertex_count=0;
	shadow_poly=shadow_render->polys;

	for (k=0;k!=shadow_render->npoly;k++) {

		glStencilFunc(GL_ALWAYS,(k+1),0xFF);

		mesh=&map.mesh.meshes[shadow_poly->ptr.mesh_idx];
		poly=&mesh->polys[shadow_poly->ptr.poly_idx];

		glDrawArrays(GL_POLYGON,vertex_count,poly->ptsz);

		vertex_count+=poly->ptsz;
		shadow_poly++;
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	view_unbind_current_vertex_object();
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

void shadow_stencil_clear_mesh_poly(shadow_render_type *shadow_render)
{
	int						k,vertex_count;
	float					*vertex_ptr;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	shadow_render_poly_type	*shadow_poly;
	
		// use the set data to setup the clear vbo
		
	vertex_ptr=view_bind_map_next_vertex_object(shadow_stencil_poly_vertex_count*3);
	if (vertex_ptr==NULL) return;
	
	memmove(vertex_ptr,shadow_poly_stencil_vbo,((shadow_stencil_poly_vertex_count*3)*sizeof(float)));
	
	view_unmap_current_vertex_object();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);
		
		// setup the stenciling
		
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
	glStencilFunc(GL_ALWAYS,0x0,0xFF);
	
		// stencil the polygon
		
	vertex_count=0;
	shadow_poly=shadow_render->polys;

	for (k=0;k!=shadow_render->npoly;k++) {
		mesh=&map.mesh.meshes[shadow_poly->ptr.mesh_idx];
		poly=&mesh->polys[shadow_poly->ptr.poly_idx];

		glDrawArrays(GL_POLYGON,vertex_count,poly->ptsz);

		vertex_count+=poly->ptsz;
		shadow_poly++;
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	view_unbind_current_vertex_object();
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

/* =======================================================

      Render Generic Shadows
      
======================================================= */

void shadow_render_generic(shadow_render_type *shad)
{
	int							k;
	float						*vertex_ptr,*cl;
	unsigned short				*index_ptr;
	shadow_render_poly_type		*shadow_poly;

		// for now, leverage the model shadow
		// arrays and only allow meshes with a limited
		// number of vertexes

	if (shad->npoly==0) return;

		// start stenciling
		// never write to the depth buffer during this operation

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);

	glDepthMask(GL_FALSE);

		// stencil in the polys shadow is casting against

	shadow_stencil_mesh_poly(shad);

		// setup the vertex object

	vertex_ptr=view_bind_map_next_vertex_object(shad->nvertex*(3+4));
	if (vertex_ptr==NULL) return;

	memmove(vertex_ptr,shad->vp,((3*shad->nvertex)*sizeof(float)));

	cl=vertex_ptr+(shad->nvertex*3);
	memmove(cl,shad->cp,((4*shad->nvertex)*sizeof(float)));

	view_unmap_current_vertex_object();

		// create the index object

	index_ptr=view_bind_map_next_index_object(shad->nindex);
	if (index_ptr==NULL) {
		view_unbind_current_vertex_object();
		return;
	}

	memmove(index_ptr,shad->indexes,(shad->nindex*sizeof(unsigned short)));

	view_unmap_current_index_object();

		// setup arrays

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,(void*)((shad->nvertex*3)*sizeof(float)));

		// run through all the stenciled polygons
		// and draw their trigs

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
			
	shadow_poly=shad->polys;

	for (k=0;k!=shad->npoly;k++) {
		glStencilFunc(GL_EQUAL,(k+1),0xFF);
		glDrawRangeElements(GL_TRIANGLES,0,shad->nvertex,shadow_poly->count_idx,GL_UNSIGNED_SHORT,(GLvoid*)shadow_poly->start_idx);

		shadow_poly++;
	}

		// disable the arrays

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
		// unbind the vertex and index object
			
	view_unbind_current_vertex_object();
	view_unbind_current_index_object();
		
		// clear the stencils

	shadow_stencil_clear_mesh_poly(shad);

		// restore depth buffer and turn
		// off stenciling
		
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}

/* =======================================================

      Render Model Bounds Eliminations
      
======================================================= */

void shadow_render_prepare_bounds_check(poly_pointer_type *poly_ptr,d3pnt *min,d3pnt *max)
{
	int						sz;
	map_mesh_poly_type		*poly;

	poly=&map.mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];

	sz=map_enlarge>>1;

	min->x=poly->box.min.x-sz;
	max->x=poly->box.max.x+sz;
	min->y=poly->box.min.y-sz;
	max->y=poly->box.max.y+sz;
	min->z=poly->box.min.z-sz;
	max->z=poly->box.max.z+sz;
}

bool shadow_render_model_trig_bounds_check(d3pnt *min,d3pnt *max,int v_idx0,int v_idx1,int v_idx2)
{
	d3pnt			*pt0,*pt1,*pt2;

	pt0=&shadow_hpt[v_idx0];
	pt1=&shadow_hpt[v_idx1];
	pt2=&shadow_hpt[v_idx2];

	if ((pt0->x<min->x) && (pt1->x<min->x) && (pt2->x<min->x)) return(FALSE);
	if ((pt0->x>max->x) && (pt1->x>max->x) && (pt2->x>max->x)) return(FALSE);
	if ((pt0->z<min->z) && (pt1->z<min->z) && (pt2->z<min->z)) return(FALSE);
	if ((pt0->z>max->z) && (pt1->z>max->z) && (pt2->z>max->z)) return(FALSE);
	if ((pt0->y<min->y) && (pt1->y<min->y) && (pt2->y<min->y)) return(FALSE);
	if ((pt0->y>max->y) && (pt1->y>max->y) && (pt2->y>max->y)) return(FALSE);

	return(TRUE);
}

bool shadow_render_mesh_poly_bounds_check(d3pnt *min,d3pnt *max,map_mesh_poly_type *poly)
{
	int				n;
	bool			min_x,max_x,min_y,max_y,min_z,max_z;
	d3pnt			*pt;

	min_x=max_x=TRUE;
	min_y=max_y=TRUE;
	min_z=max_z=TRUE;

	for (n=0;n!=poly->ptsz;n++) {
		pt=&shadow_hpt[poly->v[n]];

		min_x=min_x||(pt->x<min->x);
		max_x=max_x||(pt->x>max->x);
		min_y=min_y||(pt->y<min->y);
		max_y=max_y||(pt->y>max->y);
		min_z=min_z||(pt->z<min->z);
		max_z=max_z||(pt->z>max->z);
	}

	return(min_x||max_x||min_y||max_y||min_z||max_z);
}

/* =======================================================

      Render Model Shadows
      
======================================================= */

void shadow_render_model(int item_type,int item_idx,model_draw *draw)
{
	int							n,k,t,ray_count,poly_count,trig_count,trig_offset,
								vertex_offset,index_offset;
	int							mesh_vertex_offset[max_model_mesh];
	double						dx,dy,dz,d_alpha;
	bool						light_changed;
	float						*vp,*vl,*cl;
	unsigned short				*index_ptr;
	d3vct						*vct;
	d3pnt						*spt,*hpt,bound_min,bound_max;
    model_trig_type				*trig;
	model_mesh_type				*mesh;
	model_type					*mdl;
	view_light_spot_type		*lspot;
	shadow_render_type			*shad;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// get light

	lspot=shadow_get_light_spot(&draw->pnt,draw->size.y);

		// get render to use

	shad=shadow_find_existing_render(item_type,item_idx,&draw->pnt,lspot,&light_changed);
	if (shad!=NULL) {

			// no light change or animating, just redraw setup
		if (!light_changed) {
			shadow_render_generic(shad);
			return;
		}
	}
	else {
		shad=shadow_create_new_render(item_type,item_idx,&draw->pnt,lspot);
	}
	
		// find all polys the shadow ray hits

	poly_count=shadow_build_poly_set_model(mdl,draw);
	if (poly_count==0) {
		shadow_render_free(shad);
		return;
	}

		// get distance alpha factor

	d_alpha=(double)lspot->intensity;
	d_alpha=1.0/(d_alpha*d_alpha);

		// setup the rays

	spt=shadow_spt;
	vct=shadow_vct;

	ray_count=0;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];
		if ((ray_count+mesh->nvertex)>shadow_max_ray_trace_vertexes) {
			shadow_render_free(shad);
			return;
		}

		vp=draw->setup.mesh_arrays[n].gl_vertex_array;

		mesh_vertex_offset[n]=ray_count;
			
		for (t=0;t!=mesh->nvertex;t++) {
			spt->x=(int)*vp++;
			spt->y=(int)*vp++;
			spt->z=(int)*vp++;
				
			vct->x=(float)((spt->x-lspot->pnt.x)*100);
			vct->y=(float)((spt->y-lspot->pnt.y)*100);
			vct->z=(float)((spt->z-lspot->pnt.z)*100);
				
			spt++;
			vct++;
		}

		ray_count+=mesh->nvertex;
	}

		// ray trace the mesh against
		// the plane of the polygon

		// build the vertexes, colors and indexes
		// for each poly and store in one list

	vl=shad->vp;
	cl=shad->cp;
	index_ptr=shad->indexes;

	vertex_offset=0;
	index_offset=0;

	shad->npoly=0;

	for (k=0;k!=poly_count;k++) {

			// ray trace the vertexes

		if (!ray_trace_mesh_poly_plane_by_vector(ray_count,shadow_spt,shadow_vct,shadow_hpt,shadow_poly_ptrs[k].mesh_idx,shadow_poly_ptrs[k].poly_idx)) continue;
		
			// setup bounding eliminations
		
		shadow_render_prepare_bounds_check(&shadow_poly_ptrs[k],&bound_min,&bound_max);

			// create the index object
			// skip triangles with no collisions

		trig_count=0;
		trig_offset=index_offset/3;

		for (n=0;n!=mdl->nmesh;n++) {
			if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			mesh=&mdl->meshes[n];
			trig=mesh->trigs;
			
			for (t=0;t!=mesh->ntrig;t++) {

					// do a bounds check for quick eliminations

				if (!shadow_render_model_trig_bounds_check(&bound_min,&bound_max,trig->v[0]+mesh_vertex_offset[n],trig->v[1]+mesh_vertex_offset[n],trig->v[2]+mesh_vertex_offset[n])) {
					trig++;
					continue;
				}

					// triangle indexes

				*index_ptr++=(unsigned short)(trig->v[0]+vertex_offset+mesh_vertex_offset[n]);
				*index_ptr++=(unsigned short)(trig->v[1]+vertex_offset+mesh_vertex_offset[n]);
				*index_ptr++=(unsigned short)(trig->v[2]+vertex_offset+mesh_vertex_offset[n]);

				trig++;

				trig_count++;
				if ((trig_offset+trig_count)==shadow_max_trigs) {
					shadow_render_free(shad);
					return;
				}
			}
		}

			// if no trigs to draw, skip this poly

		if (trig_count==0) continue;

			// check to see if we are running out of vertexes
			
		if ((vertex_offset+ray_count)>=shadow_max_vertexes) {
			shadow_render_free(shad);
			return;
		}

			// setup indexed per poly drawing

		shad->polys[shad->npoly].start_idx=index_offset*sizeof(unsigned short);
		shad->polys[shad->npoly].count_idx=trig_count*3;
		shad->polys[shad->npoly].ptr.mesh_idx=shadow_poly_ptrs[k].mesh_idx;
		shad->polys[shad->npoly].ptr.poly_idx=shadow_poly_ptrs[k].poly_idx;
		
		shad->npoly++;

		index_offset+=(trig_count*3);

			// build vertex and color list

		hpt=shadow_hpt;
		
		for (n=0;n!=ray_count;n++) {

				// vertex

			*vl++=(float)hpt->x;
			*vl++=(float)hpt->y;
			*vl++=(float)hpt->z;

				// color

			dx=(hpt->x-lspot->pnt.x);
			dy=(hpt->y-lspot->pnt.y);
			dz=(hpt->z-lspot->pnt.z);
			hpt++;

			*cl++=0.0f;
			*cl++=0.0f;
			*cl++=0.0f;
			*cl++=1.0f-(float)(((dx*dx)+(dy*dy)+(dz*dz))*d_alpha);
		}
	
		vertex_offset+=ray_count;
	}

		// finish setup

	shad->nindex=index_offset;
	shad->nvertex=vertex_offset;

		// render the shadow

	shadow_render_generic(shad);
}

/* =======================================================

      Render Mesh Shadows
      
======================================================= */

void shadow_render_mesh(int mesh_idx)
{
	int							n,k,t,vertex_offset,index_offset,poly_count,
								ntrig,trig_count,trig_offset;
	double						dx,dy,dz,d_alpha;
	bool						light_changed;
	float						*vl,*cl;
	unsigned short				*index_ptr;
	d3vct						*vct;
	d3pnt						*pt,*spt,*hpt,bound_min,bound_max;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	view_light_spot_type		*lspot;
	shadow_render_type			*shad;
	
	mesh=&map.mesh.meshes[mesh_idx];
	if (mesh->nvertex>=shadow_max_ray_trace_vertexes) return;
	
		// get light

	lspot=shadow_get_light_spot(&mesh->box.mid,(mesh->box.max.y-mesh->box.min.y));
	
		// get render to use

	shad=shadow_find_existing_render(view_render_type_mesh,mesh_idx,&mesh->box.mid,lspot,&light_changed);
	if (shad!=NULL) {

			// no light change, just redraw setup

		if (!light_changed) {
			shadow_render_generic(shad);
			return;
		}
	}
	else {
		shad=shadow_create_new_render(view_render_type_mesh,mesh_idx,&mesh->box.mid,lspot);
	}

		// find all polys the shadow ray hits
		
	poly_count=shadow_build_poly_set_mesh(mesh_idx);
	if (poly_count==0) {
		shadow_render_free(shad);
		return;
	}

		// get distance alpha factor

	d_alpha=(double)lspot->intensity;
	d_alpha=1.0/(d_alpha*d_alpha);

		// setup the rays

	spt=shadow_spt;
	vct=shadow_vct;
	pt=mesh->vertexes;
	
	for (n=0;n!=mesh->nvertex;n++) {
		spt->x=pt->x;
		spt->y=pt->y;
		spt->z=pt->z;
		
		vct->x=(float)((spt->x-lspot->pnt.x)*100);
		vct->y=(float)((spt->y-lspot->pnt.y)*100);
		vct->z=(float)((spt->z-lspot->pnt.z)*100);
		
		spt++;
		vct++;
		pt++;
	}

		// build the vertexes, colors and indexes
		// for each poly and store in one list

	vl=shad->vp;
	cl=shad->cp;
	index_ptr=shad->indexes;

	vertex_offset=0;
	index_offset=0;

	shad->npoly=0;

	for (k=0;k!=poly_count;k++) {

			// ray trace the vertexes

		if (!ray_trace_mesh_poly_plane_by_vector(mesh->nvertex,shadow_spt,shadow_vct,shadow_hpt,shadow_poly_ptrs[k].mesh_idx,shadow_poly_ptrs[k].poly_idx)) continue;
		
			// setup bounding eliminations
		
		shadow_render_prepare_bounds_check(&shadow_poly_ptrs[k],&bound_min,&bound_max);

			// create the index object
			// break polygons up into triangles so we
			// can draw with one call and skip polygons
			// with no ray hits

		trig_count=0;
		trig_offset=index_offset/3;

		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {

				// polygon bounds elimination

			if (!shadow_render_mesh_poly_bounds_check(&bound_min,&bound_max,poly)) {
				poly++;
				continue;
			}

				// convert polygon into trigs

			ntrig=poly->ptsz-2;
			
			for (t=0;t<ntrig;t++) {
				*index_ptr++=(unsigned short)(poly->v[0]+vertex_offset);
				*index_ptr++=(unsigned short)(poly->v[t+1]+vertex_offset);
				*index_ptr++=(unsigned short)(poly->v[t+2]+vertex_offset);
				
				trig_count++;
				if ((trig_offset+trig_count)==shadow_max_trigs) {
					shadow_render_free(shad);
					return;
				}
			}

			poly++;
		}

			// if no trigs to draw, skip this poly

		if (trig_count==0) continue;
			
			// check to see if we are running out of vertexes
			
		if ((vertex_offset+mesh->nvertex)>=shadow_max_vertexes) {
			shadow_render_free(shad);
			return;
		}

			// setup indexed per poly drawing

		shad->polys[shad->npoly].start_idx=index_offset*sizeof(unsigned short);
		shad->polys[shad->npoly].count_idx=trig_count*3;
		shad->polys[shad->npoly].ptr.mesh_idx=shadow_poly_ptrs[k].mesh_idx;
		shad->polys[shad->npoly].ptr.poly_idx=shadow_poly_ptrs[k].poly_idx;

		shad->npoly++;

		index_offset+=(trig_count*3);

			// build vertex and color list

		hpt=shadow_hpt;
		
		for (n=0;n!=mesh->nvertex;n++) {

				// vertex

			*vl++=(float)hpt->x;
			*vl++=(float)hpt->y;
			*vl++=(float)hpt->z;

				// color

			dx=(double)(hpt->x-lspot->pnt.x);
			dy=(double)(hpt->y-lspot->pnt.y);
			dz=(double)(hpt->z-lspot->pnt.z);
			
			hpt++;

			*cl++=0.0f;
			*cl++=0.0f;
			*cl++=0.0f;
			*cl++=1.0f-(float)(((dx*dx)+(dy*dy)+(dz*dz))*d_alpha);
		}
		
		vertex_offset+=mesh->nvertex;
	}

		// finish setup

	shad->nindex=index_offset;
	shad->nvertex=vertex_offset;

		// render the shadow	

	shadow_render_generic(shad);
}

