/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Routines

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
	#include "dim3engine.h"
#endif

#include "timing.h"

extern iface_type			iface;

/* =======================================================

      Initialize and Shutdown HUD Data
      
======================================================= */

void hud_initialize(void)
{
	iface.count.bitmap=0;
	iface.count.text=0;
	iface.count.bar=0;
	iface.count.menu=0;
	iface.count.chooser=0;

	iface.chat.nline=0;
	iface.chat.type_on=FALSE;
	iface.chat.type_str[0]=0x0;
	
	iface.score.on=FALSE;
}

/* =======================================================

      Finding
      
======================================================= */

hud_bitmap_type* hud_bitmaps_find(char *name)
{
	int					n;
	hud_bitmap_type		*bitmap;
	
	bitmap=iface.bitmaps;
	
	for (n=0;n!=iface.count.bitmap;n++) {
		if (strcasecmp(bitmap->name,name)==0) return(bitmap);
		bitmap++;
	}
	
	return(NULL);
}

hud_text_type* hud_texts_find(char *name)
{
	int					n;
	hud_text_type		*text;
	
	text=iface.texts;
	
	for (n=0;n!=iface.count.text;n++) {
		if (strcasecmp(text->name,name)==0) return(text);
		text++;
	}
	
	return(NULL);
}

hud_bar_type* hud_bars_find(char *name)
{
	int					n;
	hud_bar_type		*bar;
	
	bar=iface.bars;
	
	for (n=0;n!=iface.count.bar;n++) {
		if (strcasecmp(bar->name,name)==0) return(bar);
		bar++;
	}
	
	return(NULL);
}

/* =======================================================

      HUD Set Text
      
======================================================= */

void hud_text_set(hud_text_type *text,char *data)
{
	strcpy(text->data,data);
	text->has_return=(strstr(data,"{r}")!=NULL);
}

/* =======================================================

      HUD Utilities
      
======================================================= */

void hud_bitmaps_hide_all(void)
{
	int				n;
	hud_bitmap_type	*bitmap;

	bitmap=iface.bitmaps;
	
	for (n=0;n!=iface.count.bitmap;n++) {
		bitmap->show=FALSE;
		bitmap++;
	}
}

void hud_texts_hide_all(void)
{
	int				n;
	hud_text_type	*text;

	text=iface.texts;
	
	for (n=0;n!=iface.count.text;n++) {
		text->show=FALSE;
		text++;
	}
}

void hud_bars_hide_all(void)
{
	int				n;
	hud_bar_type	*bar;

	bar=iface.bars;
	
	for (n=0;n!=iface.count.bar;n++) {
		bar->show=FALSE;
		bar++;
	}
}

/* =======================================================

      Chat
      
======================================================= */

void chat_clear_messages(void)
{
	iface.chat.nline=0;
}

void chat_add_message(char *name,char *str,d3col *col)
{
	int					idx;
	hud_chat_line_type	*line;
	
		// need to remove a line?
		
	if (iface.chat.nline>=max_chat_lines) {
		memmove(&iface.chat.lines[0],&iface.chat.lines[1],(sizeof(hud_chat_line_type)*(max_chat_lines-1)));
		idx=iface.chat.nline-1;
	}
	else {
		idx=iface.chat.nline;
		iface.chat.nline++;
	}
	
		// set line
		
	line=&iface.chat.lines[idx];
	
	strcpy(line->name,name);
	strcpy(line->str,str);
	memmove(&line->col,col,sizeof(d3col));
	
		// last addition tick
		
	iface.chat.remove_tick=game_time_get()+(iface.chat.last_add_life_sec*1000);
}

void chat_time_out(void)
{
	int				tick;

	tick=game_time_get();
	if ((iface.chat.nline==0) || (iface.chat.remove_tick>tick)) return;
	
	memmove(&iface.chat.lines[0],&iface.chat.lines[1],(sizeof(hud_chat_line_type)*(max_chat_lines-1)));
	iface.chat.nline--;
	
	iface.chat.remove_tick=tick+(iface.chat.next_life_sec*1000);
}
