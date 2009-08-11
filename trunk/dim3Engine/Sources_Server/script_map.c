/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map object

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

extern js_type			js;

JSValueRef js_map_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSClassRef			map_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_map_object(void)
{
	map_class=script_create_class("map_class",js_map_get_property,js_map_set_property);
}

void script_free_global_map_object(void)
{
	script_free_class(map_class);
}

JSObjectRef script_add_global_map_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,map_class,"map",NULL,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_map_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_map_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

