/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.motionAngle object

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

JSValueRef js_proj_motion_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_motion_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_motion_angle_get_x(void);
JSValueRef js_proj_motion_angle_get_y(void);
JSValueRef js_proj_motion_angle_get_z(void);

script_js_property	proj_motion_angle_props[]={
							{"x",					js_proj_motion_angle_get_x,			NULL},
							{"y",					js_proj_motion_angle_get_y,			NULL},
							{"z",					js_proj_motion_angle_get_z,			NULL},
							{0}};

JSClassRef			proj_motion_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_motion_angle_object(void)
{
	proj_motion_angle_class=script_create_class("proj_motion_angle_class",js_proj_motion_angle_get_property,js_proj_motion_angle_set_property);
}

void script_free_proj_motion_angle_object(void)
{
	script_free_class(proj_motion_angle_class);
}

JSObjectRef script_add_proj_motion_angle_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,proj_motion_angle_class,"motionAngle",proj_motion_angle_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_proj_motion_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,proj_motion_angle_props));
}

bool js_proj_motion_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,proj_motion_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_motion_angle_get_x(void)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);

	*vp=script_float_to_value(proj->motion.ang.x);
	
	return(TRUE);
}

JSValueRef js_proj_motion_angle_get_y(void)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);

	*vp=script_float_to_value(proj->motion.ang.y);
	
	return(TRUE);
}

JSValueRef js_proj_motion_angle_get_z(void)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);

	*vp=script_float_to_value(proj->motion.ang.z);
	
	return(TRUE);
}

