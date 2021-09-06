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

#include "JsonInputCommandRecord.hpp"
#include <nlohmann_json/FslBase/Math/Pixel/PxPoint2.hpp>
#include <nlohmann_json/FslBase/Math/Pixel/PxRectangle.hpp>
#include "JsonCustomWindowId.hpp"
#include "JsonInputCommandId.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalNames
    {
      constexpr auto FrameIndex = "FrameIndex";
      constexpr auto CommandId = "CommandId";
      constexpr auto WindowId = "WindowId";
      constexpr auto WindowRectPx = "WindowRectPx";
      constexpr auto MousePosition = "MousePosition";
    }
  }

  void to_json(nlohmann::json& j, const InputCommandRecord& src)
  {
    j = nlohmann::json{{LocalNames::FrameIndex, src.FrameIndex},
                       {LocalNames::CommandId, src.CommandId},
                       {LocalNames::WindowId, src.WindowId},
                       {LocalNames::WindowRectPx, src.WindowRectPx},
                       {LocalNames::MousePosition, src.MousePositionPx}};
  }

  void from_json(const nlohmann::json& j, InputCommandRecord& rDst)
  {
    j.at(LocalNames::FrameIndex).get_to(rDst.FrameIndex);
    j.at(LocalNames::CommandId).get_to(rDst.CommandId);
    j.at(LocalNames::WindowId).get_to(rDst.WindowId);
    j.at(LocalNames::WindowRectPx).get_to(rDst.WindowRectPx);
    j.at(LocalNames::MousePosition).get_to(rDst.MousePositionPx);
  }
}

#endif
