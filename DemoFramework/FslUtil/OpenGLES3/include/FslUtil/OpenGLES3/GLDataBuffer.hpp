#ifndef FSLUTIL_OPENGLES3_GLDATABUFFER_HPP
#define FSLUTIL_OPENGLES3_GLDATABUFFER_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslUtil/OpenGLES3/Common.hpp>
#include <FslUtil/OpenGLES3/GLBuffer.hpp>
#include <GLES3/gl3.h>

namespace Fsl::GLES3
{
  class GLDataBuffer : public GLBuffer
  {
  public:
    //! @brief Move assignment operator
    GLDataBuffer& operator=(GLDataBuffer&& other) noexcept
    {
      if (this != &other)
      {
        // Move the base class part
        GLBuffer::operator=(std::move(other));
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    GLDataBuffer(GLDataBuffer&& other) noexcept
      : GLBuffer(std::move(other))    // NOLINT(bugprone-use-after-move)
    {
    }


    //! @brief Create a uninitialized index buffer
    GLDataBuffer() = default;

    //! @brief Create a uninitialized index buffer with the given capacity
    GLDataBuffer(const GLenum target, const std::size_t elementCount, const uint32_t elementStride, const GLenum usage)
      : GLBuffer(target, nullptr, elementCount, elementStride, usage)
    {
    }

    GLDataBuffer(const GLenum target, const ReadOnlyFlexSpan span, const GLenum usage)
      : GLBuffer(target, span, usage)
    {
    }

    //! @brief Create a initialized index buffer
    template <typename T>
    GLDataBuffer(const GLenum target, const ReadOnlySpan<T> span, const GLenum usage)
      : GLBuffer(target, span, usage)
    {
    }

    using GLBuffer::Reset;

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const GLenum target, const std::size_t elementCount, const uint32_t elementStride, const GLenum usage)
    {
      DoReset(target, nullptr, elementCount, elementStride, usage);
    }

    void Reset(const GLenum target, const ReadOnlyFlexSpan span, const GLenum usage)
    {
      DoReset(target, span, usage);
    }

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    template <typename T>
    void Reset(const GLenum target, const ReadOnlySpan<T> span, const GLenum usage)
    {
      DoReset(target, span, usage);
    }
  };
}

#endif
