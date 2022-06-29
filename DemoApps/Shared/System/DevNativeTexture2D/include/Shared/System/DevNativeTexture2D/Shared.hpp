#ifndef SHARED_SYSTEM_DEVNATIVETEXTURE2D_SHARED_HPP
#define SHARED_SYSTEM_DEVNATIVETEXTURE2D_SHARED_HPP
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


#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslGraphics/Render/DynamicTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <deque>
#include <memory>
#include <utility>

namespace Fsl
{
  class AtlasFont;
  class AtlasTexture2D;
  class BaseTexture2D;
  class IGraphicsService;
  class KeyEvent;
  class MouseButtonEvent;
  class MouseMoveEvent;
  struct Point2;
  class ITextureAtlas;

  namespace UI
  {
    class BaseWindow;
    class Button;
    class LabelButton;
    class ImageButton;
    class StackLayout;
    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  class Shared : public UI::EventListener
  {
    struct TestRecord
    {
      Texture2D Texture;
      std::shared_ptr<UI::Image> Image;

      TestRecord() = default;
      TestRecord(const Texture2D& texture, std::shared_ptr<UI::Image> image)
        : Texture(texture)
        , Image(std::move(image))
      {
      }
    };

    struct DynamicTestRecord
    {
      DynamicTexture2D Texture;
      std::shared_ptr<UI::Image> Image;
      uint32_t ImageIndex{0};

      DynamicTestRecord() = default;
      DynamicTestRecord(const DynamicTexture2D& texture, std::shared_ptr<UI::Image> image, const uint32_t imageIndex)
        : Texture(texture)
        , Image(std::move(image))
        , ImageIndex(imageIndex)
      {
      }
    };

    struct UIRecord
    {
      std::shared_ptr<UI::ButtonBase> ButtonAddFront;
      std::shared_ptr<UI::ButtonBase> ButtonAddBack;
      std::shared_ptr<UI::ButtonBase> ButtonRemoveFront;
      std::shared_ptr<UI::ButtonBase> ButtonRemoveBack;
      std::shared_ptr<UI::ButtonBase> ButtonClear;

      std::shared_ptr<UI::ButtonBase> Button2AddFront;
      std::shared_ptr<UI::ButtonBase> Button2AddBack;
      std::shared_ptr<UI::ButtonBase> Button2RemoveFront;
      std::shared_ptr<UI::ButtonBase> Button2RemoveBack;
      std::shared_ptr<UI::ButtonBase> Button2Clear;

      std::shared_ptr<UI::ButtonBase> ButtonClearAll;

      std::shared_ptr<UI::Layout> MainLayout;
    };

    struct TestBitmaps
    {
      Bitmap Image0;
      Bitmap Image1;
      Bitmap ImageNum1;
      Bitmap ImageNum2;
      Bitmap ImageNum3;
      Bitmap ImageNum4;
    };


    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    TestBitmaps m_testBitmaps;

    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<UI::WrapLayout> m_test0;
    std::shared_ptr<UI::WrapLayout> m_test1;
    std::shared_ptr<UI::GridLayout> m_mainLayout;

    UIRecord m_uiRecord;

    std::deque<TestRecord> m_testRecords;
    std::deque<DynamicTestRecord> m_dynamicTestRecords;
    uint32_t m_frameCount{0};

  public:
    explicit Shared(const DemoAppConfig& config);
    ~Shared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    // From EventListener
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void Update();
    void Draw();

  private:
    TestRecord CreateTestRecord(const Bitmap& bitmap);
    void AddFrontTestRecord();
    void RemoveFrontTestRecord();
    void AddBackTestRecord();
    void RemoveBackTestRecord();
    void ClearTestRecords();

    const Bitmap& GetDynamicBitmap(const uint32_t imageIndex);
    DynamicTestRecord CreateDynamicTestRecord();
    void AddDynamicFrontTestRecord();
    void RemoveDynamicFrontTestRecord();
    void AddDynamicBackTestRecord();
    void RemoveDynamicBackTestRecord();
    void ClearDynamicTestRecords();
    UIRecord CreateUI(const std::shared_ptr<UI::WindowContext>& context, UI::Theme::IThemeControlFactory& rUIFactory);
  };
}

#endif
