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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Dp/TypeConverter.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslSimpleUI/App/UIAppConfig.hpp>
#include <FslSimpleUI/App/UIAppTextureResourceCreationInfo.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Button.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/LabelButton.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <Shared/UI/SimpleUI101/Shared.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr SpriteMaterialId CustomSpriteAtlasMaterialId(UIAppConfig::MaterialId::CustomSpriteOffset);
    }

    std::shared_ptr<UI::Label> CreateLabel(const std::shared_ptr<UI::WindowContext>& context, const std::string& text, const UI::ItemAlignment alignX,
                                           const UI::ItemAlignment alignY, const std::shared_ptr<SpriteFont>& font)
    {
      auto label = std::make_shared<UI::Label>(context);
      label->SetContent(text);
      label->SetAlignmentX(alignX);
      label->SetAlignmentY(alignY);
      label->SetFont(font);
      return label;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "TextureAtlas/MainAtlas"))    // Prepare the extension
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
  {
    //
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    auto windowContext = m_uiExtension->GetContext();
    const UIAppTextureHandle hTexture = m_uiExtension->GetAtlasTextureHandle();

    constexpr auto CustomMaterialId = LocalConfig::CustomSpriteAtlasMaterialId;

    m_uiExtension->AddSpriteMaterial(CustomMaterialId, hTexture, BlendState::AlphaBlend);

    auto& rSpriteManager = m_uiExtension->GetSpriteResourceManager();
    std::shared_ptr<ImageSprite> sprite1 = rSpriteManager.CreateImageSprite(CustomMaterialId, "RectTex");
    auto fontDefault = rSpriteManager.CreateSpriteFont(CustomMaterialId, "Font.nbf", SpriteFontConfig(true));
    auto fontSmall = rSpriteManager.CreateSpriteFont(CustomMaterialId, "SmallFont.nbf", SpriteFontConfig(true));

    auto stackLayout1 = CreateStack1(windowContext, fontSmall, sprite1);
    auto stackLayout2 = CreateStack2(windowContext, sprite1);
    auto stackLayout3 = CreateStack3(windowContext, CustomMaterialId, fontDefault);
    auto stackLayout4 = CreateStack4(windowContext, CustomMaterialId);

    std::shared_ptr<NineSliceSprite> ninesliceBackgroundSprite = rSpriteManager.CreateNineSliceSprite(CustomMaterialId, "Background9R");

    auto background = std::make_shared<UI::Background>(windowContext);
    background->SetContent(stackLayout2);
    background->SetBackground(ninesliceBackgroundSprite);

    // Create a stack which contains the other stacks
    auto stackLayout = std::make_shared<UI::StackLayout>(windowContext);
    stackLayout->SetAlignmentX(UI::ItemAlignment::Center);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Center);
    stackLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    stackLayout->AddChild(stackLayout1);
    // stackLayout->AddChild(stackLayout2);
    // stackLayout->AddChild(m_complexButton);
    stackLayout->AddChild(background);
    stackLayout->AddChild(stackLayout3);
    stackLayout->AddChild(stackLayout4);


    // Create labels for the corners
    auto labelCorner1 = CreateLabel(windowContext, "TopLeft", UI::ItemAlignment::Near, UI::ItemAlignment::Near, fontSmall);
    auto labelCorner2 = CreateLabel(windowContext, "TopRight", UI::ItemAlignment::Far, UI::ItemAlignment::Near, fontSmall);
    auto labelCorner3 = CreateLabel(windowContext, "BottomLeft", UI::ItemAlignment::Near, UI::ItemAlignment::Far, fontSmall);
    auto labelCorner4 = CreateLabel(windowContext, "BottomRight", UI::ItemAlignment::Far, UI::ItemAlignment::Far, fontSmall);

    // Create labels for the border centers
    auto labelCenter1 = CreateLabel(windowContext, "TopCenter", UI::ItemAlignment::Center, UI::ItemAlignment::Near, fontSmall);
    auto labelCenter2 = CreateLabel(windowContext, "BottomCenter", UI::ItemAlignment::Center, UI::ItemAlignment::Far, fontSmall);
    auto labelCenter3 = CreateLabel(windowContext, "LeftCenter", UI::ItemAlignment::Near, UI::ItemAlignment::Center, fontSmall);
    auto labelCenter4 = CreateLabel(windowContext, "RightCenter", UI::ItemAlignment::Far, UI::ItemAlignment::Center, fontSmall);

    // Create the root layout and add it to the window manager
    m_fillLayout = std::make_shared<UI::FillLayout>(windowContext);
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


  void Shared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
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


  std::shared_ptr<UI::BaseWindow> Shared::CreateStack1(const std::shared_ptr<UI::WindowContext>& context,
                                                       const std::shared_ptr<SpriteFont>& fontSmall, const std::shared_ptr<ImageSprite>& texImage)
  {
    // Create items that we want to add to a stack layout
    auto label1 = std::make_shared<UI::Label>(context);
    auto label2 = std::make_shared<UI::Label>(context);
    auto label3 = std::make_shared<UI::Label>(context);
    auto labelSmall = std::make_shared<UI::Label>(context);
    label1->SetContent("1");
    label2->SetContent("2");
    label3->SetContent("3");
    label1->SetAlignmentX(UI::ItemAlignment::Near);
    label2->SetAlignmentX(UI::ItemAlignment::Center);
    label3->SetAlignmentX(UI::ItemAlignment::Far);
    labelSmall->SetContent("Hello world");
    labelSmall->SetFont(fontSmall);
    labelSmall->SetAlignmentX(UI::ItemAlignment::Center);

    auto labelValue1 = std::make_shared<UI::FmtValueLabel<int32_t>>(context);
    labelValue1->SetContent(1337);
    labelValue1->SetAlignmentX(UI::ItemAlignment::Center);

    auto labelValue2 = std::make_shared<UI::FmtValueLabel<float>>(context);
    labelValue2->SetContent(3.14f);
    labelValue2->SetAlignmentX(UI::ItemAlignment::Center);

    m_button1 = std::make_shared<UI::LabelButton>(context);
    m_button1->SetAlignmentX(UI::ItemAlignment::Center);
    m_button1->SetContent("Button1");

    m_button2 = std::make_shared<UI::SimpleImageButton>(context);
    m_button2->SetAlignmentX(UI::ItemAlignment::Center);
    m_button2->SetContent(texImage);

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<UI::StackLayout>(context);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Center);
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


  std::shared_ptr<UI::BaseWindow> Shared::CreateStack2(const std::shared_ptr<UI::WindowContext>& context,
                                                       const std::shared_ptr<ImageSprite>& imageSprite)
  {
    auto label = std::make_shared<UI::Label>(context);
    label->SetContent("Images:");

    // Create items that we want to add to a another stack layout
    auto image1 = std::make_shared<UI::Image>(context);
    auto image2 = std::make_shared<UI::Image>(context);
    auto image3 = std::make_shared<UI::Image>(context);
    auto image4N = std::make_shared<UI::Image>(context);
    auto image4C = std::make_shared<UI::Image>(context);
    auto image4F = std::make_shared<UI::Image>(context);
    auto image4S = std::make_shared<UI::Image>(context);
    image1->SetContent(imageSprite);
    image2->SetContent(imageSprite);
    image3->SetContent(imageSprite);
    image4N->SetContent(imageSprite);
    image4C->SetContent(imageSprite);
    image4F->SetContent(imageSprite);
    image4S->SetContent(imageSprite);
    image1->SetScalePolicy(UI::ItemScalePolicy::Fit);
    image2->SetScalePolicy(UI::ItemScalePolicy::Fit);
    image3->SetScalePolicy(UI::ItemScalePolicy::Fit);
    image4N->SetScalePolicy(UI::ItemScalePolicy::FitKeepAR);
    image4C->SetScalePolicy(UI::ItemScalePolicy::FitKeepAR);
    image4F->SetScalePolicy(UI::ItemScalePolicy::FitKeepAR);
    image4S->SetScalePolicy(UI::ItemScalePolicy::Fit);
    image1->SetAlignmentX(UI::ItemAlignment::Center);
    image2->SetAlignmentX(UI::ItemAlignment::Center);
    image3->SetAlignmentX(UI::ItemAlignment::Center);
    image4N->SetAlignmentX(UI::ItemAlignment::Near);
    image4C->SetAlignmentX(UI::ItemAlignment::Center);
    image4F->SetAlignmentX(UI::ItemAlignment::Far);
    image4S->SetAlignmentX(UI::ItemAlignment::Stretch);

    const auto widthDp = TypeConverter::To<DpValue>(imageSprite->GetImageInfo().ExtentDp.Width);
    const auto heightDp = TypeConverter::To<DpValue>(imageSprite->GetImageInfo().ExtentDp.Height);
    image2->SetWidth(UI::DpLayoutSize1D(widthDp));
    image2->SetHeight(UI::DpLayoutSize1D(heightDp / DpValue(2)));
    image3->SetWidth(UI::DpLayoutSize1D(widthDp / DpValue(2)));
    image3->SetHeight(UI::DpLayoutSize1D(heightDp));
    image4N->SetHeight(UI::DpLayoutSize1D(heightDp / DpValue(4)));
    image4C->SetHeight(UI::DpLayoutSize1D(heightDp / DpValue(4)));
    image4F->SetHeight(UI::DpLayoutSize1D(heightDp / DpValue(4)));
    image4S->SetHeight(UI::DpLayoutSize1D(heightDp / DpValue(4)));

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<UI::StackLayout>(context);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Center);
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


  std::shared_ptr<UI::BaseWindow> Shared::CreateStack3(const std::shared_ptr<UI::WindowContext>& context,
                                                       const SpriteMaterialId customSpriteMaterialId, const std::shared_ptr<SpriteFont>& fontDefault)
  {
    auto& rSpriteManager = m_uiExtension->GetSpriteResourceManager();
    std::shared_ptr<NineSliceSprite> trackbarSprite = rSpriteManager.CreateNineSliceSprite(customSpriteMaterialId, "Slider");
    std::shared_ptr<ImageSprite> trackbarCursorSprite = rSpriteManager.CreateImageSprite(customSpriteMaterialId, "SliderCursor");

    constexpr auto CursorSizeDp = DpSize2D::Create(18, 50);
    constexpr auto CursorOriginDp = DpPoint2::Create(16, 32);

    auto labelSliders = std::make_shared<UI::Label>(context);
    labelSliders->SetContent("Sliders:");
    labelSliders->SetWidth(UI::DpLayoutSize1D(DpValue(200)));

    m_slider1 = std::make_shared<UI::Slider<int32_t>>(context);
    m_slider1->SetAlignmentX(UI::ItemAlignment::Center);
    m_slider1->SetAlignmentY(UI::ItemAlignment::Center);
    m_slider1->SetBackgroundSprite(trackbarSprite);
    m_slider1->SetCursorSprite(trackbarCursorSprite);
    m_slider1->SetCursorSize(CursorSizeDp);
    m_slider1->SetCursorOrigin(CursorOriginDp);

    m_slider2 = std::make_shared<UI::Slider<int32_t>>(context);
    m_slider2->SetAlignmentX(UI::ItemAlignment::Stretch);
    m_slider2->SetAlignmentY(UI::ItemAlignment::Center);
    m_slider2->SetBackgroundSprite(trackbarSprite);
    m_slider2->SetCursorSprite(trackbarCursorSprite);
    m_slider2->SetCursorSize(CursorSizeDp);
    m_slider2->SetCursorOrigin(CursorOriginDp);

    auto slider3 = std::make_shared<UI::SliderAndFmtValueLabel<int32_t>>(context);
    slider3->SetFont(fontDefault);
    slider3->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider3->SetBackgroundSprite(trackbarSprite);
    slider3->SetCursorSprite(trackbarCursorSprite);
    slider3->SetCursorSize(CursorSizeDp);
    slider3->SetCursorOrigin(CursorOriginDp);

    auto slider4 = std::make_shared<UI::Slider<uint32_t>>(context);
    slider4->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider4->SetBackgroundSprite(trackbarSprite);
    slider4->SetCursorSprite(trackbarCursorSprite);
    slider4->SetRange(0, 5);
    slider4->SetCursorSize(CursorSizeDp);
    slider4->SetCursorOrigin(CursorOriginDp);

    auto slider5 = std::make_shared<UI::SliderAndFmtValueLabel<int32_t>>(context);
    slider5->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider5->SetBackgroundSprite(trackbarSprite);
    slider5->SetCursorSprite(trackbarCursorSprite);
    slider5->SetRange(0, 5);
    slider5->SetCursorSize(CursorSizeDp);
    slider5->SetCursorOrigin(CursorOriginDp);

    auto slider6 = std::make_shared<UI::Slider<float>>(context);
    slider6->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider6->SetBackgroundSprite(trackbarSprite);
    slider6->SetCursorSprite(trackbarCursorSprite);
    slider6->SetRange(0.0f, 5.0f);
    slider6->SetCursorSize(CursorSizeDp);
    slider6->SetCursorOrigin(CursorOriginDp);

    auto slider7 = std::make_shared<UI::SliderAndFmtValueLabel<float>>(context);
    slider7->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider7->SetBackgroundSprite(trackbarSprite);
    slider7->SetCursorSprite(trackbarCursorSprite);
    slider7->SetRange(0.0f, 5.0f);
    slider7->SetCursorSize(CursorSizeDp);
    slider7->SetCursorOrigin(CursorOriginDp);

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<UI::StackLayout>(context);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Near);
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


  std::shared_ptr<UI::BaseWindow> Shared::CreateStack4(const std::shared_ptr<UI::WindowContext>& context,
                                                       const SpriteMaterialId customSpriteMaterialId)
  {
    auto& rSpriteManager = m_uiExtension->GetSpriteResourceManager();
    std::shared_ptr<ImageSprite> texCheckBox1C = rSpriteManager.CreateImageSprite(customSpriteMaterialId, "CheckBox1C");
    std::shared_ptr<ImageSprite> texCheckBox1U = rSpriteManager.CreateImageSprite(customSpriteMaterialId, "CheckBox1U");
    std::shared_ptr<ImageSprite> texCheckBox2C = rSpriteManager.CreateImageSprite(customSpriteMaterialId, "CheckBox2C");
    std::shared_ptr<ImageSprite> texCheckBox2U = rSpriteManager.CreateImageSprite(customSpriteMaterialId, "CheckBox2U");

    auto labelSliders = std::make_shared<UI::Label>(context);
    labelSliders->SetContent("CheckBox:");

    auto cursorPosion1 = TypeConverter::To<DpPoint2>(texCheckBox1U->GetImageInfo().ExtentDp / DpValueU(2));
    auto cursorPosion2 = TypeConverter::To<DpPoint2>(texCheckBox2U->GetImageInfo().ExtentDp / DpValueU(2));

    auto checkBox1 = std::make_shared<UI::CheckBox>(context);
    checkBox1->SetAlignmentX(UI::ItemAlignment::Near);
    checkBox1->SetAlignmentY(UI::ItemAlignment::Center);
    checkBox1->SetText("Hello");
    checkBox1->SetCheckedSprite(texCheckBox1C);
    checkBox1->SetUncheckedSprite(texCheckBox1U);
    checkBox1->SetCursorCheckedPosition(cursorPosion1);
    checkBox1->SetCursorUncheckedPosition(cursorPosion1);
    checkBox1->FinishAnimation();

    auto checkBox2 = std::make_shared<UI::CheckBox>(context);
    checkBox2->SetAlignmentX(UI::ItemAlignment::Near);
    checkBox2->SetAlignmentY(UI::ItemAlignment::Center);
    checkBox2->SetText("World");
    checkBox2->SetCheckedSprite(texCheckBox2C);
    checkBox2->SetUncheckedSprite(texCheckBox2U);
    checkBox2->SetCursorCheckedPosition(cursorPosion2);
    checkBox2->SetCursorUncheckedPosition(cursorPosion2);
    checkBox2->FinishAnimation();

    // Create a stack layout and add various items to it
    auto stackLayout = std::make_shared<UI::StackLayout>(context);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Near);
    stackLayout->AddChild(labelSliders);
    stackLayout->AddChild(checkBox1);
    stackLayout->AddChild(checkBox2);
    return stackLayout;
  }
}
