#ifndef FSLGRAPHICS_VERTICES_VERTEXELEMENTFORMAT_HPP
#define FSLGRAPHICS_VERTICES_VERTEXELEMENTFORMAT_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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


namespace Fsl
{
  enum class VertexElementFormat
  {
    // One component, 32-bit floating point
    Single,
    // two components, 32-bit floating point
    Vector2,
    // three components, 32-bit floating point
    Vector3,
    // four components, 32-bit floating point
    Vector4,
    // four by four components, 32-bit floating point
    Matrix4x4,

    // one component, 8-bit unsigned byte. The components are unsigned normalized values in the range [0,1]
    X8_UNORM,
    // one component, 8-bit unsigned byte. The components are unsigned integer values in the range [0, pow(2,8)-1]
    X8_UINT,

    // two components, 8-bit unsigned byte. The components are unsigned normalized values in the range [0,1]
    X8Y8_UNORM,
    // two components, 8-bit unsigned byte. The components are unsigned integer values in the range [0, pow(2,8)-1]
    X8Y8_UINT,

    // three components, 8-bit unsigned byte. The components are unsigned normalized values in the range [0,1]
    X8Y8Z8_UNORM,
    // three components, 8-bit unsigned byte. The components are unsigned integer values in the range [0, pow(2,8)-1]
    X8Y8Z8_UINT,

    // four components, 8-bit unsigned byte. The components are unsigned normalized values in the range [0,1]
    X8Y8Z8W8_UNORM,
    // four components, 8-bit unsigned byte. The components are unsigned integer values in the range [0, pow(2,8)-1]
    X8Y8Z8W8_UINT
  };
}

#endif
