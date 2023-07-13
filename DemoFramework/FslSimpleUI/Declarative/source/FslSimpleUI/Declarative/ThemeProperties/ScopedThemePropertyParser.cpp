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

#include <FslSimpleUI/Declarative/ThemeProperties/ScopedThemePropertyParser.hpp>
#include <fmt/format.h>

namespace Fsl::UI::Declarative
{
  ScopedThemePropertyParser::ScopedThemePropertyParser(ReadOnlySpan<RegisteredPropertyRecord> registeredProperties,
                                                       Span<PropertyParserRecord> properties)
    : m_registeredProperties(registeredProperties)
    , m_properties(properties)
  {
  }


  ScopedThemePropertyParser::FoundThemeProperty ScopedThemePropertyParser::DoClaimRequired(const std::size_t propertyIndex)
  {
    const RegisteredPropertyRecord& entry = m_registeredProperties.at(propertyIndex);
    if (!entry.Required)
    {
      throw UsageErrorException("Used ClaimRequired used for a non required theme property");
    }

    StringViewLite propertyValue;
    ClaimResult result = TryClaim(entry.Property.GetName(), propertyValue);
    switch (result)
    {
    case ClaimResult::NotFound:
      throw NotFoundException(fmt::format("Required theme property '{}' was not found", entry.Property.GetName().AsString()));
    case ClaimResult::AlreadyClaimed:
      throw NotFoundException(fmt::format("Required theme property '{}' was already used", entry.Property.GetName().AsString()));
    case ClaimResult::Claimed:
      return {entry.Property, propertyValue};
    }
    throw NotFoundException(fmt::format("Claim failed with {}", static_cast<uint32_t>(result)));
  }


  ScopedThemePropertyParser::ClaimResult ScopedThemePropertyParser::DoTryClaim(const std::size_t propertyIndex, FoundThemeProperty& rFound)
  {
    const RegisteredPropertyRecord& entry = m_registeredProperties.at(propertyIndex);
    if (entry.Required)
    {
      throw UsageErrorException("Used ClaimOptional used for a required theme property");
    }
    StringViewLite propertyValue;
    ClaimResult result = TryClaim(entry.Property.GetName(), propertyValue);

    switch (result)
    {
    case ClaimResult::NotFound:
      rFound = {};
      return result;
    case ClaimResult::AlreadyClaimed:
      throw NotFoundException(fmt::format("Required theme property '{}' was already used", entry.Property.GetName().AsString()));
    case ClaimResult::Claimed:
      rFound = FoundThemeProperty(entry.Property, propertyValue);
      return result;
    }
    throw NotFoundException(fmt::format("Claim failed with {}", static_cast<uint32_t>(result)));
  }


  ScopedThemePropertyParser::ClaimResult ScopedThemePropertyParser::TryClaim(const PropertyName& propertyName, StringViewLite& rValue)
  {
    StringViewLite propertyNameView = propertyName.AsStringViewLite();

    auto itrFind = std::find_if(m_properties.begin(), m_properties.end(),
                                [propertyNameView](const PropertyParserRecord& entry) { return (entry.Name == propertyNameView); });

    if (itrFind == m_properties.end())
    {
      rValue = {};
      return ClaimResult::NotFound;
    }

    if (itrFind->Claimed)
    {
      rValue = {};
      return ClaimResult::AlreadyClaimed;
    }

    // Mark as used
    itrFind->Claimed = true;

    rValue = itrFind->Value;
    return ClaimResult::Claimed;
  }
}
