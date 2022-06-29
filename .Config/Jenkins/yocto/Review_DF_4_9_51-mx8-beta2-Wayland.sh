#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-internal-xwayland/4.9.51-mx8-beta/environment-setup-aarch64-poky-linux

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

# all
#FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL1.1,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.2,Vulkan] --Variants [WindowSystem=Wayland] -c install --CMakeInstallPrefix bin
# No OpenCV
FslBuild.py --noGitHash --debug -t sdk -vv --BuildTime --UseFeatures [EarlyAccess,EGL,G2D,OpenCL1.2,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.2,Vulkan,HW_CAMERA_MIPI8X] --Variants [WindowSystem=Wayland] --Recipes [*] -c install --CMakeInstallPrefix bin
