#!/usr/bin/env bash
export FSL_GRAPHICS_SDK=$(pwd)
PATH=$PATH:$FSL_GRAPHICS_SDK/.Config

. FslConfigure.sh
if ! [ $? -eq 0 ]; then
  echo "FSLConfigure.sh failed"
  exit 1
fi

if [ ! -n "${FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR+1}" ]; then
export FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=$FSL_GRAPHICS_SDK/Android
fi

echo FSL_GRAPHICS_SDK=$FSL_GRAPHICS_SDK
echo FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=$FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR
