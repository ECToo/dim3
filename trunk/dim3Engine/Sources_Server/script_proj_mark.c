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
JSValueRef js_proj_mark_get_on(void);
JSValueRef js_proj_mark_get_name(void);
JSValueRef js_proj_mark_get_size(void);
JSValueRef js_proj_mark_get_alpha(void);
bool js_proj_mark_set_on(JSValueRef vp);
bool js_proj_mark_set_name(JSValueRef vp);
bool js_proj_mark_set_size(JSValueRef vp);
bool js_proj_mark_set_alpha(JSValueRef vp);

script_js_property	proj_mark_props[]={
							{"on",					js_proj_mark_get_on,				js_proj_mark_set_on},
							{"name",				js_proj_mark_get_name,				js_proj_mark_set_name},
							{"size",				js_proj_mark_get_size,				js_proj_mark_set_size},
							{"alpha",				js_proj_mark_get_alpha,				js_proj_mark_set_alpha},
							{0}};

JSClassRef			proj_mark_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_mark_object(void)
{
	proj_mark_class=script_create_class("proj_mark_class",js_proj_mark_get_property,js_proj_mark_set_property);
}

void script_free_proj_mark_object(void)
{
	script_free_class(proj_mark_class);
}

JSObjectRef script_add_proj_mark_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,proj_mark_class,"mark",proj_mark_props,NULL));
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

JSValueRef js_proj_mark_get_on(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_bool_to_value(proj_setup->mark.on);
	
	return(TRUE);
}

JSValueRef js_proj_mark_get_name(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_string_to_value(proj_setup->mark.name);
	
	return(TRUE);
}

JSValueRef js_proj_mark_get_size(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->mark.size);
	
	return(TRUE);
}

JSValueRef js_proj_mark_get_alpha(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
    *vp=script_float_to_value(proj_setup->mark.alpha);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_mark_set_on(JSValueRef vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
    proj_setup->mark.on=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_proj_mark_set_name(JSValueRef vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	script_value_to_string(*vp,proj_setup->mark.name,name_str_len);
	proj_setup_attach_mark(proj_setup);
	
	return(TRUE);
}

bool js_proj_mark_set_size(JSValueRef vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->mark.size=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_proj_mark_set_alpha(JSValueRef vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->mark.alpha=script_value_to_float(*vp);
	
	return(TRUE);
}





