#!/bin/bash
echo Configuring QNX build environment
pushd /home/shared/qnx660
source qnx660-env.sh 
popd

echo Preparing to build
set -o errexit
export FSL_PLATFORM_NAME=QNX
export FSL_GRAPHICS_SDK=$WORKSPACE
. prepare.sh
FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,OpenGLES2] --BuildThreads 4
