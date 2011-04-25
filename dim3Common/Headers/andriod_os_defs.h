/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Specific OS Definitions

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

(c) 2000-2005 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// includes
//

//
// os flags
//

#define D3_OS_LINUX			    1
#define D3_OPENGL_ES			1

//
// os specific keys
//

#define D3_KEY_TAB				0x9
#define D3_KEY_ESC				0x1B
#define D3_KEY_BACKSPACE		0x8
#define D3_KEY_DELETE			0x7F

//
// os specific types
//

typedef unsigned char			byte;
typedef unsigned char*			ptr;
typedef unsigned char			bool8;
typedef int						bool;
typedef int						d3socket;

#define TRUE					1
#define FALSE					0
#define D3_NULL_SOCKET			-1

#define closesocket(x)			close(x)