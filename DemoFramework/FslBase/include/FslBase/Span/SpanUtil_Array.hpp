#ifndef FSLBASE_SPAN_SPANUTIL_ARRAY_HPP
#define FSLBASE_SPAN_SPANUTIL_ARRAY_HPP
/****************************************************************************************************************************************************
 * Copyright 2020-2024 NXP
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

#include <FslBase/Span/Span.hpp>
#include <array>

namespace Fsl::SpanUtil
{
  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // AsSpan
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> AsReadOnlySpan(const std::array<T, TSize>& value) noexcept
  {
    return ReadOnlySpan<T>(value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value) noexcept
  {
    return ReadOnlySpan<T>(value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr Span<T> AsSpan(std::array<T, TSize>& rValue) noexcept
  {
    return Span<T>(rValue);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // UncheckedAsSpan
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> UncheckedAsReadOnlySpan(const std::array<T, TSize>& value, typename Span<T>::size_type offset,
                                                           typename Span<T>::size_type count)
  {
    assert(offset <= value.size());
    assert(count <= (value.size() - offset));
    return ReadOnlySpan<T>(std::span<const T>(value.data() + offset, count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> UncheckedAsSpan(const std::array<T, TSize>& value, typename Span<T>::size_type offset,
                                                   typename Span<T>::size_type count)
  {
    assert(offset <= value.size());
    assert(count <= (value.size() - offset));
    return ReadOnlySpan<T>(std::span<const T>(value.data() + offset, count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr Span<T> UncheckedAsSpan(std::array<T, TSize>& rValue, typename Span<T>::size_type offset,
                                           typename Span<T>::size_type count) noexcept
  {
    assert(offset <= rValue.size());
    assert(count <= (rValue.size() - offset));
    return Span<T>(std::span<T>(rValue.data() + offset, count));
  }


  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // AsSpan
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> AsReadOnlySpan(const std::array<T, TSize>& value, const std::size_t offset,
                                                  const std::size_t count = std::dynamic_extent)
  {
    // Verify the rules for subspan
    if (offset > value.size())
    {
      throw std::invalid_argument("offset is out of bounds");
    }
    if (count > (value.size() - offset) && count != std::dynamic_extent)
    {
      throw std::invalid_argument("offset+size is out of bounds");
    }
    return ReadOnlySpan<T>(std::span<const T>(value).subspan(offset, count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> AsSpan(const std::array<T, TSize>& value, const std::size_t offset, const std::size_t count = std::dynamic_extent)
  {
    // Verify the rules for subspan
    if (offset > value.size())
    {
      throw std::invalid_argument("offset is out of bounds");
    }
    if (count > (value.size() - offset) && count != std::dynamic_extent)
    {
      throw std::invalid_argument("offset+size is out of bounds");
    }
    return ReadOnlySpan<T>(std::span<const T>(value).subspan(offset, count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr Span<T> AsSpan(std::array<T, TSize>& value, const std::size_t offset, const std::size_t count = std::dynamic_extent)
  {
    // Verify the rules for subspan
    if (offset > value.size())
    {
      throw std::invalid_argument("offset is out of bounds");
    }
    if (count > (value.size() - offset) && count != std::dynamic_extent)
    {
      throw std::invalid_argument("offset+size is out of bounds");
    }
    return Span<T>(std::span<T>(value).subspan(offset, count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // ClampedAsSpan subspan
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! A safe way to create a subspan. Any attempt to access outside the span will be clamped to the source size.
  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> ClampedAsReadOnlySpan(const std::array<T, TSize>& value, typename Span<T>::size_type offset,
                                                         typename Span<T>::size_type count = std::dynamic_extent) noexcept
  {
    const auto clampedOffset = offset <= value.size() ? offset : value.size();
    const auto maxLength = value.size() - clampedOffset;
    FSLLOG3_DEBUG_INFO_IF(offset > value.size(), "offset out of bounds");
    FSLLOG3_DEBUG_INFO_IF(count > maxLength && count != std::dynamic_extent, "size out of bounds");
    return ReadOnlySpan<T>(std::span<const T>(value.data() + clampedOffset, (count <= maxLength ? count : maxLength)));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! A safe way to create a subspan. Any attempt to access outside the span will be clamped to the source size.
  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> ClampedAsSpan(const std::array<T, TSize>& value, typename Span<T>::size_type offset,
                                                 typename Span<T>::size_type count = std::dynamic_extent) noexcept
  {
    const auto clampedOffset = offset <= value.size() ? offset : value.size();
    const auto maxLength = value.size() - clampedOffset;
    FSLLOG3_DEBUG_INFO_IF(offset > value.size(), "offset out of bounds");
    FSLLOG3_DEBUG_INFO_IF(count > maxLength && count != std::dynamic_extent, "size out of bounds");
    return ReadOnlySpan<T>(std::span<const T>(value.data() + clampedOffset, (count <= maxLength ? count : maxLength)));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! A safe way to create a subspan. Any attempt to access outside the span will be clamped to the source size.
  template <typename T, std::size_t TSize>
  inline constexpr Span<T> ClampedAsSpan(std::array<T, TSize>& rValue, typename Span<T>::size_type offset,
                                         typename Span<T>::size_type count = std::dynamic_extent) noexcept
  {
    const auto clampedOffset = offset <= rValue.size() ? offset : rValue.size();
    const auto maxLength = rValue.size() - clampedOffset;
    FSLLOG3_DEBUG_INFO_IF(offset > rValue.size(), "offset out of bounds");
    FSLLOG3_DEBUG_INFO_IF(count > maxLength && count != std::dynamic_extent, "size out of bounds");
    return Span<T>(std::span<T>(rValue.data() + clampedOffset, (count <= maxLength ? count : maxLength)));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // UncheckedFirstSpan
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> UncheckedFirstReadOnlySpan(const std::array<T, TSize>& value, const std::size_t count) noexcept
  {
    assert(count <= value.size());
    return ReadOnlySpan<T>(std::span<const T>(value).first(count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> UncheckedFirstSpan(const std::array<T, TSize>& value, const std::size_t count) noexcept
  {
    assert(count <= value.size());
    return ReadOnlySpan<T>(std::span<const T>(value).first(count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr Span<T> UncheckedFirstSpan(std::array<T, TSize>& rValue, const std::size_t count) noexcept
  {
    assert(count <= rValue.size());
    return Span<T>(std::span<T>(rValue).first(count));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // FirstSpan
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> FirstReadOnlySpan(const std::array<T, TSize>& value, const std::size_t count)
  {
    if (count > value.size())
    {
      throw std::invalid_argument("size is out of bounds");
    }
    return UncheckedFirstReadOnlySpan(value, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> FirstSpan(const std::array<T, TSize>& value, const std::size_t count)
  {
    if (count > value.size())
    {
      throw std::invalid_argument("size is out of bounds");
    }
    return UncheckedFirstReadOnlySpan(value, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr Span<T> FirstSpan(std::array<T, TSize>& rValue, const std::size_t count)
  {
    if (count > rValue.size())
    {
      throw std::invalid_argument("size is out of bounds");
    }
    return UncheckedFirstSpan(rValue, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // ClampedFirstReadOnlySpan
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> ClampedFirstReadOnlySpan(const std::array<T, TSize>& value, const std::size_t count) noexcept
  {
    return UncheckedFirstReadOnlySpan(value, count <= value.size() ? count : value.size());
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr ReadOnlySpan<T> ClampedFirstSpan(const std::array<T, TSize>& value, const std::size_t count) noexcept
  {
    return UncheckedFirstSpan(value, count <= value.size() ? count : value.size());
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, std::size_t TSize>
  inline constexpr Span<T> ClampedFirstSpan(std::array<T, TSize>& rValue, const std::size_t count) noexcept
  {
    return UncheckedFirstSpan(rValue, count <= rValue.size() ? count : rValue.size());
  }
}

#endif
