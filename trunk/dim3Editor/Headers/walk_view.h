/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Functions

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

//
// walk view
//

extern bool walk_view_initialize(void);
extern void walk_view_shutdown(void);
extern void walk_view_setup_default_views(void);
extern void walk_view_split_horizontal(void);
extern void walk_view_split_vertical(void);
extern void walk_view_remove(void);
extern void walk_view_get_lookat_vector(editor_view_type *view,float dist,d3vct *look_vct);
extern void walk_view_get_pixel_box(editor_view_type *view,d3rect *box);
extern void walk_view_set_viewport_box(d3rect *box,bool erase,bool use_background);
extern void walk_view_set_viewport(editor_view_type *view,bool erase,bool use_background);
extern void walk_view_set_2D_projection(editor_view_type *view);
extern void walk_view_set_3D_projection(editor_view_type *view,int near_z,int far_z,int near_z_offset);
extern bool walk_view_project_point(editor_view_type *view,d3pnt *pnt);
extern editor_view_type* walk_view_get_current_view(void);
extern bool walk_view_point_in_view(editor_view_type *view,d3pnt *pnt);
extern void walk_view_cursor(d3pnt *pnt);
extern void walk_view_key(char ch);
extern void walk_view_get_position(d3pnt *pnt);
extern void walk_view_set_position(d3pnt *pnt);
extern void walk_view_set_position_y_shift(d3pnt *pnt,int y_shift);
extern void walk_view_move_position(d3pnt *pnt);
extern void walk_view_get_angle(d3ang *ang);
extern void walk_view_set_angle(d3ang *ang);
extern void walk_view_turn_angle(d3ang *ang);
extern void walk_view_perspective_ortho(bool on);
extern void walk_view_cull(bool on);
extern void walk_view_clip(bool on);
extern void walk_view_flip_clip(void);
extern int walk_view_get_uv_layer(void);
extern void walk_view_set_uv_layer(int uv_layer);
extern void walk_view_face_front(void);
extern void walk_view_face_left(void);
extern void walk_view_face_right(void);
extern void walk_view_face_back(void);
extern void walk_view_face_top(void);
extern void walk_view_face_bottom(void);
extern void walk_view_select_view(d3pnt *pnt);
extern bool walk_view_click(d3pnt *pnt,bool dblclick);
extern void walk_view_draw(void);

//
// walk view draw piece
//

extern bool walk_view_clip_poly(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly);
extern void walk_view_draw_select_mesh_get_grow_handles(int mesh_idx,int *px,int *py,int *pz);
extern void walk_view_draw_select_liquid_get_grow_handles(int liquid_idx,int *px,int *py,int *pz);
extern void walk_view_draw_view(editor_view_type *view);
extern void walk_view_draw_select(editor_view_type *view);

//
// view picking
//

extern bool view_pick_list_start(editor_view_type *view,bool in_2D,int count);
extern void view_pick_list_end(editor_view_type *view,d3pnt *pnt,int *type,int *main_idx,int *sub_idx);
extern int view_pick_list_end_multiple(editor_view_type *view,d3pnt *start_pnt,d3pnt *end_pnt,int *type,int *main_idx,int *sub_idx,int max_item);
extern void view_pick_list_add(int type,int main_idx,int sub_idx);
extern void view_pick_list_add_cube(d3pnt *v_pnts,int type,int main_idx,int sub_idx);
extern void view_pick_list_add_handle(int x,int y,int z,int type,int main_idx,int sub_idx);
extern void view_pick_list_add_2D_handle(int x,int y,int type,int main_idx,int sub_idx);

//
// walk view click
//

extern int walk_view_get_grid(void);
extern void walk_view_click_grid(d3pnt *pt);
extern bool walk_view_click_snap(int mesh_idx,d3pnt *pt);
extern bool walk_view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt);
extern bool walk_view_click_snap_mesh(int mesh_idx,d3pnt *pt);
extern void walk_view_click_piece_map_pick_single(editor_view_type *view,d3pnt *click_pt,int *type,int *main_idx,int *sub_idx);
extern int walk_view_click_piece_map_pick_multiple(editor_view_type *view,d3pnt *click_start_pt,d3pnt *click_end_pt,int *type,int *main_idx,int *sub_idx,int max_item);
extern void walk_view_click_piece(editor_view_type *view,d3pnt *pt,bool dblclick);
extern bool walk_view_click_drag_mesh_handle(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_mesh(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_mesh_poly(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_vertex(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_liquid_vertex(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_item(editor_view_type *view,d3pnt *pt);
extern void walk_view_click_box_select(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_liquid(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_texture(editor_view_type *view,d3pnt *pt,bool entire_mesh);

//
// walk view draw handle
//

extern bool view_handle_create_rot_handle(editor_view_type *view,d3pnt *center_pnt,d3pnt *hand_pnt);
extern void view_get_sprite_vertexes(d3pnt *pnt,d3ang *ang,d3pnt *v_pnts);

//
// walk view movement
//

extern void walk_view_mouse_get_scroll_horizontal_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void walk_view_mouse_get_scroll_vertical_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void walk_view_mouse_scroll_movement(editor_view_type *view,d3pnt *pnt);
extern void walk_view_mouse_forward_movement(editor_view_type *view,d3pnt *pnt);
extern void walk_view_scroll_wheel_z_movement(editor_view_type *view,int delta);
extern void walk_view_mouse_turn(editor_view_type *view,d3pnt *pnt);

//
// walk view models
//

extern void walk_view_models_start(void);
extern void walk_view_models_close(void);
extern void walk_view_models_reset(void);
extern void walk_view_get_model_size(char *model_name,d3pnt *size);
extern bool walk_view_model_draw(d3pnt *pnt,d3ang *ang,char *name,short *texture_frame,int frame_count);
extern void walk_view_model_cube_vertexes(char *name,d3pnt *pnt,d3ang *ang,d3pnt *v_pnts);