#ifndef SHARED_UI_CHARTS_DATAGENERATOR_DATAGENERATORDATARECORD_HPP
#define SHARED_UI_CHARTS_DATAGENERATOR_DATAGENERATORDATARECORD_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

namespace Fsl
{
  struct DataGeneratorDataRecord
  {
    uint16_t UIProcessEvents{0};
    uint16_t UIUpdate{0};
    uint16_t UIDraw{0};
    uint16_t UIDrawPreprocess{0};
    uint16_t UIDrawGenMesh{0};
    uint16_t UIDrawFillBuffers{0};
    uint16_t UIDrawSchedule{0};

    constexpr DataGeneratorDataRecord() noexcept = default;
    constexpr DataGeneratorDataRecord(const uint16_t uiProcessEvents, const uint16_t uiUpdate, const uint16_t uiDraw, const uint16_t uiDrawPreprocess,
                                      const uint16_t uiDrawGenMesh, const uint16_t uiDrawFillBuffers, const uint16_t uiDrawSchedule) noexcept
      : UIProcessEvents(uiProcessEvents)
      , UIUpdate(uiUpdate)
      , UIDraw(uiDraw)
      , UIDrawPreprocess(uiDrawPreprocess)
      , UIDrawGenMesh(uiDrawGenMesh)
      , UIDrawFillBuffers(uiDrawFillBuffers)
      , UIDrawSchedule(uiDrawSchedule)
    {
    }

    uint32_t Sum() const
    {
      return UIProcessEvents + UIUpdate + UIDraw + UIDrawPreprocess + UIDrawGenMesh + UIDrawFillBuffers + UIDrawSchedule;
    }
  };
}


#endif
