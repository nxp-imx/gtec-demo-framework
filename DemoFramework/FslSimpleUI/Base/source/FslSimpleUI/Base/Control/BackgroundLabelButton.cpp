/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/ItemAlignmentUtil.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>

namespace Fsl::UI
{
  using TClass = BackgroundLabelButton;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyBackgroundHoverOverlayColorUp =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundHoverOverlayColorUp, &TClass::SetBackgroundHoverOverlayColorUp>(
      "BackgroundHoverOverlayColorUp");
  TDef TClass::PropertyBackgroundHoverOverlayColorDown =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundHoverOverlayColorDown, &TClass::SetBackgroundHoverOverlayColorDown>(
      "BackgroundHoverOverlayColorDown");
  TDef TClass::PropertyBackgroundColorHoverUp =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorHoverUp, &TClass::SetBackgroundColorHoverUp>("BackgroundColorHoverUp");
  TDef TClass::PropertyBackgroundColorUp =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorUp, &TClass::SetBackgroundColorUp>("BackgroundColorUp");
  TDef TClass::PropertyBackgroundColorDown =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorDown, &TClass::SetBackgroundColorDown>("BackgroundColorDown");
  TDef TClass::PropertyBackgroundColorDisabled =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorDisabled, &TClass::SetBackgroundColorDisabled>("BackgroundColorDisabled");
  TDef TClass::PropertyFontColorUp = TFactory::Create<Color, TClass, &TClass::GetFontColorUp, &TClass::SetFontColorUp>("FontColorUp");
  TDef TClass::PropertyFontColorDown = TFactory::Create<Color, TClass, &TClass::GetFontColorDown, &TClass::SetFontColorDown>("FontColorDown");
  TDef TClass::PropertyFontColorDisabled =
    TFactory::Create<Color, TClass, &TClass::GetFontColorDisabled, &TClass::SetFontColorDisabled>("FontColorDisabled");
  TDef TClass::PropertyPadding = TFactory::Create<DpThicknessF, TClass, &TClass::GetPadding, &TClass::SetPadding>("Padding");
  TDef TClass::PropertyContentAlignmentX =
    TFactory::Create<ItemAlignment, TClass, &TClass::GetContentAlignmentX, &TClass::SetContentAlignmentX>("ContentAlignmentX");
  TDef TClass::PropertyContentAlignmentY =
    TFactory::Create<ItemAlignment, TClass, &TClass::GetContentAlignmentY, &TClass::SetContentAlignmentY>("ContentAlignmentY");
  TDef TClass::PropertyContent = TFactory::Create<StringViewLite, TClass, &TClass::GetContent, &TClass::SetContent>("Content");
}

namespace Fsl::UI
{
  BackgroundLabelButton::BackgroundLabelButton(const std::shared_ptr<WindowContext>& context)
    : ButtonBase(context)
    , m_windowContext(context)
    , m_fontMesh(context->TheUIContext.Get()->MeshManager, context->DefaultFont)
    , m_backgroundNormal(context->TheUIContext.Get()->MeshManager)
    , m_backgroundHover(context->TheUIContext.Get()->MeshManager)
    , m_backgroundHoverOverlay(context->TheUIContext.Get()->MeshManager)
    , m_backgroundCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    , m_backgroundCurrentHoverOverlayColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    , m_fontCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
  {
    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::MouseOver));
    UpdateAnimationState(true);
  }


  void BackgroundLabelButton::SetBackground(const std::shared_ptr<IContentSprite>& value)
  {
    if (m_backgroundNormal.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void BackgroundLabelButton::SetBackgroundHover(const std::shared_ptr<IContentSprite>& value)
  {
    if (m_backgroundHover.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void BackgroundLabelButton::SetBackgroundHoverOverlay(const std::shared_ptr<IContentSprite>& value)
  {
    if (m_backgroundHoverOverlay.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool BackgroundLabelButton::SetContent(const StringViewLite value)
  {
    const bool changed = m_propertyContent.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_fontMesh.SetText(value);
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetContent(std::string&& value)
  {
    const bool changed = m_propertyContent.Set(ThisDependencyObject(), std::move(value));
    if (changed)
    {
      m_fontMesh.SetText(m_propertyContent.Get());
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  void BackgroundLabelButton::SetFont(const std::shared_ptr<SpriteFont>& value)
  {
    if (m_fontMesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool BackgroundLabelButton::SetBackgroundHoverOverlayColorUp(const Color value)
  {
    const bool changed = m_propertyBackgroundHoverOverlayColorUp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetBackgroundHoverOverlayColorDown(const Color value)
  {
    const bool changed = m_propertyBackgroundHoverOverlayColorDown.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetBackgroundColorHoverUp(const Color value)
  {
    const bool changed = m_propertyBackgroundColorHoverUp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetBackgroundColorUp(const Color value)
  {
    const bool changed = m_propertyBackgroundColorUp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetBackgroundColorDown(const Color value)
  {
    const bool changed = m_propertyBackgroundColorDown.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetBackgroundColorDisabled(const Color value)
  {
    const bool changed = m_propertyBackgroundColorDisabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetFontColorUp(const Color value)
  {
    const bool changed = m_propertyFontColorUp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetFontColorDown(const Color value)
  {
    const bool changed = m_propertyFontColorDown.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetFontColorDisabled(const Color value)
  {
    const bool changed = m_propertyFontColorDisabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool BackgroundLabelButton::SetPadding(const DpThicknessF value)
  {
    const bool changed = m_propertyPaddingDp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetContentAlignmentX(const ItemAlignment value)
  {
    const bool changed = m_propertyContentAlignmentX.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Alignment);
    }
    return changed;
  }


  bool BackgroundLabelButton::SetContentAlignmentY(const ItemAlignment value)
  {
    const bool changed = m_propertyContentAlignmentY.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Alignment);
    }
    return changed;
  }


  void BackgroundLabelButton::WinDraw(const UIDrawContext& context)
  {
    ButtonBase::WinDraw(context);

    const PxSize2D renderSizePx = RenderSizePx();

    const Color finalColor = GetFinalBaseColor();
    if (m_backgroundNormal.IsValid())
    {
      if (!IsDown() && m_isHovering && m_backgroundHover.IsValid() && IsEnabled())
      {
        context.CommandBuffer.Draw(m_backgroundHover.Get(), context.TargetRect.TopLeft(), renderSizePx,
                                   finalColor * m_backgroundCurrentColor.GetValue());
      }
      else
      {
        context.CommandBuffer.Draw(m_backgroundNormal.Get(), context.TargetRect.TopLeft(), renderSizePx,
                                   finalColor * m_backgroundCurrentColor.GetValue());
      }
    }

    {
      const Color backgroundHoverOverlayColor = m_backgroundCurrentHoverOverlayColor.GetValue();
      if (backgroundHoverOverlayColor.A() > 0)
      {
        context.CommandBuffer.Draw(m_backgroundHoverOverlay.Get(), context.TargetRect.TopLeft(), renderSizePx,
                                   finalColor * backgroundHoverOverlayColor);
      }
    }


    if (m_fontMesh.IsValid() && !m_propertyContent.Get().empty())
    {
      const PxThickness contentMarginPx =
        PxThickness::Max(m_backgroundHover.GetRenderContentMarginPx(), m_backgroundNormal.GetRenderContentMarginPx());

      const PxThickness paddingPx = m_windowContext->UnitConverter.ToPxThickness(m_propertyPaddingDp.Get());
      const PxValue widthPx = renderSizePx.Width() - contentMarginPx.SumX() - paddingPx.SumX() - m_labelMeasureInfo.MeasureSizePx.Width();
      const PxValue heightPx = renderSizePx.Height() - contentMarginPx.SumY() - paddingPx.SumY() - m_labelMeasureInfo.MeasureSizePx.Height();
      const PxValue offsetXPx = ItemAlignmentUtil::CalcAlignmentPx(m_propertyContentAlignmentX.Get(), widthPx);
      const PxValue offsetYPx = ItemAlignmentUtil::CalcAlignmentPx(m_propertyContentAlignmentY.Get(), heightPx);

      auto posPxf = context.TargetRect.TopLeft();
      posPxf.X += TypeConverter::UncheckedTo<PxValueF>(contentMarginPx.Left() + paddingPx.Left() + offsetXPx);
      posPxf.Y += TypeConverter::UncheckedTo<PxValueF>(contentMarginPx.Top() + paddingPx.Top() + offsetYPx);

      context.CommandBuffer.Draw(m_fontMesh.Get(), posPxf, m_labelMeasureInfo.MinimalSizePx, finalColor * m_fontCurrentColor.GetValue());
    }
  }


  void BackgroundLabelButton::OnMouseOver(const RoutedEventArgs& /*args*/, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
    theEvent->Handled();
  }

  PxSize2D BackgroundLabelButton::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return finalSizePx;
  }


  PxSize2D BackgroundLabelButton::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);

    auto paddingPx = m_windowContext->UnitConverter.ToPxThickness(m_propertyPaddingDp.Get());
    m_labelMeasureInfo = m_fontMesh.ComplexMeasure(m_propertyContent.Get());
    PxSize2D desiredSizePx = paddingPx.Sum() + m_labelMeasureInfo.MeasureSizePx;

    const auto backgroundHoverInfo = m_backgroundHover.GetRenderContentInfo();
    const auto backgroundNormalInfo = m_backgroundNormal.GetRenderContentInfo();
    const auto backgroundHoverOverlayInfo = m_backgroundHoverOverlay.GetRenderContentInfo();

    // Apply the max content padding
    desiredSizePx += PxSize2D::Max(backgroundHoverInfo.RenderContentMarginPx.Sum(), backgroundNormalInfo.RenderContentMarginPx.Sum(),
                                   backgroundHoverOverlayInfo.RenderContentMarginPx.Sum());
    // Chose the largest size
    desiredSizePx =
      PxSize2D::Max(desiredSizePx, backgroundHoverInfo.RenderSizePx, backgroundNormalInfo.RenderSizePx, backgroundHoverOverlayInfo.RenderSizePx);
    return desiredSizePx;
  }


  DataBinding::DataBindingInstanceHandle BackgroundLabelButton::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef,
      DataBinding::PropLinkRefs(PropertyBackgroundHoverOverlayColorUp, m_propertyBackgroundHoverOverlayColorUp),
      DataBinding::PropLinkRefs(PropertyBackgroundHoverOverlayColorDown, m_propertyBackgroundHoverOverlayColorDown),
      DataBinding::PropLinkRefs(PropertyBackgroundColorHoverUp, m_propertyBackgroundColorHoverUp),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUp, m_propertyBackgroundColorUp),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDown, m_propertyBackgroundColorDown),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDisabled, m_propertyBackgroundColorDisabled),
      DataBinding::PropLinkRefs(PropertyFontColorUp, m_propertyFontColorUp),
      DataBinding::PropLinkRefs(PropertyFontColorDown, m_propertyFontColorDown),
      DataBinding::PropLinkRefs(PropertyFontColorDisabled, m_propertyFontColorDisabled),
      DataBinding::PropLinkRefs(PropertyPadding, m_propertyPaddingDp),
      DataBinding::PropLinkRefs(PropertyContentAlignmentX, m_propertyContentAlignmentX),
      DataBinding::PropLinkRefs(PropertyContentAlignmentY, m_propertyContentAlignmentY),
      DataBinding::PropLinkRefs(PropertyContent, m_propertyContent));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult BackgroundLabelButton::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                                const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding,
      DataBinding::PropLinkRefs(PropertyBackgroundHoverOverlayColorUp, m_propertyBackgroundHoverOverlayColorUp),
      DataBinding::PropLinkRefs(PropertyBackgroundHoverOverlayColorDown, m_propertyBackgroundHoverOverlayColorDown),
      DataBinding::PropLinkRefs(PropertyBackgroundColorHoverUp, m_propertyBackgroundColorHoverUp),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUp, m_propertyBackgroundColorUp),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDown, m_propertyBackgroundColorDown),
      DataBinding::PropLinkRefs(PropertyBackgroundColorDisabled, m_propertyBackgroundColorDisabled),
      DataBinding::PropLinkRefs(PropertyFontColorUp, m_propertyFontColorUp),
      DataBinding::PropLinkRefs(PropertyFontColorDown, m_propertyFontColorDown),
      DataBinding::PropLinkRefs(PropertyFontColorDisabled, m_propertyFontColorDisabled),
      DataBinding::PropLinkRefs(PropertyPadding, m_propertyPaddingDp),
      DataBinding::PropLinkRefs(PropertyContentAlignmentX, m_propertyContentAlignmentX),
      DataBinding::PropLinkRefs(PropertyContentAlignmentY, m_propertyContentAlignmentY),
      DataBinding::PropLinkRefs(PropertyContent, m_propertyContent));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void BackgroundLabelButton::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyBackgroundHoverOverlayColorUp);
    rProperties.push_back(PropertyBackgroundHoverOverlayColorDown);
    rProperties.push_back(PropertyBackgroundColorHoverUp);
    rProperties.push_back(PropertyBackgroundColorUp);
    rProperties.push_back(PropertyBackgroundColorDown);
    rProperties.push_back(PropertyBackgroundColorDisabled);
    rProperties.push_back(PropertyFontColorUp);
    rProperties.push_back(PropertyFontColorDown);
    rProperties.push_back(PropertyFontColorDisabled);
    rProperties.push_back(PropertyPadding);
    rProperties.push_back(PropertyContentAlignmentX);
    rProperties.push_back(PropertyContentAlignmentY);
    rProperties.push_back(PropertyContent);
  }


  void BackgroundLabelButton::UpdateAnimation(const TimeSpan& timeSpan)
  {
    ButtonBase::UpdateAnimation(timeSpan);
    m_backgroundCurrentColor.Update(timeSpan);
    m_backgroundCurrentHoverOverlayColor.Update(timeSpan);
    m_fontCurrentColor.Update(timeSpan);
  }


  bool BackgroundLabelButton::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    const bool isEnabled = IsEnabled();
    const bool isDown = IsDown();

    // Determine background color

    const auto& backgroundColor = GetBackgroundColor(isEnabled, isDown, m_isHovering);
    m_backgroundCurrentColor.SetValue(backgroundColor);

    const auto& overlayBaseColor = !isDown ? m_propertyBackgroundHoverOverlayColorUp.Get() : m_propertyBackgroundHoverOverlayColorDown.Get();
    const auto overlayColor = isEnabled && m_isHovering ? overlayBaseColor : Color::ClearA(overlayBaseColor);
    m_backgroundCurrentHoverOverlayColor.SetValue(overlayColor);

    const auto& fontColor = (isEnabled ? (!isDown ? m_propertyFontColorUp.Get() : m_propertyFontColorDown.Get()) : m_propertyFontColorDisabled.Get());
    m_fontCurrentColor.SetValue(fontColor);

    if (forceCompleteAnimation)
    {
      m_backgroundCurrentColor.ForceComplete();
      m_backgroundCurrentHoverOverlayColor.ForceComplete();
      m_fontCurrentColor.ForceComplete();
    }

    const bool isAnimating =
      !m_backgroundCurrentColor.IsCompleted() || !m_backgroundCurrentHoverOverlayColor.IsCompleted() || !m_fontCurrentColor.IsCompleted();

    // Toggle animation on/off
    return isAnimating;
  }

  Color BackgroundLabelButton::GetBackgroundColor(const bool isEnabled, const bool isDown, const bool isHovering) const
  {
    if (isEnabled)
    {
      if (!isDown)
      {
        return !isHovering ? m_propertyBackgroundColorUp.Get() : m_propertyBackgroundColorHoverUp.Get();
      }
      return m_propertyBackgroundColorDown.Get();
    }
    return m_propertyBackgroundColorDisabled.Get();
  }
}
