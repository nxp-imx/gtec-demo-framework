#ifndef FSLBASE_SPAN_READONLYTYPEDFLEXSPAN_HPP
#define FSLBASE_SPAN_READONLYTYPEDFLEXSPAN_HPP
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
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

namespace Fsl
{
  class ReadOnlyTypedFlexSpanBase
  {
  public:
    constexpr ReadOnlyTypedFlexSpanBase() noexcept = default;
    static constexpr std::size_t extent = std::size_t(-1);
  };


  // This is similar to ReadOnlyTypedFlexSpanBase but this one knows the type its supposed to represent to the user..
  // However the stride between the elements is only required to be ">= sizeof(T)".
  // So this allows us to have a span of elements of a known type but with a flexible stride, so we can actually have a span
  // to elements contained in a more complex parent type.
  // This gives us a lot of flexibility but also requires us to use it with care!
  template <typename T>
  class ReadOnlyTypedFlexSpan : public ReadOnlyTypedFlexSpanBase
  {
  public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Disabled and using a base class we can define (workaround until C++17)
    // static constexpr std::size_t extent = std::size_t(-1);

  protected:
    const_pointer m_pData{nullptr};
    size_type m_length{0u};
    size_type m_stride{0u};

  public:
    constexpr ReadOnlyTypedFlexSpan() noexcept = default;
    constexpr ReadOnlyTypedFlexSpan(const ReadOnlyTypedFlexSpan& other) noexcept = default;

    //! @brief overload that allows you to create a ReadOnlyFlexibleSpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr ReadOnlyTypedFlexSpan(const_pointer pData, size_type count, size_type stride, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pData(pData)
      , m_length(count)
      , m_stride(stride)
    {
      assert(pData != nullptr || count == 0u);
      assert(stride > 0u || count == 0u);
    }

    explicit constexpr ReadOnlyTypedFlexSpan(const_pointer pData, size_type count, size_type stride) noexcept
      : m_pData(pData)
      , m_length(pData != nullptr && stride > 0 ? count : 0u)
      , m_stride(stride)
    {
      FSLLOG3_DEBUG_INFO_IF((pData == nullptr && count != 0) || stride <= 0u, "forcing count to zero");
    }


    constexpr const_pointer flex_data() const noexcept
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

    const_reference operator[](size_type pos) const
    {
      assert(pos < size());
      assert(m_pData != nullptr);
      return GetReference(pos);
    }

    const_reference at(size_type pos) const
    {
      if (pos >= size())
      {
        throw std::out_of_range("pos out of range");
      }
      assert(m_pData != nullptr);
      return GetReference(pos);
    }

    constexpr size_type byte_size() const noexcept
    {
      return m_length * m_stride;
    }


    const_reference back() const
    {
      assert(!empty());
      return GetReference(m_length - 1);
    }

    const_reference front() const
    {
      assert(!empty());
      return GetReference(0);
    }


    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    ReadOnlyTypedFlexSpan subspan(size_type pos = 0, size_type count = extent) const
    {
      if (pos > m_length)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (m_length - pos);
      return ReadOnlyTypedFlexSpan(GetPointer(pos), (count <= maxLength ? count : maxLength), m_stride, OptimizationCheckFlag::NoCheck);
    }

    // static const_pointer SYS_GetInitialPointer(void* pData, size_type byteOffset)
    // {
    //  return pData != nullptr ? reinterpret_cast<const_pointer>(static_cast<const uint8_t*>(pData) + byteOffset) : nullptr;
    // }

  private:
    inline const_pointer GetPointer(size_type pos) const noexcept
    {
      assert(m_pData != nullptr || pos == 0u);
      assert(/*pos >= 0u &&*/ pos <= m_length);
      return reinterpret_cast<const_pointer>(reinterpret_cast<const uint8_t*>(m_pData) + (m_stride * pos));
    }

    inline const_reference GetReference(size_type pos) const noexcept
    {
      assert(m_pData != nullptr);
      assert(/*pos >= 0u &&*/ pos <= m_length);
      return *reinterpret_cast<const_pointer>(reinterpret_cast<const uint8_t*>(m_pData) + (m_stride * pos));
    }
  };
}

#endif
