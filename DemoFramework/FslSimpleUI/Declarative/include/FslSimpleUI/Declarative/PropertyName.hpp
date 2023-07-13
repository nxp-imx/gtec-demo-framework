#ifndef FSLSIMPLEUI_DECLARATIVE_PROPERTYNAME_HPP
#define FSLSIMPLEUI_DECLARATIVE_PROPERTYNAME_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/String/StringViewLite.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <utility>

namespace Fsl::UI::Declarative
{
  class PropertyName final
  {
    std::string m_name;

  public:
    explicit PropertyName(std::string name)
      : m_name(std::move(name))
    {
      if (!IsValidName(StringViewLiteUtil::AsStringViewLite(m_name)))
      {
        throw std::invalid_argument(fmt::format("not a valid name '{}'", m_name));
      }
    }

    explicit PropertyName(const StringViewLite name)
      : PropertyName(StringViewLiteUtil::ToString(name))
    {
    }

    explicit PropertyName(const char* const psz)
      : PropertyName(std::string(psz))
    {
    }

    const std::string& AsString() const noexcept
    {
      return m_name;
    }

    StringViewLite AsStringViewLite() const noexcept
    {
      return StringViewLiteUtil::AsStringViewLite(m_name);
    }

    static constexpr bool IsValidName(const StringViewLite name) noexcept
    {
      if (name.empty())
      {
        return false;
      }

      if (!IsValidStartCharacter(name[0]))
      {
        return false;
      }

      char previous = ' ';
      for (auto ch : name)
      {
        if (!IsValidCharacter(ch) || (ch == ':' && previous == ':') || (ch == '.' && previous == '.'))
        {
          return false;
        }
        previous = ch;
      }
      return true;
    }

    static constexpr bool IsValidStartCharacter(const char ch) noexcept
    {
      return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }

    static constexpr bool IsValidCharacter(const char ch) noexcept
    {
      return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == ':' || ch == '.';
    }

    bool operator==(const PropertyName& other) const noexcept
    {
      return m_name == other.m_name;
    }

    bool operator!=(const PropertyName& other) const noexcept
    {
      return m_name != other.m_name;
    }

    int compare(const PropertyName& value) const noexcept
    {
      return m_name.compare(value.m_name);
    }
  };

  // op==
  inline bool operator==(const PropertyName& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() == rhs;
  }

  inline bool operator==(const StringViewLite lhs, const PropertyName& rhs) noexcept
  {
    return lhs == rhs.AsString();
  }


  // op!=

  inline bool operator!=(const PropertyName& lhs, const StringViewLite rhs) noexcept
  {
    return !(lhs == rhs);
  }

  inline bool operator!=(const StringViewLite lhs, const PropertyName& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  // Operator <

  inline bool operator<(const PropertyName& lhs, const PropertyName& rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  inline bool operator<(const StringViewLite lhs, const PropertyName& rhs) noexcept
  {
    return lhs < rhs.AsString();
  }

  inline bool operator<(const PropertyName& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() < rhs;
  }

  // Operator <=

  inline bool operator<=(const PropertyName& lhs, const PropertyName& rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  inline bool operator<=(const StringViewLite lhs, const PropertyName& rhs) noexcept
  {
    return lhs <= rhs.AsString();
  }

  inline bool operator<=(const PropertyName& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() <= rhs;
  }

  // Operator >

  inline bool operator>(const PropertyName& lhs, const PropertyName& rhs) noexcept
  {
    return lhs.compare(rhs) > 0;
  }

  inline bool operator>(const StringViewLite lhs, const PropertyName& rhs) noexcept
  {
    return lhs > rhs.AsString();
  }

  inline bool operator>(const PropertyName& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() > rhs;
  }

  // Operator >=

  inline bool operator>=(const PropertyName& lhs, const PropertyName& rhs) noexcept
  {
    return lhs.compare(rhs) >= 0;
  }

  inline bool operator>=(const StringViewLite lhs, const PropertyName& rhs) noexcept
  {
    return lhs >= rhs.AsString();
  }

  inline bool operator>=(const PropertyName& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() >= rhs;
  }
}

#endif
