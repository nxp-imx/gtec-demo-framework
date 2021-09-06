#ifdef FSLCUSTOM_USE_JSON_FOR_COMMANDLIST
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include "JsonAppInputCommandList.hpp"
#include <nlohmann_json/FslBase/Math/Pixel/PxSize2D.hpp>
#include "JsonInputCommandRecord.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalNames
    {
      constexpr auto RecordResolution = "RecordResolution";
      constexpr auto RecordDensityDpi = "RecordDensityDpi";
      constexpr auto List = "List";
      constexpr auto FrameCount = "FrameCount";
    }
  }

  void to_json(nlohmann::json& j, const AppInputCommandList& src)
  {
    j = nlohmann::json{
      {LocalNames::RecordResolution, src.GetRecordResolution()},
      {LocalNames::RecordDensityDpi, src.GetRecordDensityDpi()},
      {LocalNames::FrameCount, src.GetFrameCount()},
      {LocalNames::List, src.AsVector()},
    };
  }

  void from_json(const nlohmann::json& j, AppInputCommandList& rDst)
  {
    PxSize2D recordResolution;
    uint32_t recordDensityDpi;
    uint32_t frameCount{};
    j.at(LocalNames::RecordResolution).get_to(recordResolution);
    j.at(LocalNames::RecordDensityDpi).get_to(recordDensityDpi);
    j.at(LocalNames::FrameCount).get_to(frameCount);
    j.at(LocalNames::List).get_to(rDst.AsVector());
    rDst.SetRecordInfo(recordResolution, recordDensityDpi);
    rDst.SetFrameCount(frameCount);
  }
}
#endif
