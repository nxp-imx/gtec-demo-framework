#ifndef FSLUTIL_OPENGLES3_GLVERTEXATTRIBLINK_HPP
#define FSLUTIL_OPENGLES3_GLVERTEXATTRIBLINK_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Vertices/VertexElementUsage.hpp>
#include <FslUtil/OpenGLES3/Common.hpp>
#include <GLES3/gl3.h>

namespace Fsl::GLES3
{
  struct GLVertexAttribLink
  {
    // The shader location
    GLint AttribIndex{0};
    // The vertex location based on the vertex declaration
    uint32_t VertexElementIndex{0};

    constexpr GLVertexAttribLink() noexcept = default;

    constexpr GLVertexAttribLink(const GLint attribIndex, const uint32_t vertexElementIndex) noexcept
      : AttribIndex(attribIndex)
      , VertexElementIndex(vertexElementIndex)
    {
    }

    constexpr bool operator==(const GLVertexAttribLink& rhs) const noexcept
    {
      return AttribIndex == rhs.AttribIndex && VertexElementIndex == rhs.VertexElementIndex;
    }

    constexpr bool operator!=(const GLVertexAttribLink& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
