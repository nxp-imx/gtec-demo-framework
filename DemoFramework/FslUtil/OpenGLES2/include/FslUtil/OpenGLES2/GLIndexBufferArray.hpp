#ifndef FSLUTIL_OPENGLES2_GLINDEXBUFFERARRAY_HPP
#define FSLUTIL_OPENGLES2_GLINDEXBUFFERARRAY_HPP
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
#include <FslBase/BasicTypes.hpp>
#include <FslUtil/OpenGLES2/Common.hpp>
#include <FslUtil/OpenGLES2/GLBufferArray.hpp>
#include <FslUtil/OpenGLES2/GLBufferArrayEntry.hpp>
#include <GLES2/gl2.h>
#include <vector>

namespace Fsl::GLES2
{
  struct GLIndexBufferArrayEntry : public GLBufferArrayEntry
  {
    GLenum Type{0};
    GLIndexBufferArrayEntry() = default;
  };

  class GLIndexBufferArray : public GLBufferArray
  {
    GLenum m_type{0};

  public:
    //! @brief Move assignment operator
    GLIndexBufferArray& operator=(GLIndexBufferArray&& other) noexcept
    {
      if (this != &other)
      {
        // Claim ownership here
        m_type = other.m_type;
        // Remove the data from other
        other.m_type = 0;
        // Move the base part
        GLBufferArray::operator=(std::move(other));
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    GLIndexBufferArray(GLIndexBufferArray&& other) noexcept
      : GLBufferArray(std::move(other))    // NOLINT(bugprone-use-after-move)
      , m_type(other.m_type)
    {
      // Remove the data from other
      // NOLINTNEXTLINE(bugprone-use-after-move)
      other.m_type = 0;
    }

    //! @brief Create a uninitialized index buffer
    GLIndexBufferArray();

    //! @brief Create a initialized index buffer
    GLIndexBufferArray(const std::size_t capacity, const GLenum type);

    using GLBufferArray::Reset;

    //! @brief Resize the array
    void Resize(const std::size_t capacity, const GLenum type);

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::size_t arrayIndex, const void* const pIndices, const std::size_t elementCount, const uint32_t elementStride,
               const GLenum usage, const GLenum type);

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::size_t arrayIndex, const uint8_t* const pIndices, const std::size_t elementCount, const GLenum usage);

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::size_t arrayIndex, const uint16_t* const pIndices, const std::size_t elementCount, const GLenum usage);

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::size_t arrayIndex, const std::vector<uint8_t>& indices, const GLenum usage);

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::size_t arrayIndex, const std::vector<uint16_t>& indices, const GLenum usage);

    //! @brief Get the type of all the arrays
    GLenum GetType() const
    {
      return m_type;
    }
  };
}

#endif
