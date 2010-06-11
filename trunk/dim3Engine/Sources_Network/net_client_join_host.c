/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Join Host

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

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;

extern bool					join_thread_quit;
extern d3socket				client_socket;

network_setup_type			net_setup;

/* =======================================================

      Find Game From Name
      
======================================================= */

int net_client_find_game(char *game_name)
{
	int			n;

	for (n=0;n!=hud.net_game.ngame;n++) {
		if (strcasecmp(hud.net_game.games[n].name,game_name)==0) return(n);
	}

	return(-1);
}

/* =======================================================

      Join and Exit Host
      
======================================================= */

int net_client_join_host_start(obj_type *obj,int *tick_offset,char *deny_reason,network_reply_join_remote_list *remote_list)
{
	int						action,wait_tick,remote_uid;
	unsigned char			msg[net_max_msg_size];
	char					err_str[256];
	bool					reply_ok;
	network_request_join	request_join;
	network_reply_join		reply_join;
	
		// create client socket
		
	client_socket=net_open_udp_socket();
	if (client_socket==D3_NULL_SOCKET) {
		strcpy(err_str,"Networking: Unable to open socket");
		return(-1);
	}
	
	net_socket_blocking(client_socket,FALSE);

		// send join request
		
	strcpy(request_join.vers,dim3_version);
	request_join.hash=ntohl(net_get_project_hash());

	strcpy(request_join.name,obj->name);
	strcpy(request_join.draw_name,obj->draw.name);
	request_join.tint_color_idx=(signed short)ntohs((short)obj->tint_color_idx);

	net_sendto_msg(client_socket,net_setup.client.host_ip_addr,net_port_host,net_action_request_join,net_player_uid_none,(unsigned char*)&request_join,sizeof(network_request_join));

		// wait for the reply
		
	reply_ok=TRUE;
	action=-1;
	
	wait_tick=time_get()+(client_timeout_wait_seconds*1000);

	while (wait_tick>time_get()) {
		reply_ok=net_recvfrom_mesage(client_socket,NULL,NULL,&action,&remote_uid,msg,NULL);
		if ((!reply_ok) || (action!=-1)) break;
		
		usleep(100000);
	}
	
		// various failures
		
	if (action==-1) {
		strcpy(deny_reason,"Host Not Responding to Join Request");
		net_close_socket(&client_socket);
		return(-1);
	}

	if (!reply_ok) {
		strcpy(deny_reason,"Connection to Host Broke");
		net_close_socket(&client_socket);
		return(-1);
	}

		// right message?

	if (action!=net_action_reply_join) {
		strcpy(deny_reason,"Host Sent Bad Reply");
		net_close_socket(&client_socket);
		return(-1);
	}

	memmove(&reply_join,msg,sizeof(network_reply_join));
	
		// check for denial
		
	if (reply_join.deny_reason[0]!=0x0) {
		strcpy(deny_reason,reply_join.deny_reason);
		net_close_socket(&client_socket);
		return(-1);
	}
	
		// finish setup
		
	obj->remote.uid=(signed short)ntohs(reply_join.remote_uid);
	obj->team_idx=(signed short)ntohs(reply_join.team_idx);
	
	*tick_offset=ntohl(reply_join.map_tick);

	memmove(remote_list,&reply_join.remote_list,sizeof(network_reply_join_remote_list));

		// setup client joined flags
		
	net_setup.mode=net_mode_client;
	net_setup.uid.machine_uid=(signed short)ntohs(reply_join.machine_uid);
	net_setup.client.latency=0;

	return(obj->remote.uid);
}

void net_client_join_host_end(void)
{
	net_close_socket(&client_socket);
	
		// turn off client joined flags
		
	net_setup.mode=net_mode_none;
}