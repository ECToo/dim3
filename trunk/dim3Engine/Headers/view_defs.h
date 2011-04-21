/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Definitions Header

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
// vertex objects
//

#define view_vertex_object_count							32

//
// back renderers
//

#define back_render_texture_pixel_size						256

//
// view maximums
//

#define max_view_render_item								5120

#define max_light_spot										128				// maximum number of lights in a scene
#define max_shader_light									4				// maximum number of lights passed to a shader

#define join_info_max_players								24				// needs to reflect host_max_remote_count in network_defs.h

//
// shaders
//

#define max_shader_custom_vars								8

#define max_core_shader_data_sz								10240

#define max_core_shader										12

#define gl_core_map_shader_light_map						0
#define gl_core_map_shader_light_map_bump					1
#define gl_core_map_shader_light_map_bump_spec				2
#define gl_core_model_shader_light							3
#define gl_core_model_shader_light_bump						4
#define gl_core_model_shader_light_bump_spec				5
#define gl_core_map_shader_fog_light_map					6
#define gl_core_map_shader_fog_light_map_bump				7
#define gl_core_map_shader_fog_light_map_bump_spec			8
#define gl_core_model_shader_fog_light						9
#define gl_core_model_shader_fog_light_bump					10
#define gl_core_model_shader_fog_light_bump_spec			11

#define gl_shader_core_index								1000

//
// diffuse
//

#define gl_diffuse_ambient_factor							0.75f

//
// shadows
//

#define view_shadows_poly_count								256
#define view_shadows_vertex_count							10240
#define view_shadows_infinite_light_shift					100
#define view_shadows_bounds_check_slop						100
#define view_shadows_clip_infinite_distance					100000

//
// input rate
//

#define input_tick_rate										33

//
// stencil constants
//

#define stencil_none										0
#define stencil_poly_start									1
#define stencil_poly_end									255

#define stencil_shadow										1

//
// remote names
//

#define remote_name_max_distance							75000				// how far away you can see map names
#define remote_name_min_distance							15000				// closest you can see map names in full

#define remote_status_icon_size								300
//
// render item type
//

#define view_render_type_mesh								0
#define view_render_type_liquid								1
#define view_render_type_object								2
#define view_render_type_projectile							3
#define view_render_type_effect								4

//
// obscure settings
//

#define obscure_grid_division								256
#define obscure_grid_byte_row_size							(obscure_grid_division>>3)
#define obscure_grid_byte_size								(obscure_grid_byte_row_size*obscure_grid_division)

#define obscure_grid_max_box_side_length					10

//
// shader variable types
//

#define shader_var_type_int									0
#define shader_var_type_float								1
#define shader_var_type_vec3								2
#define shader_var_type_vec4								3

//
// console size
//

#define console_screen_percent								0.4f


//
// screen sizes
//

#define max_screen_size									64

typedef struct		{
						int								wid,high;
					} screen_size_type;
					
//
// render info
//
 
#define max_fps											100

typedef struct		{
						int								view_x,view_y,
														monitor_x_sz,monitor_y_sz,
														monitor_refresh_rate,
														texture_unit_count,texture_max_size,
														nscreen_size;
						char							name[64],ext_string[8192];
						screen_size_type				screen_sizes[max_screen_size];
					} render_info_type;

//
// image structures
//

#define max_view_image										1024
#define max_view_image_bitmap								32

typedef struct		{
						int									msec;
						bitmap_type							bitmap;
					} view_image_bitmap_type;

typedef struct		{
						int									nbitmap,total_msec;
						char								path[1024];
						view_image_bitmap_type				bitmaps[max_view_image_bitmap];
					} view_image_type;

//
// lighting structures
//

typedef struct		{
						int									direction,i_intensity;
						bool								light_map;
						float								f_x,f_y,f_z,
															f_intensity,f_inv_intensity,f_exponent;
						double								d_x,d_y,d_z;
						d3pnt								pnt;
						d3vct								pnt_eye_space;
						d3col								col;
					} view_light_spot_type;

typedef struct		{
						int									nlight,light_idx[max_shader_light];
						bool								hilite;
						float								diffuse_boost;
						d3vct								diffuse_vct;
					} view_light_list_type;

//
// join UI structures
//

typedef struct		{
						int									score;
						bool								bot;
						char								name[name_str_len];
					} join_server_info_player;

typedef struct		{
						int									count,max_count;
						join_server_info_player				players[join_info_max_players];
					} join_server_info_player_list;

typedef struct		{
						int									ping_msec,
															score_limit,option_flags,
															respawn_secs,game_reset_secs;
						char								ip[256],name[name_str_len],
															game_name[name_str_len],map_name[name_str_len];
						join_server_info_player_list		player_list;
					} join_server_info;

//
// halo structures
//
 
typedef struct		{
						int									idx,obj_idx,pixel_sz;
						float								alpha;
						bool								in_view;
						d3pnt								pnt,proj_pnt;
					} halo_draw_type;

//
// rain structures
//

typedef struct		{
						int									x,z,y,by,
															x_off,z_off;
					} rain_draw_type;

//
// chat, console and score structures
//

#define max_view_chat_lines								15
#define max_view_chat_str_len							64

#define max_view_console_lines							14
#define max_view_console_txt_sz							128

typedef struct		{
						char							name[name_str_len],str[max_view_chat_str_len];
						d3col							col;
					} view_chat_line_type;

typedef struct		{
						int								nline,remove_tick;
						char							type_str[max_view_chat_str_len];
						bool							type_on;
						view_chat_line_type				*lines;
					} view_chat_type;

typedef struct		{
						int								style;
						char							txt[max_view_console_txt_sz];
						d3col							color;
					} view_console_line_type;
					
typedef struct		{
						int								nline;
						bool							on;
						view_console_line_type			*lines;
					} view_console_type;

typedef struct		{
						bool							on;
					} view_score_type;

//
// timing structures
//

typedef struct		{
						int									input_tick,draw_tick,draw_time,
															run_tick;
					} view_time_type;

//
// fps type
//

typedef struct		{
						int									tick,count,last_time;
						float								total;
						bool								first_skip;
					} view_fps_type;

//
// shader structures
//

typedef union		{
						int									i;
						float								f;
						float								vec3[3];
						float								vec4[4];
					} shader_value_type;
					
typedef struct		{
						int									var_type;
						char								name[name_str_len];
						shader_value_type					value;
					} shader_custom_var_type;

typedef struct		{
						GLint								position,color,
															intensity,inv_intensity,exponent,
															direction,inLightMap;					
					} shader_cached_var_light_loc;
					
typedef struct		{
						GLint								dim3FrequencySecond,
															dim3CameraPosition,dim3AmbientColor,
															dim3LightMapBoost,dim3ShineFactor,
															dim3Alpha,dim3DiffuseVector,dim3DiffuseBoost,
															dim3VertexTangent,dim3VertexNormal;
						shader_cached_var_light_loc			dim3Lights[max_shader_light];
					} shader_cached_var_loc;

typedef struct		{
						int									light_map,direction;
						float								intensity,inv_intensity,exponent;
						d3fpnt								position;
						d3col								color;
					} shader_current_var_light_value;

typedef struct		{
						int									nlight,light_idx[max_shader_light],
															tangent_offset,normal_offset;
						float								alpha,shine_factor,diffuse_boost;
						d3vct								diffuse_vct;
						shader_current_var_light_value		lights[max_shader_light];
					} shader_current_var_value;

typedef struct		{
						int									start_tick;
						char								name[64],
															vertex_name[64],fragment_name[64];
						bool								per_scene_vars_set,in_hilite;
						GLhandleARB							vertex_obj,fragment_obj,program_obj;
						shader_cached_var_loc				var_locs;
						shader_current_var_value			var_values;
					} shader_type;

//
// camera structure
//

typedef struct		{
						int									node_seek_idx,node_dest_idx,
															total_dist,msec,event_id,
															start_tick,end_tick;
						float								speed;
						bool								on,open_doors,in_freeze;
						d3pnt								start_pnt,end_pnt;
						d3ang								start_ang,end_ang;
					} camera_auto_walk_type;

typedef struct		{
						int									ang_end_tick;
						bool								ang_on;
						d3ang								ang;
					} camera_auto_move_type;

typedef struct		{
						int									obj_idx;
						d3ang								cur_chase_ang;
						map_camera_type						setup;
						camera_auto_walk_type				auto_walk;
						camera_auto_move_type				auto_move;
					} camera_type;

//
// view counts
//

typedef struct		{
						int									poly,liquid,model,shadow,effect;
					} view_count_type;

//
// view rendering structure
//

#define view_list_item_flag_model_in_view					0x1
#define view_list_item_flag_shadow_in_view					0x2

typedef struct		{
						int									under_liquid_idx,z_adjust;
						float								fov;
						bool								flip;
						d3pnt								pnt;
						d3ang								ang;
					} view_render_camera_type;
	
typedef struct		{
						int									flag;
						short								type,idx;
						double								dist;
					} view_render_draw_list_item_type;
					
typedef struct		{
						int									count;
						view_render_draw_list_item_type		items[max_view_render_item];
					} view_render_draw_list_type;

typedef struct		{
						int									count;
						view_light_spot_type				spots[max_light_spot];
					} view_render_light_type;

typedef struct		{
						int									count,in_view_count;
						halo_draw_type						halos[max_light_spot];
					} view_render_halo_type;

typedef struct		{
						bool								force_camera_obj;
						view_render_camera_type				camera;
						view_render_draw_list_type			draw_list;
						view_render_light_type				light;
						view_render_halo_type				halo_draw;
					} view_render_type;

//
// obscuring
//

typedef struct		{
						int									x_start,z_start,
															x_size,z_size;
						unsigned char						*grid;
					} view_obscure_type;

//
// UI states
//

typedef struct		{
						int									fade_start_tick,menu_idx,
															click_item_idx;
						bool								fade_in,fade_out,active,
															mouse_down;
					} view_menu_type;

typedef struct		{
						int									idx,event_id,
															start_tick,last_tick;
						bool								on;
						camera_type							camera_state;
					} view_cinema_type;

typedef struct		{
						char								str[256],str_2[256];
					} view_error_type;

typedef struct		{
						bool								on;
					} view_debug_type;

//
// main view structure
//
 
typedef struct		{
						view_count_type						count;
						view_render_type					*render;
						view_time_type						time;
						view_fps_type						fps;
						view_image_type						*images;
						rain_draw_type						*rain_draws;
						view_obscure_type					obscure;
						view_menu_type						menu;
						view_cinema_type					cinema;
						view_chat_type						chat;
						view_console_type					console;
						view_score_type						score;
						view_error_type						error;
						view_debug_type						debug;
					} view_type;

