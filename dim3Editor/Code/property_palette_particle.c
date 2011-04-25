/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Particle

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kParticleOn								0
#define kParticlePropertyName					1
#define kParticlePropertySpawnTick				2
#define kParticlePropertySlopTick				3
#define kParticlePropertySingleSpawn			4

#define kLightPropertyOn						10
#define kLightPropertyLightMap					11
#define kLightPropertyType						12
#define kLightPropertyDirection					13
#define kLightPropertyIntensity					14
#define kLightPropertyExponent					15
#define kLightPropertyColor						16
#define kLightPropertyHalo						17

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

extern char						light_property_type_list[][name_str_len],
								light_property_direction_list[][name_str_len];

/* =======================================================

      Property Palette Fill Particle
      
======================================================= */

void property_palette_fill_particle(int particle_idx)
{
	map_particle_type		*particle;

	particle=&map.particles[particle_idx];

		// particle
		
	list_palette_add_header(&property_palette,0,"Particle Settings");
	list_palette_add_checkbox(&property_palette,kParticleOn,"On",particle->on,FALSE);

	list_palette_add_header(&property_palette,0,"Particle Type");
	list_palette_add_string(&property_palette,kParticlePropertyName,"Name",particle->name,FALSE);
	list_palette_add_string_int(&property_palette,kParticlePropertySpawnTick,"Spawn Tick",particle->spawn_tick,FALSE);
	list_palette_add_string_int(&property_palette,kParticlePropertySlopTick,"Slop Tick",particle->slop_tick,FALSE);
	list_palette_add_checkbox(&property_palette,kParticlePropertySingleSpawn,"Single Spawn",particle->single_spawn,FALSE);

		// particle lighting
		
	list_palette_add_header(&property_palette,0,"Particle Light Settings");
	list_palette_add_checkbox(&property_palette,kLightPropertyOn,"On",particle->light_setting.on,FALSE);
	list_palette_add_checkbox(&property_palette,kLightPropertyLightMap,"Used in Light Map",particle->light_setting.light_map,FALSE);

	list_palette_add_header(&property_palette,0,"Particle Light Display");
	list_palette_add_string(&property_palette,kLightPropertyType,"Type",light_property_type_list[particle->light_setting.type],FALSE);
	list_palette_add_string(&property_palette,kLightPropertyDirection,"Direction",light_property_direction_list[particle->light_setting.direction],FALSE);
	list_palette_add_string_int(&property_palette,kLightPropertyIntensity,"Intensity",particle->light_setting.intensity,FALSE);
	list_palette_add_string_float(&property_palette,kLightPropertyExponent,"Exponent",particle->light_setting.exponent,FALSE);
	list_palette_add_pick_color(&property_palette,kLightPropertyColor,"Color",&particle->light_setting.col,FALSE);
	list_palette_add_string(&property_palette,kLightPropertyHalo,"Halo",particle->light_setting.halo_name,FALSE);
	
	list_palette_add_header(&property_palette,0,"Paticle Info");
	list_palette_add_string_int(&property_palette,-1,"Index",particle_idx,TRUE);
	list_palette_add_point(&property_palette,-1,"Position",&particle->pnt,TRUE);
}

/* =======================================================

      Property Palette Click Particle
      
======================================================= */

void property_palette_click_particle(int particle_idx,int id)
{
	map_particle_type		*particle;

	particle=&map.particles[particle_idx];

	switch (id) {

			// particle
			
		case kParticleOn:
			particle->on=!particle->on;
			break;

		case kParticlePropertyName:
			property_palette_pick_particle(particle->name);
			break;

		case kParticlePropertySpawnTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&particle->spawn_tick,0,0,0);
			break;
			
		case kParticlePropertySlopTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&particle->slop_tick,0,0,0);
			break;

		case kParticlePropertySingleSpawn:
			particle->single_spawn=!particle->single_spawn;
			break;
			
			// particle lighting
			
		case kLightPropertyOn:
			particle->light_setting.on=!particle->light_setting.on;
			break;

		case kLightPropertyLightMap:
			particle->light_setting.light_map=!particle->light_setting.light_map;
			break;

		case kLightPropertyType:
			property_pick_list("Pick a Light Type",(char*)light_property_type_list,&particle->light_setting.type);
			break;

		case kLightPropertyDirection:
			property_pick_list("Pick a Light Direction",(char*)light_property_direction_list,&particle->light_setting.direction);
			break;

		case kLightPropertyIntensity:
			dialog_property_string_run(list_string_value_positive_int,(void*)&particle->light_setting.intensity,0,0,0);
			break;
			
		case kLightPropertyExponent:
			dialog_property_string_run(list_string_value_positive_float,(void*)&particle->light_setting.exponent,0,0,0);
			break;
			
		case kLightPropertyColor:
			os_pick_color(&particle->light_setting.col);
			break;

		case kLightPropertyHalo:
			property_palette_pick_halo(particle->light_setting.halo_name);
			break;

	}

	main_wind_draw();
}

