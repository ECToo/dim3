/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Text Routines

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
	#ifdef D3_EDITOR
		#include "dim3editor.h"
	#else
		#include "dim3Animator.h"
	#endif
#endif

#include "glue.h"
#include "interface.h"

texture_font_type					txt_font;

/* =======================================================

      Initialize/Shutdown Text Drawing
      
======================================================= */

void text_initialize(void)
{
	bitmap_text(&txt_font,"Arial","Arial");
}

void text_shutdown(void)
{
	bitmap_close(&txt_font.bitmap);
}

/* =======================================================

      Text Drawing
      
======================================================= */

void text_draw(int x,int y,float txt_size,bool center,char *str)
{
	int			n,len,ch,xoff,yoff;
	float		f_lx,f_rx,f_wid,f_ty,f_by,
				gx_lft,gx_rgt,gy_top,gy_bot;
	char		*c;

		// get text length

	len=strlen(str);
	if (len==0) return;
	
		// centering
		
	if (center) {
	
		c=str;
		f_wid=0.0f;
	
		for (n=0;n!=len;n++) {
		
			ch=(int)*c++;
			if ((ch<'!') || (ch>'z')) {
				f_wid+=(txt_size*0.34f);
			}
			else {
				ch-='!';
				f_wid+=txt_size*txt_font.char_size[ch];
			}
		}
		
		x-=(int)(f_wid*0.5f);
	}
	
        // setup drawing
		
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
      
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,txt_font.bitmap.gl_id);

		// no wrapping

  	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		
		// create the quads

	f_lx=(float)x;
	f_by=(float)y;
	f_ty=f_by-txt_size;
	
	c=str;

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_QUADS);
	
	for (n=0;n!=len;n++) {
	
		ch=(int)*c++;
		if ((ch<'!') || (ch>'z')) {
			f_lx+=(txt_size*0.34f);
			continue;
		}

		ch-='!';

			// the UVs
			
		yoff=ch/font_bitmap_char_per_line;
		xoff=ch-(yoff*font_bitmap_char_per_line);

		gx_lft=((float)xoff)*font_bitmap_gl_xoff;
		gx_rgt=gx_lft+font_bitmap_gl_xadd;
		gy_top=((float)yoff)*font_bitmap_gl_yoff;
		gy_bot=gy_top+font_bitmap_gl_yadd;

			// the vertexes

		f_rx=f_lx+txt_size;

		glTexCoord2f(gx_lft,gy_top);
		glVertex2f(f_lx,f_ty);
		glTexCoord2f(gx_rgt,gy_top);
		glVertex2f(f_rx,f_ty);
		glTexCoord2f(gx_rgt,gy_bot);
		glVertex2f(f_rx,f_by);
		glTexCoord2f(gx_lft,gy_bot);
		glVertex2f(f_lx,f_by);

		f_lx+=(txt_size*txt_font.char_size[ch]);
	}

	glEnd();

		// fix wrapping back to default

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	glDisable(GL_TEXTURE_2D);
}
