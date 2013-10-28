#include "OctreeBuilder.h"

OctreeBuilder::OctreeBuilder(size_t gridlength, bool generate_levels) :
	gridlength(gridlength), b_node_pos(0), b_data_pos(0), b_current_morton(0), generate_levels(generate_levels) {
		// Setup building variables
		b_maxdepth = log2((unsigned int) gridlength);
		b_buffers.resize(b_maxdepth+1);
		for(int i = 0; i < b_maxdepth+1; i++){b_buffers[i].reserve(8);}
		b_max_morton = mortonEncode_LUT(int(gridlength-1),int(gridlength-1),int(gridlength-1));
		// Push back first data point (which is always NULL)
		octree_data.push_back(DataPoint());
}

// Finalize the tree: add rest of empty nodes, make sure root node is on top
void OctreeBuilder::finalizeTree(){
	// fill octree
	if(b_current_morton < b_max_morton){
		fastAddEmpty((b_max_morton - b_current_morton)+1);
	}
	// write root node
	octree_nodes.push_back(b_buffers[0][0]);
}

// Group 8 nodes, write non-empty nodes to disk and create parent node
Node OctreeBuilder::groupNodes(const vector<Node> &buffer){
	Node parent = Node();
	bool first_stored_child = true;
	for(int k = 0; k<8; k++){
		if(!buffer[k].isNull()){
			if(first_stored_child){
				octree_nodes.push_back(buffer[k]);
				parent.children_base = octree_nodes.size()-1;
				parent.children_offset[k] = 0;
				first_stored_child = false;
			} else {
				octree_nodes.push_back(buffer[k]);
				parent.children_offset[k] = (octree_nodes.size()-1) - parent.children_base;
			}
		} else {
			parent.children_offset[k] = NOCHILD;
		}
	}

	// SIMPLE LEVEL CONSTRUCTION
	if(generate_levels){
		DataPoint d = DataPoint();
		float notnull = 0.0f;
		for(int i = 0; i < 8; i++){ // this node has no data: need to refine
			if(!buffer[i].isNull())
				notnull++;
			d.opacity += buffer[i].data_cache.opacity;
			d.color += buffer[i].data_cache.color;
			d.normal += buffer[i].data_cache.normal;
		}
		d.color = d.color / notnull;
		vec3 tonormalize = (vec3) (d.normal / notnull);
		d.normal = normalize(tonormalize);
		d.opacity = d.opacity / notnull;
		// set it in the parent node
		octree_data.push_back(d);
		parent.data = octree_data.size()-1;
		parent.data_cache = d;
	}

	return parent;
}

// Add an empty datapoint at a certain buffer level, and refine upwards from there
void OctreeBuilder::addEmptyDataPoint(const int buffer){
	b_buffers[buffer].push_back(Node());
	// REFINE BUFFERS: check from touched buffer, upwards
	for(int d = buffer; d >= 0; d--){
		if(b_buffers[d].size() == 8){ // if we have 8 nodes
			assert(d-1 >= 0);
			if(isBufferEmpty(b_buffers[d])){
				b_buffers[d-1].push_back(Node()); // push back NULL node to represent 8 empty nodes
			} else { 
				b_buffers[d-1].push_back(groupNodes(b_buffers[d])); // push back parent node
			}
			b_buffers.at(d).clear(); // clear the 8 nodes on this level
		} else {
			break; // break the for loop: no upper levels will need changing
		}
	}
	b_current_morton = b_current_morton + pow(8.0,b_maxdepth-buffer); // because we're adding at a certain level
}

// Add a datapoint to the octree: this is the main method used to push datapoints
void OctreeBuilder::addDataPoint(const uint64_t morton_number, const DataPoint& data_point){
	// PADDING FOR MISSED MORTON NUMBERS
	if(morton_number != b_current_morton){
		fastAddEmpty(morton_number - b_current_morton);
	}

	// ADD NODE TO BUFFER
	Node node = Node(); // create empty node
	if(!data_point.isEmpty()) {
		octree_data.push_back(data_point);
		node.data = octree_data.size()-1;
		node.data_cache = data_point; // store data as cache
	} 
	b_buffers.at(b_maxdepth).push_back(node);

	// REFINE BUFFERS: check all levels (bottom up) and group 8 nodes on a higher level
	for(int d = b_maxdepth; d >= 0; d--){
		if(b_buffers[d].size() == 8){ // if we have 8 nodes
			if(isBufferEmpty(b_buffers[d])){
				b_buffers[d-1].push_back(Node()); // push back NULL node to represent 8 empty nodes
			} else { 
				b_buffers[d-1].push_back(groupNodes(b_buffers[d])); // push back parent node
			}
			b_buffers.at(d).clear(); // clear the 8 nodes on this level
		} else {
			break; // break the for loop: no upper levels will need changing
		}
	}

	// INCREASE CURRENT MORTON NUMBER
	b_current_morton++;
}