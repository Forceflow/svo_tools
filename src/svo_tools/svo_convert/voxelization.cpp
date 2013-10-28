#include "voxelization.h"

using namespace std;
using namespace trimesh;

// Implementation of http://research.michael-schwarz.com/publ/2010/vox/ (Schwarz & Seidel)
// Adapted for mortoncode based subgrids
// by Jeroen Baert - jeroen.baert@cs.kuleuven.be

#define X 0
#define Y 1
#define Z 2

void voxelize(const TriMesh* mesh, size_t gridsize, float unitlength, size_t* voxels, vector<VoxelData>& voxel_data, size_t& nfilled) {
	size_t morton_start = 0;
	size_t morton_end = gridsize*gridsize*gridsize;

	// Clear data
	memset(voxels,0,(morton_end-morton_start)*sizeof(size_t));
	voxel_data.clear();

	// compute partition min and max in grid coords
	AABox<uivec3> p_bbox_grid;
	mortonDecode(morton_start, p_bbox_grid.min[2], p_bbox_grid.min[1], p_bbox_grid.min[0]);
	mortonDecode(morton_end-1, p_bbox_grid.max[2], p_bbox_grid.max[1], p_bbox_grid.max[0]);

	// COMMON PROPERTIES FOR ALL TRIANGLES
	float unit_div = 1.0f / unitlength;
	vec3 delta_p = vec3(unitlength,unitlength,unitlength);

	for(size_t i = 0; i < mesh->faces.size(); i++){

		// GRAB TRIANGLE INFO;
		Triangle t;
		t.v0 = mesh->vertices[mesh->faces[i][X]];
		t.v1 = mesh->vertices[mesh->faces[i][Y]];
		t.v2 = mesh->vertices[mesh->faces[i][Z]];
		t.normal = average3Vec<3,float>(mesh->normals[mesh->faces[i][X]], mesh->normals[mesh->faces[i][Y]], mesh->normals[mesh->faces[i][Z]]);
		// TODO: grab colors

		// compute triangle bbox in world and grid
		AABox<vec3> t_bbox_world = computeBoundingBox(t.v0,t.v1,t.v2);
		AABox<ivec3> t_bbox_grid;
		t_bbox_grid.min[0] = (int) (t_bbox_world.min[0] * unit_div); // using integer rounding to construct bbox
		t_bbox_grid.min[1] = (int) (t_bbox_world.min[1] * unit_div);
		t_bbox_grid.min[2] = (int) (t_bbox_world.min[2] * unit_div);
		t_bbox_grid.max[0] = (int) (t_bbox_world.max[0] * unit_div);
		t_bbox_grid.max[1] = (int) (t_bbox_world.max[1] * unit_div);
		t_bbox_grid.max[2] = (int) (t_bbox_world.max[2] * unit_div);

		// clamp grid bounding box to partition bounding box
		t_bbox_grid.min[0]  = clampval<int>(t_bbox_grid.min[0], p_bbox_grid.min[0], p_bbox_grid.max[0]);
		t_bbox_grid.min[1]  = clampval<int>(t_bbox_grid.min[1], p_bbox_grid.min[1], p_bbox_grid.max[1]);
		t_bbox_grid.min[2]  = clampval<int>(t_bbox_grid.min[2], p_bbox_grid.min[2], p_bbox_grid.max[2]);
		t_bbox_grid.max[0]  = clampval<int>(t_bbox_grid.max[0], p_bbox_grid.min[0], p_bbox_grid.max[0]);
		t_bbox_grid.max[1]  = clampval<int>(t_bbox_grid.max[1], p_bbox_grid.min[1], p_bbox_grid.max[1]);
		t_bbox_grid.max[2]  = clampval<int>(t_bbox_grid.max[2], p_bbox_grid.min[2], p_bbox_grid.max[2]);

		// COMMON PROPERTIES FOR THE TRIANGLE
		vec3 e0 = t.v1 - t.v0;
		vec3 e1 = t.v2 - t.v1;
		vec3 e2 = t.v0 - t.v2;
		vec3 to_normalize = (e0) CROSS (e1);
		vec3 n = normalize(to_normalize); // triangle normal
		// PLANE TEST PROPERTIES
		vec3 c = vec3(0.0f,0.0f,0.0f); // critical point
		if(n[X] > 0) { c[X] = unitlength;} 
		if(n[Y] > 0) { c[Y] = unitlength;} 
		if(n[Z] > 0) { c[Z] = unitlength;} 
		float d1 = n DOT (c - t.v0); 
		float d2 = n DOT ((delta_p - c) - t.v0);
		// PROJECTION TEST PROPERTIES
		// XY plane
		vec2 n_xy_e0 = vec2(-1.0f*e0[Y], e0[X]);
		vec2 n_xy_e1 = vec2(-1.0f*e1[Y], e1[X]);
		vec2 n_xy_e2 = vec2(-1.0f*e2[Y], e2[X]);
		if(n[Z] < 0.0f) { 
			n_xy_e0 = -1.0f * n_xy_e0;
			n_xy_e1 = -1.0f * n_xy_e1;
			n_xy_e2 = -1.0f * n_xy_e2;
		}
		float d_xy_e0 = (-1.0f * (n_xy_e0 DOT vec2(t.v0[X],t.v0[Y]))) + max(0.0f, unitlength*n_xy_e0[0]) + max(0.0f, unitlength*n_xy_e0[1]);
		float d_xy_e1 = (-1.0f * (n_xy_e1 DOT vec2(t.v1[X],t.v1[Y]))) + max(0.0f, unitlength*n_xy_e1[0]) + max(0.0f, unitlength*n_xy_e1[1]);
		float d_xy_e2 = (-1.0f * (n_xy_e2 DOT vec2(t.v2[X],t.v2[Y]))) + max(0.0f, unitlength*n_xy_e2[0]) + max(0.0f, unitlength*n_xy_e2[1]);

		// YZ plane
		vec2 n_yz_e0 = vec2(-1.0f*e0[Z], e0[Y]);
		vec2 n_yz_e1 = vec2(-1.0f*e1[Z], e1[Y]);
		vec2 n_yz_e2 = vec2(-1.0f*e2[Z], e2[Y]);
		if(n[X] < 0.0f) { 
			n_yz_e0 = -1.0f * n_yz_e0;
			n_yz_e1 = -1.0f * n_yz_e1;
			n_yz_e2 = -1.0f * n_yz_e2;
		}
		float d_yz_e0 = (-1.0f * (n_yz_e0 DOT vec2(t.v0[Y],t.v0[Z]))) + max(0.0f, unitlength*n_yz_e0[0]) + max(0.0f, unitlength*n_yz_e0[1]);
		float d_yz_e1 = (-1.0f * (n_yz_e1 DOT vec2(t.v1[Y],t.v1[Z]))) + max(0.0f, unitlength*n_yz_e1[0]) + max(0.0f, unitlength*n_yz_e1[1]);
		float d_yz_e2 = (-1.0f * (n_yz_e2 DOT vec2(t.v2[Y],t.v2[Z]))) + max(0.0f, unitlength*n_yz_e2[0]) + max(0.0f, unitlength*n_yz_e2[1]);

		// ZX plane
		vec2 n_zx_e0 = vec2(-1.0f*e0[X], e0[Z]);
		vec2 n_zx_e1 = vec2(-1.0f*e1[X], e1[Z]);
		vec2 n_zx_e2 = vec2(-1.0f*e2[X], e2[Z]);
		if(n[Y] < 0.0f) { 
			n_zx_e0 = -1.0f * n_zx_e0;
			n_zx_e1 = -1.0f * n_zx_e1;
			n_zx_e2 = -1.0f * n_zx_e2;
		}
		float d_xz_e0 = (-1.0f * (n_zx_e0 DOT vec2(t.v0[Z],t.v0[X]))) + max(0.0f, unitlength*n_zx_e0[0]) + max(0.0f, unitlength*n_zx_e0[1]);
		float d_xz_e1 = (-1.0f * (n_zx_e1 DOT vec2(t.v1[Z],t.v1[X]))) + max(0.0f, unitlength*n_zx_e1[0]) + max(0.0f, unitlength*n_zx_e1[1]);
		float d_xz_e2 = (-1.0f * (n_zx_e2 DOT vec2(t.v2[Z],t.v2[X]))) + max(0.0f, unitlength*n_zx_e2[0]) + max(0.0f, unitlength*n_zx_e2[1]);

		// test possible grid boxes for overlap
		for(int x = t_bbox_grid.min[X]; x <= t_bbox_grid.max[X]; x++){
			for(int y = t_bbox_grid.min[Y]; y <= t_bbox_grid.max[Y]; y++){
				for(int z = t_bbox_grid.min[Z]; z <= t_bbox_grid.max[Z]; z++){

					uint64_t index = mortonEncode_LUT(z,y,x);

					//assert(index-morton_start < (morton_end-morton_start));

					if(! voxels[index-morton_start] == EMPTY_VOXEL){continue;} // already marked, continue

					// TRIANGLE PLANE THROUGH BOX TEST
					vec3 p = vec3(x*unitlength,y*unitlength,z*unitlength);
					float nDOTp = n DOT p;
					if( (nDOTp + d1) * (nDOTp + d2) > 0.0f ){continue;}

					// PROJECTION TESTS
					// XY
					vec2 p_xy = vec2(p[X],p[Y]);
					if (((n_xy_e0 DOT p_xy) + d_xy_e0) < 0.0f){continue;}
					if (((n_xy_e1 DOT p_xy) + d_xy_e1) < 0.0f){continue;}
					if (((n_xy_e2 DOT p_xy) + d_xy_e2) < 0.0f){continue;}

					// YZ
					vec2 p_yz = vec2(p[Y],p[Z]);
					if (((n_yz_e0 DOT p_yz) + d_yz_e0) < 0.0f){continue;}
					if (((n_yz_e1 DOT p_yz) + d_yz_e1) < 0.0f){continue;}
					if (((n_yz_e2 DOT p_yz) + d_yz_e2) < 0.0f){continue;}

					// XZ	
					vec2 p_zx = vec2(p[Z],p[X]);
					if (((n_zx_e0 DOT p_zx) + d_xz_e0) < 0.0f){continue;}
					if (((n_zx_e1 DOT p_zx) + d_xz_e1) < 0.0f){continue;}
					if (((n_zx_e2 DOT p_zx) + d_xz_e2) < 0.0f){continue;}


					voxel_data.push_back(VoxelData(t.normal, vec3(0,0,0)));
					voxels[index-morton_start] = voxel_data.size()-1;

					nfilled++;
					continue;

				}
			}
		}
	}
}