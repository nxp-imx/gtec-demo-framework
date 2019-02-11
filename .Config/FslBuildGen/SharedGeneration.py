#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
#      its contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************************************************************************************

GEN_BUILD_ENV_VARIANT_SETTING = "FSLBUILD_VARIANT_"
# Deprecated: its not about the feature but the variant instead
#             So the code generation engine needs to know if the 'feature' config needs the variant info or not :/
GEN_BUILD_ENV_FEATURE_SETTING = "FSLBUILD_FEATURE_"

class ToolEnvironmentVariableName(object):
    FSL_GRAPHICS_SDK = "FSL_GRAPHICS_SDK"
    FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR = "FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR"

class ToolAddedVariantConfigOption(object):
    Debug = "Debug"
    Release = "Release"
    Coverage = "Coverage"


class ToolAddedVariant(object):
    CONFIG = "config"

class ToolAddedVariantOptions(object):
    CONFIG = [ToolAddedVariantConfigOption.Debug, ToolAddedVariantConfigOption.Release]


# Represents 'automatic' variants added by the tool


GEN_MAGIC_VARIANT_ANDROID_ABI = "ANDROID_ABI"

class AndroidABIOption:
    All = "all"
    ArmeAbiV7a = "armeabi-v7a"
    Arm64V8a = "arm64-v8a"
    X86 = "x86"
    X86_64 = "x86_64"
    DeprecatedArmeAbi = "armeabi"  # Deprecated in r16.
    DeprecatedMips = "mips"        # Deprecated in r16.
    DeprecatedMips64 = "mips64"    # Deprecated in r16.
