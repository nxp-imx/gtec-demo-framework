#ifndef SHARED_UI_BENCHMARK_DEMOAPPEXTENSIONFORWARDER_HPP
#define SHARED_UI_BENCHMARK_DEMOAPPEXTENSIONFORWARDER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslDemoApp/Base/IDemoAppExtension.hpp>
#include <Shared/UI/Benchmark/IForwarderTweak.hpp>
#include <memory>
#include <utility>


namespace Fsl
{
  class DemoAppExtensionForwarder final : public IDemoAppExtension
  {
    std::shared_ptr<IDemoAppExtension> m_extension0;
    std::shared_ptr<IDemoAppExtension> m_extension1;
    std::shared_ptr<IForwarderTweak> m_tweak;

  public:
    DemoAppExtensionForwarder() = default;

    explicit DemoAppExtensionForwarder(std::shared_ptr<IDemoAppExtension> extension0)
      : m_extension0(std::move(extension0))
    {
    }

    DemoAppExtensionForwarder(std::shared_ptr<IDemoAppExtension> extension0, std::shared_ptr<IDemoAppExtension> extension1)
      : m_extension0(std::move(extension0))
      , m_extension1(std::move(extension1))
    {
    }

    void Clear() noexcept
    {
      ClearTweak();
      ClearExtensions();
    }

    void ClearExtensions() noexcept
    {
      m_extension0.reset();
      m_extension1.reset();
    }

    void ClearTweak() noexcept
    {
      m_tweak.reset();
    }

    void SetTweak(std::shared_ptr<IForwarderTweak> tweak)
    {
      m_tweak = std::move(tweak);
    }

    void SetExtension0(const std::shared_ptr<IDemoAppExtension>& extension)
    {
      m_extension0 = extension;
    }

    void ClearExtension1() noexcept
    {
      m_extension1.reset();
    }

    void SetExtension1(const std::shared_ptr<IDemoAppExtension>& extension)
    {
      m_extension1 = extension;
    }

    void OnKeyEvent(const KeyEvent& event) final;
    void OnMouseButtonEvent(const MouseButtonEvent& event) final;
    void OnMouseMoveEvent(const MouseMoveEvent& event) final;
    void OnMouseWheelEvent(const MouseWheelEvent& event) final;
    void OnRawMouseMoveEvent(const RawMouseMoveEvent& event) final;
    void OnTimeStateEvent(const TimeStateEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;

    void Begin(const DemoAppExtensionCallOrder callOrder) final;
    void PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo) final;
    void End(const DemoAppExtensionCallOrder callOrder) final;
  };
}


#endif
