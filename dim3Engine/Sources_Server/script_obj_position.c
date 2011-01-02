/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.position object

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
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern network_setup_type	net_setup;

extern void object_setup_motion(obj_type *obj,float ang,float speed);
							
JSValueRef js_obj_position_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_position_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_position_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_position_place_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_place_random_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_place_network_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_place_random_spot_no_telefrag_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_place_network_spot_no_telefrag_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_distance_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_position_distance_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_position_props[]={
							{"x",								js_obj_position_get_x,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"y",								js_obj_position_get_y,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"z",								js_obj_position_get_z,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_position_functions[]={
							{"place",							js_obj_position_place_func,								kJSPropertyAttributeDontDelete},
							{"placeRandomSpot",					js_obj_position_place_random_spot_func,					kJSPropertyAttributeDontDelete},
							{"placeNetworkSpot",				js_obj_position_place_network_spot_func,				kJSPropertyAttributeDontDelete},
							{"move",							js_obj_position_move_func,								kJSPropertyAttributeDontDelete},
							{"reset",							js_obj_position_reset_func,								kJSPropertyAttributeDontDelete},
							{"distanceToPlayer",				js_obj_position_distance_to_player_func,				kJSPropertyAttributeDontDelete},
							{"distanceToObject",				js_obj_position_distance_to_object_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_position_object(void)
{
	obj_position_class=script_create_class("obj_position_class",obj_position_props,obj_position_functions);
}

void script_free_obj_position_object(void)
{
	script_free_class(obj_position_class);
}

JSObjectRef script_add_obj_position_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_position_class,"position"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_position_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->pnt.x));
}

JSValueRef js_obj_position_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->pnt.y));
}

JSValueRef js_obj_position_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->pnt.z));
}

/* =======================================================

      Reposition Functions
      
======================================================= */

JSValueRef js_obj_position_place_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();

	object_set_position(obj,script_value_to_int(cx,argv[0]),script_value_to_int(cx,argv[2]),script_value_to_int(cx,argv[1]),script_value_to_float(cx,argv[3]),0);
	object_telefrag_players(obj,FALSE);
    
    return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_obj_position_place_random_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	spot_type		*spot;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	
		// find spot
		
	spot=script_find_spot_from_name_type(cx,argv[0],argv[1],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
		// move player
	
	object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
 	object_telefrag_players(obj,FALSE);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_position_place_network_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	spot_type		*spot;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();

		// get spot
		
	spot=script_find_network_spot(cx,obj,exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
		// move player
	
	object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
 	object_telefrag_players(obj,FALSE);
   
    return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_obj_position_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				xadd,zadd,yadd;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	
	xadd=script_value_to_int(cx,argv[0]);
	zadd=script_value_to_int(cx,argv[1]);
	yadd=script_value_to_int(cx,argv[2]);
	
	object_set_position(obj,(obj->pnt.x+xadd),(obj->pnt.y+yadd),(obj->pnt.z+zadd),obj->ang.y,0);
 	object_telefrag_players(obj,FALSE);

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_obj_position_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	object_reset(obj);
	object_telefrag_players(obj,FALSE);
	
	return(script_null_to_value(cx));
}

/* =======================================================

      Object Distance Functions
      
======================================================= */

JSValueRef js_obj_position_distance_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj,*player_obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	player_obj=server.obj_list.objs[server.player_obj_idx];

	return(script_int_to_value(cx,distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,player_obj->pnt.x,player_obj->pnt.y,player_obj->pnt.z)));
}

JSValueRef js_obj_position_distance_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj,*dist_obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();

	dist_obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (dist_obj==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,dist_obj->pnt.x,dist_obj->pnt.y,dist_obj->pnt.z)));
}
