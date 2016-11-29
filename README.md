Compiling
=========
Create a build folder inside the source folder and run cmake from there to
generate the proper files for your target build system.

Supported Compilers
===================
- Visual Studio 2015
- GCC
- Clang

Compilation Requirements
========================
- 64-bits LunarG Vulkan SDK

Compilation Instructions
========================
Linux
-----
Configure the project:
```
$ mkdir build
$ cd build
$ cmake ..
```

Then compile it:
```
$ make
```

Windows / NMake x64
-------
Open the VS2015 x64 Native Tools Command Prompt and navigate to the source
folder. Run the following commands to configure the project:
```
> mkdir build
> cd build
> cmake -G “NMake Makefiles” ..
```

Then compile it:
```
> nmake
```

Windows / Visual Studio
-------
Configure the project:
```
> mkdir build
> cd build
> cmake -G “Visual Studio 14 2015 Win64” ..
```

A .sln file will be generated in the build folder, you can run it and compile
the project.

Attention: don’t forget to change the configuration to “Release” if you’re
building a release build.

Debug Build
===========
With IDE’s like Visual Studio, the Debug build type is generated automatically
along with the Release build type. However, with Makefile-like generators, to
choose the Debug build type instead of Release you have to specify it explicitly
when running CMake:
```
$ cmake -G “NMake Makefiles” -D CMAKE_BUILD_TYPE=Debug ..
```

This is only useful if you’re developing the application. If not, use the default
Release build because it will run much faster and the binary will be much smaller.

Running
=======
An executable file will be generated in the build/bin folder. Just run it.

Thanks
======
Many thanks to the vulkan-tutorial.com:

https://vulkan-tutorial.com

License
=======
vkplayground - Playing around with Vulkan

Copyright 2016 Renato Utsch

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
