#!/bin/bash
set -o errexit
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
FslBuildCheck.py -u [Vulkan] -vv
FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenCL,OpenCL1.1,OpenCL1.2,Vulkan,EarlyAccess] -- -j 4
