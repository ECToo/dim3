/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Interface Functions

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

//
// main window
//

extern void main_wind_initialize(void);
extern void main_wind_shutdown(void);
extern void main_wind_open(void);
extern void main_wind_close(void);
extern void main_wind_gl_setup(void);
extern void main_wind_draw(void);
extern void main_wind_draw_no_swap(void);
extern void main_wind_play(bool play,bool blend);
extern void main_wind_click(d3pnt *pnt,bool double_click);
extern void main_wind_scroll_wheel(d3pnt *pnt,int delta);
extern bool main_wind_cursor(void);
extern void main_wind_key(char ch);
extern void main_wind_resize(void);

//
// menus
//

extern void menu_update(void);
extern bool menu_save_changes_dialog(void);
extern bool menu_event_run(int cmd);

//
// dos
//

extern void open_model_xml(void);
extern void close_model_xml(void);

//
// undo
//

extern void undo_initialize(void);
extern void undo_clear(void);
extern void undo_set_bone_move(int pose_idx,int bone_idx);
extern void undo_run(void);

//
// tool palette
//

extern void tool_palette_setup(void);

//
// texture palette
//

extern void texture_palette_setup(void);
extern int texture_palette_get_selected_texture(void);
extern void texture_palette_draw(texture_type *txt_list);

//
// item list palette
//

extern void item_palette_initialize(void);
extern void item_palette_shutdown(void);
extern void item_palette_setup(void);
extern void item_palette_draw(void);
extern void item_palette_reset(void);
extern void item_palette_select(int sel_type,int sel_idx);
extern bool item_palette_delete(void);
extern void item_palette_scroll_wheel(d3pnt *pnt,int move);
extern void item_palette_click(d3pnt *pnt,bool double_click);

//
// property list palette
//

extern void property_palette_initialize(void);
extern void property_palette_shutdown(void);
extern void property_palette_setup(void);
extern void property_palette_draw(void);
extern void property_palette_reset(void);
extern void property_palette_scroll_wheel(d3pnt *pnt,int move);
extern void property_palette_click(d3pnt *pnt,bool double_click);

//
// importing
//

extern bool import_obj(char *path,bool replace,bool *found_normals,char *err_str);
extern void insert_model(char *file_name);

extern void clear_materials(void);
extern int texture_count(void);
extern bool texture_exists(char *material_name);
extern int texture_pick(char *material_name,char *err_str);
extern bool texture_use_single(void);

//
// models
//

extern void model_wind_setup(void);
extern void model_wind_click(d3pnt *pnt);

extern void draw_model(int mesh_idx);
extern void draw_model_mesh(int mesh_idx);
extern void draw_model_bones(int sel_bone_idx);
extern void draw_model_bone_names(int sel_bone_idx);

extern void draw_model_selected_vertexes(int mesh_idx);
extern void draw_model_selected_trig(int mesh_idx);
extern void draw_model_box_view(void);
extern void draw_model_box_hit_boxes(void);
extern void draw_model_axis(void);
extern void draw_model_normals_vertexes(int mesh_idx);
extern void draw_model_normals_trig(int mesh_idx);

extern void draw_model_gl_setup(int z_offset);
extern void draw_model_gl_setup_2D(void);
extern void draw_model_2D_transform_setup(void);
extern void draw_model_2D_transform(d3fpnt *pnt,d3pnt *tran_pnt);

extern void draw_model_setup_bones_vertexes(int mesh_idx);
extern void draw_model_setup_pose(int pose_idx);
extern void draw_model_wind_pose(int mesh_idx,int pose_idx);
extern void draw_model_wind(int mesh_idx);

//
// vertex masks
//

extern bool vertex_mask_initialize(void);
extern void vertex_mask_shutdown(void);
extern void vertex_clear_sel_mask(int mesh_idx);
extern void vertex_set_sel_mask(int mesh_idx,int vertex_idx,bool value);
extern void vertex_set_sel_mask_all(int mesh_idx);
extern bool vertex_check_any(int mesh_idx);
extern bool vertex_check_sel_mask(int mesh_idx,int vertex_idx);
extern void vertex_clear_hide_mask(int mesh_idx);
extern void vertex_set_hide_mask(int mesh_idx,int vertex_idx,bool value);
extern bool vertex_check_hide_mask(int mesh_idx,int vertex_idx);
extern void vertex_hide_mask_set_sel_vertexes(int mesh_idx);
extern void vertex_hide_mask_set_non_sel_vertexes(int mesh_idx);
extern void vertex_hide_mask_show_all_vertexes(int mesh_idx);
extern bool vertex_check_hide_mask_trig(int mesh_idx,model_trig_type *trig);
extern void vertex_set_sel_mask_bone(int mesh_idx,int bone_idx);
extern void vertex_set_sel_mask_no_bone(int mesh_idx);
extern void vertex_set_sel_mask_near_bone(int mesh_idx,int bone_idx,float percentage);
extern void vertex_set_sel_vertex_to_bone(int mesh_idx,int major_bone_idx,int minor_bone_idx,float factor);
extern void vertex_set_sel_mask_material(int mesh_idx,int material_idx);

//
// vertex calcs
//

extern void vertex_find_center_sel_vertexes(int mesh_idx,int *p_cx,int *p_cy,int *p_cz);
extern void vertex_move_sel_vertexes(int mesh_idx,int x,int y,int z);
extern void vertex_scale_sel_vertexes(int mesh_idx,float x,float y,float z);
extern void vertex_rotate_sel_vertexes(int mesh_idx,float ang_x,float ang_y,float ang_z);
extern void vertex_invert_normals(int mesh_idx);
extern void vertex_set_normals_vertexes(int mesh_idx,d3vct *normal);
extern void vertex_set_normals(int mesh_idx);
extern void vertex_clear_bone_attachments_sel_vertexes(int mesh_idx);
extern void vertex_delete_sel_vertex(int mesh_idx);
extern void vertex_delete_unused_vertexes(int mesh_idx);

//
// model picking
//

extern bool model_pick_list_start(int count);
extern void model_pick_list_end(d3pnt *pnt,int *idx);
extern void model_pick_list_add_trig(int idx,d3pnt *v_pnts);

//
// dialogs
//

extern void dialog_property_list_run(char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,int *idx);
extern void dialog_property_string_run(int value_type,void *value,int value_len,int i_min,int i_max);