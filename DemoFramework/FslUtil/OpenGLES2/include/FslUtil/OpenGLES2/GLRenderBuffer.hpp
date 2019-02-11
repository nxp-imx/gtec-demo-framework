#ifndef FSLUTIL_OPENGLES2_GLRENDERBUFFER_HPP
#define FSLUTIL_OPENGLES2_GLRENDERBUFFER_HPP
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
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <FslBase/Attributes.hpp>
#include <FslBase/Math/Point2.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  namespace GLES2
  {
    class GLRenderBuffer
    {
      GLuint m_handle;
      GLenum m_format{0};
      Point2 m_size;

    public:
      GLRenderBuffer(const GLRenderBuffer&) = delete;
      GLRenderBuffer& operator=(const GLRenderBuffer&) = delete;

      //! @brief Move assignment operator
      GLRenderBuffer& operator=(GLRenderBuffer&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
          {
            Reset();
          }

          // Claim ownership here
          m_handle = other.m_handle;
          m_format = other.m_format;
          m_size = other.m_size;

          // Remove the data from other
          other.m_handle = GLValues::INVALID_HANDLE;
          other.m_format = 0;
          other.m_size = Point2();
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      GLRenderBuffer(GLRenderBuffer&& other) noexcept
        : m_handle(other.m_handle)
        , m_format(other.m_format)
        , m_size(other.m_size)
      {
        // Remove the data from other
        other.m_handle = GLValues::INVALID_HANDLE;
        other.m_format = 0;
        other.m_size = Point2();
      }

      //! @brief Create a uninitialized buffer
      GLRenderBuffer();
      GLRenderBuffer(const Point2& size, const GLenum format);

      ~GLRenderBuffer();

      //! @brief If a buffer is allocated this will releases it.
      void Reset() noexcept;

      //! @brief Release the existing buffer and replace it with the new one
      //! @param size the size of the buffer
      //! @param format the format of the buffer (for example GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, or GL_STENCIL_INDEX8)
      void Reset(const Point2& size, const GLenum format);

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

      //! @brief Get the gl handle associated with the buffer *DEPRECATED*.
      //! @return the handle or GLValues::INVALID_HANDLE if the buffer is unallocated.
      FSL_ATTR_DEPRECATED GLuint GetHandle() const
      {
        return Get();
      }

      //! @brief Get the format of the buffer
      GLenum GetFormat() const
      {
        return m_format;
      }

      //! @brief Get size of the buffer
      Point2 GetSize() const
      {
        return m_size;
      }
    };
  }
}

#endif
