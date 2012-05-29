/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Title Page

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kIntroPropertyClickSound						0
#define kIntroPropertyMusic								1

#define kIntroPropertyButtonGameNew						10
#define kIntroPropertyButtonGameLoad					11
#define kIntroPropertyButtonGameSetup					12
#define kIntroPropertyButtonMultiplayerHost				13
#define kIntroPropertyButtonMultiplayerJoin				14
#define kIntroPropertyButtonCredit						15
#define kIntroPropertyButtonQuit						16

#define kIntroPropertySimpleSaveDescTextSize			30
#define kIntroPropertySimpleSaveProgressOn				31
#define kIntroPropertySimpleSaveProgressMaxPoint		32
#define kIntroPropertySimpleSaveProgressMaxBitmap		33
#define kIntroPropertySimpleSaveProgressWid				34
#define kIntroPropertySimpleSaveProgressHigh			35
#define kIntroPropertySimpleSaveProgressAdd				36
#define kIntroPropertySimpleSaveProgressHorizontal		37
#define kIntroPropertySimpleSaveProgressWrapCount		38
#define kIntroPropertySimpleSaveProgressBitmap			39
#define kIntroPropertySimpleSaveProgressBitmapDisable	40

#define kIntroPropertyConfirmX							45
#define kIntroPropertyConfirmY							46

#define kIntroPropertyButtonSimpleSaveStart				50
#define kIntroPropertyButtonSimpleSaveErase				60

#define kIntroPropertyFont								70

#define kIntroPropertyScoreOn							80
#define kIntroPropertyScoreX							81
#define kIntroPropertyScoreY							82
#define kIntroPropertyScoreWid							83
#define kIntroPropertyScoreHigh							84
#define kIntroPropertyScoreTextSize						85
#define kIntroPropertyScoreFormat						86
#define kIntroPropertyScoreColor						87

#define kIntroPropertyModelAdd							100
#define kIntroPropertyModelName							1000
#define kIntroPropertyModelDelete						2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

extern char						score_format_str[][32];

/* =======================================================

      Property Palette Fill Title Page
      
======================================================= */

void property_palette_fill_title_page(void)
{
	int				n;
	char			name[256],str[256];

	list_palette_set_title(&property_palette,"Title Page",NULL,NULL,NULL,NULL,NULL);

		// fonts
		
	list_palette_add_header(&property_palette,0,"Font");
	for (n=0;n!=max_iface_font_variant;n++) {
		sprintf(name,"Interface Font %d",n);
		list_palette_add_string(&property_palette,(kIntroPropertyFont+n),name,iface.font.interface_name[n],name_str_len,FALSE);
	}

		// sound
		
	list_palette_add_header(&property_palette,0,"Sound");
	list_palette_add_picker_list_string(&property_palette,kIntroPropertyClickSound,"Click",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,iface.click_sound,FALSE);
	list_palette_add_picker_file(&property_palette,kIntroPropertyMusic,list_button_none,0,"Music","Music","mp3","",iface.intro.music,FALSE);

		// buttons

	list_palette_add_header(&property_palette,0,"Regular Buttons");
	list_palette_add_string_selectable(&property_palette,kIntroPropertyButtonGameNew,"Game New",NULL,-1,(state.cur_intro_button_idx==item_intro_button_game_new),FALSE);
	list_palette_add_string_selectable(&property_palette,kIntroPropertyButtonGameLoad,"Game Load",NULL,-1,(state.cur_intro_button_idx==item_intro_button_game_load),FALSE);
	list_palette_add_string_selectable(&property_palette,kIntroPropertyButtonGameSetup,"Game Setup",NULL,-1,(state.cur_intro_button_idx==item_intro_button_game_setup),FALSE);
	list_palette_add_string_selectable(&property_palette,kIntroPropertyButtonMultiplayerHost,"Multiplayer Host",NULL,-1,(state.cur_intro_button_idx==item_intro_button_multiplayer_host),FALSE);
	list_palette_add_string_selectable(&property_palette,kIntroPropertyButtonMultiplayerJoin,"Multiplayer Join",NULL,-1,(state.cur_intro_button_idx==item_intro_button_multiplayer_join),FALSE);
	list_palette_add_string_selectable(&property_palette,kIntroPropertyButtonCredit,"Credit",NULL,-1,(state.cur_intro_button_idx==item_intro_button_credit),FALSE);
	list_palette_add_string_selectable(&property_palette,kIntroPropertyButtonQuit,"Quit",NULL,-1,(state.cur_intro_button_idx==item_intro_button_quit),FALSE);

		// simple save settings
		
	list_palette_add_header(&property_palette,0,"Simple Save Settings");
	list_palette_add_int(&property_palette,kIntroPropertySimpleSaveDescTextSize,"Description Text Size",&iface.intro.simple_save_list.desc.text_size,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertyConfirmX,"Erase Confirm Dialog X",&iface.intro.confirm.x,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertyConfirmY,"Erase Confirm Dialog Y",&iface.intro.confirm.y,FALSE);

		// simple save buttons
		
	list_palette_add_header(&property_palette,0,"Simple Save Buttons");
	for (n=0;n!=max_simple_save_spot;n++) {
		sprintf(str,"Simple Save Start %d",n);
		list_palette_add_string_selectable(&property_palette,(kIntroPropertyButtonSimpleSaveStart+n),str,NULL,-1,(state.cur_intro_button_idx==(item_intro_button_simple_save_start+n)),FALSE);
	}

	list_palette_add_header(&property_palette,0,"Simple Save Erase Buttons");
	for (n=0;n!=max_simple_save_spot;n++) {
		sprintf(str,"Simple Save Erase %d",n);
		list_palette_add_string_selectable(&property_palette,(kIntroPropertyButtonSimpleSaveErase+n),str,NULL,-1,(state.cur_intro_button_idx==(item_intro_button_simple_save_erase+n)),FALSE);
	}
	
		// simple save settings
		
	list_palette_add_header(&property_palette,0,"Simple Save Progress");
	list_palette_add_checkbox(&property_palette,kIntroPropertySimpleSaveProgressOn,"On",&iface.intro.simple_save_list.progress.on,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertySimpleSaveProgressMaxPoint,"Max Points",&iface.intro.simple_save_list.progress.max_point,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertySimpleSaveProgressMaxBitmap,"Max Bitmaps",&iface.intro.simple_save_list.progress.max_bitmap,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertySimpleSaveProgressWid,"Bitmap Width",&iface.intro.simple_save_list.progress.wid,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertySimpleSaveProgressHigh,"Bitmap Height",&iface.intro.simple_save_list.progress.high,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertySimpleSaveProgressAdd,"Bitmap Add",&iface.intro.simple_save_list.progress.bitmap_add,FALSE);
	list_palette_add_checkbox(&property_palette,kIntroPropertySimpleSaveProgressHorizontal,"Horizontal",&iface.intro.simple_save_list.progress.horizontal,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertySimpleSaveProgressWrapCount,"Wrap Count",&iface.intro.simple_save_list.progress.wrap_count,FALSE);
	list_palette_add_picker_file(&property_palette,kIntroPropertySimpleSaveProgressBitmap,list_button_none,0,"Enabled Bitmap","Bitmaps/Interface","png","",iface.intro.simple_save_list.progress.bitmap_name,FALSE);
	list_palette_add_picker_file(&property_palette,kIntroPropertySimpleSaveProgressBitmapDisable,list_button_none,0,"Disabled Bitmap","Bitmaps/Interface","png","",iface.intro.simple_save_list.progress.bitmap_disable_name,FALSE);

		// score
		
	list_palette_add_header(&property_palette,0,"Scores");
	list_palette_add_checkbox(&property_palette,kIntroPropertyScoreOn,"On",&iface.intro.score.on,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertyScoreX,"X",&iface.intro.score.x,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertyScoreY,"Y",&iface.intro.score.y,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertyScoreWid,"Width",&iface.intro.score.wid,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertyScoreHigh,"Height",&iface.intro.score.high,FALSE);
	list_palette_add_int(&property_palette,kIntroPropertyScoreTextSize,"Text Size",&iface.intro.score.text_size,FALSE);
	list_palette_add_picker_list_int(&property_palette,kIntroPropertyScoreFormat,"Format",(char*)score_format_str,-1,name_str_len,0,FALSE,&iface.intro.score.format,FALSE);
	list_palette_add_pick_color(&property_palette,kIntroPropertyScoreColor,"Color",&iface.intro.score.col,FALSE);

		// models

	list_palette_add_header_button(&property_palette,kIntroPropertyModelAdd,"Models",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.intro.model_list.nmodel;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kIntroPropertyModelName+n),list_button_minus,(kIntroPropertyModelDelete+n),iface.intro.model_list.models[n].model_name,(state.cur_intro_model_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Title Page
      
======================================================= */

void property_palette_click_title_page(bool double_click)
{
	int					id,idx,sz;

	id=property_palette.item_pane.click.id;

		// select button

	if ((id>=kIntroPropertyButtonGameNew) && (id<=kIntroPropertyButtonQuit)) {
		state.cur_intro_button_idx=(id-kIntroPropertyButtonGameNew)+item_intro_button_game_new;
		state.cur_intro_model_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

		// select save state button

	if ((id>=kIntroPropertyButtonSimpleSaveStart) && (id<(kIntroPropertyButtonSimpleSaveStart+max_simple_save_spot))) {
		state.cur_intro_button_idx=(id-kIntroPropertyButtonSimpleSaveStart)+item_intro_button_simple_save_start;
		state.cur_intro_model_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

	if ((id>=kIntroPropertyButtonSimpleSaveErase) && (id<(kIntroPropertyButtonSimpleSaveErase+max_simple_save_spot))) {
		state.cur_intro_button_idx=(id-kIntroPropertyButtonSimpleSaveErase)+item_intro_button_simple_save_erase;
		state.cur_intro_model_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

		// select model

	if ((id>=kIntroPropertyModelName) && (id<(kIntroPropertyModelName+max_hud_intro_model))) {
		state.cur_intro_button_idx=-1;
		state.cur_intro_model_idx=id-kIntroPropertyModelName;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

		// delete model

	if ((id>=kIntroPropertyModelDelete) && (id<(kIntroPropertyModelDelete+max_hud_intro_model))) {
		state.cur_intro_button_idx=-1;
		state.cur_intro_model_idx=-1;

		idx=id-kIntroPropertyModelDelete;

		sz=(iface.intro.model_list.nmodel-idx)-1;
		if (sz>0) memmove(&iface.intro.model_list.models[idx],&iface.intro.model_list.models[idx+1],(sz*sizeof(iface_intro_model_type)));

		iface.intro.model_list.nmodel--;

		return;
	}

		// add model

	if (id==kIntroPropertyModelAdd) {
		state.cur_intro_button_idx=-1;
		state.cur_intro_model_idx=-1;

		if (iface.intro.model_list.nmodel>=max_hud_intro_model) {
			os_dialog_alert("Add Intro Model","Reached the maximum number of models");
			return;
		}

		idx=iface.intro.model_list.nmodel;
		iface.intro.model_list.nmodel++;

		iface.intro.model_list.models[idx].model_name[0]=0x0;
		iface.intro.model_list.models[idx].x=0;
		iface.intro.model_list.models[idx].y=0;
		iface.intro.model_list.models[idx].resize=1.0f;
		iface.intro.model_list.models[idx].rot.x=0.0f;
		iface.intro.model_list.models[idx].rot.y=0.0f;
		iface.intro.model_list.models[idx].rot.z=0.0f;
		iface.intro.model_list.models[idx].animate_name[0]=0x0;

		state.cur_intro_model_idx=idx;
		
		list_palette_set_level(&property_palette,2);
		property_pick_file("Pick a Model","Models",NULL,"Mesh.xml;Model.xml",iface.intro.model_list.models[idx].model_name);

		return;
	}
}
