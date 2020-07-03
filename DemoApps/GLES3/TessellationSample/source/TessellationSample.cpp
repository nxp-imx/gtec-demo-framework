/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include "TessellationSample.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Base/Control/BackgroundNineSlice.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/LabelNineSliceButton.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Basic/BasicThemeFactory.hpp>
#include <memory>

// Because of inconsistency in khronos extension definition both the 31 and 2 headers are needed
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "TestScene/TestScene.hpp"
#include "LoadedScene.hpp"
#include "OptionParser.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    namespace LocalConfig
    {
      constexpr ConstrainedValue<float> TInner(1.0f, 1.0f, 50.0f);
      constexpr ConstrainedValue<float> TOuter(1.0f, 1.0f, 50.0f);
      constexpr ConstrainedValue<float> DispFactor(1.0f, 0.0f, 50.0f);
      constexpr ConstrainedValue<float> DispMod(1.0f, -10.0f, 10.0f);
      constexpr ConstrainedValue<float> Shininess(1.0f, 0.0f, 100.0f);
    }

    template <typename T>
    SceneRecord CreateSceneRecord(UI::Theme::BasicThemeFactory& factory, UI::StackLayout& stack, const DemoAppConfig& config,
                                  const std::shared_ptr<OptionParser>& options, const int32_t id, const std::string& name,
                                  const std::shared_ptr<UI::RadioGroup>& radioGroup)
    {
      SceneRecord record;
      record.Scene = std::make_shared<T>(config, options, id);
      record.Button = factory.CreateRadioButton(radioGroup, name);
      stack.AddChild(record.Button);
      return record;
    }
  }


  TessellationSample::TessellationSample(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(std::dynamic_pointer_cast<GLES3::NativeBatch2D>(m_graphics->GetNativeBatch2D()))
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
  {
    RegisterExtension(m_uiExtension);

    auto options = config.GetOptions<OptionParser>();

    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();

    m_context = m_uiExtension->GetContext();
    UI::Theme::BasicThemeFactory factory(m_context, m_uiExtension->GetSpriteResourceManager(), m_uiExtension->GetDefaultMaterialId());

    m_buttonStack = std::make_shared<UI::StackLayout>(m_context);

    auto radioGroup = factory.CreateRadioGroup("Scene");
    m_scenes.push_back(CreateSceneRecord<LoadedScene>(factory, *m_buttonStack, config, options, 0, "Face", radioGroup));
    m_scenes.push_back(CreateSceneRecord<TestScene>(factory, *m_buttonStack, config, options, 0, "Torus", radioGroup));
    m_scenes.push_back(CreateSceneRecord<TestScene>(factory, *m_buttonStack, config, options, 1, "Rocks", radioGroup));
    // m_scenes.push_back(CreateSceneRecord<TestScene>(factory, *m_buttonStack, config, options, 2, "Plane", radioGroup));
    m_scenes.push_back(CreateSceneRecord<TestScene>(factory, *m_buttonStack, config, options, 3, "Cube", radioGroup));

    BuildUI(factory);


    switch (options->GetScene())
    {
    case 0:
      m_scene = m_scenes.front().Scene;
      m_scenes.front().Button->SetIsChecked(true);
      m_scenes.front().Button->FinishAnimation();
      break;
    default:
      m_scene = m_scenes.back().Scene;
      m_scenes.back().Button->SetIsChecked(true);
      m_scenes.back().Button->FinishAnimation();
      break;
    }

    UpdateUIWithSceneSettings();
  }


  TessellationSample::~TessellationSample() = default;


  void TessellationSample::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(theEvent);
  }


  void TessellationSample::OnContentChanged(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    auto source = theEvent->GetSource();
    if (source == m_sliderTInner)
    {
      m_tessellationConfig.TessLevelInner = static_cast<float>(m_sliderTInner->GetValue());
      m_scene->SetRenderTessellationConfig(m_tessellationConfig);
    }
    else if (source == m_sliderTOuter)
    {
      m_tessellationConfig.TessLevelOuter = static_cast<float>(m_sliderTOuter->GetValue());
      m_scene->SetRenderTessellationConfig(m_tessellationConfig);
    }
    else if (source == m_sliderTDispFactor)
    {
      m_tessellationConfig.DisplacementFactor = static_cast<float>(m_sliderTDispFactor->GetValue());
      m_scene->SetRenderTessellationConfig(m_tessellationConfig);
    }
    else if (source == m_sliderTDispMod)
    {
      m_tessellationConfig.DisplacementMod = static_cast<float>(m_sliderTDispMod->GetValue());
      m_scene->SetRenderTessellationConfig(m_tessellationConfig);
    }
    else if (source == m_sliderShininess)
    {
      m_material.Shininess = static_cast<float>(m_sliderShininess->GetValue());
      m_scene->SetRenderMaterial(m_material);
    }
    else if (source == m_checkDisplacement)
    {
      m_drawConfig.UseDisplacement = m_checkDisplacement->IsChecked();
    }
    else if (source == m_checkTexture)
    {
      m_drawConfig.UseTexture = m_checkTexture->IsChecked();
    }
    else if (source == m_checkWireframe)
    {
      m_drawConfig.UseWireframe = m_checkWireframe->IsChecked();
    }
    else if (source == m_checkRotate)
    {
      m_drawConfig.UseRotation = m_checkRotate->IsChecked();
    }
    else
    {
      for (auto itr = m_scenes.begin(); itr != m_scenes.end(); ++itr)
      {
        if (source == itr->Button && itr->Button->IsChecked())
        {
          m_scene = itr->Scene;
          UpdateUIWithSceneSettings();
        }
      }
    }
  }


  void TessellationSample::OnKeyEvent(const KeyEvent& event)
  {
    if (m_scene)
    {
      m_scene->OnKeyEvent(event);
    }

    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::W:
      event.Handled();
      m_checkWireframe->Toggle();
      break;
    case VirtualKey::R:
      event.Handled();
      m_checkRotate->Toggle();
      break;
    case VirtualKey::C:
      event.Handled();
      m_checkTexture->Toggle();
      break;
    case VirtualKey::D:
      event.Handled();
      m_checkDisplacement->Toggle();
      break;
    case VirtualKey::Q:
      // m_uiExtension->GetWindowManager()->ScheduleClose(m_fillLayout);
      break;
    default:
      break;
    }
  }


  void TessellationSample::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    if (m_scene)
    {
      m_scene->OnMouseButtonEvent(event);
    }
  }


  void TessellationSample::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    if (m_scene)
    {
      m_scene->OnMouseMoveEvent(event);
    }
  }


  void TessellationSample::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    if (m_scene)
    {
      m_scene->OnMouseWheelEvent(event);
    }
  }


  void TessellationSample::Update(const DemoTime& demoTime)
  {
    if (m_scene)
    {
      m_scene->Update(demoTime, m_drawConfig);
    }
  }


  void TessellationSample::Draw(const DemoTime& /*demoTime*/)
  {
    if (m_scene)
    {
      m_scene->Draw(m_drawConfig);
    }

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void TessellationSample::BuildUI(UI::Theme::BasicThemeFactory& factory)
  {
    // RenderMode:
    // Displacement on/off
    // WireFrame on/off
    // Texture on/off
    // Rotate on/off
    // TInner slider
    // TOuter slider
    // DispFactor slider
    // DispMod slider
    // Shininess slider

    m_checkDisplacement = factory.CreateSwitch("Displacement", true);
    m_checkWireframe = factory.CreateSwitch("WireFrame", false);
    m_checkTexture = factory.CreateSwitch("Texture", true);
    m_checkRotate = factory.CreateSwitch("Rotate", false);

    auto labalTInner = factory.CreateLabel("Inner tessellation");
    auto labalTOuter = factory.CreateLabel("Outer tessellation");
    auto labalDispFactor = factory.CreateLabel("Displacement factor");
    auto labalDispMod = factory.CreateLabel("Displacement mod");
    auto labalShininess = factory.CreateLabel("Shininess");
    m_sliderTInner = factory.CreateSliderFmtValue<float>(UI::LayoutOrientation::Horizontal, LocalConfig::TInner);
    m_sliderTOuter = factory.CreateSliderFmtValue<float>(UI::LayoutOrientation::Horizontal, LocalConfig::TOuter);
    m_sliderTDispFactor = factory.CreateSliderFmtValue<float>(UI::LayoutOrientation::Horizontal, LocalConfig::DispFactor);
    m_sliderTDispMod = factory.CreateSliderFmtValue<float>(UI::LayoutOrientation::Horizontal, LocalConfig::DispMod);
    m_sliderShininess = factory.CreateSliderFmtValue<float>(UI::LayoutOrientation::Horizontal, LocalConfig::Shininess);


    m_buttonStack->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    m_buttonStack->SetAlignmentX(UI::ItemAlignment::Near);
    m_buttonStack->SetAlignmentY(UI::ItemAlignment::Far);
    m_buttonStack->AddChild(m_checkDisplacement);
    m_buttonStack->AddChild(m_checkWireframe);
    m_buttonStack->AddChild(m_checkTexture);
    m_buttonStack->AddChild(m_checkRotate);
    m_buttonStack->AddChild(labalTInner);
    m_buttonStack->AddChild(m_sliderTInner);
    m_buttonStack->AddChild(labalTOuter);
    m_buttonStack->AddChild(m_sliderTOuter);
    m_buttonStack->AddChild(labalDispFactor);
    m_buttonStack->AddChild(m_sliderTDispFactor);
    m_buttonStack->AddChild(labalDispMod);
    m_buttonStack->AddChild(m_sliderTDispMod);
    m_buttonStack->AddChild(labalShininess);
    m_buttonStack->AddChild(m_sliderShininess);

    // m_caption = factory.CreateLabel();
    // m_caption->SetAlignmentX(ItemAlignment::Near);
    // m_caption->SetAlignmentY(ItemAlignment::Near);

    m_buttonStack->SetAlignmentX(UI::ItemAlignment::Near);
    m_buttonStack->SetAlignmentY(UI::ItemAlignment::Near);


    auto bar = factory.CreateLeftBar(m_buttonStack, UI::Theme::BarType::Transparent);

    // Add the fill layout to the window manager to ensure it is visible
    m_uiExtension->GetWindowManager()->Add(bar);


    m_checkDisplacement->SetIsChecked(true);
    m_checkTexture->SetIsChecked(true);

    UpdateCaption();

    m_checkDisplacement->SetIsChecked(m_drawConfig.UseDisplacement);
    m_checkRotate->SetIsChecked(m_drawConfig.UseRotation);
    m_checkTexture->SetIsChecked(m_drawConfig.UseTexture);
    m_checkWireframe->SetIsChecked(m_drawConfig.UseWireframe);
  }


  void TessellationSample::UpdateCaption()
  {
    // m_caption->SetContent(m_basicRenderingEnabled ? "RenderMode: Basic (t)" : "RenderMode: Tessellation (t)");
  }

  void TessellationSample::UpdateUIWithSceneSettings()
  {
    m_tessellationConfig = m_scene->GetRenderTessellationConfig();
    m_material = m_scene->GetRenderMaterial();

    // For now methods that cause a event to be send need to be called after the window has been added to the window manager.
    m_sliderTInner->SetValue(m_tessellationConfig.TessLevelInner);
    m_sliderTOuter->SetValue(m_tessellationConfig.TessLevelOuter);
    m_sliderTDispFactor->SetValue(m_tessellationConfig.DisplacementFactor);
    m_sliderTDispMod->SetValue(m_tessellationConfig.DisplacementMod);
    m_sliderShininess->SetValue(m_material.Shininess);
  }
}
