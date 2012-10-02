#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

ray_global_type					ray_global;

/* =======================================================

      Find Mesh Index for ID
      
======================================================= */

int ray_scene_mesh_get_index(ray_scene_type *scene,int meshId)
{
	int					n;

	if (meshId==-1) return(-1);

	for (n=0;n!=scene->mesh_list.count;n++) {
		if (scene->mesh_list.meshes[n]->id==meshId) return(n);
	}

	return(-1);
}

/* =======================================================

      Set Mesh Bounds
      
======================================================= */

void ray_scene_mesh_precalc(ray_scene_type *scene,ray_mesh_type *mesh)
{
	int					n,k;
	ray_poly_type		*poly;
	ray_trig_type		*trig;
	
		// can't do any precalcs
		// if no vertex data
		
	if (mesh->vertex_block.vertexes==NULL) return;
	
		// mesh bounds
		
	ray_precalc_mesh_bounds(mesh);
	
		// polygon and trig precalcs,
		// can't do if no polygon data
		
	if (mesh->poly_block.polys==NULL) return;

	poly=mesh->poly_block.polys;
	
	for (n=0;n!=mesh->poly_block.count;n++) {
			
			// poly precalcs
			
		ray_precalc_polygon_bounds(mesh,poly);
	
			// trig precalcs
			
		trig=poly->trig_block.trigs;
			
		for (k=0;k!=poly->trig_block.count;k++) {
			ray_precalc_triangle_vectors(mesh,trig);
			ray_precalc_triangle_bounds(mesh,trig);
			trig++;
		}

		poly++;
	}
}

/* =======================================================

      Adds a New Mesh to a Scene

	  Returns:
	   If >=0, then a mesh ID
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneMeshAdd(int sceneId,unsigned long flags)
{
	int					idx;
	ray_mesh_type		*mesh;
	ray_scene_type		*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// add mesh

	mesh=(ray_mesh_type*)malloc(sizeof(ray_mesh_type));
	if (mesh==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		// mesh settings

	mesh->flags=flags;
	mesh->hidden=FALSE;

	mesh->vertex_block.count=0;
	mesh->vertex_block.vertexes=NULL;
		
	mesh->uv_block.count=0;
	mesh->uv_block.uvs=NULL;

	mesh->normal_block.count=0;
	mesh->normal_block.normals=NULL;

	mesh->tangent_block.count=0;
	mesh->tangent_block.tangents=NULL;

	mesh->poly_block.count=0;
	mesh->poly_block.polys=NULL;

		// set id

	mesh->id=scene->mesh_list.next_id;
	scene->mesh_list.next_id++;

		// add mesh to list

	scene->mesh_list.meshes[scene->mesh_list.count]=mesh;
	scene->mesh_list.count++;

	return(mesh->id);
}

/* =======================================================

      Deletes a Mesh from a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MESH_ID
	   RL_ERROR_SCENE_IN_USE
      
======================================================= */

int rlSceneMeshDelete(int sceneId,int meshId)
{
	int				n,idx,count;
	ray_mesh_type	*mesh;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can't delete if scene in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);
	
		// get the mesh index
		
	idx=ray_scene_mesh_get_index(scene,meshId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MESH_ID);
	
		// get the mesh

	mesh=scene->mesh_list.meshes[idx];

		// clear any data

	if (mesh->vertex_block.vertexes!=NULL) free(mesh->vertex_block.vertexes);
	if (mesh->uv_block.uvs!=NULL) free(mesh->uv_block.uvs);
	if (mesh->normal_block.normals!=NULL) free(mesh->normal_block.normals);
	if (mesh->tangent_block.tangents!=NULL) free(mesh->tangent_block.tangents);
	
	for (n=0;n!=mesh->poly_block.count;n++) {
		if (mesh->poly_block.polys[n].trig_block.trigs!=NULL) free(mesh->poly_block.polys[n].trig_block.trigs);
	}
	
	if (mesh->poly_block.polys!=NULL) free(mesh->poly_block.polys);

		// remove mesh

	free(mesh);

	count=scene->mesh_list.count-2;

	for (n=idx;n<=count;n++) {
		scene->mesh_list.meshes[n]=scene->mesh_list.meshes[n+1];
	}

	scene->mesh_list.count--;

	return(RL_ERROR_OK);
}

/* =======================================================

      Deletes all Meshes from a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

int rlSceneMeshDeleteAll(int sceneId)
{
	int				idx,err;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// free memory

	while (scene->mesh_list.count!=0) {
		err=rlSceneMeshDelete(sceneId,scene->mesh_list.meshes[0]->id);
		if (err!=RL_ERROR_OK) return(err);
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Sets Hidden State For Mesh

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MESH_ID
	   RL_ERROR_SCENE_IN_USE
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneMeshSetHidden(int sceneId,int meshId,bool hidden)
{
	int				idx;
	ray_mesh_type	*mesh;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can't alter scenes in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// get mesh

	idx=ray_scene_mesh_get_index(scene,meshId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MESH_ID);

	mesh=scene->mesh_list.meshes[idx];
	mesh->hidden=hidden;
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Sets Vertexes for a Mesh

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MESH_ID
	   RL_ERROR_SCENE_IN_USE
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneMeshSetVertex(int sceneId,int meshId,int format,int count,void *vertex_data)
{
	int				n,idx;
	float			*vp;
	ray_point_type	*vertexes,*vertex;
	ray_mesh_type	*mesh;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can't alter scenes in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// get mesh

	idx=ray_scene_mesh_get_index(scene,meshId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MESH_ID);

	mesh=scene->mesh_list.meshes[idx];

		// get new memory

	vertexes=(ray_point_type*)malloc(count*sizeof(ray_point_type));
	if (vertexes==NULL) return(RL_ERROR_OUT_OF_MEMORY);

	if (mesh->vertex_block.vertexes!=NULL) free(mesh->vertex_block.vertexes);

	mesh->vertex_block.count=count;
	mesh->vertex_block.vertexes=vertexes;

		// copy the vertexes

	vertex=vertexes;
	vp=(float*)vertex_data;

	for (n=0;n!=count;n++) {
		vertex->x=*vp++;
		vertex->y=*vp++;
		vertex->z=*vp++;
		vertex++;
	}
	
		// vertex changes require
		// a rebuild of precalcs
		
	ray_scene_mesh_precalc(scene,mesh);

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets UVs for a Mesh

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MESH_ID
	   RL_ERROR_SCENE_IN_USE
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneMeshSetUV(int sceneId,int meshId,int format,int count,void *uv_data)
{
	int				n,idx;
	float			*tp;
	ray_uv_type		*uvs,*uv;
	ray_mesh_type	*mesh;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can't alter scenes in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// get mesh

	idx=ray_scene_mesh_get_index(scene,meshId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MESH_ID);

	mesh=scene->mesh_list.meshes[idx];

		// get new memory

	uvs=(ray_uv_type*)malloc(count*sizeof(ray_uv_type));
	if (uvs==NULL) return(RL_ERROR_OUT_OF_MEMORY);

	if (mesh->uv_block.uvs!=NULL) free(mesh->uv_block.uvs);

	mesh->uv_block.count=count;
	mesh->uv_block.uvs=uvs;

		// copy the vertexes

	uv=uvs;
	tp=(float*)uv_data;

	for (n=0;n!=count;n++) {
		uv->x=*tp++;
		uv->y=*tp++;
		uv++;
	}

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets Normals for a Mesh

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MESH_ID
	   RL_ERROR_UNKNOWN_FORMAT
	   RL_ERROR_SCENE_IN_USE
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneMeshSetNormal(int sceneId,int meshId,int format,int count,void *normal_data)
{
	int				n,idx;
	float			*np;
	ray_vector_type	*normals,*normal;
	ray_mesh_type	*mesh;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can't alter scenes in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// get mesh

	idx=ray_scene_mesh_get_index(scene,meshId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MESH_ID);

	mesh=scene->mesh_list.meshes[idx];

		// get the format

	if (format!=RL_MESH_FORMAT_NORMAL_3_FLOAT) return(RL_ERROR_UNKNOWN_FORMAT);

		// get new memory

	normals=(ray_vector_type*)malloc(count*sizeof(ray_vector_type));
	if (normals==NULL) return(RL_ERROR_OUT_OF_MEMORY);

	if (mesh->normal_block.normals!=NULL) free(mesh->normal_block.normals);

	mesh->normal_block.count=count;
	mesh->normal_block.normals=normals;

		// copy the normals

	normal=normals;
	np=(float*)normal_data;

	for (n=0;n!=count;n++) {
		normal->x=*np++;
		normal->y=*np++;
		normal->z=*np++;
		normal++;
	}

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets Tangents for a Mesh

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MESH_ID
	   RL_ERROR_UNKNOWN_FORMAT
	   RL_ERROR_SCENE_IN_USE
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneMeshSetTangent(int sceneId,int meshId,int format,int count,void *tangent_data)
{
	int				n,idx;
	float			*np;
	ray_vector_type	*tangents,*tangent;
	ray_mesh_type	*mesh;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can't alter scenes in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// get mesh

	idx=ray_scene_mesh_get_index(scene,meshId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MESH_ID);

	mesh=scene->mesh_list.meshes[idx];

		// get the format

	if (format!=RL_MESH_FORMAT_TANGENT_3_FLOAT) return(RL_ERROR_UNKNOWN_FORMAT);

		// get new memory

	tangents=(ray_vector_type*)malloc(count*sizeof(ray_vector_type));
	if (tangents==NULL) return(RL_ERROR_OUT_OF_MEMORY);

	if (mesh->tangent_block.tangents!=NULL) free(mesh->tangent_block.tangents);

	mesh->tangent_block.count=count;
	mesh->tangent_block.tangents=tangents;

		// copy the normals

	tangent=tangents;
	np=(float*)tangent_data;

	for (n=0;n!=count;n++) {
		tangent->x=*np++;
		tangent->y=*np++;
		tangent->z=*np++;
		tangent++;
	}

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets Polys for a Mesh

	  The current format is first short = vertexes count
	  The next shorts are count of a pair of vertex offset
	  followed by a uv offset

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MESH_ID
	   RL_ERROR_UNKNOWN_FORMAT
	   RL_ERROR_SCENE_IN_USE
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneMeshSetPoly(int sceneId,int meshId,int format,int count,void *poly_data)
{
	int						n,k,ntrig,scene_idx,mesh_idx;
	short					*pp;
	ray_polygon_index_type	*pidx;
	ray_mesh_type			*mesh;
	ray_poly_type			*polys,*poly;
	ray_trig_type			*trigs,*trig;
	ray_scene_type			*scene;

		// get scene

	scene_idx=ray_scene_get_index(sceneId);
	if (scene_idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[scene_idx];

		// can't alter scenes in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// get mesh

	mesh_idx=ray_scene_mesh_get_index(scene,meshId);
	if (mesh_idx==-1) return(RL_ERROR_UNKNOWN_MESH_ID);

	mesh=scene->mesh_list.meshes[mesh_idx];

		// get the format

	if (format!=RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT) return(RL_ERROR_UNKNOWN_FORMAT);
	
		// create polygon memory
		
	polys=(ray_poly_type*)malloc(count*sizeof(ray_poly_type));
	if (polys==NULL) return(RL_ERROR_OUT_OF_MEMORY);
	
	if (mesh->poly_block.polys!=NULL) free(mesh->poly_block.polys);
	
	mesh->poly_block.count=count;
	mesh->poly_block.polys=polys;
	
	poly=polys;

	for (k=0;k!=count;k++) {
		poly->trig_block.trigs=NULL;
		poly++;
	}

		// copy the polys and
		// create the split trigs

	poly=polys;
	pp=poly_data;

	for (n=0;n!=count;n++) {

			// first short is vertex
			// count for polygon and
			// second is the material

		poly->nvertex=(int)*pp++;
		poly->material_idx=ray_material_get_index((int)*pp++);		// supergumba -- look up errors here
		
			// load the vertexes

		pidx=poly->idxs;
			
		for (k=0;k!=poly->nvertex;k++) {
			pidx->vertex=(int)*pp++;
			pidx->uv=(int)*pp++;
			pidx->normal=(int)*pp++;
			pidx->tangent=(int)*pp++;
			pidx++;
		}

			// get memory for tesselated
			// trigs, we need to clean
			// up other trigs if we run out
			// of memory
			
		ntrig=poly->nvertex-2;
		
		trigs=(ray_trig_type*)malloc(ntrig*sizeof(ray_trig_type));
		if (trigs==NULL) {
		
			poly=polys;
		
			for (k=0;k!=count;k++) {
				if (poly->trig_block.trigs!=NULL) free(poly->trig_block.trigs);
				poly++;
			}
			
			free(polys);
			
			return(RL_ERROR_OUT_OF_MEMORY);
		}
	
		if (poly->trig_block.trigs!=NULL) free(poly->trig_block.trigs);

		poly->trig_block.count=ntrig;
		poly->trig_block.trigs=trigs;
	
			// tesselate polygon
			// into trigs
			
		trig=trigs;
			
		for (k=0;k!=ntrig;k++) {
			memmove(&trig->idxs[0],&poly->idxs[0],sizeof(ray_polygon_index_type));
			memmove(&trig->idxs[1],&poly->idxs[k+1],sizeof(ray_polygon_index_type));
			memmove(&trig->idxs[2],&poly->idxs[k+2],sizeof(ray_polygon_index_type));
			trig++;
		}

		poly++;
	}
	
		// poly change requires
		// rebuild of precalc
		
	ray_scene_mesh_precalc(scene,mesh);

	return(RL_ERROR_OK);
}
