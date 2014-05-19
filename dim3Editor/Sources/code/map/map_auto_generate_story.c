/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Room Second Story

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;

extern ag_state_type			ag_state;

extern int ag_random_int(int max);
extern bool ag_random_bool(void);
extern void ag_generate_remove_polygons_in_box(int mesh_idx,d3pnt *min,d3pnt *max);
extern int ag_generate_find_floor_polygon(int room_idx);
extern void ag_generate_add_connector_room_step(d3pnt *min,d3pnt *max);

/* =======================================================

      Second Story Steps
      
======================================================= */

void ag_generate_add_room_second_story_steps(d3pnt *min,d3pnt *max,d3pnt *mov)
{
	int				n,step_high,step_count;

	step_high=(ag_size_room_high+ag_size_floor_high)/ag_size_stair_count;
	step_count=ag_size_stair_count-1;

	for (n=0;n!=step_count;n++) {

			// down one

		min->y+=step_high;
		if (min->y>=max->y) break;

			// step length

		if (mov->x!=0) {
			if (mov->x<0) {
				min->x=max->x+mov->x;
			}
			else {
				max->x=min->x+mov->x;
			}
		}

		if (mov->z!=0) {
			if (mov->z<0) {
				min->z=max->z+mov->z;
			}
			else {
				max->z=min->z+mov->z;
			}
		}

		ag_generate_add_connector_room_step(min,max);

			// next step

		min->x+=mov->x;
		max->x+=mov->x;
		min->z+=mov->z;
		max->z+=mov->z;
	}
}

/* =======================================================

      Second Story Chunks
      
======================================================= */

void ag_generate_add_room_second_story_chunk(int room_idx,int mesh_idx,int poly_idx,int start_vertex_idx,int end_vertex_idx,int y_add)
{
	int						n,k,t,t2,ptsz,mx,my,mz,
							story_mesh_idx,story_poly_idx;
	int						px[8],py[8],pz[8],
							kx[4],ky[4],ky2[4],kz[4],
							wx[4],wy[4],wz[4];
	float					gx[8],gy[8],mgx,mgy,
							k_gx[4],k_gy[4];
	bool					no_stairs;
	d3pnt					step_min,step_max,step_mov;
	d3pnt					*pt;
	ag_room_type			*room;
	map_mesh_type			*mesh,*story_mesh;
	map_mesh_poly_type		*poly;

	room=&ag_state.rooms[room_idx];

	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	ptsz=poly->ptsz;

		// randomly eliminate
		// steps from some rooms

	no_stairs=ag_random_bool();

		// new hole vertexes

	mx=my=mz=0;
	mgx=mgy=0.0f;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;

		gx[n]=poly->main_uv.uvs[n].x;
		gy[n]=poly->main_uv.uvs[n].y;

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

		// new mesh for each vertex

	for (n=start_vertex_idx;n<=end_vertex_idx;n++) {
		
		story_mesh_idx=map_mesh_add(&map);
		if (story_mesh_idx==-1) return;

		story_mesh=&map.mesh.meshes[story_mesh_idx];

			// get internal points
		
		mesh=&map.mesh.meshes[mesh_idx];			// adding a mesh could change these pointers
		poly=&mesh->polys[poly_idx];

		k=n+1;
		if (k==ptsz) k=0;

		pt=&mesh->vertexes[poly->v[n]];
		kx[0]=pt->x;
		ky[0]=pt->y+y_add;
		ky2[0]=ky[0]+ag_size_floor_high;
		kz[0]=pt->z;

		pt=&mesh->vertexes[poly->v[k]];
		kx[1]=pt->x;
		ky[1]=pt->y+y_add;
		ky2[1]=ky[1]+ag_size_floor_high;
		kz[1]=pt->z;

		kx[2]=px[k];
		ky[2]=py[k]+y_add;
		ky2[2]=ky[2]+ag_size_floor_high;
		kz[2]=pz[k];

		kx[3]=px[n];
		ky[3]=py[n]+y_add;
		ky2[3]=ky[3]+ag_size_floor_high;
		kz[3]=pz[n];

			// walls

		for (t=0;t!=4;t++) {

			t2=t+1;
			if (t2==4) t2=0;

			wx[0]=kx[t];
			wy[0]=ky[t];
			wz[0]=kz[t];

			wx[1]=kx[t2];
			wy[1]=ky[t2];
			wz[1]=kz[t2];

			wx[2]=kx[t2];
			wy[2]=ky2[t2];
			wz[2]=kz[t2];

			wx[3]=kx[t];
			wy[3]=ky2[t];
			wz[3]=kz[t];

			k_gx[0]=0.0f;
			k_gy[0]=0.0f;

			k_gx[1]=1.0f;
			k_gy[1]=0.0f;

			k_gx[2]=1.0f;
			k_gy[2]=1.0f;

			k_gx[3]=0.0f;
			k_gy[3]=1.0f;

			map_mesh_add_poly(&map,story_mesh_idx,4,wx,wy,wz,k_gx,k_gy,ag_texture_connect);
		}

			// floor and ceiling

		k_gx[0]=poly->main_uv.uvs[n].x;
		k_gy[0]=poly->main_uv.uvs[n].y;

		k_gx[1]=poly->main_uv.uvs[k].x;
		k_gy[1]=poly->main_uv.uvs[k].y;

		k_gx[2]=gx[k];
		k_gy[2]=gy[k];

		k_gx[3]=gx[n];
		k_gy[3]=gy[n];

		story_poly_idx=map_mesh_add_poly(&map,story_mesh_idx,4,kx,ky,kz,k_gx,k_gy,ag_texture_connect);
		story_poly_idx=map_mesh_add_poly(&map,story_mesh_idx,4,kx,ky2,kz,k_gx,k_gy,ag_texture_connect);

			// recalc UVs and normals

		map_mesh_reset_uv(&map,story_mesh_idx);
		map_recalc_normals_mesh(&map,story_mesh,normal_mode_out,FALSE);

			// if this segment is straight,
			// then make it a stair

		if (no_stairs) continue;
		if (room->has_stairs) continue;

			// setup step dimensions

		step_min.y=pt->y+y_add;
		step_max.y=step_min.y+(ag_size_room_high+ag_size_floor_high);
		
		step_mov.x=0;
		step_mov.y=0;
		step_mov.z=0;

			// steps across the X

		if (px[n]==px[k]) {
			room->has_stairs=TRUE;

			step_min.x=step_max.x=px[n];

			if (pz[n]<pz[k]) {
				step_min.z=pz[n];
				step_max.z=pz[k];
			}
			else {
				step_min.z=pz[k];
				step_max.z=pz[n];
			}

			step_mov.x=(px[n]<mx)?ag_size_story_stair_tread_len:-ag_size_story_stair_tread_len;

			ag_generate_add_room_second_story_steps(&step_min,&step_max,&step_mov);
			continue;
		}

			// steps across the Z

		if (pz[n]==pz[k]) {
			room->has_stairs=TRUE;

			if (px[n]<px[k]) {
				step_min.x=px[n];
				step_max.x=px[k];
			}
			else {
				step_min.x=px[k];
				step_max.x=px[n];
			}

			step_min.z=step_max.z=pz[n];

			step_mov.z=(pz[n]<mz)?ag_size_story_stair_tread_len:-ag_size_story_stair_tread_len;

			ag_generate_add_room_second_story_steps(&step_min,&step_max,&step_mov);
			continue;
		}
	}
}

/* =======================================================

      Add Room Second Story Pieces
      
======================================================= */

void ag_generate_add_room_second_story(void)
{
	int				n,poly_idx,
					start_vertex_idx,end_vertex_idx;
	ag_room_type	*room;

		// build second stories

	for (n=0;n!=ag_state.nroom;n++) {

			// did this room have
			// a second story

		room=&ag_state.rooms[n];
		if (!room->second_story) continue;
		if (!room->require_top_floor) continue;

			// needs more than 3 vertexes

		if (room->nvertex<=3) continue;

			// get the floor to base
			// second story on

		poly_idx=ag_generate_find_floor_polygon(n);
		if (poly_idx==-1) continue;

			// determine which vertexes
			// to build around

		start_vertex_idx=0;
		end_vertex_idx=ag_random_int(map.mesh.meshes[room->mesh_idx].polys[poly_idx].ptsz);

		ag_generate_add_room_second_story_chunk(n,room->mesh_idx,poly_idx,start_vertex_idx,end_vertex_idx,-(ag_size_room_high+ag_size_floor_high));
	}
}
