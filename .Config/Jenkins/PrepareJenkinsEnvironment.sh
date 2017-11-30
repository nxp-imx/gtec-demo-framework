#!/bin/bash

# Build recipe caching setup
localJobName=$JOB_NAME
localJobName+='_'
localJobName+=$EXECUTOR_NUMBER

if [ -n "${FSL_JENKINS_THIRD_PARTY_INSTALL+1}" ]; then
  export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=$FSL_JENKINS_THIRD_PARTY_INSTALL\WorkspaceInstall\$localJobName
  export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR=$FSL_JENKINS_THIRD_PARTY_INSTALL\SharedReadonlyCache
elif [ -n "${HOME+1}" ]; then
  export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=$HOME/jenkins_thirdparty_cache/WorkspaceInstall/$localJobName
  export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR=$HOME/jenkins_thirdparty_cache/SharedReadonlyCache
else
  echo FSL_JENKINS_THIRD_PARTY_INSTALL not configured, using defalt settings for third party libs.
fi

if [ -n "${FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR+1}" ]; then
  echo Using jenkins third party install directory at $FSL_JENKINS_THIRD_PARTY_INSTALL
  echo - Defining FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR as $FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR
  echo - Defining FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR as $FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR
fi

# AMD OpenCL setup
export AMDAPPSDKROOT=/opt/AMDAPPSDK-2.9-1
export LD_LIBRARY_PATH=/opt/AMDAPPSDK-2.9-1/lib/x86_64/
# Vulkan
#pushd /home/shared/vulkan/VulkanSDK/1.0.26.0
#source setup-env.sh
#popd
# Vulkan config (for some reason the above setup does not work)
export VULKAN_SDK=/home/shared/vulkan/VulkanSDK/1.0.26.0/x86_64
export PATH=$VULKAN_SDK/bin:$PATH
export LD_LIBRARY_PATH=$VULKAN_SDK/lib:$LD_LIBRARY_PATH
export VK_LAYER_PATH=$VULKAN_SDK/etc/explicit_layer.d
export LIBRARY_PATH=$VULKAN_SDK/lib:$LIBRARY_PATH
echo VULKAN_SDK=$VULKAN_SDK
# Start building
export FSL_GRAPHICS_SDK=$WORKSPACE

source prepare.sh  
