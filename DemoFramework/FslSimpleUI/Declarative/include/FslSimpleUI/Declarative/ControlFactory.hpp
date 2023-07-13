#ifndef FSLSIMPLEUI_DECLARATIVE_CONTROLFACTORY_HPP
#define FSLSIMPLEUI_DECLARATIVE_CONTROLFACTORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2022-2023 NXP
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

#include <FslSimpleUI/Declarative/ADeclarativeControlFactory.hpp>
#include <FslSimpleUI/Declarative/PropertyRecord.hpp>
#include <FslSimpleUI/Declarative/RadioGroupManager.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/PropertyParserRecord.hpp>
#include <FslSimpleUI/Declarative/ThemeProperties/RegisteredPropertyRecord.hpp>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Fsl::UI
{
  class BaseWindow;

  namespace Theme
  {
    class IThemeControlFactory;
  }
}


namespace Fsl::UI::Declarative
{
  class ControlFactory final
  {
    std::shared_ptr<Theme::IThemeControlFactory> m_controlFactory;
    std::map<ControlName, std::unique_ptr<ADeclarativeControlFactory>, std::less<>> m_factories;

    std::vector<RegisteredPropertyRecord> m_registeredPropertyScratchpad;
    std::vector<PropertyParserRecord> m_createPropertiesScratchpad;

  public:
    explicit ControlFactory(std::shared_ptr<Theme::IThemeControlFactory> controlFactory);
    ~ControlFactory();

    void Register(std::unique_ptr<ADeclarativeControlFactory> factory);

    template <typename T>
    void Register()
    {
      Register(std::make_unique<T>());
    }

    std::shared_ptr<BaseWindow> TryCreate(RadioGroupManager& rRadioGroupManager, const StringViewLite& name,
                                          std::vector<PropertyRecord>& rPropertyRecords);
  };
}

#endif
