# CG Cameras
This project has been made for Graphics Computing (UFPel), and demanded the following tasks to be applied to the camera:
(the already implemented tasks are marked)

- [x] Look at specific position
- [x] Translate
- [x] Rotate around arbitrary axis
- [ ] Rotate around specific point (translate around a point, always showing the same face, like the moon)
- [x] Time dependent transformations
- [x] Be able to control multiple cameras
- [x] Bézier Curve
- [x] b-spline Curve (implemented using catmullRom function)
- [ ] apply some noise (like Perlin's) to the transformations

I used base a project made by our teacher, that has built it based on the learnopengl last tutorials (https://learnopengl.com).


## How to apply Transformations

* NOTE: Each one has a diferent time, and when the time is 0, the camera transforms instantly

- Every time you press [Enter], it creates another camera, with the attributes set on the beginnig of main.cpp;

- If you have more than one camera, you can switch between them pressing [Tab];

- You can press [Q,E,R] if you want to look at one of the positions previously set (I have placed some objects on these position, so I could test the function);

- By pressing [T,Y,U], you are able to move to a specific position, also previously set. By default, these are the same positions of the look at transformation (that means they also have models on them);

- [I,O,P] are for rotating around itself, in diferent axis (some bugs might happen);

- [J,K,L] should be for rotating around point, but it is not working properly;

- [B] button executes a Bézier curve, given four specific point

- [S] button executes a catmullRom based b-spline curve, given four specific point

The following lines are from the original repository, and might be helpful if you want to run the project: 

# learnopengl.com code repository
Contains code samples for all tutorials of [https://learnopengl.com](https://learnopengl.com). 

## Windows building
All relevant libraries are found in /libs and all DLLs found in /dlls (pre-)compiled for Windows. 
The CMake script knows where to find the libraries so just run CMake script and generate project of choice.
Note that you still have to manually copy the required .DLL files from the /dlls folder to your binary folder for the binaries to run.

Keep in mind the supplied libraries were generated with a specific compiler version which may or may not work on your system (generating a large batch of link errors). In that case it's advised to build the libraries yourself from the source.

## Linux building
First make sure you have CMake, Git, and GCC by typing as root (sudo) `apt-get install g++ cmake git` and then get the required packages:
Using root (sudo) and type `apt-get install libsoil-dev libglm-dev libassimp-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev  libxi-dev` .
Next, run CMake (preferably CMake-gui). The source directory is LearnOpenGL and specify the build directory as LearnOpenGL/build. Creating the build directory within LearnOpenGL is important for linking to the resource files (it also will be ignored by Git). Hit configure and specify your compiler files (Unix Makefiles are recommended), resolve any missing directories or libraries, and then hit generate. Navigate to the build directory (`cd LearnOpenGL/build`) and type `make` in the terminal. This should generate the executables in the respective chapter folders.

Note that CodeBlocks or other IDEs may have issues running the programs due to problems finding the shader and resource files, however it should still be able to generate the exectuables. To work around this problem it is possible to set an environment variable to tell the tutorials where the resource files can be found. The environment variable is named LOGL_ROOT_PATH and may be set to the path to the root of the LearnOpenGL directory tree. For example:

    `export LOGL_ROOT_PATH=/home/user/tutorials/LearnOpenGL`

Running `ls $LOGL_ROOT_PATH` should list, among other things, this README file and the resources direcory.

## Mac OS X building
Building on Mac OS X is fairly simple (thanks [@hyperknot](https://github.com/hyperknot)):
```
brew install cmake assimp glm glfw
mkdir build
cd build
cmake ../.
make -j8
```

## Glitter
Polytonic created a project called [Glitter](https://github.com/Polytonic/Glitter) that is a dead-simple boilerplate for OpenGL. 
Everything you need to run a single LearnOpenGL Project (including all libraries) and just that; nothing more. 
Perfect if you want to follow along with the tutorials, without the hassle of having to manually compile and link all third party libraries!
