#ifndef FSLSIMPLEUI_RENDER_IMBATCH_HANDLECODING_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_HANDLECODING_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslSimpleUI/Render/Base/MeshHandle.hpp>
#include <cassert>
#include "RenderDrawSpriteType.hpp"

namespace Fsl::UI::RenderIMBatch::HandleCoding
{
  constexpr int32_t ShiftType = 24;
  constexpr int32_t MaxHandleValue = 0x00FFFFFF;
  constexpr int32_t MaskType = static_cast<int32_t>(0xFF000000u);
  constexpr int32_t MaskHandle = 0x00FFFFFF;


  inline constexpr MeshHandle EncodeHandle(const RenderDrawSpriteType type, const int32_t handleValue) noexcept
  {
    assert(handleValue <= MaxHandleValue);
    return MeshHandle((static_cast<int32_t>(type) << ShiftType) | handleValue);
  }

  inline constexpr RenderDrawSpriteType GetType(const MeshHandle handle) noexcept
  {
    return static_cast<RenderDrawSpriteType>((handle.Value & MaskType) >> ShiftType);
  }

  inline constexpr int32_t GetOriginalHandle(const MeshHandle handle) noexcept
  {
    return handle.Value & MaskHandle;
  }
}

#endif
