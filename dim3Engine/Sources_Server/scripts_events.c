/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Events

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
#include "consoles.h"
#include "interfaces.h"

extern server_type			server;
extern js_type				js;
extern setup_type			setup;

/* =======================================================

      Script Events Setup
      
======================================================= */

void scripts_setup_events(int script_uid)
{
	int				idx;
	JSValueRef		func_val;
	script_type		*script;
	
	idx=scripts_find_uid(script_uid);
	if (idx==-1) return;
	
	script=&js.scripts[idx];

	func_val=(JSObjectRef)JSObjectGetProperty(js.cx,script->global,script_string_to_value("event"),NULL);
	if (func_val==NULL) return;

	script->event_func=JSValueToObject(js.cx,func_val,NULL);
}

/* =======================================================

      Script Events
      
======================================================= */

bool scripts_post_event(attach_type *attach,int main_event,int sub_event,int id,char *err_str)
{
	int				idx;
	JSValueRef		rval,argv[5];
	script_type		*script;
	attach_type		old_attach;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	if (attach->script_uid==-1) return(TRUE);
	
	idx=scripts_find_uid(attach->script_uid);
	if (idx==-1) return(TRUE);
	
	script=&js.scripts[idx];
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=OBJECT_TO_JSVAL(script->obj);
	argv[1]=script_int_to_value(main_event);
	argv[2]=script_int_to_value(sub_event);
	argv[3]=script_int_to_value(id);
	argv[4]=script_int_to_value(js.time.current_tick);

	rval=JSObjectCallAsFunction(js.cx,script->global,script->event_func,5,argv,exception);
	if (rval==NULL) {
		script_exception_to_string(exception,err_str,256);
	}
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));
	
	return(err_str[0]==0x0);
}

void scripts_post_event_console(attach_type *attach,int main_event,int sub_event,int id)
{
	char			err_str[256];
	
	if (!scripts_post_event(attach,main_event,sub_event,id,err_str)) {
		console_add_error(err_str);
		if (setup.debug_console) console_trigger_set();
	}
}

/* =======================================================

      Script Chains
      
======================================================= */

bool scripts_chain(attach_type *attach,char *func_name,char *err_str)
{
	int				idx;
	JSValueRef		func_val,rval,argv[2];
	JSObjectRef		func_obj;
	script_type		*script;
	attach_type		old_attach;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	if (attach->script_uid==-1) return(TRUE);
	
	idx=scripts_find_uid(attach->script_uid);
	if (idx==-1) return(TRUE);
	
	script=&js.scripts[idx];
	
		// get the function
		
	func_val=(JSObjectRef)JSObjectGetProperty(js.cx,script->global,script_string_to_value(func_name),NULL);
	if (func_val==NULL) {
		sprintf(err_str,"Chaining failed, unknown function '%s'",func_name);
		return(FALSE);
	}

	func_obj=JSValueToObject(js.cx,func_val,NULL);
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=OBJECT_TO_JSVAL(script->obj);
	argv[1]=script_int_to_value(js.time.current_tick);

	rval=JSObjectCallAsFunction(js.cx,script->global,func_obj,2,argv,exception);
	if (rval==NULL) {
		script_exception_to_string(exception,err_str,256);
	}
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));
	
	return(err_str[0]==0x0);
}

void scripts_chain_console(attach_type *attach,char *func_name)
{
	char			err_str[256];
	
	if (!scripts_chain(attach,func_name,err_str)) {
		console_add_error(err_str);
		if (setup.debug_console) console_trigger_set();
	}
}

/* =======================================================

      Script Direct Call
      
======================================================= */

JSValueRef scripts_direct_call(attach_type *attach,char *func_name,int arg_count,JSValueRef *args,JSValueRef *exception)
{
	int				n,idx;
	JSValueRef		rval,func_val,argv[5];
	JSObjectRef		func_obj;
	script_type		*script;
	attach_type		old_attach;
	
		// find script
		
	if (attach->script_uid==-1) return(rval);
	
	idx=scripts_find_uid(attach->script_uid);
	if (idx==-1) return(rval);
	
	script=&js.scripts[idx];

		// find function

	func_val=(JSObjectRef)JSObjectGetProperty(js.cx,script->global,script_string_to_value(func_name),NULL);
	if (func_val==NULL) {
		sprintf(err_str,"Call failed, unknown function: %s",func_name);
		*exception=script_create_exception(err_str);
		return(script_null_to_value());
	}

	func_obj=JSValueToObject(js.cx,func_val,NULL);
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=OBJECT_TO_JSVAL(script->obj);

	for (n=0;n!=arg_count;n++) { 
		argv[n+1]=args[n];
	}

	rval=JSObjectCallAsFunction(js.cx,script->global,func_obj,(arg_count+1),argv,exception);
	if (rval==NULL) rval=script_null_to_value();
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));
	
	return(rval);
}
