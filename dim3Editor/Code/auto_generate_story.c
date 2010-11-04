/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Additional Stories

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
#include "view.h"

extern map_type					map;

extern ag_state_type			ag_state;

extern int ag_shape_get_connector_index(ag_shape_type *shape,int v1_idx,int v2_idx);

/* =======================================================

      Auto Generate Room Utilities
      
======================================================= */

bool ag_room_touch_other_room(int room_idx)
{
	int				n;
	ag_room_type	*room,*chk_room;

	room=&ag_state.rooms[room_idx];

	for (n=0;n!=ag_state.nroom;n++) {
		if (n==room_idx) continue;

			// skip corridors

		chk_room=&ag_state.rooms[n];
		if (chk_room->corridor) continue;

			// check for collisions

		if (((chk_room->min.z>=room->min.z) && (chk_room->min.z<=room->max.z)) || ((chk_room->max.z>=room->min.z) && (chk_room->max.z<=room->max.z))) {
			if (chk_room->min.x==room->max.x) return(TRUE);
			if (chk_room->max.x==room->min.x) return(TRUE);
		}

		if (((chk_room->min.x>=room->min.x) && (chk_room->min.x<=room->max.x)) || ((chk_room->max.x>=room->min.x) && (chk_room->max.x<=room->max.x))) {
			if (chk_room->min.z==room->max.z) return(TRUE);
			if (chk_room->max.z==room->min.z) return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Build Additional Stories
      
======================================================= */

void ag_generate_additional_stories(void)
{
	int					n,k,t,ty,by,mesh_idx,connect_idx;
	int					px[4],py[4],pz[4];
	float				gx[4],gy[4];
	d3vct				*size;
	ag_shape_type		*shape;
	ag_room_type		*room;

		// find all the rooms that
		// are connected to another room
		// (non-corridor)  All these get
		// additional stories

	for (n=0;n!=ag_state.nroom;n++) {

			// skip corridors

		room=&ag_state.rooms[n];
		if (room->corridor) continue;

			// is it touching another room?

		if (!ag_room_touch_other_room(n)) continue;

			// get height

		shape=&ag_state.shapes[room->shape_idx];

		size=&room->size;

		by=room->min.y;
		ty=by-(int)(100.0f*size->y);

			// build next story walls
		
		mesh_idx=map_mesh_add(&map);

		for (k=0;k!=shape->nvertex;k++) {
			t=k+1;
			if (t==shape->nvertex) t=0;

				// is this a connector?

			connect_idx=ag_shape_get_connector_index(shape,k,t);

				// build the wall

			px[0]=px[3]=(int)(((float)shape->vertexes[k].x)*size->x)+room->min.x;
			px[1]=px[2]=(int)(((float)shape->vertexes[t].x)*size->x)+room->min.x;
			pz[0]=pz[3]=(int)(((float)shape->vertexes[k].z)*size->z)+room->min.z;
			pz[1]=pz[2]=(int)(((float)shape->vertexes[t].z)*size->z)+room->min.z;
			py[0]=py[1]=ty;
			py[2]=py[3]=by;

			gx[0]=gx[3]=0.0f;
			gx[1]=gx[2]=1.0f;
			gy[0]=gy[1]=0.0f;
			gy[2]=gy[3]=1.0f;
		
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,((connect_idx!=-1)?4:0));
		}

			// move up the room min

		room->multi_story=TRUE;
		room->min.y=ty;
	}
}
