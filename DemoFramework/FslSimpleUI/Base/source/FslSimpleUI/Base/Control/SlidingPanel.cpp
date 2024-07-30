/****************************************************************************************************************************************************
 * Copyright 2021-2023 NXP
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
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Control/SlidingPanel.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>

namespace Fsl::UI
{
  using TClass = SlidingPanel;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyDirection = TFactory::Create<SlideDirection, TClass, &TClass::GetDirection, &TClass::SetDirection>("Direction");
  TDef TClass::PropertyShown = TFactory::Create<bool, TClass, &TClass::IsShown, &TClass::SetShown>("Shown");
}


namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr float Visible = 1.0f;
      constexpr float Hidden = 0.0f;
    }

    PxPoint2 CalcOffset(const SlideDirection direction, const float visible, const PxSize2D sizePx)
    {
      const auto visiblePxf = PxSize1DF::Create(1.0f - visible);
      switch (direction)
      {
      case SlideDirection::Left:
        return {-TypeConverter::UncheckedChangeTo<PxValue>(TypeConverter::UncheckedTo<PxSize1DF>(sizePx.Width()) * visiblePxf), PxValue()};
      case SlideDirection::Up:
        return {PxValue(), -TypeConverter::UncheckedChangeTo<PxValue>(TypeConverter::UncheckedTo<PxSize1DF>(sizePx.Height()) * visiblePxf)};
      case SlideDirection::Right:
      case SlideDirection::Down:
      default:
        return {};
      }
    }

    PxSize2D CalcSize(const SlideDirection direction, const float visible, const PxSize2D sizePx)
    {
      if (visible <= 0.001f)
      {
        return {};
      }
      if (visible >= 0.999f)
      {
        return sizePx;
      }

      const auto visiblePxf = PxSize1DF::Create(visible);
      switch (direction)
      {
      case SlideDirection::Left:
      case SlideDirection::Right:
        return {TypeConverter::UncheckedChangeTo<PxSize1D>(TypeConverter::UncheckedTo<PxSize1DF>(sizePx.Width()) * visiblePxf), sizePx.Height()};
      case SlideDirection::Up:
      case SlideDirection::Down:
      default:
        return {sizePx.Width(), TypeConverter::UncheckedChangeTo<PxSize1D>(TypeConverter::UncheckedTo<PxSize1DF>(sizePx.Height()) * visiblePxf)};
      }
    }
  }


  SlidingPanel::SlidingPanel(const std::shared_ptr<BaseWindowContext>& context)
    : ContentControl(context)
    , m_animation(DefaultAnim::SlideTime, DefaultAnim::SlideTransitionType)
  {
    m_animation.SetActualValue(LocalConfig::Visible);
  }

  bool SlidingPanel::SetShown(const bool value)
  {
    const bool changed = m_propertyShown.Set(ThisDependencyObject(), value);
    if (changed)
    {
      if (value)
      {
        SetContentVisibility(ItemVisibility::Visible);
      }
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  bool SlidingPanel::SetDirection(const SlideDirection value)
  {
    const bool changed = m_propertyDirection.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  PxSize2D SlidingPanel::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    auto localFinalSizePx = m_desiredSizePx;
    switch (m_propertyDirection.Get())
    {
    case SlideDirection::Left:
    case SlideDirection::Right:
      localFinalSizePx.SetHeight(finalSizePx.Height());
      break;
    case SlideDirection::Up:
    case SlideDirection::Down:
      localFinalSizePx.SetWidth(finalSizePx.Width());
      break;
    default:
      break;
    }

    PxPoint2 offsetPx = CalcOffset(m_propertyDirection.Get(), m_animation.GetValue(), localFinalSizePx);
    ContentControl::CustomArrange(localFinalSizePx, offsetPx);
    return finalSizePx;
  }


  PxSize2D SlidingPanel::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    m_desiredSizePx = ContentControl::MeasureOverride(availableSizePx);
    return CalcSize(m_propertyDirection.Get(), m_animation.GetValue(), m_desiredSizePx);
  }


  DataBinding::DataBindingInstanceHandle SlidingPanel::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                                         DataBinding::PropLinkRefs(PropertyDirection, m_propertyDirection),
                                                                         DataBinding::PropLinkRefs(PropertyShown, m_propertyShown));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult SlidingPanel::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                       const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                                  DataBinding::PropLinkRefs(PropertyDirection, m_propertyDirection),
                                                                  DataBinding::PropLinkRefs(PropertyShown, m_propertyShown));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void SlidingPanel::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyDirection);
    rProperties.push_back(PropertyShown);
  }

  void SlidingPanel::UpdateAnimation(const TimeSpan& timeSpan)
  {
    float oldValue = m_animation.GetValue();
    m_animation.Update(timeSpan);
    if (oldValue != m_animation.GetValue())
    {
      SetLayoutDirty(true);
    }
  }


  bool SlidingPanel::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    const auto shown = m_propertyShown.Get();
    m_animation.SetValue(shown ? LocalConfig::Visible : LocalConfig::Hidden);

    bool isAnimating = ContentControl::UpdateAnimationState(forceCompleteAnimation);
    if (forceCompleteAnimation)
    {
      m_animation.ForceComplete();
    }
    bool isAnimating2 = !m_animation.IsCompleted();
    if (!isAnimating2 && !shown)
    {
      SetContentVisibility(ItemVisibility::Collapsed);
    }
    return isAnimating || isAnimating2;
  }
}
