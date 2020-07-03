#ifndef FSLBASE_STRING_STRINGVIEWLITE_HPP
#define FSLBASE_STRING_STRINGVIEWLITE_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>

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

    static constexpr const size_type npos = size_type(-1);

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

    constexpr StringViewLite(const const_pointer psz)    // NOLINT(google-explicit-constructor)
      : m_pStr(psz)
      , m_length(CalcLength(psz))
    {
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

    constexpr const_reference back() const
    {
      assert(!empty());
      return m_pStr[m_length - 1];
    }

    constexpr const_reference front() const
    {
      assert(!empty());
      return m_pStr[0];
    }

    constexpr void remove_prefix(size_type n)
    {
      assert(n <= m_length);
      m_pStr += n;
      m_length -= n;
    }

    constexpr void remove_suffix(size_type n)
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

    // constexpr int compare(StringViewLite value) const noexcept
    int compare(StringViewLite value) const noexcept;

    // constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const;
    int compare(size_type pos1, size_type count1, StringViewLite v) const
    {
      return substr(pos1, count1).compare(v);
    }

    // constexpr int compare(size_type pos1, size_type count1, StringViewLite v, size_type pos2, size_type count2) const
    int compare(size_type pos1, size_type count1, StringViewLite v, size_type pos2, size_type count2) const
    {
      return substr(pos1, count1).compare(v.substr(pos2, count2));
    }

    // constexpr int compare(const char* const psz) const
    int compare(const value_type* const psz) const
    {
      return compare(StringViewLite(psz));
    }

    // constexpr int compare(size_type pos1, size_type count1, const char*const psz) const;
    int compare(size_type pos1, size_type count1, const char* const psz) const
    {
      return substr(pos1, count1).compare(StringViewLite(psz));
    }

    // constexpr int compare(size_type pos1, size_type count1, const char*const psz, size_type count2) const;
    int compare(size_type pos1, size_type count1, const char* const psz, size_type count2) const
    {
      return substr(pos1, count1).compare(StringViewLite(psz, count2));
    }

    // constexpr bool starts_with(StringViewLite value) const noexcept;
    bool starts_with(StringViewLite value) const noexcept;

    constexpr bool starts_with(char ch) const noexcept
    {
      return (m_length > 0u && m_pStr[0] == ch);
    }

    // constexpr bool starts_with(const char* const psz) const;
    bool starts_with(const char* const psz) const
    {
      return starts_with(StringViewLite(psz));
    }

    // constexpr bool ends_with(StringViewLite value) const noexcept;
    bool ends_with(StringViewLite value) const noexcept;

    constexpr bool ends_with(char ch) const noexcept
    {
      return (m_length > 0u && m_pStr[m_length - 1u] == ch);
    }

    // constexpr bool ends_with(const char*const psz) const;
    bool ends_with(const char* const psz) const
    {
      return ends_with(StringViewLite(psz));
    }

    // constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept
    // size_type find(StringViewLite value, const size_type pos = 0) const noexcept;

    constexpr size_type find(const value_type ch, size_type pos = 0) const noexcept
    {
      while (pos < m_length && m_pStr[pos] != ch)
      {
        ++pos;
      }
      return (pos < m_length ? pos : npos);
    }

    // constexpr size_type rfind(basic_string_view v, size_type pos = npos) const noexcept;
    // constexpr size_type rfind(CharT c, size_type pos = npos) const noexcept;

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

    // constexpr size_type rfind(const CharT* s, size_type pos, size_type count) const;
    // constexpr size_type rfind(const CharT* s, size_type pos = npos) const;


  private:
    static constexpr inline size_type CalcLength(const char* const pszStart)
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
  };

  // Operator ==

  inline bool operator==(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  inline bool operator==(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs.compare(pszRhs) == 0;
  }

  inline bool operator==(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return rhs == pszLhs;
  }

  // Operator !=

  inline bool operator!=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return !(lhs == rhs);
  }
  inline bool operator!=(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return !(lhs == pszRhs);
  }
  inline bool operator!=(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return rhs != pszLhs;
  }

  // Operator <

  inline bool operator<(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  inline bool operator<(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs < StringViewLite(pszRhs);
  }
  inline bool operator<(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) < rhs;
  }

  // Operator <=

  inline bool operator<=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  inline bool operator<=(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs <= StringViewLite(pszRhs);
  }
  inline bool operator<=(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) <= rhs;
  }

  // Operator >

  inline bool operator>(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return rhs < lhs;
  }
  inline bool operator>(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs > StringViewLite(pszRhs);
  }
  inline bool operator>(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) > rhs;
  }

  // Operator >=

  inline bool operator>=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept
  {
    return rhs <= lhs;
  }
  inline bool operator>=(const StringViewLite& lhs, const char* const pszRhs) noexcept
  {
    return lhs >= StringViewLite(pszRhs);
  }
  inline bool operator>=(const char* const pszLhs, const StringViewLite& rhs) noexcept
  {
    return StringViewLite(pszLhs) >= rhs;
  }

}

#endif
