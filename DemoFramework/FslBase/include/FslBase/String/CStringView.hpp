#ifndef FSLBASE_STRING_CSTRINGVIEW_HPP
#define FSLBASE_STRING_CSTRINGVIEW_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslBase/String/StringViewLite.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Fsl
{
  // A basic encapsulation of a zero terminated string view that never returns a nullptr.
  class CStringView final
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
    static constexpr const char* g_empty = "";

  protected:
    const char* m_psz{g_empty};
    size_type m_length{0u};

  public:
    constexpr CStringView() noexcept = default;
    constexpr CStringView(const CStringView& other) noexcept = default;
    //! @brief overload that allows you to create a StringViewLite from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr CStringView(const const_pointer psz, size_type count, const OptimizationCheckFlag /*unused*/) noexcept
      : m_psz(psz)
      , m_length(count)
    {
      assert(psz != nullptr);
      assert(psz[count] == 0);
      assert(CalcLength(m_psz) == m_length);
    }

    explicit constexpr CStringView(const const_pointer psz, size_type count)
      : m_psz(psz != nullptr ? psz : g_empty)
      , m_length(count)
    {
      if (psz == nullptr && count != 0u)
      {
        throw std::invalid_argument("psz can not be null");
      }
      if (m_length != CalcLength(m_psz))
      {
        throw std::invalid_argument("the supplied length did not match the expected length");
      }
      if (m_psz[m_length] != 0)
      {
        throw std::invalid_argument("pointer was not null terminated at the expected location");
      }
    }

    constexpr CStringView(const const_pointer psz) noexcept    // NOLINT(google-explicit-constructor)
      : m_psz(psz != nullptr ? psz : g_empty)
      , m_length(CalcLength(m_psz))
    {
    }

    constexpr const_pointer c_str() const noexcept
    {
      return m_psz;
    }

    constexpr const_pointer data() const noexcept
    {
      return m_psz;
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
      assert(m_psz != nullptr);
      return m_psz[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
      if (pos >= size())
      {
        throw std::out_of_range("pos out of range");
      }
      assert(m_psz != nullptr);
      return m_psz[pos];
    }

    constexpr size_type length() const noexcept
    {
      return m_length;
    }

    constexpr const_reference back() const
    {
      assert(!empty());
      return m_psz[m_length - 1];
    }

    constexpr const_reference front() const
    {
      assert(!empty());
      return m_psz[0];
    }

    // constexpr void remove_prefix(size_type n)
    //{
    //   assert(n <= m_length);
    //   m_psz += n;
    //   m_length -= n;
    // }

    constexpr int compare(const char* const psz) const noexcept
    {
      return compare(StringViewLite(psz));
    }

    constexpr int compare(CStringView value) const noexcept
    {
      return compare(value.AsStringViewLite());
    }

    constexpr int compare(StringViewLite value) const noexcept
    {
      return AsStringViewLite().compare(value);
    }

    constexpr int compare(std::string_view value) const noexcept
    {
      return AsStringView().compare(value);
    }

    constexpr bool starts_with(const char* const psz) const noexcept
    {
      return starts_with(StringViewLite(psz));
    }

    constexpr bool starts_with(CStringView view) const noexcept
    {
      return starts_with(view.AsStringViewLite());
    }

    constexpr bool starts_with(StringViewLite view) const noexcept
    {
      return AsStringViewLite().starts_with(view);
    }

    constexpr bool starts_with(char ch) const noexcept
    {
      return AsStringViewLite().starts_with(ch);
    }

    constexpr bool ends_with(const char* const psz) const noexcept
    {
      return ends_with(StringViewLite(psz));
    }

    constexpr bool ends_with(const CStringView view) const noexcept
    {
      return ends_with(view.AsStringViewLite());
    }

    constexpr bool ends_with(const StringViewLite view) const noexcept
    {
      return AsStringViewLite().ends_with(view);
    }

    constexpr bool ends_with(char ch) const noexcept
    {
      return AsStringViewLite().ends_with(ch);
    }


    constexpr size_type find(const value_type ch, size_type pos = 0) const noexcept
    {
      return AsStringViewLite().find(ch, pos);
    }

    constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept
    {
      return AsStringViewLite().rfind(ch, pos);
    }


    constexpr const_iterator begin() const noexcept
    {
#ifdef NDEBUG
      return const_iterator(m_psz);
#else
      return {m_psz, m_psz, m_length};
#endif
    }

    constexpr const_iterator end() const noexcept
    {
#ifdef NDEBUG
      return const_iterator(m_psz + m_length);
#else
      return {m_psz + m_length, m_psz, m_length};
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

    constexpr StringViewLite AsStringViewLite() const noexcept
    {
      return StringViewLite(m_psz, m_length, OptimizationCheckFlag::NoCheck);
    }

    constexpr std::string_view AsStringView() const noexcept
    {
      return {m_psz, m_length};
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
  };

  // Operator ==

  inline constexpr bool operator==(const CStringView& lhs, const CStringView& rhs) noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  inline constexpr bool operator==(const CStringView& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  inline constexpr bool operator==(const StringViewLite lhs, const CStringView& rhs) noexcept
  {
    return rhs.compare(lhs) == 0;
  }

  inline constexpr bool operator==(const CStringView& lhs, const std::string_view rhs) noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  inline constexpr bool operator==(const std::string_view lhs, const CStringView& rhs) noexcept
  {
    return rhs.compare(lhs) == 0;
  }

  inline constexpr bool operator==(const CStringView& lhs, const char* const pszRhs) noexcept
  {
    return lhs.compare(pszRhs) == 0;
  }

  inline constexpr bool operator==(const char* const pszLhs, const CStringView& rhs) noexcept
  {
    return rhs.compare(pszLhs) == 0;
  }


  // Operator !=

  inline constexpr bool operator!=(const CStringView& lhs, const CStringView& rhs) noexcept
  {
    return !(lhs == rhs);
  }
  inline constexpr bool operator!=(const CStringView& lhs, const StringViewLite rhs) noexcept
  {
    return !(lhs == rhs);
  }
  inline constexpr bool operator!=(const StringViewLite lhs, const CStringView& rhs) noexcept
  {
    return !(lhs == rhs);
  }
  inline constexpr bool operator!=(const CStringView& lhs, const char* const pszRhs) noexcept
  {
    return !(lhs == pszRhs);
  }
  inline constexpr bool operator!=(const char* const pszLhs, const CStringView& rhs) noexcept
  {
    return !(pszLhs == rhs);
  }

  // Operator <

  inline constexpr bool operator<(const CStringView& lhs, const CStringView& rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  inline constexpr bool operator<(const CStringView& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsStringViewLite() < rhs;
  }

  inline constexpr bool operator<(const StringViewLite lhs, const CStringView& rhs) noexcept
  {
    return lhs < rhs.AsStringViewLite();
  }

  inline constexpr bool operator<(const CStringView& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() < pszRhs;
  }

  inline constexpr bool operator<(const char* const pszLhs, const CStringView& rhs) noexcept
  {
    return pszLhs < rhs.AsStringViewLite();
  }

  // Operator <=

  inline constexpr bool operator<=(const CStringView& lhs, const CStringView& rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  inline constexpr bool operator<=(const CStringView& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsStringViewLite() <= rhs;
  }

  inline constexpr bool operator<=(const StringViewLite lhs, const CStringView& rhs) noexcept
  {
    return lhs <= rhs.AsStringViewLite();
  }

  inline constexpr bool operator<=(const CStringView& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() <= pszRhs;
  }

  inline constexpr bool operator<=(const char* const pszLhs, const CStringView& rhs) noexcept
  {
    return pszLhs <= rhs.AsStringViewLite();
  }

  // Operator >

  inline constexpr bool operator>(const CStringView& lhs, const CStringView& rhs) noexcept
  {
    return rhs < lhs;
  }
  inline constexpr bool operator>(const CStringView& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsStringViewLite() > rhs;
  }
  inline constexpr bool operator>(const StringViewLite lhs, const CStringView& rhs) noexcept
  {
    return lhs > rhs.AsStringViewLite();
  }
  inline constexpr bool operator>(const CStringView& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() > pszRhs;
  }
  inline constexpr bool operator>(const char* const pszLhs, const CStringView& rhs) noexcept
  {
    return pszLhs > rhs.AsStringViewLite();
  }

  // Operator >=

  inline constexpr bool operator>=(const CStringView& lhs, const CStringView& rhs) noexcept
  {
    return rhs <= lhs;
  }
  inline constexpr bool operator>=(const CStringView& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsStringViewLite() >= rhs;
  }
  inline constexpr bool operator>=(const StringViewLite lhs, const CStringView& rhs) noexcept
  {
    return lhs >= rhs.AsStringViewLite();
  }

  inline constexpr bool operator>=(const CStringView& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsStringViewLite() >= StringViewLite(pszRhs);
  }

  inline constexpr bool operator>=(const char* const pszLhs, const CStringView& rhs) noexcept
  {
    return StringViewLite(pszLhs) >= rhs.AsStringViewLite();
  }
}

#endif
