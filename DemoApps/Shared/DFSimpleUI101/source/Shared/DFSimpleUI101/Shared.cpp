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
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
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
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/LabelButton.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Control/SliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>

#include <cassert>

namespace Fsl
{
  using namespace UI;

  namespace
  {
    std::shared_ptr<Label> CreateLabel(const std::shared_ptr<WindowContext>& context, const std::string& text, const ItemAlignment alignX, const ItemAlignment alignY, const std::shared_ptr<AtlasFont>& font)
    {
      std::shared_ptr<Label> label(new Label(context));
      label->SetContent(text);
      label->SetAlignmentX(alignX);
      label->SetAlignmentY(alignY);
      label->SetFont(font);
      return label;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiManager(config.ScreenResolution)
    , m_uiEventListener(this)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_context()
    , m_fillLayout()
  {
    m_uiManager.RegisterEventListener(m_uiEventListener.GetListener());

    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    // FIX: way too cumbersome to configure a font
    BasicTextureAtlas atlas;
    contentManager->Read(atlas, "MainAtlas.bta");

    const auto winMgr = m_uiManager.GetWindowManager();

    Bitmap fontBitmap;
    contentManager->Read(fontBitmap, "MainAtlas.png", PixelFormat::R8G8B8A8_UNORM);
    Texture2D atlasTexture(m_graphics->GetNativeGraphics(), fontBitmap, Texture2DFilterHint::Smooth);

    AtlasTexture2D tex1(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "RectTex"));
    AtlasTexture2D texBackground(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Background9R"));

    BasicFontKerning fontKerning;
    BasicFontKerning smallFontkerning;
    contentManager->Read(fontKerning, "MainAtlasFont.fbk");
    contentManager->Read(smallFontkerning, "MainAtlasSmallFont.fbk");

    TextureAtlasBitmapFont bitmapFont(atlas, fontKerning);
    TextureAtlasBitmapFont bitmapFontSmall(atlas, smallFontkerning);

    std::shared_ptr<AtlasFont> font(new AtlasFont(atlasTexture, bitmapFont));
    std::shared_ptr<AtlasFont> fontSmall(new AtlasFont(atlasTexture, bitmapFontSmall));

    // Setup the window context that is given to all windows
    m_context.reset(new WindowContext(winMgr, m_uiManager.GetEventSender(), m_graphics->GetNativeBatch2D(), font));


    //m_complexButton.reset(new Button(m_context));
    //m_complexButton->SetContent(stackLayout2);


    auto stackLayout1 = CreateStack1(atlasTexture, atlas, fontSmall, tex1);
    auto stackLayout2 = CreateStack2(atlasTexture, atlas, tex1);
    auto stackLayout3 = CreateStack3(atlasTexture, atlas);
    auto stackLayout4 = CreateStack4(atlasTexture, atlas);


    NineSlice backgroundNineSlice(32, 32, 32, 32);
    std::shared_ptr<Background9Slice> background(new Background9Slice(m_context));
    background->SetContent(stackLayout2);
    background->SetBackground(texBackground);
    background->SetNineSlice(backgroundNineSlice);
    background->SetPadding(ThicknessF(32, 32, 32, 32));

    // Create a stack which contains the other stacks
    std::shared_ptr<StackLayout> stackLayout(new StackLayout(m_context));
    stackLayout->SetAlignmentX(ItemAlignment::Center);
    stackLayout->SetAlignmentY(ItemAlignment::Center);
    stackLayout->SetLayoutOrientation(LayoutOrientation::Horizontal);
    stackLayout->AddChild(stackLayout1);
    //stackLayout->AddChild(stackLayout2);
    //stackLayout->AddChild(m_complexButton);
    stackLayout->AddChild(background);
    stackLayout->AddChild(stackLayout3);
    stackLayout->AddChild(stackLayout4);


    // Create labels for the corners
    auto labelCorner1 = CreateLabel(m_context, "TopLeft", ItemAlignment::Near, ItemAlignment::Near, fontSmall);
    auto labelCorner2 = CreateLabel(m_context, "TopRight", ItemAlignment::Far, ItemAlignment::Near, fontSmall);
    auto labelCorner3 = CreateLabel(m_context, "BottomLeft", ItemAlignment::Near, ItemAlignment::Far, fontSmall);
    auto labelCorner4 = CreateLabel(m_context, "BottomRight", ItemAlignment::Far, ItemAlignment::Far, fontSmall);

    // Create labels for the border centers
    auto labelCenter1 = CreateLabel(m_context, "TopCenter", ItemAlignment::Center, ItemAlignment::Near, fontSmall);
    auto labelCenter2 = CreateLabel(m_context, "BottomCenter", ItemAlignment::Center, ItemAlignment::Far, fontSmall);
    auto labelCenter3 = CreateLabel(m_context, "LeftCenter", ItemAlignment::Near, ItemAlignment::Center, fontSmall);
    auto labelCenter4 = CreateLabel(m_context, "RightCenter", ItemAlignment::Far, ItemAlignment::Center, fontSmall);

    // Create the root layout and add it to the window manager
    m_fillLayout.reset(new FillLayout(m_context));
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
    winMgr->Add(m_fillLayout);
  }


  Shared::~Shared()
  {
  }


  void Shared::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_button1)
    {
      FSLLOG("Button1 selected");
    }
    else if (theEvent->GetSource() == m_button2)
    {
      FSLLOG("Button2 selected");
    }
    else if (theEvent->GetSource() == m_complexButton)
    {
      FSLLOG("Complex button selected");
    }
  }


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
  }


  void Shared::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (!m_uiManager.SendMouseButtonEvent(event))
    {
      FSLLOG("Mouse did not hit a input enabled control");
    }
  }


  void Shared::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (!m_uiManager.SendMouseMoveEvent(event))
    {
    }
  }


  void Shared::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
  }


  void Shared::Resized(const Point2& size)
  {
    m_uiManager.Resized(size);
  }


  void Shared::FixedUpdate(const DemoTime& demoTime)
  {
    m_uiManager.FixedUpdate(demoTime);
  }


  void Shared::Update(const DemoTime& demoTime)
  {
    m_uiManager.Update(demoTime);
  }


  void Shared::Draw()
  {
    auto batch2D = m_context->Batch2D;
    batch2D->Begin();
    m_uiManager.Draw();
    batch2D->End();
  }


  std::shared_ptr<BaseWindow> Shared::CreateStack1(const Texture2D& atlasTexture, const ITextureAtlas& atlas, const std::shared_ptr<AtlasFont>& fontSmall, const AtlasTexture2D& texImage)
  {
    // Create items that we want to add to a stack layout
    std::shared_ptr<Label> label1(new Label(m_context));
    std::shared_ptr<Label> label2(new Label(m_context));
    std::shared_ptr<Label> label3(new Label(m_context));
    std::shared_ptr<Label> labelSmall(new Label(m_context));
    label1->SetContent("1");
    label2->SetContent("2");
    label3->SetContent("3");
    label1->SetAlignmentX(ItemAlignment::Near);
    label2->SetAlignmentX(ItemAlignment::Center);
    label3->SetAlignmentX(ItemAlignment::Far);
    labelSmall->SetContent("Hello world");
    labelSmall->SetFont(fontSmall);
    labelSmall->SetAlignmentX(ItemAlignment::Center);

    m_button1.reset(new LabelButton(m_context));
    m_button1->SetAlignmentX(ItemAlignment::Center);
    m_button1->SetContent("Button1");

    m_button2.reset(new ImageButton(m_context));
    m_button2->SetAlignmentX(ItemAlignment::Center);
    m_button2->SetContent(texImage);

    // Create a stack layout and add various items to it
    std::shared_ptr<StackLayout> stackLayout(new StackLayout(m_context));
    stackLayout->SetAlignmentY(ItemAlignment::Center);
    stackLayout->AddChild(label1);
    stackLayout->AddChild(label2);
    stackLayout->AddChild(label3);
    stackLayout->AddChild(labelSmall);
    stackLayout->AddChild(m_button1);
    stackLayout->AddChild(m_button2);
    return stackLayout;
  }


  std::shared_ptr<BaseWindow> Shared::CreateStack2(const Texture2D& atlasTexture, const ITextureAtlas& atlas, const AtlasTexture2D& texImage)
  {
    std::shared_ptr<Label> label(new Label(m_context));
    label->SetContent("Images:");

    // Create items that we want to add to a another stack layout
    std::shared_ptr<Image> image1(new Image(m_context));
    std::shared_ptr<Image> image2(new Image(m_context));
    std::shared_ptr<Image> image3(new Image(m_context));
    std::shared_ptr<Image> image4N(new Image(m_context));
    std::shared_ptr<Image> image4C(new Image(m_context));
    std::shared_ptr<Image> image4F(new Image(m_context));
    std::shared_ptr<Image> image4S(new Image(m_context));
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
    std::shared_ptr<StackLayout> stackLayout(new StackLayout(m_context));
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


  std::shared_ptr<BaseWindow> Shared::CreateStack3(const Texture2D& atlasTexture, const ITextureAtlas& atlas)
  {
    AtlasTexture2D texTractbar(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Slider"));
    AtlasTexture2D texTractbarCursor(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "SliderCursor"));
    NineSlice sliderNineSlice(40, 0, 46, 0);


    std::shared_ptr<Label> labelSliders(new Label(m_context));
    labelSliders->SetContent("Sliders:");
    labelSliders->SetWidth(200);

    m_slider1.reset(new Slider(m_context));
    m_slider1->SetAlignmentX(ItemAlignment::Center);
    m_slider1->SetAlignmentY(ItemAlignment::Center);
    m_slider1->SetBackgroundTexture(texTractbar);
    m_slider1->SetCursorTexture(texTractbarCursor);
    m_slider1->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    m_slider1->SetNineSlice(sliderNineSlice);

    m_slider2.reset(new Slider(m_context));
    m_slider2->SetAlignmentX(ItemAlignment::Stretch);
    m_slider2->SetAlignmentY(ItemAlignment::Center);
    m_slider2->SetBackgroundTexture(texTractbar);
    m_slider2->SetCursorTexture(texTractbarCursor);
    m_slider2->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    m_slider2->SetNineSlice(sliderNineSlice);

    std::shared_ptr<SliderAndValueLabel> slider3(new SliderAndValueLabel(m_context));
    slider3->SetAlignmentX(ItemAlignment::Stretch);
    slider3->SetBackgroundTexture(texTractbar);
    slider3->SetCursorTexture(texTractbarCursor);
    slider3->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider3->SetNineSlice(sliderNineSlice);

    std::shared_ptr<Slider> slider4(new Slider(m_context));
    slider4->SetAlignmentX(ItemAlignment::Stretch);
    slider4->SetBackgroundTexture(texTractbar);
    slider4->SetCursorTexture(texTractbarCursor);
    slider4->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider4->SetNineSlice(sliderNineSlice);
    slider4->SetValueLimits(0, 5);

    std::shared_ptr<SliderAndValueLabel> slider5(new SliderAndValueLabel(m_context));
    slider5->SetAlignmentX(ItemAlignment::Stretch);
    slider5->SetBackgroundTexture(texTractbar);
    slider5->SetCursorTexture(texTractbarCursor);
    slider5->SetCursorPadding(ThicknessF(32, 0, 32, 0));
    slider5->SetNineSlice(sliderNineSlice);
    slider5->SetValueLimits(0, 5);

    // Create a stack layout and add various items to it
    std::shared_ptr<StackLayout> stackLayout(new StackLayout(m_context));
    stackLayout->SetAlignmentY(ItemAlignment::Near);
    stackLayout->AddChild(labelSliders);
    stackLayout->AddChild(m_slider1);
    stackLayout->AddChild(m_slider2);
    stackLayout->AddChild(slider3);
    stackLayout->AddChild(slider4);
    stackLayout->AddChild(slider5);
    return stackLayout;
  }


  std::shared_ptr<BaseWindow> Shared::CreateStack4(const Texture2D& atlasTexture, const ITextureAtlas& atlas)
  {
    AtlasTexture2D texCheckBox1C(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "CheckBox1C"));
    AtlasTexture2D texCheckBox1U(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "CheckBox1U"));
    AtlasTexture2D texCheckBox2C(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "CheckBox2C"));
    AtlasTexture2D texCheckBox2U(atlasTexture, TextureAtlasHelper::GetAtlasTextureInfo(atlas, "CheckBox2U"));


    std::shared_ptr<Label> labelSliders(new Label(m_context));
    labelSliders->SetContent("CheckBox:");

    std::shared_ptr<CheckBox> checkBox1(new CheckBox(m_context));
    checkBox1->SetAlignmentX(ItemAlignment::Near);
    checkBox1->SetAlignmentY(ItemAlignment::Center);
    checkBox1->SetText("Hello");
    checkBox1->SetCheckedTexture(texCheckBox1C);
    checkBox1->SetUncheckedTexture(texCheckBox1U);

    std::shared_ptr<CheckBox> checkBox2(new CheckBox(m_context));
    checkBox2->SetAlignmentX(ItemAlignment::Near);
    checkBox2->SetAlignmentY(ItemAlignment::Center);
    checkBox2->SetText("World");
    checkBox2->SetCheckedTexture(texCheckBox2C);
    checkBox2->SetUncheckedTexture(texCheckBox2U);

    // Create a stack layout and add various items to it
    std::shared_ptr<StackLayout> stackLayout(new StackLayout(m_context));
    stackLayout->SetAlignmentY(ItemAlignment::Near);
    stackLayout->AddChild(labelSliders);
    stackLayout->AddChild(checkBox1);
    stackLayout->AddChild(checkBox2);
    return stackLayout;
  }

}
