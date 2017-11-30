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

#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <FslUtil/OpenGLES2/GLBufferArray.hpp>

#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace GLES2
  {
    GLBufferArray& GLBufferArray::operator=(GLBufferArray&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_array = std::move(other.m_array);
        m_target = other.m_target;
        m_elementStride = other.m_elementStride;

        // Remove the data from other
        other.m_target = 0;
        other.m_elementStride = 0;
      }
      return *this;
    }


    GLBufferArray::GLBufferArray(GLBufferArray&& other)
      : m_array(other.m_array)
      , m_target(other.m_target)
      , m_elementStride(other.m_elementStride)
    {
      // Remove the data from other
      other.m_target = 0;
      other.m_elementStride = 0;
    }


    GLBufferArray::GLBufferArray()
      : m_array()
      , m_target(0)
      , m_elementStride(0)
    {
    }


    GLBufferArray::GLBufferArray(const std::size_t capacity, const GLenum target, const uint32_t elementStride)
      : m_array(capacity)
      , m_target(target)
      , m_elementStride(elementStride)
    {
    }


    GLBufferArray::~GLBufferArray()
    {
      Reset();
    }


    int32_t GLBufferArray::Length() const
    {
      return static_cast<int32_t>(m_array.size());
    }


    void GLBufferArray::Reset()
    {
      auto itr = m_array.begin();
      while (itr != m_array.end())
      {
        if (itr->Handle != GLValues::INVALID_HANDLE)
        {
          glDeleteBuffers(1, &itr->Handle);
          itr->Handle = GLValues::INVALID_HANDLE;
          itr->Capacity = 0;
          itr->Usage = 0;
        }
        ++itr;
      }
      m_target = 0;
      m_elementStride = 0;
    }


    GLBufferArrayEntry GLBufferArray::Get(const std::size_t arrayIndex) const
    {
      if (arrayIndex >= m_array.size())
        throw std::invalid_argument("arrayIndex out of range");

      return m_array[arrayIndex];
    }


    void GLBufferArray::Set(const std::size_t arrayIndex, const GLBufferArrayEntry& value)
    {
      if (arrayIndex >= m_array.size())
        throw std::invalid_argument("arrayIndex out of range");

      m_array[arrayIndex] = value;
    }


    void GLBufferArray::SetData(const std::size_t arrayIndex, const std::size_t dstIndex, const void*const pElements, const std::size_t elementCount)
    {
      if (arrayIndex >= m_array.size())
        throw std::invalid_argument("arrayIndex out of range");
      const auto& arrayEntry = m_array[arrayIndex];

      if (pElements == nullptr)
        throw std::invalid_argument("pElements can not be null");
      if ((dstIndex + (m_elementStride*elementCount)) >(m_elementStride * arrayEntry.Capacity))
        throw IndexOutOfRangeException();
      if (!arrayEntry.IsValid())
        throw UsageErrorException();

      glBindBuffer(m_target, arrayEntry.Handle);
      glBufferSubData(m_target, dstIndex * m_elementStride, elementCount * m_elementStride, pElements);
      glBindBuffer(m_target, 0);
    }


    void GLBufferArray::SetDataFast(const std::size_t arrayIndex, const std::size_t dstIndex, const void*const pElements, const std::size_t elementCount)
    {
      if (arrayIndex >= m_array.size())
        throw std::invalid_argument("arrayIndex out of range");
      const auto& arrayEntry = m_array[arrayIndex];

      if (pElements == nullptr)
        throw std::invalid_argument("pElements can not be null");
      if ((dstIndex + (m_elementStride*elementCount)) >(m_elementStride * arrayEntry.Capacity))
        throw IndexOutOfRangeException();
      if (!arrayEntry.IsValid())
        throw UsageErrorException();

      glBufferSubData(m_target, dstIndex * m_elementStride, elementCount * m_elementStride, pElements);
    }


    void GLBufferArray::Reset(const std::size_t arrayIndex, const GLenum target, const void*const pEntries, const std::size_t elementCount, const uint32_t elementStride, const GLenum usage)
    {
      if (arrayIndex >= m_array.size())
        throw std::invalid_argument("arrayIndex out of range");
      if (target != m_target)
        throw std::invalid_argument("the target did not match the arrays expected target");
      if (elementStride != m_elementStride)
        throw std::invalid_argument("the elementStride did not match the arrays expected stride");

      auto& rArrayEntry = m_array[arrayIndex];

      if (elementStride < 1)
        throw std::invalid_argument("invalid argument");

      // If we don't have a handle -> allocate one
      if (rArrayEntry.Handle == GLValues::INVALID_HANDLE)
      {
        GL_CHECK(glGenBuffers(1, &rArrayEntry.Handle));
      }

      const auto cbEntry = elementCount * elementStride;

      GL_CHECK(glBindBuffer(target, rArrayEntry.Handle));
      GL_CHECK(glBufferData(target, cbEntry, pEntries, usage));
      GL_CHECK(glBindBuffer(target, 0));

      rArrayEntry.Capacity = static_cast<uint32_t>(elementCount);
      rArrayEntry.Usage = usage;
    }


    void GLBufferArray::DoResize(const std::size_t capacity, const GLenum target, const uint32_t elementStride)
    {
      assert(target != 0);

      if (capacity != m_array.size())
      {
        if (m_array.size() < capacity)
        {
          // Free the entries we are removing
          for (std::size_t i = capacity; i < m_array.size(); ++i)
          {
            if (m_array[i].Handle != GLValues::INVALID_HANDLE)
            {
              glDeleteBuffers(1, &m_array[i].Handle);
              m_array[i].Handle = GLValues::INVALID_HANDLE;
              m_array[i].Capacity = 0;
              m_array[i].Usage = 0;
            }
          }
        }
        m_array.resize(capacity);
      }
      m_target = target;
      m_elementStride = elementStride;
    }

  }

}
