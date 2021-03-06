CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT ( svo_tools )

add_custom_target(svo_tools SOURCES
./src/svo_tools/svo_convert/DataPoint.h
./src/svo_tools/svo_convert/geo_primitives.h
./src/svo_tools/svo_convert/intersection.h
./src/svo_tools/svo_convert/Node.h
./src/svo_tools/svo_convert/morton.h
./src/svo_tools/svo_convert/mesh_operations.h
./src/svo_tools/svo_convert/octree2svo.h
./src/svo_tools/svo_convert/svo_convert_util.h)

IF (NOT APPLE)
FIND_PACKAGE ( OpenMP REQUIRED )
ENDIF (NOT APPLE)

IF (MSVC)
  ADD_DEFINITIONS ( -openmp )
ELSEIF (NOT APPLE)
  # Linux uses GCC
  ADD_DEFINITIONS ( -fopenmp )
ENDIF (MSVC)

INCLUDE_DIRECTORIES (
  $ENV{TRIMESH2_ROOT}/include
)

IF (APPLE)
LINK_DIRECTORIES (
  $ENV{TRIMESH2_ROOT}/lib.Darwin64
)
ELSEIF (UNIX)
LINK_DIRECTORIES (
  $ENV{TRIMESH2_ROOT}/lib.Linux64
)
ENDIF (APPLE)

ADD_EXECUTABLE ( svo_convert
  ./src/svo_tools/svo_convert/main.cpp
  ./src/svo_tools/svo_convert/OctreeBuilder.cpp
  ./src/svo_tools/svo_convert/voxelization.cpp
)

IF (NOT APPLE)
TARGET_LINK_LIBRARIES ( svo_convert
  trimesh
  gomp
)
ELSE()
TARGET_LINK_LIBRARIES ( svo_convert
  trimesh
)
ENDIF (NOT APPLE)
