#ifndef FSLSIMPLEUI_DECLARATIVE_PROPERTYVALUE_HPP
#define FSLSIMPLEUI_DECLARATIVE_PROPERTYVALUE_HPP
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

#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <utility>

namespace Fsl::UI::Declarative
{
  class PropertyValue final
  {
    std::string m_name;

  public:
    explicit PropertyValue(std::string name)
      : m_name(std::move(name))
    {
    }

    explicit PropertyValue(const StringViewLite name)
      : m_name(name)
    {
    }

    explicit PropertyValue(const char* const psz)
      : PropertyValue(std::string(psz))
    {
    }

    const std::string& AsString() const noexcept
    {
      return m_name;
    }

    StringViewLite AsStringViewLite() const noexcept
    {
      return std::string_view(m_name);
    }

    bool operator==(const PropertyValue& other) const noexcept
    {
      return m_name == other.m_name;
    }

    bool operator!=(const PropertyValue& other) const noexcept
    {
      return m_name != other.m_name;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    int compare(const PropertyValue& value) const noexcept
    {
      return m_name.compare(value.m_name);
    }
  };

  // op==
  inline bool operator==(const PropertyValue& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() == rhs;
  }

  inline bool operator==(const StringViewLite lhs, const PropertyValue& rhs) noexcept
  {
    return lhs == rhs.AsString();
  }


  // op!=

  inline bool operator!=(const PropertyValue& lhs, const StringViewLite rhs) noexcept
  {
    return !(lhs == rhs);
  }

  inline bool operator!=(const StringViewLite lhs, const PropertyValue& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  // Operator <

  inline bool operator<(const PropertyValue& lhs, const PropertyValue& rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  inline bool operator<(const StringViewLite lhs, const PropertyValue& rhs) noexcept
  {
    return lhs < rhs.AsString();
  }

  inline bool operator<(const PropertyValue& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() < rhs;
  }

  // Operator <=

  inline bool operator<=(const PropertyValue& lhs, const PropertyValue& rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  inline bool operator<=(const StringViewLite lhs, const PropertyValue& rhs) noexcept
  {
    return lhs <= rhs.AsString();
  }

  inline bool operator<=(const PropertyValue& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() <= rhs;
  }

  // Operator >

  inline bool operator>(const PropertyValue& lhs, const PropertyValue& rhs) noexcept
  {
    return lhs.compare(rhs) > 0;
  }

  inline bool operator>(const StringViewLite lhs, const PropertyValue& rhs) noexcept
  {
    return lhs > rhs.AsString();
  }

  inline bool operator>(const PropertyValue& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() > rhs;
  }

  // Operator >=

  inline bool operator>=(const PropertyValue& lhs, const PropertyValue& rhs) noexcept
  {
    return lhs.compare(rhs) >= 0;
  }

  inline bool operator>=(const StringViewLite lhs, const PropertyValue& rhs) noexcept
  {
    return lhs >= rhs.AsString();
  }

  inline bool operator>=(const PropertyValue& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsString() >= rhs;
  }
}

#endif
