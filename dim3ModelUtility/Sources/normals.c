/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Normal Routines

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

#define normal_near_vertex_center_dist		250

/* =======================================================

      Calculate Normals for Model
      
======================================================= */

int model_recalc_normals_find_material(model_type *model,int mesh_idx,int trig_idx)
{
	int						n;
	model_material_type		*material;
    texture_type			*texture;

	texture=model->textures;
	material=model->meshes[mesh_idx].materials;

	for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].name[0]!=0x0) {
			if ((trig_idx>=material->trig_start) && (trig_idx<(material->trig_start+material->trig_count))) return(n);
		}
		texture++;
		material++;
	}

	return(0);
}

bool model_recalc_normals_compare_sign(float f1,float f2)
{
	if ((f1<0.0f) && (f2<0.0f)) return(TRUE);
	if ((f1>=0.0f) && (f2>=0.0f)) return(TRUE);
	return(FALSE);
}

void model_recalc_normals_mesh(model_type *model,int mesh_idx,bool only_tangent_binormal)
{
	int					n,k,t,j,cnt,trig_material_idx;
    float				u10,u20,v10,v20,f_denom,f;
	bool				is_out;
	d3vct				p10,p20,vlft,vrgt,v_num,face_vct;
	d3vct				*normals,*nptr,*tangents,*tptr,*binormals,*bptr;
	d3pnt				*pt,*pt_1,*pt_2,v_center;
	model_mesh_type		*mesh;
    model_vertex_type	*vertex;
	model_trig_type		*trig,*chk_trig;
	tangent_space_type	avg_space;
	
	mesh=&model->meshes[mesh_idx];
	if ((mesh->nvertex==0) || (mesh->ntrig==0)) return;

		// memory for tangent space

	normals=(d3vct*)malloc(mesh->ntrig*sizeof(d3vct));
	if (normals==NULL) return;

	tangents=(d3vct*)malloc(mesh->ntrig*sizeof(d3vct));
	if (tangents==NULL) {
		free(normals);
		return;
	}

	binormals=(d3vct*)malloc(mesh->ntrig*sizeof(d3vct));
	if (binormals==NULL) {
		free(normals);
		free(tangents);
		return;
	}
	
        // find tangent and binormal for triangles
		
	trig=mesh->trigs;

	nptr=normals;
	tptr=tangents;
	bptr=binormals;
	
	for (n=0;n!=mesh->ntrig;n++) {
    
			// get the side vectors (p1-p0) and (p2-p0)

		pt=&mesh->vertexes[trig->v[0]].pnt;
		pt_1=&mesh->vertexes[trig->v[1]].pnt;
		pt_2=&mesh->vertexes[trig->v[2]].pnt;

		vector_create(&p10,pt_1->x,pt_1->y,pt_1->z,pt->x,pt->y,pt->z);
		vector_create(&p20,pt_2->x,pt_2->y,pt_2->z,pt->x,pt->y,pt->z);

			// get the UV scalars (u1-u0), (u2-u0), (v1-v0), (v2-v0)

		u10=trig->gx[1]-trig->gx[0];
		u20=trig->gx[2]-trig->gx[0];
		v10=trig->gy[1]-trig->gy[0];
		v20=trig->gy[2]-trig->gy[0];

			// calculate the tangent
			// (v20xp10)-(v10xp20) / (u10*v20)-(v10*u20)

		vector_scalar_multiply(&vlft,&p10,v20);
		vector_scalar_multiply(&vrgt,&p20,v10);
		vector_subtract(&v_num,&vlft,&vrgt);

		f_denom=(u10*v20)-(v10*u20);
		if (f_denom!=0.0f) f_denom=1.0f/f_denom;
		vector_scalar_multiply(tptr,&v_num,f_denom);

			// calculate the binormal
			// (u20xp10)-(u10xp20) / (v10*u20)-(u10*v20)

		vector_scalar_multiply(&vlft,&p10,u20);
		vector_scalar_multiply(&vrgt,&p20,u10);
		vector_subtract(&v_num,&vlft,&vrgt);

		f_denom=(v10*u20)-(u10*v20);
		if (f_denom!=0.0f) f_denom=1.0f/f_denom;
		vector_scalar_multiply(bptr,&v_num,f_denom);

		vector_normalize(tptr);
		vector_normalize(bptr);

		vector_cross_product(nptr,tptr,bptr);
			
		trig++;

		nptr++;
		tptr++;
		bptr++;
	}
    
		// average tangent space for each
		// trig vertex that are within 45 degrees
		// of each other.  otherwise, treat as a hard
		// edge
		
	trig=mesh->trigs;

	for (n=0;n!=mesh->ntrig;n++) {

			// average each vertex of the trig

		for (t=0;t!=3;t++) {
	
			cnt=1;

			memmove(&avg_space.normal,(normals+n),sizeof(d3vct));
			memmove(&avg_space.tangent,(tangents+n),sizeof(d3vct));
			memmove(&avg_space.binormal,(binormals+n),sizeof(d3vct));
			
			tptr=tangents;
			bptr=binormals;
			
			for (k=0;k!=mesh->ntrig;k++) {

					// same trig

				if (n==k) continue;

					// get trig

				chk_trig=&mesh->trigs[k];

				nptr=normals+k;
				tptr=tangents+k;
				bptr=binormals+k;

					// check for shared vertexes
					// and normals within 45 degrees

				for (j=0;j!=3;j++) {

					if (trig->v[t]==chk_trig->v[j]) {

						if (vector_dot_product(&avg_space.normal,nptr)<0.7f) continue;

						avg_space.normal.x+=nptr->x;
						avg_space.normal.y+=nptr->y;
						avg_space.normal.z+=nptr->z;

						avg_space.tangent.x+=tptr->x;
						avg_space.tangent.y+=tptr->y;
						avg_space.tangent.z+=tptr->z;
					
						avg_space.binormal.x+=bptr->x;
						avg_space.binormal.y+=bptr->y;
						avg_space.binormal.z+=bptr->z;

						cnt++;
						break;
					}
				}
			}
			
				// create average vector

			if (cnt>1) {
				f=(float)cnt;

				avg_space.normal.x/=f;
				avg_space.normal.y/=f;
				avg_space.normal.z/=f;
				vector_normalize(&avg_space.normal);

				avg_space.tangent.x/=f;
				avg_space.tangent.y/=f;
				avg_space.tangent.z/=f;
				vector_normalize(&avg_space.tangent);

				avg_space.binormal.x/=f;
				avg_space.binormal.y/=f;
				avg_space.binormal.z/=f;
				vector_normalize(&avg_space.binormal);
			}

			if (!only_tangent_binormal) memmove(&trig->tangent_space[t].normal,&avg_space.normal,sizeof(d3vct));
			memmove(&trig->tangent_space[t].tangent,&avg_space.tangent,sizeof(d3vct));
			memmove(&trig->tangent_space[t].binormal,&avg_space.binormal,sizeof(d3vct));
		}

		trig++;
	}

		// free the tangent spaces

	free(normals);
	free(tangents);
	free(binormals);
	
		// fix any normals that are 0,0,0
		// this usually happens when there are bad
		// or no UVs
		
	cnt=0;
	v_center.x=v_center.y=v_center.z=0;
	
	if (mesh->nvertex!=0) {

		for (k=0;k!=mesh->nvertex;k++) {
			pt=&mesh->vertexes[k].pnt;
			v_center.x+=pt->x;
			v_center.y+=pt->y;
			v_center.z+=pt->z;
		}
		
		v_center.x/=mesh->nvertex;
		v_center.y/=mesh->nvertex;
		v_center.z/=mesh->nvertex;
	}
		
	trig=mesh->trigs;
	
	for (n=0;n!=mesh->ntrig;n++) {
		for (t=0;t!=3;t++) {
			if ((trig->tangent_space[t].normal.x==0.0f) && (trig->tangent_space[t].normal.y==0.0f) && (trig->tangent_space[t].normal.z==0.0f)) {
				pt=&mesh->vertexes[trig->v[t]].pnt;
				trig->tangent_space[t].normal.x=(float)(pt->x-v_center.x);
				trig->tangent_space[t].normal.y=(float)(pt->y-v_center.y);
				trig->tangent_space[t].normal.z=(float)(pt->z-v_center.z);
				vector_normalize(&trig->tangent_space[t].normal);
			}
		}
		trig++;
	}

		// determine in-out to map flips
		// skip if not calculating normals
		
	if (only_tangent_binormal) return;

		// determine in/out and invert
		
	trig=mesh->trigs;

	for (n=0;n!=mesh->ntrig;n++) {

			// get the material for the trig

		trig_material_idx=model_recalc_normals_find_material(model,mesh_idx,n);

			// for each trig, we only want to consider
			// the center being around vertexes connected
			// to the same material

		cnt=0;
		v_center.x=v_center.y=v_center.z=0;

		chk_trig=mesh->trigs;

		for (k=0;k!=mesh->ntrig;k++) {
			if (model_recalc_normals_find_material(model,mesh_idx,k)==trig_material_idx) {
				for (j=0;j!=3;j++) {
					v_center.x+=mesh->vertexes[chk_trig->v[j]].pnt.x;
					v_center.y+=mesh->vertexes[chk_trig->v[j]].pnt.y;
					v_center.z+=mesh->vertexes[chk_trig->v[j]].pnt.z;
					cnt++;
				}
			}
			chk_trig++;
		}
		
		if (cnt>1) {
			v_center.x/=cnt;
			v_center.y/=cnt;
			v_center.z/=cnt;
		}

			// determine if it's facing in or out
			// from this center
	
		for (k=0;k!=3;k++) {
		
			vertex=&mesh->vertexes[trig->v[k]];
			
			vector_create(&face_vct,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,v_center.x,v_center.y,v_center.z);
			is_out=(vector_dot_product(&trig->tangent_space[k].normal,&face_vct)>0.0f);

			if (!is_out) {
				trig->tangent_space[k].normal.x=-trig->tangent_space[k].normal.x;
				trig->tangent_space[k].normal.y=-trig->tangent_space[k].normal.y;
				trig->tangent_space[k].normal.z=-trig->tangent_space[k].normal.z;
			}
		}
		
		trig++;
	}
}

void model_recalc_normals(model_type *model,bool only_tangent_binormal)
{
	int				n;

	for (n=0;n!=model->nmesh;n++) {
		model_recalc_normals_mesh(model,n,only_tangent_binormal);
	}
}

