/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
#include <FslUtil/OpenGLES2/GLBuffer.hpp>

#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace GLES2
  {
    GLBuffer::GLBuffer()
      : m_handle(GLValues::INVALID_HANDLE)
    {
    }


    GLBuffer::GLBuffer(const GLenum target, const void* const pEntries, const std::size_t elementCount, const std::size_t elementStride,
                       const GLenum usage)
      : m_handle(GLValues::INVALID_HANDLE)

    {
      try
      {
        Reset(target, pEntries, elementCount, elementStride, usage);
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }


    GLBuffer::~GLBuffer()
    {
      Reset();
    }


    void GLBuffer::Reset() noexcept
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        glDeleteBuffers(1, &m_handle);
        m_handle = GLValues::INVALID_HANDLE;
        m_target = 0;
        m_capacity = 0;
        m_elementStride = 0;
        m_usage = 0;
      }
    }


    void GLBuffer::SetData(const std::size_t dstIndex, const void* const pElements, const std::size_t elementCount)
    {
      if (pElements == nullptr)
      {
        throw std::invalid_argument("pElements can not be null");
      }
      if ((dstIndex + (m_elementStride * elementCount)) > (m_elementStride * m_capacity))
      {
        throw IndexOutOfRangeException();
      }
      if (!IsValid())
      {
        throw UsageErrorException();
      }

      glBindBuffer(m_target, m_handle);
      glBufferSubData(m_target, dstIndex * m_elementStride, elementCount * m_elementStride, pElements);
      glBindBuffer(m_target, 0);
    }


    void GLBuffer::SetDataFast(const std::size_t dstIndex, const void* const pElements, const std::size_t elementCount)
    {
      if (pElements == nullptr)
      {
        throw std::invalid_argument("pElements can not be null");
      }
      if ((dstIndex + (m_elementStride * elementCount)) > (m_elementStride * m_capacity))
      {
        throw IndexOutOfRangeException();
      }
      if (!IsValid())
      {
        throw UsageErrorException();
      }

      glBufferSubData(m_target, dstIndex * m_elementStride, elementCount * m_elementStride, pElements);
    }


    void GLBuffer::Reset(const GLenum target, const void* const pEntries, const std::size_t elementCount, const std::size_t elementStride,
                         const GLenum usage)
    {
      // invalid stride or one that causes uint32_t overflow
      if (elementStride < 1 || elementCount > (std::numeric_limits<uint32_t>::max() / elementStride))
      {
        throw std::invalid_argument("invalid argument");
      }

      // If we don't have a handle -> allocate one
      if (m_handle == GLValues::INVALID_HANDLE)
      {
        GL_CHECK(glGenBuffers(1, &m_handle));
      }

      const auto cbEntry = elementCount * elementStride;

      GL_CHECK(glBindBuffer(target, m_handle));
      GL_CHECK(glBufferData(target, cbEntry, pEntries, usage));
      GL_CHECK(glBindBuffer(target, 0));

      assert(elementCount <= std::numeric_limits<uint32_t>::max());
      assert(elementStride <= std::numeric_limits<uint32_t>::max());

      m_target = target;
      m_capacity = static_cast<uint32_t>(elementCount);
      m_elementStride = static_cast<uint32_t>(elementStride);
      m_usage = usage;
    }
  }
}
