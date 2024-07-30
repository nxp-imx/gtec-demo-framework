#ifndef FSLSIMPLEUI_BASE_CONTROL_SCROLLVIEWER_HPP
#define FSLSIMPLEUI_BASE_CONTROL_SCROLLVIEWER_HPP
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

#include <FslBase/Time/TimeSpan.hpp>
#include <FslSimpleUI/Base/Control/ContentControl.hpp>
#include <FslSimpleUI/Base/Control/ScrollGestureHandler.hpp>
#include <FslSimpleUI/Base/Control/ScrollModeFlags.hpp>

namespace Fsl::UI
{
  class ScrollViewer final : public ContentControl
  {
    using base_type = ContentControl;

    ScrollGestureHandler m_gestureHandler;
    bool m_isDown{false};

    DataBinding::TypedDependencyProperty<ScrollModeFlags> m_propertyScrollMode;
    DataBinding::TypedDependencyProperty<float> m_propertyDragFlickDeceleration;
    DataBinding::TypedDependencyProperty<TransitionType> m_propertyDragFlickTransitionType;
    DataBinding::TypedDependencyProperty<float> m_propertyDragEndAnimTimeMultiplier;
    DataBinding::TypedDependencyProperty<float> m_propertyBounceSpringStiffness;
    DataBinding::TypedDependencyProperty<TimeSpan> m_propertyBounceAnimationTime;
    DataBinding::TypedDependencyProperty<TransitionType> m_propertyBounceTransitionType;

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyScrollMode;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyDragFlickDeceleration;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyDragFlickTransitionType;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyDragEndAnimTimeMultiplier;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyBounceSpringStiffness;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyBounceAnimationTime;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyBounceTransitionType;


    explicit ScrollViewer(const std::shared_ptr<BaseWindowContext>& context);


    void WinResolutionChanged(const ResolutionChangedInfo& info) final;

    ScrollModeFlags GetScrollMode() const noexcept;
    bool SetScrollMode(const ScrollModeFlags value) noexcept;

    float GetDragFlickDeceleration() const noexcept;
    bool SetDragFlickDeceleration(const float value) noexcept;

    TransitionType GetDragFlickTransitionType() const noexcept;
    bool SetDragFlickTransitionType(const TransitionType value) noexcept;

    float GetDragEndAnimTimeMultiplier() const noexcept;
    bool SetDragEndAnimTimeMultiplier(const float value) noexcept;

    float GetBounceSpringStiffness() const noexcept;
    bool SetBounceSpringStiffness(const float value) noexcept;

    TimeSpan GetBounceAnimationTime() const noexcept;
    bool SetBounceAnimationTime(const TimeSpan value) noexcept;

    TransitionType GetBounceTransitionType() const noexcept;
    bool SetBounceTransitionType(const TransitionType value) noexcept;

  protected:
    void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) final;

    void UpdateAnimation(const TimeSpan& timeSpan) final;
    bool UpdateAnimationState(const bool forceCompleteAnimation) final;
    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;
    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;

    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) final;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) final;
    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) final;
  };
}


#endif
