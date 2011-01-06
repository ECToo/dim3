/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Clicking

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

extern d3rect					model_box;

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;

extern void draw_model_bones_get_handle_rot(int bone_idx,d3ang *rot);
extern float draw_model_bones_drag_handle_offset(void);
extern void draw_model_bones_drag_handle_calc(d3fpnt *bone_pnt,d3vct *vct,d3ang *ang,d3fpnt *hand_pnt);

// supergumba -- this code repeats draw setups and only uses the
//               view picker for trig hits, fix all this

/* =======================================================

      Model Vertex Selection
      
======================================================= */

void model_sel_vertex(float *pv,d3rect *box,bool chg_sel)
{
	int					n,nt;
	d3fpnt				pnt;
	d3pnt				tran_pnt;
	d3rect				wbox;
	model_vertex_type	*vertex;
	model_mesh_type		*mesh;
	
	os_get_window_box(&wbox);

		// find selection
		
	mesh=&model.meshes[state.cur_mesh_idx];
		
	nt=mesh->nvertex;
	vertex=mesh->vertexes;

	for (n=0;n!=nt;n++) {
	
		pnt.x=*pv++;
		pnt.y=*pv++;
		pnt.z=*pv++;
		
		draw_model_2D_transform(&pnt,&tran_pnt);
		
		if ((tran_pnt.x>=box->lx) && (tran_pnt.x<=box->rx) && (tran_pnt.y>=box->ty) && (tran_pnt.y<=box->by)) {
			if (!vertex_check_hide_mask(state.cur_mesh_idx,n)) vertex_set_sel_mask(state.cur_mesh_idx,n,chg_sel);
		}
		
		vertex++;
	}
}

void select_model_wind_save_sel_state(char *vertex_sel)
{
	int					n,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[state.cur_mesh_idx].nvertex;
	vertex=model.meshes[state.cur_mesh_idx].vertexes;

	for (n=0;n!=nt;n++) {
		vertex_sel[n]=(char)vertex_check_sel_mask(state.cur_mesh_idx,n);
		vertex++;
	}
}

void select_model_wind_restore_sel_state(char *vertex_sel)
{
	int					n,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[state.cur_mesh_idx].nvertex;
	vertex=model.meshes[state.cur_mesh_idx].vertexes;

	for (n=0;n!=nt;n++) {
		vertex_set_sel_mask(state.cur_mesh_idx,n,(vertex_sel[n]!=0));
		vertex++;
	}
}

void select_model_wind_vertex(d3pnt *start_pnt,float *pv)
{
	char					*org_vertex_sel;
	bool					chg_sel;
	d3pnt					pnt,last_pnt;
	
		// turn on or off?
		
	org_vertex_sel=(char*)malloc(model.meshes[state.cur_mesh_idx].nvertex);
		
	if (os_key_shift_down()) {
		select_model_wind_save_sel_state(org_vertex_sel);
		os_set_add_cursor();
		chg_sel=TRUE;
	}
	else {
		if (os_key_control_down()) {
			select_model_wind_save_sel_state(org_vertex_sel);
			os_set_subtract_cursor();
			chg_sel=FALSE;
		}
		else {
			memset(org_vertex_sel,0x0,model.meshes[state.cur_mesh_idx].nvertex);
			os_set_arrow_cursor();
			chg_sel=TRUE;
		}
	}
	
		// drag the selection

	last_pnt.x=last_pnt.y=-1;
	
	state.drag_sel_on=TRUE;
	
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		if (start_pnt->x<last_pnt.x) {
			state.drag_sel_box.lx=start_pnt->x;
			state.drag_sel_box.rx=last_pnt.x;
		}
		else {
			state.drag_sel_box.rx=start_pnt->x;
			state.drag_sel_box.lx=last_pnt.x;
		}
		if (start_pnt->y<last_pnt.y) {
			state.drag_sel_box.ty=start_pnt->y;
			state.drag_sel_box.by=last_pnt.y;
		}
		else {
			state.drag_sel_box.by=start_pnt->y;
			state.drag_sel_box.ty=last_pnt.y;
		}
		
		select_model_wind_restore_sel_state(org_vertex_sel);
		model_sel_vertex(pv,&state.drag_sel_box,chg_sel);
		
		main_wind_draw();
	}
	
	state.drag_sel_on=FALSE;
	
	free(org_vertex_sel);
}

/* =======================================================

      Model Trig Selection
      
======================================================= */

bool select_model_wind_polygon(d3pnt *start_pnt)
{
	int					n,k,idx,ntrig;
	float				*pv;
	d3pnt				v_pnts[3];
    model_trig_type		*trig;
	model_mesh_type		*mesh;
	
		// clicking mesh
		
	mesh=&model.meshes[state.cur_mesh_idx];
	
		// draw and pick the triangles
		
	if (!model_pick_list_start(mesh->ntrig)) return(FALSE);
	
		// draw the mesh
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
	draw_model_setup_bones_vertexes(state.cur_mesh_idx);

	ntrig=mesh->ntrig;
	trig=mesh->trigs;
    
    for (n=0;n!=ntrig;n++) {
	
		if (!vertex_check_hide_mask_trig(state.cur_mesh_idx,trig)) {
			
			for (k=0;k!=3;k++) {
				pv=draw_setup.mesh_arrays[state.cur_mesh_idx].gl_vertex_array+(trig->v[k]*3);
				v_pnts[k].x=(int)*pv++;
				v_pnts[k].y=(int)*pv++;
				v_pnts[k].z=(int)*pv;
			}
			
			model_pick_list_add_trig(n,v_pnts);
		}
		
		trig++;
    }
	
	model_draw_setup_shutdown(&model,&draw_setup);
	
	model_pick_list_end(start_pnt,&idx);
	
	state.sel_trig_idx=idx;

	if (state.sel_trig_idx==-1) return(FALSE);
	
		// select all the vertexes attached to trig
	
	vertex_clear_sel_mask(state.cur_mesh_idx);
	
	trig=&mesh->trigs[idx];
	
	for (k=0;k!=3;k++) {
		vertex_set_sel_mask(state.cur_mesh_idx,trig->v[k],TRUE);
	}

	return(TRUE);
}

/* =======================================================

      Model Select Main Line
      
======================================================= */

void select_model_wind(d3pnt *start_pnt)
{
	int				sz;
	float			*pv;
	
		// no playing while selecting
		
	main_wind_play(FALSE,FALSE);
	
		// get the draw vertexes
		// need to save off array as drawing will reuse
		// array and free it
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
		
	draw_model_setup_pose(state.cur_pose_idx);
	
	model_create_draw_bones(&model,&draw_setup);
	model_create_draw_vertexes(&model,state.cur_mesh_idx,&draw_setup);
	
	sz=(model.meshes[state.cur_mesh_idx].nvertex*3)*sizeof(float);
	pv=(float*)malloc(sz);
	if (pv==NULL) return;
	
	memmove(pv,draw_setup.mesh_arrays[state.cur_mesh_idx].gl_vertex_array,sz);
		
	model_draw_setup_shutdown(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(0);
	draw_model_2D_transform_setup();

		// run the correct click
		
	if (state.select_mode==select_mode_polygon) {
		if (!select_model_wind_polygon(start_pnt)) {
			select_model_wind_vertex(start_pnt,pv);
		}
	}
	else {
		select_model_wind_vertex(start_pnt,pv);
	}
	
		// free the saved vertexes

	free(pv);

		// redraw the model
		
	main_wind_draw();

	os_set_arrow_cursor();	
}

/* =======================================================

      Model Movement
      
======================================================= */

void change_model_wind(d3pnt *start_pnt,bool shift_on,bool rotate_on,bool size_on)
{
	int						old_magnify_z;
	d3pnt					pnt,last_pnt,old_shift;
	d3ang					old_ang;
	
		// rotate, shift, or size the model

	last_pnt.x=last_pnt.y=-1;
	
	old_shift.x=state.shift.x;
	old_shift.y=state.shift.y;
	
	old_ang.x=state.ang.x;
	old_ang.y=state.ang.y;
	
	old_magnify_z=state.magnify_z;
	
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
			
		if (shift_on) {
			state.shift.x=old_shift.x+((last_pnt.x-start_pnt->x)*4);
			state.shift.y=old_shift.y-((last_pnt.y-start_pnt->y)*4);
		}
		if (rotate_on) {
			state.ang.x=old_ang.x-(float)((last_pnt.y-start_pnt->y)/5);
			state.ang.y=old_ang.y+(float)((last_pnt.x-start_pnt->x)/5);
		}
		if (size_on) {
			state.magnify_z=old_magnify_z+((last_pnt.y-start_pnt->y)*2);
		}
		
		if (!state.playing) main_wind_draw();
	
	}
	
	if (!state.playing) main_wind_draw();
}

/* =======================================================

      Model Window Bone Clicks
      
======================================================= */

bool draw_bone_model_wind_click_box(d3pnt *click_pnt,d3fpnt *pnt)
{
	d3pnt			tran_pnt;
	
	draw_model_2D_transform(pnt,&tran_pnt);
	return((click_pnt->x>=(tran_pnt.x-6)) && (click_pnt->x<=(tran_pnt.x+6)) && (click_pnt->y>=(tran_pnt.y-6)) && (click_pnt->y<=(tran_pnt.y+6)));
}

bool drag_bone_model_wind(d3pnt *start_pnt)
{
	int						n,k,x,drag_handle;
	float					org_ang,org_mov,
							bone_drag_handle_offset;
	float					*ang,*mov;
	d3pnt					pnt,last_pnt;
	d3fpnt					bone_pnt,hand_pnt;
	d3vct					vct;
	d3ang					hang,rot;
	model_draw_bone_type	*draw_bone;
	
	if (model.nbone==0) return(FALSE);
	
	main_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(state.cur_pose_idx);
	model_create_draw_bones(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(0);
	draw_model_2D_transform_setup();
	
		// click on any drag handles?
		
	drag_handle=drag_handle_none;
	
	if ((state.cur_pose_idx==-1) || (state.cur_bone_idx!=-1)) {
	
		draw_bone=&draw_setup.bones[state.cur_bone_idx];
		
		bone_pnt.x=draw_bone->fpnt.x+draw_setup.move.x;
		bone_pnt.y=draw_bone->fpnt.y+draw_setup.move.y;
		bone_pnt.z=draw_bone->fpnt.z+draw_setup.move.z;
		
		bone_drag_handle_offset=draw_model_bones_drag_handle_offset();

		draw_model_bones_get_handle_rot(state.cur_bone_idx,&rot);
		
			// x drag bone

		vct.x=bone_drag_handle_offset;
		vct.y=0;
		vct.z=0;
		hang.x=0;
		hang.y=rot.y;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(&bone_pnt,&vct,&hang,&hand_pnt);
		if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) drag_handle=drag_handle_x;
		
			// y drag bone
			
		vct.x=0;
		vct.y=bone_drag_handle_offset;
		vct.z=0;
		hang.x=rot.x;
		hang.y=0;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(&bone_pnt,&vct,&hang,&hand_pnt);
		if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) drag_handle=drag_handle_y;
		
			// z drag bone
			
		vct.x=0;
		vct.y=0;
		vct.z=bone_drag_handle_offset;
		hang.x=rot.x;
		hang.y=rot.y;
		hang.z=0;
		draw_model_bones_drag_handle_calc(&bone_pnt,&vct,&hang,&hand_pnt);
		if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) drag_handle=drag_handle_z;
	}
	
		// click on any bones?
		
	if (drag_handle==drag_handle_none) {
	
		k=-1;
		draw_bone=draw_setup.bones;
			
		for (n=0;n!=model.nbone;n++) {
			bone_pnt.x=draw_bone->fpnt.x+draw_setup.move.x;
			bone_pnt.y=draw_bone->fpnt.y+draw_setup.move.y;
			bone_pnt.z=draw_bone->fpnt.z+draw_setup.move.z;
			
			if (draw_bone_model_wind_click_box(start_pnt,&bone_pnt)) {
				k=n;
				break;
			}

			draw_bone++;
		}
		
		if (k==-1) return(FALSE);
	
			// select as current bone
			
		state.cur_bone_idx=k;

		main_wind_draw();

		return(TRUE);
	}
	
		// get drag angle
		
	switch (drag_handle) {
	
		case drag_handle_x:
			ang=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].rot.x;
			mov=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].mov.x;
			break;
		
		case drag_handle_y:
			ang=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].rot.y;
			mov=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].mov.y;
			break;
			
		case drag_handle_z:
			ang=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].rot.z;
			mov=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].mov.z;
			break;
		
		default:
			return(TRUE);
			
	}
	
		// drag bone

	org_ang=*ang;
	org_mov=*mov;
	last_pnt.x=last_pnt.y=-1;
	
	undo_set_bone_move(state.cur_pose_idx,state.cur_bone_idx);
	
	os_set_drag_cursor();
		
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		x=pnt.x-start_pnt->x;
		if (x<-180) x=-180;
		if (x>180) x=180;
		
		if (state.drag_bone_mode==drag_bone_mode_rotate) {
			*ang=org_ang+(((float)x)/2.0f);
		}
		else {
			*mov=org_mov+(((float)x)/20.0f);
		}
		
			// draw the model
			
		main_wind_draw();
	}

	os_set_arrow_cursor();

		// redraw model
		
	main_wind_draw();

	return(TRUE);
}

/* =======================================================

      Model Window Hit Box Clicks
      
======================================================= */

bool drag_hit_box_handle_model_wind(d3pnt *start_pnt)
{
	int						n,k,box_idx,pt_idx,xsz,zsz,ysz,offx,offz,offy,
							kx,ky,kz,x[8],y[8],z[8];
	bool					model_hit_box_drag_on;
	d3pnt					org_pnt,org_cnt,last_pnt,pnt;
	d3fpnt					hand_pnt;
	model_box_type			*box;
	
	main_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(state.cur_pose_idx);
	model_create_draw_bones(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(0);

		// find a click
		
	box_idx=pt_idx=-1;
		
	for (n=0;n<model.nhit_box;n++) {
		box=&model.hit_boxes[n].box;

		xsz=box->size.x/2;
		offx=box->offset.x;
		zsz=box->size.z/2;
		offz=box->offset.z;
		ysz=box->size.y;
		offy=box->offset.y;
		
		x[0]=x[1]=x[4]=x[5]=offx-xsz;
		x[2]=x[3]=x[6]=x[7]=offx+xsz;
		y[0]=y[1]=y[2]=y[3]=offy-ysz;
		y[4]=y[5]=y[6]=y[7]=offy;
		z[0]=z[3]=z[4]=z[7]=offz-zsz;
		z[1]=z[2]=z[5]=z[6]=offz+zsz;
	
		for (k=0;k!=8;k++) {
			model_get_point_position(&draw_setup,&x[k],&y[k],&z[k]);
			hand_pnt.x=(float)x[k];
			hand_pnt.y=(float)y[k];
			hand_pnt.z=(float)z[k];
			if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) {
				box_idx=n;
				pt_idx=k;
				break;
			}
		}
		
		if (box_idx!=-1) break;
	}
	
	if (box_idx==-1) return(FALSE);
	
		// get original size
		
	box=&model.hit_boxes[box_idx].box;
	memmove(&org_pnt,&box->size,sizeof(d3pnt));
	memmove(&org_cnt,&box->offset,sizeof(d3pnt));
	
		// drag handle
		
	model_hit_box_drag_on=FALSE;

	last_pnt.x=last_pnt.y=-1;
	
	os_set_drag_cursor();
		
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		kx=(pnt.x-start_pnt->x)*5;
		ky=(pnt.y-start_pnt->y)*5;
		kz=0;
		
		rotate_point_center(&kx,&ky,&kz,state.ang.x,state.ang.y,0.0f);

		if ((pt_idx==0) || (pt_idx==1) || (pt_idx==4) || (pt_idx==5)) {
			box->size.x=org_pnt.x-kx;
		}
		else {
			box->size.x=org_pnt.x+kx;
		}
		if ((pt_idx==0) || (pt_idx==1) || (pt_idx==2) || (pt_idx==3)) {
			box->size.y=org_pnt.y-ky;
		}
		else {
			box->size.y=org_pnt.y+ky;
			box->offset.y=org_cnt.y+ky;
		}
		if ((pt_idx==0) || (pt_idx==3) || (pt_idx==4) || (pt_idx==7)) {
			box->size.z=org_pnt.z-kz;
		}
		else {
			box->size.z=org_pnt.z+kz;
		}
		
			// draw the model
			
		main_wind_draw();
		model_hit_box_drag_on=TRUE;
	}

	os_set_arrow_cursor();

		// redraw model
		
	main_wind_draw();

	return(model_hit_box_drag_on);
}

/* =======================================================

      Model Window Clicks
      
======================================================= */

void model_wind_click(d3pnt *pnt)
{
	bool			shift_on,rotate_on,size_on;
	
		// get click within window
		
	pnt->x-=model_box.lx;
	pnt->y-=model_box.ty;
	
		// handle the clicks
		
	shift_on=os_key_space_down();
	rotate_on=os_key_command_down();
	size_on=os_key_option_down();
	
	if ((shift_on) || (rotate_on) || (size_on)) {
		change_model_wind(pnt,shift_on,rotate_on,size_on);
		return;
	}
	
	if (state.bone) {
		if (drag_bone_model_wind(pnt)) return;
	}
	
	if (state.hit_box) {
		drag_hit_box_handle_model_wind(pnt);
		return;
	}
		
	select_model_wind(pnt);
}

