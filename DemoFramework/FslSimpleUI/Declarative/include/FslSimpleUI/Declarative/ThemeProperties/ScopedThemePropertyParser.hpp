#ifndef FSLSIMPLEUI_DECLARATIVE_THEMEPROPERTIES_SCOPEDTHEMEPROPERTYPARSER_HPP
#define FSLSIMPLEUI_DECLARATIVE_THEMEPROPERTIES_SCOPEDTHEMEPROPERTYPARSER_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/PropertyParserRecord.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/RegisteredPropertyRecord.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/TypedThemeProperty.hpp>
#include <memory>
#include <optional>

namespace Fsl::UI::Declarative
{
  class ScopedThemePropertyParser final
  {
    enum ClaimResult
    {
      NotFound,
      // The entry was found but had already been claimed (this should not occur as each name can only be registered once, but a user might request
      // the parameter multiple times by accident)
      AlreadyClaimed,
      Claimed,
    };

    struct FoundThemeProperty
    {
      const ThemeProperty* pProperty{nullptr};
      StringViewLite PropertyValue;

      FoundThemeProperty() = default;

      FoundThemeProperty(const ThemeProperty& property, const StringViewLite propertyValue)
        : pProperty(&property)
        , PropertyValue(propertyValue)
      {
      }
    };

    ReadOnlySpan<RegisteredPropertyRecord> m_registeredProperties;
    Span<PropertyParserRecord> m_properties;

  public:
    ScopedThemePropertyParser(const ScopedThemePropertyParser&) = delete;
    ScopedThemePropertyParser& operator=(const ScopedThemePropertyParser&) = delete;

    ScopedThemePropertyParser(ReadOnlySpan<RegisteredPropertyRecord> registeredProperties, Span<PropertyParserRecord> properties);


    template <typename T>
    typename T::value_type ClaimRequired(const std::size_t propertyIndex)
    {
      FoundThemeProperty res = DoClaimRequired(propertyIndex);
      assert(res.pProperty != nullptr);
      const TypedThemeProperty<typename T::value_type>& typedEntry = dynamic_cast<const TypedThemeProperty<typename T::value_type>&>(*res.pProperty);
      return typedEntry.Parse(res.PropertyValue);
    }

    template <typename T>
    typename T::value_type ClaimOptional(const std::size_t propertyIndex, typename T::value_type defaultValue)
    {
      FoundThemeProperty res;
      ClaimResult result = DoTryClaim(propertyIndex, res);
      if (result != ClaimResult::Claimed)
      {
        return defaultValue;
      }
      const TypedThemeProperty<typename T::value_type>& typedEntry = dynamic_cast<const TypedThemeProperty<typename T::value_type>&>(*res.pProperty);
      return typedEntry.Parse(res.PropertyValue);
    }

  private:
    FoundThemeProperty DoClaimRequired(const std::size_t propertyIndex);
    ClaimResult DoTryClaim(const std::size_t propertyIndex, FoundThemeProperty& rFound);
    ClaimResult TryClaim(const PropertyName& propertyName, StringViewLite& rValue);
  };
}

#endif
