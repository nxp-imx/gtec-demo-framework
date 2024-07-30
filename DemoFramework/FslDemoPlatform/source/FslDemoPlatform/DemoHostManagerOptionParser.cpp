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
#include <FslBase/Getopt/Util/OptionArg.hpp>
#include <FslBase/Getopt/Util/OptionArgUtil.hpp>
#include <FslBase/Log/Common/FmtVersionInfo2.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslDemoPlatform/DemoHostManagerOptionParser.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <FslGraphics/ToneMapping/BasicToneMapper.hpp>
#include <FslVersion/FslVersion.hpp>
#include <fmt/format.h>
#include <array>

namespace Fsl
{
  namespace
  {
    namespace ArgName
    {
      constexpr auto ExitAfterFrame = "ExitAfterFrame";
      constexpr auto ExitAfterDuration = "ExitAfterDuration";
      constexpr auto LogStats = "LogStats";
      constexpr auto LogStatsMode = "LogStatsMode";
      constexpr auto Stats = "Stats";
      constexpr auto StatsFlags = "StatsFlags";
      constexpr auto AppFirewall = "AppFirewall";
      constexpr auto EnableBasic2DPrealloc = "EnableBasic2DPrealloc";
      constexpr auto ScreenshotFrequency = "ScreenshotFrequency";
      constexpr auto ScreenshotFormat = "ScreenshotFormat";
      constexpr auto ScreenshotNameScheme = "ScreenshotNameScheme";
      constexpr auto ScreenshotNamePrefix = "ScreenshotNamePrefix";
      constexpr auto ScreenshotToneMapper = "ScreenshotToneMapper";
      constexpr auto ContentMonitor = "ContentMonitor";
      constexpr auto ForceUpdateTime = "ForceUpdateTime";
      constexpr auto Version = "Version";
    }


    struct CommandId
    {
      enum Enum
      {
        ExitAfterFrame = DEMO_HOSTMANAGER_OPTION_BASE_INTERNAL,
        ExitAfterDuration,
        ScreenshotFrequency,
        ScreenshotFormat,
        ScreenshotNamePrefix,
        ScreenshotToneMapper,
        LogStats,
        LogStatsMode,
        Stats,
        StatsFlags,
        AppFirewall,
        ContentMonitor,
        EnableBasic2DPrealloc,
        ScreenshotNameScheme,
        ForceUpdateTime,
        Version
      };
    };


    enum class DurationFormat
    {
      Invalid,
      Milliseconds,
      Seconds
    };


    constexpr std::array<OptionArg<DemoAppStatsFlags::Enum>, 2> StatsFlagsArgs = {
      OptionArg<DemoAppStatsFlags::Enum>("frame", DemoAppStatsFlags::Frame), OptionArg<DemoAppStatsFlags::Enum>("cpu", DemoAppStatsFlags::CPU)};

    //  bmp, jpg, png or tga
    constexpr std::array<OptionArg<ImageFormat>, 4> ScreenshotFormat = {
      OptionArg<ImageFormat>("bmp", ImageFormat::Bmp), OptionArg<ImageFormat>("jpg", ImageFormat::Jpeg),
      OptionArg<ImageFormat>("png", "default", ImageFormat::Png), OptionArg<ImageFormat>("tga", ImageFormat::Tga)};

    //  bmp, jpg, png, tga or HDR
    constexpr std::array<OptionArg<ImageFormat>, 5> ScreenshotFormatHDR = {
      OptionArg<ImageFormat>("bmp", ImageFormat::Bmp), OptionArg<ImageFormat>("jpg", ImageFormat::Jpeg),
      OptionArg<ImageFormat>("png", "default", ImageFormat::Png), OptionArg<ImageFormat>("tga", ImageFormat::Tga),
      OptionArg<ImageFormat>("hdr", ImageFormat::Hdr)};

    // frame, sequence or exact
    constexpr std::array<OptionArg<TestScreenshotNameScheme>, 3> ScreenshotNameSchemeArgs = {
      OptionArg<TestScreenshotNameScheme>("frame", "default", TestScreenshotNameScheme::FrameNumber),
      OptionArg<TestScreenshotNameScheme>("sequence", TestScreenshotNameScheme::Sequential),
      OptionArg<TestScreenshotNameScheme>("exact", TestScreenshotNameScheme::Exact)};

    constexpr std::array<OptionArg<BasicToneMapper>, 3> ScreenshotToneMapperArgs = {
      OptionArg<BasicToneMapper>("hable", BasicToneMapper::Hable), OptionArg<BasicToneMapper>("reinhard", BasicToneMapper::Reinhard),
      OptionArg<BasicToneMapper>("clamp", "default, colors are clamped to SDR ranges", BasicToneMapper::Clamp)};


    constexpr ReadOnlySpan<OptionArg<ImageFormat>> GetScreenshotFormat(const bool hdrEnabled) noexcept
    {
      return hdrEnabled ? SpanUtil::AsReadOnlySpan(ScreenshotFormatHDR) : SpanUtil::AsReadOnlySpan(ScreenshotFormat);
    }

    OptionParseResult TryParseStatsFlags(DemoAppStatsFlags& rFlags, const StringViewLite& strArg)
    {
      auto entries = StringUtil::Split(strArg, '|', true);

      uint32_t flags = 0u;
      for (const auto& entry : entries)
      {
        auto flag = OptionArgUtil::Parse(SpanUtil::AsReadOnlySpan(StatsFlagsArgs), entry, DemoAppStatsFlags::Nothing);
        if (flag == DemoAppStatsFlags::Nothing)
        {
          FSLLOG3_WARNING("Unknown stats flag: {}", entry);
          rFlags = {};
          return OptionParseResult::Failed;
        }
        flags = flags | static_cast<uint32_t>(flag);
      }
      rFlags = DemoAppStatsFlags(flags);
      return OptionParseResult::Parsed;
    }
  }

  DemoHostManagerOptionParser::DemoHostManagerOptionParser(const ColorSpaceType colorSpaceType, const bool hdrEnabled)
    : m_colorSpaceType(colorSpaceType)
    , m_hdrEnabled(hdrEnabled)
    , m_screenshotConfig(TestScreenshotNameScheme::FrameNumber, ImageFormat::Png, 0, "Screenshot", BasicToneMapper::Clamp)
  {
  }


  void DemoHostManagerOptionParser::ArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back(ArgName::ExitAfterFrame, OptionArgument::OptionRequired, CommandId::ExitAfterFrame,
                          "Exit after the given number of frames has been rendered");
    rOptions.emplace_back(
      ArgName::ExitAfterDuration, OptionArgument::OptionRequired, CommandId::ExitAfterDuration,
      "Exit after the given duration has passed. The value can be specified in seconds or milliseconds. For example 10s or 10ms.");
    rOptions.emplace_back(ArgName::LogStats, OptionArgument::OptionNone, CommandId::LogStats,
                          "Log basic rendering stats (this is equal to setting LogStatsMode to latest)");
    rOptions.emplace_back(ArgName::LogStatsMode, OptionArgument::OptionRequired, CommandId::LogStatsMode,
                          "Set the log stats mode, more advanced version of LogStats. Can be disabled, latest, average");
    rOptions.emplace_back(ArgName::Stats, OptionArgument::OptionNone, CommandId::Stats, "Display basic frame profiling stats");
    rOptions.emplace_back(ArgName::StatsFlags, OptionArgument::OptionRequired, CommandId::StatsFlags,
                          "Select the stats to be displayed/logged. Defaults to frame|cpu. Can be 'frame', 'cpu' or any combination");
    rOptions.emplace_back(ArgName::AppFirewall, OptionArgument::OptionNone, CommandId::AppFirewall,
                          "Enable the app firewall, reporting crashes on-screen instead of exiting");
    rOptions.emplace_back(ArgName::EnableBasic2DPrealloc, OptionArgument::OptionRequired, CommandId::EnableBasic2DPrealloc,
                          "Enable/disable basic2d preallocation (Stats is enabled this is forced true)", OptionGroup::Hidden);
    rOptions.emplace_back(ArgName::ScreenshotFrequency, OptionArgument::OptionRequired, CommandId::ScreenshotFrequency,
                          "Create a screenshot at the given frame frequency");
    rOptions.emplace_back(
      ArgName::ScreenshotFormat, OptionArgument::OptionRequired, CommandId::ScreenshotFormat,
      fmt::format("Chose the format for the screenshot: {}", OptionArgUtil::BuildArgumentString(GetScreenshotFormat(m_hdrEnabled), true)));
    rOptions.emplace_back(ArgName::ScreenshotNamePrefix, OptionArgument::OptionRequired, CommandId::ScreenshotNamePrefix,
                          "Chose the screenshot name prefix (defaults to 'Screenshot')");
    rOptions.emplace_back(ArgName::ScreenshotNameScheme, OptionArgument::OptionRequired, CommandId::ScreenshotNameScheme,
                          fmt::format("Chose the screenshot name scheme: {}.",
                                      OptionArgUtil::BuildArgumentString(SpanUtil::AsReadOnlySpan(ScreenshotNameSchemeArgs), true)));
    if (m_hdrEnabled)
    {
      rOptions.emplace_back(ArgName::ScreenshotToneMapper, OptionArgument::OptionRequired, CommandId::ScreenshotToneMapper,
                            fmt::format("Chose the tone mapper to apply when converting a HDR screenshot to SDR: {}.",
                                        OptionArgUtil::BuildArgumentString(SpanUtil::AsReadOnlySpan(ScreenshotToneMapperArgs), true)));
    }
    rOptions.emplace_back(ArgName::ContentMonitor, OptionArgument::OptionNone, CommandId::ContentMonitor,
                          "Monitor the Content directory for changes and restart the app on changes.\nWARNING: Might not work on all platforms "
                          "and it might impact app performance (experimental)");
    rOptions.emplace_back(
      ArgName::ForceUpdateTime, OptionArgument::OptionRequired, CommandId::ForceUpdateTime,
      "Force the update time to be the given value in microseconds (can be useful when taking a lot of screen-shots). If 0 this option is disabled");
    rOptions.emplace_back(ArgName::Version, OptionArgument::OptionNone, CommandId::Version, "Print version information");
  }


  OptionParseResult DemoHostManagerOptionParser::Parse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    // Rectangle rectValue;
    bool boolValue = false;
    switch (cmdId)
    {
    case CommandId::ExitAfterFrame:
      StringParseUtil::Parse(m_exitAfterFrame, strOptArg);
      return OptionParseResult::Parsed;
    case CommandId::ExitAfterDuration:
      return ParseDurationExitConfig(strOptArg);
    case CommandId::ScreenshotFrequency:
      StringParseUtil::Parse(m_screenshotConfig.Frequency, strOptArg);
      return OptionParseResult::Parsed;
    case CommandId::ScreenshotFormat:
      return OptionArgUtil::TryParseOptionArg(ArgName::ScreenshotFormat, GetScreenshotFormat(m_hdrEnabled), strOptArg, m_screenshotConfig.Format);
    case CommandId::ScreenshotNameScheme:
      return OptionArgUtil::TryParseOptionArg(ArgName::ScreenshotNameScheme, SpanUtil::AsReadOnlySpan(ScreenshotNameSchemeArgs), strOptArg,
                                              m_screenshotConfig.NamingScheme);
    case CommandId::ScreenshotNamePrefix:
      return ParseScreenshotNamePrefix(strOptArg);
    case CommandId::ScreenshotToneMapper:
      return OptionArgUtil::TryParseOptionArg(ArgName::ScreenshotToneMapper, SpanUtil::AsReadOnlySpan(ScreenshotToneMapperArgs), strOptArg,
                                              m_screenshotConfig.ToneMapper);
    case CommandId::ForceUpdateTime:
      {
        uint32_t value = 0;
        StringParseUtil::Parse(value, strOptArg);
        m_forceUpdateTime = TimeSpanUtil::FromMicroseconds(value);
        return OptionParseResult::Parsed;
      }
    case CommandId::LogStats:
      m_logStatsMode = LogStatsMode::Latest;
      return OptionParseResult::Parsed;
    case CommandId::LogStatsMode:
      {
        if (strOptArg == "disabled")
        {
          m_logStatsMode = LogStatsMode::Disabled;
        }
        else if (strOptArg == "latest")
        {
          m_logStatsMode = LogStatsMode::Latest;
        }
        else if (strOptArg == "average")
        {
          m_logStatsMode = LogStatsMode::Average;
        }
        else
        {
          throw std::invalid_argument(fmt::format("Unknown logStatsMode parameter {}", strOptArg));
        }
        return OptionParseResult::Parsed;
      }
    case CommandId::Stats:
      m_stats = true;
      return OptionParseResult::Parsed;
    case CommandId::StatsFlags:
      return TryParseStatsFlags(m_statFlags, strOptArg);
    case CommandId::AppFirewall:
      m_appFirewall = true;
      return OptionParseResult::Parsed;
    case CommandId::ContentMonitor:
      m_contentMonitor = true;
      m_appFirewall = true;
      return OptionParseResult::Parsed;
    case CommandId::EnableBasic2DPrealloc:
      StringParseUtil::Parse(boolValue, strOptArg);
      m_enableBasic2DPrealloc = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Version:
      FSLLOG3_INFO("Release {}, GitCommit '{}'", ReleaseVersion::CurrentVersion(), ReleaseVersion::GetGitCommit());
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


  TimeSpan DemoHostManagerOptionParser::GetForceUpdateTime() const noexcept
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


  OptionParseResult DemoHostManagerOptionParser::ParseDurationExitConfig(const StringViewLite& strOptArg)
  {
    StringViewLite input(strOptArg);

    // First we identify the time duration format
    DurationFormat durationFormat = DurationFormat::Invalid;
    if (input.ends_with("ms"))
    {
      durationFormat = DurationFormat::Milliseconds;
      input.substr(0, input.size() - 2);
    }
    else if (input.ends_with("s"))
    {
      durationFormat = DurationFormat::Seconds;
      input.substr(0, input.size() - 1);
    }

    if (durationFormat == DurationFormat::Invalid || input.empty())
    {
      FSLLOG3_ERROR("Unsupported duration string '{}' expected a duration value like this 10s or 10ms", input);
      return OptionParseResult::Failed;
    }

    int32_t durationValue = 0;
    StringParseUtil::Parse(durationValue, input);

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

  OptionParseResult DemoHostManagerOptionParser::ParseScreenshotNamePrefix(const StringViewLite& strOptArg)
  {
    try
    {
      IO::Path path(strOptArg);
      if (IO::Path::GetFileName(path) != path)
      {
        FSLLOG3_ERROR("The prefix can only contain a filename prefix, not a path '{}'", path);
        return OptionParseResult::Failed;
      }
      m_screenshotConfig.FilenamePrefix = path.ToUTF8String();
      return OptionParseResult::Parsed;
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Failed to parse screenshot name with error: {}", ex.what());
      return OptionParseResult::Failed;
    }
  }
}
