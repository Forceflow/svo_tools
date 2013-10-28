#ifndef SVO_CONVERT_UTIL_H_
#define SVO_CONVERT_UTIL_H_

using namespace std;
using namespace trimesh;

template <size_t D, class T>
inline Vec<D,T> average3Vec(const Vec<D,T> v0, const Vec<D,T> v1, const Vec<D,T> v2){
	Vec<D,T> answer;
	for (size_t i = 0; i < D; i++){
		answer[i] = (v0[i] + v1[i] + v2[i]) / 3.0f;
	}
	return answer;
}

template <typename T> T clampval(const T& value, const T& low, const T& high) {
  return value < low ? low : (value > high ? high : value); 
}

inline int findPowerOf8(size_t n){
	if(n == 0){return 0;}
	int highest_index = 0;
	while(n >>= 1){
		highest_index++;
	}
	return (highest_index/3);
}

inline unsigned int log2(unsigned int val) {
	unsigned int ret = -1;
	while (val != 0) {
		val >>= 1;
		ret++;
	}
	return ret;
}

inline int isPowerOf2(unsigned int x){
  return ((x != 0) && !(x & (x - 1)));
}

#endif // SVO_CONVERT_UTIL_H_