/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Utility

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
extern int ag_generate_find_floor_polygon(int room_idx);
extern int ag_generate_find_ceiling_polygon(int room_idx);
extern bool ag_generate_is_polygon_window_target(int mesh_idx,int poly_idx);
extern void ag_generate_get_wall_line(int mesh_idx,int poly_idx,d3pnt *p1,d3pnt *p2);

/* =======================================================

      Collisions
      
======================================================= */

bool ag_generate_mesh_collision(d3pnt *min,d3pnt *max,int start_cmp_mesh_idx,int end_cmp_mesh_idx)
{
	int				n;
	d3pnt			cmp_min,cmp_max;

	for (n=start_cmp_mesh_idx;n<end_cmp_mesh_idx;n++) {

		map_mesh_calculate_extent(&map,n,&cmp_min,&cmp_max);
		if ((min->x>=cmp_max.x) || (max->x<=cmp_min.x)) continue;
		if ((min->y>=cmp_max.y) || (max->y<=cmp_min.y)) continue;
		if ((min->z>=cmp_max.z) || (max->z<=cmp_min.z)) continue;
		
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Decoration Locations
      
======================================================= */

void ag_generate_decoration_location_random(int room_idx,d3pnt *pnt)
{
	d3pnt			min,max,sz,margin;
	ag_room_type	*room;

		// get current room dimensions

	room=&ag_state.rooms[room_idx];
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

		// room size and margin

	sz.x=max.x-min.x;
	sz.z=max.z-min.z;

	margin.x=(int)(((float)sz.x)*0.25f);
	margin.z=(int)(((float)sz.z)*0.25f);

		// get random location

	pnt->x=(min.x+margin.x)+(ag_random_int(sz.x-(margin.x*2)));
	pnt->y=max.y;
	pnt->z=(min.z+margin.z)+(ag_random_int(sz.z-(margin.z*2)));
}

void ag_generate_decoration_location_angle(int room_idx,float ang,d3pnt *pnt)
{
	d3pnt			min,max,sz;
	float			fx,fz,rang;
	ag_room_type	*room;

		// get current room dimensions

	room=&ag_state.rooms[room_idx];
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

		// room size and radius

	sz.x=max.x-min.x;
	sz.z=max.z-min.z;

	fx=((float)sz.x)*0.33f;
	fz=((float)sz.z)*0.33f;

		// get point

	pnt->x=(min.x+max.x)>>1;
	pnt->y=max.y;
	pnt->z=(min.z+max.z)>>1;

	rang=ang*ANG_to_RAD;

	pnt->x+=(int)(fx*sinf(rang));
	pnt->z-=(int)(fz*cosf(rang));
}


/* =======================================================

      Box Stack Decorations
      
======================================================= */

void ag_generate_decoration_box(d3pnt *pnt,d3pnt *room_sz,int start_cmp_mesh_idx)
{
	int				n,mesh_idx,x,z,sz;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	bool			hit;
	d3pnt			mpt,min,max;
	d3ang			ang;

		// find a random start position
	
	hit=FALSE;
	sz=ag_size_stack_box>>1;

	for (n=0;n!=10;n++) {
		x=pnt->x+(ag_random_int(room_sz->x)-(room_sz->x>>1));
		z=pnt->z+(ag_random_int(room_sz->z)-(room_sz->z>>1));

		min.x=x-sz;
		max.x=x+sz;
		min.y=pnt->y-ag_size_stack_box;
		max.y=pnt->y;
		min.z=z-sz;
		max.z=z+sz;

		if (start_cmp_mesh_idx!=-1) {
			if (!ag_generate_mesh_collision(&min,&max,start_cmp_mesh_idx,map.mesh.nmesh)) {
				hit=TRUE;
				break;
			}
		}
	}

	if (!hit) return;

		// box mesh

	mesh_idx=map_mesh_add(&map);

	map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=min.x;
	pz[0]=pz[3]=min.z;
	pz[1]=pz[2]=max.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	px[0]=px[1]=px[2]=px[3]=max.x;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[1]=pz[2]=pz[3]=min.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	pz[0]=pz[1]=pz[2]=pz[3]=max.z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

		// top and bottom

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[1]=min.z;
	pz[2]=pz[3]=max.z;
	py[0]=py[1]=py[2]=py[3]=max.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	py[0]=py[1]=py[2]=py[3]=min.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

		// possibly rotate box

	if (ag_random_bool()) {
		ang.x=ang.z=0.0f;
		ang.y=(float)ag_random_int(30);

		map_mesh_calculate_center(&map,mesh_idx,&mpt);
		map_mesh_rotate(&map,mesh_idx,&mpt,&ang);
	}

		// reset just normals
		// as UVs are locked

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_decoration_box_stacks(int room_idx)
{
	int				n,row_count,high_count,cmp_mesh_idx;
	bool			horz;
	d3pnt			pnt,room_sz,min,max;
	ag_room_type	*room;

		// get random stack count

	row_count=ag_count_stack_box_row_start+ag_random_int(ag_count_stack_box_row_extra);

		// randomize location
		// move it so within stack of boxes

	room=&ag_state.rooms[room_idx];

	map_mesh_calculate_center(&map,room->mesh_idx,&pnt);
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

	pnt.y=max.y;

	room_sz.x=max.x-min.x;
	room_sz.z=max.z-min.z;

	if (room_sz.x>room_sz.z) {
		horz=TRUE;
		room_sz.x=room_sz.x/3;
		room_sz.z=ag_size_stack_box*3;
	}
	else {
		horz=FALSE;
		room_sz.x=ag_size_stack_box*3;
		room_sz.z=room_sz.z/3;
	}

		// box stack

	high_count=0;
	cmp_mesh_idx=map.mesh.nmesh;

	while (row_count>ag_count_stack_box_row_min) {

		for (n=0;n!=row_count;n++) {
			ag_generate_decoration_box(&pnt,&room_sz,cmp_mesh_idx);
		}

		if (horz) {
			room_sz.x-=ag_size_stack_box;
			if (room_sz.x<ag_size_stack_box) break;
		}
		else {
			room_sz.z-=ag_size_stack_box;
			if (room_sz.z<ag_size_stack_box) break;
		}

		row_count-=2;
		pnt.y-=ag_size_stack_box;
		
		high_count++;
		if (high_count>ag_count_stack_box_high_max) break;
	}
}

/* =======================================================

      Column Decorations
      
======================================================= */

void ag_generate_decoration_column(d3pnt *pnt,int radius,int high,bool circular)
{
	int				n,mesh_idx;
	int				px[4],py[4],pz[4];
	float			ang,next_ang,ang_add,f_radius;
	float			gx[4],gy[4];

		// the mesh

	mesh_idx=map_mesh_add(&map);

		// column

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// square columns

	if (!circular) {

		py[0]=py[1]=pnt->y-high;
		py[2]=py[3]=pnt->y;

		px[0]=px[1]=px[2]=px[3]=pnt->x-radius;
		pz[0]=pz[3]=pnt->z-radius;
		pz[1]=pz[2]=pnt->z+radius;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

		px[0]=px[1]=px[2]=px[3]=pnt->x+radius;
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

		px[0]=px[3]=pnt->x-radius;
		px[1]=px[2]=pnt->x+radius;
		pz[0]=pz[1]=pz[2]=pz[3]=pnt->z-radius;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

		pz[0]=pz[1]=pz[2]=pz[3]=pnt->z+radius;
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);
	}

		// circular columns

	else {

		ang=0.0f;
		ang_add=(2.0f*TRIG_PI)/10.0f;

		f_radius=(float)radius;

		for (n=0;n!=10;n++) {

			next_ang=ang+ang_add;
			if (n==9) next_ang=0.0f;

			px[0]=px[3]=pnt->x+(int)(f_radius*sinf(ang));
			px[1]=px[2]=pnt->x+(int)(f_radius*sinf(next_ang));
			pz[0]=pz[3]=pnt->z-(int)(f_radius*cosf(ang));
			pz[1]=pz[2]=pnt->z-(int)(f_radius*cosf(next_ang));

			py[0]=py[1]=pnt->y-high;
			py[2]=py[3]=pnt->y;

			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

				// exit early if we reconnect early

			ang=next_ang;
		}
	}

		// reset UVs and normals

	map_mesh_reset_uv(&map,mesh_idx);
	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_decoration_columns(int room_idx)
{
	int				high,
					ang,start_ang,end_ang,ang_add;
	d3pnt			pnt;

		// get height

	high=ag_size_room_high+ag_size_floor_high;
	if (ag_state.rooms[room_idx].second_story) high+=ag_size_room_high;

		// create columns

	switch (ag_random_int(3)) {

		case ag_decoration_column_config_square:
			start_ang=45;
			end_ang=315;
			ang_add=90;
			break;

		case ag_decoration_column_config_diamond:
			start_ang=0;
			end_ang=270;
			ang_add=90;
			break;

		case ag_decoration_column_config_circle:
			start_ang=0;
			end_ang=315;
			ang_add=45;
			break;
	
	}
	
	for (ang=start_ang;ang<=end_ang;ang+=ang_add) {
		ag_generate_decoration_location_angle(room_idx,(float)ang,&pnt);
		ag_generate_decoration_column(&pnt,ag_size_column_normal_wid,high,TRUE);
	}
}

/* =======================================================

      Equipment Decorations
      
======================================================= */

void ag_generate_decoration_equipment_piece(d3pnt *pnt,int wid_x,int wid_z,int high,int texture_idx)
{
	int				mesh_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// equipment mesh

	mesh_idx=map_mesh_add(&map);

	map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=pnt->x;
	pz[0]=pz[3]=pnt->z;
	pz[1]=pz[2]=pnt->z+wid_z;
	py[0]=py[1]=pnt->y-high;
	py[2]=py[3]=pnt->y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	px[0]=px[1]=px[2]=px[3]=pnt->x+wid_x;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	px[0]=px[3]=pnt->x;
	px[1]=px[2]=pnt->x+wid_x;
	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z+wid_z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

		// top

	px[0]=px[3]=pnt->x;
	px[1]=px[2]=pnt->x+wid_x;
	pz[0]=pz[1]=pnt->z;
	pz[2]=pz[3]=pnt->z+wid_z;
	py[0]=py[1]=py[2]=py[3]=pnt->y-high;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

		// reset just normals
		// as UVs are locked

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_decoration_equipment(int room_idx)
{
	int				n,high,
					dist,count,npoly,poly_idx;
	float			fx,fz;
	d3pnt			pnt,p1,p2;
	map_mesh_type	*mesh;
	ag_room_type	*room;

	room=&ag_state.rooms[room_idx];

		// get height

	high=ag_size_equipment_high_start+ag_random_int(ag_size_equipment_high_extra);

		// run through the polygons and find
		// walls we can put equipment against

	npoly=map.mesh.meshes[room->mesh_idx].npoly;

	for (poly_idx=0;poly_idx!=npoly;poly_idx++) {
			
		mesh=&map.mesh.meshes[room->mesh_idx];		// pointers might change as meshes are being added
		poly_idx=ag_random_int(mesh->npoly);
				
		if (!ag_generate_is_polygon_window_target(room->mesh_idx,poly_idx)) continue;

			// get distance and determine
			// count

		ag_generate_get_wall_line(room->mesh_idx,poly_idx,&p1,&p2);

		fx=(float)(p2.x-p1.x);
		fz=(float)(p2.z-p1.z);
		dist=(int)sqrtf((fx*fx)+(fz*fz));

		if (dist<(ag_size_equipment_pad_wid*2)) continue;

		count=dist/(ag_size_equipment_pad_wid*2);
		if (count<0) count=1;

			// equipment boxes

		for (n=0;n!=count;n++) {

			pnt.x=p1.x+(int)(((float)(p2.x-p1.x))*(((float)(n+1))/((float)(count+1))));
			pnt.y=room->max.y;
			pnt.z=p1.z+(int)(((float)(p2.z-p1.z))*(((float)(n+1))/((float)(count+1))));

				// pad

			pnt.x-=(ag_size_equipment_pad_wid>>1);
			pnt.z-=(ag_size_equipment_pad_wid>>1);

			ag_generate_decoration_equipment_piece(&pnt,ag_size_equipment_pad_wid,ag_size_equipment_pad_wid,ag_size_floor_high,ag_texture_connect);

				// computer

			pnt.x+=((ag_size_equipment_pad_wid-ag_size_equipment_computer_wid)>>1);
			pnt.y-=ag_size_floor_high;
			pnt.z+=((ag_size_equipment_pad_wid-ag_size_equipment_computer_wid)>>1);

			ag_generate_decoration_equipment_piece(&pnt,ag_size_equipment_computer_wid,ag_size_equipment_computer_wid,high,ag_texture_equipment);
		}
	}

		// these count as wall decorations,
		// so we avoid vertex decorations

	room->has_wall_decorations=TRUE;
}

/* =======================================================

      Core Decorations
      
======================================================= */

void ag_generate_decoration_core(int room_idx)
{
	int					n,extrude_mesh_idx,
						poly_idx;
	d3pnt				extrude_pnt;
	ag_room_type		*room;

	room=&ag_state.rooms[room_idx];

		// find the ceiling

	poly_idx=ag_generate_find_floor_polygon(room_idx);
	if (poly_idx==-1) return;

		// extrude it

	extrude_pnt.x=0;
	extrude_pnt.y=-(ag_size_room_high+ag_size_floor_high);
	extrude_pnt.z=0;

		// extrude it

	extrude_mesh_idx=map_mesh_poly_punch_hole(&map,room->mesh_idx,poly_idx,&extrude_pnt,FALSE,normal_mode_out);
	if (extrude_mesh_idx==-1) return;

	for (n=0;n!=map.mesh.meshes[extrude_mesh_idx].npoly;n++) {
		map.mesh.meshes[extrude_mesh_idx].polys[n].txt_idx=ag_texture_wall;
	}
	
	map_mesh_reset_uv(&map,extrude_mesh_idx);
}

/* =======================================================

      Floor Trench Decorations
      
======================================================= */

void ag_generate_decoration_floor_trench(int room_idx)
{
	int					n,depth_count,
						mesh_idx,poly_idx,extrude_mesh_idx;
	d3pnt				extrude_pnt;
	ag_room_type		*room;

	room=&ag_state.rooms[room_idx];

		// find the floor

	poly_idx=ag_generate_find_floor_polygon(room_idx);
	if (poly_idx==-1) return;

		// run to random depth

	mesh_idx=room->mesh_idx;
	depth_count=1+ag_random_int(2);

	for (n=0;n!=depth_count;n++) {

			// extrude it

		extrude_pnt.x=0;
		extrude_pnt.y=ag_random_bool()?ag_size_trench_high:-ag_size_trench_high;
		extrude_pnt.z=0;

			// extrude it

		extrude_mesh_idx=map_mesh_poly_punch_hole(&map,mesh_idx,poly_idx,&extrude_pnt,TRUE,((extrude_pnt.y>0)?normal_mode_in:normal_mode_out));
		if (extrude_mesh_idx==-1) break;
		
		map_mesh_reset_uv(&map,extrude_mesh_idx);

			// last polygon in extrude mesh is always
			// the new extrude

		mesh_idx=extrude_mesh_idx;
		poly_idx=map.mesh.meshes[mesh_idx].npoly-1;
	}
}

/* =======================================================

      Open Ceiling Decorations
      
======================================================= */

void ag_decoration_open_ceiling(int room_idx)
{
	int					extrude_mesh_idx,poly_idx;
	d3pnt				extrude_pnt;
	ag_room_type		*room;

	room=&ag_state.rooms[room_idx];

		// find the ceiling

	poly_idx=ag_generate_find_ceiling_polygon(room_idx);
	if (poly_idx==-1) return;

		// extrude it

	extrude_pnt.x=0;
	extrude_pnt.y=-(ag_size_floor_high*2);
	extrude_pnt.z=0;

		// extrude it

	extrude_mesh_idx=map_mesh_poly_punch_hole(&map,room->mesh_idx,poly_idx,&extrude_pnt,FALSE,normal_mode_in);
	if (extrude_mesh_idx!=-1) map_mesh_reset_uv(&map,extrude_mesh_idx);
}

/* =======================================================

      Vertex Decorations
      
======================================================= */

void ag_gnerate_decoration_vertex_column(int room_idx,bool circular)
{
	int				n,high,radius;
	ag_room_type	*room;

	room=&ag_state.rooms[room_idx];

		// get height and size

	high=ag_size_room_high+ag_size_floor_high;
	if (ag_state.rooms[room_idx].second_story) high+=ag_size_room_high;

	radius=ag_size_column_vertex_wid_start+ag_random_int(ag_size_column_vertex_wid_extra);

		// vertex columns

	for (n=0;n!=room->nvertex;n++) {
		ag_generate_decoration_column(&room->vertexes[n],radius,high,circular);
	}
}

/* =======================================================

      Decorations
      
======================================================= */

void ag_generate_decorations_add(void)
{
	int				n,k,dec_idx;
	ag_room_type	*room;

	for (n=0;n!=ag_state.nroom;n++) {
		room=&ag_state.rooms[n];

			// try a couple of times
			// to get the right decoration for
			// a room, skip any room with stairs

		dec_idx=ag_decoration_type_none;

		if (!room->has_stairs) {
			for (k=0;k!=10;k++) {
				dec_idx=ag_random_int(5);

				switch (dec_idx) {

						// columns and equipment
						// don't appear in two story rooms

					case ag_decoration_type_columns:
						if (room->second_story) continue;
						break;

					case ag_decoration_type_equipment:
						if (room->second_story) continue;
						break;

						// core decoration requires window light
						// and single story rooms

					case ag_decoration_type_core:
						if ((!room->has_windows) || (room->second_story)) continue;
						break;

				}

				break;
			}
		}

			// middle decorations

		switch (dec_idx) {

			case ag_decoration_type_columns:
				ag_generate_decoration_columns(n);
				break;

			case ag_decoration_type_box_stacks:
				ag_generate_decoration_box_stacks(n);
				break;

			case ag_decoration_type_equipment:
				ag_generate_decoration_equipment(n);
				break;

			case ag_decoration_type_core:
				ag_generate_decoration_core(n);
				break;

			case ag_decoration_type_trench:
				ag_generate_decoration_floor_trench(n);
				break;

		}

			// vertex decorations
			// skip if already things on wall

		if (!room->has_wall_decorations) {

			switch (ag_random_int(3)) {

				case ag_decoration_type_vertex_none:
					break;

				case ag_decoration_type_vertex_column_circle:
					ag_gnerate_decoration_vertex_column(n,TRUE);
					break;

				case ag_decoration_type_vertex_column_square:
					ag_gnerate_decoration_vertex_column(n,FALSE);
					break;

			}
		}

			// ceiling decorations

		if ((room->second_story) && (room->has_windows)) {
			if (ag_random_bool()) {
				ag_decoration_open_ceiling(n);
			}
		}
	}
}

/* =======================================================

      Add Lights
      
======================================================= */

void ag_generate_lights_add_fixture(d3pnt *pnt)
{
	int				mesh_idx,ty,by;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// get top and bottom y

	ty=pnt->y-(ag_size_floor_high*3);
	by=ty+ag_size_floor_high;

		// fixture mesh

	mesh_idx=map_mesh_add(&map);

	map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=pnt->x-ag_size_light_fixture_wid;
	pz[0]=pz[3]=pnt->z-ag_size_light_fixture_wid;
	pz[1]=pz[2]=pnt->z+ag_size_light_fixture_wid;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

	px[0]=px[1]=px[2]=px[3]=pnt->x+ag_size_light_fixture_wid;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

	px[0]=px[3]=pnt->x-ag_size_light_fixture_wid;
	px[1]=px[2]=pnt->x+ag_size_light_fixture_wid;
	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z-ag_size_light_fixture_wid;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z+ag_size_light_fixture_wid;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

		// bottom

	px[0]=px[3]=pnt->x-ag_size_light_fixture_wid;
	px[1]=px[2]=pnt->x+ag_size_light_fixture_wid;
	pz[0]=pz[1]=pnt->z-ag_size_light_fixture_wid;
	pz[2]=pz[3]=pnt->z+ag_size_light_fixture_wid;
	py[0]=py[1]=py[2]=py[3]=by;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

		// reset UVs and normals

	map_mesh_reset_uv(&map,mesh_idx);
	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_lights_add_spot(d3pnt *pnt)
{
	map_light_type	*lit;

		// exit if we run out of lights

	if (map.nlight==max_map_light) return;
		
		// add the light

	lit=&map.lights[map.nlight];
	sprintf(lit->name,"light %d",map.nlight);

	map.nlight++;
			
		// set the light

	memmove(&lit->pnt,pnt,sizeof(d3pnt));
	
	lit->setting.on=TRUE;
	lit->setting.light_map=TRUE;
	lit->setting.halo_name[0]=0x0;

	lit->setting.type=lt_normal;
	lit->setting.direction=ld_all;

	lit->setting.intensity=ag_size_light_width_start+ag_random_int(ag_size_light_width_extra);
	lit->setting.exponent=1.0f;

	lit->setting.col.r=1.0f-(((float)ag_random_int(25))/100.0f);
	lit->setting.col.g=1.0f-(((float)ag_random_int(25))/100.0f);
	lit->setting.col.b=1.0f-(((float)ag_random_int(25))/100.0f);
}

void ag_generate_lights_add_room_light(ag_room_type *room,int y_add)
{
	d3pnt			pnt,min,max;

		// add the light

	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

	pnt.x=(min.x+max.x)>>1;
	pnt.y=max.y-(ag_size_room_high-(ag_size_floor_high<<1));
	if (room->second_story) pnt.y-=ag_size_room_high;
	pnt.y+=y_add;
	pnt.z=(min.z+max.z)>>1;

	ag_generate_lights_add_spot(&pnt);

		// add the fixture

	ag_generate_lights_add_fixture(&pnt);
}

void ag_generate_lights_add(void)
{
	int				n;
	ag_room_type	*room;

	for (n=0;n!=ag_state.nroom;n++) {

			// skip rooms that are lit
			// by windows

		room=&ag_state.rooms[n];
		if (room->has_windows) continue;

			// add the light
			// and a second one if a complete
			// two story room

		ag_generate_lights_add_room_light(room,0);
		if (room->second_story_complete) ag_generate_lights_add_room_light(room,(ag_size_room_high+ag_size_floor_high));
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

void ag_generate_spots_add_single(char *name,int spot_obj_type,char *script_name,d3pnt *rtn_pnt)
{
	int					room_idx;
	spot_type			*spot;

		// add the spot

	spot=&map.spots[map.nspot];
	map.nspot++;

		// randomize location

	room_idx=ag_random_int(ag_state.nroom);
	ag_generate_decoration_location_random(room_idx,&spot->pnt);

	spot->ang.x=0.0f;
	spot->ang.y=0.0f;
	spot->ang.z=0.0f;

		// name and type
	
	strcpy(spot->name,name);
	spot->type=spot_obj_type;
	strcpy(spot->script,script_name);
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;

		// return point

	if (rtn_pnt!=NULL) memmove(rtn_pnt,&spot->pnt,sizeof(d3pnt));
}

//
// supergumba -- add spots to ROOMs (randomly) ignoring
// rooms with stairs, and find a FLOOR segment in the room and
// put in middle
//

void ag_generate_spots_add(void)
{
	int				n;

		// player spot

	ag_generate_spots_add_single("Start",spot_type_player,"Player",&ag_state.start_pnt);

		// monster spots
		// supergumba -- hard coded!  FIX!

	for (n=0;n!=(ag_state.nroom*2);n++) {
	//	ag_generate_spots_add_single("Monster",spot_type_object,"Cyborg Knife",NULL);
	}

}