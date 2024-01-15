#ifndef SHARED_UI_BENCHMARK_APP_TESTAPP_HPP
#define SHARED_UI_BENCHMARK_APP_TESTAPP_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/UI/Benchmark/Activity/ActivityStack.hpp>
#include <Shared/UI/Benchmark/App/BackgroundCube.hpp>
#include <Shared/UI/Benchmark/App/CustomUIDemoAppExtension.hpp>
#include <Shared/UI/Benchmark/App/ITestApp.hpp>
#include <memory>

namespace Fsl
{
  class TestApp final
    : public ITestApp
    , public UI::EventListener
  {
    struct BasicUI
    {
      std::shared_ptr<UI::Layout> Layout;
    };

    struct MainUI
    {
      std::shared_ptr<UI::Layout> Layout;
      std::shared_ptr<UI::Layout> AppLayout;
      std::shared_ptr<UI::ActivityStack> ActivityStack;
      std::shared_ptr<UI::ButtonBase> ButtonShowDialog0;
      std::shared_ptr<UI::ButtonBase> ButtonShowDialog1;
      std::shared_ptr<UI::ButtonBase> ButtonShowDialog2;
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<CustomUIDemoAppExtension> m_uiExtension;
    std::shared_ptr<UI::Theme::IThemeControlFactory> m_controlFactory;

    DemoWindowMetrics m_windowMetrics;
    BackgroundCube m_background;
    MainUI m_uiProfile;

  public:
    explicit TestApp(const UIDemoAppExtensionCreateInfo& createInfo);
    ~TestApp() override;


    // From EventListener
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;
    void OnKeyEvent(const KeyEvent& event);

    // From ITestApp
    Color GetRootColor() const final;
    bool TrySetRootColor(const Color color) final;

    const UI::IRenderSystemBase& GetRenderSystem() const final;
    UI::IRenderSystemBase* TryGetRenderSystem() final;
    std::shared_ptr<CustomUIDemoAppExtension> GetCustomUIDemoAppExtension() const final
    {
      return m_uiExtension;
    }
    void SetCustomViewport(const PxViewport& viewportPx) final;
    void SetUseDrawCache(const bool useDrawCache) final;

    bool IsUIIdle() const final;

    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void Draw() final;
    void OnDrawSkipped() final;

  private:
    void SetDefaultValues();

    MainUI CreateUI(UI::Theme::IThemeControlFactory& uiFactory);
    BasicUI CreateSwitchUI(UI::Theme::IThemeControlFactory& uiFactory);
    BasicUI CreateCheckBoxUI(UI::Theme::IThemeControlFactory& uiFactory);
    BasicUI CreateRadioButtonUI(UI::Theme::IThemeControlFactory& uiFactory);
    BasicUI CreateSliderUI(UI::Theme::IThemeControlFactory& uiFactory, const UI::LayoutOrientation orientation);
    BasicUI CreateSliderUI2(UI::Theme::IThemeControlFactory& uiFactory, const UI::LayoutOrientation orientation);
    BasicUI CreateTextButtonUI(UI::Theme::IThemeControlFactory& uiFactory);
    BasicUI CreateFmtValueLabelUI(UI::Theme::IThemeControlFactory& uiFactory);
    BasicUI CreateBasicImageUI(UI::Theme::IThemeControlFactory& uiFactory);
    BasicUI CreateLabelUI(UI::Theme::IThemeControlFactory& uiFactory);
  };
}

#endif
