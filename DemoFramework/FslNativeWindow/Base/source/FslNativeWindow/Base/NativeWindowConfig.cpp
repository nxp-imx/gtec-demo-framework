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

#include <FslNativeWindow/Base/NativeWindowConfig.hpp>

namespace Fsl
{
  NativeWindowConfig::NativeWindowConfig()
    : m_windowMode(WindowMode::Fullscreen)
    , m_displayId(0)
  {
  }


  NativeWindowConfig::NativeWindowConfig(const WindowMode windowMode, const Rectangle& windowRectangle)
    : m_windowMode(windowMode)
    , m_windowRectangle(windowRectangle)
    , m_displayId(0)
  {
  }


  WindowMode NativeWindowConfig::GetWindowMode() const
  {
    return m_windowMode;
  }


  void NativeWindowConfig::SetWindowMode(const WindowMode windowMode)
  {
    m_windowMode = windowMode;
  }


  Rectangle NativeWindowConfig::GetWindowRectangle() const
  {
    return m_windowRectangle;
  }


  void NativeWindowConfig::SetWindowRectangle(const Rectangle& rect)
  {
    m_windowRectangle = rect;
  }


  int32_t NativeWindowConfig::GetDisplayId() const
  {
    return m_displayId;
  }


  void NativeWindowConfig::SetDisplayId(const int32_t displayId)
  {
    m_displayId = std::max(displayId, static_cast<int32_t>(0));
  }


  Optional<uint32_t> NativeWindowConfig::GetForcedDensityDpi() const
  {
    return m_forcedDensityDpi;
  }


  void NativeWindowConfig::SetForcedDensityDpi(const Optional<uint32_t>& densityDpi)
  {
    m_forcedDensityDpi = densityDpi;
  }


  Optional<Point2U> NativeWindowConfig::GetForcedActualDpi() const
  {
    return m_forcedActualDpi;
  }


  void NativeWindowConfig::SetForcedActualDpi(const Optional<Point2U>& actualDpi)
  {
    m_forcedActualDpi = actualDpi;
  }
}
