#!/bin/bash

# Build recipe caching setup
localJobName=$JOB_NAME
localJobName+='_'
localJobName+=$EXECUTOR_NUMBER

if [ ! -n "${FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR+1}" ]; then
  if [ -n "${FSL_JENKINS_THIRD_PARTY_INSTALL+1}" ]; then
    export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=$FSL_JENKINS_THIRD_PARTY_INSTALL/WorkspaceInstall/$EXECUTOR_NUMBER
  elif [ -n "${HOME+1}" ] && [ "$HOME" != "/" ]; then
    export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=$HOME/jenkins_thirdparty_cache/WorkspaceInstall/$EXECUTOR_NUMBER
  else
    echo FSL_JENKINS_THIRD_PARTY_INSTALL not configured, using defalt settings for FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR.
  fi
fi

if [ ! -n "${FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR+1}" ]; then
  if [ -n "${FSL_JENKINS_THIRD_PARTY_INSTALL+1}" ]; then
    export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR=$FSL_JENKINS_THIRD_PARTY_INSTALL/SharedReadonlyCache
  else
    echo FSL_JENKINS_THIRD_PARTY_INSTALL not configured, using defalt settings for FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR.
  fi
fi

echo - FSL_JENKINS_THIRD_PARTY_INSTALL as $FSL_JENKINS_THIRD_PARTY_INSTALL
echo - FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR as $FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR
echo - FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR as $FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR

# Start building
export FSL_GRAPHICS_SDK=$WORKSPACE

echo Ensuring dirs exist
mkdir -p $FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR
mkdir -p $WORKSPACE/build

echo Setting up the demoframework

source prepare.sh  

# Since a specific executor can run build multiple version of the %JOB_NAME% and there is no way 
# to get information about that to create a unique directory name based on 'JOB_NAME' 'EXECUTOR_NUMBER' and this 'magic workspace #'
# we force claim the install area instead bypassing a security check
FslBuildExternal.py --ForceClaimInstallArea --VoidBuild
