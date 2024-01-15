#ifndef FSLNATIVEWINDOW_PLATFORM_ADAPTER_WAYLAND_PLATFORMNATIVEWINDOWADAPTERWAYLAND_HPP
#define FSLNATIVEWINDOW_PLATFORM_ADAPTER_WAYLAND_PLATFORMNATIVEWINDOWADAPTERWAYLAND_HPP
#if !defined(__ANDROID__) && defined(__linux__) && !defined(FSL_WINDOWSYSTEM_X11) && defined(FSL_WINDOWSYSTEM_WAYLAND)
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

#include <FslBase/Math/Point2.hpp>
#include <FslNativeWindow/Platform/Adapter/PlatformNativeWindowAdapter.hpp>
#include <memory>
#include <vector>

namespace Fsl
{
  class PlatformNativeWindowContextWayland;

  class PlatformNativeWindowAdapterWayland : public PlatformNativeWindowAdapter
  {
  public:
    struct WaylandDisplayGeometry
    {
      uint32_t Width{};
      uint32_t Height{};
      uint32_t PhysicalWidth{};
      uint32_t PhysicalHeight{};
    };

  private:
    std::unique_ptr<PlatformNativeWindowContextWayland> m_windowContext;
    std::function<void(void*)> m_destroyWindowCallback;
    wl_surface* m_platformSurface;
    std::vector<WaylandDisplayGeometry> m_displayOutput;
    Point2 m_cachedScreenDPI;

  public:
    PlatformNativeWindowAdapterWayland(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                                       const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams);
    ~PlatformNativeWindowAdapterWayland() override;

    wl_surface* GetWaylandSurface()
    {
      return m_platformSurface;
    }

  protected:
    bool TryGetNativeSize(PxPoint2& rSize) const override;
    bool TryGetNativeDpi(Vector2& rDPI) const override;
    // Wayland hopefully sends a event when the dpi is changed, and we then need to notify the framework with
    // eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowConfigChanged());
    // virtual bool TryGetNativeDpi(Vector2& rDPI) const override;
  };
}    // namespace Fsl

#endif
#endif
