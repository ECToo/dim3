/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.position object

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
#include "projectiles.h"

extern js_type			js;

JSBool js_proj_position_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_position_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_position_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_position_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_position_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	proj_position_props[]={
							{"x",					js_proj_position_get_x,				NULL},
							{"y",					js_proj_position_get_y,				NULL},
							{"z",					js_proj_position_get_z,				NULL},
							{0}};

JSClass				*proj_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_position_object(void)
{
	proj_position_class=script_create_class("proj_position_class",js_proj_position_get_property,js_proj_position_set_property);
}

void script_free_proj_position_object(void)
{
	script_free_class(proj_position_class);
}

void script_add_proj_position_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,proj_position_class,"position",proj_position_props,NULL);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_proj_position_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,proj_position_props));
}

JSBool js_proj_position_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,proj_position_props));
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_proj_position_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	*vp=INT_TO_JSVAL(proj->pnt.x);
	
	return(JS_TRUE);
}

JSBool js_proj_position_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	*vp=INT_TO_JSVAL(proj->pnt.y);
	
	return(JS_TRUE);
}

JSBool js_proj_position_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	*vp=INT_TO_JSVAL(proj->pnt.z);
	
	return(JS_TRUE);
}

