Prerequisites
=============
- Python 2.7

  It should be part of the default Ubuntu install.

- A working yocto build
  For example follow one of these:
  - http://git.freescale.com/git/cgit.cgi/imx/fsl-arm-yocto-bsp.git/
  - https://community.freescale.com/docs/DOC-94866 
  - This [guide](Preparing a Yocto build)
For this guide we will assume you are using a X11 image.     
  
- Download the source from git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

Preparing a Yocto build
-----------------------

Before you build one of these yocto images you need to
1. Unpack the `meta-gtec.tar.gz` file found in the demoframework root directory to `<fsl_yocto_bsp>/sources`
   Example:
    ```bash
    tar -xvzf meta-gtec.tar.gz -C ~/fsl-release-bsp/sources
    ```
2. Run the yocto build setup (X11 example). 
    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-x11 -e x11
    ```
3. Edit the `<build directory>/conf/bblayers.conf` file and add the line:
    ```bash
    BBLAYERS += " ${BSPDIR}/sources/meta-gtec "
    ```
4. Edit the `<build directory>/conf/local.conf` file and add the lines:
    ```bash
    CORE_IMAGE_EXTRA_INSTALL += "assimp "
    CORE_IMAGE_EXTRA_INSTALL += "devil "
    ```
5. Beware that running the `MACHINE` command overwrites the changes done in step 2+3!!!

You can now build one of the images below (or a custom one)
       
### x11 yocto image 

Example:

a. Perform step 1
    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-x11 -e x11
    ```
b. Perform step 3+4
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

### FB yocto image     

Example:

a. Perform step 1
    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-fb -e fb
    ```
b. Perform step 3+4
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
    
### Wayland yocto image     

Example:
a. Perform step 1
    ```bash
    MACHINE=imx6qpsabresd source fsl-setup-release.sh -b build-wayland -e wayland
    ```
b. Perform step 3+4
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


Yocto environment setup
=======================
1. Start a terminal (ctrl+alt t)
2. Prepare the yocto build environment
    ```bash
    pushd ~/fsl-release-bsp/build-x11/tmp
    source environment-setup-cortexa9hf-vfp-neon-poky-linux-gnueabi
    export ROOTFS=~/unpacked-rootfs/build-x11
    export FSL_PLATFORM_NAME=Yocto
    popd
    ```
     

Simple setup
------------
1. Make sure that you performed the [Yocto environment setup].
2. cd to the demoframework folder
3. Run the `prepare.sh` file located in the root of the framework folder to
    configure the necessary environment variables and paths.
    Please beware that the `prepare.sh` file requires the current working 
    directory to be the root of your demoframework folder to function 
    (which is also the folder it resides in).
    ```bash
    source prepare.sh
    ```
 
 
To Compile all samples
----------------------
1. Make sure that you performed the [simple setup].
2. Compile everything (a good rule of thumb for '-j N' is number of cpu cores * 2)
    ```bash
    FslBuild.py --Variants [WindowSystem=X11] -t sdk -- -j 2
    ```
    WindowSystem can be set to either: FB, Wayland or x11

     
To Compile and run an existing sample application 
-------------------------------------------------
In this example we will utilize the `GLES2.S06_Texturing` app.
1. Make sure that you performed the [simple setup].
2. Change directory to the sample directory:
    ```bash
    cd DemoApps/GLES2/S06_Texturing
    ```
3. Compile the project (a good rule of thumb for '-j N' is number of cpu cores * 2)
    ```bash
    FslBuild.py --Variants [WindowSystem=X11] -- -j 2
    ```
      
    WindowSystem can be set to either: FB, Wayland or x11


To create a new GLES2 demo project named 'CoolNewDemo'
------------------------------------------------------
1. Make sure that you performed the [simple setup]
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
5. Compile the project (a good rule of thumb for '-j N' is number of cpu cores * 2)
    ```bash
    FslBuild.py --Variants [WindowSystem=X11] -- -j 2
    ```
      
    WindowSystem can be set to either: FB, Wayland or x11

Note: 

Once a build has been done once you can just invoke the make file directly.
However this requires that you didn't change any dependencies or add files.

To do this run 
    ```bash
    make -f GNUmakefile_Yocto -j 2 WindowSystem=X11
    ```

If you add source files to a project or change the Fsl.gen file then run the 
`FslBuildGen.py` script in the project root folder to regenerate the various 
build files or just make sure you always use the `FslBuild.py` script as it 
automatically adds files and regenerate build files as needed.


### To see which features a DemoApp requires to be able to build 
1. Make sure that you performed the [simple setup]
2. Change directory to the GLES2 sample directory:
    ```bash
    cd DemoApps/GLES2
    ```
3. Create the project template using the `FslBuildNew.py` script
    ```bash
    FslBuild.py --ListFeatures
    ```


Building Vulkan demo framework apps
-----------------------------------

To build vulkan demoes you need access to the glslangValidator tool which is used to compile shaders into SPIR-V format.
The easiest way to get it is to install the linux [vulkan SDK](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html).

1. Download the vulkan sdk from https://vulkan.lunarg.com/sdk/home
2. Make the downloaded file executable
    ```bash
    chmod ugo+x vulkansdk-linux-x86_64-1.0.30.0.run
    ```
3. Move the downloaded file to a sdk dir
    ```bash
    mkdir ~/vulkan
    mv vulkansdk-linux-x86_64-1.0.30.0.run ~/vulkan
    ```
4. Run it
    ```bash
    cd ~/vulkan
    ./vulkansdk-linux-x86_64-1.0.30.0.run
    ```
5. Install the necessary packages
    ```bash
     sudo apt-get install libglm-dev graphviz libxcb-dri3-0 libxcb-present0 libpciaccess0 cmake libpng-dev libxcb-dri3-dev libx11-dev
    ```
6. Setup the vulkan environment
    ```bash
      pushd ~/vulkan/VulkanSDK/1.0.30.0
      source setup-env.sh
      popd
    ```
7. Run the normal setup.


Building OpenCV 2.4 demo framework apps
---------------------------------------
1. Edit the `<build directory>/conf/local.conf` file and add the line:
    ```
    CORE_IMAGE_EXTRA_INSTALL += "libopencv-core-dev libopencv-highgui-dev"
    ```     
  - Help: http://imxcv.blogspot.dk/2014/02/building-opencv-24x-for-freescales-imx6.html
  - Note: CORE_IMAGE_EXTRA_INSTALL += "gpu-viv-bin-mx6q gpu-viv-bin-mx6q-dev" does not appear to be needed.

  
