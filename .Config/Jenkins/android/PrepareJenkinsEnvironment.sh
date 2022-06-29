#!/bin/bash

# Build recipe caching setup
localJobName=$JOB_NAME
localJobName+='_'
localJobName+=$EXECUTOR_NUMBER

echo - localJobName: $localJobName
echo - HOME: $HOME
echo - FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR: $FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR
echo - FSL_JENKINS_THIRD_PARTY_INSTALL: $FSL_JENKINS_THIRD_PARTY_INSTALL

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
export FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=$FSL_GRAPHICS_SDK/Android
export FSL_PLATFORM_NAME=Android


echo Ensuring dirs exist
mkdir -p $FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR
mkdir -p $FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR

#echo Ensuring dirs keystore exist
#mkdir -p ~/.android
#if [ ! -f ~/.android/debug.keystore ]; then
#  echo Generating keystore begin
#  keytool -genkey -v -keystore ~/.android/debug.keystore -storepass android -alias androiddebugkey -keypass android -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=Android Debug,O=Android,C=US"
#  echo Generating keystore end
#else
#  echo Keystore exist
#fi

echo Setting up DemoFramework
source prepare.sh  

export FSL_TEST_REPORTS=$WORKSPACE/.Reports/UnitTests

# Since a specific executor can run build multiple version of the %JOB_NAME% and there is no way 
# to get information about that to create a unique directory name based on 'JOB_NAME' 'EXECUTOR_NUMBER' and this 'magic workspace #'
# we force claim the install area instead bypassing a security check
echo Claiming persistent cache
FslBuildExternal.py --noGitHash --ForceClaimInstallArea --VoidBuild
echo Prepare done
