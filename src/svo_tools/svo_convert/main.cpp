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
size_t gridsize = 512;
bool verbose = true;

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

	cout << "Loading mesh..." << endl;
	// Read mesh, calculate bbox and move to origin
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

}