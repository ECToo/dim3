/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.chaseAngle object

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
#include "cameras.h"

extern camera_type		camera;
extern js_type			js;

JSBool js_camera_chase_angle_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_angle_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_angle_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_angle_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_angle_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_angle_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_angle_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_angle_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	camera_chase_angle_props[]={
							{"x",					js_camera_chase_angle_get_x,			js_camera_chase_angle_set_x},
							{"y",					js_camera_chase_angle_get_y,			js_camera_chase_angle_set_y},
							{"z",					js_camera_chase_angle_get_z,			js_camera_chase_angle_set_z},
							{0}};

JSClass				*camera_chase_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_chase_angle_object(void)
{
	camera_chase_angle_class=script_create_class("camera_chase_angle_class",js_camera_chase_angle_get_property,js_camera_chase_angle_set_property);
}

void script_free_camera_chase_angle_object(void)
{
	script_free_class(camera_chase_angle_class);
}

void script_add_camera_chase_angle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,camera_chase_angle_class,"chaseAngle",camera_chase_angle_props,NULL);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_camera_chase_angle_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,camera_chase_angle_props));
}

JSBool js_camera_chase_angle_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,camera_chase_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_camera_chase_angle_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(camera.chase_ang.x);
	return(JS_TRUE);
}

JSBool js_camera_chase_angle_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(camera.chase_ang.y);
	return(JS_TRUE);
}

JSBool js_camera_chase_angle_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(camera.chase_ang.z);
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_camera_chase_angle_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.chase_ang.x=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_camera_chase_angle_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.chase_ang.y=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_camera_chase_angle_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.chase_ang.z=script_value_to_float(*vp);
	return(JS_TRUE);
}
