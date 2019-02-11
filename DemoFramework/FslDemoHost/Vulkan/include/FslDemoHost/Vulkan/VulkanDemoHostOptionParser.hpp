#ifndef FSLDEMOHOST_VULKAN_VULKANDEMOHOSTOPTIONPARSER_HPP
#define FSLDEMOHOST_VULKAN_VULKANDEMOHOSTOPTIONPARSER_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslDemoHost/Base/ADemoHostOptionParser.hpp>
#include <FslDemoHost/Vulkan/Config/OptionUserChoice.hpp>
#include <FslDemoHost/Vulkan/Config/VulkanLaunchOptions.hpp>

namespace Fsl
{
  class VulkanDemoHostOptionParser : public ADemoHostOptionParser
  {
    uint32_t m_physicalDeviceIndex = 0;
    OptionUserChoice m_validationLayer = OptionUserChoice::Default;
    OptionUserChoice m_apiDump = OptionUserChoice::Off;
    VulkanLaunchOptions m_launchOptions;
    bool m_logExtensions = false;
    bool m_logLayers = false;
    bool m_logSurfaceFormats = false;

  public:
    VulkanDemoHostOptionParser();

    uint32_t GetPhysicalDeviceIndex() const
    {
      return m_physicalDeviceIndex;
    }

    OptionUserChoice GetValidationLayerChoice() const
    {
      return m_validationLayer;
    }

    OptionUserChoice GetApiDumpChoice() const
    {
      return m_apiDump;
    }

    VulkanLaunchOptions GetLaunchOptions() const
    {
      return m_launchOptions;
    }

    bool IsLogExtensionsEnabled() const
    {
      return m_logExtensions;
    }

    bool IsLogLayersEnabled() const
    {
      return m_logLayers;
    }

    bool IsLogSurfaceFormatsEnabled() const
    {
      return m_logSurfaceFormats;
    }

    void ArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult::Enum Parse(const int cmdId, const char* const pszOptArg) override;
    bool ParsingComplete() override;
  };
}

#endif
