## SPECIFY TRIMESH LOCATION HERE (and do a make there first)
TRIMESH_DIR=/home/jeroen/development/trimesh2

## COMPILE AND LINK DEFINITIONS
COMPILE="g++ -c -O3 -I ${TRIMESH_DIR}/include/"
LINK="g++ -o svo_convert"
LINK_OPTS="-L${TRIMESH_DIR}/lib.Linux64 -ltrimesh -fopenmp -static"

#############################################################################################

## CLEAN
echo "Cleaning old versions ..."
rm *.o
rm svo_convert
rm a.out

## BUILD BINARY VOXELIZATION VERSION
echo "Building..."
${COMPILE} main.cpp
${LINK} main.o ${LINK_OPTS}

echo "Done"
