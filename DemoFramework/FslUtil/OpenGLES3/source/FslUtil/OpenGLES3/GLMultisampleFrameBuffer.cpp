/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLMultisampleFrameBuffer.hpp>
#include <algorithm>
#include <utility>

namespace Fsl::GLES3
{
  // move assignment operator
  GLMultisampleFrameBuffer& GLMultisampleFrameBuffer::operator=(GLMultisampleFrameBuffer&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_handle = other.m_handle;
      m_size = other.m_size;
      m_colorRenderBuffer = std::move(other.m_colorRenderBuffer);
      m_depthStencilRenderBuffer = std::move(other.m_depthStencilRenderBuffer);

      // Remove the data from other
      other.m_handle = GLValues::InvalidHandle;
      other.m_size = {};
    }
    return *this;
  }


  // Transfer ownership from other to this
  GLMultisampleFrameBuffer::GLMultisampleFrameBuffer(GLMultisampleFrameBuffer&& other) noexcept
    : m_handle(other.m_handle)
    , m_size(other.m_size)
    , m_colorRenderBuffer(std::move(other.m_colorRenderBuffer))
    , m_depthStencilRenderBuffer(std::move(other.m_depthStencilRenderBuffer))
  {
    // Remove the data from other
    other.m_handle = GLValues::InvalidHandle;
    other.m_size = {};
  }


  GLMultisampleFrameBuffer::GLMultisampleFrameBuffer()
    : m_handle(GLValues::InvalidHandle)
  {
  }


  GLMultisampleFrameBuffer::GLMultisampleFrameBuffer(const PxSize2D& size, const GLsizei maxSamples, const GLenum colorFormat,
                                                     const GLenum depthBufferFormat)
    : GLMultisampleFrameBuffer()
  {
    Reset(size, maxSamples, colorFormat, depthBufferFormat);
  }


  GLMultisampleFrameBuffer::~GLMultisampleFrameBuffer()
  {
    Reset();
  }


  void GLMultisampleFrameBuffer::Reset() noexcept
  {
    if (m_handle != GLValues::InvalidHandle)
    {
      glDeleteFramebuffers(1, &m_handle);
      m_handle = GLValues::InvalidHandle;
      m_size = {};
      m_colorRenderBuffer.Reset();
      m_depthStencilRenderBuffer.Reset();
    }
  }


  void GLMultisampleFrameBuffer::Reset(const PxSize2D& size, const GLsizei maxSamples, const GLenum colorFormat, const GLenum depthBufferFormat)
  {
    if (m_handle != GLValues::InvalidHandle)
    {
      Reset();
    }

    m_size = size;

    // Create the framebuffer and bind it.
    GL_CHECK(glGenFramebuffers(1, &m_handle));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_handle));

    // Create renderbuffer for color attachment
    m_colorRenderBuffer.Reset(size, maxSamples, colorFormat);

    // Create renderbuffer for depth and stencil
    m_depthStencilRenderBuffer.Reset(size, maxSamples, depthBufferFormat);

    // Attach renderbuffers to FBO
    GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderBuffer.Get()));
    switch (depthBufferFormat)
    {
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F:
      GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRenderBuffer.Get()));
      break;
    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
      GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRenderBuffer.Get()));
      break;
    case GL_STENCIL_INDEX8:
      GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRenderBuffer.Get()));
      break;
    default:
      throw NotSupportedException("depthBufferFormat not supported");
    }

    // Test the framebuffer for completeness. This test only needs to be performed when the framebuffer's configuration changes.
    const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
      throw GLESGraphicsException("glCheckFramebufferStatus", UncheckedNumericCast<int32_t>(status), __FILE__, __LINE__);
    }

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  }
}
