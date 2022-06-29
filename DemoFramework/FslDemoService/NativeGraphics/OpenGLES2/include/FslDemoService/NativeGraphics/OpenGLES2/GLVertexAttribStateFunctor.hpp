#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_GLVERTEXATTRIBSTATEFUNCTOR_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_GLVERTEXATTRIBSTATEFUNCTOR_HPP
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

#include <FslDemoService/NativeGraphics/OpenGLES2/VertexAttribState.hpp>
#include <GLES2/gl2.h>

namespace Fsl::GLES2
{
  struct GLVertexAttribStateFunctor
  {
    static inline VertexAttribState GetAttrib(const GLuint attribIndex) noexcept
    {
      VertexAttribState state;

      // Query OpenGL ES about its current state
      GLint enabled = 0;
      glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
      glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_SIZE, &state.Basic.Size);
      glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_TYPE, &state.Basic.Type);
      GLint normalized = 0;
      glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
      glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &state.Basic.Stride);
      GLvoid* pointer{nullptr};
      glGetVertexAttribPointerv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
      state.Enabled = (enabled != GL_FALSE);
      state.Basic.Normalized = (normalized != GL_FALSE);
      state.Basic.Pointer = pointer;
      return state;
    }

    static inline void SetAttribEnabled(const GLuint attribIndex, const bool enabled) noexcept
    {
      if (enabled)
      {
        glEnableVertexAttribArray(attribIndex);
      }
      else
      {
        glDisableVertexAttribArray(attribIndex);
      }
    }

    static inline void SetBasicAttrib(const GLuint attribIndex, const BasicVertexAttribState& state) noexcept
    {
      const GLboolean normalized = state.Normalized ? GL_TRUE : GL_FALSE;
      glVertexAttribPointer(attribIndex, state.Size, state.Type, normalized, state.Stride, state.Pointer);
    }
  };
}

#endif
