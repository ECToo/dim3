/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: File Menu Routines

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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern map_type					map;
extern file_path_setup_type		file_path_setup;
extern editor_setup_type		setup;
extern editor_state_type		state;

extern list_palette_type		file_palette,property_palette;

/* =======================================================

      Reset Windows
            
======================================================= */

void file_reset_state(void)
{
		// if there is an open map,
		// change the palette opens

	if (state.map_open) {
		os_set_title_window(state.map_file_name);

		file_palette.open=FALSE;
		property_palette.open=TRUE;
		property_palette_reset();
	}
	else {
		os_set_title_window("dim3 Editor");
	}

        // no selection
        
	select_clear();
	undo_clear();
	
 	state.texture_edit_idx=-1;
	state.in_preference=FALSE;

		// reset the UI

	tool_default();
	menu_update_view();
	menu_fix_enable();
	texture_palette_reset();
	
		// make sure window is
		// seleted

    os_select_window();
    
	main_wind_draw();
}

/* =======================================================

      New Map
      
======================================================= */

bool file_new_map(void)
{
	char		file_name[256];
	
		// close existing map

	if (!file_close_map()) return(FALSE);

		// get the map name
		
	strcpy(file_name,"NewMap");
	if (!dialog_file_new_run("Create a New Map",file_name)) return(FALSE);
	
		// set the map paths
		
	map_setup(&file_path_setup,setup.mipmap_mode,FALSE,FALSE);
	
		// create the map
		
    map_new(&map,file_name);
	view_setup_default_views();
	
		// start models
		
	view_models_start();
		
		// set flags as opened

 	state.map_open=TRUE;
	strcpy(state.map_file_name,file_name);

		// save the map so it
		// appears in the list

	file_save_map();

		// reset the state

	file_reset_state();
	
	return(TRUE);
}

/* =======================================================

      Open Map
      
======================================================= */

bool file_open_map(char *file_name)
{
	int			n;
	char		str[256];
	bool		ok;

		// close existing map

	if (!file_close_map()) return(FALSE);
	
		// open the map
		
	os_set_wait_cursor();
	
	sprintf(str,"Loading %s...",file_name);
	progress_start("Loading...",11);
	
	map_setup(&file_path_setup,setup.mipmap_mode,FALSE,FALSE);
	progress_next();
	
	ok=map_open(&map,file_name);
	progress_next();

	map_textures_read_setup(&map);

	for (n=0;n!=max_map_texture;n++) {
		map_textures_read_texture(&map,n);
		if ((n%32)==0) progress_next();
	}

	progress_end();

    os_set_arrow_cursor();
	
	if (!ok) {
		state.map_open=FALSE;
		file_reset_state();
		return(FALSE);
	}

		// start the VBOs

	view_vbo_map_initialize();
	
		// if no views in map, then
		// set to default
		
	if (map.editor_views.count==0) 	view_setup_default_views();
	
		// start models
		
	view_models_start();
	
		// set flags as opened
		
	state.map_open=TRUE;
	strcpy(state.map_file_name,file_name);

		// reset the state

	file_reset_state();
	
	return(TRUE);
}

/* =======================================================

      Save Map
      
======================================================= */

void file_save_map(void)
{
	char			err_str[256];
	bool			save_ok;

	os_set_wait_cursor();

	progress_start("Saving...",3);
	
	progress_next();
	node_path_rebuild();
	
	progress_next();
	save_ok=map_save(&map,err_str);
	
	progress_next();
	progress_end();
	
	os_set_arrow_cursor();
	
	if (!save_ok) os_dialog_alert("dim3 Editor could not save map",err_str);
}

/* =======================================================

      Close Map
      
======================================================= */

bool file_close_map(void)
{
	int			choice;

		// if no map open, just return
		// that close is OK

	if (!state.map_open) return(TRUE);

		// confirm save
	
	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this map?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) file_save_map();

		// close map

	os_set_wait_cursor();

		// delete VBOs

	view_vbo_map_free();

		// close map

	map_close(&map);
	view_models_close();

		// reset state
	
	state.map_open=FALSE;
	file_reset_state();
	
	os_set_arrow_cursor();

	return(TRUE);
}

