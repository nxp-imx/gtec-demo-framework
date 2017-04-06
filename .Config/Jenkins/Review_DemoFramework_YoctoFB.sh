#!/bin/bash
set -o errexit
#pushd /home/shared/fsl-release-bsp
#MACHINE=imx6qsabreauto source fsl-setup-release.sh -b build-fb -e fb
#cd tmp
pushd /home/shared/fsl-release-bsp/build-fb/tmp
source environment-setup-cortexa9hf-vfp-neon-poky-linux-gnueabi
export ROOTFS=/home/shared/jen-rootfs/build-fb
popd
export FSL_PLATFORM_NAME=Yocto
export FSL_GRAPHICS_SDK=$WORKSPACE
. prepare.sh
#G2D disabled for now
FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenVG,OpenCL,OpenCL1.1,EarlyAccess] --Variants [WindowSystem=FB] -- install -j 4
