/****************************************************************************************************************************************************
 * Copyright 2025 NXP
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

#include "Profiles.hpp"
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/System/ErrorFormatter.hpp>
#include <vector>
#include "CustomWriter.hpp"

namespace Fsl
{
// Check if Vulkan 1.3.280 is available
#if VK_HEADER_VERSION >= 280

  namespace
  {
    std::vector<VpProfileProperties> EnumerateProfiles(const VpCapabilities capabilities)
    {
      uint32_t count = 0;
      RAPIDVULKAN_CHECK2(vpGetProfiles(capabilities, &count, nullptr), "failed to acquire the count");

      std::vector<VpProfileProperties> result(count);
      RAPIDVULKAN_CHECK2(vpGetProfiles(capabilities, &count, result.data()), "failed to enumerate profiles properties");
      return result;
    }
  }


  Profiles::Profiles()
  {
    VpCapabilitiesCreateInfo createInfo;
    createInfo.apiVersion = VK_API_VERSION_1_1;
    createInfo.flags = VP_PROFILE_CREATE_STATIC_BIT;
    createInfo.pVulkanFunctions = nullptr;

    const auto res = vpCreateCapabilities(&createInfo, nullptr, &m_capabilities);
    if (res != VK_SUCCESS)
    {
      FSLLOG3_VERBOSE(RapidVulkan::ErrorFormatter::Format("vpCreateCapabilities failed with:", res));
      m_capabilities = VK_NULL_HANDLE;
    }
  }


  Profiles::~Profiles()
  {
    if (m_capabilities != VK_NULL_HANDLE)
    {
      vpDestroyCapabilities(m_capabilities, nullptr);
      m_capabilities = VK_NULL_HANDLE;
    }
  }


  void Profiles::LogProfiles(CustomWriter& rWriter)
  {
    if (m_capabilities == VK_NULL_HANDLE)
    {
      rWriter.Print("Profiles: Not available");
      return;
    }

    rWriter.Print("Profiles:");
    std::vector<VpProfileProperties> profiles = EnumerateProfiles(m_capabilities);
    for (const auto& entry : profiles)
    {
      rWriter.Print("- {}: {}", entry.profileName, entry.specVersion);
    }
  }

#else

  Profiles::Profiles()
  {
  }


  Profiles::~Profiles() = default;


  void Profiles::LogProfiles(CustomWriter& rWriter)
  {
    rWriter.Print("Profiles: Not available");
  }

#endif
}
