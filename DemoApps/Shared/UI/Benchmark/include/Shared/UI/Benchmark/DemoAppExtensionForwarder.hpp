#ifndef SHARED_UI_BENCHMARK_DEMOAPPEXTENSIONFORWARDER_HPP
#define SHARED_UI_BENCHMARK_DEMOAPPEXTENSIONFORWARDER_HPP
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

#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <Shared/UI/Benchmark/IForwarderTweak.hpp>
#include <memory>
#include <utility>


namespace Fsl
{
  class DemoAppExtensionForwarder final : public DemoAppExtension
  {
    std::shared_ptr<DemoAppExtension> m_extension0;
    std::shared_ptr<DemoAppExtension> m_extension1;
    std::shared_ptr<IForwarderTweak> m_tweak;

  public:
    DemoAppExtensionForwarder() = default;

    explicit DemoAppExtensionForwarder(std::shared_ptr<DemoAppExtension> extension0)
      : m_extension0(std::move(extension0))
    {
    }

    DemoAppExtensionForwarder(std::shared_ptr<DemoAppExtension> extension0, std::shared_ptr<DemoAppExtension> extension1)
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

    void SetExtension0(const std::shared_ptr<DemoAppExtension>& extension)
    {
      m_extension0 = extension;
    }

    void ClearExtension1() noexcept
    {
      m_extension1.reset();
    }

    void SetExtension1(const std::shared_ptr<DemoAppExtension>& extension)
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
    void PreUpdate(const DemoTime& demoTime) final;
    void FixedUpdate(const DemoTime& demoTime) final;
    void Update(const DemoTime& demoTime) final;
    void PostUpdate(const DemoTime& demoTime) final;

  private:
    static void OnKeyEvent(DemoAppExtension* pExtension, const KeyEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnKeyEvent(event);
      }
    }

    static void OnMouseButtonEvent(DemoAppExtension* pExtension, const MouseButtonEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnMouseButtonEvent(event);
      }
    }

    static void OnMouseMoveEvent(DemoAppExtension* pExtension, const MouseMoveEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnMouseMoveEvent(event);
      }
    }

    static void OnMouseWheelEvent(DemoAppExtension* pExtension, const MouseWheelEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnMouseWheelEvent(event);
      }
    }

    static void OnRawMouseMoveEvent(DemoAppExtension* pExtension, const RawMouseMoveEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnRawMouseMoveEvent(event);
      }
    }

    static void OnTimeStateEvent(DemoAppExtension* pExtension, const TimeStateEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnTimeStateEvent(event);
      }
    }

    static void ConfigurationChanged(DemoAppExtension* pExtension, const DemoWindowMetrics& windowMetrics)
    {
      if (pExtension != nullptr)
      {
        pExtension->ConfigurationChanged(windowMetrics);
      }
    }

    static void PreUpdate(DemoAppExtension* pExtension, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->PreUpdate(demoTime);
      }
    }

    static void FixedUpdate(DemoAppExtension* pExtension, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->FixedUpdate(demoTime);
      }
    }

    static void Update(DemoAppExtension* pExtension, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->Update(demoTime);
      }
    }

    static void PostUpdate(DemoAppExtension* pExtension, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->PostUpdate(demoTime);
      }
    }
  };
}

#endif
