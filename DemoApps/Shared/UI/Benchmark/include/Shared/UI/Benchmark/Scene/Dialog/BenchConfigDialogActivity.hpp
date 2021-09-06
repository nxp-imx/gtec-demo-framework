#ifndef SHARED_UI_BENCHMARK_SCENE_DIALOG_BENCHCONFIGDIALOGACTIVITY_HPP
#define SHARED_UI_BENCHMARK_SCENE_DIALOG_BENCHCONFIGDIALOGACTIVITY_HPP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslSimpleUI/Theme/Base/WindowType.hpp>
#include <Shared/UI/Benchmark/Activity/DialogActivity.hpp>
#include <Shared/UI/Benchmark/Persistence/AppBenchSettings.hpp>
#include <Shared/UI/Benchmark/Persistence/AppRenderMethod.hpp>
#include <Shared/UI/Benchmark/RenderMethodInfo.hpp>
#include <Shared/UI/Benchmark/Scene/Control/RenderOptionControls.hpp>
#include <memory>
#include <vector>
#include <utility>

namespace Fsl
{
  namespace UI
  {
    class ButtonBase;
    class GridLayout;
    class RadioButton;
    class Switch;

    class BenchConfigDialogActivity final : public DialogActivity
    {
      enum class State
      {
        Ready,
        Closing,
      };

      struct RenderMethodRecord
      {
        AppRenderMethod RenderMethod{AppRenderMethod::FlexImBatch};
        std::shared_ptr<UI::RadioButton> RadioButton;
        RenderMethodRecord() = default;
        RenderMethodRecord(const AppRenderMethod renderMethod, std::shared_ptr<UI::RadioButton> radioButton)
          : RenderMethod(renderMethod)
          , RadioButton(std::move(radioButton))
        {
        }
      };

      struct RenderMethodUI
      {
        std::vector<RenderMethodRecord> Methods;

        RenderMethodUI() = default;
        explicit RenderMethodUI(const std::size_t capacity)
          : Methods(capacity)
        {
        }

        void FinishAnimation();
      };

      struct MainUI
      {
        std::shared_ptr<ButtonBase> ButtonOK;
        std::shared_ptr<ButtonBase> ButtonBack;
        std::shared_ptr<Switch> SwitchGpuTimestamps;
        std::shared_ptr<Switch> SwitchNoOpaqueMaterials;
        std::shared_ptr<Switch> SwitchUseSdfFonts;
        RenderMethodUI RenderMethod;
        RenderOptionControls RenderOptions;

        void FinishAnimation();
      };

      State m_state{State::Ready};

      std::vector<RenderMethodInfo> m_renderMethods;
      uint32_t m_activeRenderIndex{0};

      MainUI m_mainUI;

      std::shared_ptr<AppBenchSettings> m_settings;

    public:
      // Since we dont have a proper activity concept with async return values we use the shared settings object for now
      BenchConfigDialogActivity(std::weak_ptr<IActivityStack> activityStack, const std::shared_ptr<Theme::IThemeControlFactory>& themeControlFactory,
                                const bool gpuTimestampsSupported, const ReadOnlySpan<RenderMethodInfo> renderMethods,
                                std::shared_ptr<AppBenchSettings> settings);


      void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) final;
      void OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent) final;
      void OnKeyEvent(const KeyEvent& theEvent) final;

    private:
      static MainUI CreateMainUI(Theme::IThemeControlFactory& uiFactory, GridLayout& rMainLayout, const bool gpuTimestampsSupported,
                                 const ReadOnlySpan<RenderMethodInfo> renderMethods, const uint32_t activeRenderIndex,
                                 const AppBenchSettings& settings);
      void DoScheduleClose(const bool completed);
      void SetSettings(const AppBenchSettings& settings, const uint32_t activeRenderIndex);
      AppBenchSettings RetrieveSettings();
    };
  }
}


#endif
