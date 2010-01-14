/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.group object

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"

extern map_type			map;
extern js_type			js;

extern void group_show(int group_idx,bool show);
extern void group_solid(int group_idx,bool solid);
extern void group_texture(int group_idx,int index);
extern void group_texture_shift(int group_idx,float x_shift,float y_shift);

JSValueRef js_map_group_get_center_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_group_set_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_group_set_solid_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_group_set_texture_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_group_set_texture_shift_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	map_group_functions[]={
							{"getCenter",			js_map_group_get_center_func,			kJSPropertyAttributeDontDelete},
							{"setShow",				js_map_group_set_show_func,				kJSPropertyAttributeDontDelete},
							{"setSolid",			js_map_group_set_solid_func,			kJSPropertyAttributeDontDelete},
							{"setTexture",			js_map_group_set_texture_func,			kJSPropertyAttributeDontDelete},
							{"setTextureShift",		js_map_group_set_texture_shift_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			map_group_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_group_object(void)
{
	map_group_class=script_create_class("map_group_class",NULL,map_group_functions);
}

void script_free_map_group_object(void)
{
	script_free_class(map_group_class);
}

JSObjectRef script_add_map_group_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,map_group_class,"group"));
}

/* =======================================================

      Information Functions
      
======================================================= */

JSValueRef js_map_group_get_center_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				group_idx;
	group_type		*group;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	group_idx=script_find_group_from_name(cx,argv[0],exception);
	if (group_idx==-1) return(FALSE);

	if ((group_idx>=0) && (group_idx<map.ngroup)) {
		group=&map.groups[group_idx];
		return(script_point_to_value(cx,group->center_pnt.x,group->center_pnt.y,group->center_pnt.z));
	}

	return(script_point_to_value(cx,0,0,0));
}

/* =======================================================

      Show and Solid Function
      
======================================================= */

JSValueRef js_map_group_set_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				group_idx;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	group_idx=script_find_group_from_name(cx,argv[0],exception);
	if (group_idx!=-1) group_show(group_idx,script_value_to_bool(cx,argv[1]));

	return(script_null_to_value(cx));
}

JSValueRef js_map_group_set_solid_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				group_idx;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	group_idx=script_find_group_from_name(cx,argv[0],exception);
	if (group_idx!=-1) group_solid(group_idx,script_value_to_bool(cx,argv[1]));

	return(script_null_to_value(cx));
}

/* =======================================================

      Texture Function
      
======================================================= */

JSValueRef js_map_group_set_texture_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				group_idx;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	group_idx=script_find_group_from_name(cx,argv[0],exception);
	if (group_idx!=-1) group_texture(group_idx,script_value_to_int(cx,argv[1]));

	return(script_null_to_value(cx));
}

JSValueRef js_map_group_set_texture_shift_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				group_idx;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	group_idx=script_find_group_from_name(cx,argv[0],exception);
	if (group_idx!=-1) group_texture_shift(group_idx,script_value_to_float(cx,argv[1]),script_value_to_float(cx,argv[2]));

	return(script_null_to_value(cx));
}
