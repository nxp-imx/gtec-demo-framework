#ifndef FSLNATIVEWINDOW_EGL_EGLNATIVEWINDOWSYSTEM_HPP
#define FSLNATIVEWINDOW_EGL_EGLNATIVEWINDOWSYSTEM_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslNativeWindow/EGL/IEGLNativeWindowSystem.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystem.hpp>
#include <memory>

namespace Fsl
{
  class IEGLNativeWindowSystemAdapter;

  class EGLNativeWindowSystem final
    : public PlatformNativeWindowSystem
    , public virtual IEGLNativeWindowSystem
  {
    IEGLNativeWindowSystemAdapter* m_pCachedAdapter{nullptr};

  public:
    explicit EGLNativeWindowSystem(const NativeWindowSystemSetup& setup, std::unique_ptr<IEGLNativeWindowSystemAdapter> adapter);
    ~EGLNativeWindowSystem() final;

    // INativeWindowSystem

    std::shared_ptr<INativeWindow> CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                      const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams) final;

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::ProcessMessages;
    bool ProcessMessages(const NativeWindowProcessMessagesArgs& args) final
    {
      return PlatformNativeWindowSystem::ProcessMessages(args);
    }

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::IsDisplayHDRCompatible;
    bool IsDisplayHDRCompatible(const int32_t displayId) const final
    {
      return PlatformNativeWindowSystem::IsDisplayHDRCompatible(displayId);
    }

    // We need to override this here to clear the m_pCachedAdapter
    void Shutdown() final;

    // IEGLNativeWindowSystem

    EGLNativeDisplayType GetDisplayType() const final;
    std::shared_ptr<IEGLNativeWindow> CreateEGLNativeWindow(const NativeWindowSetup& nativeWindowSetup, const NativeEGLSetup& nativeEglSetup) final;
  };
}

#endif
