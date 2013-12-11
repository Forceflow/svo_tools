#ifndef VOXELIZATION_H_
#define VOXELIZATION_H_

#include <TriMesh.h>
#include "svo_convert_util.h"
#include "geo_primitives.h"
#include "morton.h"
#include "intersection.h"

using namespace std;
using namespace trimesh;

#define EMPTY_VOXEL 0 // char 0 means empty voxel
#define FULL_VOXEL 1 // char 1 means filled voxel

// Voxel color options
enum ColorMode {COLOR_FROM_MODEL, COLOR_FIXED, COLOR_NORMAL};
extern vec3 FIXED_COLOR;

// Struct to hold collected voxel data
// This struct defines VoxelData for our voxelizer - this is defined different depending on compile project
// This is the main memory hogger: the less data you store here, the better.
struct VoxelData{
	uint64_t morton;
	vec3 normal;
	vec3 color;
	VoxelData() : morton(0), normal(vec3(0.0f, 0.0f, 0.0f)), color(vec3(0.0f, 0.0f, 0.0f)){}
	VoxelData(uint64_t morton, vec3 normal, vec3 color) : morton(morton), normal(normal), color(color){}

	bool operator >(VoxelData &a){
		return morton > a.morton;
	}

	bool operator <(VoxelData &a){
		return morton < a.morton;
	}
};

void voxelize(const TriMesh* mesh, size_t gridsize, float unitlength, ColorMode c, char* voxels, vector<VoxelData>& data, size_t& nfilled);

#endif