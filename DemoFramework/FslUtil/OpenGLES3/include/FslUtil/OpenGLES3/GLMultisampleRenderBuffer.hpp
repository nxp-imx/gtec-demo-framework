#ifndef FSLUTIL_OPENGLES3_GLMULTISAMPLERENDERBUFFER_HPP
#define FSLUTIL_OPENGLES3_GLMULTISAMPLERENDERBUFFER_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslBase/Attributes.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <GLES3/gl3.h>

namespace Fsl::GLES3
{
  class GLMultisampleRenderBuffer
  {
    GLuint m_handle;
    GLenum m_format{0};
    PxSize2D m_size;

  public:
    GLMultisampleRenderBuffer(const GLMultisampleRenderBuffer&) = delete;
    GLMultisampleRenderBuffer& operator=(const GLMultisampleRenderBuffer&) = delete;

    //! @brief Move assignment operator
    GLMultisampleRenderBuffer& operator=(GLMultisampleRenderBuffer&& other) noexcept
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
        other.m_handle = GLValues::InvalidHandle;
        other.m_format = 0;
        other.m_size = {};
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    GLMultisampleRenderBuffer(GLMultisampleRenderBuffer&& other) noexcept
      : m_handle(other.m_handle)
      , m_format(other.m_format)
      , m_size(other.m_size)
    {
      // Remove the data from other
      other.m_handle = GLValues::InvalidHandle;
      other.m_format = 0;
      other.m_size = {};
    }

    //! @brief Create a uninitialized buffer
    GLMultisampleRenderBuffer();
    GLMultisampleRenderBuffer(const PxSize2D& size, const GLsizei samples, const GLenum internalFormat);

    ~GLMultisampleRenderBuffer();

    //! @brief If a buffer is allocated this will releases it.
    void Reset() noexcept;

    //! @brief Release the existing buffer and replace it with the new one
    //! @param size the size of the buffer
    void Reset(const PxSize2D& size, const GLsizei samples, const GLenum internalFormat);

    //! @brief Check if this buffer contains a valid gl handle.
    bool IsValid() const noexcept
    {
      return m_handle != GLValues::InvalidHandle;
    }

    //! @brief Get the gl handle associated with the buffer.
    //! @return the handle or GLValues::InvalidHandle if the buffer is unallocated.
    GLuint Get() const
    {
      return m_handle;
    }

    //! @brief Get the format of the buffer
    GLenum GetFormat() const noexcept
    {
      return m_format;
    }

    //! @brief Get size of the buffer
    PxSize2D GetSize() const noexcept
    {
      return m_size;
    }
  };
}

#endif
