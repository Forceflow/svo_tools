CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT ( svo_tools )

#FIND_PACKAGE ( OpenMP REQUIRED )

IF (MSVC)
  ADD_DEFINITIONS ( -openmp )
ELSE ()
  # Apple and Linux both uses GCC
  ADD_DEFINITIONS ( -fopenmp )
ENDIF (MSVC)

INCLUDE_DIRECTORIES (
  $ENV{TRIMESH2_ROOT}/include
)

LINK_DIRECTORIES (
# So it's linux
IF(UNIX && !APPLE)
  $ENV{TRIMESH2_ROOT}/lib.Linux64
ENDIF(UNIX && !APPLE)
#So it's OSX
IF(UNIX && APPLE)
  $ENV{TRIMESH2_ROOT}/lib.Darwin64
ENDIF(UNIX && APPLE)
)

ADD_EXECUTABLE ( svo_convert
  ./src/svo_tools/svo_convert/main.cpp
  ./src/svo_tools/svo_convert/OctreeBuilder.cpp
  ./src/svo_tools/svo_convert/voxelization.cpp
 )

TARGET_LINK_LIBRARIES ( svo_convert
  trimesh
  gomp
)
