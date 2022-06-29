#ifndef FSLNATIVEWINDOW_PLATFORM_WIN32_PLATFORMNATIVEWINDOWSYSTEMWIN32_HPP
#define FSLNATIVEWINDOW_PLATFORM_WIN32_PLATFORMNATIVEWINDOWSYSTEMWIN32_HPP
#if defined(_WIN32) && (defined(FSL_PLATFORM_WINDOWS) && !defined(FSL_PLATFORM_EMSCRIPTEN))
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

#include <FslNativeWindow/Platform/PlatformNativeWindowAllocationFunction.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystem.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace Fsl
{
  class DPIHelperWin32;
  class INativeWindowEventQueue;
  struct PlatformGamepadStateWin32;
  class PlatformNativeWindowWin32;
  class PlatformNativeWindowSystemWin32State;

  class PlatformNativeWindowSystemWin32 : public PlatformNativeWindowSystem
  {
    PlatformNativeWindowAllocationFunction m_allocationFunction;
    std::shared_ptr<PlatformNativeWindowSystemWin32State> m_windowSystemState;
    std::shared_ptr<DPIHelperWin32> m_dpiHelper;
    std::weak_ptr<INativeWindowEventQueue> m_eventQueue;
    std::unique_ptr<std::vector<PlatformGamepadStateWin32>> m_gamepadState;

  public:
    explicit PlatformNativeWindowSystemWin32(const NativeWindowSystemSetup& setup,
                                             const PlatformNativeWindowAllocationFunction& allocateWindowFunction = nullptr,
                                             const PlatformNativeWindowSystemParams& systemParams = PlatformNativeWindowSystemParams());
    ~PlatformNativeWindowSystemWin32() override;

    std::shared_ptr<INativeWindow>
      CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                         const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams = nullptr) override;
    bool ProcessMessages(const NativeWindowProcessMessagesArgs& args) override;

  private:
    void ScanGamepads();
  };
}    // namespace Fsl

#endif
#endif
