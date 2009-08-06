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

JSBool js_weap_crosshair_color_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_weap_crosshair_color_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_weap_crosshair_color_get_red(JSValueRef *vp);
bool js_weap_crosshair_color_get_green(JSValueRef *vp);
bool js_weap_crosshair_color_get_blue(JSValueRef *vp);
bool js_weap_crosshair_color_set_red(JSValueRef *vp);
bool js_weap_crosshair_color_set_green(JSValueRef *vp);
bool js_weap_crosshair_color_set_blue(JSValueRef *vp);

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

JSObject* script_add_weap_crosshair_color_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_crosshair_color_class,"color",weap_crosshair_color_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_crosshair_color_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_crosshair_color_props));
}

JSBool js_weap_crosshair_color_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_crosshair_color_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_crosshair_color_get_red(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->crosshair.col.r);

	return(TRUE);
}

bool js_weap_crosshair_color_get_green(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->crosshair.col.g);

	return(TRUE);
}

bool js_weap_crosshair_color_get_blue(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->crosshair.col.b);

	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_crosshair_color_set_red(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.col.r=script_value_to_float(*vp);

	return(TRUE);
}

bool js_weap_crosshair_color_set_green(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.col.g=script_value_to_float(*vp);

	return(TRUE);
}

bool js_weap_crosshair_color_set_blue(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.col.b=script_value_to_float(*vp);

	return(TRUE);
}

