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

#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseWheelEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/RawMouseMoveEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/TimeStateEvent.hpp>
#include <Shared/UI/Benchmark/DemoAppExtensionForwarder.hpp>

namespace Fsl
{
  namespace
  {
    inline void InvokeOnKeyEvent(IDemoAppExtension* pExtension, const KeyEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnKeyEvent(event);
      }
    }

    inline void InvokeOnMouseButtonEvent(IDemoAppExtension* pExtension, const MouseButtonEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnMouseButtonEvent(event);
      }
    }

    inline void InvokeOnMouseMoveEvent(IDemoAppExtension* pExtension, const MouseMoveEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnMouseMoveEvent(event);
      }
    }

    inline void InvokeOnMouseWheelEvent(IDemoAppExtension* pExtension, const MouseWheelEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnMouseWheelEvent(event);
      }
    }

    inline void InvokeOnRawMouseMoveEvent(IDemoAppExtension* pExtension, const RawMouseMoveEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnRawMouseMoveEvent(event);
      }
    }

    inline void InvokeOnTimeStateEvent(IDemoAppExtension* pExtension, const TimeStateEvent& event)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnTimeStateEvent(event);
      }
    }

    inline void InvokeConfigurationChanged(IDemoAppExtension* pExtension, const DemoWindowMetrics& windowMetrics)
    {
      if (pExtension != nullptr)
      {
        pExtension->ConfigurationChanged(windowMetrics);
      }
    }

    inline void InvokeBegin(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder)
    {
      if (pExtension != nullptr)
      {
        pExtension->Begin(callOrder);
      }
    }

    inline void InvokePreUpdate(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->PreUpdate(callOrder, demoTime);
      }
    }

    inline void InvokeFixedUpdate(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->FixedUpdate(callOrder, demoTime);
      }
    }

    inline void InvokeUpdate(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->Update(callOrder, demoTime);
      }
    }

    inline void InvokePostUpdate(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->PostUpdate(callOrder, demoTime);
      }
    }

    inline void InvokeResolve(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
    {
      if (pExtension != nullptr)
      {
        pExtension->Resolve(callOrder, demoTime);
      }
    }

    inline void InvokeOnDrawSkipped(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo)
    {
      if (pExtension != nullptr)
      {
        pExtension->OnDrawSkipped(callOrder, frameInfo);
      }
    }

    inline void InvokeEnd(IDemoAppExtension* pExtension, const DemoAppExtensionCallOrder callOrder)
    {
      if (pExtension != nullptr)
      {
        pExtension->End(callOrder);
      }
    }
  }

  void DemoAppExtensionForwarder::OnKeyEvent(const KeyEvent& event)
  {
    InvokeOnKeyEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      InvokeOnKeyEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    InvokeOnMouseButtonEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      InvokeOnMouseButtonEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    InvokeOnMouseMoveEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      InvokeOnMouseMoveEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    InvokeOnMouseWheelEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      InvokeOnMouseWheelEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
  {
    InvokeOnRawMouseMoveEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      InvokeOnRawMouseMoveEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnTimeStateEvent(const TimeStateEvent& event)
  {
    InvokeOnTimeStateEvent(m_extension0.get(), event);
    InvokeOnTimeStateEvent(m_extension1.get(), event);
  }

  void DemoAppExtensionForwarder::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    InvokeConfigurationChanged(m_extension0.get(), windowMetrics);
    InvokeConfigurationChanged(m_extension1.get(), windowMetrics);
  }


  void DemoAppExtensionForwarder::Begin(const DemoAppExtensionCallOrder callOrder)
  {
    InvokeBegin(m_extension0.get(), callOrder);
    InvokeBegin(m_extension1.get(), callOrder);
  }

  void DemoAppExtensionForwarder::PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    InvokePreUpdate(m_extension0.get(), callOrder, demoTime);
    InvokePreUpdate(m_extension1.get(), callOrder, demoTime);
  }

  void DemoAppExtensionForwarder::FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    InvokeFixedUpdate(m_extension0.get(), callOrder, demoTime);
    InvokeFixedUpdate(m_extension1.get(), callOrder, demoTime);
  }


  void DemoAppExtensionForwarder::Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    InvokeUpdate(m_extension0.get(), callOrder, demoTime);
    InvokeUpdate(m_extension1.get(), callOrder, demoTime);
  }

  void DemoAppExtensionForwarder::PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    InvokePostUpdate(m_extension0.get(), callOrder, demoTime);
    if (callOrder == DemoAppExtensionCallOrder::PostApp && m_tweak)
    {
      m_tweak->PostUpdate();
    }
    InvokePostUpdate(m_extension1.get(), callOrder, demoTime);
  }

  void DemoAppExtensionForwarder::Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    InvokeResolve(m_extension0.get(), callOrder, demoTime);
    InvokeResolve(m_extension1.get(), callOrder, demoTime);
  }

  void DemoAppExtensionForwarder::OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo)
  {
    InvokeOnDrawSkipped(m_extension0.get(), callOrder, frameInfo);
    InvokeOnDrawSkipped(m_extension1.get(), callOrder, frameInfo);
  }


  void DemoAppExtensionForwarder::End(const DemoAppExtensionCallOrder callOrder)
  {
    InvokeEnd(m_extension0.get(), callOrder);
    InvokeEnd(m_extension1.get(), callOrder);
  }
}
