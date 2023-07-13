/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/Control/LabelButton.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>

namespace Fsl::UI
{
  using TClass = LabelButton;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyColorUp = TFactory::Create<Color, TClass, &TClass::GetColorUp, &TClass::SetColorUp>("ColorUp");
  TDef TClass::PropertyColorDown = TFactory::Create<Color, TClass, &TClass::GetColorDown, &TClass::SetColorDown>("ColorDown");
  TDef TClass::PropertyContent = TFactory::Create<StringViewLite, TClass, &TClass::GetContent, &TClass::SetContent>("Content");
}

namespace Fsl::UI
{
  LabelButton::LabelButton(const std::shared_ptr<WindowContext>& context)
    : ButtonBase(context)
    , m_windowContext(context)
    , m_fontMesh(context->TheUIContext.Get()->MeshManager, context->DefaultFont)
  {
    Enable(WindowFlags(WindowFlags::DrawEnabled));
  }


  bool LabelButton::SetContent(const StringViewLite value)
  {
    const bool changed = m_propertyContent.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_fontMesh.SetText(value);
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool LabelButton::SetContent(const std::string& value)
  {
    const bool changed = m_propertyContent.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_fontMesh.SetText(StringViewLiteUtil::AsStringViewLite(value));
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  void LabelButton::SetFont(const std::shared_ptr<SpriteFont>& value)
  {
    if (m_fontMesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool LabelButton::SetColorUp(const Color value)
  {
    const bool changed = m_propertyColorUp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool LabelButton::SetColorDown(const Color value)
  {
    const bool changed = m_propertyColorDown.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  void LabelButton::WinDraw(const UIDrawContext& context)
  {
    ButtonBase::WinDraw(context);

    if (m_fontMesh.IsValid())
    {
      const auto color = !IsDown() ? m_propertyColorUp.Get() : m_propertyColorDown.Get();
      context.CommandBuffer.Draw(m_fontMesh.Get(), context.TargetRect.TopLeft(), m_cachedMeasureMinimalFontSizePx, GetFinalBaseColor() * color);
    }
  }


  PxSize2D LabelButton::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return finalSizePx;
  }


  PxSize2D LabelButton::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);
    const auto measureInfo = m_fontMesh.ComplexMeasure(m_propertyContent.Get());
    m_cachedMeasureMinimalFontSizePx = measureInfo.MinimalSizePx;
    return measureInfo.MeasureSizePx;
  }


  DataBinding::DataBindingInstanceHandle LabelButton::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyColorUp, m_propertyColorUp),
      DataBinding::PropLinkRefs(PropertyColorDown, m_propertyColorDown), DataBinding::PropLinkRefs(PropertyContent, m_propertyContent));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult LabelButton::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                      const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, DataBinding::PropLinkRefs(PropertyColorUp, m_propertyColorUp),
      DataBinding::PropLinkRefs(PropertyColorDown, m_propertyColorDown), DataBinding::PropLinkRefs(PropertyContent, m_propertyContent));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void LabelButton::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyColorUp);
    rProperties.push_back(PropertyColorDown);
    rProperties.push_back(PropertyContent);
  }
}
