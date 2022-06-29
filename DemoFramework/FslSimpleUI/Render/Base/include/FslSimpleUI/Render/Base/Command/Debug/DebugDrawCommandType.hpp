#ifndef FSLSIMPLEUI_RENDER_BASE_COMMAND_DEBUG_DEBUGDRAWCOMMANDTYPE_HPP
#define FSLSIMPLEUI_RENDER_BASE_COMMAND_DEBUG_DEBUGDRAWCOMMANDTYPE_HPP
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

#include <FslSimpleUI/Render/Base/Command/DrawCommandType.hpp>

namespace Fsl::UI::Debug
{
  inline const char* TryToString(const DrawCommandType& value)
  {
    switch (value)
    {
    case DrawCommandType::Nop:
      return "DrawCommandType::Nop";
    case DrawCommandType::DrawAtOffsetAndSize:
      return "DrawCommandType::DrawAtOffsetAndSize";
    case DrawCommandType::DrawCustomBasicImageAtOffsetAndSize:
      return "DrawCommandType::DrawCustomBasicImageAtOffsetAndSize";
    case DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh:
      return "DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh";
    case DrawCommandType::DrawCustomNineSliceAtOffsetAndSize:
      return "DrawCommandType::DrawCustomNineSliceAtOffsetAndSize";
    case DrawCommandType::DrawCustomTextAtOffsetAndSize:
      return "DrawCommandType::DrawCustomTextAtOffsetAndSize";
    case DrawCommandType::DrawRot90CWAtOffsetAndSize:
      return "DrawCommandType::DrawRot90CWAtOffsetAndSize";
    default:
      return nullptr;
    }
  }

  inline const char* ToString(const DrawCommandType& value)
  {
    const auto* const psz = TryToString(value);
    return psz != nullptr ? psz : "Unknown";
  }
}

#endif
