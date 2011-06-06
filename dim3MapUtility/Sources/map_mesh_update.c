/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Mesh Updates

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern void map_prepare_mesh_poly(map_mesh_type *mesh,map_mesh_poly_type *mesh_poly);
extern void map_prepare_mesh_box(map_mesh_type *mesh);

/* =======================================================

      Combine Meshes
      
======================================================= */

void map_mesh_combine_single_mesh(map_mesh_type *mesh,map_mesh_type *mesh_copy)
{
	int					n,t,k,v_idx;
	d3pnt				*d3pt,*chk_d3pt;
	map_mesh_poly_type	*mesh_poly,*mesh_copy_poly;
	
		// get place we last stopped adding
		
	mesh_poly=&mesh->polys[mesh->npoly];
	mesh_copy_poly=mesh_copy->polys;
	
	for (n=0;n!=mesh_copy->npoly;n++) {
	
			// add the poly
			
		memmove(mesh_poly,mesh_copy_poly,sizeof(map_mesh_poly_type));
	
			// add in the vertexes, checking for combines
			
		for (t=0;t!=mesh_copy_poly->ptsz;t++) {
			chk_d3pt=&mesh_copy->vertexes[mesh_copy_poly->v[t]];
			
			v_idx=-1;
			d3pt=mesh->vertexes;

			for (k=0;k!=mesh->nvertex;k++) {
				if ((d3pt->x==chk_d3pt->x) && (d3pt->y==chk_d3pt->y) && (d3pt->z==chk_d3pt->z)) {
					v_idx=k;
					break;
				}
				
				d3pt++;
			}

				// need to add new vertex
				
			if (v_idx==-1) {
				v_idx=mesh->nvertex;
				mesh->nvertex++;
				
				d3pt=&mesh->vertexes[v_idx];
				d3pt->x=chk_d3pt->x;
				d3pt->y=chk_d3pt->y;
				d3pt->z=chk_d3pt->z;
			}

			mesh_poly->v[t]=v_idx;
		}
		
		mesh->npoly++;
		
		mesh_poly++;
		mesh_copy_poly++;
	}
}

int map_mesh_combine(map_type *map,int mesh_1_idx,int mesh_2_idx)
{
	int					mesh_idx;
	map_mesh_type		*mesh,*mesh_1,*mesh_2;
	
		// create new combined mesh
		
	mesh_idx=map_mesh_add(map);
	if (mesh_idx==-1) return(-1);

		// get combined meshes
		// need to get after mesh add as mesh add can change mesh pointers
		
	mesh_1=&map->mesh.meshes[mesh_1_idx];
	mesh_2=&map->mesh.meshes[mesh_2_idx];
	
		// setup enough vertexes and polys for new mesh
		
	if (!map_mesh_set_vertex_count(map,mesh_idx,(mesh_1->nvertex+mesh_2->nvertex))) {
		map_mesh_delete(map,mesh_idx);
		return(-1);
	}
	
	if (!map_mesh_set_poly_count(map,mesh_idx,(mesh_1->npoly+mesh_2->npoly))) {
		map_mesh_delete(map,mesh_idx);
		return(-1);
	}
	
		// transfer over some settings from first mesh
		
	mesh=&map->mesh.meshes[mesh_idx];

	mesh->npoly=0;
	mesh->nvertex=0;
		
	mesh->group_idx=mesh_1->group_idx;
	mesh->hide_mode=mesh_1->hide_mode;
	mesh->harm=mesh_1->harm;

	memmove(&mesh->rot_off,&mesh_1->rot_off,sizeof(d3pnt));
	memmove(&mesh->flag,&mesh_1->flag,sizeof(map_mesh_flag_type));
	memmove(&mesh->msg,&mesh_1->msg,sizeof(map_mesh_message_type));

		// combined meshes
		
	map_mesh_combine_single_mesh(mesh,mesh_1);
	map_mesh_combine_single_mesh(mesh,mesh_2);
	
		// get back to correct size
		// ignore failures as it's just a waste of space that
		// will be reclaimed later
		
	map_mesh_set_vertex_count(map,mesh_idx,mesh->nvertex);
	map_mesh_set_poly_count(map,mesh_idx,mesh->npoly);
	
		// delete original meshes, making sure to
		// change delete index depending on first delete
		// and return mesh index minus the two
		// deleted meshes
		
	map_mesh_delete(map,mesh_1_idx);
	
	if (mesh_1_idx<mesh_2_idx) mesh_2_idx--;
	map_mesh_delete(map,mesh_2_idx);	

	return(mesh_idx-2);
}

/* =======================================================

      Move Mesh
      
======================================================= */

void map_mesh_move(map_type *map,int mesh_idx,d3pnt *mov_pnt)
{
	int									n,nvertex,npoly;
	d3pnt								*pt;
	map_mesh_type						*mesh;
	map_mesh_poly_type					*poly;

	mesh=&map->mesh.meshes[mesh_idx];

		// move meshes vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		pt->x+=mov_pnt->x;
		pt->y+=mov_pnt->y;
		pt->z+=mov_pnt->z;
		pt++;
	}

		// move all poly boxes
		
	npoly=mesh->npoly;
	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		map_prepare_mesh_poly(mesh,poly);
		poly++;
	}

		// fix mesh box

	map_prepare_mesh_box(mesh);
}

/* =======================================================

      Resize Mesh
      
======================================================= */

void map_mesh_resize(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max)
{
	int						n,nvertex;
	d3pnt					*pt,org_min,org_max,dif,org_dif;
	d3vct					fct;
	map_mesh_type			*mesh;

		// get original size and uv center
		
	map_mesh_calculate_extent(map,mesh_idx,&org_min,&org_max);

		// get resize factor
		
	dif.x=max->x-min->x;
	dif.y=max->y-min->y;
	dif.z=max->z-min->z;
	
	org_dif.x=org_max.x-org_min.x;
	org_dif.y=org_max.y-org_min.y;
	org_dif.z=org_max.z-org_min.z;

	fct.x=fct.y=fct.z=0.0f;

	if (org_dif.x!=0) fct.x=(float)dif.x/(float)org_dif.x;
	if (org_dif.y!=0) fct.y=(float)dif.y/(float)org_dif.y;
	if (org_dif.z!=0) fct.z=(float)dif.z/(float)org_dif.z;
	
		// resize vertexes

	mesh=&map->mesh.meshes[mesh_idx];

		// resize vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {

		pt->x=(int)((float)(pt->x-org_min.x)*fct.x)+min->x;
		pt->y=(int)((float)(pt->y-org_min.y)*fct.y)+min->y;
		pt->z=(int)((float)(pt->z-org_min.z)*fct.z)+min->z;

		pt++;
	}
}

/* =======================================================

      Flip Mesh
      
======================================================= */

void map_mesh_flip(map_type *map,int mesh_idx,bool flip_x,bool flip_y,bool flip_z)
{
	int						n,nvertex,npoly;
	d3pnt					mpt;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

		// get center

	map_mesh_calculate_center(map,mesh_idx,&mpt);

		// flip vertexes

	mesh=&map->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		if (flip_x) pt->x=mpt.x-(pt->x-mpt.x);
		if (flip_y) pt->y=mpt.y-(pt->y-mpt.y);
		if (flip_z) pt->z=mpt.z-(pt->z-mpt.z);
		pt++;
	}
	
		// flip polys
		
	npoly=mesh->npoly;
	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		if (flip_x) poly->tangent_space.normal.x=-poly->tangent_space.normal.x;
		if (flip_y) poly->tangent_space.normal.y=-poly->tangent_space.normal.y;
		if (flip_z) poly->tangent_space.normal.z=-poly->tangent_space.normal.z;
		poly++;
	}
}

/* =======================================================

      Rotate Mesh
      
======================================================= */

void map_mesh_rotate(map_type *map,int mesh_idx,d3pnt *center_pnt,d3ang *rot_ang)
{
	int									n,nvertex,npoly;
	float								fx,fy,fz;
	d3vct								f_mpt;
	d3pnt								*pt;
	matrix_type							mat;
	map_mesh_type						*mesh;
	map_mesh_poly_type					*poly;

	mesh=&map->mesh.meshes[mesh_idx];

		// get mesh rotation center

	f_mpt.x=(float)(center_pnt->x+mesh->rot_off.x);
	f_mpt.y=(float)(center_pnt->y+mesh->rot_off.y);
	f_mpt.z=(float)(center_pnt->z+mesh->rot_off.z);

		// matrixes

	matrix_rotate_xyz(&mat,rot_ang->x,rot_ang->y,rot_ang->z);

		// rotate vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		fx=((float)pt->x)-f_mpt.x;
		fy=((float)pt->y)-f_mpt.y;
		fz=((float)pt->z)-f_mpt.z;

		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		pt->x=(int)(fx+f_mpt.x);
		pt->y=(int)(fy+f_mpt.y);
		pt->z=(int)(fz+f_mpt.z);

		pt++;
	}
	
		// rotate normals
		
	npoly=mesh->npoly;
	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		matrix_vertex_multiply(&mat,&poly->tangent_space.tangent.x,&poly->tangent_space.tangent.y,&poly->tangent_space.tangent.z);
		matrix_vertex_multiply(&mat,&poly->tangent_space.normal.x,&poly->tangent_space.normal.y,&poly->tangent_space.normal.z);
		poly++;
	}

		// fix boxes

	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		map_prepare_mesh_poly(mesh,poly);
		poly++;
	}

		// fix mesh box

	map_prepare_mesh_box(mesh);
}

/* =======================================================

      Skew Mesh
      
======================================================= */

void map_mesh_skew(map_type *map,int mesh_idx,int axis,int dir,int size)
{
	int						n,nvertex;
	float					skew_fact,skew_size;
	d3pnt					min,max,sz;
	d3pnt					*pt;
	map_mesh_type			*mesh;

		// get extent

	map_mesh_calculate_extent(map,mesh_idx,&min,&max);

	sz.x=max.x-min.x;
	sz.y=max.y-min.y;
	sz.z=max.z-min.z;
	
	skew_size=(float)size;

		// skew from min to max

	mesh=&map->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
	
			// get the skew direction factor
			
		skew_fact=0.0f;
		
		switch (dir) {
			case 0:
				skew_fact=(float)(pt->x-min.x)/(float)sz.x;
				break;
			case 1:
				skew_fact=(float)(pt->y-min.y)/(float)sz.y;
				break;
			case 2:
				skew_fact=(float)(pt->z-min.z)/(float)sz.z;
				break;
		}
		
			// skew that axis
			
		switch (axis) {
			case 0:
				pt->x+=(int)(skew_size*skew_fact);
				break;
			case 1:
				pt->y+=(int)(skew_size*skew_fact);
				break;
			case 2:
				pt->z+=(int)(skew_size*skew_fact);
				break;
		}

		pt++;
	}
}

/* =======================================================

      Tesselate Mesh
      
======================================================= */

bool map_mesh_tesselate(map_type *map,int mesh_idx)
{
	int						n,k,cnt,ntrig,npoly;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly,*trig_polys,*trig_poly;

	mesh=&map->mesh.meshes[mesh_idx];

	if (mesh->npoly==0) return(TRUE);

		// count number of eventually trigs

	npoly=mesh->npoly;
	poly=mesh->polys;

	ntrig=0;

	for (n=0;n!=npoly;n++) {
		ntrig+=(poly->ptsz-2);
		poly++;
	}

		// create trigs

	trig_polys=(map_mesh_poly_type*)malloc(sizeof(map_mesh_poly_type)*ntrig);
	if (trig_polys==NULL) return(FALSE);

	trig_poly=trig_polys;
	poly=mesh->polys;

	for (n=0;n!=npoly;n++) {

		cnt=poly->ptsz-2;

		for (k=0;k!=cnt;k++) {
			memmove(trig_poly,poly,sizeof(map_mesh_poly_type));

			trig_poly->ptsz=3;

			trig_poly->v[0]=poly->v[0];
			trig_poly->v[1]=poly->v[k+1];
			trig_poly->v[2]=poly->v[k+2];
			
			trig_poly->main_uv.x[0]=poly->main_uv.x[0];
			trig_poly->main_uv.x[1]=poly->main_uv.x[k+1];
			trig_poly->main_uv.x[2]=poly->main_uv.x[k+2];
			
			trig_poly->main_uv.y[0]=poly->main_uv.y[0];
			trig_poly->main_uv.y[1]=poly->main_uv.y[k+1];
			trig_poly->main_uv.y[2]=poly->main_uv.y[k+2];
			
			trig_poly++;
		}

		poly++;
	}

		// substitute the trigs

	free(mesh->polys);

	mesh->npoly=ntrig;
	mesh->polys=trig_polys;

	return(TRUE);
}

/* =======================================================

      Poly Punch Hole
      
======================================================= */

bool map_mesh_poly_punch_hole(map_type *map,int mesh_idx,int poly_idx,d3pnt *extrude_pnt)
{
	int						n,ptsz,mx,my,mz,
							px[8],py[8],pz[8],
							k,kx[4],ky[4],kz[4];
	float					gx[8],gy[8],mgx,mgy,
							k_gx[4],k_gy[4];
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	ptsz=poly->ptsz;

		// new hole vertexes

	mx=my=mz=0;
	mgx=mgy=0.0f;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;

		gx[n]=poly->main_uv.x[n];
		gy[n]=poly->main_uv.y[n];

		mx+=pt->x;
		my+=pt->y;
		mz+=pt->z;

		mgx+=gx[n];
		mgy+=gy[n];
	}

	mx/=ptsz;
	my/=ptsz;
	mz/=ptsz;

	mgx=mgx/(float)ptsz;
	mgy=mgy/(float)ptsz;

	for (n=0;n!=ptsz;n++) {
		px[n]=((px[n]-mx)/2)+mx;
		py[n]=((py[n]-my)/2)+my;
		pz[n]=((pz[n]-mz)/2)+mz;

		gx[n]=((gx[n]-mgx)/2.0f)+mgx;
		gy[n]=((gy[n]-mgy)/2.0f)+mgy;
	}

		// new polygon for each vertex

	for (n=0;n!=ptsz;n++) {

		poly=&mesh->polys[poly_idx];			// add might force memory move, need to always rebuild pointer

		k=n+1;
		if (k==ptsz) k=0;

		pt=&mesh->vertexes[poly->v[n]];
		kx[0]=pt->x;
		ky[0]=pt->y;
		kz[0]=pt->z;

		pt=&mesh->vertexes[poly->v[k]];
		kx[1]=pt->x;
		ky[1]=pt->y;
		kz[1]=pt->z;

		kx[2]=px[k];
		ky[2]=py[k];
		kz[2]=pz[k];

		kx[3]=px[n];
		ky[3]=py[n];
		kz[3]=pz[n];

		k_gx[0]=poly->main_uv.x[n];
		k_gy[0]=poly->main_uv.y[n];

		k_gx[1]=poly->main_uv.x[k];
		k_gy[1]=poly->main_uv.y[k];

		k_gx[2]=gx[k];
		k_gy[2]=gy[k];

		k_gx[3]=gx[n];
		k_gy[3]=gy[n];

		if (map_mesh_add_poly(map,mesh_idx,4,kx,ky,kz,k_gx,k_gy,poly->txt_idx)==-1) return(FALSE);
	}
	
		// extruded polys
		
	if (extrude_pnt!=NULL) {
	
		for (n=0;n!=ptsz;n++) {

			poly=&mesh->polys[poly_idx];			// add might force memory move, need to always rebuild pointer

			k=n+1;
			if (k==ptsz) k=0;

			kx[0]=px[n]+extrude_pnt->x;
			ky[0]=py[n]+extrude_pnt->y;
			kz[0]=pz[n]+extrude_pnt->z;

			kx[1]=px[n];
			ky[1]=py[n];
			kz[1]=pz[n];

			kx[2]=px[k];
			ky[2]=py[k];
			kz[2]=pz[k];

			kx[3]=px[k]+extrude_pnt->x;
			ky[3]=py[k]+extrude_pnt->y;
			kz[3]=pz[k]+extrude_pnt->z;

			k_gx[0]=0.0f;
			k_gy[0]=0.0f;

			k_gx[1]=1.0f;
			k_gy[1]=0.0f;

			k_gx[2]=1.0f;
			k_gy[2]=1.0f;

			k_gx[3]=0.0f;
			k_gy[3]=1.0f;

			if (map_mesh_add_poly(map,mesh_idx,4,kx,ky,kz,k_gx,k_gy,poly->txt_idx)==-1) return(FALSE);
		}
	}

		// finish by deleting original polygon

	return(map_mesh_delete_poly(map,mesh_idx,poly_idx));
}

/* =======================================================

      Tesselate Poly
      
======================================================= */

bool map_mesh_poly_tesselate(map_type *map,int mesh_idx,int poly_idx)
{
	int						n,cnt;
	int						new_poly_idxs[8],px[3],py[3],pz[3];
	float					gx[3],gy[3];
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly,*trig_poly;

	mesh=&map->mesh.meshes[mesh_idx];
	
	poly=&mesh->polys[poly_idx];
	if (poly->ptsz==3) return(TRUE);

		// create trigs for poly
		// we'll make fake trigs and move over
		// data later

	cnt=poly->ptsz-2;
		
	px[0]=px[1]=px[2]=0;
	py[0]=py[1]=py[2]=0;
	pz[0]=pz[1]=pz[2]=0;
	gx[0]=gx[1]=gx[2]=0.0f;
	gy[0]=gy[1]=gy[2]=0.0f;
		
	for (n=0;n!=cnt;n++) {
		new_poly_idxs[n]=map_mesh_add_poly(map,mesh_idx,3,px,py,pz,gx,gy,0);
	}
		
		// build the new polys
		// need to get poly pointer again because
		// memory might have changed
		
	poly=&mesh->polys[poly_idx];

	for (n=0;n!=cnt;n++) {
		trig_poly=&mesh->polys[new_poly_idxs[n]];
		
		memmove(trig_poly,poly,sizeof(map_mesh_poly_type));

		trig_poly->ptsz=3;

		trig_poly->v[0]=poly->v[0];
		trig_poly->v[1]=poly->v[n+1];
		trig_poly->v[2]=poly->v[n+2];
		
		trig_poly->main_uv.x[0]=poly->main_uv.x[0];
		trig_poly->main_uv.x[1]=poly->main_uv.x[n+1];
		trig_poly->main_uv.x[2]=poly->main_uv.x[n+2];
		
		trig_poly->main_uv.y[0]=poly->main_uv.y[0];
		trig_poly->main_uv.y[1]=poly->main_uv.y[n+1];
		trig_poly->main_uv.y[2]=poly->main_uv.y[n+2];
		
		trig_poly++;
	}
	
		// delete the original
		
	map_mesh_delete_poly(map,mesh_idx,poly_idx);

	return(TRUE);
}

/* =======================================================

      Update Meshes for Texture Shifting
      
======================================================= */

inline float map_mesh_poly_run_shifts_single_choord(float f_tick,float shift)
{
	int				i_add;
	float			f_add;

	f_add=(f_tick*0.001f)*shift;
	i_add=(int)f_add;				// keep within 0..1
	return(f_add-((float)i_add));
}

void map_mesh_poly_run_shifts(map_type *map,int tick)
{
	int						n,k,nmesh,npoly;
	float					f_tick;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	f_tick=(float)tick;

		// run through the meshes

	nmesh=map->mesh.nmesh;
	mesh=map->mesh.meshes;

	for (n=0;n!=nmesh;n++) {

			// shiftable?

		if (!mesh->flag.shiftable) {
			mesh++;
			continue;
		}

			// shift polygons

		npoly=mesh->npoly;
		poly=mesh->polys;

		for (k=0;k!=npoly;k++) {

			if (poly->draw.shift_on) {
				poly->draw.x_shift_offset=map_mesh_poly_run_shifts_single_choord(f_tick,poly->x_shift);
				poly->draw.y_shift_offset=map_mesh_poly_run_shifts_single_choord(f_tick,poly->y_shift);
			}

			poly++;
		}

		mesh++;
	}
}

/* =======================================================

      Poly UV Coordinates as Offset/Size
      
======================================================= */

float map_get_texture_reduce_coord(float f)
{
	int			i;

	i=(int)f;
	return(f-(float)i);
}

float map_get_texture_round_coord(float f)
{
	int			i;

	i=(int)(f*100);
	return(((float)i)/100);
}

void map_mesh_get_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float *x_txtoff,float *y_txtoff,float *x_txtfact,float *y_txtfact)
{
	int						n;
	float					gx_min,gx_max,gy_min,gy_max;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_mesh_poly_uv_type	*uv;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	if (!light_map_uv) {
		uv=&poly->main_uv;
	}
	else {
		uv=&poly->lmap_uv;
	}

		// get UV extends

	gx_min=gx_max=uv->x[0];
	gy_min=gy_max=uv->y[0];

	for (n=1;n<poly->ptsz;n++) {
		if (uv->x[n]<gx_min) gx_min=uv->x[n];
		if (uv->x[n]>gx_max) gx_max=uv->x[n];
		if (uv->y[n]<gy_min) gy_min=uv->y[n];
		if (uv->y[n]>gy_max) gy_max=uv->y[n];
	}

		// create boxed coordinates

	*x_txtoff=map_get_texture_round_coord(gx_min);
	*y_txtoff=map_get_texture_round_coord(gy_min);

	*x_txtfact=map_get_texture_round_coord(gx_max-gx_min);
	*y_txtfact=map_get_texture_round_coord(gy_max-gy_min);
}

void map_mesh_set_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float x_txtoff,float y_txtoff,float x_txtfact,float y_txtfact)
{
	int						n;
	float					gx,gy,
							org_x_txtoff,org_y_txtoff,org_x_txtfact,org_y_txtfact;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_mesh_poly_uv_type	*uv;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	if (!light_map_uv) {
		uv=&poly->main_uv;
	}
	else {
		uv=&poly->lmap_uv;
	}

		// get current box coordinates

	map_mesh_get_poly_uv_as_box(map,mesh_idx,poly_idx,light_map_uv,&org_x_txtoff,&org_y_txtoff,&org_x_txtfact,&org_y_txtfact);

		// reset to new coordinates

	for (n=0;n!=poly->ptsz;n++) {
	
		if (uv->x[n]==org_x_txtoff) {
			uv->x[n]=x_txtoff;
		}
		else {
			if (uv->x[n]==(org_x_txtoff+org_x_txtfact)) {
				uv->x[n]=x_txtoff+x_txtfact;
			}
			else {
				gx=uv->x[n]-org_x_txtoff;
				gx/=org_x_txtfact;
				gx*=x_txtfact;
				uv->x[n]=x_txtoff+gx;
			}
		}
		
		if (uv->y[n]==org_y_txtoff) {
			uv->y[n]=y_txtoff;
		}
		else {
			if (uv->y[n]==(org_y_txtoff+org_y_txtfact)) {
				uv->y[n]=y_txtoff+y_txtfact;
			}
			else {
				gy=uv->y[n]-org_y_txtoff;
				gy/=org_y_txtfact;
				gy*=y_txtfact;
				uv->y[n]=y_txtoff+gy;
			}
		}
	}
}

/* =======================================================

      Rotate and Flip UVs
      
======================================================= */

void map_mesh_rotate_poly_uv(map_type *map,int mesh_idx,int poly_idx,int rot_ang)
{
	int						n;
	float					g;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	if (rot_ang==ta_0) return;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
	
		switch (rot_ang) {
		
			case 90:
				g=poly->main_uv.x[n];
				poly->main_uv.x[n]=poly->main_uv.y[n];
				poly->main_uv.y[n]=-g;
				break;
				
			case 180:
				poly->main_uv.x[n]=-poly->main_uv.x[n];
				poly->main_uv.y[n]=-poly->main_uv.y[n];
				break;
				
			case 270:
				g=poly->main_uv.x[n];
				poly->main_uv.x[n]=-poly->main_uv.y[n];
				poly->main_uv.y[n]=g;
				break;
		
		}
	}
}

void map_mesh_flip_poly_uv(map_type *map,int mesh_idx,int poly_idx,bool flip_u,bool flip_v)
{
	int						n;
	float					min_gx,max_gx,min_gy,max_gy,g;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// get min and max
		
	min_gx=max_gx=poly->main_uv.x[0];
	min_gy=max_gy=poly->main_uv.y[0];
	
	for (n=1;n<poly->ptsz;n++) {
		if (poly->main_uv.x[n]<min_gx) min_gx=poly->main_uv.x[n];
		if (poly->main_uv.x[n]>max_gx) max_gx=poly->main_uv.x[n];
		if (poly->main_uv.y[n]<min_gy) min_gy=poly->main_uv.y[n];
		if (poly->main_uv.y[n]>max_gy) max_gy=poly->main_uv.y[n];
	}
	
	if ((flip_u) && (max_gx==min_gx)) return;
	if ((flip_v) && (max_gy==min_gy)) return;
	
		// flip
		
	for (n=0;n!=poly->ptsz;n++) {
		
		if (flip_u) {
			g=1.0f-((poly->main_uv.x[n]-min_gx)/(max_gx-min_gx));
			poly->main_uv.x[n]=min_gx+((max_gx-min_gx)*g);
		}
			
		if (flip_v) {
			g=1.0f-((poly->main_uv.y[n]-min_gy)/(max_gy-min_gy));
			poly->main_uv.y[n]=min_gy+((max_gy-min_gy)*g);
		}
	}
}

/* =======================================================

      Recalculate UVs
      
======================================================= */

void map_get_texture_uv_get_scale(map_type *map,int txt_idx,float *txt_scale_x,float *txt_scale_y)
{
	texture_type		*texture;

	texture=&map->textures[txt_idx];
	if (!texture->scale.on) {
		*txt_scale_x=map->editor_setup.txt_scale_x/144.0f;			// need to reflect original scale from older versions
		*txt_scale_y=map->editor_setup.txt_scale_y/144.0f;
		return;
	}
	
	*txt_scale_x=texture->scale.x/144.0f;
	*txt_scale_y=texture->scale.y/144.0f;
}

void map_mesh_reset_poly_uv(map_type *map,int mesh_idx,int poly_idx)
{
	int						n,kx,ky,kz;
	float					fx,fy,fz,ltxtx,rtxtx,ltxty,rtxty,ltxtz,rtxtz,
							x_txtoff,y_txtoff,x_txtfact,y_txtfact,x_txtfact_2,
							f_dist_1,f_dist_2,txt_scale_x,txt_scale_y;
	double					dx,dz,d;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// get scale

	map_get_texture_uv_get_scale(map,poly->txt_idx,&txt_scale_x,&txt_scale_y);
	
		// setup box and slope
		// needed for texture calculations

	map_prepare_mesh_poly(mesh,poly);

		// walls-like polygons
		
	if (poly->box.wall_like) {
		
		ltxtx=(float)(poly->line.lx+poly->line.lz)*txt_scale_x;
		rtxtx=(float)(poly->line.rx+poly->line.rz)*txt_scale_x;
			
			// get point texture factor
				
		x_txtfact=rtxtx-ltxtx;
			
			// get distance texture factor
				
		dx=(double)(poly->line.rx-poly->line.lx);
		dz=(double)(poly->line.rz-poly->line.lz);
		d=(dx*dx)+(dz*dz);
		x_txtfact_2=(float)(sqrt(d)*txt_scale_x);
		if (x_txtfact<0) x_txtfact_2=-x_txtfact_2;
		
		if (fabs(x_txtfact_2)>fabs(x_txtfact)) x_txtfact=x_txtfact_2;		// if distance calc is longer, use that
		
		x_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx));
		x_txtfact=map_get_texture_round_coord(x_txtfact);
		
		ltxty=((float)poly->box.min.y)*txt_scale_y;
		rtxty=((float)poly->box.max.y)*txt_scale_y;
		
		y_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxty));
		y_txtfact=map_get_texture_round_coord(rtxty-ltxty);
	
			// deal with offset locks
			
		if (map->textures[poly->txt_idx].scale.lock_offset) {
			x_txtoff=0.0f;
			y_txtoff=0.0f;
		}
				
			// create the polygons UVs
			
		dx=(double)(poly->box.max.x-poly->line.lx);
		dz=(double)(poly->box.max.z-poly->line.lz);
		f_dist_2=(float)sqrt((dx*dx)+(dz*dz));
			
		for (n=0;n!=poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];

			dx=(double)(pt->x-poly->line.lx);
			dz=(double)(pt->z-poly->line.lz);
			f_dist_1=(float)sqrt((dx*dx)+(dz*dz));
			
			ky=pt->y-poly->box.min.y;

			fx=f_dist_1/f_dist_2;
			fy=(float)ky/(float)(poly->box.max.y-poly->box.min.y);

			poly->main_uv.x[n]=x_txtoff+(x_txtfact*fx);
			poly->main_uv.y[n]=y_txtoff+(y_txtfact*fy);
		}
		
		return;
	}
			
		// floor-like polygon

	ltxtx=((float)poly->box.min.x)*txt_scale_x;
	rtxtx=((float)poly->box.max.x)*txt_scale_x;

	ltxtz=((float)poly->box.min.z)*txt_scale_y;
	rtxtz=((float)poly->box.max.z)*txt_scale_y;

	x_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx));
	x_txtfact=map_get_texture_round_coord(rtxtx-ltxtx);
	
	y_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtz));
	y_txtfact=map_get_texture_round_coord(rtxtz-ltxtz);
	
		// deal with offset locks
		
	if (map->textures[poly->txt_idx].scale.lock_offset) {
		x_txtoff=0.0f;
		y_txtoff=0.0f;
	}

		// set the polygon UVs
		
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		kx=pt->x-poly->box.min.x;
		kz=pt->z-poly->box.min.z;

		fx=(float)kx/(float)(poly->box.max.x-poly->box.min.x);
		fz=(float)kz/(float)(poly->box.max.z-poly->box.min.z);

		poly->main_uv.x[n]=x_txtoff+(x_txtfact*fx);
		poly->main_uv.y[n]=y_txtoff+(y_txtfact*fz);
	}
}

void map_mesh_reset_uv(map_type *map,int mesh_idx)
{
	int						n,npoly;
	map_mesh_type			*mesh;

	mesh=&map->mesh.meshes[mesh_idx];

	npoly=mesh->npoly;

	for (n=0;n!=npoly;n++) {
		map_mesh_reset_poly_uv(map,mesh_idx,n);
	}
}

/* =======================================================

      Whole Number UVs
      
======================================================= */

void map_mesh_whole_poly_uv(map_type *map,int mesh_idx,int poly_idx)
{
	int						n;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	for (n=0;n!=poly->ptsz;n++) {
		poly->main_uv.x[n]=(float)floor(poly->main_uv.x[n]);
		poly->main_uv.y[n]=(float)floor(poly->main_uv.y[n]);
	}
}

void map_mesh_whole_uv(map_type *map,int mesh_idx)
{
	int						n,npoly;
	map_mesh_type			*mesh;

	mesh=&map->mesh.meshes[mesh_idx];

	npoly=mesh->npoly;

	for (n=0;n!=npoly;n++) {
		map_mesh_whole_poly_uv(map,mesh_idx,n);
	}
}

/* =======================================================

      Single Stamp Number UVs
      
======================================================= */

void map_mesh_single_poly_uv(map_type *map,int mesh_idx,int poly_idx)
{
	int						n;
	float					m_gx,m_gy;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

		// find the middle gx/gy

	m_gx=m_gy=0.0f;

	for (n=0;n!=poly->ptsz;n++) {
		m_gx+=poly->main_uv.x[n];
		m_gy+=poly->main_uv.y[n];
	}

	m_gx=m_gx/(float)poly->ptsz;
	m_gy=m_gy/(float)poly->ptsz;

		// spread the 0-1 around the middle

	for (n=0;n!=poly->ptsz;n++) {

		if (poly->main_uv.x[n]<m_gx) {
			poly->main_uv.x[n]=0.0f;
		}
		else {
			poly->main_uv.x[n]=1.0f;
		}

		if (poly->main_uv.y[n]<m_gy) {
			poly->main_uv.y[n]=0.0f;
		}
		else {
			poly->main_uv.y[n]=1.0f;
		}
	}
}

void map_mesh_single_uv(map_type *map,int mesh_idx)
{
	int						n,npoly;
	map_mesh_type			*mesh;

	mesh=&map->mesh.meshes[mesh_idx];

	npoly=mesh->npoly;

	for (n=0;n!=npoly;n++) {
		map_mesh_single_poly_uv(map,mesh_idx,n);
	}
}

