#ifndef SHARED_DFSIMPLEUI101_SHARED_HPP
#define SHARED_DFSIMPLEUI101_SHARED_HPP
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

#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <memory>

namespace Fsl
{
  class AtlasFont;
  class AtlasTexture2D;
  class IGraphicsService;
  class KeyEvent;
  class MouseButtonEvent;
  class MouseMoveEvent;
  struct Point2;
  class ITextureAtlas;
  class Texture2D;

  namespace UI
  {
    class BaseWindow;
    class Button;
    class LabelButton;
    class ImageButton;
    class Slider;
    class StackLayout;
  }

  class Shared : public UI::EventListener
  {
    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<UI::FillLayout> m_fillLayout;
    std::shared_ptr<UI::LabelButton> m_button1;
    std::shared_ptr<UI::ImageButton> m_button2;
    std::shared_ptr<UI::Button> m_complexButton;
    std::shared_ptr<UI::Slider> m_slider1;
    std::shared_ptr<UI::Slider> m_slider2;

  public:
    Shared(const DemoAppConfig& config);
    ~Shared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    // From EventListener
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void Draw();

  private:
    std::shared_ptr<UI::BaseWindow> CreateStack1(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture,
                                                 const std::shared_ptr<AtlasFont>& fontSmall, const AtlasTexture2D& texImage);
    std::shared_ptr<UI::BaseWindow> CreateStack2(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture,
                                                 const AtlasTexture2D& texImage);
    std::shared_ptr<UI::BaseWindow> CreateStack3(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture);
    std::shared_ptr<UI::BaseWindow> CreateStack4(const std::shared_ptr<UI::WindowContext>& context, const Texture2D& atlasTexture);
  };
}

#endif
