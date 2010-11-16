/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Dragging Routines

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h" 
#include "view.h"

extern map_type					map;
extern editor_state_type		state;

/* =======================================================

      Drag Mesh Handles
      
======================================================= */

bool view_click_drag_mesh_handle(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,sub_idx,
							px[8],py[8],pz[8],
							type,mesh_idx,poly_idx,handle_idx;
	bool					first_drag;
	d3pnt					old_pt,*old_dpt,mpt,move_pnt,
							org_min,org_max,min,max;
	d3rect					box;
	map_mesh_type			*mesh;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
	
		// are we clicking in the grow handles?
		
	view_draw_select_mesh_get_grow_handles(mesh_idx,px,py,pz);
		
	view_pick_list_start(view,FALSE,8);
	
	for (n=0;n!=8;n++) {
		view_pick_list_add_handle(px[n],py[n],pz[n],liquid_piece,n,0);
	}

	handle_idx=-1;

	view_pick_list_end(view,pt,&type,&handle_idx,&sub_idx);
	
	if (handle_idx==-1) return(FALSE);
	
		// are we dragging?
		
    if (!os_button_down()) return(FALSE);
	
	undo_push();
	
		// hilite the drag handle
		
	state.drag_handle_idx=handle_idx;
	main_wind_draw();
	
		// backup all the vertexes
		
	old_dpt=malloc(mesh->nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	memmove(old_dpt,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
		
		// start dragging

	first_drag=TRUE;
	
	map_mesh_calculate_extent(&map,mesh_idx,&org_min,&org_max);
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// resize mesh

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*move_mouse_scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*move_mouse_scale));

		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;

		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		view_click_grid(&mpt);
		
		memmove(&min,&org_min,sizeof(d3pnt));
		memmove(&max,&org_max,sizeof(d3pnt));
		
			// use correct handle to alter the mesh
			
		if ((handle_idx==0) || (handle_idx==3) || (handle_idx==4) || (handle_idx==7)) {
			min.x+=mpt.x/2;
		}
		else {
			max.x+=mpt.x/2;
		}
		
		if (handle_idx<4) {
			min.y+=mpt.y/2;
		}
		else {
			max.y+=mpt.y/2;
		}
		
		if ((handle_idx==0) || (handle_idx==1) || (handle_idx==4) || (handle_idx==5)) {
			min.z+=mpt.z/2;
		}
		else {
			max.z+=mpt.z/2;
		}
		
			// always resize from original vertexes
			
		memmove(mesh->vertexes,old_dpt,(mesh->nvertex*sizeof(d3pnt)));
		map_mesh_resize(&map,mesh_idx,&min,&max);
		
		if ((state.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);

        main_wind_draw();
	}
	
	free(old_dpt);
	
	os_set_arrow_cursor();
	
		// turn off hilite
		
	state.drag_handle_idx=-1;
	
	main_wind_draw();
	
	return(TRUE);
}

/* =======================================================

      Drag Mesh
      
======================================================= */

bool view_click_drag_mesh(editor_view_type *view,d3pnt *pt)
{
	int						n,k,x,y,mx,my,mz,nsel,nvertex,
							type,mesh_idx,poly_idx;
	bool					first_drag,snap_hit;
	d3pnt					old_pt,*dpt,*old_dpt,*old_dpt_ptr,move_pnt,mpt,spt;
	d3rect					box;
	map_mesh_type			*mesh;
	
		// any selection?
		
	nsel=select_count();
	if (nsel==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
		
		// start drag
		
    if (!os_button_down()) return(FALSE);
	
	undo_push();
	
		// save old vertexes
		
	nvertex=0;
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		nvertex+=map.mesh.meshes[mesh_idx].nvertex;
	}
	
	old_dpt=malloc(nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	old_dpt_ptr=old_dpt;
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];

		memmove(old_dpt_ptr,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
		old_dpt_ptr+=mesh->nvertex;
	}
	
		// drag meshes
		
	mx=my=mz=0;
			
	first_drag=TRUE;
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*move_mouse_scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*move_mouse_scale));
		
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
			// if there is a grid, use first vertex
			// for grid
			
		mpt.x+=old_dpt[0].x;
		mpt.y+=old_dpt[0].y;
		mpt.z+=old_dpt[0].z;

		view_click_grid(&mpt);
		
		mpt.x-=old_dpt[0].x;
		mpt.y-=old_dpt[0].y;
		mpt.z-=old_dpt[0].z;
		
			// check all vertexes for snaps
			// and stop on first hit

		snap_hit=FALSE;
		
		old_dpt_ptr=old_dpt;
		
		for (k=0;k!=nsel;k++) {
			select_get(k,&type,&mesh_idx,&poly_idx);
			if (type!=mesh_piece) continue;
			
			mesh=&map.mesh.meshes[mesh_idx];
		
			for (n=0;n!=mesh->nvertex;n++) {
				spt.x=old_dpt_ptr->x+mpt.x;
				spt.y=old_dpt_ptr->y+mpt.y;
				spt.z=old_dpt_ptr->z+mpt.z;
				
				if (view_click_snap_mesh(mesh_idx,&spt)) {
					mpt.x=spt.x-old_dpt_ptr->x;
					mpt.y=spt.y-old_dpt_ptr->y;
					mpt.z=spt.z-old_dpt_ptr->z;
					snap_hit=TRUE;
					break;
				}
				
				old_dpt_ptr++;
			}
			
			if (snap_hit) break;
		}

			// move vertexes
		
		old_dpt_ptr=old_dpt;
		
		for (k=0;k!=nsel;k++) {
			select_get(k,&type,&mesh_idx,&poly_idx);
			if (type!=mesh_piece) continue;
			
			mesh=&map.mesh.meshes[mesh_idx];
			dpt=mesh->vertexes;
		
			for (n=0;n!=mesh->nvertex;n++) {
				dpt->x=old_dpt_ptr->x+mpt.x;
				dpt->y=old_dpt_ptr->y+mpt.y;
				dpt->z=old_dpt_ptr->z+mpt.z;
				
				dpt++;
				old_dpt_ptr++;
			}
			
			if ((state.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);
		}
		
        main_wind_draw();
	}
	
	free(old_dpt);
	
	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Poly
      
======================================================= */

bool view_click_drag_mesh_poly(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,
							type,mesh_idx,poly_idx;
	bool					first_drag;
	d3pnt					old_pt,*dpt,*old_dpt,move_pnt,mpt,spt;
	d3rect					box;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
		// any selection?
		
	if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);

		// are we clicking in mesh
		
	mesh=&map.mesh.meshes[mesh_idx];
	mesh_poly=&mesh->polys[poly_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
	
		// drag
		
    if (!os_button_down()) return(FALSE);
	
	undo_push();
			
	first_drag=TRUE;
	
	old_dpt=malloc(mesh_poly->ptsz*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		memmove(&old_dpt[n],&mesh->vertexes[mesh_poly->v[n]],sizeof(d3pnt));
	}
	
	mx=my=mz=0;
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get vertex movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*move_mouse_scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*move_mouse_scale));
		
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
			// if there is a grid, use first vertex
			// for grid
			
		mpt.x+=old_dpt[0].x;
		mpt.y+=old_dpt[0].y;
		mpt.z+=old_dpt[0].z;

		view_click_grid(&mpt);
		
		mpt.x-=old_dpt[0].x;
		mpt.y-=old_dpt[0].y;
		mpt.z-=old_dpt[0].z;
		
			// check all vertexes for snaps
			// and stop on first hit

		for (n=0;n!=mesh_poly->ptsz;n++) {
			spt.x=old_dpt[n].x+mpt.x;
			spt.y=old_dpt[n].y+mpt.y;
			spt.z=old_dpt[n].z+mpt.z;
				
			if (view_click_snap_poly(mesh_idx,poly_idx,&spt)) {
				mpt.x=spt.x-old_dpt[n].x;
				mpt.y=spt.y-old_dpt[n].y;
				mpt.z=spt.z-old_dpt[n].z;
				break;
			}
		}
			// move vertexes
			
		for (n=0;n!=mesh_poly->ptsz;n++) {
			dpt=&mesh->vertexes[mesh_poly->v[n]];
			dpt->x=old_dpt[n].x+mpt.x;
			dpt->y=old_dpt[n].y+mpt.y;
			dpt->z=old_dpt[n].z+mpt.z;
		}

		if ((state.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_poly_uv(&map,mesh_idx,poly_idx);

        main_wind_draw();
	}
	
	free(old_dpt);

	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Vertex
      
======================================================= */

bool view_click_drag_vertex(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,sub_idx,
							type,mesh_idx,poly_idx,vertex_idx;
	d3pnt					old_pt,*dpt,old_dpt,move_pnt,mpt;
	d3rect					box;
	bool					first_drag;
	map_mesh_type			*mesh;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
		// check for clicking points

	mesh=&map.mesh.meshes[mesh_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
		
	view_pick_list_start(view,FALSE,mesh->nvertex);
	
	dpt=mesh->vertexes;
	
	for (n=0;n!=mesh->nvertex;n++) {
		view_pick_list_add_handle(dpt->x,dpt->y,dpt->z,mesh_piece,n,0);
		dpt++;
	}

	vertex_idx=-1;

	view_pick_list_end(view,pt,&type,&vertex_idx,&sub_idx);

    if (vertex_idx==-1) return(FALSE);
	
		// drag mesh
	
    if (!os_button_down()) return(FALSE);
	
	undo_push();
	
		// turn on hilite
		
	state.drag_handle_idx=vertex_idx;
	main_wind_draw();

		// drag
		
	first_drag=TRUE;
	
	dpt=&mesh->vertexes[vertex_idx];
	memmove(&old_dpt,dpt,sizeof(d3pnt));
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get vertex movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*move_mouse_scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*move_mouse_scale));
			
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
			// move vertex
			
		dpt->x=old_dpt.x+mpt.x;
		dpt->y=old_dpt.y+mpt.y;
		dpt->z=old_dpt.z+mpt.z;
			
			// grid and snap
			
		view_click_grid(dpt);
		view_click_snap(mesh_idx,dpt);
	
		if ((state.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
		// turn off hilite
		
	state.drag_handle_idx=-1;
	
	main_wind_draw();
	
	return(TRUE);
}

/* =======================================================

      Drag Liquid Vertex
      
======================================================= */

bool view_click_drag_liquid_vertex(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,chk_x,chk_z,
							px[4],py[4],pz[4],
							type,liquid_idx,sub_idx,handle_idx;
	d3pnt					old_pt,old_dpt,move_pnt,mpt;
	d3rect					box;
	bool					first_drag;
	map_liquid_type			*liq;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&liquid_idx,&sub_idx);
	if (type!=liquid_piece) return(FALSE);
	
		// check for clicking handle points

	liq=&map.liquid.liquids[liquid_idx];
	
	view_draw_select_liquid_get_grow_handles(liquid_idx,px,py,pz);
		
	view_pick_list_start(view,FALSE,4);
	
	for (n=0;n!=4;n++) {
		view_pick_list_add_handle(px[n],py[n],pz[n],liquid_piece,n,0);
	}

	handle_idx=-1;

	view_pick_list_end(view,pt,&type,&handle_idx,&sub_idx);

    if (handle_idx==-1) return(FALSE);
	
		// drag liquid vertex
	
    if (!os_button_down()) return(FALSE);

			// hilite the drag handle
		
	state.drag_handle_idx=handle_idx;
	main_wind_draw();
		
		// drag
	
	undo_push();

	first_drag=TRUE;
	
	old_dpt.x=px[handle_idx];
	old_dpt.y=py[handle_idx];
	old_dpt.z=pz[handle_idx];
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get vertex move (never on Y)

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*move_mouse_scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*move_mouse_scale));
			
		mx+=move_pnt.x;
		my=liq->y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		view_click_grid(&mpt);
		
			// move vertex
			
		switch (handle_idx) {
			case 0:
				liq->lft=old_dpt.x+mpt.x;
				liq->top=old_dpt.z+mpt.z;
				break;
			case 1:
				liq->rgt=old_dpt.x+mpt.x;
				liq->top=old_dpt.z+mpt.z;
				break;
			case 2:
				liq->rgt=old_dpt.x+mpt.x;
				liq->bot=old_dpt.z+mpt.z;
				break;
			case 3:
				liq->lft=old_dpt.x+mpt.x;
				liq->bot=old_dpt.z+mpt.z;
				break;
		}
		
		if (state.auto_texture) map_liquid_reset_uv(&map,liquid_idx);

        main_wind_draw();
	}

		// fix any left/right swaps
		
	if (liq->lft>liq->rgt) {
		chk_x=liq->lft;
		liq->lft=liq->rgt;
		liq->rgt=chk_x;
	}
	
	if (liq->top>liq->bot) {
		chk_z=liq->top;
		liq->top=liq->bot;
		liq->bot=chk_z;
	}

		// remove hilite

	state.drag_handle_idx=-1;
	
	main_wind_draw();
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Liquid
      
======================================================= */

bool view_click_drag_liquid(editor_view_type *view,d3pnt *pt)
{
	int						x,y,mx,my,mz,
							old_lft,old_rgt,old_top,old_bot,old_y,
							type,main_idx,sub_idx;
	d3pnt					old_pt,move_pnt,mpt;
	d3rect					box;
	bool					first_drag;
	map_liquid_type			*liq;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&main_idx,&sub_idx);
	if (type!=liquid_piece) return(FALSE);
	
	liq=&map.liquid.liquids[main_idx];
		
		// drag item
	
    if (!os_button_down()) return(FALSE);
	
	undo_push();

	first_drag=TRUE;
	
	old_lft=liq->lft;
	old_rgt=liq->rgt;
	old_top=liq->top;
	old_bot=liq->bot;
	old_y=liq->y;
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*move_mouse_scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*move_mouse_scale));
			
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		view_click_grid(&mpt);
		
			// move liquid
			
		liq->lft=old_lft+mpt.x;
		liq->rgt=old_rgt+mpt.x;
		liq->top=old_top+mpt.z;
		liq->bot=old_bot+mpt.z;
		liq->y=old_y+mpt.y;

		if (state.auto_texture) map_liquid_reset_uv(&map,main_idx);

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Drag Item
      
======================================================= */

bool view_click_drag_item(editor_view_type *view,d3pnt *pt)
{
	int						x,y,mx,my,mz,
							type,main_idx,sub_idx;
	d3pnt					*pnt,old_pt,old_pnt,move_pnt,mpt;
	d3rect					box;
	bool					first_drag;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&main_idx,&sub_idx);
	
		// get item pos
		
	pnt=NULL;
	
	switch (type) {
		case node_piece:
			pnt=&map.nodes[main_idx].pnt;
			break;
		case spot_piece:
			pnt=&map.spots[main_idx].pnt;
			break;
		case scenery_piece:
			pnt=&map.sceneries[main_idx].pnt;
			break;
		case light_piece:
			pnt=&map.lights[main_idx].pnt;
			break;
		case sound_piece:
			pnt=&map.sounds[main_idx].pnt;
			break;
		case particle_piece:
			pnt=&map.particles[main_idx].pnt;
			break;
	}
	
	if (pnt==NULL) return(FALSE);
		
		// drag item
	
    if (!os_button_down()) return(FALSE);
	
	undo_push();

	first_drag=TRUE;
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*move_mouse_scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*move_mouse_scale));
			
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		view_click_grid(&mpt);
		
		pnt->x=old_pnt.x+mpt.x;
		pnt->y=old_pnt.y+mpt.y;
		pnt->z=old_pnt.z+mpt.z;

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Box Select
      
======================================================= */

void view_click_box_select(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,item_count,
							type[view_max_box_select_item],
							main_idx[view_max_box_select_item],
							sub_idx[view_max_box_select_item];
	d3pnt					old_pt;
	d3rect					box;
		
	view_get_pixel_box(view,&box);
	
		// draw map and get pixels
		
	view_click_piece_map_pick_start(view);
	if (!view_pick_list_multiple_setup(view)) {	
		view_pick_list_multiple_end();
		return;
	}

		// setup the start point

	state.select_box_on=TRUE;
	memmove(&state.select_box_start_pnt,pt,sizeof(d3pnt));
	
		// run the select

	memmove(&old_pt,pt,sizeof(d3pnt));

	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// setup the end point

		memmove(&state.select_box_end_pnt,pt,sizeof(d3pnt));

			// select any items
			
		item_count=view_pick_list_multiple_pick(view,&state.select_box_start_pnt,&state.select_box_end_pnt,type,main_idx,sub_idx,view_max_box_select_item);
		
		select_clear();
		for (n=0;n!=item_count;n++) {
			select_add(type[n],main_idx[n],sub_idx[n]);
		}

			// redraw the selection

        main_wind_draw();
	}

		// end picking

	view_pick_list_multiple_end();

		// clear the select box

	state.select_box_on=FALSE;
	
	main_wind_draw();
}