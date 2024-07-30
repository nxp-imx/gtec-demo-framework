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

#include <FslBase/Log/Log3Core.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Control/ScrollGestureAnimationConfig.hpp>
#include <FslSimpleUI/Base/Control/ScrollViewer.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/ResolutionChangedInfo.hpp>

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
    }
  }

  constexpr DpValueF ToDpPerSecond(const float value) noexcept
  {
    return DpValueF(value * 100);
  }


  ScrollViewer::ScrollViewer(const std::shared_ptr<BaseWindowContext>& context)
    : ContentControl(context)
    , m_gestureHandler(context->UnitConverter.GetDensityDpi())
    , m_propertyDragFlickDeceleration(LocalConfig::DefaultDragFlickDeceleration)
    , m_propertyDragFlickTransitionType(LocalConfig::DefaultDragFlickTransitionType)
    , m_propertyDragEndAnimTimeMultiplier(LocalConfig::DefaultDragEndAnimTimeMultiplier)
    , m_propertyBounceSpringStiffness(LocalConfig::DefaultSpringStiffness)
    , m_propertyBounceAnimationTime(LocalConfig::DefaultBounceTime)
    , m_propertyBounceTransitionType(LocalConfig::DefaultBounceTransitionType)
  {
    SetAlignmentX(ItemAlignment::Stretch);
    SetAlignmentY(ItemAlignment::Stretch);
    SetScrollMode(ScrollModeFlags::Translate);

    Enable(WindowFlags(WindowFlags::ClickInput));
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


  void ScrollViewer::OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    base_type::OnClickInput(theEvent);

    if (theEvent->IsHandled())
    {
      return;
    }
    const auto localPositionPx = PointFromScreen(theEvent->GetScreenPosition());

    if (theEvent->IsBegin())
    {
      m_isDown = true;
      m_gestureHandler.AddMovement(theEvent->GetTimestamp(), localPositionPx, true);
      theEvent->Handled();
    }
    else if (m_isDown)
    {
      m_gestureHandler.AddMovement(theEvent->GetTimestamp(), localPositionPx, false);
      m_isDown = false;
      theEvent->Handled();
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
    PxAvailableSize fakeAvailableSizePx;
    switch (GetScrollMode())
    {
    case ScrollModeFlags::Translate:
      fakeAvailableSizePx = PxAvailableSize(PxAvailableSize1D::InfiniteSpacePx(), PxAvailableSize1D::InfiniteSpacePx());
      break;
    case ScrollModeFlags::TranslateX:
      fakeAvailableSizePx = PxAvailableSize(PxAvailableSize1D::InfiniteSpacePx(), availableSizePx.Height());
      break;
    case ScrollModeFlags::TranslateY:
      fakeAvailableSizePx = PxAvailableSize(availableSizePx.Width(), PxAvailableSize1D::InfiniteSpacePx());
      break;
    case ScrollModeFlags::NotDefined:
    default:
      fakeAvailableSizePx = availableSizePx;
      break;
    }
    const auto desiredSizePx = base_type::MeasureOverride(fakeAvailableSizePx);
    return PxAvailableSize::MinPxSize2D(desiredSizePx, availableSizePx);
  }


  PxSize2D ScrollViewer::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    PxSize2D desiredlayoutSizePx = GetContentDesiredSizePx();
    switch (GetScrollMode())
    {
    case ScrollModeFlags::Translate:
      break;
    case ScrollModeFlags::TranslateX:
      desiredlayoutSizePx.SetHeight(finalSizePx.Height());
      break;
    case ScrollModeFlags::TranslateY:
      desiredlayoutSizePx.SetWidth(finalSizePx.Width());
      break;
    case ScrollModeFlags::NotDefined:
    default:
      desiredlayoutSizePx = finalSizePx;
      break;
    }

    const PxPoint2 positionOffsetPx = m_gestureHandler.Arrange(finalSizePx, desiredlayoutSizePx);
    base_type::CustomArrange(desiredlayoutSizePx, positionOffsetPx);
    return finalSizePx;
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
      DataBinding::PropLinkRefs(PropertyBounceTransitionType, m_propertyBounceTransitionType));
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
      DataBinding::PropLinkRefs(PropertyBounceTransitionType, m_propertyBounceTransitionType));
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
  }
}
