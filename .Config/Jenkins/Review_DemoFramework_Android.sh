#!/bin/bash
set -o errexit
export FSL_GRAPHICS_SDK=$WORKSPACE
export FSL_PLATFORM_NAME=Android
. prepare.sh
FslBuildGen.py -t sdk -vv
