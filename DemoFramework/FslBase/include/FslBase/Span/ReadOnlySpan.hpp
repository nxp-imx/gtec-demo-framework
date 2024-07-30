#ifndef FSLBASE_SPAN_READONLYSPAN_HPP
#define FSLBASE_SPAN_READONLYSPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022, 2024 NXP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Span/SpanUtil_CreateStd.hpp>
#include <cassert>
#include <span>
#include <stdexcept>

namespace Fsl
{
  // This is similar to C++20 Span
  template <typename T>
  class ReadOnlySpan
  {
  public:
    using element_type = typename std::span<const T>::element_type;
    using value_type = typename std::span<const T>::value_type;
    using size_type = typename std::span<const T>::size_type;
    using difference_type = typename std::span<const T>::difference_type;
    using pointer = typename std::span<const T>::pointer;
    using const_pointer = typename std::span<const T>::const_pointer;
    using reference = typename std::span<const T>::reference;
    using const_reference = typename std::span<const T>::const_reference;

    using iterator = typename std::span<const T>::iterator;
    //// c++23
    // using const_iterator = typename std::span<const T>::const_iterator;
    using reverse_iterator = typename std::span<const T>::reverse_iterator;
    // using const_reverse_iterator = typename std::span<const T>::const_reverse_iterator;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr std::size_t extent = std::span<const T>::extent;

  protected:
    std::span<const value_type> m_span;

  public:
    constexpr ReadOnlySpan() noexcept = default;
    constexpr ReadOnlySpan(const ReadOnlySpan& other) noexcept = default;

    explicit constexpr ReadOnlySpan(const_pointer pData, size_type count) noexcept
      : m_span(SpanUtil::CreateStd(pData, count))
    {
    }

    //! Allow direct construction from a std::span
    constexpr ReadOnlySpan(std::span<const value_type> span) noexcept    // NOLINT(google-explicit-constructor)
      : m_span(span)
    {
    }

    //! Allow direct construction from a std::span
    constexpr ReadOnlySpan(std::span<value_type> span) noexcept    // NOLINT(google-explicit-constructor)
      : m_span(span)
    {
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_pointer data() const noexcept
    {
      return m_span.data();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type size() const noexcept
    {
      return m_span.size();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type size_bytes() const noexcept
    {
      return m_span.size_bytes();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool empty() const noexcept
    {
      return m_span.empty();
    }

    constexpr const_reference operator[](size_type pos) const noexcept
    {
      assert(pos < size());
      return m_span[pos];
    }

    // From C++26
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reference at(size_type pos) const
    {
      if (pos >= size())
      {
        throw std::out_of_range("pos out of range");
      }
      return m_span[pos];
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reference back() const noexcept
    {
      assert(!empty());
      return m_span.back();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reference front() const noexcept
    {
      assert(!empty());
      return m_span.front();
    }

    //! @brief Returns a view of the subspan [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan subspan(const size_type offset, const size_type count = std::dynamic_extent) const
    {
      // Verify the rules for subspan
      if (offset > m_span.size())
      {
        throw std::out_of_range("offset is out of bounds");
      }
      if (count > (m_span.size() - offset) && count != std::dynamic_extent)
      {
        throw std::out_of_range("offset+size is out of bounds");
      }
      return m_span.subspan(offset, count);
    }

    //! A unsafe way to create a subspan.
    //! @param offset = the offset to start at. It must be <= size().
    //! @param count = the number of elements to include in the subspan. It must be <= (size() - offset).
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan unchecked_subspan(const size_type offset, const size_type count) const noexcept
    {
      assert(offset <= m_span.size());
      assert(count <= (m_span.size() - offset));
      return m_span.subspan(offset, count);
    }

    //! A safe way to create a subspan. Any attempt to access outside the span will be clamped to the source size.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan clamped_subspan(const size_type offset, const size_type count = std::dynamic_extent) const noexcept
    {
      const auto clampedOffset = offset <= m_span.size() ? offset : m_span.size();
      const auto maxLength = m_span.size() - clampedOffset;
      FSLLOG3_DEBUG_INFO_IF(offset > m_span.size(), "offset out of bounds");
      // FSLLOG3_DEBUG_INFO_IF(count > maxLength && count != std::dynamic_extent, "size out of bounds");
      return m_span.subspan(clampedOffset, (count <= maxLength ? count : maxLength));
    }

    //! A safe way to create a subspan based on the first section of the span.
    //! Any attempt to access outside the span will be clamped to the source size.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan first(const size_type count) const
    {
      if (count > m_span.size())
      {
        throw std::out_of_range("count must be <= size()");
      }
      return m_span.first(count);
    }

    //! A unsafe way to create a subspan. count must be <= size()
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan unchecked_first(const size_type count) const noexcept
    {
      assert(count <= m_span.size());
      return m_span.first(count);
    }

    //! A safe way to create a subspan based on the first section of the span.
    //! Any attempt to access outside the span will be clamped to the source size.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan clamped_first(const size_type count) const noexcept
    {
      const auto clampedCount = count <= m_span.size() ? count : m_span.size();
      // FSLLOG3_DEBUG_INFO_IF(count > m_span.size(), "count out of bounds");
      return m_span.first(clampedCount);
    }

    //! A safe way to create a subspan based on the last section of the span.
    //! Any attempt to access outside the span will be clamped to the source size.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan last(const size_type count) const
    {
      if (count > m_span.size())
      {
        throw std::out_of_range("count must be <= size()");
      }
      return m_span.last(count);
    }

    //! A unsafe way to create a subspan based on the last section of the span. count must be <= size()
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan unchecked_last(const size_type count) const noexcept
    {
      assert(count <= m_span.size());
      return m_span.last(count);
    }

    //! A safe way to create a subspan based on the last section of the span.
    //! Any attempt to access outside the span will be clamped to the source size.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr ReadOnlySpan clamped_last(const size_type count) const noexcept
    {
      const auto maxLength = count <= m_span.size() ? count : m_span.size();
      // FSLLOG3_DEBUG_INFO_IF(count > m_span.size(), "count out of bounds");
      return m_span.last(maxLength);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto begin() const noexcept
    {
      return m_span.begin();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto end() const noexcept
    {
      return m_span.end();
    }

    //// From C++23
    //// NOLINTNEXTLINE(readability-identifier-naming)
    // constexpr auto cbegin() const noexcept
    //{
    //   return m_span.cbegin();
    // }

    //// From C++23
    //// NOLINTNEXTLINE(readability-identifier-naming)
    // constexpr auto cend() const noexcept
    //{
    //   return m_span.cend();
    // }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto rbegin() const noexcept
    {
      return m_span.rbegin();
    }


    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto rend() const noexcept
    {
      return m_span.rend();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto crbegin() const noexcept
    {
      return m_span.crbegin();
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto crend() const noexcept
    {
      return m_span.crend();
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator std::span<const value_type>() const noexcept
    {
      return m_span;
    }
  };
}

#endif
