#ifndef FSLDEMOPLATFORM_DEMOHOSTMANAGEROPTIONPARSER_HPP
#define FSLDEMOPLATFORM_DEMOHOSTMANAGEROPTIONPARSER_HPP
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

#include <FslBase/ITag.hpp>
#include <FslBase/Noncopyable.hpp>
#include <FslBase/Getopt/IOptionParser.hpp>
#include <FslDemoPlatform/DurationExitConfig.hpp>
#include <FslGraphics/ImageFormat.hpp>
#include <FslNativeWindow/Base/NativeWindowConfig.hpp>
#include <FslDemoHost/Base/Service/Test/TestScreenshotConfig.hpp>
#include <FslDemoHost/Base/LogStatsMode.hpp>
#include <chrono>
#include <memory>

namespace Fsl
{

  class DemoHostManagerOptionParser
    : public IOptionParser
    , private Noncopyable
  {
    int32_t m_exitAfterFrame;
    DurationExitConfig m_exitAfterDuration;
    TestScreenshotConfig m_screenshotConfig;
    uint32_t m_forceUpdateTime;
    LogStatsMode m_logStatsMode;
    bool m_stats;
    bool m_appFirewall;
    bool m_enableBasic2DPrealloc;
    bool m_contentMonitor;
  public:
    DemoHostManagerOptionParser();

    virtual std::string GetName() const override
    {
      return std::string("DemoHostManagerOptionParser");
    }

    virtual void ArgumentSetup(std::deque<Option>& rOptions) override;
    virtual OptionParseResult::Enum Parse(const int32_t cmdId, const char*const pszOptArg) override;
    virtual bool ParsingComplete() override;

    //! Returns a negative value if we should render a unlimited amount of frames.
    int32_t GetExitAfterFrame() const;

    //! Return the current duration exit config
    DurationExitConfig GetDurationExitConfig() const
    {
      return m_exitAfterDuration;
    }

    //! Returns zero if forced timing is disabled
    uint32_t GetForceUpdateTime() const;

    //! Get the screenshot config
    TestScreenshotConfig GetScreenshotConfig() const;

    //! @brief Get the current log stats mode
    LogStatsMode GetLogStatsMode() const;

    //! Check if stat is enabled
    bool IsStatsEnabled() const;
    //! Check if the app firewall should be enabled by default
    bool IsAppFirewallEnabled() const;
    //! Check if basic2d preallocate is enabled
    bool IsBasic2DPreallocEnabled() const;
    //! Check if content monitoring is enabled
    bool IsContentMonitorEnabled() const;

    void RequestEnableAppFirewall();
  private:
    OptionParseResult::Enum ParseDurationExitConfig(const char*const pszOptArg);
    OptionParseResult::Enum ParseScreenshotImageFormat(const char*const pszOptArg);
    OptionParseResult::Enum ParseScreenshotNamePrefix(const char*const pszOptArg);
    OptionParseResult::Enum ParseScreenshotNameScheme(const char*const pszOptArg);
  };
}

#endif
