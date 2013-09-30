#ifndef VOXELIZATION_H_
#define VOXELIZATION_H_

#include <TriMesh.h>

using namespace std;
using namespace trimesh;

struct VoxelData{
	vec3 normal;
	vec3 color;
	VoxelData(): normal(vec3(0.0f,0.0f,0.0f)), color(vec3(0.0f,0.0f,0.0f)){}
	VoxelData(bool filled, vec3 normal, vec3 color): normal(normal), color(color){}
};

#endif