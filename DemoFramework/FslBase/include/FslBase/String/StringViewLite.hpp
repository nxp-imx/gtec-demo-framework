#ifndef FSLBASE_STRING_STRINGVIEWLITE_HPP
#define FSLBASE_STRING_STRINGVIEWLITE_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2022-2023 NXP
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
#include <FslBase/Iterator/PointerConstIterator.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace Fsl
{
  // This is basically a very lite implementation of C++17 string_view which we need until we adopt C++17.
  class StringViewLite
  {
  public:
    using value_type = char;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using const_iterator = PointerConstIterator<char>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr const size_type npos = static_cast<size_type>(-1);

  protected:
    const char* m_pStr{nullptr};
    size_type m_length{0u};

  public:
    constexpr StringViewLite() noexcept = default;
    constexpr StringViewLite(const StringViewLite& other) noexcept = default;
    //! @brief overload that allows you to create a StringViewLite from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr StringViewLite(const const_pointer pStr, size_type count, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pStr(pStr)
      , m_length(count)
    {
      assert(pStr != nullptr || count == 0u);
    }


    explicit constexpr StringViewLite(const const_pointer pStr, size_type count)
      : m_pStr(pStr)
      , m_length(count)
    {
      assert(pStr != nullptr || count == 0u);
    }

    constexpr StringViewLite(const const_pointer psz) noexcept    // NOLINT(google-explicit-constructor)
      : m_pStr(psz)
      , m_length(CalcLength(psz))
    {
    }

    explicit constexpr StringViewLite(const std::string_view strView) noexcept
      : m_pStr(strView.data())
      , m_length(strView.size())
    {
      assert(m_pStr != nullptr || m_length == 0u);
    }

    constexpr const_pointer data() const noexcept
    {
      return m_pStr;
    }

    constexpr size_type size() const noexcept
    {
      return m_length;
    }

    constexpr bool empty() const noexcept
    {
      return m_length == 0;
    }

    constexpr const_reference operator[](size_type pos) const
    {
      assert(pos < size());
      assert(m_pStr != nullptr);
      return m_pStr[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
      if (pos >= size())
      {
        throw std::out_of_range("pos out of range");
      }
      assert(m_pStr != nullptr);
      return m_pStr[pos];
    }

    constexpr size_type length() const noexcept
    {
      return m_length;
    }

    constexpr const_reference back() const noexcept
    {
      assert(!empty());
      return m_pStr[m_length - 1];
    }

    constexpr const_reference front() const noexcept
    {
      assert(!empty());
      return m_pStr[0];
    }

    constexpr void remove_prefix(size_type n) noexcept
    {
      assert(n <= m_length);
      m_pStr += n;
      m_length -= n;
    }

    constexpr void remove_suffix(size_type n) noexcept
    {
      assert(n <= m_length);
      m_length -= n;
    }


    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    constexpr StringViewLite substr(size_type pos = 0, size_type count = npos) const
    {
      if (pos > m_length)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (m_length - pos);
      return StringViewLite(m_pStr + pos, (count <= maxLength ? count : maxLength), OptimizationCheckFlag::NoCheck);
    }

    constexpr int compare(std::string_view value) const noexcept
    {
      return AsStringView().compare(value);
    }

    constexpr int compare(StringViewLite value) const noexcept
    {
      // min of m_length and value.m_length
      const auto countCompare = m_length <= value.m_length ? m_length : value.m_length;
      const auto result = (countCompare == 0 ? 0 : CompareN(m_pStr, value.m_pStr, countCompare));

      return (result == 0 ? (static_cast<int>(m_length > value.m_length) - static_cast<int>(m_length < value.m_length)) : (result < 0 ? -1 : 1));
    }

    int compare(size_type pos1, size_type count1, StringViewLite v) const
    {
      return substr(pos1, count1).compare(v);
    }

    int compare(size_type pos1, size_type count1, StringViewLite v, size_type pos2, size_type count2) const
    {
      return substr(pos1, count1).compare(v.substr(pos2, count2));
    }

    constexpr int compare(const value_type* const psz) const noexcept
    {
      return compare(StringViewLite(psz));
    }

    constexpr int compare(size_type pos1, size_type count1, const char* const psz) const
    {
      return substr(pos1, count1).compare(StringViewLite(psz));
    }

    constexpr int compare(size_type pos1, size_type count1, const char* const psz, size_type count2) const
    {
      return substr(pos1, count1).compare(StringViewLite(psz, count2));
    }

    constexpr bool starts_with(StringViewLite value) const noexcept
    {
      return m_length >= value.m_length ? (SafeCompareN(m_pStr, value.m_pStr, value.m_length) == 0) : false;
    }

    constexpr bool starts_with(char ch) const noexcept
    {
      return (m_length > 0u && m_pStr[0] == ch);
    }

    constexpr bool starts_with(const char* const psz) const noexcept
    {
      return starts_with(StringViewLite(psz));
    }

    constexpr bool ends_with(StringViewLite value) const noexcept
    {
      return m_length >= value.m_length ? (SafeCompareN(m_pStr + (m_length - value.m_length), value.m_pStr, value.m_length) == 0) : false;
    }

    constexpr bool ends_with(char ch) const noexcept
    {
      return (m_length > 0u && m_pStr[m_length - 1u] == ch);
    }

    constexpr bool ends_with(const char* const psz) const noexcept
    {
      return ends_with(StringViewLite(psz));
    }

    constexpr size_type find(const value_type ch, size_type pos = 0) const noexcept
    {
      while (pos < m_length && m_pStr[pos] != ch)
      {
        ++pos;
      }
      return (pos < m_length ? pos : npos);
    }

    constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept
    {
      if (m_length > 0u)
      {
        const char* pCurrent = pos <= m_length ? (m_pStr + pos) : ((m_pStr + m_length) - 1u);
        while (pCurrent >= m_pStr && *pCurrent != ch)
        {
          --pCurrent;
        }
        return (pCurrent >= m_pStr ? static_cast<size_type>(pCurrent - m_pStr) : npos);
      }
      return npos;
    }

    constexpr const_iterator begin() const noexcept
    {
#ifdef NDEBUG
      return const_iterator(m_pStr);
#else
      return {m_pStr, m_pStr, m_length};
#endif
    }

    constexpr const_iterator end() const noexcept
    {
#ifdef NDEBUG
      return const_iterator(m_pStr + m_length);
#else
      return {m_pStr + m_length, m_pStr, m_length};
#endif
    }

    constexpr const_iterator cbegin() const noexcept
    {
      return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
      return end();
    }


    constexpr const_reverse_iterator rbegin() const noexcept
    {
      return const_reverse_iterator(end());
    }


    constexpr const_reverse_iterator rend() const noexcept
    {
      return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
      return rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
      return rend();
    }

    constexpr std::string_view AsStringView() const noexcept
    {
      return {m_pStr, m_length};
    }

  private:
    static constexpr inline size_type CalcLength(const char* const pszStart) noexcept
    {
      if (pszStart != nullptr)
      {
        const auto* psz = pszStart;
        while (*psz != 0)
        {
          ++psz;
        }
        return psz - pszStart;
      }
      return 0u;
    }

    static constexpr int CompareN(const_pointer pLhs, const_pointer pRhs, const std::size_t compareCount) noexcept
    {
      assert(pLhs != nullptr);
      assert(pRhs != nullptr);
      // NOLINTNEXTLINE(readability-simplify-boolean-expr)
      if constexpr (true)
      {
        const const_pointer pLhsEnd = pLhs + compareCount;
        while (pLhs < pLhsEnd)
        {
          if (*pLhs != *pRhs)
          {
            return *pLhs < *pRhs ? -1 : 1;
          }
          ++pLhs;
          ++pRhs;
        }
        return 0;
      }
      return std::memcmp(pLhs, pRhs, compareCount);
    }

    static constexpr int SafeCompareN(const_pointer pLhs, const_pointer pRhs, const std::size_t compareCount) noexcept
    {
      // NOLINTNEXTLINE(readability-simplify-boolean-expr)
      if constexpr (true)
      {
        if (pLhs == nullptr)
        {
          assert(compareCount == 0u);
          return 0;
        }
        if (pRhs == nullptr)
        {
          assert(compareCount == 0u);
          return 0;
        }

        const const_pointer pLhsEnd = pLhs + compareCount;
        while (pLhs < pLhsEnd)
        {
          if (*pLhs != *pRhs)
          {
            return *pLhs < *pRhs ? -1 : 1;
          }
          ++pLhs;
          ++pRhs;
        }
        return 0;
      }
      return compareCount == 0 ? std::memcmp(pLhs, pRhs, compareCount) : 0;
    }
  };

  // Operator ==

  inline constexpr bool operator==(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  inline constexpr bool operator==(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs.compare(pszRhs) == 0;
  }

  inline constexpr bool operator==(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return rhs == pszLhs;
  }

  inline constexpr bool operator==(const StringViewLite& lhs, const std::string_view rhs) noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  inline constexpr bool operator==(const std::string_view lhs, const StringViewLite& rhs) noexcept
  {
    return rhs.compare(lhs) == 0;
  }

  // Operator !=

  inline constexpr bool operator!=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return !(lhs == rhs);
  }
  inline constexpr bool operator!=(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return !(lhs == pszRhs);
  }
  inline constexpr bool operator!=(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return rhs != pszLhs;
  }

  inline constexpr bool operator!=(const StringViewLite& lhs, const std::string_view rhs) noexcept
  {
    return !(lhs == rhs);
  }

  inline constexpr bool operator!=(const std::string_view lhs, const StringViewLite& rhs) noexcept
  {
    return !(lhs == rhs);
  }


  // Operator <

  inline constexpr bool operator<(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  inline constexpr bool operator<(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs < StringViewLite(pszRhs);
  }
  inline constexpr bool operator<(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) < rhs;
  }

  inline constexpr bool operator<(const StringViewLite& lhs, const std::string_view rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  inline constexpr bool operator<(const std::string_view lhs, const StringViewLite& rhs) noexcept
  {
    return rhs.compare(lhs) > 0;
  }

  // Operator <=

  inline constexpr bool operator<=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  inline constexpr bool operator<=(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs <= StringViewLite(pszRhs);
  }
  inline constexpr bool operator<=(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) <= rhs;
  }

  inline constexpr bool operator<=(const StringViewLite& lhs, const std::string_view rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  inline constexpr bool operator<=(const std::string_view lhs, const StringViewLite& rhs) noexcept
  {
    return rhs.compare(lhs) >= 0;
  }

  // Operator >

  inline constexpr bool operator>(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return rhs < lhs;
  }
  inline constexpr bool operator>(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs > StringViewLite(pszRhs);
  }
  inline constexpr bool operator>(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) > rhs;
  }

  inline constexpr bool operator>(const StringViewLite& lhs, const std::string_view rhs) noexcept
  {
    return rhs < lhs;
  }

  inline constexpr bool operator>(const std::string_view lhs, const StringViewLite& rhs) noexcept
  {
    return rhs < lhs;
  }

  // Operator >=

  inline constexpr bool operator>=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return rhs <= lhs;
  }
  inline constexpr bool operator>=(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs >= StringViewLite(pszRhs);
  }
  inline constexpr bool operator>=(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) >= rhs;
  }
  inline constexpr bool operator>=(const StringViewLite& lhs, const std::string_view rhs) noexcept
  {
    return rhs <= lhs;
  }
  inline constexpr bool operator>=(const std::string_view lhs, const StringViewLite& rhs) noexcept
  {
    return rhs <= lhs;
  }
}


// namespace std
// {
//   template <>
//   struct hash<Fsl::StringViewLite>
//   {
//     inline std::size_t operator()(const Fsl::StringViewLite& value) const
//     {
//       return hash<std::string_view>()(value.AsStringView());
//     }
//   };
// }

#endif
