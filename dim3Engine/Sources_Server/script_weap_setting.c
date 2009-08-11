/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.setting object

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

JSValueRef js_weap_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_setting_get_name(void);
JSValueRef js_weap_setting_get_failInLiquid(void);
JSValueRef js_weap_setting_get_parentObjectId(void);
bool js_weap_setting_set_failInLiquid(JSValueRef vp);

script_js_property	weap_setting_props[]={
							{"name",				js_weap_setting_get_name,				NULL},
							{"failInLiquid",		js_weap_setting_get_failInLiquid,		js_weap_setting_set_failInLiquid},
							{"parentObjectId",		js_weap_setting_get_parentObjectId,		NULL},
							{0}};

JSClassRef			weap_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_setting_object(void)
{
	weap_setting_class=script_create_class("weap_setting_class",js_weap_setting_get_property,js_weap_setting_set_property);
}

void script_free_weap_setting_object(void)
{
	script_free_class(weap_setting_class);
}

JSObjectRef script_add_weap_setting_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_setting_class,"setting",weap_setting_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_setting_props));
}

bool js_weap_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_setting_get_name(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->name);
	
	return(TRUE);
}

JSValueRef js_weap_setting_get_failInLiquid(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->fail_in_liquid);
	
	return(TRUE);
}

JSValueRef js_weap_setting_get_parentObjectId(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->obj_uid);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_setting_set_failInLiquid(JSValueRef vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->fail_in_liquid=script_value_to_bool(*vp);
	
	return(TRUE);
}


