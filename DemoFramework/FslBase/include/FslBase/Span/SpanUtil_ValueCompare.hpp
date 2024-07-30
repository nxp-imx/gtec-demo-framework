#ifndef FSLBASE_SPAN_SPANUTIL_VALUECOMPARE_HPP
#define FSLBASE_SPAN_SPANUTIL_VALUECOMPARE_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <cassert>
#include <span>

namespace Fsl::SpanUtil
{
  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // UncheckedValueEquals
  // -------------------------------------------------------------------------------------------------------------------------------------------------


  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr static bool UncheckedValueEquals(std::span<const T> lhsSpan, std::span<const T> rhsSpan,
                                                    const typename std::span<T>::size_type count) noexcept
  {
    using TSize = typename std::span<T>::size_type;

    assert(count <= lhsSpan.size());
    assert(count <= rhsSpan.size());
    const T* pLhsData = lhsSpan.data();
    const T* pRhsData = rhsSpan.data();
    for (TSize i = 0; i < count; ++i)
    {
      if (pLhsData[i] != pRhsData[i])
      {
        return false;
      }
    }
    return true;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr static bool UncheckedValueEquals(std::span<const T> lhsSpan, std::span<T> rhsSpan,
                                                    const typename std::span<T>::size_type count) noexcept
  {
    return UncheckedValueEquals(lhsSpan, std::span<const T>(rhsSpan), count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr static bool UncheckedValueEquals(std::span<T> lhsSpan, std::span<const T> rhsSpan,
                                                    const typename std::span<T>::size_type count) noexcept
  {
    return UncheckedValueEquals(std::span<const T>(lhsSpan), rhsSpan, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr static bool UncheckedValueEquals(std::span<T> lhsSpan, std::span<T> rhsSpan, const typename std::span<T>::size_type count) noexcept
  {
    return UncheckedValueEquals(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan), count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // UncheckedValueCompare
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr static int UncheckedValueCompare(std::span<const T> lhsSpan, std::span<const T> rhsSpan,
                                                    const typename std::span<const T>::size_type count) noexcept
  {
    using TSize = typename std::span<const T>::size_type;

    assert(count <= lhsSpan.size());
    assert(count <= rhsSpan.size());
    const T* pLhsData = lhsSpan.data();
    const T* pRhsData = rhsSpan.data();
    for (TSize i = 0; i < count; ++i)
    {
      auto res = pLhsData[i] - pRhsData[i];
      if (res != 0)
      {
        return res;
      }
    }
    return 0;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr static int UncheckedValueCompare(std::span<const T> lhsSpan, std::span<T> rhsSpan,
                                                    const typename std::span<T>::size_type count) noexcept
  {
    return UncheckedValueCompare(lhsSpan, std::span<const T>(rhsSpan), count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr static int UncheckedValueCompare(std::span<T> lhsSpan, std::span<const T> rhsSpan,
                                                    const typename std::span<T>::size_type count) noexcept
  {
    return UncheckedValueCompare(std::span<const T>(lhsSpan), rhsSpan, count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr static int UncheckedValueCompare(std::span<T> lhsSpan, std::span<T> rhsSpan, const typename std::span<T>::size_type count) noexcept
  {
    return UncheckedValueCompare(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan), count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr static int UncheckedValueCompare(ReadOnlySpan<T> lhsSpan, ReadOnlySpan<T> rhsSpan,
                                                    const typename std::span<T>::size_type count) noexcept
  {
    return UncheckedValueCompare(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan), count);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // ValueCompare
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(std::span<const T> lhsSpan, std::span<const T> rhsSpan) noexcept
  {
    const auto countCompare = lhsSpan.size() <= rhsSpan.size() ? lhsSpan.size() : rhsSpan.size();
    const auto result = (countCompare == 0 ? 0 : UncheckedValueCompare(lhsSpan, rhsSpan, countCompare));
    return (result == 0 ? (static_cast<int>(lhsSpan.size() > rhsSpan.size()) - static_cast<int>(lhsSpan.size() < rhsSpan.size()))
                        : (result < 0 ? -1 : 1));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(std::span<const T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return ValueCompare(lhsSpan, std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(std::span<T> lhsSpan, std::span<const T> rhsSpan) noexcept
  {
    return ValueCompare(std::span<const T>(lhsSpan), rhsSpan);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(std::span<T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return ValueCompare(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(ReadOnlySpan<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return ValueCompare(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(ReadOnlySpan<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return ValueCompare(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(Span<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return ValueCompare(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr int ValueCompare(Span<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return ValueCompare(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // ValueEquals
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(std::span<const T> lhsSpan, std::span<const T> rhsSpan) noexcept
  {
    return lhsSpan.size() == rhsSpan.size() ? UncheckedValueEquals(lhsSpan, rhsSpan, lhsSpan.size()) : false;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(std::span<const T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return ValueEquals(lhsSpan, std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(std::span<T> lhsSpan, std::span<const T> rhsSpan) noexcept
  {
    return ValueEquals(std::span<const T>(lhsSpan), rhsSpan);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(std::span<T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return ValueEquals(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(ReadOnlySpan<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return ValueEquals(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }


  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(ReadOnlySpan<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return ValueEquals(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }


  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(Span<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return ValueEquals(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief value compare the content of lhs Span to rhs Span
  template <typename T>
  inline constexpr bool ValueEquals(Span<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return ValueEquals(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // StartsWith (value)
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(std::span<const T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return (!srcSpan.empty() ? srcSpan.front() == value : false);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(std::span<T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return StartsWith(std::span<const T>(srcSpan), value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(ReadOnlySpan<T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return StartsWith(std::span<const T>(srcSpan), value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(Span<T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return StartsWith(std::span<const T>(srcSpan), value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // StartsWith (span)
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(std::span<const T> lhs, std::span<const T> rhs) noexcept
  {
    if (rhs.size() <= lhs.size())
    {
      return UncheckedValueEquals(lhs, rhs, rhs.size());
    }
    return false;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(std::span<const T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return StartsWith(lhsSpan, std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(std::span<T> lhsSpan, std::span<const T> rhsSpan) noexcept
  {
    return StartsWith(std::span<const T>(lhsSpan), rhsSpan);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(std::span<T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return StartsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(ReadOnlySpan<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return StartsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(ReadOnlySpan<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return StartsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(Span<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return StartsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool StartsWith(Span<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return StartsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // EndsWith (value)
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(std::span<const T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return (!srcSpan.empty() ? srcSpan.back() == value : false);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(std::span<T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return EndsWith(std::span<const T>(srcSpan), value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(ReadOnlySpan<T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return EndsWith(std::span<const T>(srcSpan), value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(Span<T> srcSpan, typename std::span<const T>::const_reference value) noexcept
  {
    return EndsWith(std::span<const T>(srcSpan), value);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // EndsWith (span)
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(std::span<const T> lhs, std::span<const T> rhs) noexcept
  {
    if (rhs.size() <= lhs.size())
    {
      return UncheckedValueEquals(lhs.subspan(lhs.size() - rhs.size(), rhs.size()), rhs, rhs.size());
    }
    return false;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(std::span<const T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return EndsWith(lhsSpan, std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(std::span<T> lhsSpan, std::span<const T> rhsSpan) noexcept
  {
    return EndsWith(std::span<const T>(lhsSpan), rhsSpan);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(std::span<T> lhsSpan, std::span<T> rhsSpan) noexcept
  {
    return EndsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(ReadOnlySpan<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return EndsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(ReadOnlySpan<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return EndsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(Span<T> lhsSpan, ReadOnlySpan<T> rhsSpan) noexcept
  {
    return EndsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr bool EndsWith(Span<T> lhsSpan, Span<T> rhsSpan) noexcept
  {
    return EndsWith(std::span<const T>(lhsSpan), std::span<const T>(rhsSpan));
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Find
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type Find(std::span<const T> srcSpan, typename std::span<const T>::const_reference value,
                                                         typename std::span<const T>::size_type pos = 0) noexcept
  {
    while (pos < srcSpan.size() && srcSpan[pos] != value)
    {
      ++pos;
    }
    return (pos < srcSpan.size() ? pos : std::span<T>::extent);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type Find(std::span<T> srcSpan, typename std::span<const T>::const_reference value,
                                                         typename std::span<T>::size_type pos = 0) noexcept
  {
    return Find(std::span<const T>(srcSpan), value, pos);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type Find(ReadOnlySpan<T> srcSpan, typename std::span<const T>::const_reference value,
                                                         typename std::span<T>::size_type pos = 0) noexcept
  {
    return Find(std::span<const T>(srcSpan), value, pos);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type Find(Span<T> srcSpan, typename std::span<const T>::const_reference value,
                                                         typename std::span<T>::size_type pos = 0) noexcept
  {
    return Find(std::span<const T>(srcSpan), value, pos);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // RFind
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type RFind(std::span<const T> srcSpan, typename std::span<const T>::const_reference value,
                                                          typename std::span<T>::size_type pos = std::span<const T>::extent) noexcept
  {
    if (!srcSpan.empty())
    {
      const auto* const pSrcData = srcSpan.data();
      const auto* pCurrent = pos <= srcSpan.size() ? (pSrcData + pos) : ((pSrcData + srcSpan.size()) - 1u);
      while (pCurrent >= pSrcData && *pCurrent != value)
      {
        --pCurrent;
      }
      return (pCurrent >= pSrcData ? static_cast<typename std::span<T>::size_type>(pCurrent - pSrcData) : std::span<T>::extent);
    }
    return std::span<T>::extent;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type RFind(std::span<T> srcSpan, typename std::span<const T>::const_reference value,
                                                          typename std::span<const T>::size_type pos = std::span<const T>::extent) noexcept
  {
    return RFind(std::span<const T>(srcSpan), value, pos);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type RFind(ReadOnlySpan<T> srcSpan, typename std::span<const T>::const_reference value,
                                                          typename std::span<const T>::size_type pos = std::span<const T>::extent) noexcept
  {
    return RFind(std::span<const T>(srcSpan), value, pos);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline constexpr typename std::span<T>::size_type RFind(Span<T> srcSpan, typename std::span<const T>::const_reference value,
                                                          typename std::span<const T>::size_type pos = std::span<const T>::extent) noexcept
  {
    return RFind(std::span<const T>(srcSpan), value, pos);
  }
}

#endif
