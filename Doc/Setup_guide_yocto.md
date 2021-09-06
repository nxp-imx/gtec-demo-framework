
# Setup guide Yocto (bsp)

First you need to decide how you are going to be building for Yocto

* [Building using a prebuild Yocto SDK](#building-using-a-prebuild-yocto-sdk)
* [Building using a full Yocto build](#building-using-a-full-yocto-build)

Third party software downloads are now disabled per default. To build using an old Yocto release that doesn't come with all third party software you need to add ```--Recipes [*]``` to your command line which will re-enable the download.

---------------------------------------------------------------------------------------------------

## Table of contents

<!-- #AG_TOC_BEGIN# -->
* [Building using a prebuild Yocto SDK](#building-using-a-prebuild-yocto-sdk)
  * [Prerequisites](#prerequisites)
  * [Preparing a Yocto SDK build](#preparing-a-yocto-sdk-build)
  * [Yocto SDK environment setup](#yocto-sdk-environment-setup)
  * [Ready to build via sdk](#ready-to-build-via-sdk)
* [Building using a full Yocto build](#building-using-a-full-yocto-build)
  * [BSP Build Prerequisites](#bsp-build-prerequisites)
  * [Preparing a Yocto build](#preparing-a-yocto-build)
  * [x11 yocto image](#x11-yocto-image)
  * [FB yocto image](#fb-yocto-image)
  * [Wayland yocto image](#wayland-yocto-image)
  * [Yocto environment setup](#yocto-environment-setup)
  * [Ready to build using BSP build](#ready-to-build-using-bsp-build)
* [Using the demo framework](#using-the-demo-framework)
  * [Simple setup](#simple-setup)
* [To Compile and run an existing sample application](#to-compile-and-run-an-existing-sample-application)
  * [To Compile and run an existing GLES2 sample application](#to-compile-and-run-an-existing-gles2-sample-application)
  * [To Compile and install an existing sample application](#to-compile-and-install-an-existing-sample-application)
* [To create a new demo project named 'CoolNewDemo'](#to-create-a-new-demo-project-named-'coolnewdemo')
  * [So to create a new GLES2 demo project named 'CoolNewDemo'](#so-to-create-a-new-gles2-demo-project-named-'coolnewdemo')
  * [To Compile all samples](#to-compile-all-samples)
  * [To see which features a DemoApp requires to be able to build](#to-see-which-features-a-demoapp-requires-to-be-able-to-build)
* [Copying DemoFramework apps to the sdcard](#copying-demoframework-apps-to-the-sdcard)
  * [Basic copy](#basic-copy)
  * [Using install](#using-install)
* [Building Vulkan demo framework apps](#building-vulkan-demo-framework-apps)
* [Building OpenCV demo framework apps](#building-opencv-demo-framework-apps)
<!-- #AG_TOC_END# -->

# Building using a prebuild Yocto SDK

Building using a prebuild Yocto SDK and a prebuild sd-card image.
This tend to be the fastest way to get started.

## Prerequisites

* Ubuntu 20.04
* [CMake 3.10.2 or newer](https://cmake.org/download/)
* Python 3.6 (this is standard from Ubuntu 16.04 and forward)
* A prebuild sdk for your board typically called something like ```toolchain.sh```
* A prebuild sd-card image for your board typically called ```BoardName.rootfs.sdcard.bz2```
* Git

    ```bash
    sudo apt-get install git
    ```

* Ninja build

    ```bash
    sudo apt-get install ninja-build
    ```

For this guide we will assume you are using a FB image.

* Download the DemoFramework source using git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

## Preparing a Yocto SDK build

1. Start a terminal (ctrl+alt t)
2. Install the sdk:

    ```bash
    ./fsl-imx-internal-xwayland-glibc-x86_64-fsl-image-gui-aarch64-toolchain-4.9.51-mx8-beta.sh
    ```

    Chose where to install it, you can use the default location or a location of your choice.
    For this example, we use "~/sdk/4.9.51-mx8-beta".
    When the setup is complete it will list the configuration script you need to run to configure the sdk environment.
    Something like this

    ```bash
    Each time you wish to use the SDK in a new shell session, you need to source the environment setup script e.g.
    $ . ~/sdk/4.9.51-mx8-beta/environment-setup-aarch64-poky-linux
    ```

3. Your SDK is now installed.

## Yocto SDK environment setup

1. Start a terminal (ctrl+alt t)
2. Prepare the yocto build environment by running the config command you got during the sdk install

    ```bash
    . ~/sdk/4.9.51-mx8-beta/environment-setup-aarch64-poky-linux
    ```

3. You should now be ready to build using the demo framework. However, if you experience issues with the ```prepare.sh``` script you
   can help it out by defining the platform name and the location of the root fs

    ```bash
    export FSL_PLATFORM_NAME=Yocto
    export ROOTFS=~/sdk/4.9.51-mx8-beta/sysroots/aarch64-poky-linux
    ```

   Another possible error you can encounter is that the FslBuild.py scripts fail to include the 'typing' library.
   This can happen because the SDK comes with a too old Python3 version or a incomplete Python3.6 version.
   As a workaround for that you could delete the Python3 binaries from the SDK which will cause it to use the system Python3 version instead.

## Ready to build via sdk

You are now ready to start building Yocto apps using the demo framework.
Please continue the guide at [Using the demo framework].

---------------------------------------------------------------------------------------------------

# Building using a full Yocto build

Building using a full manually build Yocto build.
This process provides the most flexible solution but it also takes significantly longer to build the initial Yocto sdcard and toolchain.

## BSP Build Prerequisites

* The Ubuntu version required by the BSP release.
* [CMake 3.10.2 or newer](https://cmake.org/download/)
* Python 3.6+ It should be part of the default Ubuntu install.
* Ninja build

    ```bash
    sudo apt-get install ninja-build
    ```

* A working yocto build
  For example follow one of these:
  * http://git.freescale.com/git/cgit.cgi/imx/fsl-arm-yocto-bsp.git/
  * https://community.freescale.com/docs/DOC-94866
  * Or read from here: [Preparing a Yocto build]

For this guide we will assume you are using a FB image.

* Download the DemoFramework source using git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

## Preparing a Yocto build

Before you build one of these yocto images you need to

1. Run the yocto build setup (X11 example).

    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-x11 -e x11
    ```

2. Bake

    ```bash
    bitbake fsl-image-gui
    bitbake meta-toolchain
    bitbake meta-ide-support
    ```

    You can now build one of the images below (or a custom one)

## x11 yocto image

Example:

* Perform step 1

    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-x11 -e x11
    ```

* Perform step 2

    ```bash
    bitbake fsl-image-gui
    bitbake meta-toolchain
    bitbake meta-ide-support
    ```

Extracted rootfs

We assume your yocto build dir is located at `~/fsl-release-bsp/build-x11` and
that the rootfs will be unpacked to `~/unpacked-rootfs/build-x11` and
the image is called `fsl-image-gui-imx6qpsabresd.rootfs.tar.bz2` (you will need to locate your image name)

```bash
runqemu-extract-sdk ~/fsl-release-bsp/build-x11/tmp/deploy/images/imx6qpsabresd/fsl-image-gui-imx6qpsabresd.rootfs.tar.bz2  ~/unpacked-rootfs/build-x11
```

## FB yocto image

Example:

* Perform step 1

    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-fb -e fb
    ```

* Perform step 2

    ```bash
    bitbake fsl-image-gui
    bitbake meta-toolchain
    bitbake meta-ide-support
    ```

Extracted rootfs

We assume your yocto build dir is located at `~/fsl-release-bsp/build-fb` and
that the rootfs will be unpacked to `~/unpacked-rootfs/build-fb` and
the image is called `fsl-image-gui-imx6qpsabresd.rootfs.tar.bz2` (you will need to locate your image name)

```bash
runqemu-extract-sdk ~/fsl-release-bsp/build-fb/tmp/deploy/images/imx6qpsabresd/fsl-image-gui-imx6qpsabresd.rootfs.tar.bz2  ~/unpacked-rootfs/build-fb
```

## Wayland yocto image

Example:

* Perform step 1

    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-wayland -e wayland
    ```

* Perform step 2

    ```bash
    bitbake fsl-image-gui
    bitbake meta-toolchain
    bitbake meta-ide-support
    ```

Extracted rootfs

We assume your yocto build dir is located at `~/fsl-release-bsp/build-wayland` and
that the rootfs will be unpacked to `~/unpacked-rootfs/build-wayland` and
the image is called `fsl-image-gui-imx6qpsabresd.rootfs.tar.bz2` (you will need to locate your image name)

```bash
runqemu-extract-sdk ~/fsl-release-bsp/build-wayland/tmp/deploy/images/imx6qpsabresd/fsl-image-gui-imx6qpsabresd.rootfs.tar.bz2  ~/unpacked-rootfs/build-wayland
```

## Yocto environment setup

1. Start a terminal (ctrl+alt t)
2. Prepare the yocto build environment

    ```bash
    pushd ~/fsl-release-bsp/build-fb/tmp
    source environment-setup-cortexa9hf-neon-poky-linux-gnueabi
    export ROOTFS=~/unpacked-rootfs/build-fb
    export FSL_PLATFORM_NAME=Yocto
    popd
    ```

## Ready to build using BSP build

You are now ready to start building Yocto apps using the demo framework,
please continue the guide at [Using the demo framework].

---------------------------------------------------------------------------------------------------

# Using the demo framework

## Simple setup

1. Make sure that you performed the Yocto environment setup for your chosen Yocto environment.
   * SDK build [Yocto SDK environment setup]
   * Custom build [Yocto environment setup].
2. cd to the demoframework folder
3. Run the `prepare.sh` file located in the root of the framework folder to
    configure the necessary environment variables and paths.
    Please beware that the `prepare.sh` file requires the current working
    directory to be the root of your demoframework folder to function
    (which is also the folder it resides in).

    ```bash
    source prepare.sh
    ```

    Also verify that the script detect that you are doing a Yocto build by outputting

    ```bash
    PlatformName: Yocto
    ```

    If it doesn't you can override the platform auto detection by setting the environment variable

    ```bash
    export FSL_PLATFORM_NAME=Yocto
    ```

    Before running the prepare.sh script.

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

3. Build and the application.

    ```bash
    FslBuild.py --Variants [WindowSystem=FB]
    ```

    WindowSystem can bet set to either `FB`, `Wayland`, `Wayland_IVI`, `X11` be sure to pick the right one.
    If the app doesn't require a WindowSystem the `--Variants [WindowSystem=FB]`can be omitted.

The following commands are also pretty useful to know.

Command                                       | Description
----------------------------------------------|---------------------------------------
`FslBuild.py`                                 | To build the example.
`FslBuild.py --ListVariants`                  | List all the build variants that can be specified. List all the build variants that can be specified. This is useful to see the WindowSystem's.
`FslBuildInfo.py --ListRequirements`          | To list the samples requirements.

It is also recommended to check out the `README.md` and `Example.jpg` that is included with all samples.

Note:

If you add source files to a project or change the Fsl.gen file then run the
`FslBuildGen.py` script in the project root folder to regenerate the various
build files or just make sure you always use the `FslBuild.py` script as it
automatically adds files and regenerate build files as needed.

## To Compile and run an existing GLES2 sample application

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup).
2. Change directory to the sample directory:

    ```bash
    cd DemoApps/GLES2/S06_Texturing
    ```

3. Compile the project

    ```bash
    FslBuild.py --Variants [WindowSystem=FB]
    ```

## To Compile and install an existing sample application

In this example we will utilize the `GLES2.S06_Texturing` app.

1. Make sure that you performed the [simple setup](#simple-setup).
2. Change directory to the sample directory:

    ```bash
    cd DemoApps/GLES2/S06_Texturing
    ```

3. Compile and install the project to ```$FSL_GRAPHICS_SDK/bin```

    ```bash
    FslBuild.py --Variants [WindowSystem=FB] -c install --CMakeInstallPrefix $FSL_GRAPHICS_SDK/bin
    ```

4. Copy the content of ```$FSL_GRAPHICS_SDK/bin``` to the target

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
    FslBuild.py --Variants [WindowSystem=FB]
    ```

Note:

If you add source files to a project or change the Fsl.gen file then run the
`FslBuildGen.py` script in the project root folder to regenerate the various
build files or just make sure you always use the `FslBuild.py` script as it
automatically adds files and regenerate build files as needed.

## So to create a new GLES2 demo project named 'CoolNewDemo'

In this example we will create a GLES2 app called CoolNewDemo.

1. Make sure that you performed the [simple setup](#simple-setup)
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
    FslBuild.py --Variants [WindowSystem=FB]
    ```

    WindowSystem can bet set to either `FB`, `Wayland`, `Wayland_IVI`, `X11` be sure to pick the right one.

## To Compile all samples

1. Make sure that you performed the [simple setup](#simple-setup).
2. Compile everything (a good rule of thumb for '--BuildThreads N' is number of cpu cores * 2) If '--BuildThreads' is not specified it will be set to 'auto' which uses your cpu core count.

    ```bash
    FslBuild.py --Variants [WindowSystem=FB] -t sdk --BuildThreads 2
    ```

    WindowSystem can bet set to either `FB`, `Wayland`, `Wayland_IVI`, `X11` be sure to pick the right one.

## To see which features a DemoApp requires to be able to build

1. Make sure that you performed the [simple setup](#simple-setup)
2. Change directory to the GLES2 sample directory:

    ```bash
    cd DemoApps/GLES2
    ```

3. Create the project template using the `FslBuildNew.py` script

    ```bash
    FslBuild.py --ListFeatures
    ```

---------------------------------------------------------------------------------------------------

# Copying DemoFramework apps to the sdcard

## Basic copy

1. Mount the SDK card in ubuntu.
2. Manually copy the build Exectuable and its content directory to the sdcard
3. Unmount the sdcard

## Using install

1. Mount the SDK card in ubuntu.
2. Build using

    ```bash
    FslBuild.py -- install
    ```

   This will cause the app to install itself and its content to the the demo framework root under a directory called bin.
   This directory can then be copied manually to the sdcard.
   Beware that 'install' can be used for all build commands, so you could build all apps and then just copy the bin directory.
3. Manually copy the build Executable and its content directory to the sdcard
4. Unmount the sdcard

---------------------------------------------------------------------------------------------------

# Building Vulkan demo framework apps

To build Vulkan demoes you need access to the glslangValidator tool which is used to compile shaders into SPIR-V format.
The easiest way to get it is to install the Vulkan SDK, See the [official SDK guide](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html)

1. Download the Vulkan sdk from https://vulkan.lunarg.com/sdk/home
2. Move the downloaded file to a sdk dir

    ```bash
    mkdir ~/vulkan
    mv vulkansdk-linux-x86_64-1.1.92.1.tar.gz ~/vulkan
    ```

3. Unpack it it

    ```bash
    cd ~/vulkan
    tar zxf vulkansdk-linux-x86_64-1.1.92.1.tar.gz
    ```

4. Install the necessary packages

    ```bash
    sudo apt-get install libglm-dev cmake libxcb-dri3-0 libxcb-present0 libpciaccess0 libpng-dev libxcb-keysyms1-dev libxcb-dri3-dev libx11-dev libmirclient-dev libwayland-dev libxrandr-dev libxcb-ewmh-dev
    ```

5. Setup the vulkan environment

    ```bash
    pushd ~/vulkan/1.1.92.1
    source setup-env.sh
    popd
    ```

6. Ensure that the LIBRARY_PATH is set for GCC

    ```bash
    export LIBRARY_PATH=$VULKAN_SDK/lib:$LIBRARY_PATH
    ```

7. Run the normal setup.

---------------------------------------------------------------------------------------------------

# Building OpenCV demo framework apps

1. Edit the `<build directory>/conf/local.conf` file and add the line:

    ```bash
    CORE_IMAGE_EXTRA_INSTALL += "libopencv-core-dev libopencv-highgui-dev"
    ```

    * Help: http://imxcv.blogspot.dk/2014/02/building-opencv-24x-for-freescales-imx6.html
    * Note: CORE_IMAGE_EXTRA_INSTALL += "gpu-viv-bin-mx6q gpu-viv-bin-mx6q-dev" does not appear to be needed.
