#ifndef SHARED_UI_DECLARATIVE_DECLARATIVESHARED_HPP
#define SHARED_UI_DECLARATIVE_DECLARATIVESHARED_HPP
/****************************************************************************************************************************************************
 * Copyright 2022-2023 NXP
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
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>

namespace Fsl
{
  class IContentManager;
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  class DeclarativeShared : public UI::EventListener
  {
    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;

    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    UIRecord m_uiRecord;

  public:
    explicit DeclarativeShared(const DemoAppConfig& config);
    ~DeclarativeShared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    // From EventListener
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

    void OnKeyEvent(const KeyEvent& event);
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Draw();

  private:
    UIRecord CreateUI(const IContentManager& contentManager, const std::shared_ptr<UI::Theme::IThemeControlFactory>& uiFactory);
  };
}

#endif
