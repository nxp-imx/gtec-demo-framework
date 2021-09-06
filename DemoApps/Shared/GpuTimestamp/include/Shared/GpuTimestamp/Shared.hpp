#ifndef SHARED_GPUTIMESTAMP_SHARED_HPP
#define SHARED_GPUTIMESTAMP_SHARED_HPP
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

#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <Shared/GpuTimestamp/DefaultValues.hpp>
#include <Shared/GpuTimestamp/Chart/AreaChartData.hpp>
#include <Shared/GpuTimestamp/Chart/AverageData.hpp>

namespace Fsl
{
  namespace UI
  {
    class IChartComplexDataWindow;

    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  class Shared final : public UI::EventListener
  {
    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> MainWindow;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> SliderIterations;
      std::shared_ptr<UI::BackgroundLabelButton> BtnDefault;
      std::shared_ptr<UI::FmtValueLabel<float>> GpuTimeFmtLabel;
      std::shared_ptr<UI::BackgroundLabelButton> BtnClearChart;
      std::shared_ptr<UI::Switch> SwitchPause;
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;


    std::shared_ptr<UI::AreaChartData> m_data;
    UI::AverageData m_dataAverage;

    uint16_t m_defaultIterations{DefaultValues::MandelbrotDefaultIterations};

    UIRecord m_ui;

  public:
    explicit Shared(const DemoAppConfig& config, const bool chartSupported);
    ~Shared() override;

    const std::shared_ptr<UIDemoAppExtension>& GetDemoAppExtension() const
    {
      return m_uiExtension;
    }

    uint32_t GetIterations() const
    {
      return m_ui.SliderIterations->GetValue();
    }

    bool IsPaused() const
    {
      return m_ui.SwitchPause->IsChecked();
    }

    void SetProfileTime(const uint64_t time);

    // From EventListener
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

    void OnKeyEvent(const KeyEvent& event);
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);
    void OnFrameSequenceEnd();

  private:
    void SetDefaultValues();
    void ClearGraph();

    UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const uint16_t iterations, const std::shared_ptr<UI::IChartComplexDataWindow>& data,
                      const bool chartSupported);
  };
}

#endif
