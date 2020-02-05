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

#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoHost/Base/ADemoHostOptionParser.hpp>
#include <cstdlib>
#include <cstring>

namespace Fsl
{
  namespace
  {
    const auto ENV_PREFERRED_WINDOW_RESOLUTION = "FSLDEMOAPP_PREFERRED_WINDOW_RESOLUTION";

    struct CommandId
    {
      enum Enum
      {
        Window = DEMO_HOST_OPTION_BASE_INTERNAL,
        DisplayId
      };
    };
  }


  ADemoHostOptionParser::ADemoHostOptionParser()
    : m_nativeWindowConfig(WindowMode::Fullscreen, Rectangle())
    , m_userControlledWindow(false)
  {
  }


  void ADemoHostOptionParser::ArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("Window", OptionArgument::OptionRequired, CommandId::Window, "Window mode [left,top,width,height]", OptionGroup::Host);
    rOptions.emplace_back("DisplayId", OptionArgument::OptionRequired, CommandId::DisplayId, "DisplayId <number>", OptionGroup::Host);
  }


  OptionParseResult ADemoHostOptionParser::Parse(const int32_t cmdId, const char* const pszOptArg)
  {
    Rectangle rectValue;
    int32_t intValue;
    switch (cmdId)
    {
    case CommandId::Window:
      if (std::strcmp(pszOptArg, "[]") != 0)
      {
        StringParseUtil::Parse(rectValue, pszOptArg);
        m_nativeWindowConfig.SetWindowMode(WindowMode::Window);
        m_nativeWindowConfig.SetWindowRectangle(rectValue);
        m_userControlledWindow = true;
      }
      else
      {
        m_nativeWindowConfig.SetWindowMode(WindowMode::Fullscreen);
        m_userControlledWindow = true;
      }
      return OptionParseResult::Parsed;
    case CommandId::DisplayId:
      StringParseUtil::Parse(intValue, pszOptArg);
      m_nativeWindowConfig.SetDisplayId(intValue);
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool ADemoHostOptionParser::ParsingComplete()
  {
    if (m_nativeWindowConfig.GetWindowMode() == WindowMode::Fullscreen && !m_userControlledWindow)
    {
#ifdef _WIN32
#pragma warning(push)
      // Disable the warning about unsafe method under windows (unfortunately visual studio does not remove this warning for C++11)
#pragma warning(disable : 4996)
#endif
      auto psz = std::getenv(ENV_PREFERRED_WINDOW_RESOLUTION);
#ifdef _WIN32
#pragma warning(pop)
#endif

      if (psz != nullptr)
      {
        FSLLOG3_INFO("Using environment variable '{}' = '{}' to set window resolution.", ENV_PREFERRED_WINDOW_RESOLUTION, psz);
        Rectangle rectValue;
        StringParseUtil::Parse(rectValue, psz);
        m_nativeWindowConfig.SetWindowMode(WindowMode::Window);
        m_nativeWindowConfig.SetWindowRectangle(rectValue);
      }
    }
    return true;
  }


  NativeWindowConfig ADemoHostOptionParser::GetNativeWindowConfig() const
  {
    return m_nativeWindowConfig;
  }


  void ADemoHostOptionParser::SetNativeWindowTag(const std::shared_ptr<ITag>& tag)
  {
    m_nativeWindowTag = tag;
  }


  std::shared_ptr<ITag> ADemoHostOptionParser::GetNativeWindowTag() const
  {
    return m_nativeWindowTag;
  }
}
