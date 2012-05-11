/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Group Particle

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

#define kGroupParticleSettingsName		0
#define kGroupParticleSettingsShift		1

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Group Particle
      
======================================================= */

void property_palette_fill_group_particle(int particle_idx,int group_particle_idx)
{
	iface_particle_type					*particle;
	iface_particle_group_piece_type			*group_particle;

	particle=&iface.particle_list.particles[particle_idx];
	group_particle=&particle->group.particles[group_particle_idx];

	list_palette_set_title(&property_palette,"Particles",NULL,"Particle Group",particle->name,"Particle",group_particle->name);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_picker_list_string(&property_palette,kGroupParticleSettingsName,"Particle",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),FALSE,group_particle->name,FALSE);
	list_palette_add_int(&property_palette,kGroupParticleSettingsShift,"Shift",&group_particle->shift,FALSE);
}

/* =======================================================

      Property Palette Click Group Particle
      
======================================================= */

void property_palette_click_group_particle(int particle_idx,int group_particle_idx,bool double_click)
{
}
