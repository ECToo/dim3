/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Routines

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

#include "lights.h"
#include "cameras.h"
#include "consoles.h"
#include "video.h"

extern int game_time_get(void);
extern float game_time_fequency_second_get(int start_tick);

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							gl_shader_current_txt_idx,gl_shader_current_frame,
							gl_shader_current_lmap_txt_idx;
shader_type					*gl_shader_current;

extern int					nuser_shader;
extern shader_type			user_shaders[max_user_shader];

extern float				light_shader_direction[7][3];

/* =======================================================

      Setup Shader Variables
      
======================================================= */

void gl_shader_set_instance_variables(shader_type *shader)
{
	int						n;
	GLint					var;
	shader_custom_var_type	*cvar;
	
		// need to use program before calling these
		
	glUseProgramObjectARB(shader->program_obj);

		// texture pointers
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3Tex");
	if (var!=-1) glUniform1iARB(var,0);

	var=glGetUniformLocationARB(shader->program_obj,"dim3TexBump");
	if (var!=-1) glUniform1iARB(var,1);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1iARB(var,2);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexLightMap");
	if (var!=-1) glUniform1iARB(var,3);

		// custom variables
		
	for (n=0;n!=shader->custom_var_list.nvar;n++) {
		cvar=&shader->custom_var_list.vars[n];
		
		var=glGetUniformLocationARB(shader->program_obj,cvar->name);
		if (var==-1) continue;
		
		switch (cvar->var_type) {
		
			case shader_var_type_int:
				glUniform1iARB(var,cvar->value.i);
				break;
				
			case shader_var_type_float:
				glUniform1fARB(var,cvar->value.f);
				break;
				
			case shader_var_type_vec3:
				glUniform3fARB(var,cvar->value.vec3[0],cvar->value.vec3[1],cvar->value.vec3[2]);
				break;
				
			case shader_var_type_vec4:
				glUniform4fARB(var,cvar->value.vec4[0],cvar->value.vec4[1],cvar->value.vec4[2],cvar->value.vec4[3]);
				break;
		}
	}
	
		// cancel program
		
	glUseProgramObjectARB(0);
}

void gl_shader_cache_dynamic_variable_locations(shader_type *shader)
{
	int				n;
	char			var_name[256];
	
	shader->var_locs.dim3TimeMillisec=glGetUniformLocationARB(shader->program_obj,"dim3TimeMillisec");
	shader->var_locs.dim3FrequencySecond=glGetUniformLocationARB(shader->program_obj,"dim3FrequencySecond");
	shader->var_locs.dim3CameraPosition=glGetUniformLocationARB(shader->program_obj,"dim3CameraPosition");
	shader->var_locs.dim3AmbientColor=glGetUniformLocationARB(shader->program_obj,"dim3AmbientColor");
	shader->var_locs.dim3LightMapBoost=glGetUniformLocationARB(shader->program_obj,"dim3LightMapBoost");
	shader->var_locs.dim3ShineFactor=glGetUniformLocationARB(shader->program_obj,"dim3ShineFactor");
	shader->var_locs.dim3TexColor=glGetUniformLocationARB(shader->program_obj,"dim3TexColor");
	shader->var_locs.dim3TintColor=glGetUniformLocationARB(shader->program_obj,"dim3TintColor");
	shader->var_locs.dim3Alpha=glGetUniformLocationARB(shader->program_obj,"dim3Alpha");

	shader->var_locs.dim3Tangent=glGetUniformLocationARB(shader->program_obj,"dim3Tangent");
	shader->var_locs.dim3Binormal=glGetUniformLocationARB(shader->program_obj,"dim3Binormal");
	shader->var_locs.dim3Normal=glGetUniformLocationARB(shader->program_obj,"dim3Normal");

	shader->var_locs.dim3VertexTangent=glGetUniformLocationARB(shader->program_obj,"dim3VertexTangent");
	shader->var_locs.dim3VertexBinormal=glGetUniformLocationARB(shader->program_obj,"dim3VertexBinormal");
	shader->var_locs.dim3VertexNormal=glGetUniformLocationARB(shader->program_obj,"dim3VertexNormal");
	
	for (n=0;n!=max_shader_light;n++) {
		sprintf(var_name,"dim3Light_%d.position",n);
		shader->var_locs.dim3Lights[n].position=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.color",n);
		shader->var_locs.dim3Lights[n].color=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.intensity",n);
		shader->var_locs.dim3Lights[n].intensity=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.exponent",n);
		shader->var_locs.dim3Lights[n].exponent=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.direction",n);
		shader->var_locs.dim3Lights[n].direction=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.inLightMap",n);
		shader->var_locs.dim3Lights[n].inLightMap=glGetUniformLocationARB(shader->program_obj,var_name);
	}
}

/* =======================================================

      Shader Error Reporter
      
======================================================= */

bool gl_shader_report_error(char *err_str,char *vertex_name,char *fragment_name,GLhandleARB hand,char *err_type,char *code,int check_type)
{
	int				line;
	GLint			result,len;
	char			*c,*str,log_line[256];
	FILE			*file;
	struct tm		*tm;
	time_t			curtime;
	
		// operation OK?
		
	glGetObjectParameterivARB(hand,check_type,&result);
	if (result==1) return(TRUE);
	
		// start or append log file
		
	file=fopen("glsl_error_log.txt","a");
	if (file==NULL) {
		strcpy(err_str,"GLSL Error: Could not write log file");
		return(FALSE);
	}
	
		// header
		
	fwrite("***\n",1,4,file);

	sprintf(log_line,"Error Type: %s\n",err_type);
	fwrite(log_line,1,strlen(log_line),file);
	
	if (vertex_name!=NULL) {
		sprintf(log_line,"Vertex: %s\n",vertex_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	if (fragment_name!=NULL) {
		sprintf(log_line,"Fragment: %s\n",fragment_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	curtime=time(NULL);
	tm=localtime(&curtime);
	
	sprintf(log_line,"Time: %.4d %.2d %.2d %.2d:%.2d.%.2d\n",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	fwrite(log_line,1,strlen(log_line),file);

		// the error
		
	glGetObjectParameterivARB(hand,GL_OBJECT_INFO_LOG_LENGTH_ARB,&len);
	if (len>0) {
		str=malloc(len);
		if (str!=NULL) {
			glGetInfoLogARB(hand,len,NULL,str);

			fwrite(str,1,len,file);
			fwrite("\n",1,1,file);

			free(str);
		}
	}
	
		// the code
		
	if (code!=NULL) {
	
		c=code;
		line=2;
		
		fwrite("001:",1,4,file);
		
		while (*c!=0x0) {
			
			if (*c=='\r') {
				c++;
				continue;
			}
			
			fwrite(c,1,1,file);
			
			if (*c=='\n') {
				sprintf(log_line,"%.3d:",line);
				fwrite(log_line,1,4,file);
				line++;
			}

			c++;
		}
	}
	
	fwrite("\n",1,1,file);
		
		// finish log

	fclose(file);

		// fatal error string

	strcpy(err_str,"GLSL: Could not compile or link code, check out glsl_error_log.txt for more information");
	
	return(FALSE);
}

/* =======================================================

      Compile Shader
      
======================================================= */

void gl_shader_code_clear(shader_type *shader)
{
	shader->vertex_obj=0;
	shader->fragment_obj=0;
	shader->program_obj=0;
}

bool gl_shader_code_compile(shader_type *shader,char *vertex_data,char *fragment_data,char *err_str)
{
	const GLcharARB			*vertex_ptr,*fragment_ptr;

		// create the shader
			
	shader->program_obj=glCreateProgramObjectARB();
		
	shader->vertex_obj=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	
	vertex_ptr=vertex_data;
	glShaderSourceARB(shader->vertex_obj,1,&vertex_ptr,NULL);

	glCompileShaderARB(shader->vertex_obj);
	if (!gl_shader_report_error(err_str,shader->vertex_name,NULL,shader->vertex_obj,"Vertex",vertex_data,GL_OBJECT_COMPILE_STATUS_ARB)) return(FALSE);

	glAttachObjectARB(shader->program_obj,shader->vertex_obj);
	
	shader->fragment_obj=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	fragment_ptr=fragment_data;
	glShaderSourceARB(shader->fragment_obj,1,&fragment_ptr,NULL);
	
	glCompileShaderARB(shader->fragment_obj);
	if (!gl_shader_report_error(err_str,NULL,shader->fragment_name,shader->fragment_obj,"Fragment",fragment_data,GL_OBJECT_COMPILE_STATUS_ARB)) return(FALSE);
	
	glAttachObjectARB(shader->program_obj,shader->fragment_obj);
	
		// link shaders into program
		
	glLinkProgramARB(shader->program_obj);
	if (!gl_shader_report_error(err_str,shader->vertex_name,shader->fragment_name,shader->program_obj,"Program",NULL,GL_OBJECT_LINK_STATUS_ARB)) return(FALSE);

		// per instance shader variables

	gl_shader_set_instance_variables(shader);
	
		// cache the location of the dymaic variables
		
	gl_shader_cache_dynamic_variable_locations(shader);
	
		// timing only works for full screen shaders
		
	shader->start_tick=0;

	return(TRUE);
}

void gl_shader_code_shutdown(shader_type *shader)
{
	if (shader->vertex_obj!=0) {
		glDetachObjectARB(shader->program_obj,shader->vertex_obj);
		glDeleteObjectARB(shader->vertex_obj);
	}

	if (shader->fragment_obj!=0) {
		glDetachObjectARB(shader->program_obj,shader->fragment_obj);
		glDeleteObjectARB(shader->fragment_obj);
	}

	if (shader->program_obj!=0) glDeleteObjectARB(shader->program_obj);
}

/* =======================================================

      Attach Shaders to Textures
      
======================================================= */

void gl_shader_attach_map(void)
{
	int					n;
	bool				shader_on;
	texture_type		*texture;
	
	shader_on=gl_check_shader_ok();

	texture=map.textures;
	
	for (n=0;n!=max_map_texture;n++) {
		texture->shader_idx=-1;
		
		if ((shader_on) && (texture->shader_name[0]!=0x0)) {
			if (strcasecmp(texture->shader_name,"default")==0) {
				texture->shader_idx=gl_shader_core_index;
			}
			else {
				texture->shader_idx=gl_user_shader_find(texture->shader_name);
			}
		}
		
		texture++;
	}
}

void gl_shader_attach_model(model_type *mdl)
{
	int					n;
	bool				shader_on;
	texture_type		*texture;
	
	shader_on=gl_check_shader_ok();

	texture=mdl->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		texture->shader_idx=-1;
		
		if ((shader_on) && (texture->shader_name[0]!=0x0)) {
			if (strcasecmp(texture->shader_name,"default")==0) {
				texture->shader_idx=gl_shader_core_index;
			}
			else {
				texture->shader_idx=gl_user_shader_find(texture->shader_name);
			}
		}
		
		texture++;
	}
}

/* =======================================================

      Shader Variables
      
======================================================= */

void gl_shader_set_scene_variables(shader_type *shader,view_light_list_type *light_list)
{
	if (shader->var_locs.dim3TimeMillisec!=-1) glUniform1iARB(shader->var_locs.dim3TimeMillisec,(game_time_get()-shader->start_tick));
	if (shader->var_locs.dim3FrequencySecond!=-1) glUniform1fARB(shader->var_locs.dim3FrequencySecond,game_time_fequency_second_get(shader->start_tick));
	if (shader->var_locs.dim3CameraPosition!=-1) glUniform3fARB(shader->var_locs.dim3CameraPosition,(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z);

	if (shader->var_locs.dim3AmbientColor!=-1) {
		if (light_list!=NULL) {
			glUniform3fARB(shader->var_locs.dim3AmbientColor,light_list->ambient.r,light_list->ambient.g,light_list->ambient.b);
		}
		else {
			glUniform3fARB(shader->var_locs.dim3AmbientColor,0.0f,0.0f,0.0f);
		}
	}
	
	if (shader->var_locs.dim3LightMapBoost!=-1) glUniform1fARB(shader->var_locs.dim3LightMapBoost,map.ambient.light_map_boost);
}

void gl_shader_set_texture_variables(shader_type *shader,texture_type *texture)
{
	if (shader->var_locs.dim3ShineFactor!=-1) glUniform1fARB(shader->var_locs.dim3ShineFactor,texture->shine_factor);
	if (shader->var_locs.dim3TexColor!=-1) glUniform3fARB(shader->var_locs.dim3TexColor,texture->col.r,texture->col.g,texture->col.b);
}

void gl_shader_set_light_normal_variables(shader_type *shader,view_light_list_type *light_list)
{
	int						n;
	float					fx,fy,fz;
	view_light_spot_type	*lspot;
	
	for (n=0;n!=max_shader_light;n++) {
	
			// intensity of 0 = light off
			
		if (n>=light_list->nlight) {
			if (shader->var_locs.dim3Lights[n].intensity!=-1) glUniform1fARB(shader->var_locs.dim3Lights[n].intensity,0.0f);
		}
		
			// set regular light
			
		else {
			lspot=&view.render->light.spots[light_list->light_idx[n]];
			
			fx=lspot->f_x;
			fy=lspot->f_y;
			fz=lspot->f_z;
			
			gl_project_to_eye_coordinates(&fx,&fy,&fz);		// lights need to be in eye coordinates
			
			if (shader->var_locs.dim3Lights[n].position!=-1) glUniform3fARB(shader->var_locs.dim3Lights[n].position,fx,fy,fz);
			if (shader->var_locs.dim3Lights[n].color!=-1) glUniform3fARB(shader->var_locs.dim3Lights[n].color,lspot->col.r,lspot->col.g,lspot->col.b);
			if (shader->var_locs.dim3Lights[n].intensity!=-1) glUniform1fARB(shader->var_locs.dim3Lights[n].intensity,(float)lspot->intensity);
			if (shader->var_locs.dim3Lights[n].exponent!=-1) glUniform1fARB(shader->var_locs.dim3Lights[n].exponent,lspot->exponent);
			if (shader->var_locs.dim3Lights[n].direction!=-1) glUniform3fvARB(shader->var_locs.dim3Lights[n].direction,3,light_shader_direction[lspot->direction]);
			if (shader->var_locs.dim3Lights[n].inLightMap!=-1) glUniform1iARB(shader->var_locs.dim3Lights[n].inLightMap,(lspot->light_map?0x1:0x0));
		}
	}
}

void gl_shader_set_poly_variables(shader_type *shader,float alpha,d3col *tint_col,tangent_space_type *tangent_space,model_draw_array_type *model_draw_array)
{
		// set tint color
		
	if (shader->var_locs.dim3TintColor!=-1) {
	
		if (tint_col==NULL) {
			if ((shader->cur_tint_col.r!=1.0f) || (shader->cur_tint_col.g!=1.0f) || (shader->cur_tint_col.b!=1.0f)) {
				shader->cur_tint_col.r=shader->cur_tint_col.g=shader->cur_tint_col.b=1.0f;
				glUniform3fARB(shader->var_locs.dim3TintColor,1.0f,1.0f,1.0f);
			}
		}
		else {
			if ((shader->cur_tint_col.r!=tint_col->r) || (shader->cur_tint_col.g!=tint_col->g) || (shader->cur_tint_col.b!=tint_col->b)) {
				memmove(&shader->cur_tint_col,tint_col,sizeof(d3col));
				glUniform3fARB(shader->var_locs.dim3TintColor,tint_col->r,tint_col->g,tint_col->b);
			}
		}
	}
	
		// alpha
		
	if (shader->var_locs.dim3Alpha!=-1) {
		if (shader->cur_alpha!=alpha) {
			shader->cur_alpha=alpha;
			glUniform1fARB(shader->var_locs.dim3Alpha,alpha);
		}
	}

		// tangent spaces

	if (tangent_space!=NULL) {
		if (shader->var_locs.dim3Tangent!=-1) glUniform3fARB(shader->var_locs.dim3Tangent,tangent_space->tangent.x,tangent_space->tangent.y,tangent_space->tangent.z);
		if (shader->var_locs.dim3Binormal!=-1) glUniform3fARB(shader->var_locs.dim3Binormal,tangent_space->binormal.x,tangent_space->binormal.y,tangent_space->binormal.z);
		if (shader->var_locs.dim3Normal!=-1) glUniform3fARB(shader->var_locs.dim3Normal,tangent_space->normal.x,tangent_space->normal.y,tangent_space->normal.z);
	}

	if (model_draw_array!=NULL) {
		if (!shader->normal_vertex_attrib_active) {
			shader->normal_vertex_attrib_active=TRUE;
			if (shader->var_locs.dim3VertexTangent!=-1) glEnableVertexAttribArrayARB(shader->var_locs.dim3VertexTangent); 
			if (shader->var_locs.dim3VertexBinormal!=-1) glEnableVertexAttribArrayARB(shader->var_locs.dim3VertexBinormal); 
			if (shader->var_locs.dim3VertexNormal!=-1) glEnableVertexAttribArrayARB(shader->var_locs.dim3VertexNormal); 
		}

		if (shader->var_locs.dim3VertexTangent!=-1) glVertexAttribPointerARB(shader->var_locs.dim3VertexTangent,3,GL_FLOAT,0,0,model_draw_array->gl_tangent_array);
		if (shader->var_locs.dim3VertexBinormal!=-1) glVertexAttribPointerARB(shader->var_locs.dim3VertexBinormal,3,GL_FLOAT,0,0,model_draw_array->gl_binormal_array);
		if (shader->var_locs.dim3VertexNormal!=-1) glVertexAttribPointerARB(shader->var_locs.dim3VertexNormal,3,GL_FLOAT,0,0,model_draw_array->gl_normal_array);
	}
	else {
		if (shader->normal_vertex_attrib_active) {
			shader->normal_vertex_attrib_active=FALSE;
			if (shader->var_locs.dim3VertexTangent!=-1) glDisableVertexAttribArrayARB(shader->var_locs.dim3VertexTangent); 
			if (shader->var_locs.dim3VertexBinormal!=-1) glDisableVertexAttribArrayARB(shader->var_locs.dim3VertexBinormal); 
			if (shader->var_locs.dim3VertexNormal!=-1) glDisableVertexAttribArrayARB(shader->var_locs.dim3VertexNormal);
		}
	}
}

/* =======================================================

      Shader Drawing Initialize/Start/Stop
      
======================================================= */

void gl_shader_draw_scene_initialize_code(shader_type *shader)
{
		// use this flag to mark scene only variables
		// as needing a load.  In this way we optimize
		// out the amount of variable setting we need to do
	
	shader->per_scene_vars_set=FALSE;
		
		// also setup some per poly current values
		// so we can skip setting if the values haven't changed

	shader->cur_nlight=-1;
	shader->cur_tint_col.r=shader->cur_tint_col.g=shader->cur_tint_col.b=-1.0f;
	shader->cur_alpha=-1.0f;
	shader->normal_vertex_attrib_active=FALSE;
}

void gl_shader_draw_scene_initialize(void)
{
	gl_core_shader_draw_scene_initialize();
	gl_user_shader_draw_scene_initialize();
}

void gl_shader_draw_start(void)
{
		// remember current shader

	gl_shader_current=NULL;

		// only reset textures when
		// needed
	
	gl_shader_current_txt_idx=-1;
	gl_shader_current_frame=-1;
	gl_shader_current_lmap_txt_idx=-1;

		// make all textures replace
		
	glColor4f(1.0f,0.0f,1.0f,1.0f);

	glActiveTexture(GL_TEXTURE3);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	
	glActiveTexture(GL_TEXTURE2);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glActiveTexture(GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glActiveTexture(GL_TEXTURE0);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void gl_shader_draw_end(void)
{
		// deactivate any current shader
		
	if (gl_shader_current!=NULL) glUseProgramObjectARB(0);
	
		// turn off any used textures
		
	glActiveTexture(GL_TEXTURE3);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE2);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Set Shader Textures
      
======================================================= */

void gl_shader_texture_set(shader_type *shader,texture_type *texture,int txt_idx,int lmap_txt_idx,int frame)
{
	GLuint			gl_id;

		// any changes?

	if ((gl_shader_current_txt_idx==txt_idx) && (gl_shader_current_frame==frame) && (gl_shader_current_lmap_txt_idx==lmap_txt_idx)) return;

	gl_shader_current_txt_idx=txt_idx;
	gl_shader_current_frame=frame;
	gl_shader_current_lmap_txt_idx=lmap_txt_idx;
	
		// extra texture map

	if (lmap_txt_idx!=-1) {
		gl_id=map.textures[lmap_txt_idx].frames[0].bitmap.gl_id;

		if (gl_id!=-1) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D,gl_id);
		}
	}
	
		// spec map

	gl_id=texture->frames[frame].specularmap.gl_id;

	if (gl_id!=-1) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D,gl_id);
	}

		// bump map

	gl_id=texture->frames[frame].bumpmap.gl_id;

	if (gl_id!=-1) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,gl_id);
	}
	
		// color map

	gl_id=texture->frames[frame].bitmap.gl_id;

	if (gl_id!=-1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,gl_id);
	}

		// set per-texture specific variables

	gl_shader_set_texture_variables(shader,texture);
}

void gl_shader_texture_override(GLuint gl_id)
{
		// normally used to override for back rendering

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,gl_id);

		// need to force a reset for next time

	gl_shader_current_txt_idx=-1;
	gl_shader_current_frame=-1;
	gl_shader_current_lmap_txt_idx=-1;
}

/* =======================================================

      Execute Shader
      
======================================================= */

void gl_shader_draw_execute(bool map_shader,texture_type *texture,int txt_idx,int frame,int lmap_txt_idx,float alpha,view_light_list_type *light_list,d3pnt *pnt,d3col *tint_col,tangent_space_type *tangent_space,model_draw_array_type *model_draw_array)
{
	int							n;
	bool						light_change;
	shader_type					*shader;
	
		// get shader based on number of lights.
		
	if (texture->shader_idx==gl_shader_core_index) {
		shader=gl_core_shader_find_ptr(light_list->nlight,map_shader,texture,(lmap_txt_idx!=-1));
	}
	else {
		shader=&user_shaders[texture->shader_idx];
	}
	
		// if we are not in this shader, then
		// change over
		
	if (shader!=gl_shader_current) {
	
			// set in the new program
			
		gl_shader_current=shader;
		glUseProgramObjectARB(shader->program_obj);
			
			// set per-scene variables, only do this once
			// as they don't change per scene
		
		if (!shader->per_scene_vars_set) {
			shader->per_scene_vars_set=TRUE;
			gl_shader_set_scene_variables(shader,light_list);
		}

			// a shader change will force a texture
			// change as certain variables might not
			// be loaded in the texture

		gl_shader_current_txt_idx=-1;
	}
	
		// textures and per-texture variables
		
	gl_shader_texture_set(shader,texture,txt_idx,lmap_txt_idx,frame);
	
		// per polygon variables
		
	gl_shader_set_poly_variables(shader,alpha,tint_col,tangent_space,model_draw_array);
	
		// lighting variables
			
	light_change=(light_list->nlight!=shader->cur_nlight);
	
	for (n=0;n!=light_list->nlight;n++) {
		if (shader->cur_light_idx[n]!=light_list->light_idx[n]) light_change=TRUE;
		shader->cur_light_idx[n]=light_list->light_idx[n];
	}
	
	shader->cur_nlight=light_list->nlight;
	
	if (light_change) gl_shader_set_light_normal_variables(shader,light_list);
}
