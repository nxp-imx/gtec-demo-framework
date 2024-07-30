#ifndef SHARED_UI_GESTURES_MOVEABLERECTANGLES_HPP
#define SHARED_UI_GESTURES_MOVEABLERECTANGLES_HPP
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

#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Gesture/GestureManager.hpp>
#include <FslSimpleUI/Base/Mesh/SizedSpriteMesh.hpp>
#include <Shared/UI/Gestures/MoveableRectangleManager.hpp>
#include <array>
#include <memory>
#include <vector>

namespace Fsl::UI
{
  class MoveableRectangles final : public BaseWindow
  {
    using base_type = BaseWindow;

    Fsl::UI::GestureManager m_gestureManager;

    MoveableRectangleManager m_moveableRectangleManager;

    bool m_isDown{false};

    std::array<SizedSpriteMesh, 3> m_meshes;

    DataBinding::TypedDependencyProperty<float> m_propertyDragFlickDeceleration;
    DataBinding::TypedDependencyProperty<float> m_propertyDragEndAnimTimeMod;

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyDragFlickDeceleration;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyDragEndAnimTimeMod;


    explicit MoveableRectangles(const std::shared_ptr<BaseWindowContext>& context);

    void SetFillSprite(const std::shared_ptr<ISizedSprite>& value);

    void WinResolutionChanged(const ResolutionChangedInfo& info) final;

    void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) final;

    void WinDraw(const UIDrawContext& context) final;

    void SetToDefaults();

    float GetDragFlickDeceleration() const noexcept;
    bool SetDragFlickDeceleration(const float value) noexcept;

    float GetDragEndAnimTimeMod() const noexcept;
    bool SetDragEndAnimTimeMod(const float value) noexcept;

  protected:
    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;

    void UpdateAnimation(const TimeSpan& timeSpan) final;
    bool UpdateAnimationState(const bool forceCompleteAnimation) final;

    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) final;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) final;
    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) final;
  };
}


#endif
