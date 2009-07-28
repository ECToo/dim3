/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.light object

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

JSBool js_model_light_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_get_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_get_direction(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_get_intensity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_get_exponent(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_direction(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_intensity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_exponent(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

script_js_property	model_light_props[]={
							{"index",				js_model_light_get_index,				js_model_light_set_index},
							{"on",					js_model_light_get_on,					js_model_light_set_on},
							{"type",				js_model_light_get_type,				js_model_light_set_type},
							{"direction",			js_model_light_get_direction,			js_model_light_set_direction},
							{"intensity",			js_model_light_get_intensity,			js_model_light_set_intensity},
							{"exponent",			js_model_light_get_exponent,			js_model_light_set_exponent},
							{0}};
							
JSClass				*model_light_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_light_object(void)
{
	model_light_class=script_create_class("model_light_class",js_model_light_get_property,js_model_light_set_property);
}

void script_free_model_light_object(void)
{
	script_free_class(model_light_class);
}

void script_add_model_light_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,model_light_class,"light",model_light_props,NULL);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_model_light_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,model_light_props));
}

JSBool js_model_light_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,model_light_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_model_light_get_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	*vp=INT_TO_JSVAL(draw->script_light_idx);

	return(JS_TRUE);
}

JSBool js_model_light_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];

	*vp=script_bool_to_value(light->on);

	return(JS_TRUE);
}

JSBool js_model_light_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	*vp=INT_TO_JSVAL(light->type-sd_light_type_normal);

	return(JS_TRUE);
}

JSBool js_model_light_get_direction(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];

	*vp=INT_TO_JSVAL(light->direction+sd_light_direction_all);

	return(JS_TRUE);
}

JSBool js_model_light_get_intensity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	*vp=INT_TO_JSVAL(light->intensity);

	return(JS_TRUE);
}

JSBool js_model_light_get_exponent(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_float_to_value(light->exponent);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_model_light_set_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	draw->script_light_idx=JSVAL_TO_INT(*vp);
	if ((draw->script_light_idx<0) || (draw->script_light_idx>=max_model_light)) draw->script_light_idx=0;

	return(JS_TRUE);
}

JSBool js_model_light_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	light->on=script_value_to_bool(*vp);

	return(JS_TRUE);
}

JSBool js_model_light_set_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	light->type=JSVAL_TO_INT(*vp)-sd_light_type_normal;

	return(JS_TRUE);
}

JSBool js_model_light_set_direction(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	light->direction=JSVAL_TO_INT(*vp)-sd_light_direction_all;

	return(JS_TRUE);
}

JSBool js_model_light_set_intensity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];

	light->intensity=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

JSBool js_model_light_set_exponent(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	light->exponent=script_value_to_float(*vp);

	return(JS_TRUE);
}

