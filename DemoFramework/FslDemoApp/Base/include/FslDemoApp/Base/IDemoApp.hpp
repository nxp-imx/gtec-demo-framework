#ifndef FSLDEMOAPP_BASE_IDEMOAPP_HPP
#define FSLDEMOAPP_BASE_IDEMOAPP_HPP
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

#include <FslDemoApp/Base/AppDrawResult.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>

namespace Fsl
{
  class IEvent;
  struct FrameInfo;

  class IDemoApp
  {
  public:
    IDemoApp(const IDemoApp&) = delete;
    IDemoApp& operator=(const IDemoApp&) = delete;
    IDemoApp() = default;
    virtual ~IDemoApp() = default;

    //! @brief Called just after the app has been constructed
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _PostConstruct() = 0;
    //! brief Called just before the app is destroyed (this is a good place for shutdown code that can throw exceptions)
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _PreDestruct() = 0;
    //! Called as the first function in the 'frame processing' execution order (beware event processing might be called before this).
    //! Beware even though the name might indicate that _End() will be called too this might not be the case
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _Begin() = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _OnEvent(IEvent* const pEvent) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _ConfigurationChanged(const DemoWindowMetrics& windowMetrics) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _PreUpdate(const DemoTime& demoTime) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _FixedUpdate(const DemoTime& demoTime) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _Update(const DemoTime& demoTime) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _PostUpdate(const DemoTime& demoTime) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _Resolve(const DemoTime& demoTime) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual AppDrawResult _TryPrepareDraw(const FrameInfo& frameInfo) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _BeginDraw(const FrameInfo& frameInfo) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _Draw(const FrameInfo& frameInfo) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _EndDraw(const FrameInfo& frameInfo) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _OnDrawSkipped(const FrameInfo& frameInfo) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    // @brief If the demo host says the swap buffers is app controlled then this will be called to execute the swap buffers.
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual AppDrawResult _TrySwapBuffers(const FrameInfo& frameInfo) = 0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void _End() = 0;
  };
}

#endif
