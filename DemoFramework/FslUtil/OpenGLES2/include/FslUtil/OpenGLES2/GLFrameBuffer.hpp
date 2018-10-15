#ifndef FSLUTIL_OPENGLES2_GLFRAMEBUFFER_HPP
#define FSLUTIL_OPENGLES2_GLFRAMEBUFFER_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/OpenGLES2/Common.hpp>

#include <FslUtil/OpenGLES2/GLRenderBuffer.hpp>
#include <FslUtil/OpenGLES2/GLTexture.hpp>
#include <FslUtil/OpenGLES2/GLTextureImageParameters.hpp>
#include <FslBase/Attributes.hpp>

namespace Fsl
{
  namespace GLES2
  {
    class GLFrameBuffer
    {
      GLuint m_handle;
      Point2 m_size;
      GLRenderBuffer m_depthRenderBuffer;
      GLRenderBuffer m_stencilRenderBuffer;
      GLTexture m_texture;

    public:
      GLFrameBuffer(const GLFrameBuffer&) = delete;
      GLFrameBuffer& operator=(const GLFrameBuffer&) = delete;

      // move assignment operator
      GLFrameBuffer& operator=(GLFrameBuffer&& other) noexcept;
      // move constructor
      GLFrameBuffer(GLFrameBuffer&& other) noexcept;

      struct Flags
      {
        using type = uint32_t;

        enum Enum
        {
          ColorBuffer = 0x01,
          DepthBuffer = 0x02,
          StencilBuffer = 0x04,
        };


        uint32_t Value{0};
        Flags() = default;
        Flags(const uint32_t value)
          : Value(value)
        {
        }
        bool IsEnabled(const Enum value) const
        {
          return (Value & static_cast<uint32_t>(value)) != 0;
        }
      };

      //! @brief Create a uninitialized buffer
      GLFrameBuffer();

      //! @brief Create a framebuffer with a color buffer.
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      // (GL_DEPTH_COMPONENT16, or GL_STENCIL_INDEX8)
      GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams);

      //! @brief Create a framebuffer with a color and depth buffer.
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      //! @param depthBufferFormat the format of the framebuffers depth renderbuffer (for example GL_DEPTH_COMPONENT16)
      // (GL_DEPTH_COMPONENT16, or GL_STENCIL_INDEX8)
      GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                    const GLenum depthBufferFormat);

      //! @brief Create a framebuffer with a color, depth and stencil buffer.
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      //! @param depthBufferFormat the format of the framebuffers depth renderbuffer (for example GL_DEPTH_COMPONENT16)
      //! @param stencilBufferFormat the format of the framebuffers stencil renderbuffer (for example GL_STENCIL_INDEX8)
      GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                    const GLenum depthBufferFormat, const GLenum stencilBufferFormat);

      //! @brief Create a framebuffer with configurable buffers.
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      //! @param depthBufferFormat the format of the framebuffers depth renderbuffer (for example GL_DEPTH_COMPONENT16)
      //! @param stencilBufferFormat the format of the framebuffers stencil renderbuffer (for example GL_STENCIL_INDEX8)
      GLFrameBuffer(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                    const GLenum depthBufferFormat, const GLenum stencilBufferFormat, const Flags::type bufferFlags);

      ~GLFrameBuffer();

      //! @brief If a buffer is allocated this will releases it.
      void Reset() noexcept;

      //! @brief Release the existing buffer and replace it with the new one
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      // (GL_DEPTH_COMPONENT16, or GL_STENCIL_INDEX8)
      void Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams);

      //! @brief Release the existing buffer and replace it with the new one
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      //! @param depthBufferFormat the format of the framebuffers depth renderbuffer (for example GL_DEPTH_COMPONENT16)
      // (GL_DEPTH_COMPONENT16, or GL_STENCIL_INDEX8)
      void Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                 const GLenum depthBufferFormat);

      //! @brief Release the existing buffer and replace it with the new one
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      //! @param depthBufferFormat the format of the framebuffers depth renderbuffer (for example GL_DEPTH_COMPONENT16)
      //! @param stencilBufferFormat the format of the framebuffers stencil renderbuffer (for example GL_STENCIL_INDEX8)
      void Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                 const GLenum depthBufferFormat, const GLenum stencilBufferFormat);

      //! @brief Release the existing buffer and replace it with the new one
      //! @param size the size of the color render buffer
      //! @param colorBufferFormat the format of the framebuffers color renderbuffer (for example GL_RGBA4, GL_RGB565 or GL_RGB5_A1)
      //! @param depthBufferFormat the format of the framebuffers depth renderbuffer (for example GL_DEPTH_COMPONENT16)
      //! @param stencilBufferFormat the format of the framebuffers stencil renderbuffer (for example GL_STENCIL_INDEX8)
      void Reset(const Point2& size, const GLTextureParameters& textureParameters, const GLTextureImageParameters& texImageParams,
                 const GLenum depthBufferFormat, const GLenum stencilBufferFormat, const Flags::type bufferFlags);

      //! @brief Check if this buffer contains a valid gl handle.
      bool IsValid() const
      {
        return m_handle != GLValues::INVALID_HANDLE;
      }

      //! @brief Get the gl handle associated with the buffer.
      //! @return the handle or GLValues::INVALID_HANDLE if the buffer is unallocated.
      GLuint Get() const
      {
        return m_handle;
      }

      //! @brief Get the gl handle associated with the buffer.
      //! @return the handle or GLValues::INVALID_HANDLE if the buffer is unallocated.
      FSL_ATTR_DEPRECATED GLuint GetHandle() const
      {
        return Get();
      }

      //! @brief Get the size of the framebuffer
      Point2 GetSize() const
      {
        return m_size;
      }

      //! @brief Get the texture
      GLTextureInfo GetTextureInfo() const
      {
        return m_texture.GetTextureInfo();
      }

      //! @brief Extract information about this texture as a GLTextureInfo struct
      operator GLTextureInfo() const
      {
        return m_texture.GetTextureInfo();
      }
    };
  }
}

#endif
