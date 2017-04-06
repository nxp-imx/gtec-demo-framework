#ifndef FSLUTILOPENVX1_1_VALUES_HPP
#define FSLUTILOPENVX1_1_VALUES_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

// Based on RapidOpenVX https://github.com/Unarmed1000/RapidOpenVX with permission.

#include <VX/vx.h>

namespace Fsl
{
  namespace OpenVX
  {
  namespace Values
  {
    static const vx_array INVALID_ARRAY = nullptr;
    static const vx_context INVALID_CONTEXT = nullptr;
    static const vx_convolution INVALID_CONVOLUTION = nullptr;
    static const vx_delay INVALID_DELAY = nullptr;
    static const vx_distribution INVALID_DISTRIBUTION = nullptr;
    static const vx_graph INVALID_GRAPH = nullptr;
    static const vx_image INVALID_IMAGE = nullptr;
    static const vx_lut INVALID_LUT = nullptr;
    static const vx_matrix INVALID_MATRIX = nullptr;
    static const vx_object_array INVALID_OBJECT_ARRAY = nullptr;
    static const vx_pyramid INVALID_PYRAMID = nullptr;
    static const vx_remap INVALID_REMAP = nullptr;
    static const vx_scalar INVALID_SCALAR = nullptr;
    static const vx_threshold INVALID_THRESHOLD = nullptr;
  };
  }
}

#endif
