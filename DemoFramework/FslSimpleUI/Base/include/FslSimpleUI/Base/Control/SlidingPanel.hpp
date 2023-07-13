#ifndef FSLSIMPLEUI_BASE_CONTROL_SLIDINGPANEL_HPP
#define FSLSIMPLEUI_BASE_CONTROL_SLIDINGPANEL_HPP
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

#include <FslBase/Transition/TransitionValue.hpp>
#include <FslSimpleUI/Base/Control/ContentControl.hpp>
#include <FslSimpleUI/Base/SlideDirection.hpp>

namespace Fsl::UI
{
  class SlidingPanel final : public ContentControl
  {
    using base_type = ContentControl;

    TransitionValue m_animation;
    DataBinding::TypedDependencyProperty<SlideDirection> m_propertyDirection{SlideDirection::Left};
    DataBinding::TypedDependencyProperty<bool> m_propertyShown{true};

    PxSize2D m_desiredSizePx;

  public:
    static DataBinding::DependencyPropertyDefinition PropertyDirection;
    static DataBinding::DependencyPropertyDefinition PropertyShown;

    explicit SlidingPanel(const std::shared_ptr<BaseWindowContext>& context);

    bool IsShown() const noexcept
    {
      return m_propertyShown.Get();
    }

    bool SetShown(const bool value);

    SlideDirection GetDirection() const noexcept
    {
      return m_propertyDirection.Get();
    }

    bool SetDirection(const SlideDirection value);

  protected:
    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override;
    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;

    void UpdateAnimation(const TimeSpan& timeSpan) final;
    bool UpdateAnimationState(const bool forceCompleteAnimation) final;
  };
}

#endif
