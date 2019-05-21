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
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <FslDemoPlatform/DemoHostManagerOptionParser.hpp>
#include <iostream>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        ExitAfterFrame = DEMO_HOSTMANAGER_OPTION_BASE_INTERNAL,
        ExitAfterDuration,
        ScreenshotFrequency,
        ScreenshotFormat,
        ScreenshotNamePrefix,
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

    enum DurationFormat
    {
      Invalid,
      Milliseconds,
      Seconds
    };
  }

  DemoHostManagerOptionParser::DemoHostManagerOptionParser()
    : m_exitAfterFrame(-1)
    , m_screenshotConfig(TestScreenshotNameScheme::FrameNumber, ImageFormat::Png, 0, "Screenshot")
    , m_forceUpdateTime(0)
    , m_logStatsMode(LogStatsMode::Disabled)
    , m_stats(false)
    , m_appFirewall(false)
    , m_enableBasic2DPrealloc(true)
    , m_contentMonitor(false)
  {
  }


  void DemoHostManagerOptionParser::ArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("ExitAfterFrame", OptionArgument::OptionRequired, CommandId::ExitAfterFrame,
                          "Exit after the given number of frames has been rendered");
    rOptions.emplace_back(
      "ExitAfterDuration", OptionArgument::OptionRequired, CommandId::ExitAfterDuration,
      "Exit after the given duration has passed. The value can be specified in seconds or milliseconds. For example 10s or 10ms.");
    rOptions.emplace_back("LogStats", OptionArgument::OptionNone, CommandId::LogStats,
                          "Log basic rendering stats (this is equal to setting LogStatsMode to latest)");
    rOptions.emplace_back("LogStatsMode", OptionArgument::OptionRequired, CommandId::LogStatsMode,
                          "Set the log stats mode, more advanced version of LogStats. Can be disabled, latest, average");
    rOptions.emplace_back("Stats", OptionArgument::OptionNone, CommandId::Stats, "Display basic frame profiling stats");
    rOptions.emplace_back("AppFirewall", OptionArgument::OptionNone, CommandId::AppFirewall,
                          "Enable the app firewall, reporting crashes on-screen instead of exiting");
    rOptions.emplace_back("EnableBasic2DPrealloc", OptionArgument::OptionRequired, CommandId::EnableBasic2DPrealloc,
                          "Enable/disable basic2d preallocation (Stats is enabled this is forced true)", OptionGroup::Hidden);
    rOptions.emplace_back("ScreenshotFrequency", OptionArgument::OptionRequired, CommandId::ScreenshotFrequency,
                          "Create a screenshot at the given frame frequency");
    rOptions.emplace_back("ScreenshotFormat", OptionArgument::OptionRequired, CommandId::ScreenshotFormat,
                          "Chose the format for the screenshot: bmp, jpg, png or tga (defaults to png)");
    rOptions.emplace_back("ScreenshotNamePrefix", OptionArgument::OptionRequired, CommandId::ScreenshotNamePrefix,
                          "Chose the screenshot name prefix (defaults to 'Screenshot')");
    rOptions.emplace_back("ScreenshotNameScheme", OptionArgument::OptionRequired, CommandId::ScreenshotNameScheme,
                          "Chose the screenshot name scheme: frame, sequence or exact (defaults to frame)");
    rOptions.emplace_back("ContentMonitor", OptionArgument::OptionNone, CommandId::ContentMonitor,
                          "Monitor the Content directory for changes and restart the app on changes. WARNING: Might not work on all platforms "
                          "and it might impact app performance (experimental)");
    rOptions.emplace_back(
      "ForceUpdateTime", OptionArgument::OptionRequired, CommandId::ForceUpdateTime,
      "Force the update time to be the given value in microseconds (can be useful when taking a lot of screen-shots). If 0 this option is disabled");
  }


  OptionParseResult::Enum DemoHostManagerOptionParser::Parse(const int32_t cmdId, const char* const pszOptArg)
  {
    // Rectangle rectValue;
    bool boolValue;
    switch (cmdId)
    {
    case CommandId::ExitAfterFrame:
      StringParseUtil::Parse(m_exitAfterFrame, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::ExitAfterDuration:
      return ParseDurationExitConfig(pszOptArg);
    case CommandId::ScreenshotFrequency:
      StringParseUtil::Parse(m_screenshotConfig.Frequency, pszOptArg);
      return OptionParseResult::Parsed;
    case CommandId::ScreenshotFormat:
      return ParseScreenshotImageFormat(pszOptArg);
    case CommandId::ScreenshotNameScheme:
      return ParseScreenshotNameScheme(pszOptArg);
    case CommandId::ScreenshotNamePrefix:
      return ParseScreenshotNamePrefix(pszOptArg);
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
      {
        m_logStatsMode = LogStatsMode::Disabled;
      }
      else if (str == "latest")
      {
        m_logStatsMode = LogStatsMode::Latest;
      }
      else if (str == "average")
      {
        m_logStatsMode = LogStatsMode::Average;
      }
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


  TestScreenshotConfig DemoHostManagerOptionParser::GetScreenshotConfig() const
  {
    return m_screenshotConfig;
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


  OptionParseResult::Enum DemoHostManagerOptionParser::ParseDurationExitConfig(const char* const pszOptArg)
  {
    std::string input(pszOptArg);

    // First we identify the time duration format
    DurationFormat durationFormat = DurationFormat::Invalid;
    if (StringUtil::EndsWith(input, "ms"))
    {
      durationFormat = DurationFormat::Milliseconds;
      input.erase(input.size() - 2, 2);
    }
    else if (StringUtil::EndsWith(input, "s"))
    {
      durationFormat = DurationFormat::Seconds;
      input.erase(input.size() - 1, 1);
    }

    if (durationFormat == DurationFormat::Invalid || input.empty())
    {
      FSLLOG_ERROR("Unsupported duration string '" << input << "' expected a duration value like this 10s or 10ms");
      return OptionParseResult::Failed;
    }

    int32_t durationValue;
    StringParseUtil::Parse(durationValue, input.c_str());

    switch (durationFormat)
    {
    case DurationFormat::Milliseconds:
      m_exitAfterDuration = DurationExitConfig(true, std::chrono::milliseconds(durationValue));
      break;
    case DurationFormat::Seconds:
      m_exitAfterDuration = DurationExitConfig(true, std::chrono::seconds(durationValue));
      break;
    default:
      return OptionParseResult::Failed;
    }

    return OptionParseResult::Parsed;
  }


  OptionParseResult::Enum DemoHostManagerOptionParser::ParseScreenshotImageFormat(const char* const pszOptArg)
  {
    ImageFormat format = ImageFormatUtil::TryDetectImageFormat(std::string(pszOptArg));
    if (format == ImageFormat::Undefined)
    {
      FSLLOG_ERROR("Unsupported image format '" << pszOptArg << "' expected 'bmp', 'jpg', 'png' or 'tga'");
      return OptionParseResult::Failed;
    }
    switch (format)
    {
    case ImageFormat::Bmp:
    // case ImageFormat::Hdr:
    case ImageFormat::Jpeg:
    case ImageFormat::Png:
    case ImageFormat::Tga:
      m_screenshotConfig.Format = format;
      break;
    default:
      FSLLOG_ERROR("Unsupported image format '" << pszOptArg << "' expected 'bmp', 'jpg', 'png' or 'tga'");
      return OptionParseResult::Failed;
    }
    return OptionParseResult::Parsed;
  }


  OptionParseResult::Enum DemoHostManagerOptionParser::ParseScreenshotNamePrefix(const char* const pszOptArg)
  {
    try
    {
      IO::Path path(pszOptArg);
      if (IO::Path::GetFileName(path) != path)
      {
        FSLLOG_ERROR("The prefix can only contain a filename prefix, not a path '" << path << "'");
        return OptionParseResult::Failed;
      }
      m_screenshotConfig.FilenamePrefix = path.ToUTF8String();
      return OptionParseResult::Parsed;
    }
    catch (const std::exception& ex)
    {
      FSLLOG_ERROR("Failed to parse screenshot name with error: " << ex.what());
      return OptionParseResult::Failed;
    }
  }


  OptionParseResult::Enum DemoHostManagerOptionParser::ParseScreenshotNameScheme(const char* const pszOptArg)
  {
    std::string input(pszOptArg);

    if (input == "frame")
    {
      m_screenshotConfig.NamingScheme = TestScreenshotNameScheme::FrameNumber;
      return OptionParseResult::Parsed;
    }
    if (input == "sequential")
    {
      m_screenshotConfig.NamingScheme = TestScreenshotNameScheme::Sequential;
      return OptionParseResult::Parsed;
    }
    if (input == "exact")
    {
      m_screenshotConfig.NamingScheme = TestScreenshotNameScheme::Exact;
      return OptionParseResult::Parsed;
    }
    FSLLOG_ERROR("Unsupported ScreenshotNameScheme '" << input << "' expected 'frame', 'sequential 'or 'exact'.");
    return OptionParseResult::Failed;
  }
}
