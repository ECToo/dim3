/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.crosshair.color object

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

JSValueRef js_weap_crosshair_color_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_crosshair_color_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_crosshair_color_get_red(void);
JSValueRef js_weap_crosshair_color_get_green(void);
JSValueRef js_weap_crosshair_color_get_blue(void);
bool js_weap_crosshair_color_set_red(JSValueRef vp);
bool js_weap_crosshair_color_set_green(JSValueRef vp);
bool js_weap_crosshair_color_set_blue(JSValueRef vp);

extern js_type			js;

script_js_property	weap_crosshair_color_props[]={
							{"red",					js_weap_crosshair_color_get_red,		js_weap_crosshair_color_set_red},
							{"green",				js_weap_crosshair_color_get_green,		js_weap_crosshair_color_set_green},
							{"blue",				js_weap_crosshair_color_get_blue,		js_weap_crosshair_color_set_blue},
							{0}};

JSClassRef			weap_crosshair_color_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_crosshair_color_object(void)
{
	weap_crosshair_color_class=script_create_class("weap_crosshair_color_class",js_weap_crosshair_color_get_property,js_weap_crosshair_color_set_property);
}

void script_free_weap_crosshair_color_object(void)
{
	script_free_class(weap_crosshair_color_class);
}

JSObjectRef script_add_weap_crosshair_color_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_crosshair_color_class,"color",weap_crosshair_color_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_crosshair_color_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_crosshair_color_props));
}

bool js_weap_crosshair_color_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_crosshair_color_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_crosshair_color_get_red(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->crosshair.col.r);

	return(TRUE);
}

JSValueRef js_weap_crosshair_color_get_green(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->crosshair.col.g);

	return(TRUE);
}

JSValueRef js_weap_crosshair_color_get_blue(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->crosshair.col.b);

	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_crosshair_color_set_red(JSValueRef vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.col.r=script_value_to_float(*vp);

	return(TRUE);
}

bool js_weap_crosshair_color_set_green(JSValueRef vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.col.g=script_value_to_float(*vp);

	return(TRUE);
}

bool js_weap_crosshair_color_set_blue(JSValueRef vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.col.b=script_value_to_float(*vp);

	return(TRUE);
}

