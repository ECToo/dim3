/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.crosshair object

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
#include "weapons.h"

extern js_type			js;

JSValueRef js_weap_crosshair_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_crosshair_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_crosshair_get_on(void);
JSValueRef js_weap_crosshair_get_type(void);
JSValueRef js_weap_crosshair_get_name(void);
JSValueRef js_weap_crosshair_get_minSize(void);
JSValueRef js_weap_crosshair_get_maxSize(void);
JSValueRef js_weap_crosshair_get_distance(void);
bool js_weap_crosshair_set_on(JSValueRef vp);
bool js_weap_crosshair_set_type(JSValueRef vp);
bool js_weap_crosshair_set_name(JSValueRef vp);
bool js_weap_crosshair_set_minSize(JSValueRef vp);
bool js_weap_crosshair_set_maxSize(JSValueRef vp);
bool js_weap_crosshair_set_distance(JSValueRef vp);

script_js_property	weap_crosshair_props[]={
							{"on",					js_weap_crosshair_get_on,			js_weap_crosshair_set_on},
							{"type",				js_weap_crosshair_get_type,			js_weap_crosshair_set_type},
							{"name",				js_weap_crosshair_get_name,			js_weap_crosshair_set_name},
							{"minSize",				js_weap_crosshair_get_minSize,		js_weap_crosshair_set_minSize},
							{"maxSize",				js_weap_crosshair_get_maxSize,		js_weap_crosshair_set_maxSize},
							{"distance",			js_weap_crosshair_get_distance,		js_weap_crosshair_set_distance},
							{0}};

JSClassRef			weap_crosshair_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_crosshair_object(void)
{
	weap_crosshair_class=script_create_class("weap_crosshair_class",js_weap_crosshair_get_property,js_weap_crosshair_set_property);
}

void script_free_weap_crosshair_object(void)
{
	script_free_class(weap_crosshair_class);
}

JSObjectRef script_add_weap_crosshair_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_crosshair_class,"crosshair",weap_crosshair_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_crosshair_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_crosshair_props));
}

bool js_weap_crosshair_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_crosshair_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_crosshair_get_on(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->crosshair.on);
	
	return(TRUE);
}

JSValueRef js_weap_crosshair_get_type(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->crosshair.type+sd_weap_crosshair_type_center);
	
	return(TRUE);
}

JSValueRef js_weap_crosshair_get_name(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->crosshair.fire_name);
	
	return(TRUE);
}

JSValueRef js_weap_crosshair_get_minSize(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->crosshair.min_size);
	
	return(TRUE);
}

JSValueRef js_weap_crosshair_get_maxSize(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->crosshair.max_size);
	
	return(TRUE);
}

JSValueRef js_weap_crosshair_get_distance(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->crosshair.distance);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_crosshair_set_on(JSValueRef vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.on=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_weap_crosshair_set_type(JSValueRef vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.type=script_value_to_int(*vp)-sd_weap_crosshair_type_center;
	
	return(TRUE);
}

bool js_weap_crosshair_set_name(JSValueRef vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->crosshair.fire_name,name_str_len);
	weapon_attach_fire_crosshair(weap);
	
	return(TRUE);
}

bool js_weap_crosshair_set_minSize(JSValueRef vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.min_size=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_crosshair_set_maxSize(JSValueRef vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.max_size=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_crosshair_set_distance(JSValueRef vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.distance=script_value_to_int(*vp);
	
	return(TRUE);
}



