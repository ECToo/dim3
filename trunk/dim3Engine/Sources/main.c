/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Line

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

#ifdef D3_OS_MAC
	#include <mach-o/arch.h>
#endif

#include "xmls.h"

int							os_vers_major,os_vers_minor_1,os_vers_minor_2;
long						os_vers_hex;
char						arch_type[64];
bool						arch_is_ppc;

bool						game_app_active,game_loop_quit;

extern setup_type			setup;

extern bool app_start(char *err_str);
extern void app_end(void);
extern bool loop_main(char *err_str);

/* =======================================================

      OS Support and Setup
      
======================================================= */

bool app_check_os_support(char *err_str)
{
#ifdef D3_OS_MAC
	
	NXArchInfo		*info;

		// system version

	Gestalt(gestaltSystemVersion,&os_vers_hex);
	
	os_vers_major=(((os_vers_hex&0xF000)>>12)*10)+((os_vers_hex&0x0F00)>>8);
	os_vers_minor_1=(os_vers_hex&0x00F0)>>4;
	os_vers_minor_2=os_vers_hex&0x000F;

	if (os_vers_hex<0x1039) {
		strcpy(err_str,"Unsupported System Version: You need at least MacOS X 10.3.9 to run dim3.");
		return(FALSE);
	}

		// arch type

	info=(NXArchInfo*)NXGetLocalArchInfo();
	strncpy(arch_type,info->name,name_str_len);
	arch_type[name_str_len-1]=0x0;
	
	arch_is_ppc=(info->cputype==CPU_TYPE_POWERPC);

#else
	os_vers_hex=0;
	os_vers_major=0;
	os_vers_minor_1=0;
	os_vers_minor_2=0;

	strcpy(arch_type,"x86");
	arch_is_ppc=FALSE;
#endif

	return(TRUE);
}

/* =======================================================

      Check for Editor Link Files
      
======================================================= */

#ifdef D3_OS_MAC	

void app_check_editor_link(void)
{
	int				len;
	char			path[1024];
	unsigned char	uc_len;
	FILE			*file;
	
	setup.editor_override.on=FALSE;

		// attempt to open editor link file
		
//	file_paths_base(&setup.file_path_setup,path,"editor_link","tmp");
	file_paths_preferences(path,"dim3 Editor Link","tmp");
	
	file=fopen(path,"rb");
	if (file==NULL) return;
	
		// name

	fread(&uc_len,1,1,file);
	len=(int)uc_len;
	fread(setup.editor_override.map,1,len,file);

		// position

	fread(&setup.editor_override.pt,1,sizeof(d3pnt),file);
	fread(&setup.editor_override.ang,1,sizeof(d3ang),file);

	fclose(file);
	
		// get rid of the file
		
	remove(path);

		// mark editor override

	setup.editor_override.on=TRUE;
}

#else

void app_check_editor_link(void)
{
	setup.editor_override.on=FALSE;
}

#endif

/* =======================================================

      Attempt to fix XML on Startup Error
      
======================================================= */

void app_report_error(char *err_str)
{
#ifdef D3_OS_MAC	
	CFStringRef			cf_str;
	CFOptionFlags		resp;
	
	SetThemeCursor(kThemeArrowCursor);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,err_str,kCFStringEncodingMacRoman);
	CFUserNotificationDisplayAlert(0,kCFUserNotificationStopAlertLevel,NULL,NULL,NULL,CFSTR("dim3 Error"),cf_str,NULL,NULL,NULL,&resp);
	CFRelease(cf_str);
#endif

#ifdef D3_OS_LINUX
    fprintf(stderr,"dim3 Error: %s\n", err_str);
#endif

#ifdef D3_OS_WINDOWS
	MessageBox(NULL,err_str,"dim3 Error",MB_OK);
#endif
}

/* =======================================================

      Infamous Main Code
      
======================================================= */

int main(int argc,char *argv[])
{
	bool			app_start_ok;
	char			err_str[256];
	
		// setup paths

	if (!file_paths_setup(&setup.file_path_setup)) {
		app_report_error("dim3 requires a data folder with project files.");
		return(0);
	}
	
		// get the project name
		
	read_settings_interface_project_name();
	
		// check for required OS
		
	if (!app_check_os_support(err_str)) {
		app_report_error(err_str);
		return(0);
	}

		// check if editor is launching engine and
		// if a map needs to be auto-loaded
		
	app_check_editor_link();

		// run dim3
		
	err_str[0]=0x0;
	
	app_start_ok=app_start(err_str);
	if (app_start_ok) {

		game_app_active=TRUE;
		game_loop_quit=FALSE;

		while (!game_loop_quit) {
			if (!loop_main(err_str)) break;
		}

		app_end();
	}
	
		// report any errors
		
	if (err_str[0]!=0x0) app_report_error(err_str);
	
	return(0);
}
