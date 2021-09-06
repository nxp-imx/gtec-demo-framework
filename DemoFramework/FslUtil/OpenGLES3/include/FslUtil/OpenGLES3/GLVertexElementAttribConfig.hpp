#ifndef FSLUTIL_OPENGLES3_GLVERTEXELEMENTATTRIBCONFIG_HPP
#define FSLUTIL_OPENGLES3_GLVERTEXELEMENTATTRIBCONFIG_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <GLES3/gl3.h>

namespace Fsl
{
  class VertexDeclarationSpan;

  namespace GLES3
  {
    struct GLVertexElementAttribConfig
    {
      GLuint AttribIndex{0};
      GLint Size{0};
      GLenum Type{0};
      GLboolean Normalized{GL_FALSE};
      const GLvoid* Pointer{nullptr};

      constexpr GLVertexElementAttribConfig() = default;
      constexpr GLVertexElementAttribConfig(const GLuint attribIndex, const GLint size, const GLenum type, const GLboolean normalized,
                                            const GLvoid* const pointer)
        : AttribIndex(attribIndex)
        , Size(size)
        , Type(type)
        , Normalized(normalized)
        , Pointer(pointer)
      {
      }

      constexpr bool operator==(const GLVertexElementAttribConfig& rhs) const noexcept
      {
        return AttribIndex == rhs.AttribIndex && Size == rhs.Size && Type == rhs.Type && Normalized == rhs.Normalized && Pointer == rhs.Pointer;
      }

      constexpr bool operator!=(const GLVertexElementAttribConfig& rhs) const noexcept
      {
        return !(*this == rhs);
      }
    };
  }
}

#endif
