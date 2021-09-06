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

#include <Shared/UI/Benchmark/DemoAppExtensionForwarder.hpp>


namespace Fsl
{
  void DemoAppExtensionForwarder::OnKeyEvent(const KeyEvent& event)
  {
    OnKeyEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      OnKeyEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    OnMouseButtonEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      OnMouseButtonEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    OnMouseMoveEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      OnMouseMoveEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    OnMouseWheelEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      OnMouseWheelEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
  {
    OnRawMouseMoveEvent(m_extension0.get(), event);
    if (!event.IsHandled())
    {
      OnRawMouseMoveEvent(m_extension1.get(), event);
    }
  }

  void DemoAppExtensionForwarder::OnTimeStateEvent(const TimeStateEvent& event)
  {
    OnTimeStateEvent(m_extension0.get(), event);
    OnTimeStateEvent(m_extension1.get(), event);
  }

  void DemoAppExtensionForwarder::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    ConfigurationChanged(m_extension0.get(), windowMetrics);
    ConfigurationChanged(m_extension1.get(), windowMetrics);
  }

  void DemoAppExtensionForwarder::PreUpdate(const DemoTime& demoTime)
  {
    PreUpdate(m_extension0.get(), demoTime);
    PreUpdate(m_extension1.get(), demoTime);
  }

  void DemoAppExtensionForwarder::FixedUpdate(const DemoTime& demoTime)
  {
    FixedUpdate(m_extension0.get(), demoTime);
    FixedUpdate(m_extension1.get(), demoTime);
  }

  void DemoAppExtensionForwarder::Update(const DemoTime& demoTime)
  {
    Update(m_extension0.get(), demoTime);
    Update(m_extension1.get(), demoTime);
  }

  void DemoAppExtensionForwarder::PostUpdate(const DemoTime& demoTime)
  {
    PostUpdate(m_extension0.get(), demoTime);
    if (m_tweak)
    {
      m_tweak->PostUpdate();
    }
    PostUpdate(m_extension1.get(), demoTime);
  }
}
