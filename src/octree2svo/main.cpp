#define _CRT_SECURE_NO_DEPRECATE // so yeah, no fopen_s (it's not portable)
#include <string>
#include <iostream>
#include <bitset>
#include "octree_io.h"
#include "Node.h"
#include "DataPoint.h"

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

using namespace std;
using namespace trimesh;

enum ColorMode {COLOR_FIXED, COLOR_FROMOCTREE};

// Program version
string version = "1.0";

// Program parameters
string filename = "";
ColorMode color_mode = COLOR_FIXED; // by default, just use a fixed color
ivec3 fixed_color = ivec3(255,255,255);

// Program data
Node* nodes;
DataPoint* data;
FILE* svo_out;

void printInfo() {
	cout << "--------------------------------------------------------------------" << endl;
	cout << "octree2svo v" << version << endl;
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
	cout << "--------------------------------------------------------------------" << endl << endl;
}

void printHelp() {
	std::cout << "Example: octree2svo -f /home/jeroen/bunny.octree" << endl;
	std::cout << "" << endl;
	std::cout << "All available program options:" << endl;
	std::cout << "" << endl;
	std::cout << "-f <filename.tri>     Path to a .octree input file." << endl;
	std::cout << "-c <color_mode>       Color mode. Options: fixed (default), from_octree" << endl;
	std::cout << "-h                    Print help and exit." << endl;
}

void printInvalid() {
	std::cout << "Not enough or invalid arguments, please try again." << endl;
	std::cout << "At the bare minimum, I need a path to an .octree file" << endl<< "" << endl;
	printHelp();
}

void parseParameters(int argc, char* argv[], string& filename){
	cout << "Reading program parameters ..." << endl;
	// Input argument validation
	if(argc < 3){printInvalid(); exit(0);}
	for (int i = 1; i < argc; i++) {
		// parse filename
		if (string(argv[i]) == "-f") {
			filename = argv[i + 1]; 
			size_t check_tri = filename.find(".octree");
			if(check_tri == string::npos){
				cout << "Data filename does not end in .trip - I only support that file format" << endl; printInvalid();exit(0);
			}
			i++;
		} else if (string(argv[i]) == "-c") {
			string color_input = string(argv[i+1]);
			if(color_input == "fixed") {
				color_mode = COLOR_FIXED;
			} else if(color_input == "from_octree") { 
				color_mode = COLOR_FROMOCTREE;
			} else {
				cout << "Unrecognized color switch: " << color_input << ", so reverting to fixed color." << endl;
			}
			i++;
		} else if (string(argv[i]) == "-h") {
			printHelp(); exit(0);
		} else {
			printInvalid(); exit(0);
		}
	}
	cout << "  Filename: " << filename << endl;
}

void parseNode(const Node &node){
	// create bitsets we'll write out 
	bitset<8> child_tree_exists;
	bitset<8> child_colored;

	child_tree_exists.reset();
	child_colored.reset();

	for(int i = 0; i < 8; i++) {
		if(node.hasChild(7-i)){ // if this node has a child at position i
			if(! nodes[node.getChildPos(7-i)].isLeaf()){ // if it is not a leaf, there's a tree 
				child_tree_exists.set(i, true);
			} else {
				child_colored.set(i, true); // it's a leaf child: color it
			}
		} 
	}

	// write these to file: convert bitset to unsigned char to write to file
	unsigned char child_tree_exists_byte = (unsigned char) child_tree_exists.to_ulong();
	unsigned char child_colored_byte = (unsigned char) child_colored.to_ulong();

	// write child_colored byte
	fwrite(& child_colored_byte, sizeof(unsigned char), 1, svo_out);

	// write color for each child (in RGB)
	for(int i = 7; i >= 0; i--){ // we write them left to right in the byte order
		if(child_colored[i]){
			unsigned char r = (unsigned char) fixed_color[0];
			unsigned char g = (unsigned char) fixed_color[1];
			unsigned char b = (unsigned char) fixed_color[2];
			if(color_mode == COLOR_FROMOCTREE){
				DataPoint dpoint = data[nodes[node.getChildPos(7-i)].data]; // fetch datapoint
				r = (unsigned char) (dpoint.color[0] * 255.0f);
				g = (unsigned char) (dpoint.color[1] * 255.0f);
				b = (unsigned char) (dpoint.color[2] * 255.0f);
			}
			fwrite(& r, sizeof(unsigned char), 1, svo_out);
			fwrite(& g, sizeof(unsigned char), 1, svo_out);
			fwrite(& b, sizeof(unsigned char), 1, svo_out);
		}
	}

	// write tree exists bytes
	fwrite(& child_tree_exists_byte, sizeof(unsigned char), 1, svo_out);

	// start writing child nodes (recursively)
	for(int i = 7; i >= 0; i--) { // we write them left to right in the byte order
		if(child_tree_exists[i]){
			parseNode(nodes[node.getChildPos(7-i)]);
		}
	}
}

int main(int argc, char *argv[]){
	printInfo();

	// Parse program parameters
	parseParameters(argc,argv,filename);

	// Get octree info
	OctreeInfo tree_info;
	parseOctreeHeader(filename,tree_info);
	tree_info.print();
	/*if(!tree_info.filesExist()){
		cout << "Not all required octree files (.octree, .octreenodes, .octreedata) exist. Regenerate them with svo_builder." << endl;
		exit(0);
	}*/

	// allocate memory for nodes and read them
	cout << "Allocating " << (tree_info.n_nodes * sizeof(Node))/1024/1024 << " Mb for Nodes" << endl;
	nodes = new Node[tree_info.n_nodes];
	string nodes_file = tree_info.base_filename + string(".octreenodes");
	FILE* f_nodes = fopen(nodes_file.c_str(), "rb");
	for(size_t i = 0; i < tree_info.n_nodes; i++){
		Node node = Node();
		readNode(f_nodes,node);
		nodes[i] = node;
	}
	fclose(f_nodes);

	// allocate memory for data points and read them
	cout << "Allocating " << (tree_info.n_data * sizeof(DataPoint))/1024/1024 << " Mb for Data" << endl;
	data = new DataPoint[tree_info.n_data];
	string data_file = tree_info.base_filename + string(".octreedata");
	FILE* f_data = fopen(data_file.c_str(), "rb");
	for(size_t i = 0; i < tree_info.n_data; i++){
		DataPoint datapoint = DataPoint();
		readDataPoint(f_data,datapoint);
		data[i] = datapoint;
	}
	fclose(f_data);

	// open output file
	string svo_file = tree_info.base_filename + string(".svo");
	svo_out = fopen(svo_file.c_str(), "wb");

	// Write octal code for root node (all zero)
	unsigned char zero = 0;
	fwrite(& zero, sizeof(unsigned char), 1, svo_out);

	// open root node and start parsing recursively
	parseNode(nodes[tree_info.n_nodes-1]); // parse root node
	fclose(svo_out);
}
