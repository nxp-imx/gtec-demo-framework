#ifndef SHARED_UI_BASICDATABINDING_BASICDATABINDINGSHARED_HPP
#define SHARED_UI_BASICDATABINDING_BASICDATABINDINGSHARED_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslSimpleUI/Base/Control/SliderBase.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>
#include <array>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    class ButtonBase;
    class Layout;

    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  class BasicDataBindingShared : public UI::EventListener
  {
    struct UIExampleRecord1
    {
      std::shared_ptr<UI::Layout> Main;
      std::shared_ptr<UI::ButtonBase> BtnSub;
      std::shared_ptr<UI::SliderBase<int32_t>> Slider;
      std::shared_ptr<UI::ButtonBase> BtnAdd;
    };
    struct UIExampleRecord2
    {
      std::shared_ptr<UI::Layout> Main;
    };
    struct UIExampleRecord3
    {
      std::shared_ptr<UI::Layout> Main;
    };
    struct UIExampleRecord4
    {
      std::shared_ptr<UI::Layout> Main;
    };


    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
      UIExampleRecord1 Example1;
      UIExampleRecord2 Example2;
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;


    UIRecord m_uiRecord;

  public:
    explicit BasicDataBindingShared(const DemoAppConfig& config);
    ~BasicDataBindingShared() override;


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
    void SetDefaultValues();
    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory);
    static UIExampleRecord1 CreateExample1UI(UI::Theme::IThemeControlFactory& uiFactory);
    static UIExampleRecord2 CreateExample2UI(UI::Theme::IThemeControlFactory& uiFactory);
    static UIExampleRecord3 CreateExample3UI(UI::Theme::IThemeControlFactory& uiFactory);
    static UIExampleRecord4 CreateExample4UI(UI::Theme::IThemeControlFactory& uiFactory);
  };
}
#endif
