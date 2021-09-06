
# FslBuild CMake support (Early Access)

**We currently use CMake as a backend of most systems now.**

But full support for all cmake generators is a work in progress so there might be issues with it.
Which is why we still recommend that the default builders for each platform is used.

## Table Of Contents

<!-- #AG_TOC_BEGIN# -->
* [Force use the CMake generator](#force-use-the-cmake-generator)
* [CMake specific arguments](#cmake-specific-arguments)
  * [CMakeBuildDir](#cmakebuilddir)
  * [CMakeInstallPrefix](#cmakeinstallprefix)
  * [CMakeGeneratorName](#cmakegeneratorname)
  * [CMakeConfigArgs](#cmakeconfigargs)
  * [CMakeConfigGlobalArgs](#cmakeconfigglobalargs)
* [Default cmake generators per platform](#default-cmake-generators-per-platform)
* [CMake configuration](#cmake-configuration)
* [Known issues](#known-issues)
<!-- #AG_TOC_END# -->

# Force use the CMake generator

For the platforms that currently use CMake per default this is not necessary.
But to test the cmake backend on windows you can force enable it.
It should be as simple as specifying it to the command line tool

```bash
FslBuild.py -g cmake
FslBuildInfo.py -g cmake
FslBuildGen.py -g cmake
FslBuildRun.py -g cmake
```

This will execute the command with the CMake build generator.
As CMake uses out of source builds the toolchain uses a default directory of ```${TopProjectRoot}/build/<platform-name>``` as build directory (See CMake configuration for more info about ```${TopProjectRoot}```).

For the demo framework this normally resolves to ```${TopProjectRoot} == $(FSL_GRAPHICS_SDK)```. So the default build folder will be ```$(FSL_GRAPHICS_SDK)/build```. The ```--CMakeBuildDir``` argument can override the build directory so you can use a directory of your choice.

# CMake specific arguments

Extra arguments can be supplied to control the CMake build.
The arguments are experimental and work in progress so their names could change in the future!

Argument                                | Description
----------------------------------------|--------------------------------------------
--CMakeBuildDir CMAKEBUILDDIR           | Select the build dir to use for cmake
--CMakeGeneratorName CMAKEGENERATORNAME | Select the cmake generator name
--CMakeConfigArgs                       | Speficies extra arguments for the cmake config step (this does not include recipe builds). For example *--CMakeConfigArgs="--debug-output -Wdeprecated"*
--CMakeConfigGlobalArgs                 | Speficies extra arguments for the cmake config step for both normal and recipe builds. For example *--CMakeConfigArgs="--debug-output -Wdeprecated"*
--CMakeInstallPrefix CMAKEINSTALLPREFIX | Select the install prefix for cmake
--CMakeAllowFindPackage                 | Override the default platform settings for cmake FindPackage support (if disabled find_package statements will not be added for thirdparty dependencies).

## CMakeBuildDir

Override the default out of source build directory used by FslBuild.

## CMakeInstallPrefix

Override the default install prefix. This is equal to supplying -DCMAKE_INSTALL_PREFIX

## CMakeGeneratorName

Override the default generator name supplied to cmake. See the "cmake -G" option for what it supports.

## CMakeConfigArgs

Add additional command line arguments to the cmake configure step for normal builds, this does not get added to recipe builds. See "cmake -h" and the cmake documentation for what arguments cmake support.

## CMakeConfigGlobalArgs

Add additional command line arguments to the cmake configure step for both normal and recipe builds. See "cmake -h" and the cmake documentation for what arguments cmake support.


# Default cmake generators per platform

Platform | Generator                                | Status          | AllowFindPackage |
---------|------------------------------------------|-----------------|------------------|
Android  | Not supported (use the normal generator) | Not supported   | true             |
Windows  | Visual Studio 16 2019 (win64)            | Partial support | true             |
Ubuntu   | Ninja                                    | Supported       | true             |
Yocto    | Ninja                                    | Supported       | false            |

Both "Ninja" and "Unix makefiles" should work on most linux platforms.
Yocto sdk's do not currently contain the required cmake files necessary for modern cmake find_package to work, so we disable the support per default.

# CMake configuration

The main configuration is stored in the Project.gen file at the root of this project.
Like this:

```xml
    <CMakeConfiguration DefaultBuildDir="${TopProjectRoot}/build">
      <Platform Name="Ubuntu" DefaultGeneratorName="Ninja"/>
      <Platform Name="Windows"/>
      <Platform Name="Yocto" DefaultGeneratorName="Ninja"/>
    </CMakeConfiguration>
```

The default build directory is controlled by DefaultBuildDir and ```${TopProjectRoot}``` resolves to the path to the top project root. For simple projects this is the project root folder. For extended projects its the root folder of the topmost extended project.

# Known issues

* **Not all cmake build configuration is stored out of source. So dont use the same repo for multiple concurrent builds.**
  * This is something we will improve in future versions.
* Android is not supported!
* Samples that rely on "dynamic variants" are not supported.
  * This is mainly used for OpenGL ES support on windows.
* Your chosen generator might not work
  * We currently only test the "default" backends for each platform.
* FslBuild.py -c clean does not perform a full clean. Delete the our of source build folder to do that!
