#ifndef FSLSIMPLEUI_DECLARATIVE_THEMEPROPERTIES_THEMEPROPERTY_HPP
#define FSLSIMPLEUI_DECLARATIVE_THEMEPROPERTIES_THEMEPROPERTY_HPP
/****************************************************************************************************************************************************
 * Copyright 2023-2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslSimpleUI/Declarative/PropertyName.hpp>
#include <FslSimpleUI/Declarative/PropertyValue.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/ThemePropertyValueRecord.hpp>
#include <span>
#include <string>
#include <typeindex>

namespace Fsl::UI::Declarative
{
  class ThemeProperty
  {
    PropertyName m_name;
    std::type_index m_type;
    std::vector<ThemePropertyValueRecord> m_validValues;

  public:
    explicit ThemeProperty(PropertyName name, const std::type_index& type);
    ThemeProperty(PropertyName name, const std::type_index& type, std::vector<ThemePropertyValueRecord> validValues);

    virtual ~ThemeProperty() = default;


    const PropertyName& GetName() const noexcept
    {
      return m_name;
    }

    const std::type_index& GetType() const noexcept
    {
      return m_type;
    }

    std::span<const ThemePropertyValueRecord> ValidValues() const
    {
      return m_validValues;
    }

  protected:
    bool IsFreeform() const noexcept
    {
      return m_validValues.empty();
    }

    uint32_t GenericParse(const StringViewLite value) const;

  private:
    std::string ValidValuesString() const;
    static void ValidateProperties(const std::vector<ThemePropertyValueRecord>& validValues);
  };
}

#endif
