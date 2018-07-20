# Setup guide Windows

## Prerequisites

- [Visual Studio 2017](https://www.visualstudio.com/vs/community/)
  (community or better)
- [Python 3.4+](https://www.python.org/ftp/python/3.6.2/python-3.6.2-amd64.exe)
  To be able run python scripts.
  If you use 3.4 you need to install the 'typing' library manually so we highly recommended using 3.5 or newer.
  To install the typing library in Python **3.4** run:
    ```bash
    python3 -m pip install typing
    ```
- One of these:
  - [Arm Mali OpenGL ES Emulator 3.0.2.g694a9 (64 bit)](https://developer.arm.com/products/software-development-tools/graphics-development-tools/opengl-es-emulator/downloads)

    *Please use the exact version (64bit) and use the installer to install it to the default location!*
  - Vivante OpenGL ES Emulator

For OpenCL, OpenCV and OpenVX support additional packages are required, see below.
  
To get started its recommended to utilize the Arm Mali OpenGL ES 3.0.2 emulator (64 bit)
which this guide will assume you are using.

- Download the source from git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

## Simple setup

1. Start a windows console (cmd.exe) in the DemoFramework folder
2. Run the visual studio ```vcvarsall.bat x64``` to prepare your command line compiler environment for x64 compilation.
   - For VS2017 its often located here: ```"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64```
3. Run the `prepare.bat` file located in the root of the framework folder to
   configure the necessary environment variables and paths.
   Please beware that the `prepare.bat` file requires the current working
   directory to be the root of your demoframework folder to function
   (which is also the folder it resides in).

## To Compile and run an existing GLES2 sample application

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup].
2. Change directory to the sample directory:
    ```bash
    cd DemoApps\GLES2\S06_Texturing
    ```
3. Generate build files
    ```bash
    FslBuildGen.py
    ```
4. Launch visual studio using the Arm Mali Emulator:
    ```bash
    .StartProject.bat arm
    ```
5. Compile and run the project (The default is to press F5)

To utilize a different emulator the `.StartProject.bat` file can be launched with the following arguments

argument|emulator
--------|-----------------------------------------------------------------------------------
arm     |the arm mali emulator
powervr |the powervr emulator
qualcomm|the qualcomm andreno adreno emulator (expects its installed in "c:\AdrenoSDK")
vivante |the vivante emulator
  
## To create a new GLES2 demo project named 'CoolNewDemo'

1. Make sure that you performed the [simple setup].
2. Change directory to the GLES2 sample directory:
    ```bash
    cd DemoApps/GLES2
    ```
3. Create the project template using the FslBuildNew.py script
    ```bash
       FslBuildNew.py GLES2 CoolNewDemo  
    ```
4. Change directory to the newly created project folder 'CoolNewDemo'
    ```bash
    cd CoolNewDemo
    ```
5. Generate build files for Android, Ubuntu and Yocto (this step will be simplified soon)
    ```bash
    FslBuildGen.py
    ```
6. Launch visual studio using the Arm Mali Emulator:
    ```bash
    .StartProject.bat arm
    ```
7. Compile and run the project (The default is to press F5) or start creating your new demo.

If you add source files to a project or change the Fsl.gen file then run the
FslBuildGen.py script in the project root folder to regenerate the various
build files.

## Building OpenCV 3.2 demo framework apps

1. See [Setup OpenCV 3.2]
2. Run `prepare.bat` as usual.
3. Add a dependency to OpenCV to your "fsl.gen" file like this
    ```xml
    <Dependency Name="OpenCV3"/>
    ```
   See DemoApps/GLES3/OpenCV101/Fsl.gen for how its done.
4. Run `FslBuildGen.py` to regenerate the project files.

## Building OpenCL demo framework apps

1. Download and install the desired OpenCL sdk from Amd
   - [Amd (AMD APP SDK 3.0)](http://developer.amd.com/amd-accelerated-parallel-processing-app-sdk/)
2. Generate a new project add a dependency to OpenCL by adding
    ```xml
    <Dependency Name="OpenCL"/>
    ```
   to the projects "Fsl.gen" file.
   See [DemoApps/GLES3/OpenCL101/Fsl.gen](../DemoApps/GLES3/OpenCL101/Fsl.gen) for a example.
3. Run `FslBuildGen.py` to regenerate the project files.
4. Start the project

## Building OpenVX demo framework apps:

1. See [Setup OpenVX]
2. Generate a new project add a dependency to OpenVX by adding 
    ```xml
    <Dependency Name="OpenVX"/>
    ```
   to the projects "Fsl.gen" file.
   See [DemoApps/GLES3/OpenVX101/Fsl.gen](../DemoApps/GLES3/OpenVX101/Fsl.gen) for a example.
3. Run `FslBuildGen.py` to regenerate the project files.
4. Start the project

## Setup OpenCV 3.2

1. Install the 3.2 sdk files.
2. Configure the environment variable OPENCV_DIR to point to the sdk location like this
    ```bash
    set OPENCV_DIR=c:\_sdk\OpenCV-3.2.\build
    ```

## Setup OpenVX

1. Follow the guide for setting up OpenCL and OpenCV support. (need OpenCV3.2)

## Legacy Git note

When you add the generated build.sh to git on windows then please remember to set the
executable bit using:

```bash
git update-index --chmod=+x build.sh
```

--------------------------------------------------------------------------------

## Notes for building AMD OpenVX files yourself

WARNING: only do this if you have issues with the precompiled version we include

### Building OpenVX demo framework apps

1. Download and build the OpenVX sdk using the AMD SDK Setup below
2. Generate a new project add a dependency to OpenVX by adding
    ```xml
    <Dependency Name="OpenVX"/>
    ```
   to the projects "Fsl.gen" file
   See [DemoApps/GLES3/OpenVX101/Fsl.gen](../DemoApps/GLES3/OpenVX101/Fsl.gen) for a example.
3. Run `FslBuildGen.py` to regenerate the project files.
4. Start the project
