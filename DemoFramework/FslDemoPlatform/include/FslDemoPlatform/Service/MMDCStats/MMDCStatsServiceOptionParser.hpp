#ifndef FSLDEMOPLATFORM_SERVICE_MMDCSTATS_MMDCSTATSSERVICEOPTIONPARSER_HPP
#define FSLDEMOPLATFORM_SERVICE_MMDCSTATS_MMDCSTATSSERVICEOPTIONPARSER_HPP
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

#include <FslService/Impl/AServiceOptionParser.hpp>
#include <string>

namespace Fsl
{
  class MmdcData;

  class MMDCStatsServiceOptionParser : public AServiceOptionParser
  {
    bool m_enabled;
    bool m_logEnabled;
    uint32_t m_duration;
    std::string m_usage;

  public:
    MMDCStatsServiceOptionParser();
    void OnArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult::Enum OnParse(const int32_t cmdId, const char* const pszOptArg) override;
    bool OnParsingComplete() override;

    bool IsEnabled() const
    {
      return m_enabled;
    }
    bool IsLogEnabled() const
    {
      return m_logEnabled;
    }
    uint32_t GetMillisecondDuration() const
    {
      return m_duration;
    }
    std::string GetUsageString() const
    {
      return m_usage;
    }
  };
}

#endif
