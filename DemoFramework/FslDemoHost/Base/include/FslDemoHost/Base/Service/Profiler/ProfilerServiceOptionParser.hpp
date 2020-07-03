#ifndef FSLDEMOHOST_BASE_SERVICE_PROFILER_PROFILERSERVICEOPTIONPARSER_HPP
#define FSLDEMOHOST_BASE_SERVICE_PROFILER_PROFILERSERVICEOPTIONPARSER_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslService/Impl/AServiceOptionParser.hpp>

namespace Fsl
{
  class ProfilerServiceOptionParser final : public AServiceOptionParser
  {
    uint32_t m_averageEntries;

  public:
    ProfilerServiceOptionParser();

    std::string GetName() const final
    {
      return std::string("ProfilerServiceOptionParser");
    }

    void OnArgumentSetup(std::deque<Option>& rOptions) final;
    OptionParseResult OnParse(const int32_t cmdId, const StringViewLite& strOptArg) final;
    bool OnParsingComplete() final;

    //! @brief Get the number of frames to average the FPS over for Average mode.
    uint32_t GetAverageEntries() const
    {
      return m_averageEntries;
    }
  };
}

#endif
