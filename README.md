svo_tools
==========
This repository contains tools for working with the SVO file format, the format used in the alpha-stage virtual world developed by Hifi (**[Hifi repo](https://github.com/worklist/hifi "Hifi repo")**).

Currently, this repository contains **svo_convert**, a tool to convert any model file (.ply, .3ds, ...) to an SVO structure, through voxelization and SVO building.

The method is based on **[Out-of-core Sparse Voxel Octree Builder](https://github.com/Forceflow/ooc_svo_builder "ooc_svo_builder github repo")**.

Still very much in active development.

Building
--------
**Dependencies** for building are **[Trimesh2](http://gfx.cs.princeton.edu/proj/trimesh2/)** (used for model file I/O) and OpenMP support.

* **Windows** : Build using *src/svo_tools/svo_tools.sln* VS2012 on Win 64-bit. 32-bit building is not encouraged, but should work.
* **Linux & Mac**: Build using cmake. Make sure you specify the environment variable TRIMESH2_ROOT.

Typical compile on Linux would go:
<pre>
git clone https://github.com/Forceflow/svo_tools
export TRIMESH2_ROOT=/home/jeroen/development/trimesh2/
cd svo_tools
cmake .
make
</pre>

Usage
-----
* **svo_convert** -f */path/to/file.ply* -s *gridsize*

Full option list:
* **-f** */path/to/file.ply* : Path to model file. Currently the file formats supported are those by TriMesh2: .ply, .3ds, .off, .obj.
* **-s** *value* : Gridsize - only powers of 2 up to 1024 are supported: (1,2,4,8,16,32,64,128,256,512,1024).

Examples
--------

* **svo_convert** -f *bunny.ply* -s *512* : Will generate a file named bunny.svo in the same folder, with an SVO of gridsize 512x512x512.
