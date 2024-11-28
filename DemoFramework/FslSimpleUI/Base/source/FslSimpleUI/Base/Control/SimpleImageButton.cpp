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
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>

namespace Fsl::UI
{
  using TClass = SimpleImageButton;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyScalePolicy = TFactory::Create<ItemScalePolicy, TClass, &TClass::GetScalePolicy, &TClass::SetScalePolicy>("ScalePolicy");
  TDef TClass::PropertyColorUp = TFactory::Create<UIColor, TClass, &TClass::GetColorUp, &TClass::SetColorUp>("ColorUp");
  TDef TClass::PropertyColorDown = TFactory::Create<UIColor, TClass, &TClass::GetColorDown, &TClass::SetColorDown>("ColorDown");
  TDef TClass::PropertyColorDisabled = TFactory::Create<UIColor, TClass, &TClass::GetColorDisabled, &TClass::SetColorDisabled>("ColorDisabled");
}

namespace Fsl::UI
{
  SimpleImageButton::SimpleImageButton(const std::shared_ptr<WindowContext>& context)
    : ButtonBase(context)
    , m_windowContext(context)
    , m_content(context->TheUIContext.Get()->MeshManager)
    , m_propertyColorUp(context->ColorConverter, DefaultColor::Button::Up)
    , m_propertyColorDown(context->ColorConverter, DefaultColor::Button::Down)
    , m_propertyColorDisabled(context->ColorConverter, DefaultColor::Button::BackgroundDisabled)
    , m_currentColor(DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
  {
    m_currentColor.SetActualValue(context->ColorConverter.Convert(DefaultColor::Button::Up));
    Enable(WindowFlags::DrawEnabled);
  }


  void SimpleImageButton::SetContent(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_content.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void SimpleImageButton::SetContent(std::shared_ptr<ISizedSprite>&& value)
  {
    if (m_content.SetSprite(std::move(value)))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool SimpleImageButton::SetScalePolicy(const ItemScalePolicy value)
  {
    const bool changed = m_propertyScalePolicy.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::ScalePolicy);
    }
    return changed;
  }


  bool SimpleImageButton::SetColorUp(const UIColor value)
  {
    const bool changed = m_propertyColorUp.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool SimpleImageButton::SetColorDown(const UIColor value)
  {
    const bool changed = m_propertyColorDown.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool SimpleImageButton::SetColorDisabled(const UIColor value)
  {
    const bool changed = m_propertyColorDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  void SimpleImageButton::WinDraw(const UIDrawContext& context)
  {
    ButtonBase::WinDraw(context);

    // ImageImpl::Draw(*m_windowContext->Batch2D, m_content.get(), context.TargetRect.Location(), RenderSizePx(), m_currentColor.GetValue());
    context.CommandBuffer.Draw(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), GetFinalBaseColor() * m_currentColor.GetValue(),
                               context.ClipContext);
  }


  PxSize2D SimpleImageButton::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return m_content.Measure(finalSizePx, m_propertyScalePolicy.Get());
  }


  PxSize2D SimpleImageButton::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);
    return m_content.Measure();
  }


  void SimpleImageButton::UpdateAnimation(const TimeSpan& timeSpan)
  {
    BaseWindow::UpdateAnimation(timeSpan);
    m_currentColor.Update(timeSpan);
  }

  bool SimpleImageButton::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    const bool isEnabled = IsEnabled();

    auto color =
      isEnabled ? (!IsDown() ? m_propertyColorUp.InternalColor : m_propertyColorDown.InternalColor) : m_propertyColorDisabled.InternalColor;
    m_currentColor.SetValue(color);

    if (forceCompleteAnimation)
    {
      m_currentColor.ForceComplete();
    }

    const bool isAnimating = !m_currentColor.IsCompleted();
    return isAnimating;
  }


  DataBinding::DataBindingInstanceHandle SimpleImageButton::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyScalePolicy, m_propertyScalePolicy),
      DataBinding::PropLinkRefs(PropertyColorUp, m_propertyColorUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDown, m_propertyColorDown.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDisabled, m_propertyColorDisabled.ExternalColor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult SimpleImageButton::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                            const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, DataBinding::PropLinkRefs(PropertyScalePolicy, m_propertyScalePolicy),
      DataBinding::PropLinkRefs(PropertyColorUp, m_propertyColorUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDown, m_propertyColorDown.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDisabled, m_propertyColorDisabled.ExternalColor));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void SimpleImageButton::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyScalePolicy);
    rProperties.push_back(PropertyColorUp);
    rProperties.push_back(PropertyColorDown);
    rProperties.push_back(PropertyColorDisabled);
  }

}
