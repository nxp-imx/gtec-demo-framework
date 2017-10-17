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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLFrameBuffer.hpp>

#include <algorithm>

namespace Fsl
{
  namespace GLES3
  {

    GLFrameBuffer::GLFrameBuffer()
      : m_handle(GLValues::INVALID_HANDLE)
      , m_size()
      , m_colorRenderBuffer()
      , m_depthRenderBuffer()
      , m_stencilRenderBuffer()
    {
    }


    GLFrameBuffer::GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat)
      : m_handle(GLValues::INVALID_HANDLE)
      , m_size()
      , m_colorRenderBuffer()
      , m_depthRenderBuffer()
      , m_stencilRenderBuffer()
    {
      Reset(size, textureParameters, colorBufferFormat);
    }


    GLFrameBuffer::GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat, const GLenum depthBufferFormat)
      : m_handle(GLValues::INVALID_HANDLE)
      , m_size()
      , m_colorRenderBuffer()
      , m_depthRenderBuffer()
      , m_stencilRenderBuffer()
    {
      Reset(size, textureParameters, colorBufferFormat, depthBufferFormat);
    }


    GLFrameBuffer::GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat, const GLenum depthBufferFormat, const GLenum stencilBufferFormat)
      : m_handle(GLValues::INVALID_HANDLE)
      , m_size()
      , m_colorRenderBuffer()
      , m_depthRenderBuffer()
      , m_stencilRenderBuffer()
    {
      Reset(size, textureParameters, colorBufferFormat, depthBufferFormat, stencilBufferFormat);
    }


    GLFrameBuffer::GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat, const GLenum depthBufferFormat, const GLenum stencilBufferFormat, const Flags::type bufferFlags)
      : m_handle(GLValues::INVALID_HANDLE)
      , m_size()
      , m_colorRenderBuffer()
      , m_depthRenderBuffer()
      , m_stencilRenderBuffer()
    {
      Reset(size, textureParameters, colorBufferFormat, depthBufferFormat, stencilBufferFormat, bufferFlags);
    }


    GLFrameBuffer::~GLFrameBuffer()
    {
      Reset();
    }


    void GLFrameBuffer::Reset()
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        glDeleteFramebuffers(1, &m_handle);
        m_handle = GLValues::INVALID_HANDLE;
        m_size = Point2();
        m_colorRenderBuffer.Reset();
        m_depthRenderBuffer.Reset();
        m_stencilRenderBuffer.Reset();
        m_texture.Reset();
      }
    }


    void GLFrameBuffer::Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat)
    {
      Reset(size, textureParameters, colorBufferFormat, 0, 0, Flags::ColorBuffer);
    }


    void GLFrameBuffer::Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat, const GLenum depthBufferFormat)
    {
      Reset(size, textureParameters, colorBufferFormat, depthBufferFormat, 0, Flags::ColorBuffer | Flags::DepthBuffer);
    }


    void GLFrameBuffer::Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat, const GLenum depthBufferFormat, const GLenum stencilBufferFormat)
    {
      Reset(size, textureParameters, colorBufferFormat, depthBufferFormat, stencilBufferFormat, Flags::ColorBuffer | Flags::DepthBuffer | Flags::StencilBuffer);
    }


    void GLFrameBuffer::Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLenum colorBufferFormat, const GLenum depthBufferFormat, const GLenum stencilBufferFormat, const Flags::type bufferFlags)
    {
      if (m_handle != GLValues::INVALID_HANDLE)
        Reset();

      const Flags flags(bufferFlags);

      m_size = size;

      // Create the framebuffer and bind it.
      GL_CHECK(glGenFramebuffers(1, &m_handle));
      GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_handle));


      if (flags.IsEnabled(Flags::ColorBuffer))
      {
        // Create a color renderbuffer, allocate storage for it, and attach it to the framebuffer's color attachment point
        m_colorRenderBuffer.Reset(size, colorBufferFormat);
        GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderBuffer.GetHandle()));
      }

      if (flags.IsEnabled(Flags::DepthBuffer))
      {
        m_depthRenderBuffer.Reset(size, depthBufferFormat);
        GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer.GetHandle()));
      }

      if (flags.IsEnabled(Flags::StencilBuffer))
      {
        m_stencilRenderBuffer.Reset(size, stencilBufferFormat);
        GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_stencilRenderBuffer.GetHandle()));
      }

      // Test the framebuffer for completeness. This test only needs to be performed when the framebuffer's configuration changes.
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE)
        throw GraphicsException("glCheckFramebufferStatus failed");

      // create the texture
      GLuint hTex;
      GL_CHECK(glGenTextures(1, &hTex));
      m_texture.Reset(hTex, size);
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, hTex));

      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureParameters.MinFilter));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureParameters.MagFilter));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureParameters.WrapS));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureParameters.WrapT));

      GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.X, size.Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
      GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hTex, 0));

      // Test the framebuffer for completeness. This test only needs to be performed when the framebuffer's configuration changes.
      status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE)
        throw GraphicsException("glCheckFramebufferStatus failed");

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

  }
}
