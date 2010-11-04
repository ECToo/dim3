/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Dialog Functions

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
// dialog utility
//

extern void dialog_open(WindowRef *wind,char *name);
extern void dialog_set_text(WindowRef wind,unsigned long sig,int id,char *str);
extern void dialog_get_text(WindowRef wind,unsigned long sig,int id,char *str,int len);
extern void dialog_set_int(WindowRef wind,unsigned long sig,int id,int value);
extern int dialog_get_int(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_float(WindowRef wind,unsigned long sig,int id,float value);
extern float dialog_get_float(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_boolean(WindowRef wind,unsigned long sig,int id,bool b);
extern bool dialog_get_boolean(WindowRef wind,unsigned long sig,int id);
extern void dialog_clear_combo(WindowRef wind,unsigned long sig,int id);
extern void dialog_add_combo_item(WindowRef wind,unsigned long sig,int id,char *str,unsigned long cmd);
extern void dialog_set_combo(WindowRef wind,unsigned long sig,int id,int idx);
extern int dialog_get_combo(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_radio_buttons(WindowRef wind,unsigned long sig,int value,int count);
extern int dialog_get_radio_buttons(WindowRef wind,unsigned long sig,int count);
extern void dialog_set_value(WindowRef wind,unsigned long sig,int id,int value);
extern int dialog_get_value(WindowRef wind,unsigned long sig,int id);
extern void dialog_switch_tab(WindowRef wind,unsigned long sig,int id,int npane);
extern void dialog_set_tab(WindowRef wind,unsigned long sig,int id,int index,int npane);
extern void dialog_set_color(WindowRef wind,unsigned long sig,int id,d3col *col);
extern void dialog_get_color(WindowRef wind,unsigned long sig,int id,d3col *col);
extern void dialog_set_focus(WindowRef wind,unsigned long sig,int id);
extern void dialog_enable(WindowRef wind,unsigned long sig,int id,bool enable);
extern void dialog_hide(WindowRef wind,unsigned long sig,int id,bool show);
extern void dialog_set_numeric_only(WindowRef wind,int sig,int id);
extern void dialog_redraw(WindowRef wind,unsigned long sig,int id);
extern void dialog_special_combo_fill_hud_text(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_hud_bitmap(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_sound(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_particle(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_ring(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_shader(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_spot(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_node(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_movement(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_get_hud_text(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_hud_bitmap(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_sound(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_particle(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_ring(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_shader(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_spot(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_node(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_movement(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_fill_group_combo(WindowRef wind,unsigned long sig,int id,int idx);
extern int dialog_get_group_combo(WindowRef wind,unsigned long sig,int id);
extern void dialog_fill_texture_combo(WindowRef wind,unsigned long sig,int id,bool none,int idx);
extern int dialog_get_texture_combo(WindowRef wind,unsigned long sig,int id,bool none);

//
// editor dialogs
//

extern void dialog_about_run(void);
extern void dialog_preference_run(void);

//
// open/new/save dialogs
//

extern bool dialog_file_new_run(char *title,char *file_name);
extern bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *required_file_name,char *file_name);

//
// map dialogs
//

extern bool dialog_map_settings_run(void);
extern bool dialog_map_cinemas_run(void);
extern bool dialog_cinema_settings_run(map_cinema_type *cinema);
extern bool dialog_cinema_action_settings_run(map_cinema_action_type *action);
extern bool dialog_map_groups_run(void);
extern bool dialog_map_movements_run(void);
extern bool dialog_movement_settings_run(movement_type *movement);
extern bool dialog_movement_move_settings_run(movement_move_type *move);
extern bool dialog_optimize_run(int *poly_threshold);
extern bool dialog_light_map_run(void);

//
// piece dialogs
//

extern bool dialog_group_settings_run(group_type *group);
extern bool dialog_resize_run(float *fct_x,float *fct_y,float *fct_z);
extern bool dialog_reposition_run(d3pnt *min,d3pnt *max);
extern bool dialog_skew_run(int *axis,int *dir,int *size);
extern bool dialog_free_rotate_run(float *rot_x,float *rot_y,float *rot_z);

//
// mesh dialogs
//

extern bool dialog_create_grid_mesh_run(int *xdiv,int *ydiv,int *zdiv);
extern bool dialog_mesh_scale_run(d3fpnt *min,d3fpnt *max,bool replace_ok,float old_scale,float *scale);

//
// texture dialogs
//

extern void dialog_texture_setting_run(int txt_idx);

//
// import dialogs
//

extern bool dialog_height_import_run(int *div_cnt,int *size,int *high);
extern bool dialog_map_auto_generate_setting_run(void);


