#include <TriMesh.h>
#include <vector>
#include <string>
#include <sstream>
#include "svo_convert_util.h"
#include "mesh_operations.h"
#include "voxelization.h"

using namespace std;
using namespace trimesh;

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Program version
string version = "1.0";

// Program parameters
string filename = "";
size_t gridsize = 1024;
bool verbose = true;

// Program variables
TriMesh* mesh;

void printInfo(){
	cout << "-------------------------------------------------------------" << endl;
	cout << "Model to .SVO converter " << version << endl;
#ifdef _WIN32 || _WIN64
	cout << "Windows ";
#endif
#ifdef __linux__
	cout << "Linux ";
#endif
#ifdef ENVIRONMENT64
	cout << "64-bit version" << endl;
#endif
#ifdef ENVIRONMENT32
	cout << "32-bit version" << endl;
#endif
	cout << "Jeroen Baert - jeroen.baert@cs.kuleuven.be - www.forceflow.be" << endl;
	cout << "-------------------------------------------------------------" << endl << endl;
}

void printHelp(){
	std::cout << "Example: "<< endl;
}

void printInvalid(){
	std::cout << "Not enough or invalid arguments, please try again.\n" << endl; 
	printHelp();
}

void parseProgramParameters(int argc, char* argv[]){
	// Input argument validation
	if(argc<3){ // not enough arguments
		printInvalid(); exit(0);
	} 
	for (int i = 1; i < argc; i++) {
		if (string(argv[i]) == "-f") {
			filename = argv[i + 1]; 
			i++;
		} else if (string(argv[i]) == "-s") {
			gridsize = atoi(argv[i + 1]);
			if (!isPowerOf2(gridsize)) {
				cout << "Requested gridsize is not a power of 2" << endl; printInvalid(); exit(0);
			}
			i++;
		} else {
			printInvalid(); exit(0);
		}
	}
	if(verbose){
		cout << "  filename: " << filename << endl;
		cout << "  gridsize: " << gridsize << endl;
	}
}

int main(int argc, char *argv[]){
	printInfo();

	// Parse parameters
	parseProgramParameters(argc,argv);

	// Read mesh, calculate bbox and move to origin
	mesh = TriMesh::read(filename.c_str());
	mesh->need_faces(); // unpack triangle strips so we have faces
	mesh->need_bbox(); // compute the bounding box
	mesh->need_normals();
	AABox<vec3> mesh_bbcube = createMeshBBCube(mesh);
	moveToOrigin(mesh, mesh_bbcube);

	// Prepare voxel storage
	size_t* voxels = new size_t[gridsize*gridsize*gridsize]; // Array holds 0 if there is no voxel, and an index if there is voxel data
	vector<VoxelData> voxel_data; // Dynamic-sized array holding voxel data
	voxel_data.push_back(VoxelData()); // first voxel_data is empty





//
//	// Write mesh to format we can stream in
//	string base = filename.substr(0,filename.find_last_of("."));
//	std::string tri_header_out_name = base + string(".tri");
//	std::string tri_out_name = base + string(".tridata");
//
//	FILE* tri_out = fopen(tri_out_name.c_str(), "wb");
//
//	cout << "Writing mesh triangles ... "; timer.reset();
//	Triangle t;
//	// Write all triangles to data file
//	for(size_t i = 0; i < themesh->faces.size(); i++){
//		t.v0 = themesh->vertices[themesh->faces[i][0]];
//		t.v1 = themesh->vertices[themesh->faces[i][1]];
//		t.v2 = themesh->vertices[themesh->faces[i][2]];
//#ifndef BINARY_VOXELIZATION
//		// COLLECT VERTEX COLORS
//		// TODO: Make this user-switchable (in case you want to ignore colors from mesh)
//		if(!themesh->colors.empty()){ // if this mesh has colors, we're going to use them
//			t.v0_color = themesh->colors[themesh->faces[i][0]];
//			t.v1_color = themesh->colors[themesh->faces[i][1]];
//			t.v2_color = themesh->colors[themesh->faces[i][2]];
//		} 
//		// COLLECT NORMALS
//		if(recompute_normals){
//			t.normal = computeFaceNormal(themesh,i); // recompute normals
//		} else {
//			t.normal = getShadingFaceNormal(themesh,i); // use mesh provided normals
//		}
//#endif
//		writeTriangle(tri_out,t);
//	}
//	cout << "done in " << timer.getTotalTimeSeconds() << " ms." << endl;
//
//	// Prepare tri_info and write header
//	cout << "Writing header to " << tri_header_out_name << " ... " << endl;
//	TriInfo tri_info;
//	tri_info.version = 1;
//	tri_info.mesh_bbox = mesh_bbox;
//	tri_info.n_triangles = themesh->faces.size();
//#ifdef BINARY_VOXELIZATION
//	tri_info.geometry_only = 1;
//#else
//	tri_info.geometry_only = 0;
//#endif
//	writeTriHeader(tri_header_out_name, tri_info);
//	tri_info.print();
//	cout << "Done." << endl;
}