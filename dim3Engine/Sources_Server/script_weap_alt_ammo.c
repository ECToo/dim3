/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.altAmmo object

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
#include "objects.h"
#include "weapons.h"

extern js_type				js;

JSValueRef js_weap_alt_ammo_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_alt_ammo_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_clip(void);
JSValueRef js_weap_alt_ammo_get_count(void);
JSValueRef js_weap_alt_ammo_get_initCount(void);
JSValueRef js_weap_alt_ammo_get_maxCount(void);
JSValueRef js_weap_alt_ammo_get_clipCount(void);
JSValueRef js_weap_alt_ammo_get_initClipCount(void);
JSValueRef js_weap_alt_ammo_get_maxClipCount(void);
JSValueRef js_weap_alt_ammo_get_lastReloadTick(void);
bool js_weap_alt_ammo_set_clip(JSValueRef vp);
bool js_weap_alt_ammo_set_count(JSValueRef vp);
bool js_weap_alt_ammo_set_initCount(JSValueRef vp);
bool js_weap_alt_ammo_set_maxCount(JSValueRef vp);
bool js_weap_alt_ammo_set_clipCount(JSValueRef vp);
bool js_weap_alt_ammo_set_initClipCount(JSValueRef vp);
bool js_weap_alt_ammo_set_maxClipCount(JSValueRef vp);
JSValueRef js_weap_alt_ammo_use_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_alt_ammo_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_alt_ammo_change_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	weap_alt_ammo_props[]={
							{"clip",				js_weap_alt_ammo_get_clip,				js_weap_alt_ammo_set_clip},
							{"count",				js_weap_alt_ammo_get_count,				js_weap_alt_ammo_set_count},
							{"initCount",			js_weap_alt_ammo_get_initCount,			js_weap_alt_ammo_set_initCount},
							{"maxCount",			js_weap_alt_ammo_get_maxCount,			js_weap_alt_ammo_set_maxCount},
							{"clipCount",			js_weap_alt_ammo_get_clipCount,			js_weap_alt_ammo_set_clipCount},
							{"initClipCount",		js_weap_alt_ammo_get_initClipCount,		js_weap_alt_ammo_set_initClipCount},
							{"maxClipCount",		js_weap_alt_ammo_get_maxClipCount,		js_weap_alt_ammo_set_maxClipCount},
							{"lastReloadTick",		js_weap_alt_ammo_get_lastReloadTick,	NULL},
							{0}};
							
script_js_function	weap_alt_ammo_functions[]={
							{"useAmmo",				js_weap_alt_ammo_use_ammo_func,			1},
							{"addAmmo",				js_weap_alt_ammo_add_ammo_func,			1},
							{"changeClip",			js_weap_alt_ammo_change_clip_func,		0},
							{0}};

JSClassRef			weap_alt_ammo_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_alt_ammo_object(void)
{
	weap_alt_ammo_class=script_create_class("weap_alt_ammo_class",js_weap_alt_ammo_get_property,js_weap_alt_ammo_set_property);
}

void script_free_weap_alt_ammo_object(void)
{
	script_free_class(weap_alt_ammo_class);
}

JSObjectRef script_add_weap_alt_ammo_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_alt_ammo_class,"altAmmo",weap_alt_ammo_props,weap_alt_ammo_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_alt_ammo_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_alt_ammo_props));
}

bool js_weap_alt_ammo_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_alt_ammo_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_alt_ammo_get_clip(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;
	
	*vp=script_bool_to_value(alt_ammo->use_clips);
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_get_count(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	if (weap->dual.in_dual) {
		*vp=script_int_to_value(alt_ammo->count_dual);
	}
	else {
		*vp=script_int_to_value(alt_ammo->count);
	}
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_get_initCount(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	*vp=script_int_to_value(alt_ammo->init_count);
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_get_maxCount(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	*vp=script_int_to_value(alt_ammo->max_count);
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_get_clipCount(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	*vp=script_int_to_value(alt_ammo->clip_count);
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_get_initClipCount(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	*vp=script_int_to_value(alt_ammo->init_clip_count);
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_get_maxClipCount(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	*vp=script_int_to_value(alt_ammo->max_clip_count);
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_get_lastReloadTick(void)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;
	
	if (weap->dual.in_dual) {
		*vp=script_int_to_value(alt_ammo->last_reload_dual_tick);
	}
	else {
		*vp=script_int_to_value(alt_ammo->last_reload_tick);
	}
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_alt_ammo_set_clip(JSValueRef vp)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;
	
	alt_ammo->use_clips=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_weap_alt_ammo_set_count(JSValueRef vp)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	if (weap->dual.in_dual) {
		alt_ammo->count_dual=script_value_to_int(*vp);
	}
	else {
		alt_ammo->count=script_value_to_int(*vp);
	}
	
	return(TRUE);
}

bool js_weap_alt_ammo_set_initCount(JSValueRef vp)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->init_count=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_alt_ammo_set_maxCount(JSValueRef vp)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->max_count=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_alt_ammo_set_clipCount(JSValueRef vp)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->clip_count=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_alt_ammo_set_initClipCount(JSValueRef vp)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->init_clip_count=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_alt_ammo_set_maxClipCount(JSValueRef vp)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->max_clip_count=script_value_to_int(*vp);
	
	return(TRUE);
}

/* =======================================================

      Alternate Ammo Functions
      
======================================================= */

JSValueRef js_weap_alt_ammo_use_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				count;
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	count=script_value_to_int(argv[0]);
	
	*rval=script_bool_to_value(FALSE);
	
	if (weap->dual.in_dual) {
		if (alt_ammo->count_dual>=count) {
			alt_ammo->count_dual-=count;
			*rval=script_bool_to_value(TRUE);
		}
	}
	else {
		if (alt_ammo->count>=count) {
			alt_ammo->count-=count;
			*rval=script_bool_to_value(TRUE);
		}
	}
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				add;
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;
	
	add=script_value_to_int(argv[0]);
	
	*rval=script_bool_to_value(TRUE);

	if (weap->dual.in_dual) {
		if (alt_ammo->count_dual==alt_ammo->max_count) {
			*rval=script_bool_to_value(FALSE);
		}
		else {
			alt_ammo->count_dual+=add;
			if (alt_ammo->count_dual>alt_ammo->max_count) alt_ammo->count_dual=alt_ammo->max_count;
		}
	}
	else {
		if (alt_ammo->count==alt_ammo->max_count) {
			*rval=script_bool_to_value(FALSE);
		}
		else {
			alt_ammo->count+=add;
			if (alt_ammo->count>alt_ammo->max_count) alt_ammo->count=alt_ammo->max_count;
		}
	}
	
	return(TRUE);
}

JSValueRef js_weap_alt_ammo_change_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	alt_ammo=&weap->alt_ammo;

	obj=object_find_uid(weap->obj_uid);
	
		// is this a clip based weapon and enough clips?
		
	if ((!alt_ammo->use_clips) || (alt_ammo->clip_count==0)) {
		*rval=script_bool_to_value(FALSE);
		return(TRUE);
	}
	
		// change the clip
		
	if (weap->dual.in_dual) {
		alt_ammo->count_dual=alt_ammo->max_count;
		alt_ammo->last_reload_dual_tick=js.time.current_tick;
	}
	else {
		alt_ammo->count=alt_ammo->max_count;
		alt_ammo->last_reload_tick=js.time.current_tick;
	}
	
	alt_ammo->clip_count--;
	
		// alert object of clip change
		
	scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_clip_change,0);
	
	*rval=script_bool_to_value(TRUE);
	return(TRUE);
}


