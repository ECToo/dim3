/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Host Client Message Handler Threads

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

#ifdef D3_OS_MAC
	#include <ifaddrs.h>
#endif

#include "network.h"

extern int					net_host_player_count;

extern map_type				map;
extern hud_type				hud;
extern network_setup_type	net_setup;

extern int game_time_get(void);
extern void group_moves_synch_with_client(int group_idx,network_reply_group_synch *synch);

/* =======================================================

      Host Client Networking Message Handlers
      
======================================================= */

int net_host_client_handle_local_join(network_request_join *request_join,char *err_str)
{
	int							net_node_uid,
								tint_color_idx,character_idx;
	network_request_object_add	remote_add;

		// join directly to host

	tint_color_idx=htons((short)request_join->tint_color_idx);
	character_idx=htons((short)request_join->character_idx);

	net_node_uid=net_host_player_join(-1,-1,TRUE,request_join->name,tint_color_idx,character_idx);
	if (net_node_uid==-1) return(-1);

		// send all other players on host the new player for remote add
		
	strncpy(remote_add.name,request_join->name,name_str_len);
	remote_add.name[name_str_len-1]=0x0;
	remote_add.team_idx=htons((short)net_team_none);
	remote_add.tint_color_idx=htons((short)request_join->tint_color_idx);
	remote_add.character_idx=htons((short)request_join->character_idx);
	remote_add.score=0;
	remote_add.pnt_x=remote_add.pnt_y=remote_add.pnt_z=0;
	net_host_player_send_message_others(net_node_uid,net_action_request_remote_add,(unsigned char*)&remote_add,sizeof(network_request_object_add));

	return(net_node_uid);
}

void net_host_client_handle_leave(int net_node_uid)
{
		// leave the host
		
	net_host_player_leave(net_node_uid);
	
		// now send all other players on host the remote remove
		
	net_host_player_send_message_others(net_node_uid,net_action_request_remote_remove,NULL,0);
}

void net_host_client_handle_set_team(int net_node_uid,network_request_team *team)
{
	net_host_player_update_team(team);
	net_host_player_send_message_others(net_node_uid,net_action_request_team,(unsigned char*)team,sizeof(network_request_team));
}

void net_host_client_handle_update(int net_node_uid,network_request_remote_update *update)
{
	net_host_player_update(update);
	net_host_player_send_message_others(net_node_uid,net_action_request_remote_update,(unsigned char*)update,sizeof(network_request_remote_update));
}

void net_host_client_handle_group_synch(int sock)
{
	int								n;
	network_reply_group_synch		reply_synch;

		// send a synch for all groups that were
		// ever moved

	for (n=0;n!=map.ngroup;n++) {

		if (map.groups[n].move.was_moved) {
			group_moves_synch_with_client(n,&reply_synch);
			net_send_message(sock,net_action_reply_group_synch,net_remote_uid_host,(unsigned char*)&reply_synch,sizeof(network_reply_group_synch));
		}

	}
}

/* =======================================================

      Host Client Networking Message Thread
      
======================================================= */

int net_host_client_handler_thread(void *arg)
{
	d3socket				sock;
	int						net_node_uid,action,len;
	unsigned char			data[net_max_msg_size];
	net_host_player_type	*player;
	
		// get player from argument
		
	player=(net_host_player_type)arg;
	
		// no network node until join request
		
	net_node_uid=-1;
	
		// wait for messages
		
	while (TRUE) {

			// feed the queue and get messages
		// supergumba -- feed from outside
		if (!net_queue_feed(sock,&player->queue)) {
			if (net_node_uid!=-1) net_host_client_handle_leave(net_node_uid);
			net_node_uid=-1;
			break;
		}
			
		if (!net_queue_check_message(&player->queue,&action,&net_node_uid,data,&len)) {
			usleep(host_no_data_u_wait);
			continue;
		}
		
			// route messages

		switch (action) {
		
			case net_action_request_ready:
				net_host_player_ready(net_node_uid);
				break;
				
			case net_action_request_team:
				net_host_client_handle_set_team(net_node_uid,(network_request_team*)data);
				break;
				
			case net_action_request_leave:
				net_host_client_handle_leave(net_node_uid);
				net_node_uid=-1;
				break;
				
			case net_action_request_remote_update:
				net_host_client_handle_update(net_node_uid,(network_request_remote_update*)data);
				break;
				
			case net_action_request_remote_death:
			case net_action_request_remote_chat:
			case net_action_request_remote_sound:
			case net_action_request_remote_fire:
			case net_action_request_remote_pickup:
			case net_action_request_remote_click:
				net_host_player_send_message_others(net_node_uid,action,data,len);
				break;

			case net_action_request_latency_ping:
				net_send_message(sock,action,net_remote_uid_host,NULL,0);
				break;

			case net_action_request_group_synch:
				net_host_client_handle_group_synch(sock);
				break;

		}
		
			// if no player attached, close socket and exit thread
			
		if (net_node_uid==-1) break;
	}
	
	net_close_socket(&sock);
	
	return(0);
}


