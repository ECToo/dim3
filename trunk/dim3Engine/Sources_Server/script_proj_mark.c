/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.mark object

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

JSValueRef js_proj_mark_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_mark_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_mark_get_on(JSContextRef cx);
JSValueRef js_proj_mark_get_name(JSContextRef cx);
JSValueRef js_proj_mark_get_size(JSContextRef cx);
JSValueRef js_proj_mark_get_alpha(JSContextRef cx);
void js_proj_mark_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_proj_mark_set_name(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_proj_mark_set_size(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_proj_mark_set_alpha(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		proj_mark_props[]={
							{"on",					js_proj_mark_get_on,				js_proj_mark_set_on,		kJSPropertyAttributeDontDelete},
							{"name",				js_proj_mark_get_name,				js_proj_mark_set_name,		kJSPropertyAttributeDontDelete},
							{"size",				js_proj_mark_get_size,				js_proj_mark_set_size,		kJSPropertyAttributeDontDelete},
							{"alpha",				js_proj_mark_get_alpha,				js_proj_mark_set_alpha,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			proj_mark_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_mark_object(void)
{
	proj_mark_class=script_create_class("proj_mark_class",proj_mark_props,NULL);
}

void script_free_proj_mark_object(void)
{
	script_free_class(proj_mark_class);
}

JSObjectRef script_add_proj_mark_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,proj_mark_class,"mark",proj_mark_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_proj_mark_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,proj_mark_props));
}

bool js_proj_mark_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,proj_mark_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_mark_get_on(JSContextRef cx)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_bool_to_value(cx,proj_setup->mark.on));
}

JSValueRef js_proj_mark_get_name(JSContextRef cx)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,proj_setup->mark.name));
}

JSValueRef js_proj_mark_get_size(JSContextRef cx)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->mark.size));
}

JSValueRef js_proj_mark_get_alpha(JSContextRef cx)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
    return(script_float_to_value(cx,proj_setup->mark.alpha));
}

/* =======================================================

      Setters
      
======================================================= */

void js_proj_mark_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->mark.on=script_value_to_bool(cx,vp);
}

void js_proj_mark_set_name(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) {
		script_value_to_string(cx,vp,proj_setup->mark.name,name_str_len);
		proj_setup_attach_mark(proj_setup);
	}
}

void js_proj_mark_set_size(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->mark.size=script_value_to_int(cx,vp);
}

void js_proj_mark_set_alpha(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->mark.alpha=script_value_to_float(cx,vp);
}





