#ifndef SHARED_UI_RENDERTEST_CUSTOMCONTROLS_EASINGFUNCTIONGRAPH_HPP
#define SHARED_UI_RENDERTEST_CUSTOMCONTROLS_EASINGFUNCTIONGRAPH_HPP
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

#include <FslBase/Transition/EasingFunctionUtil.hpp>
#include <FslBase/Transition/TransitionType.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Render/Basic/BasicPrimitiveTopology.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteBasicMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Render/Base/ICustomDrawData.hpp>

namespace Fsl::UI::Custom
{
  class EasingFunctionGraphInfo final : public ICustomDrawData
  {
    TransitionType m_transitionType{TransitionType::Linear};
    EasingFunctionUtil::FNEasingFunction m_fnEasingFunction{};

  public:
    EasingFunctionGraphInfo()
    {
      m_fnEasingFunction = EasingFunctionUtil::GetEasingFunction(m_transitionType);
    }

    TransitionType GetTransitionType() const noexcept
    {
      return m_transitionType;
    }


    EasingFunctionUtil::FNEasingFunction TryGet() const noexcept
    {
      return m_fnEasingFunction;
    }

    void SetTransitionType(const TransitionType value) noexcept
    {
      m_transitionType = value;
      m_fnEasingFunction = EasingFunctionUtil::GetEasingFunction(m_transitionType);
    }
  };


  class EasingFunctionGraph final : public BaseWindow
  {
    using base_type = BaseWindow;

    std::shared_ptr<EasingFunctionGraphInfo> m_drawInfo;

    CustomBasicSpriteBasicMesh m_content;
    BasicPrimitiveTopology m_contentPrimitiveTopology{BasicPrimitiveTopology::TriangleList};


    DependencyPropertyUIColor m_propertyContentColor;
    DataBinding::TypedDependencyProperty<TransitionType> m_propertyTransitionType{TransitionType::Linear};

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyContentColor;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyTransitionType;

    explicit EasingFunctionGraph(const std::shared_ptr<BaseWindowContext>& context);
    ~EasingFunctionGraph() final;

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

    TransitionType GetTransitionType() const noexcept
    {
      return m_propertyTransitionType.Get();
    }

    bool SetTransitionType(const TransitionType value);

    void WinDraw(const UIDrawContext& context) override;

  protected:
    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override;
    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
  };
}

#endif
