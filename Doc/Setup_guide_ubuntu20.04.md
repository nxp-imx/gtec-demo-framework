
# Setup guide Ubuntu 20.04

## Table of contents

<!-- #AG_TOC_BEGIN# -->
* [Prerequisites](#prerequisites)
* [Simple setup](#simple-setup)
  * [Add OpenGL ES support](#add-opengl-es-support)
  * [Add Vulkan support](#add-vulkan-support)
  * [Add OpenCL support](#add-opencl-support)
  * [Add OpenCV 4.2 support](#add-opencv-42-support)
  * [Add OpenVX support](#add-openvx-support)
* [To Compile and run an existing sample application](#to-compile-and-run-an-existing-sample-application)
  * [To Compile and run an existing GLES2 sample application](#to-compile-and-run-an-existing-gles2-sample-application)
* [To create a new demo project named 'CoolNewDemo'](#to-create-a-new-demo-project-named-'coolnewdemo')
  * [So to create a new GLES2 demo project named 'CoolNewDemo'](#so-to-create-a-new-gles2-demo-project-named-'coolnewdemo')
* [To Compile all samples](#to-compile-all-samples)
* [Visual Studio Code support](#visual-studio-code-support)
  * [VS Code Prerequisites](#vs-code-prerequisites)
  * [To Compile and run an existing sample application using VSCode](#to-compile-and-run-an-existing-sample-application-using-vscode)
<!-- #AG_TOC_END# -->

# Prerequisites

* Ubuntu20.04 64 bit
* Build tools and xrand

    ```bash
    sudo apt-get install build-essential libxrandr-dev ninja-build
    ```

  **Beware we now use ninja-build as the default build system instead of make!**
* [CMake 3.16 or newer](https://cmake.org/download/)

    ```bash
    sudo apt-get install cmake
    ```

* Python 3.6+
  To be able run python scripts, they are not needed to build.
  It should be part of the default Ubuntu install.
* DevIL (Developer's Image Library)

    ```bash
    sudo apt-get install libdevil-dev
    ```

* Assimp (Open Asset Import Library)
  Is now downloaded and build from source when needed. So its no longer necessary to run "sudo apt-get install libassimp-dev".
* Download the source from git.

For OpenGL ES, OpenCL, OpenCV, OpenVX and Vulkan support additional packages are required, see below.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

---------------------------------------------------------------------------------------------------

# Simple setup

1. Decide what API's you want to compile and run apps for then install them using one of the guides below.
2. Start a terminal (ctrl+alt t) in the DemoFramework folder
3. Run the `prepare.sh` file located in the root of the framework folder to
   configure the necessary environment variables and paths.
   Please beware that the `prepare.sh` file requires the current working
   directory to be the root of your demoframework folder to function
   (which is also the folder it resides in).

    ```bash
    source prepare.sh
    ```

## Add OpenGL ES support

1. One of these (they are mutually exclusive):
    * [Arm Mali OpenGL ES Emulator v3.0.4 (64 bit)](https://developer.arm.com/products/software-development-tools/graphics-development-tools/opengl-es-emulator/downloads)
    * Mesa OpenGL ES 2 (might not run OpenGL ES 3 apps)

        ```bash
        sudo apt-get install libgles2-mesa-dev
        ```

    * [ANGLE - Almost Native Graphics Layer Engine](https://github.com/google/angle)

2. Continue the normal setup.

## Add Vulkan support

Install the Vulkan SDK, See the [official SDK guide](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html)

1. [Download the Vulkan sdk](https://vulkan.lunarg.com/sdk/home)
2. Follow the [documentation](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html)
3. Ensure that you run the `setup-env.sh` as recommended by the documentation.
4. Ensure that the LIBRARY_PATH is set for GCC

    ```bash
    export LIBRARY_PATH=$VULKAN_SDK/lib:$LIBRARY_PATH
    ```

5. Continue the normal setup.

## Add OpenCL support

1. Download and install a OpenCL implementation. Beware we depend on cmake's find_package support to locate the installed OpenCL implementation.
2. Continue the normal setup.

## Add OpenCV 4.2 support

1. Install the required packages

    ```bash
    sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
    ```

2. Install the optional packages

    ```bash
    sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
    ```

3. Create a sdk directory

    ```bash
    mkdir ~/sdk
    cd ~/sdk
    ```

4. Download the 4.5.2 release for ubuntu, unzip it, remove the download, enter the directory

    ```bash
    wget https://github.com/opencv/opencv/archive/4.5.2.zip
    unzip 4.5.2.zip
    rm 4.5.2.zip
    cd opencv-4.5.2
    ```

5. Build OpenCV

    ```bash
    mkdir release
    cd release
    cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
    make -j $(nproc)
    sudo make install
    ```

6. Ensure that the OpenCV 4 headers are where they used to be

    ```bash
    sudo ln -s /usr/local/include/opencv4/opencv2/ /usr/local/include/opencv2
    ```

7. Ensure that you can locate the OpenCV .so files

    ```bash
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
    ```

8. Continue the normal setup.

---------------------------------------------------------------------------------------------------

## Add OpenVX support

1. Follow the guide for setting up OpenCL and OpenCV support. (need OpenCV4.2)
2. Continue the normal setup.

The build chain will download and compile a OpenVX implementation.

---------------------------------------------------------------------------------------------------

# To Compile and run an existing sample application

The general approach will be:

1. Make sure that you performed the [simple setup](#simple-setup), including the API's the sample need.
2. Change directory to the sample directory:

    ```bash
    cd DemoApps/<MAIN-API-NAME>/<SAMPLE-NAME>
    ```

    Example MAIN-API-NAME's: GLES2, GLES3, OpenCL, Vulkan.

    Type ```dir``` to see the dir choice.

    Example SAMPLE-NAME's: Bloom, S06_Texturing.

    Type ```dir``` after entering a API folder to see the list of samples

3. Build and run the application

    ```bash
    FslBuildRun.py
    ```

The following commands are also pretty useful to know.

Command                                       | Description
----------------------------------------------|---------------------------------------
`FslBuild.py`                                 | To build the example.
`FslBuildRun.py`                              | To build and run the example.
`FslBuild.py --ListVariants`                  | List all the build variants that can be specified.
`FslBuildInfo.py --ListRequirements`          | To list the samples requirements.
`FslBuild.py -c open2`                        | *Experimental* [Open the project in Visual Studio Code](#Visual-Studio-Code-Support)

It is also recommended to check out the `README.md` and `Example.jpg` that is included with all samples.

Note:

If you add source files to a project or change the Fsl.gen file then run the
`FslBuildGen.py` script in the project root folder to regenerate the various
build files or just make sure you always use the `FslBuild.py` script as it
automatically adds files and regenerate build files as needed.

## To Compile and run an existing GLES2 sample application

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup), including installing and configuring a OpenGLES emulator.
2. Change directory to the sample directory:

    ```bash
    cd DemoApps/GLES2/S06_Texturing
    ```

3. Build and run the application

    ```bash
    FslBuildRun.py
    ```

---------------------------------------------------------------------------------------------------

# To create a new demo project named 'CoolNewDemo'

1. Make sure that you performed the [simple setup](#simple-setup) including the additional OpenGL ES setup.
2. Change directory to the appropriate sample directory:

    ```bash
    cd DemoApps/<MAIN-API-NAME>
    ```

    Example MAIN-API-NAME's: GLES2, GLES3, OpenCL, Vulkan.

    Type ```dir``` to see the dir choice.

3. Create the project template using the FslBuildNew.py script

    ```bash
    FslBuildNew.py <TEMPLATE-NAME> CoolNewDemo
    ```

    Example TEMPLATE-NAME's: GLES2, GLES3, OpenCL1_2, OpenCV4, Vulkan.

    To get a full list run ```FslBuildNew.py . . --List```

4. Change directory to the newly created project folder 'CoolNewDemo'

    ```bash
    cd CoolNewDemo
    ```

5. Compile the project

    ```bash
    FslBuild.py
    ```

Note:

If you add source files to a project or change the Fsl.gen file then run the
`FslBuildGen.py` script in the project root folder to regenerate the various
build files or just make sure you always use the `FslBuild.py` script as it
automatically adds files and regenerate build files as needed.

## So to create a new GLES2 demo project named 'CoolNewDemo'

In this example we will create a GLES2 app called CoolNewDemo.

1. Make sure that you performed the [simple setup](#simple-setup) including the additional OpenGL ES setup.
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

5. Compile the project

    ```bash
    FslBuild.py
    ```

---------------------------------------------------------------------------------------------------

# To Compile all samples

1. Make sure that you performed the [simple setup](#simple-setup).
2. Compile everything (a good rule of thumb for '--BuildThreads N' is number of cpu cores * 2)

    ```bash
    FslBuild.py -t sdk --BuildThreads 2
    ```

    Beware the FslBuild scripts will chose a sensible default for --BuildThreads if its not supplied.

---------------------------------------------------------------------------------------------------

# Visual Studio Code support

There is now ***experimental*** visual studio code support.
To support visual studio code we generate/patch the configuration files stored under ```.vscode```.

* ```launch.json``` contains the configuration used to launch the executable if one exist
* ```settings.json``` contains the cmake configuration required to build the project.

## VS Code Prerequisites

* [Visual Studio Code](https://code.visualstudio.com/docs/setup/linux)
* Visual Studio Code Extensions:
  * [CMake Tools](https://code.visualstudio.com/docs/cpp/CMake-linux)
  * [C/C++](https://code.visualstudio.com/docs/languages/cpp)
* Recommended visual studio code extensions:
  * [Code Spell Checker](https://github.com/streetsidesoftware/vscode-spell-checker)
  * [Gitlens - Git supercharged](https://github.com/eamodio/vscode-gitlens)
  * [Visual Studio Keymap](https://github.com/rebornix/vscode-vs-keybindings)

## To Compile and run an existing sample application using VSCode

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup).
2. Change directory to the sample directory:

    ```bash
    cd DemoApps/GLES2/S06_Texturing
    ```

3. Open the project in visual studio code

    ```bash
    FslBuild.py -c open2
    ```

    This should launch visual studio code configured for the app.

***BEWARE***: Visual Studio Code has its own setting for debug/release build which is configured in the bottom blue line.
***BEWARE***: Make sure you use the correct launch script. "FslBuild-Windows" for windows and "FslBuild-Ubuntu" for ubuntu.
