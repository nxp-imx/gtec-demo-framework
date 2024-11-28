#ifndef SHARED_ANTIALIASING_CUSTOMCONTROLS_ZOOMAREA_HPP
#define SHARED_ANTIALIASING_CUSTOMCONTROLS_ZOOMAREA_HPP
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

#include <FslBase/Transition/TransitionValue.hpp>
#include <FslGraphics/Render/Basic/BasicPrimitiveTopology.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteBasicMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Base/Transition/TransitionUIRenderColor.hpp>
#include <Shared/AntiAliasing/CustomControls/ZoomInfo.hpp>

namespace Fsl::UI::Custom
{
  class ZoomArea final : public BaseWindow
  {
    using base_type = BaseWindow;

    CustomBasicSpriteBasicMesh m_content;
    DependencyPropertyUIColor m_propertyContentColor;
    DataBinding::TypedDependencyProperty<DpSize1DF> m_propertyAreaWidth;
    DataBinding::TypedDependencyProperty<DpSize1DF> m_propertyAreaHeight;
    DataBinding::TypedDependencyProperty<float> m_propertyZoomFactor;
    PxSize2D m_cachedAreaSizePx;
    bool m_isDown{false};
    PxPoint2 m_cachedMousePositionPx;
    PxPoint2 m_cachedClampedMousePositionPx;
    PxPoint2 m_cachedClampedOffsetPositionPx;
    TransitionValue m_zoomFactorAnim;
    TransitionUIRenderColor m_zoomBorderColor;

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyContentColor;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyAreaWidth;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyAreaHeight;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyZoomFactor;

    explicit ZoomArea(const std::shared_ptr<BaseWindowContext>& context);
    ~ZoomArea() final;

    const std::shared_ptr<BasicImageSprite>& GetContent() const
    {
      return m_content.GetSprite();
    }

    void SetContent(const std::shared_ptr<BasicImageSprite>& value);
    void SetContent(std::shared_ptr<BasicImageSprite>&& value);

    UIColor GetContentColor() const noexcept
    {
      return m_propertyContentColor.Get();
    }

    bool SetContentColor(const UIColor value);

    DpSize1DF GetAreaWidth() const noexcept
    {
      return m_propertyAreaWidth.Get();
    }

    bool SetAreaWidth(const DpSize1DF value);

    DpSize1DF GetAreaHeight() const noexcept
    {
      return m_propertyAreaHeight.Get();
    }

    bool SetAreaHeight(const DpSize1DF value);

    float GetZoomFactor() const noexcept
    {
      return m_propertyZoomFactor.Get();
    }

    bool SetZoomFactor(const float value);

    bool IsZoomEnabled() const;

    //! @brief Get the resolved area in window coordinates
    PxRectangle GetResolvedArea() const;

    //! @brief Get the resolved area in screen space coordinates
    PxRectangle GetResolvedAreaInScreenSpace() const;

    ZoomInfo GetResolvedZoomInfo() const;

    void WinResolve(const TimeSpan& timespan) override;
    void WinDraw(const UIDrawContext& context) override;

  protected:
    void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) override;

    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

    void UpdateAnimation(const TimeSpan& timeSpan) override;
    bool UpdateAnimationState(const bool forceCompleteAnimation) override;

    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override;
    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
  };
}

#endif
