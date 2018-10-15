#ifndef FSLUTIL_OPENGLES2_GLBUFFERARRAY_HPP
#define FSLUTIL_OPENGLES2_GLBUFFERARRAY_HPP
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
#include <FslUtil/OpenGLES2/GLBufferArrayEntry.hpp>
#include <GLES2/gl2.h>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Noncopyable.hpp>
#include <vector>

namespace Fsl
{
  namespace GLES2
  {
    class GLBufferArray : private Noncopyable
    {
      std::vector<GLBufferArrayEntry> m_array;
      GLenum m_target;
      uint32_t m_elementStride;

    public:
      GLBufferArray(const GLBufferArray&) = delete;
      GLBufferArray& operator=(const GLBufferArray&) = delete;

      //! @brief Move assignment operator
      GLBufferArray& operator=(GLBufferArray&& other) noexcept;

      //! @brief Move constructor
      //! Transfer ownership from other to this
      GLBufferArray(GLBufferArray&& other) noexcept;

      //! @brief Create a empty buffer array
      GLBufferArray();

      //! @brief Create a buffer array with the given capacity
      //! @param elementStride the size of one element in bytes
      GLBufferArray(const std::size_t capacity, const GLenum target, const uint32_t elementStride);

      ~GLBufferArray();

      bool IsValid() const
      {
        return m_target != 0;
      }

      GLenum GetTarget() const
      {
        return m_target;
      }

      uint32_t GetElementStride() const
      {
        return m_elementStride;
      }

      //! @brief return the length of the array
      int32_t Length() const;

      //! @brief Release the entire array.
      void Reset() noexcept;

      //! @brief Get the entry at the arrayIndex
      GLBufferArrayEntry Get(const std::size_t arrayIndex) const;

      //! @brief Set the entry at the arrayIndex
      void Set(const std::size_t arrayIndex, const GLBufferArrayEntry& value);

      //! @brief Update the given area of the buffer at arrayIndex
      //!        This is the recommended way of updating the content of a buffer both for full and partial updates!
      //! @param dstIndex the dst index where the data will be written.
      //! @param pElements the elements that should be written.
      //! @param elementCount the number of elements to write.
      //! @note   This method does not check for glErrors since its intended for use during rendering.
      //! @throws std::invalid_argument if pElements == nullptr
      //! @throws IndexOutOfRangeException if the dstIndex + elementCount exceeds the capacity of the buffer.
      //! @throws UsageErrorException if the object isn't valid
      void SetData(const std::size_t arrayIndex, const std::size_t dstIndex, const void* const pElements, const std::size_t elementCount);

      //! @brief Update the given area of the buffer at arrayIndex (Unlike SetData this call assumes that the buffer is already bound to the correct
      //! target)
      //! @param dstIndex the dst index where the data will be written.
      //! @param pElements the elements that should be written.
      //! @param elementCount the number of elements to write.
      //! @note   This method does not check for glErrors since its intended for use during rendering.
      //! @throws std::invalid_argument if pElements == nullptr
      //! @throws IndexOutOfRangeException if the dstIndex + elementCount exceeds the capacity of the buffer.
      //! @throws UsageErrorException if the object isn't valid
      void SetDataFast(const std::size_t arrayIndex, const std::size_t dstIndex, const void* const pElements, const std::size_t elementCount);

    protected:
      //! @brief Resize the array
      void DoResize(const std::size_t capacity, const GLenum target, const uint32_t elementStride);

      //! @brief Fill the buffer with the given entries
      //! @param pElements a pointer to the data that will be copied (or null to just initialize it to the given size but no copy is done)
      //! @param elementStride the size of one element in bytes
      void Reset(const std::size_t arrayIndex, const GLenum target, const void* const pEntries, const std::size_t elementCount,
                 const uint32_t elementStride, const GLenum usage);
    };
  }
}

#endif
