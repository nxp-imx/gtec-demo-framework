/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <Shared/DFSimpleUI101/Shared.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Control/Background9Slice.hpp>
#include <FslSimpleUI/Base/Control/Button.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/FloatSlider.hpp>
#include <FslSimpleUI/Base/Control/FloatSliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/LabelButton.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Control/SliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Control/ValueLabel.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>

#include <cassert>

namespace Fsl
{
  using namespace UI;

  namespace
  {
    std::shared_ptr<Label> CreateLabel(const std::shared_ptr<WindowContext>& context, const std::string& text, const ItemAlignment alignX,
                                       const ItemAlignment alignY, const std::shared_ptr<AtlasFont>& font)
    {
      auto label = std::make_shared<Label>(context);
      label->SetContent(text);
      label->SetAlignmentX(alignX);
      label->SetAlignmentY(alignY);
      label->SetFont(font);
      return label;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))    // Prepare the extension
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
  {
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    auto atlasTexture = m_uiExtension->GetAtlasTexture();
    auto windowContext = m_uiExtension->GetContext();

    // FIX: way too cumbersome to configure a font

    AtlasTexture2D tex1 = m_uiExtension->GetAtlasTexture2D("RectTex");
    AtlasTexture2D texBackground = m_uiExtension->GetAtlasTexture2D("Background9R");

    BasicFontKerning smallFontkerning;
    contentManager->Read(smallFontkerning, "MainAtlasSmallFont.fbk");

    TextureAtlasBitmapFont bitmapFontSmall(m_uiExtension->GetDefaultTextureAtlas(), smallFontkerning);

    std::shared_ptr<AtlasFont> fontSmall = std::make_shared<AtlasFont>(atlasTexture, bitmapFontSmall);

    auto stackLayout1 = CreateStack1(windowContext, atlasTexture, fontSmall, tex1);
    auto stackLayout2 = CreateStack2(windowContext, atlasTexture, tex1);
    auto stackLayout3 = CreateStack3(windowContext, atlasTexture);
    auto stackLayout4 = CreateStack4(windowContext, atlasTexture);

    NineSlice backgroundNineSlice(32, 32, 32, 32);
    auto background = std::make_shared<Background9Slice>(windowContext);
    background->SetContent(stackLayout2);
    background->SetBackground(texBackground);
    background->SetNineSlice(backgroundNineSlice);
    background->SetPadding(ThicknessF(32, 32, 32, 32));

    // Create a stack which contains the other stacks
    auto stackLayout = std::make_shared<StackLayout>(windowContext);
    stackLayout->SetAlignmentX(ItemAlignment::Center);
    stackLayout->SetAlignmentY(ItemAlignment::Center);
    stackLayout->SetLayoutOrientation(LayoutOrientation::Horizontal);
    stackLayout->AddChild(stackLayout1);
    // stackLayout->AddChild(stackLayout2);
    // stackLayout->AddChild(m_complexButton);
    stackLayout->AddChild(background);
    stackLayout->AddChild(stackLayout3);
    stackLayout->AddChild(stackLayout4);


    // Create labels for the corners
    auto labelCorner1 = CreateLabel(windowContext, "TopLeft", ItemAlignment::Near, ItemAlignment::Near, fontSmall);
    auto labelCorner2 = CreateLabel(windowContext, "TopRight", ItemAlignment::Far, ItemAlignment::Near, fontSmall);
    auto labelCorner3 = CreateLabel(windowContext, "BottomLeft", ItemAlignment::Near, ItemAlignment::Far, fontSmall);
    auto labelCorner4 = CreateLabel(windowContext, "BottomRight", ItemAlignment::Far, ItemAlignment::Far, fontSmall);

    // Create labels for the border centers
    auto labelCenter1 = CreateLabel(windowContext, "TopCenter", ItemAlignment::Center, ItemAlignment::Near, fontSmall);
    auto labelCenter2 = CreateLabel(windowContext, "BottomCenter", ItemAlignment::Center, ItemAlignment::Far, fontSmall);
    auto labelCenter3 = CreateLabel(windowContext, "LeftCenter", ItemAlignment::Near, ItemAlignment::Center, fontSmall);
    auto labelCenter4 = CreateLabel(windowContext, "RightCenter", ItemAlignment::Far, ItemAlignment::Center, fontSmall);

    // Create the root layout and add it to the window manager
    m_fillLayout = std::make_shared<FillLayout>(windowContext);
    m_fillLayout->AddChild(labelCorner1);
    m_fillLayout->AddChild(labelCorner2);
    m_fillLayout->AddChild(labelCorner3);
    m_fillLayout->AddChild(labelCorner4);
    m_fillLayout->AddChild(labelCenter1);
    m_fillLayout->AddChild(labelCenter2);
    m_fillLayout->AddChild(labelCenter3);
    m_fillLayout->AddChild(labelCenter4);
    m_fillLayout->AddChild(stackLayout);

    // Register the root layout with the window manager
    m_uiExtension->GetWindowManager()->Add(m_fillLayout);
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_button1)
    {
      FSLLOG3_INFO("Button1 selected");
    }
    else if (theEvent->GetSource() == m_button2)
    {
      FSLLOG3_INFO("Button2 selected");
    }
    else if (theEvent->GetSource() == m_complexButton)
    {
      FSLLOG3_INFO("Complex button selected");
    }
  }


  void Shared::Draw()
  {
    m_uiExtension->Draw();
  }


  std::shared_ptr<BaseWindow> Shared::CreateStack1(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture,
                                                   const std::shared_ptr<AtlasFont>& fontSmall, const AtlasTexture2D& texImage)
  {
    // Create items that we want to add to a stack layout
    auto label1 = std::make_shared<Label>(context);
    auto label2 = std::make_shared<Label>(context);
    auto label3 = std::make_shared<Label>(context);
    auto labelSmall = std::make_shared<Label>(context);
    label1->SetContent("1");
    label2->SetContent("2");
    label3->SetContent("3");
    label1->SetAlignmentX(ItemAlignment::Near);
    label2->SetAlignmentX(ItemAlignment::Center);
    label3->SetAlignmentX(ItemAlignment::Far);
    labelSmall->SetContent("Hello world");
    labelSmall->SetFont(fontSmall);
    labelSmall->SetAlignmentX(ItemAlignment::Center);

    auto labelValue1 = std::make_shared<ValueLabel>(context);
    labelValue1->SetContent(1337);
    labelValue1->SetAlignmentX(ItemAlignment::Center);

    auto labelValue2 = std::make_shared<FloatValueLabel>(context);
    labelValue2->SetContent(3.14f);
    labelValue2->SetAlignmentX(ItemAlignment::Center);

    m_button1 = std::make_shared<LabelButton>(context);
    m_button1->SetAlignmentX(ItemAlignment::Center);
    m_button1->SetContent("Button1");

    m_button2 = std::make_shared<ImageButton>(context);
    m_button2->SetAlignmentX(ItemAlignment::Center);
    m_button2->SetContent(texImage);

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<StackLayout>(context);
    stackLayout->SetAlignmentY(ItemAlignment::Center);
    stackLayout->AddChild(label1);
    stackLayout->AddChild(label2);
    stackLayout->AddChild(label3);
    stackLayout->AddChild(labelSmall);
    stackLayout->AddChild(labelValue1);
    stackLayout->AddChild(labelValue2);
    stackLayout->AddChild(m_button1);
    stackLayout->AddChild(m_button2);
    return stackLayout;
  }


  std::shared_ptr<BaseWindow> Shared::CreateStack2(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture,
                                                   const AtlasTexture2D& texImage)
  {
    auto label = std::make_shared<Label>(context);
    label->SetContent("Images:");

    // Create items that we want to add to a another stack layout
    auto image1 = std::make_shared<Image>(context);
    auto image2 = std::make_shared<Image>(context);
    auto image3 = std::make_shared<Image>(context);
    auto image4N = std::make_shared<Image>(context);
    auto image4C = std::make_shared<Image>(context);
    auto image4F = std::make_shared<Image>(context);
    auto image4S = std::make_shared<Image>(context);
    image1->SetContent(texImage);
    image2->SetContent(texImage);
    image3->SetContent(texImage);
    image4N->SetContent(texImage);
    image4C->SetContent(texImage);
    image4F->SetContent(texImage);
    image4S->SetContent(texImage);
    image1->SetScalePolicy(ItemScalePolicy::Fit);
    image2->SetScalePolicy(ItemScalePolicy::Fit);
    image3->SetScalePolicy(ItemScalePolicy::Fit);
    image4N->SetScalePolicy(ItemScalePolicy::FitKeepAR);
    image4C->SetScalePolicy(ItemScalePolicy::FitKeepAR);
    image4F->SetScalePolicy(ItemScalePolicy::FitKeepAR);
    image4S->SetScalePolicy(ItemScalePolicy::Fit);
    image1->SetAlignmentX(ItemAlignment::Center);
    image2->SetAlignmentX(ItemAlignment::Center);
    image3->SetAlignmentX(ItemAlignment::Center);
    image4N->SetAlignmentX(ItemAlignment::Near);
    image4C->SetAlignmentX(ItemAlignment::Center);
    image4F->SetAlignmentX(ItemAlignment::Far);
    image4S->SetAlignmentX(ItemAlignment::Stretch);

    image2->SetWidth(texImage.GetSize().X);
    image2->SetHeight(texImage.GetSize().Y / 2);
    image3->SetWidth(texImage.GetSize().X / 2);
    image3->SetHeight(texImage.GetSize().Y);
    image4N->SetHeight(texImage.GetSize().Y / 4);
    image4C->SetHeight(texImage.GetSize().Y / 4);
    image4F->SetHeight(texImage.GetSize().Y / 4);
    image4S->SetHeight(texImage.GetSize().Y / 4);

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<StackLayout>(context);
    stackLayout->SetAlignmentY(ItemAlignment::Center);
    stackLayout->AddChild(label);
    stackLayout->AddChild(image1);
    stackLayout->AddChild(image2);
    stackLayout->AddChild(image3);
    stackLayout->AddChild(image4N);
    stackLayout->AddChild(image4C);
    stackLayout->AddChild(image4F);
    stackLayout->AddChild(image4S);
    return stackLayout;
  }


  std::shared_ptr<BaseWindow> Shared::CreateStack3(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture)
  {
    AtlasTexture2D texTractbar = m_uiExtension->GetAtlasTexture2D("Slider");
    AtlasTexture2D texTractbarCursor = m_uiExtension->GetAtlasTexture2D("SliderCursor");
    NineSlice sliderNineSlice(40, 0, 46, 0);


    auto labelSliders = std::make_shared<Label>(context);
    labelSliders->SetContent("Sliders:");
    labelSliders->SetWidth(200);

    m_slider1 = std::make_shared<Slider>(context);
    m_slider1->SetAlignmentX(ItemAlignment::Center);
    m_slider1->SetAlignmentY(ItemAlignment::Center);
    m_slider1->SetBackgroundTexture(texTractbar);
    m_slider1->SetCursorTexture(texTractbarCursor);
    m_slider1->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    m_slider1->SetNineSlice(sliderNineSlice);

    m_slider2 = std::make_shared<Slider>(context);
    m_slider2->SetAlignmentX(ItemAlignment::Stretch);
    m_slider2->SetAlignmentY(ItemAlignment::Center);
    m_slider2->SetBackgroundTexture(texTractbar);
    m_slider2->SetCursorTexture(texTractbarCursor);
    m_slider2->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    m_slider2->SetNineSlice(sliderNineSlice);

    auto slider3 = std::make_shared<SliderAndValueLabel>(context);
    slider3->SetAlignmentX(ItemAlignment::Stretch);
    slider3->SetBackgroundTexture(texTractbar);
    slider3->SetCursorTexture(texTractbarCursor);
    slider3->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider3->SetNineSlice(sliderNineSlice);

    auto slider4 = std::make_shared<Slider>(context);
    slider4->SetAlignmentX(ItemAlignment::Stretch);
    slider4->SetBackgroundTexture(texTractbar);
    slider4->SetCursorTexture(texTractbarCursor);
    slider4->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider4->SetNineSlice(sliderNineSlice);
    slider4->SetValueLimits(0, 5);

    auto slider5 = std::make_shared<SliderAndValueLabel>(context);
    slider5->SetAlignmentX(ItemAlignment::Stretch);
    slider5->SetBackgroundTexture(texTractbar);
    slider5->SetCursorTexture(texTractbarCursor);
    slider5->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider5->SetNineSlice(sliderNineSlice);
    slider5->SetValueLimits(0, 5);

    auto slider6 = std::make_shared<FloatSlider>(context);
    slider6->SetAlignmentX(ItemAlignment::Stretch);
    slider6->SetBackgroundTexture(texTractbar);
    slider6->SetCursorTexture(texTractbarCursor);
    slider6->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider6->SetNineSlice(sliderNineSlice);
    slider6->SetValueLimits(0.0f, 5.0f);

    auto slider7 = std::make_shared<FloatSliderAndValueLabel>(context);
    slider7->SetAlignmentX(ItemAlignment::Stretch);
    slider7->SetBackgroundTexture(texTractbar);
    slider7->SetCursorTexture(texTractbarCursor);
    slider7->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider7->SetNineSlice(sliderNineSlice);
    slider7->SetValueLimits(0.0f, 5.0f);

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<StackLayout>(context);
    stackLayout->SetAlignmentY(ItemAlignment::Near);
    stackLayout->AddChild(labelSliders);
    stackLayout->AddChild(m_slider1);
    stackLayout->AddChild(m_slider2);
    stackLayout->AddChild(slider3);
    stackLayout->AddChild(slider4);
    stackLayout->AddChild(slider5);
    stackLayout->AddChild(slider6);
    stackLayout->AddChild(slider7);
    return stackLayout;
  }


  std::shared_ptr<BaseWindow> Shared::CreateStack4(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture)
  {
    AtlasTexture2D texCheckBox1C = m_uiExtension->GetAtlasTexture2D("CheckBox1C");
    AtlasTexture2D texCheckBox1U = m_uiExtension->GetAtlasTexture2D("CheckBox1U");
    AtlasTexture2D texCheckBox2C = m_uiExtension->GetAtlasTexture2D("CheckBox2C");
    AtlasTexture2D texCheckBox2U = m_uiExtension->GetAtlasTexture2D("CheckBox2U");

    auto labelSliders = std::make_shared<Label>(context);
    labelSliders->SetContent("CheckBox:");

    auto checkBox1 = std::make_shared<CheckBox>(context);
    checkBox1->SetAlignmentX(ItemAlignment::Near);
    checkBox1->SetAlignmentY(ItemAlignment::Center);
    checkBox1->SetText("Hello");
    checkBox1->SetCheckedTexture(texCheckBox1C);
    checkBox1->SetUncheckedTexture(texCheckBox1U);

    auto checkBox2 = std::make_shared<CheckBox>(context);
    checkBox2->SetAlignmentX(ItemAlignment::Near);
    checkBox2->SetAlignmentY(ItemAlignment::Center);
    checkBox2->SetText("World");
    checkBox2->SetCheckedTexture(texCheckBox2C);
    checkBox2->SetUncheckedTexture(texCheckBox2U);

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<StackLayout>(context);
    stackLayout->SetAlignmentY(ItemAlignment::Near);
    stackLayout->AddChild(labelSliders);
    stackLayout->AddChild(checkBox1);
    stackLayout->AddChild(checkBox2);
    return stackLayout;
  }
}
