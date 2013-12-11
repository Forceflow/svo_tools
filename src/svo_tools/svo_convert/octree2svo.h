#ifndef OCTREE2SVO_H_
#define OCTREE2SVO_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <bitset>
#include "Node.h"
#include "DataPoint.h"

using namespace std;

// config and convert2svo stuff
FILE* svo_out;

inline void parseNode(const Node &node, const vector<Node> &nodes, const vector<DataPoint> &data){
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
			DataPoint dpoint = data[nodes[node.getChildPos(7-i)].data]; // fetch datapoint
			unsigned char r = (unsigned char) (dpoint.color[0] * 255.0f);
			unsigned char g = (unsigned char) (dpoint.color[1] * 255.0f);
			unsigned char b = (unsigned char) (dpoint.color[2] * 255.0f);
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
			parseNode(nodes[node.getChildPos(7-i)], nodes, data);
		}
	}
}

inline void octree2SVO(string filename, const vector<Node> &nodes, const vector<DataPoint> &data){
	// open output file
	string svo_file = filename + string(".svo");
	svo_out = fopen(svo_file.c_str(), "wb");

	// Write octal code for root node (all zero)
	unsigned char zero = 0;
	fwrite(& zero, sizeof(unsigned char), 1, svo_out);

	// open root node and start parsing recursively
	parseNode(nodes[(nodes.size()-1)], nodes,data); // parse root node
	fclose(svo_out);
	cout << "  written to " << svo_file << endl;
}

#endif