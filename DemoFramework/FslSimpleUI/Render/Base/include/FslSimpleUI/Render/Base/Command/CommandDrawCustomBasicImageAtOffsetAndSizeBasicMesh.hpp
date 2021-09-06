#ifndef FSLSIMPLEUI_RENDER_BASE_COMMAND_COMMANDDRAWCUSTOMBASICIMAGEATOFFSETANDSIZEBASICMESH_HPP
#define FSLSIMPLEUI_RENDER_BASE_COMMAND_COMMANDDRAWCUSTOMBASICIMAGEATOFFSETANDSIZEBASICMESH_HPP
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

#include <FslSimpleUI/Render/Base/Command/EncodedCommand.hpp>

namespace Fsl
{
  namespace UI
  {
    struct CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh final : private EncodedCommand
    {
      constexpr explicit CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh(const EncodedCommand& command) noexcept
        : EncodedCommand(command)
      {
        assert(command.Type == DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh);
      }

      constexpr MeshHandle GetMesh() const noexcept
      {
        return Mesh;
      }

      constexpr const PxVector2& GetDstPositionPxf() const noexcept
      {
        return DstPositionPxf;
      }

      constexpr const PxSize2D& GetDstSizePx() const noexcept
      {
        return DstSizePx;
      }

      constexpr Color GetDstColor() const noexcept
      {
        return DstColor;
      }

      constexpr uint32_t CustomDrawFunctionIndex() const noexcept
      {
        return Custom0;
      }

      inline constexpr static EncodedCommand Encode(const MeshHandle hMesh, const PxVector2& dstPositionPxf, const PxSize2D dstSizePx,
                                                    const Color& dstColor, const uint32_t customDrawFunctionIndex) noexcept
      {
        return {DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh, hMesh, dstPositionPxf, dstSizePx, dstColor, customDrawFunctionIndex};
      }
    };
  }
}

#endif
