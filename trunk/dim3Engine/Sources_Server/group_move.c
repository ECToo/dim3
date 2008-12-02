/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Group Movements

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

#include "scripts.h"
#include "objects.h"
#include "effects.h"
#include "consoles.h"

extern map_type			map;
extern js_type			js;

extern bool map_movement_next_move(int movement_idx,attach_type *attach);

/* =======================================================

      Start Group Movement
      
======================================================= */

bool group_move_start(int group_idx,int movement_idx,d3pnt *mov,d3ang *rot,int count,int user_id,bool main_move)
{
	int					n,unit_cnt;
	float				f_count;
	char				err_str[256];
	group_type			*group;
	group_move_type		*move;
	group_unit_type		*unit_list;

	group=&map.groups[group_idx];

		// no negative move counts
		
	if (count<=0) {
		sprintf(err_str,"Attemping to move group '%s' with negative time count",group->name);
		console_add_error(err_str);
		return(FALSE);
	}
	
		// check for certain error states
		// this check is a little bit heavy but these are
		// common errors with users that are hard to track down

	unit_cnt=group->unit_count;

	if (unit_cnt==0) {
		sprintf(err_str,"Attemping to move group '%s' which has no meshes attached to it",group->name);
		console_add_error(err_str);
		return(FALSE);
	}
	
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {
		if (unit_list->type==group_type_mesh) {
			if (!map.mesh.meshes[unit_list->idx].flag.moveable) {
				sprintf(err_str,"Attemping to move a mesh in group '%s' which does not have the moveable flag set",group->name);
				console_add_error(err_str);
				return(FALSE);
			}
		}
		unit_list++;
	}
	
		// setup move over time
		
	move=&group->move;

	move->count=count;
	
	f_count=(float)move->count;
	move->mov_add.x=((float)mov->x)/f_count;
	move->mov_add.y=((float)mov->y)/f_count;
	move->mov_add.z=((float)mov->z)/f_count;

	move->rot_add.x=rot->x/f_count;
	move->rot_add.y=rot->y/f_count;
	move->rot_add.z=rot->z/f_count;

	move->cur_mov.x=move->cur_mov.y=move->cur_mov.z=0.0f;
	
	move->has_mov=((mov->x!=0) || (mov->y!=0) || (mov->z!=0));
	move->has_rot=((rot->x!=0.0f) || (rot->y!=0.0f) || (rot->z!=0.0f));
		
	move->movement_idx=movement_idx;
	move->user_id=user_id;
	
	move->main_move=main_move;
	
	move->freeze=FALSE;
	move->on=TRUE;
	
		// the movement attach is a duplicate of the script
		// attach, this is done to fix some references in header files

	memmove(&move->attach,&js.attach,sizeof(attach_type));

	return(TRUE);
}

void group_move_clear_all(void)
{
	int				n;
	group_type		*group;

	group=map.groups;

	for (n=0;n!=map.ngroup;n++) {
		group->move.on=FALSE;
		group++;
	}
}

/* =======================================================

      Freeze and UnFreeze
      
======================================================= */

void group_move_freeze(int group_idx,bool freeze)
{
	map.groups[group_idx].move.freeze=freeze;
}

bool group_move_frozen(int group_idx)
{
	return(map.groups[group_idx].move.freeze);
}

/* =======================================================

      Move Groups
      
======================================================= */

void group_move(int group_idx,int xmove,int ymove,int zmove)
{
	int					n,unit_cnt;
	bool				move_objs;
	group_type			*group;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;

		// can this group move objects?

	move_objs=(xmove!=0) || (zmove!=0);
	
		// move the meshes

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		switch (unit_list->type) {

			case group_type_mesh:

					// is mesh moveable?

				mesh=&map.mesh.meshes[unit_list->idx];
				if (!mesh->flag.moveable) break;

					// move mesh and mark as
					// touched so it can be saved with games

				map_mesh_move(&map,unit_list->idx,xmove,ymove,zmove,FALSE);
				mesh->flag.touched=TRUE;

					// move objects and decals with mesh

				if (move_objs) object_move_with_mesh(unit_list->idx,xmove,zmove);
				decal_move_with_mesh(unit_list->idx,xmove,ymove,zmove);
				break;

			case group_type_liquid:

				liq=&map.liquid.liquids[unit_list->idx];

				liq->lft+=xmove;
				liq->rgt+=xmove;
				liq->top+=zmove;
				liq->bot+=zmove;
				liq->y+=ymove;

				break;

		}
		
		unit_list++;
	}
}

/* =======================================================

      Rotate Groups
      
======================================================= */

void group_rotate(int group_idx,float x,float y,float z)
{
	int					n,unit_cnt;
	bool				move_objs;
	group_type			*group;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;

		// can this group move objects?

	move_objs=(y!=0.0f);
	
		// move the meshes

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {

				// is mesh moveable?

			mesh=&map.mesh.meshes[unit_list->idx];
			if (!mesh->flag.moveable) break;

				// move mesh and mark as
				// touched so it can be saved with games

			map_mesh_rotate(&map,unit_list->idx,x,y,z,FALSE);
			mesh->flag.touched=TRUE;

				// rotate objects and decals with mesh
			
			if (move_objs) object_rotate_with_mesh(unit_list->idx,y);
			decal_rotate_with_mesh(unit_list->idx,y);
		}
		
		unit_list++;
	}
}

/* =======================================================

      Run Moves
      
======================================================= */

void group_moves_run(bool run_events)
{
	int				n,x,y,z,user_id;
	group_move_type	*move;

		// run all moves
		
	for (n=0;n!=map.ngroup;n++) {

		move=&map.groups[n].move;
		if ((!move->on) || (move->freeze)) continue;
		
			// movements
			
		if (move->has_mov) {
			move->cur_mov.x+=move->mov_add.x;
			move->cur_mov.y+=move->mov_add.y;
			move->cur_mov.z+=move->mov_add.z;
				
			x=(int)move->cur_mov.x;
			y=(int)move->cur_mov.y;
			z=(int)move->cur_mov.z;
				
			move->cur_mov.x-=(float)x;
			move->cur_mov.y-=(float)y;
			move->cur_mov.z-=(float)z;

			group_move(n,x,y,z);
		}
		
			// rotations
			
		if (move->has_rot) {
			group_rotate(n,move->rot_add.x,move->rot_add.y,move->rot_add.z);
		}
		
		move->count--;
	}
	
		// handle any finished moves
		// we do this separately as multiple group moves
		// hooked up to a move can interfere with each other
		
	for (n=0;n!=map.ngroup;n++) {

		move=&map.groups[n].move;
		if ((!move->on) || (move->freeze) || (move->count!=0)) continue;

		move->on=FALSE;

			// only post events and single map movements if
			// this move is the main move of a group of movements
			
		if (!move->main_move) continue;

			// post the finished event
	
		user_id=move->user_id;
		if (run_events) scripts_post_event_console((attach_type*)&move->attach,sd_event_move,sd_event_move_done,user_id);

			// signal back to the original map movement
	
		if (map_movement_next_move(move->movement_idx,(attach_type*)&move->attach)) {
			if (run_events) scripts_post_event_console((attach_type*)&move->attach,sd_event_move,sd_event_move_loop,user_id);
		}
	}
}


/* =======================================================

      Synch Group Movements with Host
      
======================================================= */

void group_moves_synch_with_host(int tick_offset)
{
	int			n,count;
	
	return;		// supergumba -- fix this later, do it in a different way
	
	count=tick_offset/10;
	
	for (n=0;n!=count;n++) {
		group_moves_run(FALSE);
	}
}
