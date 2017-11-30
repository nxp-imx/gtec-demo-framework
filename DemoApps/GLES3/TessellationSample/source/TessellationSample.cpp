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
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Base/Service/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/LabelButton.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>

// Because of inconsistency in khronos extension definition both the 31 and 2 headers are needed
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "TestScene/TestScene.hpp"
#include "LoadedScene.hpp"
#include "OptionParser.hpp"
#include "SliderControl.hpp"

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    std::shared_ptr<SliderControl> CreateSlider(const std::shared_ptr<WindowContext>& context, const std::string& name, const int32_t min, const int32_t max, const float widthF1, const float widthF2, const float widthF3,
                                                const AtlasTexture2D& texBackground, const AtlasTexture2D& texCursor, const NineSlice& nineSlice, const ThicknessF& cursorPadding)
    {
      std::shared_ptr<SliderControl> slider(new SliderControl(context, name, min, max, widthF1, widthF2, widthF3));
      slider->GetSlider()->SetBackgroundTexture(texBackground);
      slider->GetSlider()->SetCursorTexture(texCursor);
      slider->GetSlider()->SetCursorPadding(cursorPadding);
      slider->GetSlider()->SetNineSlice(nineSlice);
      return slider;
    }

    template<typename T>
    SceneRecord CreateSceneRecord(StackLayout& stack, const std::shared_ptr<WindowContext>& context, const DemoAppConfig& config, const std::shared_ptr<OptionParser>& options, const int32_t id, const std::string& name)
    {
      SceneRecord record;
      record.Scene = std::make_shared<T>(config, options, id);
      record.Button.reset(new LabelButton(context));
      record.Button->SetContent(name);
      stack.Children.Add(record.Button);
      return record;
    }
  }



  TessellationSample::TessellationSample(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(std::dynamic_pointer_cast<GLES3::NativeBatch2D>(m_graphics->GetNativeBatch2D()))
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))
    , m_context()
    , m_caption()
    , m_checkDisplacement()
    , m_checkWireframe()
    , m_checkTexture()
    , m_checkRotate()
    , m_sliderTInner()
    , m_sliderTOuter()
    , m_sliderTDispFactor()
    , m_sliderTDispMod()
    , m_sliderShininess()
    , m_fillLayout()
    , m_scene()
    , m_tessellationConfig()
  {
    RegisterExtension(m_uiExtension);

    if (!GLUtil::HasExtension("GL_EXT_tessellation_shader"))
      throw NotSupportedException("GL_EXT_tessellation_shader extension not supported");
    if (!GLUtil::HasExtension("GL_EXT_geometry_shader"))
      throw NotSupportedException("GL_EXT_geometry_shader extension not supported");

    auto options = config.GetOptions<OptionParser>();



    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();
    BuildUI(contentManager);

    m_scenes.push_back(CreateSceneRecord<LoadedScene>(*m_buttonStack, m_context, config, options, 0, "Face"));
    m_scenes.push_back(CreateSceneRecord<TestScene>(*m_buttonStack, m_context, config, options, 0, "Torus"));
    m_scenes.push_back(CreateSceneRecord<TestScene>(*m_buttonStack, m_context, config, options, 1, "Rocks"));
    //m_scenes.push_back(CreateSceneRecord<TestScene>(*m_buttonStack, m_context, config, options, 2, "Plane"));
    m_scenes.push_back(CreateSceneRecord<TestScene>(*m_buttonStack, m_context, config, options, 3, "Cube"));


    switch (options->GetScene())
    {
    case 0:
      m_scene = m_scenes.front().Scene;
      break;
    default:
      m_scene = m_scenes.back().Scene;
      break;
    }

    UpdateUIWithSceneSettings();
  }


  TessellationSample::~TessellationSample()
  {

  }


  void TessellationSample::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    auto source = theEvent->GetSource();

    for (auto itr = m_scenes.begin(); itr != m_scenes.end(); ++itr)
    {
      if (source == itr->Button)
      {
        m_scene = itr->Scene;
        UpdateUIWithSceneSettings();
      }
    }
  }



  void TessellationSample::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
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

  }


  void TessellationSample::OnKeyEvent(const KeyEvent& event)
  {
    if (m_scene)
      m_scene->OnKeyEvent(event);

    if (event.IsHandled() || ! event.IsPressed() )
      return;

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
      //m_uiExtension->GetWindowManager()->ScheduleClose(m_fillLayout);
      break;
    default:
      break;
    }
  }


  void TessellationSample::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
      return;

    if (m_scene)
      m_scene->OnMouseButtonEvent(event);
  }


  void TessellationSample::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (event.IsHandled())
      return;

    if (m_scene)
      m_scene->OnMouseMoveEvent(event);
  }


  void TessellationSample::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    if (m_scene)
      m_scene->OnMouseWheelEvent(event);
  }


  void TessellationSample::Update(const DemoTime& demoTime)
  {
    if (m_scene)
      m_scene->Update(demoTime, m_drawConfig);
  }


  void TessellationSample::Draw(const DemoTime& demoTime)
  {
    if (m_scene)
      m_scene->Draw(m_drawConfig);

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void TessellationSample::BuildUI(const std::shared_ptr<IContentManager>& contentManager)
  {
    m_context = m_uiExtension->GetContext();

    AtlasTexture2D texSlider = m_uiExtension->GetAtlasTexture2D("Slider");
    AtlasTexture2D texSliderCursor = m_uiExtension->GetAtlasTexture2D("SliderCursor");
    AtlasTexture2D texCheckBoxC = m_uiExtension->GetAtlasTexture2D("CheckBoxC");
    AtlasTexture2D texCheckBoxU = m_uiExtension->GetAtlasTexture2D("CheckBoxU");

    NineSlice sliderNineSlice(13, 0, 13, 0);
    ThicknessF sliderCursorPadding(13, 0, 13, 0);

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

    m_checkDisplacement.reset(new CheckBox(m_context));
    m_checkDisplacement->SetText("Displacement");
    m_checkDisplacement->SetCheckedTexture(texCheckBoxC);
    m_checkDisplacement->SetUncheckedTexture(texCheckBoxU);

    m_checkWireframe.reset(new CheckBox(m_context));
    m_checkWireframe->SetText("WireFrame");
    m_checkWireframe->SetCheckedTexture(texCheckBoxC);
    m_checkWireframe->SetUncheckedTexture(texCheckBoxU);

    m_checkTexture.reset(new CheckBox(m_context));
    m_checkTexture->SetText("Texture");
    m_checkTexture->SetCheckedTexture(texCheckBoxC);
    m_checkTexture->SetUncheckedTexture(texCheckBoxU);

    m_checkRotate.reset(new CheckBox(m_context));
    m_checkRotate->SetText("Rotate");
    m_checkRotate->SetCheckedTexture(texCheckBoxC);
    m_checkRotate->SetUncheckedTexture(texCheckBoxU);

    float widthF1 = 250;
    float widthF2 = 200;
    float widthF3 = 80;

    m_sliderTInner = CreateSlider(m_context, "TInner", 1, 50, widthF1, widthF2, widthF3, texSlider, texSliderCursor, sliderNineSlice, sliderCursorPadding);
    m_sliderTOuter = CreateSlider(m_context, "TOuter", 1, 50, widthF1, widthF2, widthF3, texSlider, texSliderCursor, sliderNineSlice, sliderCursorPadding);
    m_sliderTDispFactor = CreateSlider(m_context, "DispFactor", 0, 50, widthF1, widthF2, widthF3, texSlider, texSliderCursor, sliderNineSlice, sliderCursorPadding);
    m_sliderTDispMod = CreateSlider(m_context, "DispMod", -10, 10, widthF1, widthF2, widthF3, texSlider, texSliderCursor, sliderNineSlice, sliderCursorPadding);
    m_sliderShininess = CreateSlider(m_context, "Shininess", 0, 100, widthF1, widthF2, widthF3, texSlider, texSliderCursor, sliderNineSlice, sliderCursorPadding);

    std::shared_ptr<StackLayout> stackLayout(new StackLayout(m_context));
    stackLayout->SetLayoutOrientation(LayoutOrientation::Vertical);
    stackLayout->SetAlignmentX(ItemAlignment::Near);
    stackLayout->SetAlignmentY(ItemAlignment::Far);
    stackLayout->Children.Add(m_checkDisplacement);
    stackLayout->Children.Add(m_checkWireframe);
    stackLayout->Children.Add(m_checkTexture);
    stackLayout->Children.Add(m_checkRotate);
    stackLayout->Children.Add(m_sliderTInner);
    stackLayout->Children.Add(m_sliderTOuter);
    stackLayout->Children.Add(m_sliderTDispFactor);
    stackLayout->Children.Add(m_sliderTDispMod);
    stackLayout->Children.Add(m_sliderShininess);

    m_caption.reset(new Label(m_context));
    m_caption->SetAlignmentX(ItemAlignment::Near);
    m_caption->SetAlignmentY(ItemAlignment::Near);

    m_buttonStack.reset(new StackLayout(m_context));
    m_buttonStack->SetAlignmentX(ItemAlignment::Near);
    m_buttonStack->SetAlignmentY(ItemAlignment::Near);

    m_fillLayout.reset(new FillLayout(m_context));
    m_fillLayout->Children.Add(m_caption);
    m_fillLayout->Children.Add(stackLayout);
    m_fillLayout->Children.Add(m_buttonStack);


    // Add the fill layout to the window manager to ensure it is visible
    m_context->WindowManager->Add(m_fillLayout);


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
    //m_caption->SetContent(m_basicRenderingEnabled ? "RenderMode: Basic (t)" : "RenderMode: Tessellation (t)");
  }

  void TessellationSample::UpdateUIWithSceneSettings()
  {
    m_tessellationConfig = m_scene->GetRenderTessellationConfig();
    m_material = m_scene->GetRenderMaterial();

    // For now methods that cause a event to be send need to be called after the window has been added to the window manager.
    m_sliderTInner->SetValue(static_cast<int32_t>(m_tessellationConfig.TessLevelInner));
    m_sliderTOuter->SetValue(static_cast<int32_t>(m_tessellationConfig.TessLevelOuter));
    m_sliderTDispFactor->SetValue(static_cast<int32_t>(m_tessellationConfig.DisplacementFactor));
    m_sliderTDispMod->SetValue(static_cast<int32_t>(m_tessellationConfig.DisplacementMod));
    m_sliderShininess->SetValue(static_cast<int32_t>(m_material.Shininess));
  }


}
