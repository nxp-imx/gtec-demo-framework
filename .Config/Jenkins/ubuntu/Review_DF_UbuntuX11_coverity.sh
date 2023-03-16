#!/bin/bash
echo ********************
echo *** Start script ***
echo ********************
set -o errexit

if [ ! -n "${FSL_CI_BUILD_PARAM+1}" ]; then
#export FSL_CI_BUILD_PARAM="-c install --CMakeInstallPrefix bin"
export FSL_CI_BUILD_PARAM=""
fi 

if [ ! -z "${FSL_CI_COVERITY_PARAM}" ]; then
PATH=$PATH:$FSL_CI_COVERITY_PARAM
fi 

echo *****************************
echo *** Preparing environment ***
echo *****************************

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
export FSL_CI_FEATURES=[EarlyAccess,EGL,GoogleUnitTest,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenGLES3.2,OpenVX,OpenVX1.2,Vulkan,Lib_NlohmannJson]
fi


echo *******************
echo *** Build clean ***
echo *******************
echo Cleaning up build to ensure coverity gets executed on all files.

FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES -c clean

echo ******************************************
echo *** Coverity: Removing previous builds ***
echo ******************************************

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

echo ***************************

if [ -n "${FSL_COVERITY_BIN_DIR+1}" ]; then
  echo Adding coverity directory at $FSL_COVERITY_BIN_DIR to path
  PATH=$PATH:$FSL_COVERITY_BIN_DIR
fi

echo ***************************
echo *** Coverity: Configure ***
echo ***************************

cov-configure --clang

echo ************************
echo *** Coverity: Build  ***
echo ************************

cov-build --dir $FSL_COVERITY_DIR FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES $FSL_CI_BUILD_PARAM --CMakeConfigArgs="-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++"
# cov-build --dir $FSL_COVERITY_DIR FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES $FSL_CI_BUILD_PARAM --CMakeConfigGlobalArgs="-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++"

echo **************************
echo *** Coverity: Analyze  ***
echo **************************

cov-analyze --dir $FSL_COVERITY_DIR --all

echo *********************************
echo *** Coverity: Generate report ***
echo *********************************

# Generate a html report for now
cov-format-errors --dir $FSL_COVERITY_DIR --html-output $FSL_COVERITY_HTML_DIR

# Archive the report
rm -f $WORKSPACE/coverity-html-report.tar.gz
tar -czvf $WORKSPACE/coverity-html-report.tar.gz $FSL_COVERITY_HTML_DIR

echo ******************
echo *** End script ***
echo ******************
