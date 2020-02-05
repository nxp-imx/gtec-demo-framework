# FslBuild CMake support (Early Access)

The CMake generator for the FslBuild tools is a work in progress so there might be issues with it.
We still recommend that the default builders for each platform is used.

## Use the CMake generator

To use the CMake generator it should be as simple as specifying it to the command line tool

```bash
FslBuild.py -g cmake
FslBuildInfo.py -g cmake
FslBuildGen.py -g cmake
FslBuildRun.py -g cmake
```

This will execute the command with the CMake build generator.
As CMake uses out of source builds the toolchain uses a default directory of "${TopProjectRoot}/build/<platform-name>" as build directory (See CMake configuration for more info about ${TopProjectRoot}). 

For the demo framework this normally resolves to ${TopProjectRoot} == $(FSL_GRAPHICS_SDK). So the default build folder will be $(FSL_GRAPHICS_SDK)/build. The ```--CMakeBuildDir``` argument can override the build directory so you can use a directory of your choice.


### CMake specific arguments

Extra arguments can be supplied to control the CMake build.
The arguments are experimental and work in progress so their names could change in the future!

Argument                                | Description
----------------------------------------|--------------------------------------------
--CMakeBuildDir CMAKEBUILDDIR           | Select the build dir to use for cmake
--CMakeInstallPrefix CMAKEINSTALLPREFIX | Select the install prefix for cmake
--CMakeGeneratorName CMAKEGENERATORNAME | Select the cmake generator name

#### CMakeBuildDir

Override the default out of source build directory used by FslBuild.

#### CMakeInstallPrefix

Override the default install prefix. This is equal to supplying -DCMAKE_INSTALL_PREFIX

### CMakeGeneratorName

Override the default generator name supplied to cmake. See the "cmake -G" option for what it supports.

## Default cmake generators per platform

Platform | Generator                                | Status
---------|------------------------------------------|-----------------
Android  | Not supported (use the normal generator) | Not supported
Windows  | Visual Studio 15 2017 Win64              | Partial support
Ubuntu   | Unix Makefiles                           | Supported
Yocto    | Unix Makefiles                           | Supported

## CMake configuration

The main configuration is stored in the Project.gen file at the root of this project.
Like this:

```xml
    <CMakeConfiguration DefaultBuildDir="${TopProjectRoot}/build">
      <Platform Name="Ubuntu" DefaultGeneratorName="Unix Makefiles"/>
      <Platform Name="Windows"/>
      <Platform Name="Yocto" DefaultGeneratorName="Unix Makefiles"/>
    </CMakeConfiguration>
```

The default build directory is controlled by DefaultBuildDir and ${TopProjectRoot} resolves to the path to the top project root. For simple projects this is the project root folder. For extended projects its the root folder of the topmost extended project.

## Known issues

* Android is not supported!
* Samples that rely on "dynamic variants" are not supported.
  * This is mainly used for OpenGL ES support on windows.
* Your chosen generator might not work
  * We currently only test the "default" backends for each platform.
* FslBuild.py -c clean does not perform a full clean. Delete the our of source build folder to do that!
* Not all cmake build configuration is stored out of source. So dont use the same repo for multiple concurrent builds.
  * This is something we will improve in future versions.
* The cmake builds will probably produce a larger executable than the existing builders.
