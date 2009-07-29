/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.bitmap object

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
#include "interfaces.h"

extern js_type			js;

JSBool js_interface_bitmap_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_bitmap_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_bitmap_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_hide_all_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_move_relative_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_resize_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_repeat_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_flash_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_start_fade_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	interface_bitmap_functions[]={
							{"show",				js_interface_bitmap_show_func,			1},
							{"hide",				js_interface_bitmap_hide_func,			1},
							{"hideAll",				js_interface_bitmap_hide_all_func,		0},
							{"move",				js_interface_bitmap_move_func,			3},
							{"moveRelative",		js_interface_bitmap_move_relative_func,	3},
							{"resize",				js_interface_bitmap_resize_func,		3},
							{"setAlpha",			js_interface_bitmap_set_alpha_func,		2},
							{"setRepeatCount",		js_interface_bitmap_set_repeat_count_func,2},
							{"setFlash",			js_interface_bitmap_set_flash_func,		2},
							{"setRotate",			js_interface_bitmap_set_rotate_func,	2},
							{"startFade",			js_interface_bitmap_start_fade_func,	1},
							{0}};

JSClass				*interface_bitmap_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_bitmap_object(void)
{
	interface_bitmap_class=script_create_class("interface_bitmap_class",js_interface_bitmap_get_property,js_interface_bitmap_set_property);
}

void script_free_interface_bitmap_object(void)
{
	script_free_class(interface_bitmap_class);
}

void script_add_interface_bitmap_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,interface_bitmap_class,"bitmap",NULL,interface_bitmap_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_interface_bitmap_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_interface_bitmap_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Bitmap Functions
      
======================================================= */

JSBool js_interface_bitmap_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->show=TRUE;
	bitmap->show_tick=js.time.current_tick;		// use to synch animations with shows
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->show=FALSE;
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_hide_all_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmaps_hide_all();
	return(JS_TRUE);
}

JSBool js_interface_bitmap_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->x=script_value_to_int(argv[1]);
	bitmap->y=script_value_to_int(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_move_relative_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->x+=script_value_to_int(argv[1]);
	bitmap->y+=script_value_to_int(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_resize_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->x_size=script_value_to_int(argv[1]);
	bitmap->y_size=script_value_to_int(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->alpha=script_value_to_float(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_repeat_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->repeat.count=script_value_to_int(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_flash_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->flash=script_value_to_bool(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->rot=script_value_to_float(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_start_fade_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->show=TRUE;
	bitmap->fade.on=TRUE;
	bitmap->fade.start_tick=js.time.current_tick;
	
	return(JS_TRUE);
}
