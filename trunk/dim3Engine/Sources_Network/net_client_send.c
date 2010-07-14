/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Send Message to Host

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#include "objects.h"
#include "interfaces.h"
#include "network.h"
#include "timing.h"

extern map_type					map;
extern server_type				server;
extern js_type					js;
extern setup_type				setup;

extern d3socket					client_socket;

extern network_setup_type		net_setup;

/* =======================================================

      Client Message Mainline
      
======================================================= */

void net_client_send_msg(obj_type *obj,int action,unsigned char *msg,int msg_len)
{
		// if we are a client, send all
		// messages to the host

	if (net_setup.mode==net_mode_client) {
		net_sendto_msg(client_socket,net_setup.client.host_ip_addr,net_port_host,action,obj->remote.uid,msg,msg_len);
	}

		// if we are the host, some of the
		// actions we route to other players

	else {
		net_host_player_send_message_others(obj->remote.uid,action,msg,msg_len);
	}
}

/* =======================================================

      Host Leave, Ping Messages
      
======================================================= */

void net_client_send_leave_host(obj_type *obj)
{
	net_client_send_msg(obj,net_action_request_leave,NULL,0);
}

void net_client_send_latency_ping(obj_type *obj)
{
	if (net_setup.mode==net_mode_client) net_client_send_msg(obj,net_action_request_latency_ping,NULL,0);
}	

/* =======================================================

      Host Synching
      
======================================================= */

void net_client_request_group_synch_ping(obj_type *obj)
{
	if (net_setup.mode==net_mode_client) net_client_send_msg(obj,net_action_request_group_synch,NULL,0);
}	

/* =======================================================

      Object Death Messages
      
======================================================= */

void net_client_send_death(obj_type *obj,bool telefrag)
{
	int								send_remote_kill_uid;
	obj_type						*chk_obj;
	network_request_remote_death	death;
	
		// normal deaths

	if (!telefrag) {
		send_remote_kill_uid=-1;
		
		if (obj->damage_obj_uid!=-1) {
			chk_obj=server.obj_list.objs[obj->damage_obj_uid];
			if (chk_obj!=NULL) {
				if ((chk_obj->type==object_type_player) || (chk_obj->type==object_type_remote)) {
					send_remote_kill_uid=chk_obj->remote.uid;
				}
			}
		}
		
		death.remote_killer_obj_uid=htons((short)send_remote_kill_uid);
		death.telefrag=htons(0);
	}

		// telefrag deaths

	else {
		chk_obj=server.obj_list.objs[obj->damage_obj_uid];		// only remote objects can telefrag each other, so no other checks necessary
		
		death.remote_killer_obj_uid=htons((short)chk_obj->remote.uid);
		death.telefrag=htons(1);
	}

	net_client_send_msg(obj,net_action_request_remote_death,(unsigned char*)&death,sizeof(network_request_remote_death));
}

/* =======================================================

      Object Update Messages
      
======================================================= */

void net_client_send_remote_update(obj_type *obj,bool chat_on)
{
	int								n,tick,flags;
	model_draw						*draw;
	model_draw_animation			*animation;
	model_draw_dynamic_bone			*dyn_bone;
	network_request_animation		*net_animation;
	network_request_dynamic_bone	*net_dyn_bone;
	network_request_remote_update	update;
	
		// no updates if pipe full
		
	if (net_setup.mode==net_mode_client) {
		if (!net_send_ready(client_socket)) return;
	}

	draw=&obj->draw;

		// create flags
		
	flags=0;
	
	if (obj->hidden) flags|=net_update_flag_hidden;
	if (!obj->contact.object_on) flags|=net_update_flag_no_contact_object;
	if (!obj->contact.projectile_on) flags|=net_update_flag_no_contact_object;
	if (!obj->contact.force_on) flags|=net_update_flag_no_contact_object;
	if (chat_on) flags|=net_update_flag_talking;
	
	update.flags=htonl(flags);
	
		// status

	update.score=htons((short)obj->score.score);
	update.health=htons((short)obj->status.health);
	
		// position
		
	update.pnt_x=htonl(obj->pnt.x);
	update.pnt_y=htonl(obj->pnt.y);
	update.pnt_z=htonl(obj->pnt.z);
	
	update.fp_ang_x=htonf(obj->ang.x);
	update.fp_ang_y=htonf(obj->ang.y);
	update.fp_ang_z=htonf(obj->ang.z);

	update.offset_x=htons((short)draw->offset.x);
	update.offset_y=htons((short)draw->offset.y);
	update.offset_z=htons((short)draw->offset.z);

	update.fp_predict_move_x=htonl(obj->pnt.x-obj->last_pnt.x);
	update.fp_predict_move_y=htonl(obj->pnt.y-obj->last_pnt.y);
	update.fp_predict_move_z=htonl(obj->pnt.z-obj->last_pnt.z);
	
	update.fp_predict_turn_y=htonf(obj->last_ang.y-obj->ang.y);
	
		// model animations

	animation=draw->animations;
	net_animation=update.animation;

	tick=game_time_get();
	
	for (n=0;n!=max_model_blend_animation;n++) {
		net_animation->model_tick=htonl(animation->tick-tick);
		net_animation->model_mode=htons((short)animation->mode);
		net_animation->model_animate_idx=htons((short)animation->animate_idx);
		net_animation->model_animate_next_idx=htons((short)animation->animate_next_idx);
		net_animation->model_pose_move_idx=htons((short)animation->pose_move_idx);
		net_animation->model_smooth_animate_idx=htons((short)animation->smooth_animate_idx);
		net_animation->model_smooth_pose_move_idx=htons((short)animation->smooth_pose_move_idx);

		animation++;
		net_animation++;
	}
	
	update.model_mesh_mask=htonl(draw->mesh_mask);
	memmove(update.model_cur_texture_frame,draw->cur_texture_frame,max_model_texture);

		// dynamic bones

	dyn_bone=draw->dynamic_bones;
	net_dyn_bone=update.dynamic_bones;

	for (n=0;n!=max_model_dynamic_bone;n++) {

		net_dyn_bone->bone_idx=htons((short)dyn_bone->bone_idx);

		if (dyn_bone->bone_idx!=-1) {
			net_dyn_bone->fp_mov_x=htonf(dyn_bone->mov.x);
			net_dyn_bone->fp_mov_y=htonf(dyn_bone->mov.y);
			net_dyn_bone->fp_mov_z=htonf(dyn_bone->mov.z);
			net_dyn_bone->fp_rot_x=htonf(dyn_bone->rot.x);
			net_dyn_bone->fp_rot_y=htonf(dyn_bone->rot.y);
			net_dyn_bone->fp_rot_z=htonf(dyn_bone->rot.z);
			net_dyn_bone->fp_resize=htonf(dyn_bone->resize);
		}

		dyn_bone++;
		net_dyn_bone++;
	}

		// send update
		
	net_client_send_msg(obj,net_action_request_remote_update,(unsigned char*)&update,sizeof(network_request_remote_update));
}

/* =======================================================

      Chats and Sound Messages
	        
======================================================= */

void net_client_send_chat(obj_type *obj,char *str)
{
	network_request_remote_chat		chat;
	
	strcpy(chat.str,str);

	net_client_send_msg(obj,net_action_request_remote_chat,(unsigned char*)&chat,sizeof(network_request_remote_chat));
}

void net_client_send_sound(obj_type *obj,d3pnt *pnt,float pitch,char *name)
{
	network_request_remote_sound	sound;

	sound.pnt_x=htonl(pnt->x);
	sound.pnt_y=htonl(pnt->y);
	sound.pnt_z=htonl(pnt->z);
	
	sound.fp_pitch=htonf(pitch);
	
	strcpy(sound.name,name);

	net_client_send_msg(obj,net_action_request_remote_sound,(unsigned char*)&sound,sizeof(network_request_remote_sound));
}

/* =======================================================

      Weapon Messages
	        
======================================================= */

void net_client_send_projectile_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_type=htons(net_remote_fire_type_projectile);

	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	strcpy(fire.proj_setup_name,proj_setup_name);

	fire.radius=0;
	fire.distance=0;
	fire.damage=0;
	fire.force=0;

	net_client_send_msg(obj,net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
}

void net_client_send_hitscan_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_type=htons(net_remote_fire_type_hit_scan);
	
	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	strcpy(fire.proj_setup_name,proj_setup_name);

	fire.radius=0;
	fire.distance=0;
	fire.damage=0;
	fire.force=0;

	net_client_send_msg(obj,net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
}

void net_client_send_melee_add(obj_type *obj,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_type=htons(net_remote_fire_type_melee);
	
	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	fire.proj_setup_name[0]=0x0;
	
	fire.radius=htons((short)radius);
	fire.distance=htons((short)distance);
	fire.damage=htons((short)damage);
	fire.force=htons((short)force);

	net_client_send_msg(obj,net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
}

/* =======================================================

      Pickup Messages
	        
======================================================= */

void net_client_setup_pickup(obj_type *obj,network_request_remote_pickup *pickup)
{
	int									n,idx;
	weapon_type							*weap;
	
		// make sure pickup can work by
		// moving over settings pickups are concerned with

	pickup->pt_x=htonl(obj->pnt.x);
	pickup->pt_y=htonl(obj->pnt.y);
	pickup->pt_z=htonl(obj->pnt.z);

	pickup->health=htons((short)obj->status.health);

	idx=0;
		
	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;

		pickup->ammos[idx].hidden=htons((short)(weap->hidden?0:1));
		pickup->ammos[idx].ammo_count=htons((short)weap->ammo.count);
		pickup->ammos[idx].clip_count=htons((short)weap->ammo.clip_count);
		pickup->ammos[idx].alt_ammo_count=htons((short)weap->alt_ammo.count);
		pickup->ammos[idx].alt_clip_count=htons((short)weap->alt_ammo.clip_count);

		idx++;
		if (idx==net_max_weapon_per_remote) break;
	}
}

void net_client_send_pickup(obj_type *obj,network_request_remote_pickup *pickup)
{
	net_client_send_msg(obj,net_action_request_remote_pickup,(unsigned char*)pickup,sizeof(network_request_remote_pickup));
}

/* =======================================================

      Click Messages
	        
======================================================= */

void net_client_send_click(obj_type *obj,d3pnt *pt,d3ang *ang)
{
	network_request_remote_click		click;
	
	click.pt_x=htonl(pt->x);
	click.pt_y=htonl(pt->y);
	click.pt_z=htonl(pt->z);
	
	click.fp_ang_x=htonf(ang->x);
	click.fp_ang_y=htonf(ang->y);
	click.fp_ang_z=htonf(ang->z);
	
	net_client_send_msg(obj,net_action_request_remote_click,(unsigned char*)&click,sizeof(network_request_remote_click));
}