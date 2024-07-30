/****************************************************************************************************************************************************
 * Copyright 2022-2024 NXP
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

#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Declarative/ControlFactory.hpp>
#include <FslSimpleUI/Declarative/ControlInfoUtil.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <fmt/format.h>
#include <cassert>
#include <utility>
#include "Factories/Control/BackgroundWindowDeclarativeControlFactory.hpp"
#include "Factories/Control/BottomBarDeclarativeControlFactory.hpp"
#include "Factories/Control/CheckBoxDeclarativeControlFactory.hpp"
#include "Factories/Control/ContainedTextButtonDeclarativeControlFactory.hpp"
#include "Factories/Control/DividerDeclarativeControlFactory.hpp"
#include "Factories/Control/FloatingButtonDeclarativeControlFactory.hpp"
#include "Factories/Control/FmtValueLabelFloatDeclarativeControlFactory.hpp"
#include "Factories/Control/FmtValueLabelInt32DeclarativeControlFactory.hpp"
#include "Factories/Control/FmtValueLabelUInt32DeclarativeControlFactory.hpp"
#include "Factories/Control/FmtValueLabelUInt8DeclarativeControlFactory.hpp"
#include "Factories/Control/LabelDeclarativeControlFactory.hpp"
#include "Factories/Control/LeftBarDeclarativeControlFactory.hpp"
#include "Factories/Control/OutlinedTextButtonDeclarativeControlFactory.hpp"
#include "Factories/Control/RadioButtonDeclarativeControlFactory.hpp"
#include "Factories/Control/RightBarDeclarativeControlFactory.hpp"
#include "Factories/Control/ScrollViewerDeclarativeControlFactory.hpp"
#include "Factories/Control/SliderFloatDeclarativeControlFactory.hpp"
#include "Factories/Control/SliderFmtValueFloatDeclarativeControlFactory.hpp"
#include "Factories/Control/SliderFmtValueInt32DeclarativeControlFactory.hpp"
#include "Factories/Control/SliderFmtValueUInt32DeclarativeControlFactory.hpp"
#include "Factories/Control/SliderFmtValueUInt8DeclarativeControlFactory.hpp"
#include "Factories/Control/SliderInt32DeclarativeControlFactory.hpp"
#include "Factories/Control/SliderUInt32DeclarativeControlFactory.hpp"
#include "Factories/Control/SliderUInt8DeclarativeControlFactory.hpp"
#include "Factories/Control/SwitchDeclarativeControlFactory.hpp"
#include "Factories/Control/TextButtonDeclarativeControlFactory.hpp"
#include "Factories/Control/TopBarDeclarativeControlFactory.hpp"
#include "Factories/Layout/CanvasLayoutDeclarativeControlFactory.hpp"
#include "Factories/Layout/ComplexStackLayoutDeclarativeControlFactory.hpp"
#include "Factories/Layout/FillLayoutDeclarativeControlFactory.hpp"
#include "Factories/Layout/GridLayoutDeclarativeControlFactory.hpp"
#include "Factories/Layout/StackLayoutDeclarativeControlFactory.hpp"
#include "Factories/Layout/UniformStackLayoutDeclarativeControlFactory.hpp"
#include "Factories/Layout/UniformWrapLayoutDeclarativeControlFactory.hpp"
#include "Factories/Layout/WrapLayoutDeclarativeControlFactory.hpp"
#include "RegisterDefaultPrimitiveTypes.hpp"

namespace Fsl::UI::Declarative
{
  namespace
  {
    Span<RegisteredPropertyRecord> FillScratchpad(std::vector<RegisteredPropertyRecord>& rScratchPad,
                                                  const std::span<const ControlPropertyRecord> propertyRecords)
    {
      rScratchPad.clear();
      rScratchPad.reserve(propertyRecords.size());
      for (const auto& entry : propertyRecords)
      {
        const ThemeProperty* pProperty = entry.Property.get();
        assert(pProperty != nullptr);
        rScratchPad.emplace_back(*pProperty, entry.Required);
      }
      return SpanUtil::AsSpan(rScratchPad, 0, propertyRecords.size());
    }

    Span<PropertyParserRecord> FillScratchpad(std::vector<PropertyParserRecord>& rScratchPad, const ReadOnlySpan<PropertyRecord> propertyRecords)
    {
      rScratchPad.clear();
      rScratchPad.reserve(propertyRecords.size());
      for (const auto& entry : propertyRecords)
      {
        rScratchPad.emplace_back(entry.Name.AsStringViewLite(), entry.Value.AsStringViewLite());
      }
      return SpanUtil::AsSpan(rScratchPad, 0, propertyRecords.size());
    }

    void EraseClaimed(std::vector<PropertyRecord>& rPropertyRecords, Span<PropertyParserRecord> properties)
    {
      assert(rPropertyRecords.size() == properties.size());

      std::size_t index = 0;
      auto itr = rPropertyRecords.begin();
      while (itr != rPropertyRecords.end())
      {
        if (properties[index].Claimed)
        {
          itr = rPropertyRecords.erase(itr);
        }
        else
        {
          ++itr;
        }
        ++index;
      }
    }
  }

  ControlFactory::ControlFactory(std::shared_ptr<Theme::IThemeControlFactory> controlFactory)
    : m_controlFactory(std::move(controlFactory))
  {
    if (!m_controlFactory)
    {
      throw std::invalid_argument("Must provide a valid theme control factory");
    }

    Fsl::UI::Declarative::Internal::RegisterDefaultPrimitiveTypes(m_primitiveTypeRegistry);

    // Theme controls
    Register<BackgroundWindowDeclarativeControlFactory>();
    Register<BottomBarDeclarativeControlFactory>();
    Register<CheckBoxDeclarativeControlFactory>();
    Register<ContainedTextButtonDeclarativeControlFactory>();
    Register<DividerDeclarativeControlFactory>();
    Register<FloatingButtonDeclarativeControlFactory>();
    Register<FmtValueLabelFloatDeclarativeControlFactory>();
    Register<FmtValueLabelInt32DeclarativeControlFactory>();
    Register<FmtValueLabelUInt32DeclarativeControlFactory>();
    Register<FmtValueLabelUInt8DeclarativeControlFactory>();
    Register<LabelDeclarativeControlFactory>();
    Register<LeftBarDeclarativeControlFactory>();
    Register<OutlinedTextButtonDeclarativeControlFactory>();
    Register<RadioButtonDeclarativeControlFactory>();
    Register<RightBarDeclarativeControlFactory>();
    Register<SliderFmtValueFloatDeclarativeControlFactory>();
    Register<SliderFmtValueInt32DeclarativeControlFactory>();
    Register<SliderFmtValueUInt32DeclarativeControlFactory>();
    Register<SliderFmtValueUInt8DeclarativeControlFactory>();
    Register<ScrollViewerDeclarativeControlFactory>();
    Register<SliderFloatDeclarativeControlFactory>();
    Register<SliderInt32DeclarativeControlFactory>();
    Register<SliderUInt32DeclarativeControlFactory>();
    Register<SliderUInt8DeclarativeControlFactory>();
    Register<SwitchDeclarativeControlFactory>();
    Register<TextButtonDeclarativeControlFactory>();
    Register<TopBarDeclarativeControlFactory>();

    // Layouts
    Register<CanvasLayoutDeclarativeControlFactory>();
    Register<ComplexStackLayoutDeclarativeControlFactory>();
    Register<FillLayoutDeclarativeControlFactory>();
    Register<GridLayoutDeclarativeControlFactory>();
    Register<StackLayoutDeclarativeControlFactory>();
    Register<UniformStackLayoutDeclarativeControlFactory>();
    Register<UniformWrapLayoutDeclarativeControlFactory>();
    Register<WrapLayoutDeclarativeControlFactory>();
  }


  ControlFactory::~ControlFactory() = default;


  void ControlFactory::Register(std::unique_ptr<ADeclarativeControlFactory> factory)
  {
    if (!factory)
    {
      throw std::invalid_argument("can not be null");
    }

    const ControlName& controlName = factory->GetName();
    if (m_factories.find(controlName) != m_factories.end())
    {
      throw UsageErrorException(fmt::format("Can not register a factory twice for the same control '{}'", controlName.AsString()));
    }

    m_factories.emplace(controlName, std::move(factory));
  }


  std::shared_ptr<BaseWindow> ControlFactory::TryCreate(RadioGroupManager& rRadioGroupManager, const std::string_view name,
                                                        std::vector<PropertyRecord>& rPropertyRecords)
  {
    Theme::IThemeControlFactory& uiFactory = *m_controlFactory;

    const auto itrFind = m_factories.find(name);
    if (itrFind != m_factories.end())
    {
      Span<RegisteredPropertyRecord> registeredProperties = FillScratchpad(m_registeredPropertyScratchpad, itrFind->second->Properties());
      Span<PropertyParserRecord> properties = FillScratchpad(m_createPropertiesScratchpad, SpanUtil::AsReadOnlySpan(rPropertyRecords));

      ScopedThemePropertyParser propertyParser(registeredProperties, properties);
      auto res = itrFind->second->Create(DeclarativeControlFactoryCreateInfo(uiFactory, rRadioGroupManager, propertyParser));
      EraseClaimed(rPropertyRecords, properties);
      return res;
    }

    // Missing: ImageButton
    return {};
  }

  std::vector<ControlName> ControlFactory::GetControlNames() const
  {
    std::vector<ControlName> names;
    names.reserve(m_factories.size());

    for (const auto& entry : m_factories)
    {
      names.push_back(entry.first);
    }
    return names;
  }


  std::span<const ControlPropertyRecord> ControlFactory::GetControlThemeProperties(const ControlName& name) const
  {
    const auto itrFind = m_factories.find(name);
    if (itrFind == m_factories.end())
    {
      return {};
    }

    // Extract the 'theme' properties
    return itrFind->second->Properties();
  }


  DataBinding::DependencyPropertyDefinitionVector ControlFactory::GetControlProperties(const ControlName& name)
  {
    auto window = TryCreateDummyControl(name.AsString());
    if (!window)
    {
      return {};
    }

    DataBinding::DependencyPropertyDefinitionVector result;
    window->ExtractProperties(result);
    return result;
  }


  ControlType ControlFactory::GetControlType(const ControlName& name)
  {
    auto window = TryCreateDummyControl(name.AsString());
    if (!window)
    {
      return ControlType::Normal;
    }

    if (ControlInfoUtil::IsContentControl(window.get()))
    {
      return ControlType::Content;
    }
    if (ControlInfoUtil::IsContainer(window.get()))
    {
      return ControlType::Layout;
    }
    return ControlType::Normal;
  }


  std::shared_ptr<BaseWindow> ControlFactory::TryCreateDummyControl(const std::string_view name)
  {
    const auto itrFind = m_factories.find(name);
    if (itrFind == m_factories.end())
    {
      return {};
    }

    RadioGroupManager dummyRadioGroupManager;
    std::vector<PropertyRecord> propertyRecordsDummy;

    std::uint32_t groupNameCount = 0;
    for (const auto& propertyRecord : itrFind->second->Properties())
    {
      std::span<const ThemePropertyValueRecord> validValues = propertyRecord.Property->ValidValues();
      if (propertyRecord.Required)
      {
        if (!validValues.empty())
        {
          // for now just select the first value
          propertyRecordsDummy.emplace_back(propertyRecord.Property->GetName(), validValues[0].Name);
        }
        else
        {
          // FIX: this is kind of a workaround, as we dont really know for sure its a groupname. But it works for now
          std::string groupName(fmt::format("groupName:{}", groupNameCount));
          propertyRecordsDummy.emplace_back(propertyRecord.Property->GetName(), PropertyValue(groupName));
          dummyRadioGroupManager.Get(groupName.c_str());
          ++groupNameCount;
        }
      }
    }

    return TryCreate(dummyRadioGroupManager, name, propertyRecordsDummy);
  }

}
