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
#include <FslNativeWindow/Vulkan/Adapter/IVulkanNativeWindowAdapter.hpp>
#include <FslNativeWindow/Vulkan/VulkanNativeWindow.hpp>

namespace Fsl
{
  VulkanNativeWindow::VulkanNativeWindow(std::shared_ptr<IVulkanNativeWindowAdapter> adapter)
    : PlatformNativeWindow(adapter)
    , m_adapter(std::move(adapter))
  {
    if (!m_adapter)
    {
      throw std::invalid_argument("can not be null");
    }
  }

  VulkanNativeWindow::~VulkanNativeWindow() = default;


  PlatformNativeWindowType VulkanNativeWindow::GetWindowType() const
  {
    if (!m_adapter)
    {
      throw ObjectShutdownException("GetWindowType");
    }
    return m_adapter->GetWindowType();
  }

  VkSurfaceKHR VulkanNativeWindow::GetVulkanSurface() const
  {
    if (!m_adapter)
    {
      throw ObjectShutdownException("GetVulkanSurface");
    }
    return m_adapter->GetVulkanSurface();
  }


  bool VulkanNativeWindow::TryGetActualSize(PxPoint2& rSize) const
  {
    if (!m_adapter)
    {
      FSLLOG3_WARNING("TryGetActualSize: Object shutdown");
      return false;
    }
    return m_adapter->TryGetActualSize(rSize);
  }
}
