#ifndef FSLNATIVEWINDOW_PLATFORM_ADAPTER_SDL_PLATFORMNATIVEWINDOWSYSTEMADAPTERSDL_HPP
#define FSLNATIVEWINDOW_PLATFORM_ADAPTER_SDL_PLATFORMNATIVEWINDOWSYSTEMADAPTERSDL_HPP
#ifdef FSL_WINDOWSYSTEM_SDL2
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

#include <FslNativeWindow/Platform/Adapter/PlatformNativeWindowSystemAdapter.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowAllocationFunction.hpp>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  class INativeWindowEventQueue;
  class PlatformNativeWindowAdapterSDL;

  class PlatformNativeWindowSystemAdapterSDL : public PlatformNativeWindowSystemAdapter
  {
    struct WindowRecord
    {
      std::weak_ptr<PlatformNativeWindowAdapterSDL> Window;
      Uint32 WindowId{0};

      WindowRecord() = default;
      WindowRecord(std::weak_ptr<PlatformNativeWindowAdapterSDL> window, const Uint32 windowId);
      ~WindowRecord();
    };

    PlatformNativeWindowAllocationFunction m_allocationFunction;
    std::weak_ptr<INativeWindowEventQueue> m_eventQueue;
    std::vector<WindowRecord> m_windows;

  public:
    explicit PlatformNativeWindowSystemAdapterSDL(const NativeWindowSystemSetup& setup,
                                                  const PlatformNativeWindowAllocationFunction& allocateWindowFunction = nullptr,
                                                  const PlatformNativeWindowSystemParams& systemParams = PlatformNativeWindowSystemParams());
    ~PlatformNativeWindowSystemAdapterSDL() override;

    std::shared_ptr<IPlatformNativeWindowAdapter>
      CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                         const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams = nullptr) override;
    bool ProcessMessages(const NativeWindowProcessMessagesArgs& args) override;

  private:
    void Dispose() noexcept;
    void OnKeyEvent(INativeWindowEventQueue& eventQueue, const SDL_KeyboardEvent& theEvent, const bool pressed);
    void OnMouseMotion(INativeWindowEventQueue& eventQueue, const SDL_MouseMotionEvent& theEvent);
    void OnMouseButton(INativeWindowEventQueue& eventQueue, const SDL_MouseButtonEvent& theEvent);
    void OnMouseWheel(INativeWindowEventQueue& eventQueue, const SDL_MouseWheelEvent& theEvent);
    void OnWindowEvent(INativeWindowEventQueue& eventQueue, const SDL_WindowEvent& theEvent);

    std::optional<std::shared_ptr<PlatformNativeWindowAdapterSDL>> TryFindWindow(const Uint32 windowId);
  };
}

#endif
#endif
