/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Property String Routines

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_EDITOR
	#include "dim3editor.h"
#endif
#ifdef D3_ANIMATOR
	#include "dim3Animator.h"
#endif
#ifdef D3_SETUP
	#include "dim3Setup.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

int								dialog_property_string_value_type,
								dialog_property_string_value_len,
								dialog_property_string_i_min,
								dialog_property_string_i_max;
void							*dialog_property_string_value;

// controls

#define diag_prop_string_str	0
#define diag_prop_string_ok		1

os_dialog_ctrl_type		diag_property_string_ctrls[]={
							{os_dialog_ctrl_type_text_edit,diag_prop_string_str,"",5,10,440,20,TRUE,TRUE},
							{os_dialog_ctrl_type_default_button,diag_prop_string_ok,"OK",365,40,80,25,FALSE,FALSE},
							{-1,-1,"",0,0,0,0,FALSE,FALSE}
						};

/* =======================================================

      Run Property String
      
======================================================= */

void dialog_property_string_proc(int msg_type,int id)
{
	char			str[256];

	switch (msg_type) {

		case os_dialog_msg_type_init:
			property_string_get_values(dialog_property_string_value_type,dialog_property_string_value,str);
			os_dialog_set_text(diag_prop_string_str,str);
			break;

		case os_dialog_msg_type_button:
			if (id==diag_prop_string_ok) {
				os_dialog_get_text(diag_prop_string_str,str,256);
				property_string_set_values(dialog_property_string_value_type,dialog_property_string_value,dialog_property_string_value_len,dialog_property_string_i_min,dialog_property_string_i_max,str);
				os_dialog_close();
			}
			break;
	}
}

void dialog_property_string_run(int value_type,void *value,int value_len,int i_min,int i_max)
{
	char			title[256];

		// set values

	dialog_property_string_value_type=value_type;
	dialog_property_string_value=value;
	dialog_property_string_value_len=value_len;
	dialog_property_string_i_min=i_min;
	dialog_property_string_i_max=i_max;

	property_string_get_title(value_type,i_min,i_max,title);

		// run dialog

	os_dialog_run(title,450,70,diag_property_string_ctrls,dialog_property_string_proc);
}
