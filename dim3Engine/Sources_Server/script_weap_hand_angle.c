/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.handAngle object

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

JSValueRef js_weap_hand_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_hand_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_hand_angle_get_x(void);
JSValueRef js_weap_hand_angle_get_y(void);
JSValueRef js_weap_hand_angle_get_z(void);
void js_weap_hand_angle_set_x(JSValueRef vp,JSValueRef *exception);
void js_weap_hand_angle_set_y(JSValueRef vp,JSValueRef *exception);
void js_weap_hand_angle_set_z(JSValueRef vp,JSValueRef *exception);

script_js_property	weap_hand_angle_props[]={
							{"x",				js_weap_hand_angle_get_x,			js_weap_hand_angle_set_x},
							{"y",				js_weap_hand_angle_get_y,			js_weap_hand_angle_set_y},
							{"z",				js_weap_hand_angle_get_z,			js_weap_hand_angle_set_z},
							{0}};

JSClassRef			weap_hand_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_hand_angle_object(void)
{
	weap_hand_angle_class=script_create_class("weap_hand_angle_class",js_weap_hand_angle_get_property,js_weap_hand_angle_set_property);
}

void script_free_weap_hand_angle_object(void)
{
	script_free_class(weap_hand_angle_class);
}

JSObjectRef script_add_weap_hand_angle_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_hand_angle_class,"handAngle",weap_hand_angle_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_hand_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_hand_angle_props));
}

bool js_weap_hand_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_hand_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_hand_angle_get_x(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.ang.x);
	
	return(TRUE);
}

bool js_weap_hand_angle_get_y(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.ang.y);
	
	return(TRUE);
}

bool js_weap_hand_angle_get_z(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.ang.z);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_weap_hand_angle_set_x(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.ang.x=script_value_to_float(*vp);
}

void js_weap_hand_angle_set_y(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.ang.y=script_value_to_float(*vp);
}

void js_weap_hand_angle_set_z(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.ang.z=script_value_to_float(*vp);
}


