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
#include <FslNativeWindow/Vulkan/Adapter/IVulkanNativeWindowAdapter.hpp>
#include <FslNativeWindow/Vulkan/Adapter/IVulkanNativeWindowSystemAdapter.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindow.hpp>
#include <FslNativeWindow/Vulkan/VulkanNativeWindow.hpp>
#include <FslNativeWindow/Vulkan/VulkanNativeWindowSystem.hpp>
#include <utility>

namespace Fsl
{
  VulkanNativeWindowSystem::VulkanNativeWindowSystem(const NativeWindowSystemSetup& setup, std::unique_ptr<IVulkanNativeWindowSystemAdapter> adapter)
    : PlatformNativeWindowSystem(setup, std::move(adapter))
    , m_pCachedAdapter(dynamic_cast<IVulkanNativeWindowSystemAdapter*>(TryGetAdapter()))
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw std::invalid_argument("can not be null");
    }
  }

  VulkanNativeWindowSystem::~VulkanNativeWindowSystem() = default;


  std::shared_ptr<INativeWindow>
    VulkanNativeWindowSystem::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                 const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw ObjectShutdownException("CreateNativeWindow");
    }

    std::shared_ptr<IVulkanNativeWindowAdapter> adapterEx;
    {
      std::shared_ptr<IPlatformNativeWindowAdapter> adapter =
        m_pCachedAdapter->CreateNativeWindow(nativeWindowSetup, pPlatformCustomWindowAllocationParams);

      adapterEx = std::dynamic_pointer_cast<IVulkanNativeWindowAdapter>(adapter);
      if (!adapterEx)
      {
        throw InternalErrorException("window not of the correct type");
      }
    }
    return std::make_shared<VulkanNativeWindow>(adapterEx);
  }


  void VulkanNativeWindowSystem::Shutdown()
  {
    m_pCachedAdapter = nullptr;
    PlatformNativeWindowSystem::Shutdown();
  }


  std::string VulkanNativeWindowSystem::GetKHRSurfaceExtensionName() const
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw ObjectShutdownException("GetKHRSurfaceExtensionName");
    }
    return m_pCachedAdapter->GetKHRSurfaceExtensionName();
  }


  PlatformNativeDisplayType VulkanNativeWindowSystem::GetDisplayType() const
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw ObjectShutdownException("GetDisplayType");
    }
    return m_pCachedAdapter->GetDisplayType();
  }


  std::shared_ptr<IVulkanNativeWindow> VulkanNativeWindowSystem::CreateVulkanNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                                                          const NativeVulkanSetup& nativeVulkanSetup)
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw ObjectShutdownException("CreateVulkanNativeWindow");
    }

    std::shared_ptr<IVulkanNativeWindowAdapter> adapter = m_pCachedAdapter->CreateVulkanNativeWindow(nativeWindowSetup, nativeVulkanSetup);
    return std::make_shared<VulkanNativeWindow>(adapter);
  }
}
