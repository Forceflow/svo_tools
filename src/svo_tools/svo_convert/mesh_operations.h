#ifndef MESH_OPERATIONS_H_
#define MESH_OPERATIONS_H_

#include <TriMesh.h>
#include "geo_primitives.h"

using namespace std;
using namespace trimesh;

// create bounding cube around a mesh (pad if the bbox is not a cube)
inline AABox<vec3> createMeshBBCube(const TriMesh* mesh){
	vec3 mesh_min = mesh->bbox.min;
	vec3 mesh_max = mesh->bbox.max;
	vec3 lengths = mesh_max-mesh_min;
	for(int i=0; i<3;i++){
		float delta = lengths.max() - lengths[i];
		if(delta != 0){
			mesh_min[i] = mesh_min[i] - (delta / 2.0f);
			mesh_max[i] = mesh_max[i] + (delta / 2.0f);
		}
	}
	return AABox<vec3>(mesh_min,mesh_max);
}

inline void moveToOrigin(TriMesh* mesh, AABox<vec3> bounding_box){
	for(size_t i = 0; i < mesh->vertices.size() ; i++){
		mesh->vertices[i] = mesh->vertices[i] - bounding_box.min;
	}
}

#endif