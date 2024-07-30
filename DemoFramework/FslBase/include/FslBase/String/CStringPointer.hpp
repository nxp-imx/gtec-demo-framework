#ifndef FSLBASE_STRING_CSTRINGPOINTER_HPP
#define FSLBASE_STRING_CSTRINGPOINTER_HPP
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

#include <FslBase/String/CStringUtil_Length.hpp>
#include <FslBase/String/StringViewUtil.hpp>
#include <cassert>
#include <cstddef>
#include <string_view>

namespace Fsl
{
  // A basic encapsulation of a zero terminated string 'c-style' that never returns a nullptr.
  class CStringPointer final
  {
  public:
    using value_type = char;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr const size_type npos = static_cast<size_type>(-1);
    static constexpr const char* Empty = "";

  protected:
    const char* m_psz{Empty};

  public:
    constexpr CStringPointer() noexcept = default;
    constexpr CStringPointer(const CStringPointer& other) noexcept = default;

    constexpr CStringPointer(const const_pointer psz) noexcept    // NOLINT(google-explicit-constructor)
      : m_psz(psz != nullptr ? psz : Empty)
    {
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_pointer c_str() const noexcept
    {
      return m_psz;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_pointer data() const noexcept
    {
      return m_psz;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type size() const noexcept
    {
      return CStringUtil::UncheckedLength(m_psz);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool empty() const noexcept
    {
      return m_psz[0] == 0;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type length() const noexcept
    {
      return CStringUtil::UncheckedLength(m_psz);
    }


    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(const char* const psz) const noexcept
    {
      return compare(CStringPointer(psz));
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(CStringPointer value) const noexcept
    {
      return compare(value.AsStringView());
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(std::string_view value) const noexcept
    {
      return AsStringView().compare(value);
    }

    constexpr std::string_view AsStringView() const noexcept
    {
      return StringViewUtil::UncheckedCreate(m_psz);
    }

    constexpr operator std::string_view() const noexcept    // NOLINT(google-explicit-constructor)
    {
      return StringViewUtil::UncheckedCreate(m_psz);
    }


    constexpr std::strong_ordering operator<=>(const CStringPointer& other) const noexcept
    {
      const auto val = compare(other);
      return val == 0 ? std::strong_ordering::equal : (val < 0 ? std::strong_ordering::less : std::strong_ordering::greater);
    }

    constexpr bool operator==(const CStringPointer& other) const noexcept
    {
      return compare(other) == 0;
    }
  };


  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // const char*const psz
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr auto operator<=>(const CStringPointer lhs, const char* const pszRhz) noexcept
  {
    return lhs <=> CStringPointer(pszRhz);
  }

  inline constexpr bool operator==(const CStringPointer lhs, const char* const pszRhz) noexcept
  {
    return lhs == CStringPointer(pszRhz);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // std::string_view
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr auto operator<=>(const CStringPointer lhs, std::string_view rhs) noexcept
  {
    return lhs.AsStringView() <=> rhs;
  }

  inline constexpr bool operator==(const CStringPointer lhs, std::string_view rhs) noexcept
  {
    return lhs.AsStringView() == rhs;
  }
}

#endif
