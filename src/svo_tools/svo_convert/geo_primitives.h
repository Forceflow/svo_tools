#ifndef GEO_PRIMITIVES_H_
#define GEO_PRIMITIVES_H_

template <typename T>
struct AABox {
	T min;
	T max;
	AABox(): min(T()), max(T()){}
	AABox(T min, T max): min(min), max(max){}
};

#endif