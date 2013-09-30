#ifndef SVO_CONVERT_UTIL_H_
#define SVO_CONVERT_UTIL_H_

using namespace std;

inline int isPowerOf2(unsigned int x){
  return ((x != 0) && !(x & (x - 1)));
}

#endif // SVO_CONVERT_UTIL_H_