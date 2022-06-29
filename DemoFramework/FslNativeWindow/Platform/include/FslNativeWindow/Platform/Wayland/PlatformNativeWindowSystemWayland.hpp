#ifndef FSLNATIVEWINDOW_PLATFORM_WAYLAND_PLATFORMNATIVEWINDOWSYSTEMWAYLAND_HPP
#define FSLNATIVEWINDOW_PLATFORM_WAYLAND_PLATFORMNATIVEWINDOWSYSTEMWAYLAND_HPP
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

#include <FslNativeWindow/Platform/PlatformNativeWindowAllocationFunction.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystem.hpp>

namespace Fsl
{
  class PlatformNativeWindowWayland;

  class PlatformNativeWindowSystemWayland : public PlatformNativeWindowSystem
  {
    PlatformNativeWindowAllocationFunction m_allocationFunction;

  public:
    PlatformNativeWindowSystemWayland(const NativeWindowSystemSetup& setup,
                                      const PlatformNativeWindowAllocationFunction& allocateWindowFunction = nullptr,
                                      const PlatformNativeWindowSystemParams& systemParams = PlatformNativeWindowSystemParams());
    virtual ~PlatformNativeWindowSystemWayland();

    virtual std::shared_ptr<INativeWindow>
      CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                         const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams = nullptr) override;
    virtual bool ProcessMessages(const NativeWindowProcessMessagesArgs& args) override;
  };
}    // namespace Fsl

#endif
#endif
