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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include "SpringBackground.hpp"
#include "OptionParser.hpp"
#include <GLES3/gl3.h>
#include <cassert>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
  }


  // Geometry Wars had 60.000 points in its spring grid :)
  SpringBackground::SpringBackground(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))
    , m_batch(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_texFill(m_uiExtension->GetAtlasTexture2D("Fill"))
    , m_texBall(m_uiExtension->GetAtlasTexture2D("SliderCursor"))
    , m_gridScene(config, m_texFill, config.GetOptions<OptionParser>()->GetGridResolution())
    , m_isLeftButtonDown(false)
    , m_bloomRender(config)
    , m_balls(4)
    , m_explostionType(false)
  {
    RegisterExtension(m_uiExtension);

    auto options = config.GetOptions<OptionParser>();

    if (options->GetRenderId() >= 0)
    {
      m_gridScene.SetRenderId(options->GetRenderId());
    }

    m_config.Bloom = options->IsBloomEnabled();

    auto bloomConfig = m_bloomRender.GetConfig();
    bloomConfig.IsShowBuffersEnabled = options->IsShowBuffersEnabled();
    m_bloomRender.SetConfig(bloomConfig);


    m_balls[0].Position = Vector2(100, 100);
    m_balls[0].OldPosition = Vector2(103, 103);
    m_balls[1].Position = Vector2(520, 500);
    m_balls[1].OldPosition = Vector2(523, 504);
    m_balls[2].Position = Vector2(880, 600);
    m_balls[2].OldPosition = Vector2(875, 603);
    m_balls[3].Position = Vector2(1800, 900);
    m_balls[3].OldPosition = Vector2(1793, 896);
    BuildUI();
  }


  SpringBackground::~SpringBackground() = default;

  void SpringBackground::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::M:
      ToggleMenu();
      event.Handled();
      break;
    case VirtualKey::B:
      event.Handled();
      m_config.Bloom = !m_config.Bloom;
      UpdateControls();
      break;
    case VirtualKey::D:
    {
      event.Handled();
      auto bloomConfig = m_bloomRender.GetConfig();
      bloomConfig.IsShowBuffersEnabled = !bloomConfig.IsShowBuffersEnabled;
      m_bloomRender.SetConfig(bloomConfig);
      UpdateControls();
      break;
    }
    default:
      break;
    }
  }


  void SpringBackground::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_btnMenu)
    {
      theEvent->Handled();
      ToggleMenu();
    }
    else if (theEvent->GetSource() == m_btnRenderTypePrev)
    {
      PrevGridRender();
    }
    else if (theEvent->GetSource() == m_btnRenderTypeNext)
    {
      NextGridRender();
    }
  }


  void SpringBackground::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_cbBloom)
    {
      m_config.Bloom = m_cbBloom->IsChecked();
      UpdateControls();
    }
  }


  void SpringBackground::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Left:
      m_isLeftButtonDown = event.IsPressed();
      m_mousePosition = event.GetPosition();
      break;
    case VirtualMouseButton::Right:
      m_mousePosition = event.GetPosition();
      if (event.IsPressed())
      {
        Vector3 mousePos(static_cast<float>(m_mousePosition.X), static_cast<float>(m_mousePosition.Y), 0.0f);
        if (!m_explostionType)
        {
          m_gridScene.ApplyExplosiveForce(30, mousePos, 300);
        }
        else
        {
          m_gridScene.ApplyDirectedForce(Vector3(0, 0, 1000), mousePos, 300);
        }
        m_explostionType = !m_explostionType;
      }
      break;
    case VirtualMouseButton::Middle:
      m_mousePosition = event.GetPosition();
      if (event.IsPressed())
      {
        Vector3 mousePos(static_cast<float>(m_mousePosition.X), static_cast<float>(m_mousePosition.Y), 0.0f);
        m_gridScene.ApplyImplosiveForce(500, mousePos, 300);
      }
      break;
    default:
      break;
    }
  }


  void SpringBackground::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    m_mousePosition = event.GetPosition();
  }


  void SpringBackground::FixedUpdate(const DemoTime& demoTime)
  {
    if (m_isLeftButtonDown)
    {
      Vector3 mousePos(static_cast<float>(m_mousePosition.X), static_cast<float>(m_mousePosition.Y), 0.0f);
      Vector3 mousePosDeep(mousePos.X, mousePos.Y, 50);
      Vector3 delta = mousePos - m_oldMouse;

      // m_gridScene.ApplyExplosiveForce(30, mousePosDeep, 200);
      // m_gridScene.ApplyExplosiveForce(100, mousePos, 80);
      // m_gridScene.ApplyImplosiveForce(100, mousePos, 80);
      // m_gridScene.ApplyDirectedForce(Vector3(100, 0, 0), mousePos, 80);
      // m_gridScene.ApplyDirectedForce(delta, mousePos, 80);
      m_gridScene.ApplyDirectedForce(delta * 0.2f * 2, mousePos, 40);
      //      m_gridScene.ApplyDirectedForce(delta * 0.2f * 20, mousePos - delta, 40);
    }

    if (m_config.Balls)
    {
      auto screenRes = GetScreenResolution();
      const float boundaryLeft = 0;
      const auto boundaryRight = static_cast<float>(screenRes.X);
      const float boundaryTop = 0;
      const auto boundaryBottom = static_cast<float>(screenRes.Y);
      const float bounce = 1.0f;
      for (auto itr = m_balls.begin(); itr != m_balls.end(); ++itr)
      {
        Vector2 velocity = itr->Position - itr->OldPosition;
        itr->OldPosition = itr->Position;
        itr->Position += velocity;

        m_gridScene.ApplyDirectedForce(Vector3(velocity.X, velocity.Y, 0.0f) * 1.0f, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);

        // m_gridScene.ApplyDirectedForce(Vector3(0, 0, 100.0f), Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene.ApplyDirectedForce(Vector3(0, 0, -100.0f), Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene.ApplyImplosiveForce(-100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene.ApplyImplosiveForce(100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene.ApplyImplosiveForce(30, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene.ApplyImplosiveForce(10, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);

        // m_gridScene.ApplyExplosiveForce(100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene.ApplyExplosiveForce(-100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene.ApplyExplosiveForce(-30, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);


        if (itr->Position.X > boundaryRight)
        {
          itr->Position.X = boundaryRight;
          itr->OldPosition.X = itr->Position.X + (velocity.X * bounce);
        }
        else if (itr->Position.X < boundaryLeft)
        {
          itr->Position.X = boundaryLeft;
          itr->OldPosition.X = itr->Position.X + (velocity.X * bounce);
        }
        if (itr->Position.Y > boundaryBottom)
        {
          itr->Position.Y = boundaryBottom;
          itr->OldPosition.Y = itr->Position.Y + (velocity.Y * bounce);
        }
        else if (itr->Position.Y < boundaryTop)
        {
          itr->Position.Y = boundaryTop;
          itr->OldPosition.Y = itr->Position.Y + (velocity.Y * bounce);
        }
      }
    }

    m_gridScene.FixedUpdate(demoTime);
  }


  void SpringBackground::Update(const DemoTime& demoTime)
  {
    Vector3 mousePos(static_cast<float>(m_mousePosition.X), static_cast<float>(m_mousePosition.Y), 0.0f);
    m_oldMouse = mousePos;

    auto resolution = GetScreenResolution();
    Vector2 screenSize(static_cast<float>(resolution.X), static_cast<float>(resolution.Y));

    m_gridScene.Update(demoTime);
    m_bloomRender.Update(demoTime);
  }


  void SpringBackground::Draw(const DemoTime& demoTime)
  {
    if (m_config.Bloom)
    {
      m_bloomRender.Draw(m_gridScene);
    }
    else
    {
      m_gridScene.Draw();
    }


    if (m_config.Balls)
    {
      m_batch->Begin();

      const Color ballColor = Color::White();
      Vector2 scale(1, 1);
      Vector2 origin(m_texBall.GetSize().X * 0.5f, m_texBall.GetSize().Y * 0.5f);
      for (auto itr = m_balls.begin(); itr != m_balls.end(); ++itr)
      {
        m_batch->Draw(m_texBall, itr->Position, ballColor, origin, scale);
      }

      m_batch->End();
    }

    m_uiExtension->Draw();
  }


  void SpringBackground::BuildUI()
  {
    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();
    AtlasTexture2D texMenu = m_uiExtension->GetAtlasTexture2D("Player/Pause");

    m_btnMenu = std::make_shared<ImageButton>(context);
    m_btnMenu->SetContent(texMenu);
    m_btnMenu->SetAlignmentX(ItemAlignment::Near);
    m_btnMenu->SetAlignmentY(ItemAlignment::Far);

    m_mainMenuStack = std::make_shared<StackLayout>(context);
    m_mainMenuStack->SetLayoutOrientation(LayoutOrientation::Vertical);
    m_mainMenuStack->SetAlignmentX(ItemAlignment::Near);
    m_mainMenuStack->SetAlignmentY(ItemAlignment::Near);

    auto internalStack = std::make_shared<StackLayout>(context);
    internalStack->SetLayoutOrientation(LayoutOrientation::Vertical);
    internalStack->SetAlignmentX(ItemAlignment::Near);
    internalStack->SetAlignmentY(ItemAlignment::Far);
    internalStack->AddChild(m_mainMenuStack);
    internalStack->AddChild(m_btnMenu);

    m_rootLayout = std::make_shared<FillLayout>(context);
    m_rootLayout->AddChild(internalStack);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(m_rootLayout);
  }


  void SpringBackground::ToggleMenu()
  {
    if (m_layoutMenu)
    {
      DestroyMenuUI();
    }
    else
    {
      CreateMenuUI();
    }
  }


  void SpringBackground::CreateMenuUI()
  {
    if (m_layoutMenu)
    {
      return;
    }

    auto context = m_uiExtension->GetContext();
    AtlasTexture2D texCheckBoxC(m_uiExtension->GetAtlasTexture2D("CheckBoxC"));
    AtlasTexture2D texCheckBoxU(m_uiExtension->GetAtlasTexture2D("CheckBoxU"));
    // AtlasTexture2D texSlider(m_uiExtension->GetAtlasTexture2D("Slider"));
    // AtlasTexture2D texSliderCursor(m_uiExtension->GetAtlasTexture2D("SliderCursor"));
    // ThicknessF sliderCursorPadding(13, 0, 13, 0);
    // NineSlice sliderNineSlice(13, 0, 13, 0);
    AtlasTexture2D texBackSmall(m_uiExtension->GetAtlasTexture2D("Player/BackSmall"));
    AtlasTexture2D texNextSmall(m_uiExtension->GetAtlasTexture2D("Player/NextSmall"));


    // Create the outer stack for the menu
    m_layoutMenu = std::make_shared<StackLayout>(context);
    m_layoutMenu->SetLayoutOrientation(LayoutOrientation::Vertical);
    m_layoutMenu->SetAlignmentX(ItemAlignment::Near);
    m_layoutMenu->SetAlignmentY(ItemAlignment::Near);
    m_mainMenuStack->AddChild(m_layoutMenu);


    auto stack = std::make_shared<StackLayout>(context);
    stack->SetLayoutOrientation(LayoutOrientation::Horizontal);
    m_menuLabelRenderType = std::make_shared<Label>(context);
    m_menuLabelRenderType->SetAlignmentY(ItemAlignment::Center);

    m_btnRenderTypePrev = std::make_shared<ImageButton>(context);
    m_btnRenderTypePrev->SetContent(texBackSmall);
    m_btnRenderTypePrev->SetAlignmentY(ItemAlignment::Center);

    m_btnRenderTypeNext = std::make_shared<ImageButton>(context);
    m_btnRenderTypeNext->SetContent(texNextSmall);
    m_btnRenderTypeNext->SetAlignmentY(ItemAlignment::Center);

    m_cbBloom = std::make_shared<CheckBox>(context);
    m_cbBloom->SetText("Bloom");
    m_cbBloom->SetCheckedTexture(texCheckBoxC);
    m_cbBloom->SetUncheckedTexture(texCheckBoxU);

    stack->AddChild(m_btnRenderTypePrev);
    stack->AddChild(m_btnRenderTypeNext);
    stack->AddChild(m_menuLabelRenderType);

    m_layoutMenu->AddChild(m_cbBloom);
    m_layoutMenu->AddChild(stack);
    UpdateControls();
  }


  void SpringBackground::DestroyMenuUI()
  {
    if (!m_layoutMenu)
    {
      return;
    }

    // Close the menu window
    m_mainMenuStack->RemoveChild(m_layoutMenu);
    m_layoutMenu.reset();

    m_btnRenderTypePrev.reset();
    m_btnRenderTypeNext.reset();
    m_menuLabelRenderType.reset();
    m_cbBloom.reset();
  }


  void SpringBackground::UpdateControls()
  {
    if (!m_layoutMenu)
    {
      return;
    }

    m_menuLabelRenderType->SetContent(m_gridScene.GetRenderName());
    m_cbBloom->SetIsChecked(m_config.Bloom);
  }


  void SpringBackground::PrevGridRender()
  {
    m_gridScene.PrevGridRender();
    UpdateControls();
  }


  void SpringBackground::NextGridRender()
  {
    m_gridScene.NextGridRender();
    UpdateControls();
  }
}
