#ifndef FSLNATIVEWINDOW_EGL_EGLNATIVEWINDOW_HPP
#define FSLNATIVEWINDOW_EGL_EGLNATIVEWINDOW_HPP
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

#include <FslNativeWindow/EGL/IEGLNativeWindow.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindow.hpp>

namespace Fsl
{
  class IEGLNativeWindowAdapter;

  class EGLNativeWindow final
    : public PlatformNativeWindow
    , public virtual IEGLNativeWindow
  {
    std::shared_ptr<IEGLNativeWindowAdapter> m_adapter;

  public:
    explicit EGLNativeWindow(std::shared_ptr<IEGLNativeWindowAdapter> adapter);
    ~EGLNativeWindow() final;

    // INativeWindow

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::GetCapabilityFlags;
    NativeWindowCapabilityFlags GetCapabilityFlags() const final
    {
      return PlatformNativeWindow::GetCapabilityFlags();
    }

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::GetWindowMetrics;
    NativeWindowMetrics GetWindowMetrics() const final
    {
      return PlatformNativeWindow::GetWindowMetrics();
    }

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::TryGetDpi;
    bool TryGetDpi(Vector2& rDPI) const final
    {
      return PlatformNativeWindow::TryGetDpi(rDPI);
    }

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::TryGetDensityDpi;
    bool TryGetDensityDpi(uint32_t& rDensityDpi) const final
    {
      return PlatformNativeWindow::TryGetDensityDpi(rDensityDpi);
    }

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::TryGetExtent;
    bool TryGetExtent(PxExtent2D& rExtent) const final
    {
      return PlatformNativeWindow::TryGetExtent(rExtent);
    }

    // Done this way because of a "inherit via dominance warning on visual studio'
    // using PlatformNativeWindowSystem::TryCaptureMouse;
    bool TryCaptureMouse(const bool enableCapture) final
    {
      return PlatformNativeWindow::TryCaptureMouse(enableCapture);
    }

    // IEGLNativeWindow

    EGLNativeWindowType GetWindowType() const final;
  };
}

#endif
