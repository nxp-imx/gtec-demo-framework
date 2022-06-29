#ifndef FSLDEMOAPP_BASE_IDEMOAPPEXTENSION_HPP
#define FSLDEMOAPP_BASE_IDEMOAPPEXTENSION_HPP
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

#include <FslDemoApp/Base/DemoAppExtensionCallOrder.hpp>

namespace Fsl
{
  struct DemoTime;
  struct DemoWindowMetrics;
  struct FrameInfo;
  class KeyEvent;
  class MouseButtonEvent;
  class MouseMoveEvent;
  class MouseWheelEvent;
  class RawMouseMoveEvent;
  class TimeStateEvent;

  class IDemoAppExtension
  {
  public:
    virtual ~IDemoAppExtension() = default;

    virtual void OnKeyEvent(const KeyEvent& event) = 0;
    virtual void OnMouseButtonEvent(const MouseButtonEvent& event) = 0;
    virtual void OnMouseMoveEvent(const MouseMoveEvent& event) = 0;
    virtual void OnMouseWheelEvent(const MouseWheelEvent& event) = 0;
    virtual void OnRawMouseMoveEvent(const RawMouseMoveEvent& event) = 0;
    virtual void OnTimeStateEvent(const TimeStateEvent& event) = 0;
    virtual void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) = 0;
    virtual void Begin(const DemoAppExtensionCallOrder callOrder) = 0;
    virtual void PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) = 0;
    virtual void FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) = 0;
    virtual void Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) = 0;
    virtual void PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) = 0;
    virtual void Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) = 0;
    virtual void OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo) = 0;
    virtual void End(const DemoAppExtensionCallOrder callOrder) = 0;
  };
}

#endif
