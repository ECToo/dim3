/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object State

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

#include "network.h"
#include "scripts.h"
#include "objects.h"
#include "weapons.h"
#include "remotes.h"
#include "models.h"
#include "physics.h"
#include "timing.h"

extern int					game_obj_rule_idx;

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern js_type				js;
extern hud_type				hud;
extern network_setup_type	net_setup;

/* =======================================================

      Object Scoring
      
======================================================= */

void object_score_recalc_place(void)
{
	int				n,k,idx,count,sz;
	short			score_order_idx[max_obj_list];
	obj_type		*obj,*order_obj;
	
	count=0;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		if ((obj->type!=object_type_player) && (obj->type!=object_type_remote) && (obj->type!=object_type_bot_multiplayer)) continue;
		
			// find place
			
		idx=count;
			
		for (k=0;k!=count;k++) {
			order_obj=server.obj_list.objs[(int)score_order_idx[k]];
			if (obj->score.score>order_obj->score.score) {
				idx=k;
				break;
			}
		}

		if (idx!=count) {
			sz=(count-idx)*sizeof(short);
			memmove(&score_order_idx[idx+1],&score_order_idx[idx],sz);
		}
		
		score_order_idx[idx]=(short)n;
		count++;
	}
	
	for (n=0;n!=count;n++) {
		server.obj_list.objs[(int)score_order_idx[n]]->score.place=n+1;
	}
}

void object_score_update(obj_type *obj)
{
		// only run rules for players or multiplayer bots

	if ((obj->type!=object_type_player) && (obj->type!=object_type_bot_multiplayer)) return;

		// run rule to update score

	game_obj_rule_idx=obj->idx;
	scripts_post_event_console(&js.game_attach,sd_event_rule,sd_event_rule_score,0);
	game_obj_rule_idx=-1;
	
		// update placing information
		
	object_score_recalc_place();

		// alert object of score change

	scripts_post_event_console(&obj->attach,sd_event_score,0,0);
}

void object_score_death(obj_type *obj)
{
	obj_type		*source_obj;
	
		// suicide
		
	if ((obj->idx==obj->damage_obj_idx) || (obj->damage_obj_idx==-1)) {
		obj->score.suicide++;
		object_score_update(obj);
	}
	else {

			// death

		obj->score.death++;
		object_score_update(obj);
	
			// kill
		
		if (obj->damage_obj_idx!=-1) {
			source_obj=server.obj_list.objs[obj->damage_obj_idx];
		
			source_obj->score.kill++;
			object_score_update(source_obj);
		}
	}
}

void object_score_goal(obj_type *obj)
{
	obj->score.goal++;
	object_score_update(obj);
}

void object_score_reset(obj_type *obj)
{
	obj->score.kill=obj->score.death=obj->score.suicide=obj->score.goal=obj->score.score=0;
	obj->score.place=1;
}

/* =======================================================

      Object Death
      
======================================================= */

void object_death(obj_type *obj)
{
		// has a death been triggered?
		
	if (!obj->death_trigger) return;
	
		// remove all health
		
    obj->status.health=0;
    
		// send death message
		
	scripts_post_event_console(&obj->attach,sd_event_die,0,0);
	
		// change score
		
	object_score_death(obj);
	
		// turn off trigger
		
	obj->death_trigger=FALSE;
	obj->death_telefrag=FALSE;

		// send alert to all objects with watches on

	object_watch_death_alert(obj);
	
		// send death if joined and is player or multiplayer bot
		
	if (net_setup.mode!=net_mode_none) {
		if ((obj->type==object_type_player) || (obj->type==object_type_bot_multiplayer)) net_client_send_death(obj,FALSE);
	}
	
		// setup respawn timer
		
	if (net_setup.mode==net_mode_none) {
		obj->status.respawn_tick=game_time_get()+(setup.network.respawn_secs*1000);
	}
	else {
		obj->status.respawn_tick=game_time_get()+(net_setup.respawn_secs*1000);
	}
}

/* =======================================================

      Object Telefragging Check
      
======================================================= */

void object_telefrag(obj_type *obj,obj_type *source_obj)
{
	scripts_post_event_console(&obj->attach,sd_event_telefrag,0,0);
	
	obj->death_trigger=TRUE;		// trigger death
	obj->death_telefrag=TRUE;
	obj->damage_obj_idx=source_obj->idx;
	
	object_death(obj);
}

bool object_telefrag_players(obj_type *obj,bool check_only)
{
	int			n;
	bool		hit;
	obj_type	*check_obj;

		// only players, remotes, and multiplayer bots can telefrag

	if ((obj->type!=object_type_player) && (obj->type!=object_type_remote) && (obj->type!=object_type_bot_multiplayer)) return(FALSE);

		// colliding with remotes
		
	hit=FALSE;
	
	for (n=0;n!=max_obj_list;n++) {
		check_obj=server.obj_list.objs[n];
		if (check_obj==NULL) continue;
	
		if ((check_obj->type!=object_type_player) && (check_obj->type!=object_type_remote) && (check_obj->type!=object_type_bot_multiplayer)) continue;
		if ((check_obj->hidden) || (!check_obj->contact.object_on)) continue;
		if (obj->idx==check_obj->idx) continue;
		
		if (collide_object_to_object(obj,NULL,check_obj,TRUE)) {
		
				// if we are only checking, just return TRUE on
				// first hit
				
			if (check_only) return(TRUE);
		
				// telefrag
				
			object_telefrag(check_obj,obj);

				// send network message to telefrag

			if (net_setup.mode!=net_mode_none) {
				if ((check_obj->idx==server.player_obj_idx) || (check_obj->type==object_type_bot_multiplayer)) {
					check_obj->damage_obj_idx=obj->idx;
					net_client_send_death(check_obj,TRUE);
				}
			}
			
			hit=TRUE;
		}
	}
	
	return(hit);
}

/* =======================================================

      Object Touch
      
======================================================= */

void object_setup_touch(obj_type *obj,obj_type *source_obj,bool stand)
{
	obj_touch		*touch;
	
	touch=&obj->touch;
	
	touch->obj_idx=source_obj->idx;

	touch->stand=stand;
	
	touch->pnt.x=(obj->pnt.x+source_obj->pnt.x)>>1;
	touch->pnt.y=(obj->pnt.y+source_obj->pnt.y)>>1;
	touch->pnt.z=(obj->pnt.z+source_obj->pnt.z)>>1;
	
	touch->ang.x=angle_find(source_obj->pnt.y,source_obj->pnt.z,obj->pnt.y,obj->pnt.z);
	touch->ang.y=angle_find(source_obj->pnt.x,source_obj->pnt.z,obj->pnt.x,obj->pnt.z);
	touch->ang.z=angle_find(source_obj->pnt.x,source_obj->pnt.y,obj->pnt.x,obj->pnt.y);
}

void object_touch(obj_type *obj)
{
	int				idx,tick;
	bool			stand;
	obj_type		*hit_obj;

		// get the standing or touching object
		// touching always overrides standing
	
	idx=obj->contact.obj_idx;
	if (idx==-1) idx=obj->contact.stand_obj_idx;

		// if we are touching nothing, only reset
		// the last touch if we are over a threshold wait
		// this is because pushing things can sometimes
		// move them ahead of the player so you aren't
		// constantly touching them

	tick=game_time_get();

	if (idx==-1) {
		if (obj->touch.last_obj_idx!=-1) {
			if ((obj->touch.last_touch_tick+touch_retouch_msec_wait)<tick) obj->touch.last_obj_idx=-1;
		}
		return;
	}

		// get touch object

	hit_obj=server.obj_list.objs[idx];
	if (hit_obj->type==object_type_remote) return;

	stand=(idx==obj->contact.stand_obj_idx);

		// last touch is reset

	obj->touch.last_touch_tick=tick;
	
		// is this a change hit, i.e., a new object
		// or change in stand/hit boolean?
		
	if ((idx==obj->touch.last_obj_idx) && (stand==obj->touch.last_stand)) return;
	
	obj->touch.last_obj_idx=idx;
	obj->touch.last_stand=stand;
		
		// send events

	object_setup_touch(hit_obj,obj,stand);
	scripts_post_event_console(&hit_obj->attach,sd_event_touch,0,0);

	object_setup_touch(obj,hit_obj,stand);
	scripts_post_event_console(&obj->attach,sd_event_touch,0,0);
}

/* =======================================================

      Setup Object Hit
      
======================================================= */

void object_setup_hit(obj_type *obj,obj_type *from_obj,weapon_type *from_weap,proj_type *from_proj,d3pnt *melee_hit_pt,int damage)
{
	int				x,z,y,hit_box_idx;
	obj_hit			*hit;
	model_type		*mdl;

	hit=&obj->hit;

		// setup hit object, weapon, projectile
		
	hit->obj_idx=-1;
	hit->weap_idx=-1;
	hit->proj_setup_idx=-1;
	
    if (from_obj!=NULL) hit->obj_idx=from_obj->idx;
	if (from_weap!=NULL) hit->weap_idx=from_weap->idx;
	if (from_proj!=NULL) hit->proj_setup_idx=from_proj->proj_setup_idx;
	
		// damage
		
	hit->damage=damage;
	    
		// hit x,z,y from object or projectile
		
	if (from_proj==NULL) {
		if (from_obj!=NULL) {
			if (melee_hit_pt!=NULL) {
				x=melee_hit_pt->x;
				y=melee_hit_pt->y;
				z=melee_hit_pt->z;
			}
			else {
				x=(from_obj->pnt.x+obj->pnt.x)>>1;
				z=(from_obj->pnt.z+obj->pnt.z)>>1;
				y=(from_obj->pnt.y+obj->pnt.y)>>1;
			}
		}
		else {
			x=obj->pnt.x;
			z=obj->pnt.z;
			y=obj->pnt.y;
		}
	}
	else {
		x=from_proj->pnt.x;
		z=from_proj->pnt.z;
		y=from_proj->pnt.y;
	}
	
	hit->pnt.x=x;
	hit->pnt.y=y;
	hit->pnt.z=z;
		
	hit->ang.x=angle_find(obj->pnt.y,obj->pnt.z,y,z);
	hit->ang.y=angle_find(obj->pnt.x,obj->pnt.z,x,z);
	hit->ang.z=angle_find(obj->pnt.x,obj->pnt.y,x,y);
	
		// find the hit boxes
		
	hit->hit_box_name[0]=0x0;

	if ((obj->hit_box.on) && (obj->draw.model_idx!=-1)) {
		hit_box_idx=obj->hit_box.obj_hit_box_idx;
		if (hit_box_idx==-1) hit_box_idx=obj->hit_box.proj_hit_box_idx;
		
		mdl=server.model_list.models[obj->draw.model_idx];
		if ((hit_box_idx>=0) && (hit_box_idx<mdl->nhit_box)) strcpy(hit->hit_box_name,mdl->hit_boxes[hit_box_idx].name);
	}
}

/* =======================================================

      Setup Click Hit
      
======================================================= */

void object_click(obj_type *obj,obj_type *from_obj)
{
	obj_click		*click;

		// setup click structure
		
	click=&obj->click;
	click->current_click_obj_idx=from_obj->idx;
	
		// post the event
		
	scripts_post_event_console(&obj->attach,sd_event_click,0,0);

		// and any network events

	if (net_setup.mode!=net_mode_none) {
		if ((from_obj->idx==server.player_obj_idx) || (from_obj->type==object_type_bot_multiplayer)) net_client_send_click(from_obj,&from_obj->pnt,&from_obj->ang);
	}
}

/* =======================================================

      Damage Object
      
======================================================= */

void object_damage(obj_type *obj,obj_type *source_obj,weapon_type *source_weap,proj_type *source_proj,d3pnt *melee_hit_pt,int damage)
{
	if (obj->type==object_type_remote) return;
	if (!obj->damage.on) return;
	
		// need to make sure cascading scripts
		// don't re-enter damage calls
		
	if (obj->damage.in_damage) return;
	
		// reduce any damage

	damage=(int)(((float)damage)*obj->status.health_factor);
	
        // health
		
	if (!obj->damage.invincible) {
		if (obj->status.health<=0) return;
	
			// setup damage object
			
		obj->damage_obj_idx=-1;
		
		if (source_obj!=NULL) {
			if ((source_obj->type!=object_type_object) && (source_obj->idx!=obj->idx)) {		// no damage from regular objects and same object
				obj->damage_obj_idx=source_obj->idx;
			}

		}
	
			// remove the health
			
		obj->status.health-=damage;
		if (obj->status.health<=0) {
			obj->status.health=0;
			obj->death_trigger=TRUE;
			return;
		}
	}
	
		// setup callback
		
	object_setup_hit(obj,source_obj,source_weap,source_proj,melee_hit_pt,damage);

		// run callback

	obj->damage.in_damage=TRUE;
	
	scripts_post_event_console(&obj->attach,sd_event_damage,0,0);
	
	obj->damage.in_damage=FALSE;

		// any watches

	object_watch_damage_alert(&obj->pnt,obj->idx);
}

/* =======================================================

      Melee Hits
      
======================================================= */

void object_melee_hit(obj_type *obj,obj_type *melee_obj,weapon_type *melee_weap,melee_type *melee,d3pnt *sphere_pnt)
{
	int			damage,dist;

		// get damage

	damage=melee->damage;

	if (melee->fall_off) {
		dist=distance_get(sphere_pnt->x,sphere_pnt->y,sphere_pnt->z,obj->pnt.x,obj->pnt.y,obj->pnt.z);
		dist-=obj->size.radius;

		if (dist>0) damage=damage-((damage*dist)/melee->radius);
		if (damage<1) damage=1;
	}
	
		// run damage
        
	if (damage>=0) {
		object_damage(obj,melee_obj,melee_weap,NULL,sphere_pnt,damage);
	}
	else {
		object_heal(obj,-damage);
	}

		// events

	if (melee_weap==NULL) {
		scripts_post_event_console(&melee_obj->attach,sd_event_melee,sd_event_melee_hit,0);
	}
	else {
		scripts_post_event_console(&melee_weap->attach,sd_event_melee,sd_event_melee_hit,0);
	}

	scripts_post_event_console(&obj->attach,sd_event_melee,sd_event_melee_hit,0);
}

/* =======================================================

      Health Recover and Heal Object
      
======================================================= */

void object_heal(obj_type *obj,int heal)
{
	obj_status		*status;
    
    status=&obj->status;
	
    status->health+=heal;
    if (status->health>status->max_health) status->health=status->max_health;
	
		// alert object health has been added
		
	scripts_post_event_console(&obj->attach,sd_event_health,0,0);
}

void object_health_recover(obj_type *obj)
{
	int			tick;

	if (obj->status.health==0) return;
	if ((obj->status.health_recover_tick==0) || (obj->status.health_recover_amount==0)) return;

	obj->status.health_recover_count+=10;

	tick=obj->status.health_recover_tick*100;
	if (obj->status.health_recover_count<tick) return;

	obj->status.health_recover_count-=tick;

	object_heal(obj,obj->status.health_recover_amount);
}

/* =======================================================

      Object Crushing
      
======================================================= */

void object_crush(obj_type *obj,bool auto_crush)
{
		// no damage, crush, or health
		
	if ((!obj->damage.on) || (!obj->damage.crushable)) return;
	if (obj->status.health<=0) return;
	
		// moving walls automatically crush you
		// and call this routine.  The only crushing
		// that will be needed to be check
		// regularly is crushing from above
		
	if (!auto_crush) if (!map_stand_crush_object(obj)) return;
	
		// send events
		
	scripts_post_event_console(&obj->attach,sd_event_crush,0,0);
	
		// kill if not a remote or invincible
		
	if (obj->type==object_type_remote) return;
	if (obj->damage.invincible) return;

	obj->damage_obj_idx=-1;
	obj->status.health=0;
	obj->death_trigger=TRUE;
}

/* =======================================================

      Object Mesh Harm
      
======================================================= */

void object_mesh_harm(obj_type *obj)
{
	int						mesh_idx;
	map_mesh_type			*mesh;
	
		// no damage if no health
		
	if (!obj->damage.on) return;
	if (obj->status.health<=0) return;
	
		// check mesh harm
		
	mesh_idx=obj->contact.stand_poly.mesh_idx;
	if (mesh_idx==-1) mesh_idx=obj->contact.hit_poly.mesh_idx;
	
	if (mesh_idx==-1) {
		obj->status.mesh_harm_count=0;
		return;
	}
	
	mesh=&map.mesh.meshes[mesh_idx];
	if (mesh->harm==0) return;
	
		// run the harm
		
	if (obj->status.mesh_harm_count==0) {
		obj->status.mesh_harm_count=100;

		if (mesh->harm>0) {
			object_damage(obj,NULL,NULL,NULL,NULL,mesh->harm);
		}
		else {
			object_heal(obj,-mesh->harm);
		}
		
		return;
	}

	obj->status.mesh_harm_count--;
}

/* =======================================================

      Hide or Freeze State Changes
      
======================================================= */

void object_hide(obj_type *obj,bool hide)
{
	if (hide==obj->hidden) return;
	
	obj->hidden=hide;
	
	object_turn_stop(obj);
	object_player_look_stop(obj);
	object_move_stop(obj);
}

void object_input_freeze(obj_type *obj,bool freeze)
{
	if (freeze==obj->input_freeze) return;
	
	obj->input_freeze=freeze;
	
	object_turn_stop(obj);
	object_player_look_stop(obj);
	object_move_stop(obj);
}

/* =======================================================

      Push and Shove Objects
      
======================================================= */

void object_push(obj_type *obj,d3ang *ang,int force,bool external_force)
{
	int				weight;
	float			speed,xmove,zmove,ymove,temp_z;
	
		// can't push hidden, suspended, or no-contact objects
	
	if ((obj->hidden) || (obj->suspend)) return;
	if (external_force) {
		if (!obj->contact.force_on) return;
	}
	else {
		if (!obj->contact.object_on) return;
	}
	
		// push power
	
	weight=obj->size.weight;
	if (weight<100) weight=100;
	if (weight>1000) weight=1000;
	
	speed=(float)((force*500)/weight)*(2-map.settings.resistance);
	
		// push vector
		
	angle_get_movement_float(ang->y,speed,&xmove,&zmove);
	angle_get_movement_float(ang->x,speed,&ymove,&temp_z);
	
	obj->force.vct.x+=xmove;
	obj->force.vct.z+=zmove;
	obj->force.vct.y+=ymove;
	
		// reset gravity
		
	obj->force.gravity=gravity_start_power;
}

void object_shove(obj_type *obj,d3ang *ang,float speed)
{
	float			xmove,zmove,ymove,temp_z;
	
		// can't push hidden, suspended, or no-contact objects
	
	if ((obj->hidden) || (obj->suspend) || (!obj->contact.force_on)) return;
	
		// shove vector
		
	angle_get_movement_float(ang->y,speed,&xmove,&zmove);
	angle_get_movement_float(ang->x,speed,&ymove,&temp_z);
	
	if ((obj->force.vct.x*xmove)<0) obj->force.vct.x=0;
	obj->force.vct.x+=xmove;
	
	if ((obj->force.vct.z*zmove)<0) obj->force.vct.z=0;
	obj->force.vct.z+=zmove;
	
	if ((obj->force.vct.y*ymove)<0) obj->force.vct.y=0;
	obj->force.vct.y+=ymove;
	
		// reset gravity
		
	obj->force.gravity=gravity_start_power;
}

void object_shove_direct(obj_type *obj,d3vct *vct)
{
		// can't push hidden, suspended, or no-contact objects
	
	if ((obj->hidden) || (obj->suspend) || (!obj->contact.force_on)) return;
	
		// shove vector
		
	if ((obj->force.vct.x*vct->x)<0) obj->force.vct.x=0;
	obj->force.vct.x+=vct->x;
	
	if ((obj->force.vct.z*vct->z)<0) obj->force.vct.z=0;
	obj->force.vct.z+=vct->z;
	
	if ((obj->force.vct.y*vct->y)<0) obj->force.vct.y=0;
	obj->force.vct.y+=vct->y;
	
		// reset gravity
		
	obj->force.gravity=gravity_start_power;
}

/* =======================================================

      Object Target
      
======================================================= */

bool object_is_targetted(obj_type *obj,d3col *col)
{
	int				n;
	obj_type		*player_obj;
	weapon_type		*weap;

		// look for any targetting on player's weapons

	player_obj=server.obj_list.objs[server.player_obj_idx];

	for (n=0;n!=max_weap_list;n++) {
		weap=player_obj->weap_list.weaps[n];
		if (weap==NULL) continue;
		
		if ((weap->target.on) && (weap->target.obj_idx==obj->idx)) {
			memmove(col,&weap->target.col,sizeof(d3col));
			return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Object Set Radar Icon
      
======================================================= */

bool object_set_radar_icon(obj_type *obj,char *err_str)
{
	int				n,nicon;
	
	obj->radar.icon_idx=-1;
	
		// find radar icon
		
	nicon=hud.radar.nicon;
	
	for (n=0;n!=nicon;n++) {
		if (strcasecmp(hud.radar.icons[n].name,obj->radar.icon)==0) {
			obj->radar.icon_idx=n;
			return(TRUE);
		}
	}

		// unknown radar icon

	sprintf(err_str,"No radar icon exists with this name: %s",obj->radar.icon);
	return(FALSE);
}

