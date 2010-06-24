/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Start/Stop

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
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "scenery.h"
#include "models.h"
#include "cameras.h"
#include "effects.h"
#include "consoles.h"
#include "interfaces.h"
#include "sounds.h"
#include "inputs.h"
#include "video.h"
#include "timing.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

char						current_map_name[name_str_len];

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern void map_clear_changes(void);
extern void map_start_ambient(void);
extern void map_set_ambient(char *name,float pitch);
extern void map_end_ambient(void);
extern bool gl_check_shader_ok(void);
extern void map_movements_initialize(void);
extern void view_draw_fade_start(void);
extern void group_move_clear_all(void);
extern void draw_sky_init(void);
extern bool view_compile_mesh_gl_list_init(void);
extern void view_compile_mesh_gl_list_free(void);
extern bool render_transparent_create_sort_list(void);
extern void render_transparent_dispose_sort_list(void);
extern void map_multiplayer_show_hide_meshes(void);
extern void view_visibility_check_calculate(void);
extern void view_clear_fps(void);

/* =======================================================

      Map Based Media and Music
      
======================================================= */

void map_media_start(map_media_type *media)
{
		// no media when launched from editor

	if (setup.editor_override.on) return;

		// no media if last change was a skip change

	if (server.skip_media) {
		server.skip_media=FALSE;
		return;
	}

		// run the media type

	switch (media->type) {
	
		case mi_story:
			story_setup(media->name,-1);
			server.next_state=gs_story;
			break;
			
		case mi_title:
			title_setup("Titles",media->name,media->title_sound_name,-1);
			server.next_state=gs_title;
			break;
			
		case mi_movie:
			movie_setup(media->name,-1);
			server.next_state=gs_movie;
			break;
		
	}
}

void map_music_start(map_music_type *music)
{
	bool			ok;
	char			err_str[256];
	
	if (!setup.music_on) return;
	if (music->name[0]==0x0) return;
	
		// stop old music
		
	if (al_music_playing()) al_music_stop();
	
		// start new music
	
	if (music->fade_msec==0) {
		ok=al_music_play(music->name,err_str);
	}
	else {
		ok=al_music_fade_in(music->name,music->fade_msec,err_str);
	}
	
		// report any errors
		
	if (!ok) console_add_error(err_str);
}

/* =======================================================

      Cache Lookups and Map Draw Flags
      
======================================================= */

void map_lookups_setup(void)
{
	int					n;
	map_liquid_type		*liq;
	map_sound_type		*sound;
	map_particle_type	*particle;
	
	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
		liq->ambient.buffer_idx=al_find_buffer(liq->ambient.sound_name);
		liq++;
	}

	sound=map.sounds;
	
	for (n=0;n!=map.nsound;n++) {
		sound->buffer_idx=al_find_buffer(sound->name);
		sound++;
	}
	
	particle=map.particles;
	
	for (n=0;n!=map.nparticle;n++) {
		particle->particle_idx=particle_find_index(particle->name);
		particle++;
	}
}

void map_mesh_polygon_draw_flag_setup(void)
{
	int					n,k;
	bool				has_opaque,has_transparent,has_glow;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// run through the meshes
		// and setup mesh and polygon draw flags
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// clear mesh flags

		has_opaque=FALSE;
		has_transparent=FALSE;
		has_glow=FALSE;
		
			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture and frame

			texture=&map.textures[poly->txt_idx];

				// set the flags

			poly->draw.transparent_on=(texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent);
			poly->draw.glow_on=(texture->frames[0].glowmap.gl_id!=-1);

				// or to the mesh flags

			has_opaque|=(!poly->draw.transparent_on);
			has_transparent|=poly->draw.transparent_on;
			has_glow|=poly->draw.glow_on;

			poly++;
		}
		
			// set mesh level flags
		
		mesh->draw.has_opaque=has_opaque;
		mesh->draw.has_transparent=has_transparent;
		mesh->draw.has_glow=has_glow;
		
		mesh++;
	}
}

/* =======================================================

      Map Start and End
      
======================================================= */

bool map_start(bool in_file_load,bool skip_media,char *err_str)
{
	int				tick;
	char			txt[256];
	obj_type		*obj;

	game_time_pause_start();

		// start progress
		
	progress_initialize("Opening",map.info.name);
	progress_draw(10);
	
	strcpy(current_map_name,map.info.name);		// remember for close
	
		// load the map
		
	sprintf(txt,"Opening %s",map.info.name);
	console_add_system(txt);
	
	map_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_quality_mode,setup.compress_on,TRUE);

// supergumba -- auto generator testing
/*
	if (!map_auto_generate_test(&map)) {
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s",map.info.name);
		return(FALSE);
	}
*/
	if (!map_open(&map,map.info.name)) {
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s.  If this map is from an older version of dim3, use Editor to upgrade it.",map.info.name);
		return(FALSE);
	}

		// don't run blank maps

	if ((map.mesh.nmesh==0) || (map.nspot==0)) {
		map_close(&map);
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s.  This map contains no meshes or spots.",map.info.name);
		return(FALSE);
	}

	gl_shader_attach_map();

		// prepare map surfaces
	
	progress_draw(20);

	map_prepare(&map);
	map_multiplayer_show_hide_meshes();

	view_visibility_check_calculate();

		// map lists

	progress_draw(30);

	if (!render_transparent_create_sort_list()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	if (!map_group_create_unit_list(&map)) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	if (!view_compile_mesh_gl_list_init()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	draw_sky_init();

		// start map ambients
		// and clear all proj, effects, decals, etc
		
	progress_draw(40);

	map_start_ambient();
	if (map.ambient.sound_name[0]!=0x0) map_set_ambient(map.ambient.sound_name,map.ambient.sound_pitch);

	if (!projectile_initialize_list()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	if (!effect_initialize_list()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	if (!decal_initialize_list()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	particle_map_initialize();
	group_move_clear_all();
	
		// reset rain
		
	map.rain.reset=TRUE;
	
		// setup FS shaders
		
	gl_fs_shader_map_start();

        // run the course script

	progress_draw(50);
		
	js.course_attach.thing_type=thing_type_course;

	scripts_clear_attach_data(&js.course_attach);
	
	if (!scripts_add(&js.course_attach,"Courses",map.info.name,err_str)) {
		progress_shutdown();
		return(FALSE);
	}
	
		// create object and scenery
		// and call spawn on all the objects

	progress_draw(70);
	
	map_find_random_spot_clear(&map,NULL,-1);

	map_objects_create();
	remote_setup_coop_bots();

	scenery_create();
	scenery_start();
	
		// if not restoring a existing game,
		// spawn objects into map
		
	if (!in_file_load) {
		if (!map_object_attach_all(err_str)) {
			progress_shutdown();
			return(FALSE);
		}
	}
	
		// attach player to map

	progress_draw(80);

	if (net_setup.mode!=net_mode_host_dedicated) {

		player_clear_input();
		
			// connect camera to player
			
		if (!in_file_load) {
			obj=server.obj_list.objs[server.player_obj_idx];
			camera_connect(obj);
		}
	}
	
		// initialize movements and lookups

	progress_draw(90);
	
	map_movements_initialize();
	map_lookups_setup();
	map_mesh_polygon_draw_flag_setup();
	
	gl_back_render_map_start();
	
		// map start event
		
	progress_draw(95);

	scripts_post_event_console(&js.game_attach,sd_event_map,sd_event_map_open,0);
	scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_map_open,0);

	if (net_setup.mode!=net_mode_host_dedicated) {
		scripts_post_event_console(&obj->attach,sd_event_map,sd_event_map_open,0);
	}
	
		// finish up

	progress_draw(100);
	
	progress_shutdown();
	
		// map open
		
	server.map_open=TRUE;
	
		// flag off all map changes
		
	map_clear_changes();
	
		// restart all the timing
		
	tick=game_time_get();
	
	map.start_game_tick=tick;
	server.time.run_tick=tick;
	server.time.network_update_tick=tick;
	server.time.network_group_synch_tick=tick;
	server.time.network_latency_ping_tick=tick;	
	view.time.run_tick=tick;
	js.timer_tick=tick;

	view_clear_fps();

		// clear all input
		
	input_clear();
	
	game_time_pause_end();
	
		// start any map open media
		
	if (!skip_media) map_media_start(&map.media);
	map_music_start(&map.music);

		// start any map fades

	if (!skip_media) view_draw_fade_start();
	
	return(TRUE);
}

void map_end(void)
{
	obj_type		*obj;
	
	game_time_pause_start();
	
		// detach objects
		
	map_object_detach_all();

		// setup progress
		
	progress_initialize("Closing",current_map_name);
	progress_draw(5);
	
	console_add_system("Closing Map");
	
		// map close event
		
	scripts_post_event_console(&js.game_attach,sd_event_map,sd_event_map_close,0);
	scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_map_close,0);

	if (net_setup.mode!=net_mode_host_dedicated) {
		obj=server.obj_list.objs[server.player_obj_idx];
		scripts_post_event_console(&obj->attach,sd_event_map,sd_event_map_close,0);
	}

		// clear all back buffers

	gl_fs_shader_map_end();
	gl_back_render_map_end();
	
		// stop sounds
			
	progress_draw(15);

	map_end_ambient();
	al_stop_all_sources();

		// remove all projectiles
	
	progress_draw(25);

	projectile_dispose_all();

		// free some map lists

	progress_draw(30);

	projectile_free_list();
	effect_free_list();
	decal_free_list();

        // end course script
		
	progress_draw(35);

	scripts_dispose(js.course_attach.script_idx);

		// free map bound items
		
	progress_draw(45);

	object_dispose_2(bt_map);
	
		// free group, portal segment, vertex and light lists
		
	progress_draw(65);

	view_compile_mesh_gl_list_free();
	render_transparent_dispose_sort_list();
	map_group_dispose_unit_list(&map);
	
		// close map
	
	progress_draw(85);

	map_close(&map);
	
		// finish

	progress_draw(100);
	
	progress_shutdown();
	
		// map closed
		
	server.map_open=FALSE;
	
	game_time_pause_end();
}

/* =======================================================

      In Game Map changes
      
======================================================= */

void map_clear_changes(void)
{
	server.map_change=FALSE;
	server.skip_media=FALSE;
}

bool map_need_rebuild(void)
{
	return(server.map_change);
}

bool map_rebuild_changes(char *err_str)
{
	if (server.map_open) map_end();
	return(map_start(FALSE,FALSE,err_str));
}
