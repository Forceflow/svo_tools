#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include <TriMesh.h>
#include "geo_primitives.h"

using namespace trimesh;
using namespace std;

// Intersection methods
inline AABox<vec3> computeBoundingBox(const vec3 &v0, const vec3 &v1, const vec3 &v2){
	AABox<vec3> answer = AABox<vec3>(vec3(0.0f,0.0f,0.0f), vec3(0.0f,0.0f,0.0f)); 
	answer.min[0] = min(v0[0],min(v1[0],v2[0]));
	answer.min[1] = min(v0[1],min(v1[1],v2[1]));
	answer.min[2] = min(v0[2],min(v1[2],v2[2]));
	answer.max[0] = max(v0[0],max(v1[0],v2[0]));
	answer.max[1] = max(v0[1],max(v1[1],v2[1]));
	answer.max[2] = max(v0[2],max(v1[2],v2[2]));
	return answer;
}

template <typename T>
inline bool intersectBoxBox(const AABox<T> &a, const AABox<T> &b){
	if(a.max[0] < b.min[0]) {return false;}
	if(a.min[0] > b.max[0]) {return false;}
	if(a.max[1] < b.min[1]) {return false;}
	if(a.min[1] > b.max[1]) {return false;}
	if(a.max[2] < b.min[2]) {return false;}
	if(a.min[2] > b.max[2]) {return false;}
	return true; // intersection or inside
}

#endif /*INTERSECTION_H_*/