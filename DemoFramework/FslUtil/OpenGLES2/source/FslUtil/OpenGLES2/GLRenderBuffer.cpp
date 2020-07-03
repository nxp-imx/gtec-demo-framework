/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <FslUtil/OpenGLES2/GLRenderBuffer.hpp>

#include <algorithm>

namespace Fsl
{
  namespace GLES2
  {
    GLRenderBuffer::GLRenderBuffer()
      : m_handle(GLValues::INVALID_HANDLE)
    {
    }


    GLRenderBuffer::GLRenderBuffer(const PxSize2D& size, const GLenum format)
      : m_handle(GLValues::INVALID_HANDLE)
    {
      Reset(size, format);
    }


    GLRenderBuffer::~GLRenderBuffer()
    {
      Reset();
    }


    void GLRenderBuffer::Reset() noexcept
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        glDeleteRenderbuffers(1, &m_handle);
        m_handle = GLValues::INVALID_HANDLE;
      }
    }


    void GLRenderBuffer::Reset(const PxSize2D& size, const GLenum format)
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        Reset();
      }

      // Create a renderbuffer and allocate storage for it
      GL_CHECK(glGenRenderbuffers(1, &m_handle));
      GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, m_handle));
      GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, format, size.Width(), size.Height()));
    }
  }
}
