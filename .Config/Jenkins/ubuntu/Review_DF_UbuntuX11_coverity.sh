#!/bin/bash
set -o errexit

if [ ! -n "${FSL_CI_BUILD_PARAM+1}" ]; then
export FSL_CI_BUILD_PARAM="-c install --CMakeInstallPrefix bin"
fi 

if [ ! -z "${FSL_CI_COVERITY_PARAM}" ]; then
PATH=$PATH:$FSL_CI_COVERITY_PARAM
fi 

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
export FSL_CI_FEATURES=[EarlyAccess,EGL,GoogleUnitTest,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenGLES3.2,OpenVX,OpenVX1.1,Vulkan]
fi

echo Cleaning up build to ensure coverity gets executed on all files.

FslBuild.py -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES -c clean

echo ******************************
echo *** Running coverity build ***
echo ******************************

export FSL_COVERITY_DIR=/data/coverity
export FSL_COVERITY_HTML_DIR=$WORKSPACE/coverity.html

if [[ -d $FSL_COVERITY_DIR ]]; then
  echo Removing coverity data directory at $FSL_COVERITY_DIR
  rm -rf $FSL_COVERITY_DIR
fi
if [[ -d $FSL_COVERITY_HTML_DIR ]]; then
  echo Removing coverity html directory at $FSL_COVERITY_HTML_DIR
  rm -rf $FSL_COVERITY_HTML_DIR
fi

echo FSL_COVERITY_DIR=$FSL_COVERITY_DIR

cov-configure --clang
cov-build --dir $FSL_COVERITY_DIR FslBuild.py -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES $FSL_CI_BUILD_PARAM --CMakeConfigArgs="-DCMAKE_TOOLCHAIN_FILE=$FSL_GRAPHICS_SDK/.Config/Jenkins/ubuntu/clang-toolchain.cmake"
cov-analyze --dir $FSL_COVERITY_DIR --all
# Generate a html report for now
cov-format-errors --dir $FSL_COVERITY_DIR --html-output $FSL_COVERITY_HTML_DIR
