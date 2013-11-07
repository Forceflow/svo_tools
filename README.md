svo_tools
==========
This repository contains tools for working with the SVO file format, the format used in the alpha-stage virtual world developed by Hifi (**[Hifi repo](https://github.com/worklist/hifi "Hifi repo")**).

Currently, this repository contains **svo_convert**, a tool to convert any model file (.ply, .3ds, ...) to an SVO structure, through voxelization and SVO building.

The method is based on **[Out-of-core Sparse Voxel Octree Builder](https://github.com/Forceflow/ooc_svo_builder "ooc_svo_builder github repo")**.

Still very much in active development.

Building
--------

* **Windows** : Build using VS2012 on Win 64-bit. 32-bit building is not encouraged, but should work.
* **Linux** : Build using build_svo_convert.sh.

**Dependencies** for building are **[Trimesh2](http://gfx.cs.princeton.edu/proj/trimesh2/)** and OpenMP support. You can specify the location of the Trimesh2 library in both the VS solution file and the linux build script.

Usage
-----

* **svo_convert** -f /path/to/file.ply

Full option list:
* **-f** */path/to/file.ply* : Path to model file. Currently the file formats supported are those by TriMesh2: .ply, .3ds, .off, .obj.
* **-s** *<value>* : Gridsize - only powers of 2 up to 1024 are supported: (1,2,4,8,16,32,64,128,256,512,1024).

Examples
--------

* **svo_convert** -f *bunny.ply* -s *512* : Will generate a file named bunny.svo in the same folder, with an SVO of gridsize 512x512x512.
