#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_BASICVERTEXATTRIBSTATE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_BASICVERTEXATTRIBSTATE_HPP
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
  namespace GLES3
  {
    struct BasicVertexAttribState
    {
      // The actual state of the attrib index
      GLint Size{0};
      GLint Type{0};
      bool Normalized{false};
      GLint Stride{0};
      const GLvoid* Pointer{nullptr};

      constexpr BasicVertexAttribState() noexcept = default;
      constexpr BasicVertexAttribState(const GLint size, const GLint type, const bool normalized, const GLint stride, const GLvoid* pointer) noexcept
        : Size(size)
        , Type(type)
        , Normalized(normalized)
        , Stride(stride)
        , Pointer(pointer)
      {
      }

      constexpr bool operator==(const BasicVertexAttribState& rhs) const noexcept
      {
        return Size == rhs.Size && Type == rhs.Type && Normalized == rhs.Normalized && Stride == rhs.Stride && Pointer == rhs.Pointer;
      }

      constexpr bool operator!=(const BasicVertexAttribState& rhs) const noexcept
      {
        return !(*this == rhs);
      }
    };
  }
}

#endif
