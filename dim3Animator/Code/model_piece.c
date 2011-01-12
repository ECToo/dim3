/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Add or Delete Pieces

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type				model;
extern animator_state_type		state;

/* =======================================================

      Add or Delete Meshes
      
======================================================= */

void model_piece_add_mesh(void)
{
	int				idx;

	idx=model_mesh_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Mesh","You've reached the mesh limit for this model.");
		return;
	}

	state.cur_item=item_mesh;
	state.cur_mesh_idx=idx;

	item_palette_scroll_into_view(item_mesh,idx);
}

void model_piece_duplicate_mesh(int mesh_idx)
{
	int				idx;

	idx=model_mesh_duplicate(&model,mesh_idx);
	if (idx==-1) {
		os_dialog_alert("Duplicate Mesh","You've reached the mesh limit for this model.");
		return;
	}
			
	state.cur_item=item_mesh;
	state.cur_mesh_idx=idx;

	item_palette_scroll_into_view(item_mesh,idx);
}

void model_piece_delete_mesh(int mesh_idx)
{
	if (model.nmesh==1) {
		os_dialog_alert("Delete Mesh","Can not delete -- you need at least one mesh in the model.");
		return;
	}

	model_mesh_delete(&model,mesh_idx);

	state.cur_item=item_model;
	state.cur_mesh_idx=0;

	item_palette_scroll_into_view(item_model,0);
}

/* =======================================================

      Add or Delete Bones
      
======================================================= */

void model_piece_add_bone(void)
{
	int				x,y,z,idx;

	vertex_find_center_sel_vertexes(state.cur_mesh_idx,&x,&y,&z);
	
	idx=model_bone_add(&model,x,y,z);
	if (idx==-1) {
		os_dialog_alert("Add Bone","You've reached the bone limit for this model.");
		return;
	}
	
	vertex_set_sel_vertex_to_bone(state.cur_mesh_idx,idx,-1,1.0f);

	state.cur_item=item_bone;
	state.cur_bone_idx=idx;

	item_palette_scroll_into_view(item_bone,idx);
}

void model_piece_delete_bone(int bone_idx)
{
 	model_bone_delete(&model,bone_idx);

	state.cur_item=item_model;

	state.cur_bone_idx=-1;
	state.cur_pose_bone_move_idx=-1;

	item_palette_scroll_into_view(item_model,0);
}

/* =======================================================

      Add or Delete Poses
      
======================================================= */

void model_piece_add_pose(void)
{
	int				idx;

	if (model.nbone==0) {
		os_dialog_alert("Add Pose","You need to have at least one bone before creating a pose.");
		return;
	}
	
	idx=model_pose_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Pose","You've reached the pose limit for this model.");
		return;
	}
	
	state.cur_item=item_pose;
	state.cur_pose_idx=idx;
	state.cur_pose_bone_move_idx=-1;

	item_palette_scroll_into_view(item_pose,idx);
}

void model_piece_duplicate_pose(int pose_idx)
{
	int				idx;

	idx=model_pose_duplicate(&model,state.cur_pose_idx);
	if (idx==-1) {
		os_dialog_alert("Duplicate Pose","You've reached the pose limit for this model.");
		return;
	}
	
	state.cur_item=item_pose;
	state.cur_pose_idx=idx;
	state.cur_pose_bone_move_idx=-1;

	item_palette_scroll_into_view(item_pose,idx);
}

void model_piece_delete_pose(int pose_idx)
{
	if (model_check_pose_in_animation(&model,pose_idx)) {
		os_dialog_alert("Delete Pose","Can not delete -- this pose is being used in an animation.");
		return;
	}
			
	model_pose_delete(&model,pose_idx);

	state.cur_item=item_model;

	if (model.npose==0) {
		state.cur_pose_idx=-1;
	}
	else {
		state.cur_pose_idx=0;
	}
	state.cur_pose_bone_move_idx=-1;

	item_palette_scroll_into_view(item_model,0);
}

/* =======================================================

      Add or Delete Animations
      
======================================================= */

void model_piece_add_animate(void)
{
	int				idx;

	if (model.npose==0) {
		os_dialog_alert("Add Animation","You need to have at least one pose before creating an animation.");
		return;
	}
	
	main_wind_play(FALSE,FALSE);
	
	idx=model_animate_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Animation","You've reached the animation limit for this model.");
		return;
	}
	
	state.cur_item=item_animate;
	state.cur_animate_idx=idx;
	state.cur_animate_pose_move_idx=-1;

	item_palette_scroll_into_view(item_animate,idx);
}

void model_piece_duplicate_animate(int animate_idx)
{
	int				idx;

	main_wind_play(FALSE,FALSE);

	idx=model_animate_duplicate(&model,state.cur_animate_idx);
	if (idx==-1) {
		os_dialog_alert("Duplicate Animation","You've reached the animation limit for this model.");
		return;
	}
	
	state.cur_item=item_animate;
	state.cur_animate_idx=idx;
	state.cur_animate_pose_move_idx=-1;

	item_palette_scroll_into_view(item_animate,idx);
}

void model_piece_delete_animate(int animate_idx)
{
	main_wind_play(FALSE,FALSE);

	model_animate_delete(&model,animate_idx);

	state.cur_item=item_model;

	if (model.nanimate==0) {
		state.cur_animate_idx=-1;
	}
	else {
		state.cur_animate_idx=0;
	}
	state.cur_animate_pose_move_idx=-1;

	item_palette_scroll_into_view(item_model,0);
}

/* =======================================================

      Add or Delete Hit Boxes
      
======================================================= */

void model_piece_add_hit_box(void)
{
	int				idx;

	idx=model_hit_box_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Hit Box","You've reached the hit box limit for this model.");
		return;
	}

	state.hit_box=TRUE;
	state.cur_item=item_hit_box;
	state.cur_hit_box_idx=idx;

	item_palette_scroll_into_view(item_hit_box,idx);
}

void model_piece_delete_hit_box(int hit_box_idx)
{
	model_hit_box_delete(&model,hit_box_idx);

	state.cur_item=item_model;
	state.cur_hit_box_idx=-1;

	item_palette_scroll_into_view(item_model,0);
}
