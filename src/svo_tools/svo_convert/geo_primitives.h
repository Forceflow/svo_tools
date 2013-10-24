#ifndef GEO_PRIMITIVES_H_
#define GEO_PRIMITIVES_H_

struct Triangle {
	vec3 v0;
	vec3 v1;
	vec3 v2;
	vec3 normal;
	Color v0_color;
	Color v1_color;
	Color v2_color;

	// Default constructor
	Triangle(): v0(vec3()), v1(vec3()), v2(vec3()), normal(vec3()), v0_color(Color()),v1_color(Color()),v2_color(Color()){}

	// Constructor with all fields
	Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 normal, Color v0_color, Color v1_color, Color v2_color): v0(v0), v1(v1), v2(v2), normal(normal),
		v0_color(v0_color), v1_color(v1_color),v2_color(v2_color){}
};

template <typename T>
struct AABox {
	T min;
	T max;
	AABox(): min(T()), max(T()){}
	AABox(T min, T max): min(min), max(max){}
};

#endif