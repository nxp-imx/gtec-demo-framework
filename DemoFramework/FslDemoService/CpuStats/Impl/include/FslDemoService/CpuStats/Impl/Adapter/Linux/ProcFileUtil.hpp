#ifndef FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_PROCFILEUTIL_HPP
#define FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_PROCFILEUTIL_HPP
#ifdef __linux__
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/Linux/BasicFileReader.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/Linux/BufferedFileParser.hpp>
#include <vector>

namespace Fsl::ProcFileUtil
{
  struct CPUInfo
  {
    uint64_t Id{0};
    uint64_t User{0};
    uint64_t Nice{0};
    uint64_t System{0};
    uint64_t Idle{0};
  };

  struct RAMInfo
  {
    uint64_t VmRSS{0};
  };

  //! @brief intended for parsing information about CPU load from "/proc/stat"
  extern bool TryParseCPUStats(std::vector<CPUInfo>& rParsed, BasicFileReader& file, BufferedFileParser<4096>& fileParser);
  //! @brief intended for parsing information about CPU load from "/proc/self/status"
  extern bool TryParseRAMStats(RAMInfo& rParsed, BasicFileReader& file, BufferedFileParser<4096>& fileParser);
}

#endif
#endif
