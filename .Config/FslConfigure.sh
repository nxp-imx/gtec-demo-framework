#!/bin/bash
if [ ! -n "${FSL_PLATFORM_NAME+1}" ]; then
  # Crude platform check, but it works for now.
  if [ -n "${BUILDDIR+1}" ]; then
    export FSL_PLATFORM_NAME=Yocto
  elif [ -n "${SDKTARGETSYSROOT+1}" ]; then
    export FSL_PLATFORM_NAME=Yocto
    if [ -z "${ROOTFS}" ]; then
      export ROOTFS=$SDKTARGETSYSROOT
    fi
  elif [ -n "${QNX_TARGET+1}" ]; then
    export FSL_PLATFORM_NAME=QNX
  else
    export FSL_PLATFORM_NAME=Ubuntu
  fi
fi

echo PlatformName: $FSL_PLATFORM_NAME

# Very simple FSL_HOST_ROOTFS auto configuration
allowHostRootFSPathCorrection=false
if [ ! -n "${FSL_HOST_ROOTFS+1}" ]; then
  if [ -n "${FSL_PLATFORM_NAME+1}" ]; then
    if [ "${FSL_PLATFORM_NAME}" = "Yocto" ]; then
      if [ -n "${OECORE_NATIVE_SYSROOT+1}" ]; then
        export FSL_HOST_ROOTFS=$OECORE_NATIVE_SYSROOT
        allowHostRootFSPathCorrection=true
      else
        echo WARNING: Could not auto configure the environment variable FSL_HOST_ROOTFS. If cross compiling point it to the Yocto host root fs.
      fi
    fi
  fi
fi

# Ensure that FSL_HOST_ROOTFS does not end with a slash
if [ -n "${FSL_HOST_ROOTFS+1}" ]; then
  if [ ! -z "${FSL_HOST_ROOTFS}" ]; then
    if [[ "${FSL_HOST_ROOTFS}" == */ ]]; then
      if [ "$allowHostRootFSPathCorrection" = true ]; then
        export FSL_HOST_ROOTFS="${FSL_HOST_ROOTFS%/}"
      else
        echo ERROR: FSL_HOST_ROOTFS can not end with a slash but was defind as \'${FSL_HOST_ROOTFS}\'
        exit 20
      fi
    fi
  fi  
fi
