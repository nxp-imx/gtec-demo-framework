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

if [ ! -n "${FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR+1}" ]; then
export FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=$FSL_GRAPHICS_SDK/.Thirdparty
echo WARNING: Environment variable FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR was not defined, so we force defined it.
echo          Its highly recommended to manually define this variable so you can reuse the thirdparty libs between builds and repositories.
echo          Read Doc\BuildingExternals.md for detailed information about this environment variable.
fi

echo FSL_GRAPHICS_SDK=$FSL_GRAPHICS_SDK
echo FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=$FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR
echo FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=$FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR
