/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Common UI Definitions

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

//
// list palette
//

#define list_max_item_count									4096
#define list_max_section_count								32

#define list_title_font_size								14.0f
#define list_item_font_size									12.0f
#define list_item_font_high									14
#define list_item_scroll_size								(list_item_font_high*5)

#define list_palette_border_sz								10
#define list_palette_tree_sz								250
#define list_palette_scroll_wid								15
#define list_title_high										20

#define list_item_ctrl_header								0
#define list_item_ctrl_text									1
#define list_item_ctrl_color								2
#define list_item_ctrl_string								3
#define list_item_ctrl_checkbox								4
#define list_item_ctrl_pick_color							5

#define list_string_value_string							0
#define list_string_value_int								1
#define list_string_value_positive_int						2
#define list_string_value_range_int							3
#define list_string_value_float								4
#define list_string_value_positive_float					5
#define list_string_value_0_to_1_float						6
#define list_string_value_point								7
#define list_string_value_angle								8
#define list_string_value_vector							9
#define list_string_value_uv								10
#define list_string_value_tag								11

typedef union		{
						bool								checkbox;
						char								str[32];
						d3col								col;
					} list_palette_item_value_type;

typedef struct		{
						int									x,y,ctrl_type,
															id,type,count,idx;
						bool								selected,disabled;
						char								name[name_str_len];
						list_palette_item_value_type		value;
					} list_palette_item_type;

typedef struct		{
						int									item_count,push_idx,
															item_id,item_type,item_idx,
															pixel_sz,scroll_page,
															total_high;
						bool								push_on;
						char								title[name_str_len];
						d3rect								box;
						list_palette_item_type				*items;
					} list_palette_type;

//
// property utilities
//

extern void property_string_get_values(int value_type,void *value,int i_min,int i_max,char *str,char *desc);
extern void property_string_set_values(int value_type,void *value,int value_len,int i_min,int i_max,char *str);

extern void property_pick_list(char *list,int *idx);

extern void property_get_parameter(int idx,char *param_list,char *str);
extern void property_set_parameter(int idx,char *param_list,char *str);

//
// text
//

extern void text_initialize(void);
extern void text_shutdown(void);
extern void text_draw(int x,int y,float txt_size,d3col *col,char *str);
extern void text_draw_center(int x,int y,float txt_size,d3col *col,char *str);
extern void text_draw_right(int x,int y,float txt_size,d3col *col,char *str);

//
// progress
//

extern void progress_start(char *title,int count);
extern void progress_end(void);
extern void progress_next(void);
extern void progress_next_title(char *title);

//
// list palette
//

extern void list_palette_initialize(void);
extern void list_palette_shutdown(void);
extern void list_palette_list_initialize(list_palette_type *list,char *title);
extern void list_palette_list_shutdown(list_palette_type *list);
extern void list_palette_set_title(list_palette_type *list,char *title);
extern void list_palette_add_header(list_palette_type *list,int piece_type,char *name);
extern void list_palette_add_header_count(list_palette_type *list,int piece_type,char *name,int count);
extern void list_palette_add_item(list_palette_type *list,int piece_type,int piece_idx,char *name,bool selected,bool disabled);
extern void list_palette_add_color(list_palette_type *list,int piece_type,int piece_idx,d3col *col,bool selected,bool disabled);
extern void list_palette_add_string_selectable(list_palette_type *list,int id,char *name,char *value,bool selected,bool disabled);
extern void list_palette_add_string(list_palette_type *list,int id,char *name,char *value,bool disabled);
extern void list_palette_add_string_int(list_palette_type *list,int id,char *name,int value,bool disabled);
extern void list_palette_add_string_float(list_palette_type *list,int id,char *name,float value,bool disabled);
extern void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool value,bool disabled);
extern void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col,bool disabled);
extern void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt,bool disabled);
extern void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang,bool disabled);
extern void list_palette_add_vector(list_palette_type *list,int id,char *name,d3vct *vct,bool disabled);
extern void list_palette_add_uv(list_palette_type *list,int id,char *name,d3fpnt *fpnt,bool disabled);
extern void list_palette_add_texture(list_palette_type *list,texture_type *textures,int id,char *name,int txt_idx,bool disabled);
extern void list_palette_add_shader(list_palette_type *list,int id,char *name,char *shader_name,bool disabled);
extern void list_palette_add_string_tag(list_palette_type *list,int id,char *name,unsigned long tag,bool disabled);
extern void list_palette_delete_all_items(list_palette_type *list);
extern void list_palette_draw(list_palette_type *list,bool close_border);
extern void list_palette_scroll_up(list_palette_type *list);
extern void list_palette_scroll_down(list_palette_type *list);
extern void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move);
extern bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click);

//
// tool palette
//

extern void tool_palette_initialize(char *app_name);
extern void tool_palette_shutdown(void);
extern void tool_palette_draw(void);
extern void tool_palette_click(d3pnt *pnt);

//
// text palette
//

extern void texture_palette_draw(texture_type *txt_list);
extern void texture_palette_click(texture_type *txt_list,d3pnt *pnt,bool dbl_click);

//
// texture editing
//

#define texture_edit_item_high						140
#define texture_edit_scroll_wheel_move				25

#define texture_edit_frame_click_return_idx			100
#define texture_edit_frame_click_delete_idx			101

extern void texture_edit_switch(int texture_idx);
extern void texture_edit_draw(void);
extern bool texture_edit_cursor(void);
extern void texture_edit_scroll_wheel(int delta);
extern bool texture_edit_click(d3pnt *pnt,bool double_click);

//
// import text decoder
//

extern int textdecode_count_linestarts(void);
extern void textdecode_linestarts(void);
extern bool textdecode_open(char *path,char p_piece_break);
extern void textdecode_close(void);
extern int textdecode_count(void);
extern int textdecode_find(int str_line,char *txt);
extern void textdecode_get_line(int i,char *txt);
extern void textdecode_get_piece(int i,int k,char *txt);

