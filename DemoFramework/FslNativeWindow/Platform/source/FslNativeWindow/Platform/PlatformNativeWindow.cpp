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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslNativeWindow/Platform/Adapter/IPlatformNativeWindowAdapter.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindow.hpp>
#include <utility>

namespace Fsl
{
  // For now this is just a proxy
  PlatformNativeWindow::PlatformNativeWindow(std::shared_ptr<IPlatformNativeWindowAdapter> adapter)
    : m_adapter(std::move(adapter))
  {
    if (!m_adapter)
    {
      throw std::invalid_argument("can not be null");
    }
  }


  PlatformNativeWindow::~PlatformNativeWindow() = default;


  NativeWindowMetrics PlatformNativeWindow::GetWindowMetrics() const
  {
    if (!m_adapter)
    {
      throw ObjectShutdownException("GetWindowMetrics");
    }
    return m_adapter->GetWindowMetrics();
  }


  NativeWindowCapabilityFlags PlatformNativeWindow::GetCapabilityFlags() const
  {
    if (!m_adapter)
    {
      throw ObjectShutdownException("GetCapabilityFlags");
    }
    return m_adapter->GetCapabilityFlags();
  }


  bool PlatformNativeWindow::TryGetDpi(Vector2& rDPI) const
  {
    if (!m_adapter)
    {
      FSLLOG3_WARNING("TryGetDpi: Object shutdown");
      return false;
    }
    return m_adapter->TryGetDpi(rDPI);
  }


  bool PlatformNativeWindow::TryGetDensityDpi(uint32_t& rDensityDpi) const
  {
    if (!m_adapter)
    {
      FSLLOG3_WARNING("TryGetDensityDpi: Object shutdown");
      return false;
    }
    return m_adapter->TryGetDensityDpi(rDensityDpi);
  }


  bool PlatformNativeWindow::TryGetExtent(PxExtent2D& rExtent) const
  {
    if (!m_adapter)
    {
      FSLLOG3_WARNING("TryGetExtent: Object shutdown");
      return false;
    }
    return m_adapter->TryGetExtent(rExtent);
  }


  bool PlatformNativeWindow::TryCaptureMouse(const bool enableCapture)
  {
    if (!m_adapter)
    {
      FSLLOG3_WARNING("TryCaptureMouse: Object shutdown");
      return false;
    }
    return m_adapter->TryCaptureMouse(enableCapture);
  }
}
