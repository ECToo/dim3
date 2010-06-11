/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.touch object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_touch_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_stand(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		obj_touch_props[]={
							{"objectId",			js_obj_touch_get_objectId,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectName",			js_obj_touch_get_objectName,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayer",		js_obj_touch_get_objectIsPlayer,	NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"stand",				js_obj_touch_get_stand,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_touch_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_touch_object(void)
{
	obj_touch_class=script_create_class("obj_touch_class",obj_touch_props,NULL);
}

void script_free_obj_touch_object(void)
{
	script_free_class(obj_touch_class);
}

JSObjectRef script_add_obj_touch_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_touch_class,"touch"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_touch_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->touch.obj_uid));
}

JSValueRef js_obj_touch_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*touch_obj;

	obj=object_script_lookup();
	
	touch_obj=server.obj_list.objs[obj->touch.obj_uid];
	if (touch_obj==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,touch_obj->name));
}

JSValueRef js_obj_touch_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_bool_to_value(cx,obj->touch.obj_uid==server.player_obj_idx));
}

JSValueRef js_obj_touch_get_stand(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_bool_to_value(cx,obj->touch.stand));
}

