
#!/bin/bash

## SPECIFY TRIMESH LOCATION HERE (and do a make there first)
TRIMESH_DIR=/home/jeroen/development/trimesh2

## COMPILE AND LINK DEFINITIONS
COMPILE="g++ -c -O3 -g -std=c++11 -I ${TRIMESH_DIR}/include/"
LINK="g++ -o octree2svo"

#############################################################################################
## BUILDING STARTS HERE

## CLEAN
echo "Removing old versions ..."
rm *.o
rm octree2svo

## BUILD BINARY VOXELIZATION VERSION
echo "Compiling..."
${COMPILE} main.cpp
echo "Linking binary voxelization build..."
${LINK} *.o

echo "Cleaning up ..."
rm *.o

echo "Done"
