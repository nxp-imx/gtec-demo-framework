#ifndef SHARED_UI_BENCHMARK_SCENE_BASICSCENE_HPP
#define SHARED_UI_BENCHMARK_SCENE_BASICSCENE_HPP
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

#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <memory>
#include "IScene.hpp"

namespace Fsl
{
  struct SceneCreateInfo;

  namespace UI
  {
    class Layout;

    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  class BasicScene : public IScene
  {
    enum class SceneState
    {
      Ready,
      Closing,
    };

    SceneState m_state{SceneState::Ready};
    SceneId m_nextSceneId{SceneId::Playground};
    bool m_allowBackKey{true};

  protected:
    std::shared_ptr<UI::Theme::IThemeControlFactory> m_uiControlFactory;
    DemoWindowMetrics m_windowMetrics;
    std::shared_ptr<UI::Layout> m_rootLayout;

  public:
    explicit BasicScene(const SceneCreateInfo& createInfo);
    ~BasicScene() override;

    std::optional<NextSceneRecord> TryGetNextScene() const final
    {
      return m_state == SceneState::Closing ? std::optional<NextSceneRecord>(NextSceneRecord(m_nextSceneId)) : std::optional<NextSceneRecord>();
    }

    void OnFrameSequenceBegin() override;
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;
    void OnKeyEvent(const KeyEvent& event) override;
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
    void Update(const DemoTime& demoTime) override;
    bool Resolve(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
    void OnDrawSkipped(const FrameInfo& frameInfo) override;
    void OnFrameSequenceEnd() override;

  protected:
    DemoWindowMetrics GetwindowMetrics() const noexcept
    {
      return m_windowMetrics;
    }

    bool IsClosing() const
    {
      return m_state == SceneState::Closing;
    }

    bool AllowBackKey() const noexcept
    {
      return m_allowBackKey;
    }

    void SetAllowBackKey(const bool enabled)
    {
      m_allowBackKey = enabled;
    }

    void ScheduleClose();
    void ScheduleClose(const SceneId nextSceneId);
    virtual void OnBeginClose() {};
  };
}

#endif
