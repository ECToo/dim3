/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Object Creation

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
	#include "dim3engine.h"
#endif

#include "scripts.h"

extern js_type			js;

/* =======================================================

      Initialize/Release Classes
      
======================================================= */

void script_initialize_classes(void)
{
	script_init_global_map_object();
	script_init_map_action_object();
	script_init_map_fog_object();
	script_init_map_fog_color_object();
	script_init_map_group_object();
	script_init_map_info_object();
	script_init_map_light_color_object();
	script_init_map_movement_object();
	script_init_map_node_object();
	script_init_map_object_object();
	script_init_map_setting_object();
	script_init_map_spot_object();
	script_init_map_light_object();

	script_init_global_multiplayer_object();
	script_init_multiplayer_bot_object();
	script_init_multiplayer_score_object();
	script_init_multiplayer_setting_object();

	script_init_global_camera_object();
	script_init_camera_angle_object();
	script_init_camera_chase_object();
	script_init_camera_chase_angle_object();
	script_init_camera_chase_offset_object();
	script_init_camera_chase_slop_object();
	script_init_camera_plane_object();
	script_init_camera_position_object();
	script_init_camera_setting_object();
	script_init_camera_state_object();
	script_init_camera_static_position_object();

	script_init_global_interface_object();
	script_init_interface_bar_object();
	script_init_interface_bitmap_object();
	script_init_interface_console_object();
	script_init_interface_fade_object();
	script_init_interface_interaction_object();
	script_init_interface_radar_object();
	script_init_interface_screen_object();
	script_init_interface_text_object();

	script_init_global_data_object();

	script_init_global_sound_object();

	script_init_global_spawn_object();

	script_init_global_utility_object();
	script_init_utility_angle_object();
	script_init_utility_pack_object();
	script_init_utility_point_object();
	script_init_utility_random_object();

	script_init_game_setting_object();
	script_init_game_score_object();
	script_init_game_join_object();

	script_init_model_object();
	script_init_model_animation_object();
	script_init_model_bone_object();
	script_init_model_fill_object();
	script_init_model_halo_object();
	script_init_model_light_object();
	script_init_model_light_color_object();
	script_init_model_mesh_object();
	script_init_model_offset_object();
	script_init_model_rotate_object();
	script_init_model_shadow_object();
	script_init_model_spin_object();

	script_init_event_object();

	script_init_obj_setting_object();
	script_init_obj_position_object();
	script_init_obj_angle_object();
	script_init_obj_lock_object();
	script_init_obj_status_object();
	script_init_obj_motion_vector_object();
	script_init_obj_motion_angle_object();
	script_init_obj_forward_speed_object();
	script_init_obj_side_speed_object();
	script_init_obj_vert_speed_object();
	script_init_obj_turn_speed_object();
	script_init_obj_object_speed_object();
	script_init_obj_thrust_object();
	script_init_obj_look_object();
	script_init_obj_sight_object();
	script_init_obj_health_object();
	script_init_obj_size_object();
	script_init_obj_rigid_body_object();
	script_init_obj_vehicle_object();
	script_init_obj_radar_object();
	script_init_obj_score_object();
	script_init_obj_touch_object();
	script_init_obj_touch_position_object();
	script_init_obj_touch_angle_object();
	script_init_obj_hit_object();
	script_init_obj_hit_position_object();
	script_init_obj_hit_angle_object();
	script_init_obj_click_object();
	script_init_obj_held_object();
	script_init_obj_pickup_object();
	script_init_obj_watch_object();
	script_init_obj_weapon_object();
	script_init_obj_weapon_fire_object();
	script_init_obj_melee_object();

	script_init_weap_setting_object();
	script_init_weap_hand_object();
	script_init_weap_hand_position_object();
	script_init_weap_hand_angle_object();
	script_init_weap_projectile_object();
	script_init_weap_crosshair_object();
	script_init_weap_crosshair_color_object();
	script_init_weap_crosshair_empty_color_object();
	script_init_weap_crosshair_pickup_color_object();
	script_init_weap_ammo_object();
	script_init_weap_alt_ammo_object();
	script_init_weap_kickback_object();
	script_init_weap_recoil_object();
	script_init_weap_melee_object();
	script_init_weap_fire_object();
	script_init_weap_dual_object();
	script_init_weap_target_object();
	script_init_weap_target_color_object();
	script_init_weap_zoom_object();

	script_init_proj_setting_object();
	script_init_proj_position_object();
	script_init_proj_origin_object();
	script_init_proj_motion_vector_object();
	script_init_proj_motion_angle_object();
	script_init_proj_speed_object();
	script_init_proj_hit_object();
	script_init_proj_action_object();
	script_init_proj_push_object();
	script_init_proj_size_object();
	script_init_proj_mark_object();
	script_init_proj_melee_object();
}

void script_release_classes(void)
{
	script_free_global_map_object();
	script_free_map_action_object();
	script_free_map_fog_object();
	script_free_map_fog_color_object();
	script_free_map_group_object();
	script_free_map_info_object();
	script_free_map_light_color_object();
	script_free_map_movement_object();
	script_free_map_node_object();
	script_free_map_object_object();
	script_free_map_setting_object();
	script_free_map_spot_object();
	script_free_map_light_object();

	script_free_global_multiplayer_object();
	script_free_multiplayer_bot_object();
	script_free_multiplayer_score_object();
	script_free_multiplayer_setting_object();

	script_free_global_camera_object();
	script_free_camera_angle_object();
	script_free_camera_chase_object();
	script_free_camera_chase_angle_object();
	script_free_camera_chase_offset_object();
	script_free_camera_chase_slop_object();
	script_free_camera_plane_object();
	script_free_camera_position_object();
	script_free_camera_setting_object();
	script_free_camera_state_object();
	script_free_camera_static_position_object();

	script_free_global_interface_object();
	script_free_interface_bar_object();
	script_free_interface_bitmap_object();
	script_free_interface_console_object();
	script_free_interface_fade_object();
	script_free_interface_interaction_object();
	script_free_interface_radar_object();
	script_free_interface_screen_object();
	script_free_interface_text_object();

	script_free_global_data_object();

	script_free_global_sound_object();

	script_free_global_spawn_object();

	script_free_global_utility_object();
	script_free_utility_angle_object();
	script_free_utility_pack_object();
	script_free_utility_point_object();
	script_free_utility_random_object();

	script_free_game_setting_object();
	script_free_game_score_object();
	script_free_game_join_object();

	script_free_model_object();
	script_free_model_animation_object();
	script_free_model_bone_object();
	script_free_model_fill_object();
	script_free_model_halo_object();
	script_free_model_light_object();
	script_free_model_light_color_object();
	script_free_model_mesh_object();
	script_free_model_offset_object();
	script_free_model_rotate_object();
	script_free_model_shadow_object();
	script_free_model_spin_object();

	script_free_event_object();

	script_free_obj_setting_object();
	script_free_obj_position_object();
	script_free_obj_angle_object();
	script_free_obj_lock_object();
	script_free_obj_status_object();
	script_free_obj_motion_vector_object();
	script_free_obj_motion_angle_object();
	script_free_obj_forward_speed_object();
	script_free_obj_side_speed_object();
	script_free_obj_vert_speed_object();
	script_free_obj_turn_speed_object();
	script_free_obj_object_speed_object();
	script_free_obj_thrust_object();
	script_free_obj_look_object();
	script_free_obj_sight_object();
	script_free_obj_health_object();
	script_free_obj_size_object();
	script_free_obj_rigid_body_object();
	script_free_obj_vehicle_object();
	script_free_obj_radar_object();
	script_free_obj_score_object();
	script_free_obj_touch_object();
	script_free_obj_touch_position_object();
	script_free_obj_touch_angle_object();
	script_free_obj_hit_object();
	script_free_obj_hit_position_object();
	script_free_obj_hit_angle_object();
	script_free_obj_click_object();
	script_free_obj_held_object();
	script_free_obj_pickup_object();
	script_free_obj_watch_object();
	script_free_obj_weapon_object();
	script_free_obj_weapon_fire_object();
	script_free_obj_melee_object();

	script_free_weap_setting_object();
	script_free_weap_hand_object();
	script_free_weap_hand_position_object();
	script_free_weap_hand_angle_object();
	script_free_weap_projectile_object();
	script_free_weap_crosshair_object();
	script_free_weap_crosshair_color_object();
	script_free_weap_crosshair_empty_color_object();
	script_free_weap_crosshair_pickup_color_object();
	script_free_weap_ammo_object();
	script_free_weap_alt_ammo_object();
	script_free_weap_kickback_object();
	script_free_weap_recoil_object();
	script_free_weap_melee_object();
	script_free_weap_fire_object();
	script_free_weap_dual_object();
	script_free_weap_target_object();
	script_free_weap_target_color_object();
	script_free_weap_zoom_object();

	script_free_proj_setting_object();
	script_free_proj_position_object();
	script_free_proj_origin_object();
	script_free_proj_motion_vector_object();
	script_free_proj_motion_angle_object();
	script_free_proj_speed_object();
	script_free_proj_hit_object();
	script_free_proj_action_object();
	script_free_proj_push_object();
	script_free_proj_size_object();
	script_free_proj_mark_object();
	script_free_proj_melee_object();
}

/* =======================================================

      Create dim3 Script Classes
      
======================================================= */

JSClassRef script_create_class(const char *name,JSObjectGetPropertyCallback getter,JSObjectSetPropertyCallback setter)
{
	JSClassDefinition		def;

	def.version=0;
	def.attributes=kJSClassAttributeNone;
	def.className=name;
	def.parentClass=NULL;
	def.staticValues=NULL;
	def.staticFunctions=NULL;
	def.initialize=NULL;
	def.finalize=NULL;
	def.hasProperty=NULL;
	def.getProperty=getter;
	def.setProperty=setter;
	def.deleteProperty=NULL;
	def.getPropertyNames=NULL;
	def.callAsFunction=NULL;
	def.callAsConstructor=NULL;
	def.hasInstance=NULL;
	def.convertToType=NULL;

	return(JSClassCreate(&def));
}

void script_free_class(JSClassRef cls)
{
	JSClassRelease(cls);
}

/* =======================================================

      Add Global Objects to Script
      
======================================================= */

bool script_add_global_object(script_type *script,char *err_str)
{
	JSObject			*j_sub_obj;

		// must remove other globals to stop parenting problems
		
	JS_SetGlobalObject(js.cx,NULL);
	
		// create global for this script
	
	script->global=JS_NewObject(js.cx,NULL,NULL,NULL);
	if (script->global==NULL) {
		strcpy(err_str,"Not enough memory to create global object");
		return(FALSE);
	}

		// set sub objects

	j_sub_obj=script_add_global_map_object(script->global);
	script_add_map_info_object(j_sub_obj);
	script_add_map_setting_object(j_sub_obj);
	script_add_map_light_color_object(j_sub_obj);
	script_add_map_fog_object(j_sub_obj);
	script_add_map_fog_color_object(j_sub_obj);
	script_add_map_spot_object(j_sub_obj);
	script_add_map_node_object(j_sub_obj);
	script_add_map_object_object(j_sub_obj);
	script_add_map_group_object(j_sub_obj);
	script_add_map_movement_object(j_sub_obj);
	script_add_map_action_object(j_sub_obj);
	script_add_map_light_object(j_sub_obj);

	j_sub_obj=script_add_global_multiplayer_object(script->global);
	script_add_multiplayer_setting_object(j_sub_obj);
	script_add_multiplayer_bot_object(j_sub_obj);
	script_add_multiplayer_score_object(j_sub_obj);

	j_sub_obj=script_add_global_camera_object(script->global);
	script_add_camera_setting_object(j_sub_obj);
	script_add_camera_position_object(j_sub_obj);
	script_add_camera_angle_object(j_sub_obj);
	script_add_camera_chase_object(j_sub_obj);
	script_add_camera_chase_angle_object(j_sub_obj);
	script_add_camera_chase_offset_object(j_sub_obj);
	script_add_camera_chase_slop_object(j_sub_obj);
	script_add_camera_static_position_object(j_sub_obj);
	script_add_camera_plane_object(j_sub_obj);
	script_add_camera_state_object(j_sub_obj);

	j_sub_obj=script_add_global_interface_object(script->global);
	script_add_interface_screen_object(j_sub_obj);
	script_add_interface_console_object(j_sub_obj);
	script_add_interface_text_object(j_sub_obj);
	script_add_interface_bitmap_object(j_sub_obj);
	script_add_interface_bar_object(j_sub_obj);
	script_add_interface_radar_object(j_sub_obj);
	script_add_interface_fade_object(j_sub_obj);
	script_add_interface_interaction_object(j_sub_obj);

	script_add_global_data_object(script->global);

	script_add_global_sound_object(script->global);

	script_add_global_spawn_object(script->global);

	j_sub_obj=script_add_global_utility_object(script->global);
	script_add_utility_angle_object(j_sub_obj);
	script_add_utility_point_object(j_sub_obj);
	script_add_utility_random_object(j_sub_obj);
	script_add_utility_pack_object(j_sub_obj);
	
		// set global

	JS_SetGlobalObject(js.cx,script->global);

		// initialize standard classes

	if (!JS_InitStandardClasses(js.cx,script->global)) {
		strcpy(err_str,"Could not initialize standard classes");
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Script Property Utilities
      
======================================================= */

inline void script_set_single_property(JSObjectRef j_obj,const char *prop_name,JSValueRef vp,int flags)
{
	JSStringRef			j_prop_name;

	j_prop_name=JSStringCreateWithUTF8CString(prop_name);
	JSObjectSetProperty(js.cx,j_obj,j_prop_name,vp,flags,NULL);
	JSStringRelease(j_prop_name);
}

inline JSValueRef script_get_single_property(JSObjectRef j_obj,const char *prop_name)
{
	JSStringRef			j_prop_name;
	JSValueRef			vp;

	j_prop_name=JSStringCreateWithUTF8CString(prop_name);
	vp=JSObjectGetProperty(js.cx,j_obj,j_prop_name,NULL);
	JSStringRelease(j_prop_name);

	return(vp);
}

/* =======================================================

      Create dim3 Script Objects
      
======================================================= */

void script_create_main_object_add_model_object(JSObjectRef parent_obj)
{
    JSObject		*j_obj;

	j_obj=script_add_model_object(parent_obj);
	
	script_add_model_offset_object(j_obj);
	script_add_model_rotate_object(j_obj);
	script_add_model_spin_object(j_obj);
	script_add_model_light_object(j_obj);
	script_add_model_light_color_object(j_obj);
	script_add_model_halo_object(j_obj);
	script_add_model_shadow_object(j_obj);
	script_add_model_animation_object(j_obj);
	script_add_model_mesh_object(j_obj);
	script_add_model_bone_object(j_obj);
	script_add_model_fill_object(j_obj);
}

JSObjectRef script_create_main_object(attach_type *attach)
{
	JSObject		*j_obj,*j_sub_obj;

	j_obj=JS_NewObject(js.cx,NULL,NULL,NULL);
	if (j_obj==NULL) return(NULL);

		// add sub-objects

	switch (attach->thing_type) {

		case thing_type_game:
			script_add_game_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_game_score_object(j_obj);
			script_add_game_join_object(j_obj);
			break;

		case thing_type_course:
			script_add_event_object(j_obj);
			break;

		case thing_type_object:
			script_add_obj_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_obj_position_object(j_obj);
			script_add_obj_angle_object(j_obj);
			script_add_obj_lock_object(j_obj);
			script_add_obj_status_object(j_obj);
			script_add_obj_motion_vector_object(j_obj);
			script_add_obj_motion_angle_object(j_obj);
			script_add_obj_forward_speed_object(j_obj);
			script_add_obj_side_speed_object(j_obj);
			script_add_obj_vert_speed_object(j_obj);
			script_add_obj_turn_speed_object(j_obj);
			script_add_obj_object_speed_object(j_obj);
			script_add_obj_thrust_object(j_obj);
			script_add_obj_look_object(j_obj);
			script_add_obj_sight_object(j_obj);
			script_add_obj_health_object(j_obj);
			script_add_obj_size_object(j_obj);
			script_add_obj_rigid_body_object(j_obj);
			script_add_obj_vehicle_object(j_obj);
			script_add_obj_radar_object(j_obj);
			script_add_obj_score_object(j_obj);
			script_add_obj_touch_object(j_obj);
			script_add_obj_touch_position_object(j_obj);
			script_add_obj_touch_angle_object(j_obj);
			script_add_obj_hit_object(j_obj);
			script_add_obj_hit_position_object(j_obj);
			script_add_obj_hit_angle_object(j_obj);
			script_add_obj_click_object(j_obj);
			script_add_obj_held_object(j_obj);
			script_add_obj_pickup_object(j_obj);
			script_add_obj_watch_object(j_obj);
			script_add_obj_weapon_object(j_obj);
			script_add_obj_weapon_fire_object(j_obj);
			script_add_obj_melee_object(j_obj);
			script_create_main_object_add_model_object(j_obj);
			break;

		case thing_type_weapon:
			script_add_weap_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_weap_hand_object(j_obj);
			script_add_weap_hand_position_object(j_obj);
			script_add_weap_hand_angle_object(j_obj);
			script_add_weap_projectile_object(j_obj);
			j_sub_obj=script_add_weap_crosshair_object(j_obj);
				script_add_weap_crosshair_color_object(j_sub_obj);
				script_add_weap_crosshair_empty_color_object(j_sub_obj);
				script_add_weap_crosshair_pickup_color_object(j_sub_obj);
			script_add_weap_kickback_object(j_obj);
			script_add_weap_recoil_object(j_obj);
			script_add_weap_ammo_object(j_obj);
			script_add_weap_alt_ammo_object(j_obj);
			script_add_weap_melee_object(j_obj);
			script_add_weap_fire_object(j_obj);
			script_add_weap_dual_object(j_obj);
			j_sub_obj=script_add_weap_target_object(j_obj);
				script_add_weap_target_color_object(j_sub_obj);
			script_add_weap_zoom_object(j_obj);
			script_create_main_object_add_model_object(j_obj);
			break;

		case thing_type_projectile:
		case thing_type_projectile_setup:
			script_add_proj_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_proj_position_object(j_obj);
			script_add_proj_origin_object(j_obj);
			script_add_proj_motion_vector_object(j_obj);
			script_add_proj_motion_angle_object(j_obj);
			script_add_proj_speed_object(j_obj);
			script_add_proj_hit_object(j_obj);
			script_add_proj_action_object(j_obj);
			script_add_proj_push_object(j_obj);
			script_add_proj_size_object(j_obj);
			script_add_proj_mark_object(j_obj);
			script_add_proj_melee_object(j_obj);
			script_create_main_object_add_model_object(j_obj);
			break;

	}
	
	return(j_obj);
}

JSObjectRef script_create_child_object(JSObjectRef parent_obj,JSClassRef cls,char *name,script_js_property *props,script_js_function *funcs)
{
	int					flags;
	script_js_property	*prop;
	script_js_function	*func;
	JSObject			*j_obj;

		// object

	j_obj=JS_DefineObject(js.cx,parent_obj,name,cls,NULL,0);

		// properties

	if (props!=NULL) {

		prop=props;

		while (prop->name!=NULL) {

			flags=JSPROP_PERMANENT|JSPROP_SHARED;
			if (prop->setter==NULL) flags|=JSPROP_READONLY;

			JS_DefineProperty(js.cx,j_obj,prop->name,JSVAL_NULL,NULL,NULL,flags);

			prop++;
		}
	}

		// functions

	if (funcs!=NULL) {

		func=funcs;

		while (func->name!=NULL) {
			JS_DefineFunction(js.cx,j_obj,func->name,func->call,func->nargs,0);
			func++;
		}
	}

	return(j_obj);
}

/* =======================================================

      Object Setters/Getters
      
======================================================= */

JSValueRef script_read_only_property_error(JSValueRef name)
{
	char				c_name[64],err_str[256];

	script_value_to_string(name,c_name,64);
	sprintf(err_str,"The property %s is read-only",name);

	return(script_create_exception(err_str));
}

inline int script_find_object_property_index(JSValueRef name,script_js_property *props)
{
	int					idx;
	char				c_name[64];

	script_value_to_string(id,c_name,64);

	idx=0;

	while (props[idx].name!=NULL) {
		if (strcmp(props[idx].name,c_name)==0) return(idx);
		idx++;
	}

	return(-1);
}

JSValueRef script_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,script_js_property *props)
{
	int					idx;
	script_js_property	*prop;

		// any properties?

	if (props==NULL) return(TRUE);

		// find the property

	idx=script_find_object_property_index(name,props);
	if (idx==-1) return(TRUE);

	prop=&props[idx];

		// call getter

	return((*prop->getter)());
}

bool script_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception,script_js_property *props)
{
	int					idx;
	script_js_property	*prop;

		// any properties?

	if (props==NULL) return(TRUE);

		// find the property

	idx=script_find_object_property_index(name,props);
	if (idx==-1) return(TRUE);

	prop=&props[idx];

		// is it read only?

	if (prop->setter==NULL) {
		*exception=script_read_only_property_error(name);
		return(TRUE);
	}

		// call setter

	(*prop->setter)(vp)

		// always return TRUE as we are always handling
		// the values

	return(TRUE);
}
