#ifndef GLES3_SPRINGBACKGROUND_SPRINGBACKGROUND_HPP
#define GLES3_SPRINGBACKGROUND_SPRINGBACKGROUND_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeBatch2D.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <deque>
#include "Bloom/BloomRender.hpp"
#include "Grid.hpp"
#include "GridRender/GridRenderNativeBatchBasic.hpp"
#include "GridRender/GridRenderNativeBatchCRSpline1.hpp"
#include "GridRender/GridRenderNativeBatchCRSpline2.hpp"
#include "GridRender/GridRenderNativeBatchCRSpline3.hpp"
#include "GridScene.hpp"

namespace Fsl
{
  class SpringBackground
    : public DemoAppGLES3
    , public UI::EventListener
  {
    struct RenderConfig
    {
      bool Bloom{true};
      bool Balls{true};

      RenderConfig() = default;
    };

    struct BallRecord
    {
      Vector2 Position;
      Vector2 OldPosition;
    };

    UI::CallbackEventListenerScope m_uiEventListener;
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    std::shared_ptr<GLES3::NativeBatch2D> m_batch;
    AtlasTexture2D m_texFill;
    AtlasTexture2D m_texBall;

    std::unique_ptr<GridScene> m_gridScene;


    bool m_isLeftButtonDown;
    PxPoint2 m_mousePosition;
    Vector3 m_oldMouse;


    std::shared_ptr<UI::Label> m_menuLabelRenderType;
    std::shared_ptr<UI::ImageButton> m_btnRenderTypePrev;
    std::shared_ptr<UI::ImageButton> m_btnRenderTypeNext;
    std::shared_ptr<UI::Switch> m_cbBloom;

    BloomRender m_bloomRender;
    RenderConfig m_config;

    std::vector<BallRecord> m_balls;
    bool m_explostionType;

  public:
    explicit SpringBackground(const DemoAppConfig& config);
    ~SpringBackground() override;

    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;

  protected:
    void FixedUpdate(const DemoTime& demoTime) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void BuildUI();
    void UpdateControls();
    void PrevGridRender();
    void NextGridRender();
  };
}

#endif
