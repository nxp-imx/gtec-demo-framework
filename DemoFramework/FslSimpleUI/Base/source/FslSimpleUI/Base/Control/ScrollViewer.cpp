/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/TypeConverter_Float.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Control/ScrollGestureAnimationConfig.hpp>
#include <FslSimpleUI/Base/Control/ScrollViewer.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Log/FmtMovementOwnership.hpp>
#include <FslSimpleUI/Base/Log/FmtMovementTransactionAction.hpp>
#include <FslSimpleUI/Base/MovementOwnership.hpp>
#include <FslSimpleUI/Base/MovementTransactionAction.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/ResolutionChangedInfo.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>

namespace Fsl::UI
{
  using TClass = ScrollViewer;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyScrollMode = TFactory::Create<ScrollModeFlags, TClass, &TClass::GetScrollMode, &TClass::SetScrollMode>("ScrollMode");
  TDef TClass::PropertyDragFlickDeceleration =
    TFactory::Create<float, TClass, &TClass::GetDragFlickDeceleration, &TClass::SetDragFlickDeceleration>("DragFlickDeceleration");
  TDef TClass::PropertyDragFlickTransitionType =
    TFactory::Create<TransitionType, TClass, &TClass::GetDragFlickTransitionType, &TClass::SetDragFlickTransitionType>("DragFlickTransitionType");
  TDef TClass::PropertyDragEndAnimTimeMultiplier =
    TFactory::Create<float, TClass, &TClass::GetDragEndAnimTimeMultiplier, &TClass::SetDragEndAnimTimeMultiplier>("DragEndAnimTimeMultiplier");
  TDef TClass::PropertyBounceSpringStiffness =
    TFactory::Create<float, TClass, &TClass::GetBounceSpringStiffness, &TClass::SetBounceSpringStiffness>("BounceSpringStiffness");
  TDef TClass::PropertyBounceAnimationTime =
    TFactory::Create<TimeSpan, TClass, &TClass::GetBounceAnimationTime, &TClass::SetBounceAnimationTime>("BounceAnimationTime");
  TDef TClass::PropertyBounceTransitionType =
    TFactory::Create<TransitionType, TClass, &TClass::GetBounceTransitionType, &TClass::SetBounceTransitionType>("BounceTransitionType");
  TDef TClass::PropertyClipContent = TFactory::Create<bool, TClass, &TClass::GetClipContent, &TClass::SetClipContent>("ClipContent");
  TDef TClass::PropertyCursorColor = TFactory::Create<UIColor, TClass, &TClass::GetCursorColor, &TClass::SetCursorColor>("CursorColor");
}

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr float DefaultDragFlickDeceleration = 200.0f;    //  x*100 dp per second
      constexpr int16_t DefaultDragEndAnimTimeMultiplier = 4;
      constexpr float DefaultSpringStiffness = 5.0f;
      constexpr TimeSpan DefaultBounceTime(TimeSpan::FromMilliseconds(1000));
      constexpr TransitionType DefaultDragFlickTransitionType = TransitionType::EaseOutSine;
      constexpr TransitionType DefaultBounceTransitionType = TransitionType::EaseInOutQuad;
      // TransitionType::EaseOutQuad
      // TransitionType::EaseOutBounce;
      constexpr bool DefaultClipContent = false;
      constexpr UIColor DefaultCursorColor = UIColors::White();
    }


    constexpr DpValueF ToDpPerSecond(const float value) noexcept
    {
      return DpValueF(value * 100);
    }

    constexpr PxPoint2 CalcCursorOffsetAndSize(const PxSize1D cursorRenderSizePx, const PxSize1D viewRenderSizePx, const PxSize1D contentRenderSizePx,
                                               const PxValue scrollPositionOffsetPx) noexcept
    {
      if (cursorRenderSizePx <= PxSize1D() || cursorRenderSizePx > viewRenderSizePx || viewRenderSizePx > contentRenderSizePx ||
          contentRenderSizePx <= PxSize1D())
      {
        return {};
      }

      const float cursorScaleToFillView = static_cast<float>(viewRenderSizePx.RawValue()) / static_cast<float>(cursorRenderSizePx.RawValue());

      // Calculate how much of the content the view can see
      const float viewPercentageOfContent = static_cast<float>(viewRenderSizePx.RawValue()) / static_cast<float>(contentRenderSizePx.RawValue());

      int32_t scaledCursorHeightPx = TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(cursorRenderSizePx.RawValue()) *
                                                                               (cursorScaleToFillView * viewPercentageOfContent));
      scaledCursorHeightPx = MathHelper::Clamp(scaledCursorHeightPx, cursorRenderSizePx.RawValue(), viewRenderSizePx.RawValue());

      const int32_t viewPixelsLeft = viewRenderSizePx.RawValue() - scaledCursorHeightPx;

      const PxValue maxViewScrollOffset = contentRenderSizePx - viewRenderSizePx;
      if (maxViewScrollOffset <= PxValue())
      {
        return {};
      }

      const float viewPositionInContentPercentage =
        (-static_cast<float>(scrollPositionOffsetPx.Value) / static_cast<float>(maxViewScrollOffset.Value));

      return PxPoint2::Create(TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(viewPixelsLeft) * viewPositionInContentPercentage),
                              scaledCursorHeightPx);
    }

    constexpr MovementOwnership ToMovementOwnership(EventHandlingStatus handlingStatus) noexcept
    {
      switch (handlingStatus)
      {
      case EventHandlingStatus::Unhandled:
        return MovementOwnership::Unhandled;
      case EventHandlingStatus::Handled:
        return MovementOwnership::Handled;
      case EventHandlingStatus::Claimed:
        return MovementOwnership::HandledAndControlled;
      }
      FSLLOG3_WARNING("unknown handling status");
      return MovementOwnership::HandledAndControlled;
    }


    PxSize2D ExtractCursorSize(const ScrollModeFlags scrollModeFlags, const ContentSpriteMesh& cursorX, const ContentSpriteMesh& cursorY)
    {
      PxSize1D widthPx;
      PxSize1D heightPx;
      if (ScrollModeFlagsUtil::IsEnabled(scrollModeFlags, ScrollModeFlags::TranslateX) && cursorX.IsValid())
      {
        heightPx = cursorX.GetRenderSizePx().Height();
      }
      if (ScrollModeFlagsUtil::IsEnabled(scrollModeFlags, ScrollModeFlags::TranslateY) && cursorY.IsValid())
      {
        widthPx = cursorY.GetRenderSizePx().Width();
      }
      return {widthPx, heightPx};
    }
  }


  ScrollViewer::ScrollViewer(const std::shared_ptr<BaseWindowContext>& context)
    : ContentControl(context)
    , m_gestureHandler(context->UnitConverter.GetDensityDpi())
    , m_cursorX(context->TheUIContext.Get()->MeshManager)
    , m_cursorY(context->TheUIContext.Get()->MeshManager)
    , m_propertyDragFlickDeceleration(LocalConfig::DefaultDragFlickDeceleration)
    , m_propertyDragFlickTransitionType(LocalConfig::DefaultDragFlickTransitionType)
    , m_propertyDragEndAnimTimeMultiplier(LocalConfig::DefaultDragEndAnimTimeMultiplier)
    , m_propertyBounceSpringStiffness(LocalConfig::DefaultSpringStiffness)
    , m_propertyBounceAnimationTime(LocalConfig::DefaultBounceTime)
    , m_propertyBounceTransitionType(LocalConfig::DefaultBounceTransitionType)
    , m_propertyClipContent(LocalConfig::DefaultClipContent)
    , m_propertyCursorColor(context->ColorConverter, LocalConfig::DefaultCursorColor)
  {
    SetAlignmentX(ItemAlignment::Stretch);
    SetAlignmentY(ItemAlignment::Stretch);
    SetScrollMode(ScrollModeFlags::Translate);

    Enable(WindowFlags(WindowFlags::ClickInput | WindowFlags::DrawEnabled));
  }


  void ScrollViewer::WinResolutionChanged(const ResolutionChangedInfo& info)
  {
    base_type::WinResolutionChanged(info);

    m_gestureHandler.ConfigurationChanged(info.DensityDpi);
  }


  ScrollModeFlags ScrollViewer::GetScrollMode() const noexcept
  {
    return m_propertyScrollMode.Get();
  }


  bool ScrollViewer::SetScrollMode(const ScrollModeFlags value) noexcept
  {
    const bool changed = m_propertyScrollMode.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_gestureHandler.SetScrollMode(value);
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  float ScrollViewer::GetDragFlickDeceleration() const noexcept
  {
    return m_propertyDragFlickDeceleration.Get();
  }


  bool ScrollViewer::SetDragFlickDeceleration(const float value) noexcept
  {
    const bool changed = m_propertyDragFlickDeceleration.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  TransitionType ScrollViewer::GetDragFlickTransitionType() const noexcept
  {
    return m_propertyDragFlickTransitionType.Get();
  }


  bool ScrollViewer::SetDragFlickTransitionType(const TransitionType value) noexcept
  {
    const bool changed = m_propertyDragFlickTransitionType.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  float ScrollViewer::GetDragEndAnimTimeMultiplier() const noexcept
  {
    return m_propertyDragEndAnimTimeMultiplier.Get();
  }


  bool ScrollViewer::SetDragEndAnimTimeMultiplier(const float value) noexcept
  {
    const bool changed = m_propertyDragEndAnimTimeMultiplier.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  float ScrollViewer::GetBounceSpringStiffness() const noexcept
  {
    return m_propertyBounceSpringStiffness.Get();
  }


  bool ScrollViewer::SetBounceSpringStiffness(const float value) noexcept
  {
    const bool changed = m_propertyBounceSpringStiffness.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  TimeSpan ScrollViewer::GetBounceAnimationTime() const noexcept
  {
    return m_propertyBounceAnimationTime.Get();
  }


  bool ScrollViewer::SetBounceAnimationTime(const TimeSpan value) noexcept
  {
    const bool changed = m_propertyBounceAnimationTime.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  TransitionType ScrollViewer::GetBounceTransitionType() const noexcept
  {
    return m_propertyBounceTransitionType.Get();
  }


  bool ScrollViewer::SetBounceTransitionType(const TransitionType value) noexcept
  {
    const bool changed = m_propertyBounceTransitionType.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  UIColor ScrollViewer::GetCursorColor() const noexcept
  {
    return m_propertyCursorColor.Get();
  }


  bool ScrollViewer::SetCursorColor(const UIColor value) noexcept
  {
    const bool changed = m_propertyCursorColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ScrollViewer::GetClipContent() const noexcept
  {
    return m_propertyClipContent.Get();
  }


  bool ScrollViewer::SetClipContent(const bool value) noexcept
  {
    const bool changed = m_propertyClipContent.Set(ThisDependencyObject(), value);
    if (changed)
    {
      Set(WindowFlags::ClipEnabled, value);
      PropertyUpdated(PropertyType::ContentDraw);
    }
    return changed;
  }


  bool ScrollViewer::SetCursorX(const std::shared_ptr<IContentSprite>& value)
  {
    const bool changed = value != m_cursorX.GetSprite();
    if (changed)
    {
      m_cursorX.SetSprite(value);
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ScrollViewer::SetCursorX(std::shared_ptr<IContentSprite>&& value)
  {
    const bool changed = value != m_cursorX.GetSprite();
    if (changed)
    {
      m_cursorX.SetSprite(std::move(value));
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ScrollViewer::SetCursorY(const std::shared_ptr<IContentSprite>& value)
  {
    const bool changed = value != m_cursorY.GetSprite();
    if (changed)
    {
      m_cursorY.SetSprite(value);
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ScrollViewer::SetCursorY(std::shared_ptr<IContentSprite>&& value)
  {
    const bool changed = value != m_cursorY.GetSprite();
    if (changed)
    {
      m_cursorY.SetSprite(std::move(value));
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  void ScrollViewer::WinDraw(const UIDrawContext& context)
  {
    base_type::WinDraw(context);

    const auto finalBaseColor = GetFinalBaseColor();

    {
      auto renderCursorInfo = GetRenderCursorInfoX();
      if (m_cursorX.IsValid() && renderCursorInfo.Enabled)
      {
        const auto positionPxf = context.TargetRect.Location() + TypeConverter::To<PxVector2>(renderCursorInfo.OffsetPx);
        const auto finalCursorColor = (finalBaseColor * renderCursorInfo.Color);
        context.CommandBuffer.Draw(m_cursorX.Get(), positionPxf, renderCursorInfo.SizePx, finalCursorColor, context.ClipContext);
      }
    }
    {
      auto renderCursorInfo = GetRenderCursorInfoY();
      if (m_cursorY.IsValid() && renderCursorInfo.Enabled)
      {
        const auto positionPxf = context.TargetRect.Location() + TypeConverter::To<PxVector2>(renderCursorInfo.OffsetPx);
        const auto finalCursorColor = (finalBaseColor * renderCursorInfo.Color);
        context.CommandBuffer.Draw(m_cursorY.Get(), positionPxf, renderCursorInfo.SizePx, finalCursorColor, context.ClipContext);
      }
    }
  }


  void ScrollViewer::OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    base_type::OnClickInput(theEvent);

    const auto localPositionPx = PointFromScreen(theEvent->GetScreenPosition());

    const MovementOwnership movementOwnerShip = ToMovementOwnership(theEvent->GetHandlingStatus());
    const MovementTransactionAction result =
      m_gestureHandler.AddMovement(theEvent->GetTimestamp(), localPositionPx, theEvent->GetState(), theEvent->IsRepeat(), movementOwnerShip);

    switch (result)
    {
    case MovementTransactionAction::NotInterested:
      FSLLOG3_VERBOSE3("NotInterested: {}", movementOwnerShip);
      break;
    case MovementTransactionAction::Evaluate:
      FSLLOG3_VERBOSE3("Evaluate: {}", movementOwnerShip);
      break;
    case MovementTransactionAction::Handle:
      // Flagging a already handled event as handled is fine
      theEvent->Handled();
      FSLLOG3_VERBOSE3("Handle: {}", movementOwnerShip);
      break;
    case MovementTransactionAction::Control:
      // Flagging a already handled event as handled is fine
      theEvent->Claimed();
      FSLLOG3_VERBOSE3("Control: {}", movementOwnerShip);
      break;
    case MovementTransactionAction::InterceptAndControl:
      // The intercept clears the handled state, so claim it right away
      theEvent->Intercept();
      theEvent->Claimed();
      FSLLOG3_VERBOSE3("Intercept: {}", movementOwnerShip);
      break;
    }
  }


  void ScrollViewer::UpdateAnimation(const TimeSpan& timeSpan)
  {
    base_type::UpdateAnimation(timeSpan);

    const ScrollGestureAnimationConfig animConfig(ToDpPerSecond(GetDragFlickDeceleration()), GetDragFlickTransitionType(),
                                                  GetDragEndAnimTimeMultiplier(), Vector2(GetBounceSpringStiffness(), GetBounceSpringStiffness()),
                                                  GetBounceAnimationTime(), GetBounceTransitionType());
    if (m_gestureHandler.UpdateAnimation(timeSpan, animConfig))
    {
      PropertyUpdated(PropertyType::Layout);
    }
  }


  bool ScrollViewer::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    return m_gestureHandler.UpdateAnimationState(forceCompleteAnimation);
  }

  PxSize2D ScrollViewer::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    const auto cursorSizePx = ExtractCursorSize(GetScrollMode(), m_cursorX, m_cursorY);

    PxAvailableSize localAvailableSizePx = PxAvailableSize::Subtract(availableSizePx, cursorSizePx);
    switch (GetScrollMode())
    {
    case ScrollModeFlags::Translate:
      localAvailableSizePx = PxAvailableSize(PxAvailableSize1D::InfiniteSpacePx(), PxAvailableSize1D::InfiniteSpacePx());
      break;
    case ScrollModeFlags::TranslateX:
      localAvailableSizePx = PxAvailableSize(PxAvailableSize1D::InfiniteSpacePx(), localAvailableSizePx.Height());
      break;
    case ScrollModeFlags::TranslateY:
      localAvailableSizePx = PxAvailableSize(localAvailableSizePx.Width(), PxAvailableSize1D::InfiniteSpacePx());
      break;
    case ScrollModeFlags::NotDefined:
    default:
      // localAvailableSizePx = localAvailableSizePx;
      break;
    }
    const auto desiredSizePx = base_type::MeasureOverride(localAvailableSizePx) + cursorSizePx;
    return PxAvailableSize::MinPxSize2D(desiredSizePx, availableSizePx);
  }


  PxSize2D ScrollViewer::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    const PxSize2D cursorSizePx = ExtractCursorSize(GetScrollMode(), m_cursorX, m_cursorY);
    const PxSize2D localFinalSizePx(finalSizePx - cursorSizePx);

    PxSize2D desiredlayoutSizePx = GetContentDesiredSizePx();
    switch (GetScrollMode())
    {
    case ScrollModeFlags::Translate:
      break;
    case ScrollModeFlags::TranslateX:
      desiredlayoutSizePx.SetHeight(localFinalSizePx.Height());
      break;
    case ScrollModeFlags::TranslateY:
      desiredlayoutSizePx.SetWidth(localFinalSizePx.Width());
      break;
    case ScrollModeFlags::NotDefined:
    default:
      desiredlayoutSizePx = localFinalSizePx;
      break;
    }

    const PxPoint2 positionOffsetPx = m_gestureHandler.Arrange(localFinalSizePx, desiredlayoutSizePx);
    m_scrollPositionOffsetPx = positionOffsetPx;
    base_type::CustomArrange(desiredlayoutSizePx, positionOffsetPx);
    return finalSizePx;
  }


  ScrollViewer::RenderCursorInfo ScrollViewer::GetRenderCursorInfoX() const noexcept
  {
    const auto& content = GetContent();
    if (!m_cursorX.IsValid() || !content || !m_gestureHandler.IsScrollingEnabled() ||
        !ScrollModeFlagsUtil::IsEnabled(m_propertyScrollMode.Get(), ScrollModeFlags::TranslateX))
    {
      return {};
    }

    const PxSize2D viewRenderSizePx = RenderSizePx();
    const PxSize2D cursorRenderSizePx = m_cursorX.GetRenderSizePx();
    const PxSize2D contentRenderSizePx = content->RenderSizePx();

    const PxPoint2 offsetAndSize =
      CalcCursorOffsetAndSize(cursorRenderSizePx.Width(), viewRenderSizePx.Width(), contentRenderSizePx.Width(), m_scrollPositionOffsetPx.X);
    if (offsetAndSize.Y <= 0)
    {
      return {};
    }

    const auto offsetY = (viewRenderSizePx.Height() - cursorRenderSizePx.Height());
    return {true, PxPoint2(offsetAndSize.X, offsetY), PxSize2D(offsetAndSize.Y, cursorRenderSizePx.Height()), m_propertyCursorColor.InternalColor};
  }


  ScrollViewer::RenderCursorInfo ScrollViewer::GetRenderCursorInfoY() const noexcept
  {
    const auto& content = GetContent();
    if (!m_cursorY.IsValid() || !content || !m_gestureHandler.IsScrollingEnabled() ||
        !ScrollModeFlagsUtil::IsEnabled(m_propertyScrollMode.Get(), ScrollModeFlags::TranslateY))
    {
      return {};
    }

    const PxSize2D viewRenderSizePx = RenderSizePx();
    const PxSize2D cursorRenderSizePx = m_cursorY.GetRenderSizePx();
    const PxSize2D contentRenderSizePx = content->RenderSizePx();

    const PxPoint2 offsetAndSize =
      CalcCursorOffsetAndSize(cursorRenderSizePx.Height(), viewRenderSizePx.Height(), contentRenderSizePx.Height(), m_scrollPositionOffsetPx.Y);
    if (offsetAndSize.Y <= 0)
    {
      return {};
    }

    const auto offsetX = (viewRenderSizePx.Width() - cursorRenderSizePx.Width());
    return {true, PxPoint2(offsetX, offsetAndSize.X), PxSize2D(cursorRenderSizePx.Width(), offsetAndSize.Y), m_propertyCursorColor.InternalColor};
  }


  DataBinding::DataBindingInstanceHandle ScrollViewer::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyScrollMode, m_propertyScrollMode),
      DataBinding::PropLinkRefs(PropertyDragFlickDeceleration, m_propertyDragFlickDeceleration),
      DataBinding::PropLinkRefs(PropertyDragFlickTransitionType, m_propertyDragFlickTransitionType),
      DataBinding::PropLinkRefs(PropertyDragEndAnimTimeMultiplier, m_propertyDragEndAnimTimeMultiplier),
      DataBinding::PropLinkRefs(PropertyBounceSpringStiffness, m_propertyBounceSpringStiffness),
      DataBinding::PropLinkRefs(PropertyBounceAnimationTime, m_propertyBounceAnimationTime),
      DataBinding::PropLinkRefs(PropertyBounceTransitionType, m_propertyBounceTransitionType),
      DataBinding::PropLinkRefs(PropertyClipContent, m_propertyClipContent),
      DataBinding::PropLinkRefs(PropertyCursorColor, m_propertyCursorColor.ExternalColor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult ScrollViewer::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                       const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, DataBinding::PropLinkRefs(PropertyScrollMode, m_propertyScrollMode),
      DataBinding::PropLinkRefs(PropertyDragFlickDeceleration, m_propertyDragFlickDeceleration),
      DataBinding::PropLinkRefs(PropertyDragFlickTransitionType, m_propertyDragFlickTransitionType),
      DataBinding::PropLinkRefs(PropertyDragEndAnimTimeMultiplier, m_propertyDragEndAnimTimeMultiplier),
      DataBinding::PropLinkRefs(PropertyBounceSpringStiffness, m_propertyBounceSpringStiffness),
      DataBinding::PropLinkRefs(PropertyBounceAnimationTime, m_propertyBounceAnimationTime),
      DataBinding::PropLinkRefs(PropertyBounceTransitionType, m_propertyBounceTransitionType),
      DataBinding::PropLinkRefs(PropertyClipContent, m_propertyClipContent),
      DataBinding::PropLinkRefs(PropertyCursorColor, m_propertyCursorColor.ExternalColor));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void ScrollViewer::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyScrollMode);
    rProperties.push_back(PropertyDragFlickDeceleration);
    rProperties.push_back(PropertyDragFlickTransitionType);
    rProperties.push_back(PropertyDragEndAnimTimeMultiplier);
    rProperties.push_back(PropertyBounceSpringStiffness);
    rProperties.push_back(PropertyBounceAnimationTime);
    rProperties.push_back(PropertyBounceTransitionType);
    rProperties.push_back(PropertyClipContent);
    rProperties.push_back(PropertyCursorColor);
  }
}
