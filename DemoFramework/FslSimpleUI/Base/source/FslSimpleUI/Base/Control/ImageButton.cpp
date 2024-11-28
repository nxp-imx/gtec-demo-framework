/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>

namespace Fsl::UI
{
  using TClass = ImageButton;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyBackgroundColorHoverUp =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorHoverUp, &TClass::SetBackgroundColorHoverUp>("BackgroundColorHoverUp");
  TDef TClass::PropertyBackgroundColorUp =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorUp, &TClass::SetBackgroundColorUp>("BackgroundColorUp");
  TDef TClass::PropertyBackgroundColorDown =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorDown, &TClass::SetBackgroundColorDown>("BackgroundColorDown");
  TDef TClass::PropertyBackgroundColorDisabled =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorDisabled, &TClass::SetBackgroundColorDisabled>("BackgroundColorDisabled");
  TDef TClass::PropertyBackgroundScalePolicy =
    TFactory::Create<ItemScalePolicy, TClass, &TClass::GetBackgroundScalePolicy, &TClass::SetBackgroundScalePolicy>("BackgroundScalePolicy");
  TDef TClass::PropertyScalePolicy = TFactory::Create<ItemScalePolicy, TClass, &TClass::GetScalePolicy, &TClass::SetScalePolicy>("ScalePolicy");
  TDef TClass::PropertyColorUp = TFactory::Create<UIColor, TClass, &TClass::GetColorUp, &TClass::SetColorUp>("ColorUp");
  TDef TClass::PropertyColorDown = TFactory::Create<UIColor, TClass, &TClass::GetColorDown, &TClass::SetColorDown>("ColorDown");
  TDef TClass::PropertyColorDisabled = TFactory::Create<UIColor, TClass, &TClass::GetColorDisabled, &TClass::SetColorDisabled>("ColorDisabled");
}

namespace Fsl::UI
{
  ImageButton::ImageButton(const std::shared_ptr<WindowContext>& context)
    : ButtonBase(context)
    , m_windowContext(context)
    , m_content(context->TheUIContext.Get()->MeshManager)
    , m_background(context->TheUIContext.Get()->MeshManager, context->ColorConverter)
    , m_propertyColorUp(context->ColorConverter, DefaultColor::Button::Up)
    , m_propertyColorDown(context->ColorConverter, DefaultColor::Button::Down)
    , m_propertyColorDisabled(context->ColorConverter, DefaultColor::Button::BackgroundDisabled)
    , m_currentColor(DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    , m_backgroundCurrentColor(DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
  {
    m_currentColor.SetActualValue(m_propertyColorUp.InternalColor);
    m_backgroundCurrentColor.SetActualValue(m_background.PropertyColorUp.InternalColor);
    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::MouseOver));
  }


  void ImageButton::SetContent(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_content.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ImageButton::SetContent(std::shared_ptr<ISizedSprite>&& value)
  {
    if (m_content.SetSprite(std::move(value)))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  bool ImageButton::SetBackgroundColorHoverUp(const UIColor value)
  {
    const bool changed = m_background.PropertyColorHoverUp.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool ImageButton::SetBackgroundColorUp(const UIColor value)
  {
    const bool changed = m_background.PropertyColorUp.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool ImageButton::SetBackgroundColorDown(const UIColor value)
  {
    const bool changed = m_background.PropertyColorDown.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool ImageButton::SetBackgroundColorDisabled(const UIColor value)
  {
    const bool changed = m_background.PropertyColorDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool ImageButton::SetBackgroundScalePolicy(const ItemScalePolicy value)
  {
    const bool changed = m_background.PropertyScalePolicy.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::ScalePolicy);
    }
    return changed;
  }


  bool ImageButton::SetScalePolicy(const ItemScalePolicy value)
  {
    const bool changed = m_propertyScalePolicy.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::ScalePolicy);
    }
    return changed;
  }


  bool ImageButton::SetColorUp(const UIColor value)
  {
    const bool changed = m_propertyColorUp.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ImageButton::SetColorDown(const UIColor value)
  {
    const bool changed = m_propertyColorDown.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ImageButton::SetColorDisabled(const UIColor value)
  {
    const bool changed = m_propertyColorDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  void ImageButton::SetBackground(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_background.Sprite.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ImageButton::SetBackgroundHover(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_background.HoverSprite.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ImageButton::WinDraw(const UIDrawContext& context)
  {
    ButtonBase::WinDraw(context);

    PxSize2D renderSizePx = RenderSizePx();
    const UIRenderColor finalColor(GetFinalBaseColor());
    {
      // auto* backgroundSprite = (!IsEnabled() || !m_isHovering) ? m_background.Sprite.get() : m_background.HoverSprite.get();
      // ImageImpl::Draw(*m_windowContext->Batch2D, backgroundSprite, context.TargetRect.Location(), renderSizePx,
      // m_backgroundCurrentColor.GetValue());
      const auto hMesh = (!IsEnabled() || !m_isHovering) ? m_background.Sprite.Get() : m_background.HoverSprite.Get();
      context.CommandBuffer.Draw(hMesh, context.TargetRect.Location(), renderSizePx, finalColor * m_backgroundCurrentColor.GetValue(),
                                 context.ClipContext);
    }

    {
      auto desiredImageSizePx = m_content.Measure(renderSizePx, m_propertyScalePolicy.Get());

      PxPoint2 adjustPx = (renderSizePx - desiredImageSizePx) / PxValue(2);
      PxVector2 dstPositionPxf = context.TargetRect.Location() + TypeConverter::To<PxVector2>(adjustPx);
      // ImageImpl::Draw(*m_windowContext->Batch2D, m_content.get(), dstPositionPxf, desiredImageSize, m_currentColor.GetValue());
      context.CommandBuffer.Draw(m_content.Get(), dstPositionPxf, desiredImageSizePx, finalColor * m_currentColor.GetValue(), context.ClipContext);
    }
  }


  void ImageButton::OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
    theEvent->Handled();
  }


  PxSize2D ImageButton::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    const auto backgroundScalePolicy = m_background.PropertyScalePolicy.Get();
    PxSize2D desiredBackgroudSize0Px = m_background.Sprite.Measure(finalSizePx, backgroundScalePolicy);
    PxSize2D desiredBackgroudSize1Px = m_background.HoverSprite.Measure(finalSizePx, backgroundScalePolicy);
    PxSize2D desiredImageSizePx = m_content.Measure(finalSizePx, m_propertyScalePolicy.Get());
    return PxSize2D::Max(PxSize2D::Max(desiredBackgroudSize0Px, desiredImageSizePx), desiredBackgroudSize1Px);
  }


  PxSize2D ImageButton::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);

    auto desiredBackgroudSize0Px = m_background.Sprite.Measure();
    auto desiredBackgroudSize1Px = m_background.HoverSprite.Measure();
    auto desiredImageSizePx = m_content.Measure();
    return PxSize2D::Max(PxSize2D::Max(desiredBackgroudSize0Px, desiredImageSizePx), desiredBackgroudSize1Px);
  }


  DataBinding::DataBindingInstanceHandle ImageButton::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef,
      DataBinding::PropLinkRefs(PropertyBackgroundColorHoverUp, m_background.PropertyColorHoverUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUp, m_background.PropertyColorUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDown, m_background.PropertyColorDown.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDisabled, m_background.PropertyColorDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundScalePolicy, m_background.PropertyScalePolicy),
      DataBinding::PropLinkRefs(PropertyScalePolicy, m_propertyScalePolicy),
      DataBinding::PropLinkRefs(PropertyColorUp, m_propertyColorUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDown, m_propertyColorDown.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDisabled, m_propertyColorDisabled.ExternalColor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult ImageButton::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                      const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding,
      DataBinding::PropLinkRefs(PropertyBackgroundColorHoverUp, m_background.PropertyColorHoverUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUp, m_background.PropertyColorUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDown, m_background.PropertyColorDown.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDisabled, m_background.PropertyColorDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundScalePolicy, m_background.PropertyScalePolicy),
      DataBinding::PropLinkRefs(PropertyScalePolicy, m_propertyScalePolicy),
      DataBinding::PropLinkRefs(PropertyColorUp, m_propertyColorUp.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDown, m_propertyColorDown.ExternalColor),
      DataBinding::PropLinkRefs(PropertyColorDisabled, m_propertyColorDisabled.ExternalColor));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void ImageButton::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyBackgroundColorHoverUp);
    rProperties.push_back(PropertyBackgroundColorUp);
    rProperties.push_back(PropertyBackgroundColorDown);
    rProperties.push_back(PropertyBackgroundColorDisabled);
    rProperties.push_back(PropertyBackgroundScalePolicy);
    rProperties.push_back(PropertyScalePolicy);
    rProperties.push_back(PropertyColorUp);
    rProperties.push_back(PropertyColorDown);
    rProperties.push_back(PropertyColorDisabled);
  }

  void ImageButton::UpdateAnimation(const TimeSpan& timeSpan)
  {
    BaseWindow::UpdateAnimation(timeSpan);
    m_currentColor.Update(timeSpan);
    m_backgroundCurrentColor.Update(timeSpan);
  }

  bool ImageButton::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    const bool isEnabled = IsEnabled();
    const bool isUp = !IsDown();

    auto backgroundColor = isEnabled
                             ? (isUp ? (!m_isHovering ? m_background.PropertyColorUp.InternalColor : m_background.PropertyColorHoverUp.InternalColor)
                                     : m_background.PropertyColorDown.InternalColor)
                             : m_background.PropertyColorDisabled.InternalColor;
    m_backgroundCurrentColor.SetValue(backgroundColor);

    auto color = isEnabled ? (isUp ? m_propertyColorUp.InternalColor : m_propertyColorDown.InternalColor) : m_propertyColorDisabled.InternalColor;
    m_currentColor.SetValue(color);

    if (forceCompleteAnimation)
    {
      m_backgroundCurrentColor.ForceComplete();
      m_currentColor.ForceComplete();
    }

    const bool isAnimating = !m_currentColor.IsCompleted() || !m_backgroundCurrentColor.IsCompleted();
    return isAnimating;
  }

}
