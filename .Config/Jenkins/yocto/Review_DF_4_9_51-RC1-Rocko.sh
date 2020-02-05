#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-xwayland/4.9-rocko/environment-setup-aarch64-poky-linux

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

FslBuild.py --debug -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2] --Variants [WindowSystem=FB] --Recipes [*] -c install --CMakeInstallPrefix bin
#FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.1,Vulkan] --Variants [WindowSystem=FB] -c install --CMakeInstallPrefix bin
#FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,OpenCL,OpenCL1.2,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.1,Vulkan] --Variants [WindowSystem=FB] -c install --CMakeInstallPrefix bin
