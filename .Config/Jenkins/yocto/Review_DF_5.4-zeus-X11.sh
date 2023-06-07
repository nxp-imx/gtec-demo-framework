#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-internal-xwayland/5.4-zeus/environment-setup-aarch64-poky-linux

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

# all
#FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures [EGL,ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,G2D,OpenCL1.1,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.2,Vulkan] --Variants [WindowSystem=X11] -c install --CMakeInstallPrefix bin
# No OpenCV, Vulkan
FslBuild.py --noGitHash --debug -t sdk -vv --BuildTime --UseFeatures [ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,G2D,OpenCL1.2,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.2,HW_CAMERA_MIPI8X] --Variants [WindowSystem=X11] -c install --CMakeInstallPrefix bin
