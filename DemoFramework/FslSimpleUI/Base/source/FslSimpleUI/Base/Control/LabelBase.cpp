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
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/Control/LabelBase.hpp>
#include <FslSimpleUI/Base/ItemAlignmentUtil.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>
#include <limits>

namespace Fsl::UI
{
  using TClass = LabelBase;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyIsEnabled = TFactory::Create<bool, TClass, &TClass::IsEnabled, &TClass::SetEnabled>("Enabled");
  TDef TClass::PropertyFontColor = TFactory::Create<Color, TClass, &TClass::GetFontColor, &TClass::SetFontColor>("FontColor");
  TDef TClass::PropertyFontDisabledColor =
    TFactory::Create<Color, TClass, &TClass::GetFontDisabledColor, &TClass::SetFontDisabledColor>("FontDisabledColor");
  TDef TClass::PropertyContentAlignmentX =
    TFactory::Create<ItemAlignment, TClass, &TClass::GetContentAlignmentX, &TClass::SetContentAlignmentX>("ContentAlignmentX");
  TDef TClass::PropertyContentAlignmentY =
    TFactory::Create<ItemAlignment, TClass, &TClass::GetContentAlignmentY, &TClass::SetContentAlignmentY>("ContentAlignmentY");
}

namespace Fsl::UI
{
  LabelBase::LabelBase(const std::shared_ptr<WindowContext>& context)
    : BaseWindow(context)
    , m_windowContext(context)
    , m_fontMesh(context->TheUIContext.Get()->MeshManager, context->DefaultFont)
  {
    Enable(WindowFlags(WindowFlags::DrawEnabled));
  }

  bool LabelBase::SetEnabled(const bool value)
  {
    const bool changed = m_propertyIsEnabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool LabelBase::SetContentAlignmentX(const ItemAlignment value)
  {
    const bool changed = m_propertyContentAlignmentX.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Alignment);
    }
    return changed;
  }


  bool LabelBase::SetContentAlignmentY(const ItemAlignment value)
  {
    const bool changed = m_propertyContentAlignmentY.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Alignment);
    }
    return changed;
  }


  void LabelBase::SetFont(const std::shared_ptr<SpriteFont>& value)
  {
    if (m_fontMesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  bool LabelBase::SetFontColor(const Color value)
  {
    const bool changed = m_propertyFontColor.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool LabelBase::SetFontDisabledColor(const Color value)
  {
    const bool changed = m_propertyFontDisabledColor.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  void LabelBase::WinDraw(const UIDrawContext& context)
  {
    BaseWindow::WinDraw(context);

    const auto content = DoGetContent();
    if (!m_fontMesh.IsValid() || content.empty())
    {
      return;
    }


    PxSize2D stringSizePx = m_fontMesh.Measure(content);
    auto dstPosPxf = context.TargetRect.TopLeft();

    PxSize2D renderSizePx = RenderSizePx();
    dstPosPxf.X += TypeConverter::UncheckedTo<PxValueF>(
      ItemAlignmentUtil::CalcAlignmentPx(m_propertyContentAlignmentX.Get(), renderSizePx.Width() - stringSizePx.Width()));
    dstPosPxf.Y += TypeConverter::UncheckedTo<PxValueF>(
      ItemAlignmentUtil::CalcAlignmentPx(m_propertyContentAlignmentY.Get(), renderSizePx.Height() - stringSizePx.Height()));

    const auto color = m_propertyIsEnabled.Get() ? m_propertyFontColor.Get() : m_propertyFontDisabledColor.Get();
    context.CommandBuffer.Draw(m_fontMesh.Get(), dstPosPxf, m_cachedMeasureMinimalFontSizePx, GetFinalBaseColor() * color);
  }


  void LabelBase::DoSetContent(const StringViewLite value)
  {
    m_fontMesh.SetText(value);
  }


  PxSize2D LabelBase::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return finalSizePx;
  }


  PxSize2D LabelBase::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);
    const auto content = DoGetContent();
    auto measureRes = m_fontMesh.ComplexMeasure(content);
    m_cachedMeasureMinimalFontSizePx = measureRes.MinimalSizePx;
    return measureRes.MeasureSizePx;
  }


  DataBinding::DataBindingInstanceHandle LabelBase::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyIsEnabled, m_propertyIsEnabled),
      DataBinding::PropLinkRefs(PropertyFontColor, m_propertyFontColor),
      DataBinding::PropLinkRefs(PropertyFontDisabledColor, m_propertyFontDisabledColor),
      DataBinding::PropLinkRefs(PropertyContentAlignmentX, m_propertyContentAlignmentX),
      DataBinding::PropLinkRefs(PropertyContentAlignmentY, m_propertyContentAlignmentY));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult LabelBase::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                    const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                                  DataBinding::PropLinkRefs(PropertyIsEnabled, m_propertyIsEnabled),
                                                                  DataBinding::PropLinkRefs(PropertyFontColor, m_propertyFontColor),
                                                                  DataBinding::PropLinkRefs(PropertyFontDisabledColor, m_propertyFontDisabledColor),
                                                                  DataBinding::PropLinkRefs(PropertyContentAlignmentX, m_propertyContentAlignmentX),
                                                                  DataBinding::PropLinkRefs(PropertyContentAlignmentY, m_propertyContentAlignmentY));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void LabelBase::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyIsEnabled);
    rProperties.push_back(PropertyFontColor);
    rProperties.push_back(PropertyFontDisabledColor);
    rProperties.push_back(PropertyContentAlignmentX);
    rProperties.push_back(PropertyContentAlignmentY);
  }
}
