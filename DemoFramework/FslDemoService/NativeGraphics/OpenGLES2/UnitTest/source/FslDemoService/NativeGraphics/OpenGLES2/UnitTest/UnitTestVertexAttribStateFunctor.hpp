#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_UNITTEST_FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_UNITTEST_UNITTESTVERTEXATTRIBSTATEFUNCTOR_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_UNITTEST_FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_UNITTEST_UNITTESTVERTEXATTRIBSTATEFUNCTOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexAttribState.hpp>
#include <GLES2/gl2.h>
#include <array>

namespace Fsl::GLES2
{
  struct UnitTestVertexAttribStateFunctor
  {
    // NOLINTNEXTLINE(readability-identifier-naming)
    static std::array<VertexAttribState, 32> GlobalState;

    static void Populate()
    {
      for (std::size_t i = 0; i < GlobalState.size(); ++i)
      {
        const auto size = NumericCast<GLint>((i % 3) + 1);
        const auto type = NumericCast<GLint>((i + 1) * 10);
        const bool normalized = (i % 2) != 0;
        const auto stride = NumericCast<GLint>(1000 + i);
        const GLvoid* pointer = reinterpret_cast<const GLvoid*>(NumericCast<uintptr_t>(i % 12));
        BasicVertexAttribState basicState(size, type, normalized, stride, pointer);
        GlobalState[i] = VertexAttribState((i % 1) == 0, basicState);
      }
    }

    static inline VertexAttribState GetAttrib(const GLuint attribIndex) noexcept
    {
      if (attribIndex >= GlobalState.size())
      {
        std::abort();
      }
      return GlobalState[attribIndex];
    }

    static inline void SetAttribEnabled(const GLuint attribIndex, const bool enabled) noexcept
    {
      if (attribIndex >= GlobalState.size())
      {
        std::abort();
      }
      GlobalState[attribIndex].Enabled = enabled;
    }

    static inline void SetBasicAttrib(const GLuint attribIndex, const BasicVertexAttribState& state) noexcept
    {
      if (attribIndex >= GlobalState.size())
      {
        std::abort();
      }
      GlobalState[attribIndex].Basic = state;
    }
  };
}

#endif
