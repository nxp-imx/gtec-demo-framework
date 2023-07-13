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

#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/ThemeProperty.hpp>
#include <fmt/format.h>

namespace Fsl::UI::Declarative
{
  ThemeProperty::ThemeProperty(PropertyName name)
    : m_name(std::move(name))
  {
  }


  ThemeProperty::ThemeProperty(PropertyName name, std::vector<ThemePropertyValueRecord> validValues)
    : m_name(std::move(name))
    , m_validValues(std::move(validValues))
  {
    ValidateProperties(m_validValues);
  }


  uint32_t ThemeProperty::GenericParse(const StringViewLite value) const
  {
    for (const auto& entry : m_validValues)
    {
      if (entry.Name == value)
      {
        return entry.GenericValueId;
      }
    }
    throw NotFoundException(fmt::format("Could not find '{}' valid values are: '{}'", value, ValidValuesString()));
  }


  std::string ThemeProperty::ValidValuesString() const
  {
    if (m_validValues.empty())
    {
      return "";
    }
    std::string res;
    const std::size_t count = m_validValues.size() - 1;
    for (std::size_t i = 0; i < count; ++i)
    {
      res += m_validValues[i].Name.AsString() + ", ";
    }
    res += m_validValues[count].Name.AsString();
    return res;
  }


  void ThemeProperty::ValidateProperties(const std::vector<ThemePropertyValueRecord>& validValues)
  {
    if (validValues.empty())
    {
      throw UsageErrorException("A property must contain valid values");
    }

    for (std::size_t i = 0; i < validValues.size(); ++i)
    {
      for (std::size_t j = i + 1; j < validValues.size(); ++j)
      {
        if (validValues[i].Name == validValues[j].Name)
        {
          throw UsageErrorException("A property value must be unique: {}");
        }
      }
    }
  }
}
