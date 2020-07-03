#ifndef FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOW_HPP
#define FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOW_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Optional.hpp>
#include <FslBase/Math/Point2U.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslNativeWindow/Base/INativeWindow.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystemTypes.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowAllocationParams.hpp>
#include <memory>

namespace Fsl
{
  class NativeWindowSetup;
  class INativeWindowEventQueue;

  // Base class for platform windows
  class PlatformNativeWindow : public virtual INativeWindow
  {
    std::weak_ptr<INativeWindowEventQueue> m_eventQueue;

    Optional<Point2U> m_forcedActualDpi;
    Optional<uint32_t> m_forcedDensityDpi;
    NativeWindowCapabilityFlags m_capabilityFlags;

    mutable bool m_loggedOnceGetWindowMetrics{false};

  protected:
    PlatformNativeDisplayType m_platformDisplay;
    PlatformNativeWindowType m_platformWindow;

  public:
    PlatformNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                         const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams,
                         const NativeWindowCapabilityFlags capabilityFlags);
    ~PlatformNativeWindow() override;


    NativeWindowCapabilityFlags GetCapabilityFlags() const final
    {
      return m_capabilityFlags;
    }


    NativeWindowMetrics GetWindowMetrics() const final;
    bool TryGetExtent(PxExtent2D& rExtent) const final;
    bool TryGetDpi(Vector2& rDPI) const final;
    bool TryGetDensityDpi(uint32_t& rDensityDpi) const final;

    bool TryCaptureMouse(const bool /*enableCapture*/) override
    {
      return false;
    }

    PlatformNativeDisplayType GetPlatformDisplay() const
    {
      return m_platformDisplay;
    }


    PlatformNativeWindowType GetPlatformWindow() const
    {
      return m_platformWindow;
    }

  protected:
    virtual bool TryGetNativeSize(PxPoint2& rSize) const
    {
      rSize = {};
      return false;
    }
    virtual bool TryGetNativeDpi(Vector2& rDPI) const
    {
      rDPI = {};
      return false;
    }


    virtual bool TryGetNativeDensityDpi(uint32_t& rDensityDpi) const
    {
      rDensityDpi = 0u;
      return false;
    }


    //! @brief Try to acquire the event queue
    std::shared_ptr<INativeWindowEventQueue> TryGetEventQueue();
  };
}    // namespace Fsl

#endif
