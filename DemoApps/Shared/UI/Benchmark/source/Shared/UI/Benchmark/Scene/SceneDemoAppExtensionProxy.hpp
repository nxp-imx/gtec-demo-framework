#ifndef SHARED_UI_BENCHMARK_SCENE_SCENEDEMOAPPEXTENSIONPROXY_HPP
#define SHARED_UI_BENCHMARK_SCENE_SCENEDEMOAPPEXTENSIONPROXY_HPP
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

#include <FslBase/Optional.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <Shared/UI/Benchmark/Persistence/Input/InputCommandRecord.hpp>
#include <memory>
#include <utility>
#include "ISceneTestDemoControl.hpp"


namespace Fsl
{
  class ICustomWindowInfoModule;
  class InputRecorder;

  class SceneDemoAppExtensionProxy final
    : public DemoAppExtension
    , public ISceneTestDemoControl
  {
    struct PlayRecord
    {
      bool IsPlaying{false};
      uint32_t FrameIndex{0};
    };

    std::shared_ptr<ICustomWindowInfoModule> m_info;
    std::shared_ptr<DemoAppExtension> m_extension;
    std::shared_ptr<InputRecorder> m_recorder;
    DemoTime m_controlledDemoTime;
    bool m_fixedUpdateCalled{false};
    PlayRecord m_playRecord;

  public:
    explicit SceneDemoAppExtensionProxy(std::shared_ptr<ICustomWindowInfoModule> info, std::shared_ptr<DemoAppExtension> extension);
    ~SceneDemoAppExtensionProxy() final;


    void Clear() noexcept
    {
      m_extension.reset();
      m_recorder.reset();
    }

    void SetExtension(const std::shared_ptr<DemoAppExtension>& extension)
    {
      m_extension = extension;
    }

    void OnKeyEvent(const KeyEvent& event) final;
    void OnMouseButtonEvent(const MouseButtonEvent& event) final;
    void OnMouseMoveEvent(const MouseMoveEvent& event) final;
    void OnMouseWheelEvent(const MouseWheelEvent& event) final;
    void OnRawMouseMoveEvent(const RawMouseMoveEvent& event) final;
    void OnTimeStateEvent(const TimeStateEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void PreUpdate(const DemoTime& demoTime) final;
    void FixedUpdate(const DemoTime& demoTime) final;
    void Update(const DemoTime& demoTime) final;
    void PostUpdate(const DemoTime& demoTime) final;

    // From ISceneTestDemoControl
    ReadOnlySpan<InputCommandRecord> GetRecordingAsSpan() const final;
    void SetRecording(ReadOnlySpan<InputCommandRecord> span, const uint32_t frameCount) final;
    uint32_t FrameCount() const final;
    bool IsRecording() const final;
    void RecordBegin() final;
    void RecordEnd() final;

    bool IsPlaying() const final;
    uint32_t PlayFrameIndex() const final;
    void PlayBegin() final;
    void PlayEnd() final;

    Optional<DemoTime> TryGetDemoTime() const;

  private:
    void StopAll();
    bool AllowInputForwarding() const;
    bool UseCustomTime() const;
    DemoTime GetDemoTime(const DemoTime& demoTime) const;
    void GenerateFakeInput(ReadOnlySpan<InputCommandRecord> span);
    void GenerateFakeMouseDown(const InputCommandRecord& entry);
    void GenerateFakeMouseUp(const InputCommandRecord& entry);
    void GenerateFakeMouseDownMove(const InputCommandRecord& entry);
    void GenerateFakeMouseMove(const InputCommandRecord& entry);
    void GenerateFakeMouseClear(const InputCommandRecord& entry);

    void ForwardMouseButtonEvent(const MouseButtonEvent& event);
    void ForwardMouseMoveEvent(const MouseMoveEvent& event);
    Optional<PxPoint2> TryRewriteScreenCoordinate(const InputCommandRecord& entry) const;
  };
}

#endif
