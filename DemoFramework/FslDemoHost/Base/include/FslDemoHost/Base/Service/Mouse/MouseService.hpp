#ifndef FSLDEMOHOST_BASE_SERVICE_MOUSE_MOUSESERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_MOUSE_MOUSESERVICE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslDemoApp/Base/Service/NativeWindowEvents/INativeWindowEventListener.hpp>
#include <FslNativeWindow/Base/VirtualMouseButtonFlags.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>

namespace Fsl
{
  class IEventPoster;

  class MouseService final
    : public ThreadLocalService
    , public std::enable_shared_from_this<MouseService>
    , public IMouse
    , public INativeWindowEventListener
  {
    VirtualMouseButtonFlags m_buttonState;
    PxPoint2 m_position;
    PxPoint2 m_rawPosition;
    std::shared_ptr<IEventPoster> m_eventPoster;

  public:
    explicit MouseService(const ServiceProvider& serviceProvider);
    ~MouseService() final;

    void Link(const ServiceProvider& serviceProvider) final;

    // From IMouse
    MouseState GetState() final;

    // From INativeWindowEventListener
    void OnNativeWindowEvent(const NativeWindowEvent& event) final;

  private:
    void OnMouseButton(const NativeWindowEvent& event);
    void OnMouseMove(const NativeWindowEvent& event);
    void OnMouseWheel(const NativeWindowEvent& event);
    void OnRawMouseMove(const NativeWindowEvent& event);
  };
}

#endif
