/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Definitions Header

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
// GUI Defines
//

#define max_element								128
#define max_element_str_len						256
#define max_element_value_str_len				64
#define max_element_column						32
#define max_element_tab							8

//
// GUI elements
//

#define element_type_button						0
#define element_type_bitmap						1
#define element_type_text						2
#define element_type_text_field					3
#define element_type_number						4
#define element_type_checkbox					5
#define element_type_combo						6
#define element_type_slider						7
#define element_type_table						8
#define element_type_tab						9
#define element_type_color						10
#define element_type_text_box					11
#define element_type_info_field					12
#define element_type_model						13

//
// GUI position mode
//

#define element_pos_top							0
#define element_pos_bottom						1
#define element_pos_center						2

#define element_pos_left						0
#define element_pos_right						1

//
// GUI table bitmap mode
//

#define element_table_bitmap_none				0
#define element_table_bitmap_data				1
#define element_table_bitmap_document			2

//
// font selection
//

#define font_interface_index					0
#define font_hud_index							1

//
// GUI scaling factors
//

#define cursor_size_factor						0.05f

#define text_height_factor						1.2f

#define text_small_wid_factor					0.02f
#define text_medium_wid_factor					0.025f
#define text_large_wid_factor					0.038f

#define element_control_padding_factor			0.01f

#define element_control_draw_height				0.025f
#define element_control_draw_short_width		0.15f
#define element_control_draw_long_width			0.4f

#define element_table_max_image					16
#define element_table_bitmap_size				64
#define element_table_max_check					128
#define element_table_check_column_size			0.05f

//
// gradients
//

#define element_gradient_factor					0.75f

//
// screenshot backgrounds
//

#define gui_screenshot_blur_count				10

//
// GUI element structures
//

typedef struct		{
						int						image_idx,image_select_idx;
						char					name[name_str_len];
						bool					text_only;
					} element_button_type;

typedef struct		{
						int						size,just;
						bool					alert;
					} element_text_type;

typedef struct		{
						int						min,max;
					} element_number_type;
					
typedef struct		{
						float					value,min,max;
					} element_slider_type;

typedef struct		{
						float					percent_size;
						char					name[name_str_len];
					} element_column_type;

typedef struct		{
						int						image_idx;
						char					path[1024];
					} element_image_type;

typedef struct		{
						int						ncolumn,next_image_idx,bitmap_mode,
												busy_count,busy_total_count;
						char					busy_str[256],checks[element_table_max_check];
						bool					checkbox;
						element_column_type		cols[max_element_column];
						element_image_type		images[element_table_max_image];
					} element_table_type;

typedef struct		{
						int						ntab;
						char					name[max_element_tab][name_str_len];
					} element_tab_type;

typedef struct		{
						int						line_count,pos_y;
						bool					scroll_up_ok,scroll_down_ok;
					} element_text_box_type;

typedef struct		{
						model_draw				*draw;
					} element_model_type;

typedef union		{
						element_button_type		button;
						element_text_type		text;
						element_number_type		number;
						element_slider_type		slider;
						element_table_type		table;
						element_tab_type		tab;
						element_text_box_type	text_box;
						element_model_type		model;
					} element_setup_type;

typedef struct		{
						int						id,type,x,y,wid,high,
												value,offset,max_value_str_sz;
						bool					selectable,enabled,hidden,framed;
						char					str[max_element_str_len],
												value_str[max_element_value_str_len],
												*data;
						element_setup_type		setup;
					} element_type;

//
// HUD defines
//

#define max_hud_image							300	

#define max_hud_bitmap							256
#define max_hud_text							256
#define max_hud_bar								32

#define max_radar_icon							16

#define max_hud_text_str_sz						256

#define max_menu								16
#define max_menu_item							24
#define max_menu_item_data_sz					64

#define max_chooser								128
#define max_chooser_piece						64
#define max_chooser_frame_text_sz				64
#define max_chooser_text_data_sz				256
#define max_chooser_button_text_sz				64

#define max_chooser_sub_txt						16

#define max_tint_color							8

#define max_character							32

#define max_net_bot								16
#define max_net_game							32
#define max_net_option							32

//
// text specials
//

#define text_special_none						0
#define text_special_fps						1
#define text_special_score						2
#define text_special_place						3
#define text_special_spread						4

#define text_special_list_def					{"none","fps","score","place","spread",""}

//
// simple save spots
//

#define max_simple_save_spot					5

//
// chat
//

#define max_chat_lines							15
#define chat_str_len							64

//
// console
//

#define max_console_line						14
#define max_console_txt_sz						128

typedef struct		{
						int						style;
						char					txt[max_console_txt_sz];
						d3col					color;
					} console_line_type;

//
// color and font settings
//

typedef struct		{
						d3col					dialog_background,dialog_base,dialog_dimmed,dialog_outline,
												control_label,control_text,control_fill,
												control_header,control_outline,
												control_mouse_over,control_hilite,control_disabled,
												button_fill,button_text,button_outline,
												default_tint,tints[max_tint_color];
					} hud_color_type;

typedef struct		{
						int						text_size_small,text_size_medium,text_size_large;
						char					interface_name[name_str_len],alt_interface_name[name_str_len],
												hud_name[name_str_len],alt_hud_name[name_str_len];
					} hud_font_type;

//
// bitmaps, text, bars
//

typedef struct		{
						int						start_tick,
												fade_in_tick,life_tick,fade_out_tick;
						bool					on;
					} hud_item_fade_type;
					
typedef struct		{
						int						x_add,y_add,col,row,count;
						bool					on;
					} hud_bitmap_repeat_type;
					
typedef struct		{
						int						x,y,x_size,y_size,image_idx,show_tick;
						float					alpha,rot;
						char					name[name_str_len],filename[file_str_len];
						bool					show,old_show,
												flash,flip_horz,flip_vert,team_tint;
						hud_bitmap_repeat_type	repeat;
						image_animation_type	animate;
						hud_item_fade_type		fade;
					} hud_bitmap_type;

typedef struct		{
						int						x,y,size,just,special;
						float					alpha;
						char					name[name_str_len],data[max_hud_text_str_sz];
						bool					show,old_show,has_return;
						d3col					color;
						hud_item_fade_type		fade;
					} hud_text_type;
					
typedef struct		{
						int						x,y,x_size,y_size;
						float					fill_alpha,outline_alpha,value;
						char					name[name_str_len];
						bool					show,old_show,outline,vert;
						d3col					fill_start_color,fill_end_color,outline_color;
					} hud_bar_type;

//
// radar
//

typedef struct		{
						int						size,image_idx;
						char					name[name_str_len],bitmap_name[name_str_len];
						bool					rot;
					} hud_radar_icon_type;

typedef struct		{
						int						x,y,display_radius,view_radius,
												background_image_idx,nicon,no_motion_fade;
						bool					on,rot,team_tint;
						char					background_bitmap_name[name_str_len];
						hud_radar_icon_type		icons[max_radar_icon];
					} hud_radar_type;
			
//
// menus
//

typedef struct		{
						int						item_id;
						char					data[max_menu_item_data_sz],sub_menu[name_str_len];
						bool					multiplayer_disable,quit;
					} menu_item_type;
					
typedef struct		{
						int						nitem;
						char					name[name_str_len];
						menu_item_type			items[max_menu_item];
					} menu_type;
					
//
// chooser
//

#define chooser_piece_type_text					0
#define chooser_piece_type_item					1
#define chooser_piece_type_model				2
#define chooser_piece_type_button				3

typedef struct		{
						int						size,just;
						char					str[max_chooser_text_data_sz];
					} chooser_text_type;

typedef struct		{
						char					file[file_str_len];
					} chooser_item_type;

typedef struct		{
						float					resize;
						char					model_name[name_str_len],animate_name[name_str_len];
						d3ang					rot;
					} chooser_model_type;

typedef struct		{
						char					name[max_chooser_button_text_sz];
					} chooser_button_type;

typedef union		{
						chooser_text_type		text;
						chooser_item_type		item;
						chooser_model_type		model;
						chooser_button_type		button;
					} chooser_piece_data_type;

typedef struct		{
						int						type,id,
												x,y,wid,high;
						bool					clickable;
						char					goto_name[name_str_len];
						chooser_piece_data_type	data;
					} chooser_piece_type;

typedef struct		{
						int						x,y,wid,high;
						char					title[max_chooser_frame_text_sz];
						bool					on;
						d3col					background_col;
					} chooser_frame_type;
					
typedef struct		{
						int						ok_id,cancel_id;
					} chooser_key_type;
					
typedef struct		{
						int						npiece;
						char					name[name_str_len];
						chooser_frame_type		frame;
						chooser_key_type		key;
						chooser_piece_type		pieces[max_chooser_piece];
					} chooser_type;
					
//
// progress
//

typedef struct		{
						int						lx,rx,ty,by,text_size;
						bool					outline;
						d3col					base_color_start,base_color_end,
												hilite_color_start,hilite_color_end,
												text_color,outline_color;
					} hud_progress_type;
					
//
// intro
//

typedef struct		{
						int								x,y,wid,high,element_id;
						bool							on;
					} hud_intro_button_type;

typedef struct		{
						int								x,y,text_size;
					} hud_intro_simple_save_desc_type;

typedef struct		{
						hud_intro_button_type			button_start,button_erase;
						hud_intro_simple_save_desc_type	desc;
					} hud_intro_simple_save_type;
					
typedef struct		{
						hud_intro_button_type			button_game_new,button_game_load,button_game_setup,
														button_game_new_easy,button_game_new_medium,button_game_new_hard,button_game_new_cancel,
														button_multiplayer_host,button_multiplayer_join,button_multiplayer_setup,
														button_credit,button_quit;
						hud_intro_simple_save_type		simple_save[max_simple_save_spot];
					} hud_intro_type;

//
// hud fades
//

typedef struct		{
						int						title_msec,
												map_msec;
					} hud_fade_type;

//
// simple save structures
//

typedef struct		{
						int						save_id;
						char					desc[64];
					} hud_simple_save_type;

typedef struct		{
						hud_simple_save_type	saves[max_simple_save_spot];
					} hud_simple_save_list;
					
//
// network structures
//

typedef struct		{
						char					name[name_str_len],str[chat_str_len];
						d3col					col;
					} hud_chat_line_type;

typedef struct		{
						int						nline,x,y,
												last_add_life_sec,next_life_sec,
												remove_tick;
						char					type_str[chat_str_len];
						bool					type_on;
						hud_chat_line_type		*lines;
					} hud_chat_type;

typedef struct		{
						bool					on;
					} hud_score_type;

typedef struct		{
						char					name[name_str_len],model_name[name_str_len],
												param[name_str_len];
						float					interface_resize;
						d3pnt					interface_offset;
					} hud_character_item_type;

typedef struct		{
						int						ncharacter;
						hud_character_item_type	characters[max_character];
					} hud_character_type;
	
typedef struct		{
						char					name[name_str_len];
					} hud_net_bot_type;

typedef struct		{
						bool					on;
						hud_net_bot_type		bots[max_net_bot];
					} hud_net_bots_type;

typedef struct		{
						char					name[name_str_len];
						bool					use_teams,monsters;
					} hud_net_game_type;

typedef struct		{
						int						ngame;
						hud_net_game_type		games[max_net_game];
					} hud_net_games_type;

typedef struct		{
						char					name[name_str_len],descript[64];
					} hud_net_option_type;

typedef struct		{
						int						noption;
						hud_net_option_type		options[max_net_option];
					} hud_net_options_type;
					
typedef struct		{
						int						port;
						char					host[64],url[256];
					} hud_net_news_type;
					
//
// HUD counts
//

typedef struct		{
						int						bitmap,text,bar,menu,chooser;
					} hud_count_type;
					
//
// main HUD structure
//
 
typedef struct		{
						int						scale_x,scale_y;
						char					project_name[name_str_len],
												click_sound[name_str_len],
												intro_music[name_str_len];
						bool					debug,skill;
						hud_count_type			count;
						hud_color_type			color;
						hud_font_type			font;
						hud_progress_type		progress;
						hud_intro_type			intro;
						hud_fade_type			fade;
						hud_bitmap_type			*bitmaps;
						hud_text_type			*texts;
						hud_bar_type			*bars;
						menu_type				*menus;
						chooser_type			*choosers;
						hud_radar_type			radar;
						hud_chat_type			chat;
						hud_score_type			score;
						hud_character_type		character;
						hud_simple_save_list	simple_save_list;
						hud_net_bots_type		net_bot;
						hud_net_games_type		net_game;
						hud_net_options_type	net_option;
						hud_net_news_type		net_news;
					} hud_type;
					

