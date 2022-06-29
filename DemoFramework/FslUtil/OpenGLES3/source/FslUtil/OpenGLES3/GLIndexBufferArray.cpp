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
#include <FslUtil/OpenGLES3/GLIndexBufferArray.hpp>
#include <algorithm>

namespace Fsl::GLES3
{
  namespace
  {
    int32_t ConvertTypeToStride(const GLenum type)
    {
      switch (type)
      {
      case GL_UNSIGNED_BYTE:
        return 1;
      case GL_UNSIGNED_SHORT:
        return 2;
      default:
        throw NotSupportedException("Unsupported index buffer type");
      }
    }
  }


  GLIndexBufferArray::GLIndexBufferArray() = default;


  GLIndexBufferArray::GLIndexBufferArray(const std::size_t capacity, const GLenum type)
    : GLBufferArray(capacity, GL_ELEMENT_ARRAY_BUFFER, ConvertTypeToStride(type))
    , m_type(type)
  {
  }


  void GLIndexBufferArray::Resize(const std::size_t capacity, const GLenum type)
  {
    DoResize(capacity, GL_ELEMENT_ARRAY_BUFFER, ConvertTypeToStride(type));
    m_type = type;
  }


  void GLIndexBufferArray::Reset(const std::size_t arrayIndex, const void* const pIndices, const std::size_t elementCount,
                                 const uint32_t elementStride, const GLenum usage, const GLenum type)
  {
    if (type != m_type)
    {
      throw NotSupportedException("This array is not configured to hold indices of the requested type");
    }

    GLBufferArray::Reset(arrayIndex, GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, elementStride, usage);
  }


  void GLIndexBufferArray::Reset(const std::size_t arrayIndex, const uint8_t* const pIndices, const std::size_t elementCount, const GLenum usage)
  {
    if (m_type != GL_UNSIGNED_BYTE)
    {
      throw NotSupportedException("This array is not configured to hold uint8_t indices");
    }

    GLBufferArray::Reset(arrayIndex, GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, sizeof(uint8_t), usage);
  }


  void GLIndexBufferArray::Reset(const std::size_t arrayIndex, const uint16_t* const pIndices, const std::size_t elementCount, const GLenum usage)
  {
    if (m_type != GL_UNSIGNED_SHORT)
    {
      throw NotSupportedException("This array is not configured to hold uint16_t indices");
    }

    GLBufferArray::Reset(arrayIndex, GL_ELEMENT_ARRAY_BUFFER, pIndices, elementCount, sizeof(uint16_t), usage);
  }


  void GLIndexBufferArray::Reset(const std::size_t arrayIndex, const std::vector<uint8_t>& indices, const GLenum usage)
  {
    if (m_type != GL_UNSIGNED_BYTE)
    {
      throw NotSupportedException("This array is not configured to hold uint8_t indices");
    }

    GLBufferArray::Reset(arrayIndex, GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint8_t), usage);
  }


  void GLIndexBufferArray::Reset(const std::size_t arrayIndex, const std::vector<uint16_t>& indices, const GLenum usage)
  {
    if (m_type != GL_UNSIGNED_SHORT)
    {
      throw NotSupportedException("This array is not configured to hold uint16_t indices");
    }

    GLBufferArray::Reset(arrayIndex, GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size(), sizeof(uint16_t), usage);
  }
}
