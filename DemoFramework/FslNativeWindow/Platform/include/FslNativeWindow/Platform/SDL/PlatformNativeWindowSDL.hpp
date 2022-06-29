#ifndef FSLNATIVEWINDOW_PLATFORM_SDL_PLATFORMNATIVEWINDOWSDL_HPP
#define FSLNATIVEWINDOW_PLATFORM_SDL_PLATFORMNATIVEWINDOWSDL_HPP
#ifdef FSL_PLATFORM_EMSCRIPTEN
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslNativeWindow/Base/VirtualMouseButtonFlags.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindow.hpp>
#include <SDL2/SDL.h>
#include <optional>
#include <vector>

namespace Fsl
{
  class PlatformNativeWindowSDL : public PlatformNativeWindow
  {
    struct CachedMouseRecord
    {
      PxPoint2 LastPositionPx;
    };

    SDL_Window* m_pSdlWindow{nullptr};
    bool m_isActivated{false};
    CachedMouseRecord m_cachedMouse;
    PxSize2D m_cachedSize;
    std::optional<Vector2> m_cachedDpi;

  public:
    PlatformNativeWindowSDL(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                            const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams);
    ~PlatformNativeWindowSDL() override;

    bool TryCaptureMouse(const bool enableCapture) override;

    void Dispose() noexcept;
    void OnMouseMotion(INativeWindowEventQueue& eventQueue, const SDL_MouseMotionEvent& theEvent);
    void OnMouseButton(INativeWindowEventQueue& eventQueue, const SDL_MouseButtonEvent& theEvent);
    void OnMouseWheel(INativeWindowEventQueue& eventQueue, const SDL_MouseWheelEvent& theEvent);
    void OnWindowEvent(INativeWindowEventQueue& eventQueue, const SDL_WindowEvent& theEvent);


    SDL_Window* GetSdlWindow() const
    {
      return m_pSdlWindow;
    }

  protected:
    bool TryGetNativeSize(PxPoint2& rSize) const override;
    bool TryGetNativeDpi(Vector2& rDPI) const override;
  };
}

#endif
#endif
