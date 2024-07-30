#ifndef FSLBASE_SPAN_TYPEDFLEXSPAN_HPP
#define FSLBASE_SPAN_TYPEDFLEXSPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/ReadOnlyTypedFlexSpan.hpp>
#include <FslBase/TypeAlignmentUtil.hpp>
#include <cassert>
#include <cstddef>
#include <type_traits>

namespace Fsl
{
  class TypedFlexSpanBase
  {
  public:
    constexpr TypedFlexSpanBase() noexcept = default;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr std::size_t extent = static_cast<std::size_t>(-1);
  };


  // This is similar to TypedFlexSpanBase but this one knows the type its supposed to represent to the user..
  // However the stride between the elements is only required to be ">= sizeof(T)".
  // So this allows us to have a span of elements of a known type but with a flexible stride, so we can actually have a span
  // to elements contained in a more complex parent type.
  // This gives us a lot of flexibility but also requires us to use it with care!
  template <typename T>
  class TypedFlexSpan : public TypedFlexSpanBase
  {
  public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    // Disabled and using a base class we can define (workaround until C++17)
    // static constexpr std::size_t extent = std::size_t(-1);

  protected:
    pointer m_pData{nullptr};
    size_type m_length{0u};
    size_type m_stride{0u};

  public:
    constexpr TypedFlexSpan() noexcept = default;
    constexpr TypedFlexSpan(const TypedFlexSpan& other) noexcept = default;

    //! @brief overload that allows you to create a ReadOnlyFlexibleSpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr TypedFlexSpan(pointer pData, size_type count, size_type stride, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pData(pData)
      , m_length(count)
      , m_stride(stride)
    {
      assert(pData != nullptr || count == 0u);
      assert(stride >= sizeof(T) || count == 0u);
      assert(pData == nullptr || TypeAlignmentUtil::UnsafeIsPointerAligned(pData, std::alignment_of<T>::value));
      assert(count == 0u || TypeAlignmentUtil::IsAligned(stride, std::alignment_of<T>::value));
    }

    explicit constexpr TypedFlexSpan(pointer pData, size_type count, size_type stride) noexcept
      : m_pData(pData)
      , m_length(pData != nullptr && stride > 0 ? count : 0u)
      , m_stride(stride)
    {
      assert(stride >= sizeof(T) || count == 0u);
      FSLLOG3_DEBUG_INFO_IF((pData == nullptr && count != 0) || stride <= 0u, "forcing count to zero");
      assert(pData == nullptr || TypeAlignmentUtil::UnsafeIsPointerAligned(pData, std::alignment_of<T>::value));
      assert(count == 0u || TypeAlignmentUtil ::IsAligned(stride, std::alignment_of<T>::value));
    }


    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_pointer flex_data() const noexcept
    {
      return m_pData;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr pointer flex_data() noexcept
    {
      return m_pData;
    }


    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type size() const noexcept
    {
      return m_length;
    }

    //! @brief return the element size in bytes
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type stride() const noexcept
    {
      return m_stride;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool empty() const noexcept
    {
      return m_length == 0;
    }

    const_reference operator[](size_type pos) const
    {
      assert(pos < size());
      assert(m_pData != nullptr);
      return GetConstReference(pos);
    }

    reference operator[](size_type pos)
    {
      assert(pos < size());
      assert(m_pData != nullptr);
      return GetReference(pos);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    const_reference at(size_type pos) const
    {
      if (pos >= size())
      {
        throw std::out_of_range("pos out of range");
      }
      assert(m_pData != nullptr);
      return GetConstReference(pos);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    reference at(size_type pos)
    {
      if (pos >= size())
      {
        throw std::out_of_range("pos out of range");
      }
      assert(m_pData != nullptr);
      return GetReference(pos);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type byte_size() const noexcept
    {
      return m_length * m_stride;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    const_reference back() const
    {
      assert(!empty());
      return GetConstReference(m_length - 1);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    reference back()
    {
      assert(!empty());
      return GetReference(m_length - 1);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    const_reference front() const
    {
      assert(!empty());
      return GetConstReference(0);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    reference front()
    {
      assert(!empty());
      return GetReference(0);
    }


    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    // NOLINTNEXTLINE(readability-identifier-naming)
    ReadOnlyTypedFlexSpan<T> subspan(size_type pos = 0, size_type count = extent) const
    {
      if (pos > m_length)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (m_length - pos);
      return ReadOnlyTypedFlexSpan<T>(GetConstPointer(pos), (count <= maxLength ? count : maxLength), m_stride, OptimizationCheckFlag::NoCheck);
    }

    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    // NOLINTNEXTLINE(readability-identifier-naming)
    TypedFlexSpan subspan(size_type pos = 0, size_type count = extent)
    {
      if (pos > m_length)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (m_length - pos);
      return TypedFlexSpan(GetPointer(pos), (count <= maxLength ? count : maxLength), m_stride, OptimizationCheckFlag::NoCheck);
    }

    //! implicit conversion to ReadOnlySpan
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator ReadOnlyTypedFlexSpan<T>() const
    {
      return ReadOnlyTypedFlexSpan<T>(m_pData, m_length, m_stride);
    }

    constexpr ReadOnlyTypedFlexSpan<T> AsReadOnlySpan() const
    {
      return ReadOnlyTypedFlexSpan<T>(m_pData, m_length, m_stride);
    }

    // static pointer SYS_GetInitialPointer(void* pData, size_type byteOffset)
    // {
    //  return pData != nullptr ? reinterpret_cast<pointer>(static_cast<uint8_t*>(pData) + byteOffset) : nullptr;
    // }

    void Fill(const T& value)
    {
      if (!empty())
      {
        auto* pDst = reinterpret_cast<uint8_t*>(m_pData);
        auto* pDstEnd = reinterpret_cast<uint8_t*>(m_pData) + (m_stride * m_length);
        while (pDst < pDstEnd)
        {
          assert(TypeAlignmentUtil::UnsafeIsPointerAligned(pDst, std::alignment_of<T>::value));
          *reinterpret_cast<T*>(pDst) = value;
          pDst += m_stride;
        }
      }
    }


    template <typename TSrc, typename TConvSrcToDstFunc>
    void Copy(const ReadOnlySpan<TSrc> src, TConvSrcToDstFunc fnConvert)
    {
      if (!src.empty())
      {
        if (src.size() > size())
        {
          throw IndexOutOfRangeException("The span could not contain all the entries");
        }

        auto* pDst = reinterpret_cast<uint8_t*>(m_pData);
        auto* pDstEnd = reinterpret_cast<uint8_t*>(m_pData) + (m_stride * m_length);
        std::size_t i = 0;
        while (pDst < pDstEnd)
        {
          assert(TypeAlignmentUtil::UnsafeIsPointerAligned(pDst, std::alignment_of<T>::value));
          *reinterpret_cast<T*>(pDst) = fnConvert(src[i]);
          pDst += m_stride;
          ++i;
        }
      }
    }

  private:
    inline const_pointer GetConstPointer(size_type pos) const noexcept
    {
      assert(m_pData != nullptr || pos == 0u);
      assert(/*pos >= 0u &&*/ pos <= m_length);
      return reinterpret_cast<const_pointer>(reinterpret_cast<const uint8_t*>(m_pData) + (m_stride * pos));
    }

    inline const_reference GetConstReference(size_type pos) const noexcept
    {
      assert(m_pData != nullptr);
      assert(/*(pos >= 0u &&*/ pos <= m_length);
      return *reinterpret_cast<const_pointer>(reinterpret_cast<const uint8_t*>(m_pData) + (m_stride * pos));
    }

    inline pointer GetPointer(size_type pos) noexcept
    {
      assert(m_pData != nullptr || pos == 0u);
      assert(/*pos >= 0u &&*/ pos <= m_length);
      return reinterpret_cast<pointer>(reinterpret_cast<uint8_t*>(m_pData) + (m_stride * pos));
    }

    inline reference GetReference(size_type pos) noexcept
    {
      assert(m_pData != nullptr);
      assert(/*pos >= 0u &&*/ pos <= m_length);
      return *reinterpret_cast<pointer>(reinterpret_cast<uint8_t*>(m_pData) + (m_stride * pos));
    }
  };
}

#endif
