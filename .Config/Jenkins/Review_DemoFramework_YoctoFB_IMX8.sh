#!/bin/bash
set -o errexit
#pushd /home/shared/fsl-release-bsp
#MACHINE=imx6qsabreauto source fsl-setup-release.sh -b build-fb -e fb
#cd tmp
pushd /home/shared/fsl-beta-bsp/build-fb/tmp
source environment-setup-aarch64-poky-linux
export ROOTFS=/home/shared/jen-rootfs/build-fb-imx8
popd

export VULKAN_SDK=/home/shared/vulkan/VulkanSDK/1.0.51.0/x86_64
export PATH=$VULKAN_SDK/bin:$PATH
export LD_LIBRARY_PATH=$VULKAN_SDK/lib:$LD_LIBRARY_PATH
export VK_LAYER_PATH=$VULKAN_SDK/etc/explicit_layer.d
export LIBRARY_PATH=$VULKAN_SDK/lib:$LIBRARY_PATH
echo VULKAN_SDK=$VULKAN_SDK

export FSL_PLATFORM_NAME=Yocto
export FSL_GRAPHICS_SDK=$WORKSPACE
. prepare.sh
#G2D disabled for now
FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL,OpenCL1.1,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.0.1,Vulkan] --Variants [WindowSystem=FB] --BuildThreads 4 -- install
