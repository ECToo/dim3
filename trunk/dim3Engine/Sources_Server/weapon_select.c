/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Selecting

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
#include "weapons.h"
#include "models.h"

extern server_type				server;
extern camera_type				camera;

/* =======================================================

      Reset Ammo for Weapons
      
======================================================= */

void weapon_reset_ammo(weapon_type *weap)
{
	weap->ammo.count=weap->ammo.init_count;
	weap->ammo.clip_count=weap->ammo.init_clip_count;
	
	weap->alt_ammo.count=weap->alt_ammo.init_count;
	weap->alt_ammo.clip_count=weap->alt_ammo.init_clip_count;
}	


void weapon_reset_ammo_object(obj_type *obj)
{
    int					n;
	weapon_type			*weap;
    
    weap=server.weapons;
    
    for (n=0;n!=server.count.weapon;n++) {
		if (weap->obj_uid==obj->uid) weapon_reset_ammo(weap);
        weap++;
    }
}
	
/* =======================================================

      Set Weapon
      
======================================================= */

void weapon_clear_animation(weapon_type *weap)
{
	int						n;
	model_draw_animation	*animation;
	
	animation=weap->draw.animations;

	for (n=0;n!=max_model_blend_animation;n++) {
		animation->mode=am_stopped;
		animation->smooth_animate_idx=-1;
		animation->smooth_pose_move_idx=0;
		animation++;
	}
}

void weapon_set(obj_type *obj,weapon_type *weap)
{
	obj->held_weapon.current_uid=weap->uid;
	obj->held_weapon.next_uid=weap->uid;
    obj->held_weapon.mode=wm_held;
    
	weapon_clear_animation(weap);

	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_held,0);
	
	scripts_post_event_console(&weap->attach,sd_event_weapon_select,0,0);
	scripts_post_event_console(&obj->attach,sd_event_weapon_select,0,0);
}

/* =======================================================

      Change Weapon
      
======================================================= */

void weapon_lower(int tick,obj_type *obj)
{
	weapon_type			*weap;
	
	obj->held_weapon.mode=wm_lower;
	obj->held_weapon.swap_tick=tick;
    
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
	
	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_lower,0);
}

void weapon_raise(int tick,obj_type *obj)
{
	weapon_type			*weap;
	
    obj->held_weapon.mode=wm_raise;
 	obj->held_weapon.swap_tick=tick;
   
	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_raise,0);
}

void weapon_held(obj_type *obj)
{
 	weapon_type			*weap;
	
	obj->held_weapon.mode=wm_held;
    
	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_held,0);
}

void weapon_cur_weapon_recoil_copy(obj_type *obj,int copy_weap_uid)
{
	weapon_type		*to_weap,*cur_weap;

	to_weap=weapon_find_uid(copy_weap_uid);
	if (to_weap==NULL) return;
				
	cur_weap=weapon_find_current(obj);
	if (cur_weap==NULL) {
		weapon_recoil_clear(to_weap);
		return;
	}

	weapon_recoil_copy(to_weap,cur_weap);
}

void weapon_goto(int tick,obj_type *obj,weapon_type *weap)
{
		// copy any recoils

	weapon_cur_weapon_recoil_copy(obj,weap->uid);

		// set next weapon
	
	obj->held_weapon.next_uid=weap->uid;

		// clear any animation smoothing

	weapon_clear_animation(weap);

        // tell player about new weapon
		
	scripts_post_event_console(&weap->attach,sd_event_weapon_select,0,0);
	scripts_post_event_console(&obj->attach,sd_event_weapon_select,0,0);
        
        // start hand swap

    if (obj->held_weapon.mode!=wm_lower) weapon_lower(tick,obj);
}

void weapon_switch(int tick,obj_type *obj,int dir)
{
    int				weap_mode,weap_uid,weap_idx;
    weapon_type		*weap;

    weap_mode=obj->held_weapon.mode;
    
        // already changing weapons?
        
    if (weap_mode!=wm_lower) {
		weap_uid=obj->held_weapon.current_uid;
    }
    else {
		weap_uid=obj->held_weapon.next_uid;
    }
	
	weap_idx=weapon_index_find_uid(weap_uid);
	if (weap_idx==-1) return;
	
		// change to new weapon
    
    while (TRUE) {
        weap_idx+=dir;
        if (weap_idx<0) weap_idx=server.count.weapon-1;
        if (weap_idx>=server.count.weapon) weap_idx=0;
		
		weap=&server.weapons[weap_idx];
		
			// have we wrapped back around?
			
		if (weap->uid==obj->held_weapon.current_uid) return;
			
			// can we select this weapon?
			
		if (weap->hidden) continue;
		
			// is this the right weapon?
			
        if (weap->obj_uid==obj->uid) break;
    }
    
    weapon_goto(tick,obj,weap);
}

void weapon_pick(int tick,obj_type *obj,int offset)
{
    weapon_type			*weap;
	
		// find weapon in list
		
	weap=weapon_find_offset(obj,offset);
	if (weap==NULL) return;
	
    if (weap->uid==obj->held_weapon.current_uid) return;
	if (weap->hidden) return;
	
		// set weapon
    
    weapon_goto(tick,obj,weap);
}    
   
/* =======================================================

      Weapon Zooming
      
======================================================= */

void weapon_zoom_on(obj_type *obj,weapon_type *weap)
{
	if (weap->zoom.active) return;

	weap->zoom.active=TRUE;
	obj->zoom_draw.on=FALSE;

		// fix looking problems when zooming

	object_player_look_fix_zoom(obj,weap);

		// save old camera mode and switch to fpp

	obj->zoom_draw.old_camera_mode=camera.mode;
	camera.mode=cv_fpp;

 	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_zoom_enter,0);
}
void weapon_zoom_off(obj_type *obj,weapon_type *weap)
{
	if (!weap->zoom.active) return;

		// switch back to original camera mode

	camera.mode=obj->zoom_draw.old_camera_mode;

		// turn off zoom

	weap->zoom.active=FALSE;
	obj->zoom_draw.on=FALSE;

	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_zoom_exit,0);
}

/* =======================================================

      Run Player Weapons
      
======================================================= */

void weapon_run_hand(obj_type *obj,int tick)
{
    int					weap_mode,swap_tick;
	weapon_type			*weap;
	
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
    
        // model animations
       
	model_run_animation(&weap->draw);
	model_fade_run(tick,&weap->draw);
	model_mesh_fade_run(tick,&weap->draw);

		// recoil changes

	weapon_recoil_run(obj,weap);
    
        // raising or lowering models
	
	weap_mode=obj->held_weapon.mode;
    swap_tick=tick-obj->held_weapon.swap_tick;
        
    if (weap_mode==wm_lower) {
        if (swap_tick>weap->hand.lower_tick) {
			weapon_cur_weapon_recoil_copy(obj,obj->held_weapon.next_uid);
            obj->held_weapon.current_uid=obj->held_weapon.next_uid;
            weapon_raise(tick,obj);
        }
        return;
    }
    
    if (weap_mode==wm_raise) {
        if (swap_tick>weap->hand.raise_tick) {
            weapon_held(obj);
        }
        return;
    }    
}

/* =======================================================

      Run Weapon Bouncing
      
======================================================= */

void weapon_hand_bounce(obj_type *obj)
{
	int			bounce_add;
	
		// return weapon to neutral
		
	if ((!obj->forward_move.moving) && (!obj->side_move.moving)) {
	
		if (obj->held_weapon.bounce_y>0) {
			obj->held_weapon.bounce_y-=8;
			if (obj->held_weapon.bounce_y<0) obj->held_weapon.bounce_y=0;
		}
	
		return;
	}
	
		// bounce weapon
		
	bounce_add=3;
	if (obj->forward_move.running) bounce_add=5;
	
	obj->held_weapon.bounce_y+=bounce_add;
	if (obj->held_weapon.bounce_y>=360) obj->held_weapon.bounce_y=0;
}

int weapon_get_bounce(obj_type *obj)
{
	float		f;
	
	f=(float)sin(((float)obj->held_weapon.bounce_y)*ANG_to_RAD);
	return((int)(f*30));
}

