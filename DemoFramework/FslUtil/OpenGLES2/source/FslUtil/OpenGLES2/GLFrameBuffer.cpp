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

#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <FslUtil/OpenGLES2/GLFrameBuffer.hpp>
#include <algorithm>
#include <utility>

namespace Fsl::GLES2
{
  // move assignment operator
  GLFrameBuffer& GLFrameBuffer::operator=(GLFrameBuffer&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_handle = other.m_handle;
      m_size = other.m_size;
      m_depthRenderBuffer = std::move(other.m_depthRenderBuffer);
      m_stencilRenderBuffer = std::move(other.m_stencilRenderBuffer);
      m_texture = std::move(other.m_texture);

      // Remove the data from other
      other.m_handle = GLValues::InvalidHandle;
      other.m_size = {};
    }
    return *this;
  }


  // Transfer ownership from other to this
  GLFrameBuffer::GLFrameBuffer(GLFrameBuffer&& other) noexcept
    : m_handle(other.m_handle)
    , m_size(other.m_size)
    , m_depthRenderBuffer(std::move(other.m_depthRenderBuffer))
    , m_stencilRenderBuffer(std::move(other.m_stencilRenderBuffer))
    , m_texture(std::move(other.m_texture))
  {
    // Remove the data from other
    other.m_handle = GLValues::InvalidHandle;
    other.m_size = {};
  }


  GLFrameBuffer::GLFrameBuffer()
    : m_handle(GLValues::InvalidHandle)
  {
  }


  GLFrameBuffer::GLFrameBuffer(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams)
    : GLFrameBuffer()
  {
    Reset(size, textureParameters, texImageParams);
  }


  GLFrameBuffer::GLFrameBuffer(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                               const GLenum depthBufferFormat)
    : GLFrameBuffer()
  {
    Reset(size, textureParameters, texImageParams, depthBufferFormat);
  }


  GLFrameBuffer::GLFrameBuffer(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                               const GLenum depthBufferFormat, const GLenum stencilBufferFormat)
    : GLFrameBuffer()
  {
    Reset(size, textureParameters, texImageParams, depthBufferFormat, stencilBufferFormat);
  }


  GLFrameBuffer::GLFrameBuffer(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                               const GLenum depthBufferFormat, const GLenum stencilBufferFormat, const Flags::type bufferFlags)
    : GLFrameBuffer()
  {
    Reset(size, textureParameters, texImageParams, depthBufferFormat, stencilBufferFormat, bufferFlags);
  }


  GLFrameBuffer::~GLFrameBuffer()
  {
    Reset();
  }


  void GLFrameBuffer::Reset() noexcept
  {
    if (m_handle != GLValues::InvalidHandle)
    {
      glDeleteFramebuffers(1, &m_handle);
      m_handle = GLValues::InvalidHandle;
      m_size = {};
      m_depthRenderBuffer.Reset();
      m_stencilRenderBuffer.Reset();
      m_texture.Reset();
    }
  }


  void GLFrameBuffer::Reset(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams)
  {
    Reset(size, textureParameters, texImageParams, 0, 0, Flags::ColorBuffer);
  }


  void GLFrameBuffer::Reset(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                            const GLenum depthBufferFormat)
  {
    Reset(size, textureParameters, texImageParams, depthBufferFormat, 0, Flags::ColorBuffer | Flags::DepthBuffer);
  }


  void GLFrameBuffer::Reset(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                            const GLenum depthBufferFormat, const GLenum stencilBufferFormat)
  {
    Reset(size, textureParameters, texImageParams, depthBufferFormat, stencilBufferFormat,
          Flags::ColorBuffer | Flags::DepthBuffer | Flags::StencilBuffer);
  }


  void GLFrameBuffer::Reset(const PxSize2D& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                            const GLenum depthBufferFormat, const GLenum stencilBufferFormat, const Flags::type bufferFlags)
  {
    if (m_handle != GLValues::InvalidHandle)
    {
      Reset();
    }

    const Flags flags(bufferFlags);

    m_size = size;

    // Create the framebuffer and bind it.
    GL_CHECK(glGenFramebuffers(1, &m_handle));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_handle));

    if (flags.IsEnabled(Flags::DepthBuffer))
    {
      m_depthRenderBuffer.Reset(size, depthBufferFormat);
      GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer.Get()));
    }

    if (flags.IsEnabled(Flags::StencilBuffer))
    {
      m_stencilRenderBuffer.Reset(size, stencilBufferFormat);
      GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_stencilRenderBuffer.Get()));
    }

    // create the texture
    GLuint hTex = 0;
    GL_CHECK(glGenTextures(1, &hTex));
    m_texture.Reset(hTex, size);
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, hTex));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureParameters.MinFilter));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureParameters.MagFilter));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureParameters.WrapS));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureParameters.WrapT));

    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, texImageParams.InternalFormat, size.RawWidth(), size.RawHeight(), 0, texImageParams.Format,
                          texImageParams.Type, nullptr));
    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hTex, 0));

    // Test the framebuffer for completeness. This test only needs to be performed when the framebuffer's configuration changes.
    const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
      throw GLESGraphicsException("glCheckFramebufferStatus", UncheckedNumericCast<int32_t>(status), __FILE__, __LINE__);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}
