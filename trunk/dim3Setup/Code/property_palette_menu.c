/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Menu

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kMenuPropertyMenuAdd					0

#define kMenuPropertyMenuName					1000
#define kMenuPropertyMenuDelete					2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Menu
      
======================================================= */

void property_palette_fill_menu(void)
{
	int				n;
	char			str[256];

	list_palette_set_title(&property_palette,"Menus");

	list_palette_add_header_button(&property_palette,kMenuPropertyMenuAdd,"Menus",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.menu_list.nmenu;n++) {
		strcpy(str,iface.menu_list.menus[n].name);
		if (n==0) strcat(str," (root)");
		list_palette_add_string_selectable_button(&property_palette,(kMenuPropertyMenuName+n),list_button_minus,(kMenuPropertyMenuDelete+n),str,NULL,(state.cur_menu_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Menu
      
======================================================= */

void property_palette_click_menu(int id)
{
	int					idx,sz;

		// select menu

	if ((id>=kMenuPropertyMenuName) && (id<(kMenuPropertyMenuName+max_iface_menu))) {
		state.cur_menu_idx=id-kMenuPropertyMenuName;
		state.cur_menu_item_idx=-1;
		list_palette_set_level(2);
		main_wind_draw();
		return;
	}

		// delete item

	if ((id>=kMenuPropertyMenuDelete) && (id<(kMenuPropertyMenuDelete+max_iface_menu))) {
		state.cur_menu_idx=-1;
		state.cur_menu_item_idx=-1;

		idx=id-kMenuPropertyMenuDelete;

		sz=(iface.menu_list.nmenu-idx)-1;
		if (sz>0) memmove(&iface.menu_list.menus[idx],&iface.menu_list.menus[idx+1],(sz*sizeof(iface_menu_type)));

		iface.menu_list.nmenu--;

		main_wind_draw();
		return;
	}

		// add item

	if (id==kMenuPropertyMenuAdd) {
		state.cur_menu_idx=-1;
		state.cur_menu_item_idx=-1;

		if (iface.menu_list.nmenu>=max_iface_menu) {
			os_dialog_alert("Add Menu","Reached the maximum number of menus");
			return;
		}

		idx=iface.menu_list.nmenu;
		iface.menu_list.nmenu++;
		
		iface.menu_list.menus[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.menu_list.menus[idx].name,name_str_len,0,0);
	
		iface.menu_list.menus[idx].nitem=0;

		state.cur_menu_idx=idx;

		main_wind_draw();
		return;
	}

		// redraw

	main_wind_draw();
}

