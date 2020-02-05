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

  protected:
    const char* m_pStr{nullptr};
    size_type m_length{0u};

  public:
    constexpr StringViewLite() noexcept = default;
    constexpr StringViewLite(const StringViewLite& other) noexcept = default;
    constexpr StringViewLite(const const_pointer pStr, size_type count)
      : m_pStr(pStr)
      , m_length(count)
    {
      assert(pStr != nullptr || count == 0u);
    }

    constexpr StringViewLite(const const_pointer psz)
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

    const_reference at(size_type pos) const
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

  private:
    static inline constexpr size_type CalcLength(const char* const pszStart)
    {
      assert(pszStart != nullptr);
      auto psz = pszStart;
      while (*psz != 0)
      {
        ++psz;
      }
      return psz - pszStart;
    }
  };

  extern bool operator==(const StringViewLite& lhs, const StringViewLite& rhs) noexcept;
  extern bool operator!=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept;
  extern bool operator<(const StringViewLite& lhs, const StringViewLite& rhs) noexcept;
  extern bool operator<=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept;
  extern bool operator>(const StringViewLite& lhs, const StringViewLite& rhs) noexcept;
  extern bool operator>=(const StringViewLite& lhs, const StringViewLite& rhs) noexcept;

  extern bool operator==(const StringViewLite& lhs, const char* const pszRhs) noexcept;
  extern bool operator!=(const StringViewLite& lhs, const char* const pszRhs) noexcept;
  extern bool operator==(const char* const pszLhs, const StringViewLite& rhs) noexcept;
  extern bool operator!=(const char* const pszLhs, const StringViewLite& rhs) noexcept;
}

#endif
