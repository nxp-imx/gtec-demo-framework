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

#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslSimpleUI/Base/Control/BackgroundNineSlice.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Basic/BasicThemeFactory.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include "SpringBackground.hpp"
#include "OptionParser.hpp"
#include <GLES3/gl3.h>
#include <cassert>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;


  // Geometry Wars had 60.000 points in its spring grid :)
  SpringBackground::SpringBackground(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_batch(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_isLeftButtonDown(false)
    , m_bloomRender(config)
    , m_balls(4)
    , m_explostionType(false)
  {
    {
      auto nativeGraphicsService = config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeGraphics();
      auto contentManager = GetContentManager();
      BasicTextureAtlas atlas;
      contentManager->Read(atlas, "MainAtlas.bta");
      auto texture = contentManager->ReadTexture("MainAtlas.png");
      Texture2D atlasTexture(nativeGraphicsService, texture, Texture2DFilterHint::Smooth);
      TextureAtlasMap textureAtlasMap(atlas);

      m_texFill = AtlasTexture2D(atlasTexture, textureAtlasMap.GetAtlasTextureInfo("Fill"));
      m_texBall = AtlasTexture2D(atlasTexture, textureAtlasMap.GetAtlasTextureInfo("SliderCursor"));
    }
    m_gridScene = std::make_unique<GridScene>(config, m_texFill, config.GetOptions<OptionParser>()->GetGridResolution());


    RegisterExtension(m_uiExtension);

    auto options = config.GetOptions<OptionParser>();

    if (options->GetRenderId() >= 0)
    {
      m_gridScene->SetRenderId(options->GetRenderId());
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


  void SpringBackground::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_btnRenderTypePrev)
    {
      PrevGridRender();
    }
    else if (theEvent->GetSource() == m_btnRenderTypeNext)
    {
      NextGridRender();
    }
  }


  void SpringBackground::OnContentChanged(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
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
          m_gridScene->ApplyExplosiveForce(30, mousePos, 300);
        }
        else
        {
          m_gridScene->ApplyDirectedForce(Vector3(0, 0, 1000), mousePos, 300);
        }
        m_explostionType = !m_explostionType;
      }
      break;
    case VirtualMouseButton::Middle:
      m_mousePosition = event.GetPosition();
      if (event.IsPressed())
      {
        Vector3 mousePos(static_cast<float>(m_mousePosition.X), static_cast<float>(m_mousePosition.Y), 0.0f);
        m_gridScene->ApplyImplosiveForce(500, mousePos, 300);
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

      // m_gridScene->ApplyExplosiveForce(30, mousePosDeep, 200);
      // m_gridScene->ApplyExplosiveForce(100, mousePos, 80);
      // m_gridScene->ApplyImplosiveForce(100, mousePos, 80);
      // m_gridScene->ApplyDirectedForce(Vector3(100, 0, 0), mousePos, 80);
      // m_gridScene->ApplyDirectedForce(delta, mousePos, 80);
      m_gridScene->ApplyDirectedForce(delta * 0.2f * 2, mousePos, 40);
      //      m_gridScene->ApplyDirectedForce(delta * 0.2f * 20, mousePos - delta, 40);
    }

    if (m_config.Balls)
    {
      auto windowSizePx = GetWindowSizePx();
      const float boundaryLeft = 0;
      const auto boundaryRight = static_cast<float>(windowSizePx.Width());
      const float boundaryTop = 0;
      const auto boundaryBottom = static_cast<float>(windowSizePx.Height());
      const float bounce = 1.0f;
      for (auto itr = m_balls.begin(); itr != m_balls.end(); ++itr)
      {
        Vector2 velocity = itr->Position - itr->OldPosition;
        itr->OldPosition = itr->Position;
        itr->Position += velocity;

        m_gridScene->ApplyDirectedForce(Vector3(velocity.X, velocity.Y, 0.0f) * 1.0f, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);

        // m_gridScene->ApplyDirectedForce(Vector3(0, 0, 100.0f), Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene->ApplyDirectedForce(Vector3(0, 0, -100.0f), Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene->ApplyImplosiveForce(-100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene->ApplyImplosiveForce(100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene->ApplyImplosiveForce(30, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene->ApplyImplosiveForce(10, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);

        // m_gridScene->ApplyExplosiveForce(100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene->ApplyExplosiveForce(-100, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);
        // m_gridScene->ApplyExplosiveForce(-30, Vector3(itr->Position.X, itr->Position.Y, 0.0f), 40);


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

    m_gridScene->FixedUpdate(demoTime);
  }


  void SpringBackground::Update(const DemoTime& demoTime)
  {
    Vector3 mousePos(static_cast<float>(m_mousePosition.X), static_cast<float>(m_mousePosition.Y), 0.0f);
    m_oldMouse = mousePos;

    m_gridScene->Update(demoTime);
    m_bloomRender.Update(demoTime);
  }


  void SpringBackground::Draw(const DemoTime& /*demoTime*/)
  {
    if (m_config.Bloom)
    {
      m_bloomRender.Draw(*m_gridScene);
    }
    else
    {
      m_gridScene->Draw();
    }


    if (m_config.Balls)
    {
      m_batch->Begin();

      const Color ballColor = Color::White();
      Vector2 scale(1, 1);
      Vector2 origin(m_texBall.GetSize().Width() * 0.5f, m_texBall.GetSize().Height() * 0.5f);
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

    UI::Theme::BasicThemeFactory factory(context, m_uiExtension->GetSpriteResourceManager(), m_uiExtension->GetDefaultMaterialId());

    auto layoutMenu = std::make_shared<UI::StackLayout>(context);
    auto menuBar = factory.CreateBottomBar(layoutMenu, UI::Theme::BarType::Transparent);
    {    // Create the menu
      layoutMenu->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
      layoutMenu->SetAlignmentX(UI::ItemAlignment::Near);
      layoutMenu->SetAlignmentY(UI::ItemAlignment::Near);
      layoutMenu->SetSpacing(10.0f);

      ISpriteResourceManager& rSpriteManager = m_uiExtension->GetSpriteResourceManager();
      const auto defaultMaterialId = m_uiExtension->GetDefaultMaterialId();

      auto spriteBackSmall = rSpriteManager.CreateImageSprite(defaultMaterialId, "Icon/Navigation/ic_chevron_left_white_36dp");
      auto spriteNextSmall = rSpriteManager.CreateImageSprite(defaultMaterialId, "Icon/Navigation/ic_chevron_right_white_36dp");

      m_menuLabelRenderType = factory.CreateLabel("");
      m_menuLabelRenderType->SetAlignmentY(UI::ItemAlignment::Center);

      m_btnRenderTypePrev = factory.CreateImageButton(UI::Theme::ImageButtonType::Small, spriteBackSmall);
      m_btnRenderTypePrev->SetAlignmentY(UI::ItemAlignment::Center);

      m_btnRenderTypeNext = factory.CreateImageButton(UI::Theme::ImageButtonType::Small, spriteNextSmall);
      m_btnRenderTypeNext->SetAlignmentY(UI::ItemAlignment::Center);

      m_cbBloom = factory.CreateSwitch("Bloom");
      m_cbBloom->SetAlignmentY(UI::ItemAlignment::Center);

      auto stack = std::make_shared<UI::StackLayout>(context);
      stack->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
      stack->SetAlignmentY(UI::ItemAlignment::Center);
      stack->AddChild(m_btnRenderTypePrev);
      stack->AddChild(m_btnRenderTypeNext);
      stack->AddChild(m_menuLabelRenderType);

      layoutMenu->AddChild(m_cbBloom);
      layoutMenu->AddChild(stack);
      UpdateControls();
    }

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(menuBar);
  }

  void SpringBackground::UpdateControls()
  {
    m_menuLabelRenderType->SetContent(m_gridScene->GetRenderName());
    m_cbBloom->SetIsChecked(m_config.Bloom);
  }


  void SpringBackground::PrevGridRender()
  {
    m_gridScene->PrevGridRender();
    UpdateControls();
  }


  void SpringBackground::NextGridRender()
  {
    m_gridScene->NextGridRender();
    UpdateControls();
  }
}
