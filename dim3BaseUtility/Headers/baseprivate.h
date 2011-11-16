/****************************** File *********************************

Module: dim3 Math Utility
Author: Brian Barnes
 Usage: Private Header

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

/* =======================================================

      XML
      
======================================================= */

//
// Tag Defines
//

#define xml_root_tag				-1
#define xml_tag_chunk_count			(1024*20)
#define xml_buffer_chunk_sz			(1024*100)
#define max_tag_depth				256

//
// Tag Structure
//

typedef struct	{
					int				parent,nchild,first_child,last_child;
					unsigned long	tag_offset,attrib_offset,start_offset,end_offset;
					char			name[64];
				} xmltagtype;

/* =======================================================

      Bitmaps
      
======================================================= */

unsigned char* png_utility_read(char *path,int *p_wid,int *p_high,bool *alpha_channel);
bool png_utility_write(unsigned char *data,int wid,int high,bool alpha_channel,char *path);
bool png_utility_check(char *path,char *err_str);
bool png_utility_copy(char *srce_path,char *dest_path);
bool bitmap_texture_open(bitmap_type *bitmap,unsigned char *data,int anisotropic_mode,int mipmap_mode,bool compress,bool rectangle,bool pixelated);
void bitmap_texture_close(bitmap_type *bitmap);

/* =======================================================

      Interface
      
======================================================= */

extern void iface_read_settings_interface(iface_type *iface);
extern void iface_refresh_settings_interface_hud_only(iface_type *iface);
extern void iface_read_settings_chooser(iface_type *iface);
extern void iface_read_settings_multiplayer(iface_type *iface);
extern void iface_read_settings_particle(iface_type *iface);
extern void iface_read_settings_ring(iface_type *iface);
extern void iface_read_settings_halo(iface_type *iface);
extern void iface_read_settings_mark(iface_type *iface);
extern void iface_read_settings_crosshair(iface_type *iface);
extern void iface_read_settings_sound(iface_type *iface);
extern void iface_read_settings_action(iface_type *iface);
extern void iface_read_settings_shader(iface_type *iface);

extern bool iface_write_settings_interface(iface_type *iface,char *err_str);
extern bool iface_write_settings_chooser(iface_type *iface,char *err_str);
extern bool iface_write_settings_multiplayer(iface_type *iface,char *err_str);
extern bool iface_write_settings_particle(iface_type *iface,char *err_str);
extern bool iface_write_settings_ring(iface_type *iface,char *err_str);
extern bool iface_write_settings_halo(iface_type *iface,char *err_str);
extern bool iface_write_settings_mark(iface_type *iface,char *err_str);
extern bool iface_write_settings_crosshair(iface_type *iface,char *err_str);
extern bool iface_write_settings_sound(iface_type *iface,char *err_str);
extern bool iface_write_settings_action(iface_type *iface,char *err_str);
extern bool iface_write_settings_shader(iface_type *iface,char *err_str);

