#ifndef FSLBASE_SPAN_READONLYFLEXSPAN_HPP
#define FSLBASE_SPAN_READONLYFLEXSPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

namespace Fsl
{
  // This is similar to ReadOnlySpan but it represents a 'void*' memory segment
  class ReadOnlyFlexSpan
  {
  public:
    static constexpr std::size_t extent = std::size_t(-1);

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_pointer = const void*;

    // Disabled and using a base class we can define (workaround until C++17)
    // static constexpr std::size_t extent = std::size_t(-1);

  protected:
    const_pointer m_pData{nullptr};
    size_type m_length{0u};
    size_type m_stride{0u};

  public:
    constexpr ReadOnlyFlexSpan() noexcept = default;
    constexpr ReadOnlyFlexSpan(const ReadOnlyFlexSpan& other) noexcept = default;

    //! @brief overload that allows you to create a ReadOnlyFlexibleSpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr ReadOnlyFlexSpan(const_pointer pData, size_type count, size_type stride, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pData(pData)
      , m_length(count)
      , m_stride(stride)
    {
      assert(pData != nullptr || count == 0u);
      assert(stride > 0u || count == 0u);
    }

    explicit constexpr ReadOnlyFlexSpan(const_pointer pData, size_type count, size_type stride) noexcept
      : m_pData(pData)
      , m_length(pData != nullptr && stride > 0 ? count : 0u)
      , m_stride(stride)
    {
      FSLLOG3_DEBUG_INFO_IF((pData == nullptr && count != 0) || stride <= 0u, "forcing count to zero");
    }


    constexpr const_pointer data() const noexcept
    {
      return m_pData;
    }

    constexpr size_type size() const noexcept
    {
      return m_length;
    }

    //! @brief return the element size in bytes
    constexpr size_type stride() const noexcept
    {
      return m_stride;
    }

    constexpr bool empty() const noexcept
    {
      return m_length == 0;
    }

    constexpr size_type length() const noexcept
    {
      return m_length;
    }

    constexpr size_type byte_size() const noexcept
    {
      return m_length * m_stride;
    }

    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    constexpr ReadOnlyFlexSpan subspan(size_type pos = 0, size_type count = extent) const
    {
      if (pos > m_length)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (m_length - pos);
      return ReadOnlyFlexSpan(static_cast<const uint8_t*>(m_pData) + (m_stride * pos), (count <= maxLength ? count : maxLength), m_stride,
                              OptimizationCheckFlag::NoCheck);
    }
  };
}

#endif
