#ifndef SVO_CONVERT_H_
#define SVO_CONVERT_H_

#include <vector>
#include <string>
#include <sstream>
#include <TriMesh.h>
#include "svo_convert_util.h"
#include "mesh_operations.h"
#include "voxelization.h"
#include "OctreeBuilder.h"
#include "octree2svo.h"

void convert2svo(string filename, size_t gridsize){
	// Read mesh, calculate bbox and move to origin
	cout << "Loading mesh..." << endl;
	TriMesh* mesh = TriMesh::read(filename.c_str());
	mesh->need_faces(); // unpack triangle strips so we have faces
	mesh->need_bbox(); // compute the bounding box
	mesh->need_normals();
	AABox<vec3> mesh_bbcube = createMeshBBCube(mesh);

	cout << "Moving to origin ..." << endl;
	moveToOrigin(mesh, mesh_bbcube);
	float unitlength = (mesh_bbcube.max[0] - mesh_bbcube.min[0]) / ((float) gridsize);

	// Prepare voxel storage
	cout << "Preparing voxel storage ..." << endl;
	size_t max_index = gridsize*gridsize*gridsize;
	size_t* voxels = new size_t[max_index]; // Array holds 0 if there is no voxel, and an index if there is voxel data
	vector<VoxelData> voxel_data; // Dynamic-sized array holding voxel data
	size_t nfilled = 0;

	// Voxelize
	cout << "Voxelizing ..." << endl;
	voxelize(mesh,gridsize,unitlength,voxels,voxel_data, nfilled);

	// SVO builder
	cout << "Building SVO ..." << endl;
	OctreeBuilder builder = OctreeBuilder(gridsize,false);
	builder.buildOctree(voxels, voxel_data);

	// Free memory
	cout << "Free memory ..." << endl;
	delete voxels; voxel_data.clear();

	// Converting to SVO file
	cout << "Convert to .SVO file" << endl;
	string base_filename = filename.substr(0,filename.find_last_of("."));
	octree2SVO(base_filename,builder.octree_nodes, builder.octree_data);
}

#endif