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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslDemoHost/Vulkan/VulkanDemoHostOptionParser.hpp>
#include <algorithm>
#include <array>
#include <cstring>
#include <string>
#include <utility>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        VkPhysicalDevice = DEMO_HOST_OPTION_BASE,
        VkValidate,
        VkApiDump,
        VkPresentMode,
        LogExtensions,
        LogLayers,
        LogSurfaceFormats,
        VkScreenshot,
      };
    };


    struct PresentMode
    {
      VkPresentModeKHR Mode;
      StringViewLite StrMode;

      constexpr PresentMode(const VkPresentModeKHR mode, StringViewLite str)
        : Mode(mode)
        , StrMode(str)
      {
      }
    };

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    constexpr const PresentMode g_presentModes[] = {
      PresentMode(VK_PRESENT_MODE_IMMEDIATE_KHR, "VK_PRESENT_MODE_IMMEDIATE_KHR"),
      PresentMode(VK_PRESENT_MODE_MAILBOX_KHR, "VK_PRESENT_MODE_MAILBOX_KHR"),
      PresentMode(VK_PRESENT_MODE_FIFO_KHR, "VK_PRESENT_MODE_FIFO_KHR"),
      PresentMode(VK_PRESENT_MODE_FIFO_RELAXED_KHR, "VK_PRESENT_MODE_FIFO_RELAXED_KHR"),
#if VK_HEADER_VERSION >= 51
      PresentMode(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR, "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR"),
      PresentMode(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR, "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR"),
#endif
    };
    constexpr auto g_presentModeCount = sizeof(g_presentModes) / sizeof(PresentMode);


    std::string GetPresentModesString()
    {
      fmt::memory_buffer buf;
      fmt::format_to(std::back_inserter(buf), "{} ({})", g_presentModes[0].StrMode, static_cast<int64_t>(g_presentModes[0].Mode));
      for (std::size_t i = 1; i < g_presentModeCount; ++i)
      {
        fmt::format_to(std::back_inserter(buf), ", {} ({})", g_presentModes[i].StrMode, static_cast<int64_t>(g_presentModes[i].Mode));
      }
      return fmt::to_string(buf);
    }

    bool TryParseAsString(VkPresentModeKHR& rPresentMode, const StringViewLite& strOptArg)
    {
      // Try to see if we can find a string match
      for (std::size_t i = 0; i < g_presentModeCount; ++i)
      {
        if (strOptArg == g_presentModes[i].StrMode)
        {
          rPresentMode = g_presentModes[i].Mode;
          return true;
        }
      }
      rPresentMode = VK_PRESENT_MODE_FIFO_KHR;
      return false;
    }

    bool TryParseAsValue(VkPresentModeKHR& rPresentMode, const StringViewLite& strOptArg)
    {
      rPresentMode = VK_PRESENT_MODE_FIFO_KHR;
      // Try to see if we can parse it as a number
      int32_t value = 0;
      try
      {
        StringParseUtil::Parse(value, strOptArg);
      }
      catch (const std::exception&)
      {
        return false;
      }
      for (std::size_t i = 0; i < g_presentModeCount; ++i)
      {
        if (value == g_presentModes[i].Mode)
        {
          rPresentMode = g_presentModes[i].Mode;
          return true;
        }
      }
      FSLLOG3_WARNING("Unknown presentMode '{}' supplied, trying it out", value);
      rPresentMode = static_cast<VkPresentModeKHR>(value);
      return true;
    }

    bool TryParse(VkPresentModeKHR& rPresentMode, const StringViewLite& strOptArg)
    {
      if (TryParseAsString(rPresentMode, strOptArg))
      {
        return true;
      }
      return TryParseAsValue(rPresentMode, strOptArg);
    }
  }


  VulkanDemoHostOptionParser::VulkanDemoHostOptionParser()
    : ADemoHostOptionParser(DemoHostOptionConfig::WindowApp)
  {
  }

  void VulkanDemoHostOptionParser::ArgumentSetup(std::deque<Option>& rOptions)
  {
    ADemoHostOptionParser::ArgumentSetup(rOptions);

    auto presentModes = GetPresentModesString();
    std::string presentModeDesc = std::string("Override the present mode with the supplied value. Known values: ") + presentModes;

    rOptions.emplace_back("VkPhysicalDevice", OptionArgument::OptionRequired, CommandId::VkPhysicalDevice, "Set the physical device index.",
                          OptionGroup::Host);
    rOptions.emplace_back("VkValidate", OptionArgument::OptionRequired, CommandId::VkValidate,
                          "Enable/disable the VK_LAYER_LUNARG_standard_validation layer.", OptionGroup::Host);
    rOptions.emplace_back("VkApiDump", OptionArgument::OptionNone, CommandId::VkApiDump, "Enable the VK_LAYER_LUNARG_api_dump layer.",
                          OptionGroup::Host);
    rOptions.emplace_back("VkPresentMode", OptionArgument::OptionRequired, CommandId::VkPresentMode, presentModeDesc, OptionGroup::Host);
    rOptions.emplace_back("LogExtensions", OptionArgument::OptionNone, CommandId::LogExtensions, "Output the extensions to the log",
                          OptionGroup::Host);
    rOptions.emplace_back("LogLayers", OptionArgument::OptionNone, CommandId::LogLayers, "Output the layers to the log", OptionGroup::Host);
    rOptions.emplace_back("LogSurfaceFormats", OptionArgument::OptionNone, CommandId::LogSurfaceFormats,
                          "Output the supported surface formats to the log", OptionGroup::Host);
    rOptions.emplace_back("VkScreenshot", OptionArgument::OptionRequired, CommandId::VkScreenshot,
                          "Enable/disable screenshot support (defaults to enabled)", OptionGroup::Host);
  }


  OptionParseResult VulkanDemoHostOptionParser::Parse(const int cmdId, const StringViewLite& strOptArg)
  {
    bool boolValue = false;
    switch (cmdId)
    {
    case CommandId::VkPhysicalDevice:
      StringParseUtil::Parse(m_physicalDeviceIndex, strOptArg);
      return OptionParseResult::Parsed;
    case CommandId::VkValidate:
      StringParseUtil::Parse(boolValue, strOptArg);
      m_validationLayer = boolValue ? OptionUserChoice::On : OptionUserChoice::Off;
      return OptionParseResult::Parsed;
    case CommandId::VkApiDump:
      m_apiDump = OptionUserChoice::On;
      return OptionParseResult::Parsed;
    case CommandId::VkPresentMode:
      {
        VkPresentModeKHR presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
        if (TryParse(presentMode, strOptArg))
        {
          m_launchOptions.OverridePresentMode = true;
          m_launchOptions.PresentMode = presentMode;
          return OptionParseResult::Parsed;
        }

        FSLLOG3_INFO("Known presentMode value or strings: {}", GetPresentModesString());
        return OptionParseResult::Failed;
      }
    case CommandId::LogExtensions:
      m_logExtensions = true;
      m_launchOptions.LogDeviceExtensions = true;
      return OptionParseResult::Parsed;
    case CommandId::LogLayers:
      m_logLayers = true;
      return OptionParseResult::Parsed;
    case CommandId::LogSurfaceFormats:
      m_logSurfaceFormats = true;
      return OptionParseResult::Parsed;
    case CommandId::VkScreenshot:
      StringParseUtil::Parse(boolValue, strOptArg);
      m_launchOptions.ScreenshotsEnabled = boolValue ? OptionUserChoice::On : OptionUserChoice::Off;
      return OptionParseResult::Parsed;
    default:
      return ADemoHostOptionParser::Parse(cmdId, strOptArg);
    }
  }


  bool VulkanDemoHostOptionParser::ParsingComplete()
  {
    return ADemoHostOptionParser::ParsingComplete();
  }
}
