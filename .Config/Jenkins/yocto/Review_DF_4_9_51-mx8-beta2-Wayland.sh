#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-internal-xwayland/4.9.51-mx8-beta/environment-setup-aarch64-poky-linux

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

# all
#FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL1.1,OpenCV,OpenCV3,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.1,Vulkan] --Variants [WindowSystem=Wayland] -- install
# No OpenCV
FslBuild.py --debug -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL1.1,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.1,Vulkan,HW_CAMERA_MIPI8X] --Variants [WindowSystem=Wayland] -- install
