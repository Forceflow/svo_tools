# svo_tools
This repository contains tools for working with the SVO file format, the format used in the alpha-stage virtual world developed by Hifi (**[Hifi repo](https://github.com/worklist/hifi "Hifi repo")**).

Currently, this repository contains: 
 * **svo_convert**, a command-line tool to convert any model file (.ply, .3ds, ...) to an SVO structure, through voxelization and SVO building.
 * **octree2svo**, a command-line tool to convert octrees generated using my **[Out-of-core Sparse Voxel Octree Builder](https://github.com/Forceflow/ooc_svo_builder "ooc_svo_builder github repo")** to .svo files.

All methods are based on **[Out-of-core Sparse Voxel Octree Builder](https://github.com/Forceflow/ooc_svo_builder "ooc_svo_builder github repo")**.
Still very much in active development.

## Binaries
I put up some precompiled binaries for Windows, Linux and OSX on the **[releases page](https://github.com/Forceflow/svo_tools/releases)**

# svo_convert

## svo_convert Building
**The only dependencies** for building is **[TriMesh2](http://gfx.cs.princeton.edu/proj/trimesh2/)** (used for model file I/O). The Trimesh2 distribution comes with pre-built binaries for all platforms, so just go ahead and **[download](http://gfx.cs.princeton.edu/proj/trimesh2/)** it, and unzip it to a location you remember.
OpenMP support is optional, and is disabled for OSX.

### Windows
Build using *src/svo_tools/svo_tools.sln* VS2012 on Win 64-bit. 32-bit building is not encouraged, but should work. You can grab a free version of Visual Studio Express **[here](http://www.microsoft.com/visualstudio/eng/downloads)**. The TriMesh2 libraries are built against MinGW, so when you're using the VS compiler, you have to rebuild the TriMesh2 library. This can be easily done using the MSVC project **[here](http://gfx.cs.princeton.edu/proj/trimesh2/src/trimesh2-2.11-MSVC.zip)**.

### Linux
Build using gcc and cmake. Make sure you specify the environment variable TRIMESH2_ROOT.

A typical compile on Linux would go like this:
<pre>
git clone https://github.com/Forceflow/svo_tools
export TRIMESH2_ROOT=/home/jeroen/development/trimesh2/
cd svo_tools
cmake .
make
</pre>

### OSX
We'll be using some tools which are available in the Apple-provided *Command Line Tools for Xcode*, which can be downloaded from within Xcode (like described **[here](http://stackoverflow.com/questions/9353444/how-to-use-install-gcc-on-mac-os-x-10-8-xcode-4-4 here)**) or as a **[stand-alone download](https://developer.apple.com/downloads/)**.

#### Compiling TriMesh2 with clang
In order to be able to use TriMesh2 with the default Apple clang compiler, we have to recompile the GCC-built binaries that came with it.
To do this, we have to disable OpenMP support, because clang doesn't have it.

The following steps are required:
* Download the latest TriMesh2 zip from the **[TriMesh2 page](http://gfx.cs.princeton.edu/proj/trimesh2/)** and unzip it.
* Navigate to your TriMesh2 folder
* Open the file *MakeDefs.Darwin64* and remove the *-fopenmp* switch from *ARCHOPTS*
* In the TriMesh2 folder, run *make*

This should recompile TriMesh2 with clang. If you cannot complete the steps in this paragraph, I've prepared a clang-compiled TriMesh2 version for download **[here](http://www.forceflow.be/temp/trimesh2-2.12-clang_version.zip)**.

#### Compiling 
The only thing left to do is to set the TRIMESH2_ROOT environment variable to point to your local TriMesh2 folder.

A typical compile on OSX would go like this:
<pre>
git clone https://github.com/Forceflow/svo_tools
export TRIMESH2_ROOT=/Users/jeroen/development/trimesh2/
cd svo_tools
cmake .
make
</pre>

svo_convert Usage
-----------------
<pre>
svo_convert -f /path/to/file.ply -s (gridsize) -c (color mode)
</pre>

Full option list:
* **-f** */path/to/file.ply* : Path to model file. Currently the file formats supported are those by TriMesh2: .ply, .3ds, .off, .obj.
* **-s** *value* : Gridsize - only powers of 2 up to 1024 are supported: (1,2,4,8,16,32,64,128,256,512,1024).
* **-c** *colormode* : Colormode -Options :
    * *model* : Tries to grab colors from vertices. If the mesh has no colored vertices, it falls back to fixed color.
    * *fixed* : All voxels get a fixed color (white).
    * *normal* : Voxels get colored according to the normals of the mesh.

svo_convert Examples
--------------------

<pre>
svo_convert -f /home/jeroen/models/bunny.ply -s 512
</pre>
Will generate a file named bunny.svo in the same folder, with an SVO of gridsize 512x512x512. It will use the default color mode, trying to fetch colors from the vertices, and failing that, will color the model white.

<pre>
svo_convert -f /home/jeroen/models/horse.ply -s 256 -c normal
</pre>
Will generate a file named horse.svo in the same folder, with an SVO of gridsize 256x256x256. It will use the normal-based color mode, in which each voxel gets colored according to the mesh face normal.

#octree2svo

octree2svo Building
-------------------

* **Windows** : Build using VS2012 on Win 64-bit. 32-bit building is not encouraged.
* **Linux** : Build using build_linux.sh.

**Dependencies** for building are **[Trimesh2](http://gfx.cs.princeton.edu/proj/trimesh2/)** and OpenMP support. You can specify the location of the Trimesh2 library in both the VS solution file and the linux build script.

octree2svo Usage
----------------

* **octree2svo** -f /path/to/file.octree

Full option list:
* **-f** */path/to/file.octree* : Path to *.octree* file. The *.octreenodes* and *.octreedata* files should be in the same folder.
* **-c** *color_mode* : (color_mode can be: *fixed*, *from_octree*) Specify the mode in which voxels should be colored. Default is a fixed white color, but you can also force octree2svo to use the colors embedded in the *.octree* file format by using switch *from_octree*.
* **-h** : Print help and exit

octree2svo Examples
-------------------

So the full pipeline (including tools from [ooc_svo_builder](https://github.com/Forceflow/ooc_svo_builder)) to get from a model file in .ply, obj or .3ds format to an .svo file might look like this if you want a bunny with fixed colors:

* **tri_convert_binary** -f *bunny.ply*
* **svo_builder_binary** -f *bunny.tri* -s *1024*
* **octree2svo** -f *bunny1024_1.octree*

And if you want a bunny with voxel colors from the sampled normals:

* **tri_convert** -f *bunny.ply*
* **svo_builder** -f *bunny.tri* -s *1024* -c *normal*
* **octree2svo** -f *bunny1024_1.octree* -c *from_octree*

Each of these tools has more configuration options, please refer to the documentation.
