#ifndef OCTREE_BUILDER_H_
#define OCTREE_BUILDER_H_

#include <stdio.h>
#include <fstream>
#include <vector>
#include <assert.h>
#include "morton.h"
#include "Node.h"
#include "svo_convert_util.h"
#include "voxelization.h"

using namespace std;

// Octreebuilder class. You pass this class DataPoints, it builds an octree from them.
class OctreeBuilder {
public:
	vector< vector< Node > > b_buffers;
	size_t gridlength;
	int b_maxdepth; // maximum octree depth
	uint64_t b_current_morton; // current morton position
	uint64_t b_max_morton; // maximum morton position
	size_t b_data_pos; // current output data position (array index)
	size_t b_node_pos; // current output node position (array index)

	vector<Node> octree_nodes;
	vector<DataPoint> octree_data;

	// configuration
	bool generate_levels; // switch to enable basic generation of higher octree levels

	OctreeBuilder(size_t gridlength, bool generate_levels);
	void buildOctree(char* voxels, vector<VoxelData>& data);

private:
	void addDataPoint(const uint64_t morton_number, const DataPoint& point);
	void finalizeTree();
	bool isBufferEmpty(const vector<Node> &buffer);
	Node groupNodes(const vector<Node> &buffer);
	void addEmptyDataPoint(const int buffer);
	int highestNonEmptyBuffer();
	int computeBestFillBuffer(const size_t budget);
	void fastAddEmpty(const size_t budget);
};

// Read voxels and data from arrays and build the SVO
inline void OctreeBuilder::buildOctree(char* voxels, vector<VoxelData>& data){
	sort(data.begin(), data.end()); // sort voxel data
	for (std::vector<VoxelData>::iterator it = data.begin(); it != data.end(); ++it){
		DataPoint d = DataPoint();
		d.opacity = 1.0;
		d.normal = it->normal;
		d.color = it->color;
		addDataPoint(it->morton, d);
	}
	finalizeTree();
}

// Check if a buffer contains non-empty nodes
inline bool OctreeBuilder::isBufferEmpty(const vector<Node> &buffer){
	for(int k = 0; k<8; k++){
		if(!buffer[k].isNull()){
			return false;
		}
	}
	return true;
}

// Find the highest non empty buffer, return its index
inline int OctreeBuilder::highestNonEmptyBuffer(){
	int highest_found = b_maxdepth; // highest means "lower in buffer id" here.
	for(int k = b_maxdepth; k>=0; k--){
		if(b_buffers[k].size() == 0){ // this buffer level is empty
			highest_found--;
		} else { // this buffer level is nonempty: break
			return highest_found;
		}
	}
	return highest_found;
}

// Compute the best fill buffer given the budget
inline int OctreeBuilder::computeBestFillBuffer(const size_t budget){
	// which power of 8 fits in budget?
	int budget_buffer_suggestion = b_maxdepth-findPowerOf8(budget);
	// if our current guess is already b_maxdepth, return that, no need to test further
	if(budget_buffer_suggestion == b_maxdepth){return b_maxdepth;}
	// best fill buffer is maximum of suggestion and highest non_empty buffer
	return max(budget_buffer_suggestion, highestNonEmptyBuffer());
}

// A method to quickly add empty nodes
inline void OctreeBuilder::fastAddEmpty(const size_t budget){
	size_t r_budget = budget;
	while (r_budget > 0){
		int buffer = computeBestFillBuffer(r_budget);
		addEmptyDataPoint(buffer);
		size_t budget_hit = (size_t) pow(8.0,b_maxdepth-buffer);
		r_budget = r_budget - budget_hit;
	}
}

#endif  // OCTREE_BUILDER_H_