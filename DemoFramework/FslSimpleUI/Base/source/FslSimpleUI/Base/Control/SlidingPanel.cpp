/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Control/SlidingPanel.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>

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
      PxPoint2 offsetPx;
      switch (direction)
      {
      case SlideDirection::Left:
        offsetPx = PxPoint2(TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(-sizePx.Width()) * (1.0f - visible)), 0);
        break;
      case SlideDirection::Right:
        offsetPx = {};
        break;
      case SlideDirection::Up:
        offsetPx = PxPoint2(0, TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(-sizePx.Height()) * (1.0f - visible)));
        break;
      case SlideDirection::Down:
      default:
        offsetPx = {};
        break;
      }
      return offsetPx;
    }

    PxSize2D CalcSize(const SlideDirection direction, const float visible, const PxSize2D sizePx)
    {
      PxSize2D modifiedSizePx;
      switch (direction)
      {
      case SlideDirection::Left:
        modifiedSizePx = PxSize2D(TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(sizePx.Width()) * visible), sizePx.Height());
        break;
      case SlideDirection::Right:
        modifiedSizePx = PxSize2D(TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(sizePx.Width()) * visible), sizePx.Height());
        break;
      case SlideDirection::Up:
        modifiedSizePx = PxSize2D(sizePx.Width(), TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(sizePx.Height()) * visible));
        break;
      case SlideDirection::Down:
      default:
        modifiedSizePx = PxSize2D(sizePx.Width(), TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(sizePx.Height()) * visible));
        break;
      }
      return modifiedSizePx;
    }
  }


  SlidingPanel::SlidingPanel(const std::shared_ptr<BaseWindowContext>& context)
    : ContentControl(context)
    , m_animation(context->UITransitionCache, DefaultAnim::SlideTime, DefaultAnim::SlideTransitionType)
  {
    m_animation.SetActualValue(LocalConfig::Visible);
  }

  bool SlidingPanel::SetShow(const bool isVisible)
  {
    const auto state = isVisible ? State::Shown : State::Hidden;
    if (m_state == state)
    {
      return false;
    }
    m_state = state;
    if (m_state == State::Shown)
    {
      SetContentVisibility(ItemVisibility::Visible);
    }

    PropertyUpdated(PropertyType::Layout);
    return true;
  }


  bool SlidingPanel::SetDirection(const SlideDirection direction)
  {
    if (direction == m_slideDirection)
    {
      return false;
    }
    m_slideDirection = direction;
    PropertyUpdated(PropertyType::Layout);
    return true;
  }


  PxSize2D SlidingPanel::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    auto localFinalSizePx = m_desiredSizePx;
    switch (m_slideDirection)
    {
    case SlideDirection::Left:
    case SlideDirection::Right:
      localFinalSizePx.SetHeight(finalSizePx.Height());
      break;
    case SlideDirection::Up:
    case SlideDirection::Down:
      localFinalSizePx.SetWidth(finalSizePx.Width());
    default:
      break;
    }

    PxPoint2 offsetPx = CalcOffset(m_slideDirection, m_animation.GetValue(), localFinalSizePx);
    ContentControl::CustomArrange(localFinalSizePx, offsetPx);
    return finalSizePx;
  }


  PxSize2D SlidingPanel::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    m_desiredSizePx = ContentControl::MeasureOverride(availableSizePx);
    return CalcSize(m_slideDirection, m_animation.GetValue(), m_desiredSizePx);
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
    m_animation.SetValue(m_state == State::Shown ? LocalConfig::Visible : LocalConfig::Hidden);

    bool isAnimating = ContentControl::UpdateAnimationState(forceCompleteAnimation);
    if (forceCompleteAnimation)
    {
      m_animation.ForceComplete();
    }
    bool isAnimating2 = !m_animation.IsCompleted();
    if (!isAnimating2 && m_state == State::Hidden)
    {
      SetContentVisibility(ItemVisibility::Collapsed);
    }
    return isAnimating || isAnimating2;
  }
}
