#ifndef VOXELIZATION_H_
#define VOXELIZATION_H_

#include <TriMesh.h>
#include "svo_convert_util.h"
#include "geo_primitives.h"
#include "morton.h"
#include "intersection.h"

using namespace std;
using namespace trimesh;

#define EMPTY_VOXEL 0
typedef Vec<3, unsigned int> uivec3;

// Struct to hold collected voxel data
struct VoxelData{
	vec3 normal;
	vec3 color;
	VoxelData(): normal(vec3(0.0f,0.0f,0.0f)), color(vec3(0.0f,0.0f,0.0f)){}
	VoxelData(bool filled, vec3 normal, vec3 color): normal(normal), color(color){}
};

void voxelize(const TriMesh* mesh, size_t gridsize, float unitlength, size_t* voxels, vector<VoxelData>& voxel_data, size_t& nfilled);

#endif