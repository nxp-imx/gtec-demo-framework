#ifndef FSLSIMPLEUI_DECLARATIVE_ADECLARATIVECONTROLFACTORY_HPP
#define FSLSIMPLEUI_DECLARATIVE_ADECLARATIVECONTROLFACTORY_HPP
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
#include <FslSimpleUI/Declarative/ControlName.hpp>
#include <FslSimpleUI/Declarative/ControlPropertyRecord.hpp>
#include <FslSimpleUI/Declarative/RadioGroupManager.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/ScopedThemePropertyParser.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <memory>
#include <span>
#include <string>
#include <typeinfo>
#include <vector>

namespace Fsl::UI::Declarative
{
  struct DeclarativeControlFactoryCreateInfo
  {
    Theme::IThemeControlFactory& ThemeControlFactory;
    RadioGroupManager& RadioGroups;
    ScopedThemePropertyParser& PropertyParser;


    explicit DeclarativeControlFactoryCreateInfo(Theme::IThemeControlFactory& themeControlFactory, RadioGroupManager& radioGroups,
                                                 ScopedThemePropertyParser& propertyParser)
      : ThemeControlFactory(themeControlFactory)
      , RadioGroups(radioGroups)
      , PropertyParser(propertyParser)
    {
    }
  };

  class ADeclarativeControlFactory
  {
    ControlName m_name;
    std::vector<ControlPropertyRecord> m_properties;

  public:
    explicit ADeclarativeControlFactory(ControlName name);
    virtual ~ADeclarativeControlFactory() = default;

    void AddThemeProperty(std::unique_ptr<ThemeProperty> themeProperty, const bool required);

    template <typename T>
    void AddThemeProperty(bool required)
    {
      AddThemeProperty(std::make_unique<T>(), required);
    }


    std::span<const ControlPropertyRecord> Properties() const;

    /// <summary>
    /// The name of the control
    /// </summary>
    /// <returns></returns>
    const ControlName& GetName() const noexcept;

    /// <summary>
    /// Create a instance of the control
    /// </summary>
    /// <returns></returns>
    virtual std::shared_ptr<BaseWindow> Create(const DeclarativeControlFactoryCreateInfo& createInfo) = 0;
  };
}

#endif
