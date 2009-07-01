/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.position object

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
extern view_type		view;
extern js_type			js;

JSBool js_camera_position_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_position_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_position_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			camera_position_class={"camera_position_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	camera_position_props[]={
							{"x",					js_camera_position_get_x,				NULL},
							{"y",					js_camera_position_get_y,				NULL},
							{"z",					js_camera_position_get_z,				NULL},
							{0}};
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_position_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"position",&camera_position_class,camera_position_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_camera_position_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(view.render->camera.pnt.x);
	return(JS_TRUE);
}

JSBool js_camera_position_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(view.render->camera.pnt.y);
	return(JS_TRUE);
}

JSBool js_camera_position_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(view.render->camera.pnt.z);
	return(JS_TRUE);
}

