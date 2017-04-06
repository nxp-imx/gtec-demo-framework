#ifndef FSLGRAPHICSGLES3_GLBUFFER_HPP
#define FSLGRAPHICSGLES3_GLBUFFER_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslGraphicsGLES3/Common.hpp>
#include <FslGraphicsGLES3/GLValues.hpp>
#include <FslBase/Attributes.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Noncopyable.hpp>
#include <cstdlib>
#include <GLES3/gl3.h>

namespace Fsl
{
  namespace GLES3
  {
    class GLBuffer : private Noncopyable
    {
      GLuint m_handle;
      GLenum m_target;
      uint32_t m_capacity;
      uint32_t m_elementStride;
      GLenum m_usage;
    public:
      //! @brief Create a uninitialized buffer
      GLBuffer();

      //! @brief Create a initialized buffer
      //! @param elementStride the size of one element in bytes
      GLBuffer(const GLenum target, const void*const pElements, const std::size_t elementCount, const std::size_t elementStride, const GLenum usage);

      ~GLBuffer();

      //! @brief If a buffer is allocated this will releases it.
      void Reset();

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
        return m_handle;
      }

      //! @brief Get the buffer target
      GLenum GetTarget() const
      {
        return m_target;
      }

      //! @brief Get the capacity
      uint32_t GetCapacity() const
      {
        return m_capacity;
      }

      //! @brief Get the element stride (size of one element in bytes)
      uint32_t GetElementStride() const
      {
        return m_elementStride;
      }

      //! @brief Get the buffer usage
      GLenum GetUsage() const
      {
        return m_usage;
      }

      //! @brief Update the given area of the buffer
      //!        This is the recommended way of updating the content of a buffer both for full and partial updates!
      //! @param dstIndex the dst index where the data will be written.
      //! @param pElements the elements that should be written.
      //! @param elementCount the number of elements to write.
      //! @note   This method does not check for glErrors since its intended for use during rendering.
      //! @throws std::invalid_argument if pElements == nullptr
      //! @throws IndexOutOfRangeException if the dstIndex + elementCount exceeds the capacity of the buffer.
      //! @throws UsageErrorException if the object isn't valid
      void SetData(const std::size_t dstIndex, const void*const pElements, const std::size_t elementCount);

      //! @brief Update the given area of the buffer (Unlike SetData this call assumes that the buffer is already bound to the correct target)
      //! @param dstIndex the dst index where the data will be written.
      //! @param pElements the elements that should be written.
      //! @param elementCount the number of elements to write.
      //! @note   This method does not check for glErrors since its intended for use during rendering.
      //! @throws std::invalid_argument if pElements == nullptr
      //! @throws IndexOutOfRangeException if the dstIndex + elementCount exceeds the capacity of the buffer.
      //! @throws UsageErrorException if the object isn't valid
      void SetDataFast(const std::size_t dstIndex, const void*const pElements, const std::size_t elementCount);
    protected:
      //! @brief Fill the buffer with the given entries
      //! @param pElements a pointer to the data that will be copied (or null to just initialize it to the given size but no copy is done)
      //! @param elementStride the size of one element in bytes
      void Reset(const GLenum target, const void*const pElements, const std::size_t elementCount, const std::size_t elementStride, const GLenum usage);
    };
  }
}

#endif
