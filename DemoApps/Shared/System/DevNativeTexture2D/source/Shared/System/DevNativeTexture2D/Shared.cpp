/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <Shared/System/DevNativeTexture2D/Shared.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/ICustomSpriteResourceManager.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    std::shared_ptr<UI::Label> CreateLabel(const std::shared_ptr<UI::WindowContext>& context, const std::string& text, const UI::ItemAlignment alignX,
                                           const UI::ItemAlignment alignY)
    {
      auto label = std::make_shared<UI::Label>(context);
      label->SetContent(text);
      label->SetAlignmentX(alignX);
      label->SetAlignmentY(alignY);
      return label;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
  {
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    FSLLOG3_INFO("Preparing textures");
    {
      m_testBitmaps.Image0 = contentManager->ReadBitmap("Text2/Test64x64_240dpi.png", PixelFormat::R8G8B8A8_UNORM);
      m_testBitmaps.Image1 = contentManager->ReadBitmap("Text2/Test64x64_320dpi.png", PixelFormat::R8G8B8A8_UNORM);
      m_testBitmaps.ImageNum1 = contentManager->ReadBitmap("TestText/Premultiplied/1.png", PixelFormat::R8G8B8A8_UNORM);
      m_testBitmaps.ImageNum2 = contentManager->ReadBitmap("TestText/Premultiplied/2.png", PixelFormat::R8G8B8A8_UNORM);
      m_testBitmaps.ImageNum3 = contentManager->ReadBitmap("TestText/Premultiplied/3.png", PixelFormat::R8G8B8A8_UNORM);
      m_testBitmaps.ImageNum4 = contentManager->ReadBitmap("TestText/Premultiplied/4.png", PixelFormat::R8G8B8A8_UNORM);
    }

    FSLLOG3_INFO("Preparing UI");

    {    // Build a simple UI

      auto windowContext = m_uiExtension->GetContext();
      auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
      auto& uiFactory = *uiControlFactory;


      m_uiRecord = CreateUI(windowContext, uiFactory);

      auto background = uiFactory.CreateLeftBar(m_uiRecord.MainLayout);

      auto imageLine = uiFactory.CreateImage(uiFactory.GetResources().GetDividerNineSliceSprite());
      imageLine->SetAlignmentX(UI::ItemAlignment::Stretch);
      imageLine->SetAlignmentY(UI::ItemAlignment::Stretch);
      imageLine->SetRotateImageCW(true);

      m_test0 = std::make_shared<UI::WrapLayout>(windowContext);
      m_test0->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
      m_test0->SetAlignmentX(UI::ItemAlignment::Center);
      m_test1 = std::make_shared<UI::WrapLayout>(windowContext);
      m_test1->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
      m_test1->SetAlignmentX(UI::ItemAlignment::Center);

      // Create the root layout and add it to the window manager
      m_mainLayout = std::make_shared<UI::GridLayout>(windowContext);
      m_mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
      m_mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
      m_mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
      m_mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto, 1.0f));
      m_mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
      m_mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
      m_mainLayout->AddChild(background, 0, 0);
      m_mainLayout->AddChild(m_test0, 1, 0);
      m_mainLayout->AddChild(imageLine, 2, 0);
      m_mainLayout->AddChild(m_test1, 3, 0);

      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(m_mainLayout);
    }
    FSLLOG3_INFO("Ready");
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_uiRecord.ButtonAddFront)
    {
      AddFrontTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.ButtonAddBack)
    {
      AddBackTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.ButtonRemoveFront)
    {
      RemoveFrontTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.ButtonRemoveBack)
    {
      RemoveBackTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.ButtonClear)
    {
      ClearTestRecords();
    }
    else if (theEvent->GetSource() == m_uiRecord.Button2AddFront)
    {
      AddDynamicFrontTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.Button2AddBack)
    {
      AddDynamicBackTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.Button2RemoveFront)
    {
      RemoveDynamicFrontTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.Button2RemoveBack)
    {
      RemoveDynamicBackTestRecord();
    }
    else if (theEvent->GetSource() == m_uiRecord.Button2Clear)
    {
      ClearDynamicTestRecords();
    }
    else if (theEvent->GetSource() == m_uiRecord.ButtonClearAll)
    {
      ClearTestRecords();
      ClearDynamicTestRecords();
    }
  }

  void Shared::Update()
  {
    ++m_frameCount;
    if (m_frameCount > 10u)
    {
      m_frameCount = 0u;
      for (auto& rEntry : m_dynamicTestRecords)
      {
        rEntry.ImageIndex = (rEntry.ImageIndex + 1) & 3;
        rEntry.Texture.SetData(GetDynamicBitmap(rEntry.ImageIndex), Texture2DFilterHint::Smooth);
      }
    }
  }

  void Shared::Draw()
  {
    m_uiExtension->Draw();
  }

  Shared::TestRecord Shared::CreateTestRecord(const Bitmap& bitmap)
  {
    auto windowContext = m_uiExtension->GetContext();

    Texture2D texture(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);
    ICustomSpriteResourceManager& resManager = m_uiExtension->GetCustomSpriteResourceManager();
    auto contentSprite = resManager.CreateCustomTextureSprite(texture.GetNative(), BlendState::AlphaBlend);

    auto image = std::make_shared<UI::Image>(windowContext);
    image->SetContent(contentSprite);
    image->SetAlignmentX(UI::ItemAlignment::Center);
    image->SetAlignmentY(UI::ItemAlignment::Center);

    return TestRecord(texture, std::move(image));
  }

  void Shared::AddFrontTestRecord()
  {
    auto testRecord = CreateTestRecord((m_testRecords.size() & 1) == 0 ? m_testBitmaps.Image0 : m_testBitmaps.Image1);
    // TODO: For now we add it to the back in the list since the UI doesn't support adding it at the front
    m_test0->AddChild(testRecord.Image);
    m_testRecords.push_front(std::move(testRecord));
  }

  void Shared::RemoveFrontTestRecord()
  {
    if (m_testRecords.empty())
    {
      return;
    }
    m_test0->RemoveChild(m_testRecords.front().Image);
    m_testRecords.pop_front();
  }

  void Shared::AddBackTestRecord()
  {
    auto testRecord = CreateTestRecord((m_testRecords.size() & 1) == 0 ? m_testBitmaps.Image0 : m_testBitmaps.Image1);
    m_test0->AddChild(testRecord.Image);
    m_testRecords.push_back(std::move(testRecord));
  }

  void Shared::RemoveBackTestRecord()
  {
    if (m_testRecords.empty())
    {
      return;
    }
    m_test0->RemoveChild(m_testRecords.back().Image);
    m_testRecords.pop_back();
  }

  void Shared::ClearTestRecords()
  {
    while (!m_testRecords.empty())
    {
      RemoveBackTestRecord();
    }
  }

  const Bitmap& Shared::GetDynamicBitmap(int imageIndex)
  {
    switch (imageIndex)
    {
    case 0:
      return m_testBitmaps.ImageNum1;
    case 1:
      return m_testBitmaps.ImageNum2;
    case 2:
      return m_testBitmaps.ImageNum3;
    default:
      return m_testBitmaps.ImageNum4;
    }
  }


  Shared::DynamicTestRecord Shared::CreateDynamicTestRecord()
  {
    auto windowContext = m_uiExtension->GetContext();

    auto imageIndex = static_cast<uint32_t>(m_dynamicTestRecords.size() & 3);
    const auto& bitmap = GetDynamicBitmap(imageIndex);

    DynamicTexture2D texture(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);
    ICustomSpriteResourceManager& resManager = m_uiExtension->GetCustomSpriteResourceManager();
    auto contentSprite = resManager.CreateCustomTextureSprite(texture.GetNative(), BlendState::AlphaBlend);

    auto image = std::make_shared<UI::Image>(windowContext);
    image->SetContent(contentSprite);
    image->SetAlignmentX(UI::ItemAlignment::Center);
    image->SetAlignmentY(UI::ItemAlignment::Center);

    return DynamicTestRecord(texture, std::move(image), imageIndex);
  }

  void Shared::AddDynamicFrontTestRecord()
  {
    auto testRecord = CreateDynamicTestRecord();
    // TODO: For now we add it to the back in the list since the UI doesn't support adding it at the front
    m_test0->AddChild(testRecord.Image);
    m_dynamicTestRecords.push_front(std::move(testRecord));
  }

  void Shared::RemoveDynamicFrontTestRecord()
  {
    if (m_dynamicTestRecords.empty())
    {
      return;
    }
    m_test0->RemoveChild(m_dynamicTestRecords.front().Image);
    m_dynamicTestRecords.pop_front();
  }

  void Shared::AddDynamicBackTestRecord()
  {
    auto testRecord = CreateDynamicTestRecord();
    m_test0->AddChild(testRecord.Image);
    m_dynamicTestRecords.push_back(std::move(testRecord));
  }

  void Shared::RemoveDynamicBackTestRecord()
  {
    if (m_dynamicTestRecords.empty())
    {
      return;
    }
    m_test0->RemoveChild(m_dynamicTestRecords.back().Image);
    m_dynamicTestRecords.pop_back();
  }

  void Shared::ClearDynamicTestRecords()
  {
    while (!m_dynamicTestRecords.empty())
    {
      RemoveDynamicBackTestRecord();
    }
  }

  Shared::UIRecord Shared::CreateUI(const std::shared_ptr<UI::WindowContext>& context, UI::Theme::IThemeControlFactory& rUIFactory)
  {
    auto labelResPxCaption = CreateLabel(context, "Resolution: ", UI::ItemAlignment::Near, UI::ItemAlignment::Center);
    auto labelDpiCaption = CreateLabel(context, "Actual DPI: ", UI::ItemAlignment::Near, UI::ItemAlignment::Center);

    auto buttonType = UI::Theme::ButtonType::Contained;

    auto buttonAddFront = rUIFactory.CreateTextButton(buttonType, "Add front");
    buttonAddFront->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto buttonRemoveFront = rUIFactory.CreateTextButton(buttonType, "Remove front");
    buttonRemoveFront->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto buttonAddBack = rUIFactory.CreateTextButton(buttonType, "Add back");
    buttonAddBack->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto buttonRemoveBack = rUIFactory.CreateTextButton(buttonType, "Remove back");
    buttonRemoveBack->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto buttonClear = rUIFactory.CreateTextButton(buttonType, "Clear");
    buttonClear->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto button2AddFront = rUIFactory.CreateTextButton(buttonType, "Add D front");
    button2AddFront->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto button2RemoveFront = rUIFactory.CreateTextButton(buttonType, "Remove D front");
    button2RemoveFront->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto button2AddBack = rUIFactory.CreateTextButton(buttonType, "Add D back");
    button2AddBack->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto button2RemoveBack = rUIFactory.CreateTextButton(buttonType, "Remove D back");
    button2RemoveBack->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto button2Clear = rUIFactory.CreateTextButton(buttonType, "Clear D");
    button2Clear->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto buttonClearAll = rUIFactory.CreateTextButton(buttonType, "Clear All");
    buttonClearAll->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto stackLayout = std::make_shared<UI::StackLayout>(context);
    stackLayout->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    stackLayout->AddChild(buttonAddFront);
    stackLayout->AddChild(buttonRemoveFront);
    stackLayout->AddChild(buttonAddBack);
    stackLayout->AddChild(buttonRemoveBack);
    stackLayout->AddChild(buttonClear);
    stackLayout->AddChild(button2AddFront);
    stackLayout->AddChild(button2RemoveFront);
    stackLayout->AddChild(button2AddBack);
    stackLayout->AddChild(button2RemoveBack);
    stackLayout->AddChild(button2Clear);
    stackLayout->AddChild(buttonClearAll);

    auto gridLayout = std::make_shared<UI::GridLayout>(context);
    gridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 0.10f));
    gridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    gridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 0.10f));
    gridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout->AddChild(stackLayout, 1, 0);
    gridLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    gridLayout->SetAlignmentY(UI::ItemAlignment::Stretch);

    UIRecord record;
    record.ButtonAddFront = buttonAddFront;
    record.ButtonAddBack = buttonAddBack;
    record.ButtonRemoveFront = buttonRemoveFront;
    record.ButtonRemoveBack = buttonRemoveBack;
    record.ButtonClear = buttonClear;
    record.Button2AddFront = button2AddFront;
    record.Button2AddBack = button2AddBack;
    record.Button2RemoveFront = button2RemoveFront;
    record.Button2RemoveBack = button2RemoveBack;
    record.Button2Clear = button2Clear;
    record.ButtonClearAll = buttonClearAll;
    record.MainLayout = gridLayout;
    return record;
  }
}
