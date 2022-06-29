#ifndef FSLDEMOAPP_BASE_DEMOAPPEXTENSIONPROXY_HPP
#define FSLDEMOAPP_BASE_DEMOAPPEXTENSIONPROXY_HPP
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

#include <FslDemoApp/Base/IDemoAppExtension.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  template <typename T>
  class DemoAppExtensionProxy : public IDemoAppExtension
  {
    std::shared_ptr<T> m_proxy;

  public:
    using value_type = T;

    DemoAppExtensionProxy() = default;
    explicit DemoAppExtensionProxy(std::shared_ptr<value_type> proxy)
      : m_proxy(std::move(proxy))
    {
    }

    bool IsEmpty() const
    {
      return m_proxy.get() == nullptr;
    }

    bool IsReady() const
    {
      return m_proxy.get() != nullptr;
    }

    const std::shared_ptr<value_type>& Get() const
    {
      return m_proxy;
    }

    value_type* TryGet()
    {
      return m_proxy.get();
    }

    void Reset()
    {
      m_proxy.reset();
    }

    void SetProxy(std::shared_ptr<value_type> proxy)
    {
      m_proxy = std::move(proxy);
    }


    // --- proxy methods ---

    void OnKeyEvent(const KeyEvent& event) override
    {
      if (m_proxy)
      {
        m_proxy->OnKeyEvent(event);
      }
    }

    void OnMouseButtonEvent(const MouseButtonEvent& event) override
    {
      if (m_proxy)
      {
        m_proxy->OnMouseButtonEvent(event);
      }
    }

    void OnMouseMoveEvent(const MouseMoveEvent& event) override
    {
      if (m_proxy)
      {
        m_proxy->OnMouseMoveEvent(event);
      }
    }

    void OnMouseWheelEvent(const MouseWheelEvent& event) override
    {
      if (m_proxy)
      {
        m_proxy->OnMouseWheelEvent(event);
      }
    }

    void OnRawMouseMoveEvent(const RawMouseMoveEvent& event) override
    {
      if (m_proxy)
      {
        m_proxy->OnRawMouseMoveEvent(event);
      }
    }

    void OnTimeStateEvent(const TimeStateEvent& event) override
    {
      if (m_proxy)
      {
        m_proxy->OnTimeStateEvent(event);
      }
    }

    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override
    {
      if (m_proxy)
      {
        m_proxy->ConfigurationChanged(windowMetrics);
      }
    }

    void Begin(const DemoAppExtensionCallOrder callOrder) override
    {
      if (m_proxy)
      {
        m_proxy->Begin(callOrder);
      }
    }

    void PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override
    {
      if (m_proxy)
      {
        m_proxy->PreUpdate(callOrder, demoTime);
      }
    }

    void FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override
    {
      if (m_proxy)
      {
        m_proxy->FixedUpdate(callOrder, demoTime);
      }
    }

    void Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override
    {
      if (m_proxy)
      {
        m_proxy->Update(callOrder, demoTime);
      }
    }

    void PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override
    {
      if (m_proxy)
      {
        m_proxy->PostUpdate(callOrder, demoTime);
      }
    }


    void Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override
    {
      if (m_proxy)
      {
        m_proxy->Resolve(callOrder, demoTime);
      }
    }

    void OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo) override
    {
      if (m_proxy)
      {
        m_proxy->OnDrawSkipped(callOrder, frameInfo);
      }
    }

    void End(const DemoAppExtensionCallOrder callOrder) override
    {
      if (m_proxy)
      {
        m_proxy->End(callOrder);
      }
    }
  };
}

#endif
