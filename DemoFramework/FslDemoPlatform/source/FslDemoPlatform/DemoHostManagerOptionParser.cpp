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

#include <iostream>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslDemoPlatform/DemoHostManagerOptionParser.hpp>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        ExitAfterFrame = DEMO_HOSTMANAGER_OPTION_BASE_INTERNAL,
        ScreenshotFrequency,
        LogStats,
        LogStatsMode,
        Stats,
        AppFirewall,
        ContentMonitor,
        EnableBasic2DPrealloc,
        ScreenshotNameScheme,
        ForceUpdateTime
      };
    };
  }

  DemoHostManagerOptionParser::DemoHostManagerOptionParser()
    : m_exitAfterFrame(-1)
    , m_screenshotFrequency(0)
    , m_forceUpdateTime(0)
    , m_screenshotNameScheme(TestScreenshotNameScheme::FrameNumber)
    , m_logStatsMode(LogStatsMode::Disabled)
    , m_stats(false)
    , m_appFirewall(false)
    , m_enableBasic2DPrealloc(true)
    , m_contentMonitor(false)
  {
  }


  void DemoHostManagerOptionParser::ArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.push_back(Option("ExitAfterFrame", OptionArgument::OptionRequired, CommandId::ExitAfterFrame, "Exit after the given number of frames has been rendered"));
    rOptions.push_back(Option("ScreenshotFrequency", OptionArgument::OptionRequired, CommandId::ScreenshotFrequency, "Create a screenshot at the given frame frequency"));
    rOptions.push_back(Option("LogStats", OptionArgument::OptionNone, CommandId::LogStats, "Log basic rendering stats (this is equal to setting LogStatsMode to latest)"));
    rOptions.push_back(Option("LogStatsMode", OptionArgument::OptionRequired, CommandId::LogStatsMode, "Set the log stats mode, more advanced version of LogStats. Can be disabled, latest, average"));
    rOptions.push_back(Option("Stats", OptionArgument::OptionNone, CommandId::Stats, "Display basic frame profiling stats"));
    rOptions.push_back(Option("AppFirewall", OptionArgument::OptionNone, CommandId::AppFirewall, "Enable the app firewall, reporting crashes on-screen instead of exiting"));
    rOptions.push_back(Option("EnableBasic2DPrealloc", OptionArgument::OptionRequired, CommandId::EnableBasic2DPrealloc, "Enable/disable basic2d preallocation (Stats is enabled this is forced true)", OptionGroup::Hidden));
    rOptions.push_back(Option("ScreenshotNameScheme", OptionArgument::OptionRequired, CommandId::ScreenshotNameScheme, "Chose the screenshot name scheme (0=frame number, 1=sequence number)", OptionGroup::Hidden));
    rOptions.push_back(Option("ContentMonitor", OptionArgument::OptionNone, CommandId::ContentMonitor, "Monitor the Content directory for changes and restart the app on changes. WARNING: Might not work on all platforms and it might impact app performance (experimental)"));
    rOptions.push_back(Option("ForceUpdateTime", OptionArgument::OptionRequired, CommandId::ForceUpdateTime, "Force the update time to be the given value in microseconds (can be useful when taking a lot of screen-shots). If 0 this option is disabled"));
  }


  OptionParseResult::Enum DemoHostManagerOptionParser::Parse(const int32_t cmdId, const char*const pszOptArg)
  {
    int32_t intValue;
    Rectangle rectValue;
    bool boolValue;
    switch (cmdId)
    {
    case CommandId::ExitAfterFrame:
      StringParseUtil::Parse(m_exitAfterFrame, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::ScreenshotFrequency:
      StringParseUtil::Parse(m_screenshotFrequency, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::ScreenshotNameScheme:
      StringParseUtil::Parse(intValue, pszOptArg);
      if( intValue == 0 )
      {
        m_screenshotNameScheme = TestScreenshotNameScheme::FrameNumber;
        return OptionParseResult::Parsed;
      }
      else if (intValue == 1)
      {
        m_screenshotNameScheme = TestScreenshotNameScheme::Sequential;
        return OptionParseResult::Parsed;
      }
      return OptionParseResult::Failed;
    case CommandId::ForceUpdateTime:
      StringParseUtil::Parse(m_forceUpdateTime, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::LogStats:
      m_logStatsMode = LogStatsMode::Latest;
      return OptionParseResult::Parsed;
    case CommandId::LogStatsMode:
    {
      std::string str(pszOptArg);
      if (str == "disabled")
        m_logStatsMode = LogStatsMode::Disabled;
      else if (str == "latest")
        m_logStatsMode = LogStatsMode::Latest;
      else if (str == "average")
        m_logStatsMode = LogStatsMode::Average;
      else
      {
        throw std::invalid_argument(std::string("Unknown logStatsMode parameter " + str));
      }
      return OptionParseResult::Parsed;
    }
    case CommandId::Stats:
      m_stats = true;
      return OptionParseResult::Parsed;
    case CommandId::AppFirewall:
      m_appFirewall = true;
      return OptionParseResult::Parsed;
    case CommandId::ContentMonitor:
      m_contentMonitor = true;
      m_appFirewall = true;
      return OptionParseResult::Parsed;
    case CommandId::EnableBasic2DPrealloc:
      StringParseUtil::Parse(boolValue, pszOptArg);
      m_enableBasic2DPrealloc = boolValue;
      return OptionParseResult::Parsed;
    default:
      break;
    }
    return OptionParseResult::NotHandled;
  }


  bool DemoHostManagerOptionParser::ParsingComplete()
  {
    return true;
  }


  int32_t DemoHostManagerOptionParser::GetExitAfterFrame() const
  {
    return m_exitAfterFrame;
  }


  uint32_t DemoHostManagerOptionParser::GetForceUpdateTime() const
  {
    return m_forceUpdateTime;
  }


  uint32_t DemoHostManagerOptionParser::GetScreenshotFrequency() const
  {
    return m_screenshotFrequency;
  }


  TestScreenshotNameScheme DemoHostManagerOptionParser::GetScreenshotNameScheme() const
  {
    return m_screenshotNameScheme;
  }


  LogStatsMode DemoHostManagerOptionParser::GetLogStatsMode() const
  {
    return m_logStatsMode;
  }


  bool DemoHostManagerOptionParser::IsStatsEnabled() const
  {
    return m_stats;
  }



  bool DemoHostManagerOptionParser::IsAppFirewallEnabled() const
  {
    return m_appFirewall;
  }


  bool DemoHostManagerOptionParser::IsBasic2DPreallocEnabled() const
  {
    return m_stats || m_enableBasic2DPrealloc;
  }


  bool DemoHostManagerOptionParser::IsContentMonitorEnabled() const
  {
    return m_contentMonitor;
  }


  void DemoHostManagerOptionParser::RequestEnableAppFirewall()
  {
    m_appFirewall = true;
  }

}
