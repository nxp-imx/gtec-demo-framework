#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-xwayland/4.9-rocko/environment-setup-aarch64-poky-linux

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

FslBuild.py --debug -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2] --Variants [WindowSystem=FB] --Recipes [*] -c install --CMakeInstallPrefix bin
