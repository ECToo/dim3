/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Piece Click Routines

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

extern int					txt_pixel_sz;
extern d3rect				main_wind_box;

extern map_type				map;
extern setup_type			setup;
extern editor_state_type	state;

/* =======================================================

      Grid and Snap
      
======================================================= */

int walk_view_get_grid(void)
{
	switch (state.grid_mode) {
		case grid_mode_small:
			return(map_enlarge);
		case grid_mode_medium:
			return(map_enlarge<<1);
		case grid_mode_large:
			return(map_enlarge<<3);
	}

	return(1);
}

void walk_view_click_grid(d3pnt *pt)
{
	int			sz;
	
	sz=walk_view_get_grid();
	
	pt->x/=sz;
	pt->y/=sz;
	pt->z/=sz;
	
	pt->x*=sz;
	pt->y*=sz;
	pt->z*=sz;
}

bool walk_view_click_snap(int mesh_idx,d3pnt *pt)
{
	int				n,t;
	d3pnt			*dpt;
	map_mesh_type	*mesh;
	
	if (state.vertex_mode!=vertex_mode_snap) return(FALSE);
	
		// any vertexes to snap to?
		
	mesh=map.mesh.meshes;
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		
		if (n==mesh_idx) {
			mesh++;
			continue;
		}

		dpt=mesh->vertexes;
		
		for (t=0;t!=mesh->nvertex;t++) {
		
			if (distance_get(dpt->x,dpt->y,dpt->z,pt->x,pt->y,pt->z)<(setup.snap_size*map_enlarge)) {
				memmove(pt,dpt,sizeof(d3pnt));
				return(TRUE);
			}
			
			dpt++;
		}
	
		mesh++;
	}
	
	return(FALSE);
}

bool walk_view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt)
{
	int					n,d,cur_dist;
	d3pnt				hpt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	cur_dist=-1;
	
	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
		memmove(&hpt,pt,sizeof(d3pnt));
		
		if (walk_view_click_snap(mesh_idx,&hpt)) {
			d=distance_get(pt->x,pt->y,pt->z,hpt.x,hpt.y,hpt.z);
			if (d>(setup.snap_size*map_enlarge)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pt,&hpt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

bool walk_view_click_snap_mesh(int mesh_idx,d3pnt *pt)
{
	int					n,d,cur_dist;
	d3pnt				hpt;
	map_mesh_type		*mesh;
	
	cur_dist=-1;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		memmove(&hpt,pt,sizeof(d3pnt));
		
		if (walk_view_click_snap(mesh_idx,&hpt)) {
			d=distance_get(pt->x,pt->y,pt->z,hpt.x,hpt.y,hpt.z);
			if (d>(setup.snap_size*map_enlarge)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pt,&hpt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

/* =======================================================

      Rot Handle Rotation Run
      
======================================================= */

d3ang* walk_view_click_rot_handle_rotate_setup(void)
{
	int				type,main_idx,sub_idx;

	select_get(0,&type,&main_idx,&sub_idx);
			
	switch (type) {
	
		case spot_piece:
			return(&map.spots[main_idx].ang);
			
		case scenery_piece:
			return(&map.sceneries[main_idx].ang);
			
		case node_piece:
			return(&map.nodes[main_idx].ang);
			
	}
	
	return(NULL);
}

void walk_view_click_rot_handle_rotate_run_axis(float *value,float org_value,float ang_add)
{
	int				k;
	
	*value=angle_add(org_value,ang_add);
	
	if (os_key_shift_down()) {
		k=(int)*value;
		k/=45;
		*value=(float)(k*45);
	}
}

void walk_view_click_rot_handle_rotate_run(d3ang *ang,d3ang *org_ang,float ang_add,int which_axis)
{
	switch (which_axis) {
		case 0:
			walk_view_click_rot_handle_rotate_run_axis(&ang->x,org_ang->x,ang_add);
			break;
		case 1:
			walk_view_click_rot_handle_rotate_run_axis(&ang->y,org_ang->y,ang_add);
			break;
		case 2:
			walk_view_click_rot_handle_rotate_run_axis(&ang->z,org_ang->z,ang_add);
			break;
	}
}

/* =======================================================

      Rot Handle Move Run
      
======================================================= */

d3pnt* walk_view_click_rot_handle_move_setup(void)
{
	int				type,main_idx,sub_idx;

	select_get(0,&type,&main_idx,&sub_idx);
			
	switch (type) {
	
		case spot_piece:
			return(&map.spots[main_idx].pnt);
			
		case scenery_piece:
			return(&map.sceneries[main_idx].pnt);
			
		case node_piece:
			return(&map.nodes[main_idx].pnt);
			
	}
	
	return(NULL);
}

void walk_view_click_rot_handle_move_run_axis(int *value,int org_value,int mv_add)
{
	*value=org_value+mv_add;
}

void walk_view_click_rot_handle_move_run(d3pnt *pnt,d3pnt *org_pnt,int mv_add,int which_axis)
{
	mv_add=-(mv_add*view_handle_move_scale);
	
	switch (which_axis) {
		case 0:
			walk_view_click_rot_handle_move_run_axis(&pnt->x,org_pnt->x,mv_add);
			break;
		case 1:
			walk_view_click_rot_handle_move_run_axis(&pnt->y,org_pnt->y,mv_add);
			break;
		case 2:
			walk_view_click_rot_handle_move_run_axis(&pnt->z,org_pnt->z,mv_add);
			break;
	}
}

/* =======================================================

      Rot Handle Clicking Main
      
======================================================= */

bool walk_view_click_rot_handles(editor_view_type *view,d3pnt *click_pt)
{
	int			n,type,sub_idx,which_axis;
	bool		first_drag;
	d3pnt		pt,center_pnt,hand_pnt[3],
				*pnt,org_pnt;
	d3ang		*ang,org_ang;
	d3rect		box;
	
		// create rot handle
		
	if (!view_handle_create_rot_handle(view,&center_pnt,hand_pnt)) return(FALSE);
	
		// run pick list for handles
	
	view_pick_list_start(view,TRUE,3);
	
	for (n=0;n!=3;n++) {
		view_pick_list_add_2D_handle(hand_pnt[n].x,hand_pnt[n].y,0,n,0);
	}
	
	view_pick_list_end(view,click_pt,&type,&which_axis,&sub_idx);
	if (type==-1) return(FALSE);
	
		// handle drag
	
    if (!os_button_down()) return(FALSE);
	
	undo_push();
	
	first_drag=TRUE;
	
	if (state.handle_mode==handle_mode_rotate) {
		ang=walk_view_click_rot_handle_rotate_setup();
		if (ang!=NULL) memmove(&org_ang,ang,sizeof(d3ang));
	}
	else {
		pnt=walk_view_click_rot_handle_move_setup();
		if (pnt!=NULL) memmove(&org_pnt,pnt,sizeof(d3pnt));
	}
	
	walk_view_get_pixel_box(view,&box);
	
	while (!os_track_mouse_location(&pt,&box)) {
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}

			// handle movement
			
		if (state.handle_mode==handle_mode_rotate) {
			if (ang!=NULL) walk_view_click_rot_handle_rotate_run(ang,&org_ang,(float)(click_pt->x-pt.x),which_axis);
		}
		else {
			if (pnt!=NULL) walk_view_click_rot_handle_move_run(pnt,&org_pnt,(click_pt->x-pt.x),which_axis);
		}
		
        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Run Pick List for Map Items
      
======================================================= */

void walk_view_click_piece_map_pick_run(editor_view_type *view)
{
	int					n,k,t,count;
	d3pnt				*pt;
	d3pnt				v_pnts[8];
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;
	spot_type			*spot;
	map_scenery_type	*scenery;
	map_light_type		*map_light;
	map_sound_type		*map_sound;
	map_particle_type	*map_particle;
	node_type			*node;

		// get picker count
		// we create a list that marks the color
		// for each item we are going to pick
		// from

	count=0;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		count+=mesh->npoly;
		mesh++;
	}
	
	if (state.show_liquid) count+=map.liquid.nliquid;
	
	if (state.show_object) {
		count+=map.nspot;
		count+=map.nscenery;
	}
	
	if (state.show_lightsoundparticle) {
		count+=map.nlight;
		count+=map.nsound;
		count+=map.nparticle;
	}
	
	if (state.show_node) count+=map.nnode;

		// start the pick list

	view_pick_list_start(view,FALSE,count);

		// run through all the items
		// in the map and draw them to the
		// back buffer with unique colors

		// meshes
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
					
				// clipping
				
			if (walk_view_clip_poly(view,mesh,poly)) {
				poly++;
				continue;
			}
			
				// draw color poly

			view_pick_list_add(mesh_piece,n,k);

			glBegin(GL_POLYGON);
			
			for (t=0;t!=poly->ptsz;t++) {
				pt=&mesh->vertexes[poly->v[t]];
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();

			poly++;
		}

		mesh++;
	}

		// liquids
		
	if (state.show_liquid) {

		liq=map.liquid.liquids;
		
		for (n=0;n!=map.liquid.nliquid;n++) {
		
			view_pick_list_add(liquid_piece,n,-1);

			glBegin(GL_POLYGON);
			glVertex3i(liq->lft,liq->y,liq->top);
			glVertex3i(liq->rgt,liq->y,liq->top);
			glVertex3i(liq->rgt,liq->y,liq->bot);
			glVertex3i(liq->lft,liq->y,liq->bot);
			glEnd();
			
			liq++;
		}
	}

		// objects and scenery

	if (state.show_object) {

		spot=map.spots;

		for (n=0;n!=map.nspot;n++) {
			walk_view_model_cube_vertexes(spot->display_model,&spot->pnt,&spot->ang,v_pnts);
			view_pick_list_add_cube(v_pnts,spot_piece,n,-1);
			spot++;
		}
			
		scenery=map.sceneries;

		for (n=0;n!=map.nscenery;n++) {
			walk_view_model_cube_vertexes(scenery->model_name,&scenery->pnt,&scenery->ang,v_pnts);
			view_pick_list_add_cube(v_pnts,scenery_piece,n,-1);
			scenery++;
		}

	}

		// lights, sounds, and particles

	if (state.show_lightsoundparticle) {
	
		map_light=map.lights;

		for (n=0;n!=map.nlight;n++) {
			view_get_sprite_vertexes(&map_light->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,light_piece,n,-1);
			map_light++;
		}
		
		map_sound=map.sounds;

		for (n=0;n!=map.nsound;n++) {
			view_get_sprite_vertexes(&map_sound->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,sound_piece,n,-1);
			map_sound++;
		}
		
		map_particle=map.particles;

		for (n=0;n!=map.nparticle;n++) {
			view_get_sprite_vertexes(&map_particle->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,particle_piece,n,-1);
			map_particle++;
		}
	}

		// nodes

	if (state.show_node) {
		node=map.nodes;

		for (n=0;n!=map.nnode;n++) {
			view_get_sprite_vertexes(&node->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,node_piece,n,-1);
			node++;
		}
	}
}

void walk_view_click_piece_map_pick_single(editor_view_type *view,d3pnt *click_pt,int *type,int *main_idx,int *sub_idx)
{
	walk_view_click_piece_map_pick_run(view);
	view_pick_list_end(view,click_pt,type,main_idx,sub_idx);
}

int walk_view_click_piece_map_pick_multiple(editor_view_type *view,d3pnt *click_start_pt,d3pnt *click_end_pt,int *type,int *main_idx,int *sub_idx,int max_item)
{
	walk_view_click_piece_map_pick_run(view);
	return(view_pick_list_end_multiple(view,click_start_pt,click_end_pt,type,main_idx,sub_idx,max_item));
}
	
/* =======================================================

      View Map Clicking
      
======================================================= */

void walk_view_click_piece(editor_view_type *view,d3pnt *pt,bool dblclick)
{
	int				type,main_idx,sub_idx;
	bool			toggle_select;
	d3rect			box;

		// convert point to view

	walk_view_get_pixel_box(view,&box);

	pt->x-=box.lx;
	pt->y-=box.ty;

		// box selection

	if (os_key_shift_down()) {
		walk_view_click_box_select(view,pt);
		return;
	}

		// rotation handles

	if (walk_view_click_rot_handles(view,pt)) return;
	
		// liquid vertex drags
		
	if (walk_view_click_drag_liquid_vertex(view,pt)) return;
	
		// mesh vertex drags
		
	switch (state.drag_mode) {
	
		case drag_mode_vertex:
			if (walk_view_click_drag_vertex(view,pt)) return;
			break;
	
		case drag_mode_mesh:
			if (walk_view_click_drag_mesh_handle(view,pt)) return;
			break;
			
	}

		// pick clicked map item (mesh, liquid, nodes, etc)
		
	walk_view_click_piece_map_pick_single(view,pt,&type,&main_idx,&sub_idx);
	
		// if a node, check link
		// connections
		
	if ((type==node_piece) && (state.node_mode!=node_mode_select)) {
		if (node_link_click(main_idx)) {
			main_wind_draw();
			return;
		}
	}
	
		// regular or toggle selection
		
	toggle_select=state.select_add;
	
		// clear or add to selection
		
	if (type==-1) {
		if (!toggle_select) select_clear();
	}
	else {
		if (!toggle_select) {
			if (!select_check(type,main_idx,sub_idx)) {			// keep selection if selecting an already selected piece
				select_clear();	
				select_add(type,main_idx,sub_idx);
			}
		}
		else {
			select_flip(type,main_idx,sub_idx);
		}
	}
	
		// redraw and reset palettes and menus
		
	menu_fix_enable();
	
	main_wind_draw();
	texture_palette_reset();
	palette_reset();

		// if no item or item was toggled off,
		// nothing to drag

	if (type==-1) return;
	if (!select_check(type,main_idx,sub_idx)) return;

		// item (spots, lights, sounds, etc) drags

	if (walk_view_click_drag_item(view,pt)) return;
	
		// liquid drags
		
	if (walk_view_click_drag_liquid(view,pt)) return;
	
		// mesh or poly drags

	switch (state.drag_mode) {
	
		case drag_mode_mesh:
			if (!os_key_control_down()) {
				walk_view_click_drag_mesh(view,pt);
			}
			else {
				walk_view_click_drag_texture(view,pt,TRUE);
			}
			break;

		case drag_mode_polygon:
			if (!os_key_control_down()) {
				walk_view_click_drag_mesh_poly(view,pt);
			}
			else {
				walk_view_click_drag_texture(view,pt,FALSE);
			}
			break;
			
	}
}



