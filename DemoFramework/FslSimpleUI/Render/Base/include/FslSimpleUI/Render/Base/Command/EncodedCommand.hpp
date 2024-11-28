#ifndef FSLSIMPLEUI_RENDER_BASE_COMMAND_ENCODEDCOMMAND_HPP
#define FSLSIMPLEUI_RENDER_BASE_COMMAND_ENCODEDCOMMAND_HPP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Render/Base/Command/DrawCommandType.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommandState.hpp>
#include <FslSimpleUI/Render/Base/DrawClipContext.hpp>
#include <FslSimpleUI/Render/Base/MeshHandle.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>

namespace Fsl::UI
{
  struct EncodedCommand
  {
    EncodedCommandState State;

    MeshHandle Mesh;
    PxVector2 DstPositionPxf;
    PxSize2D DstSizePx;
    UIRenderColor DstColor;
    PxAreaRectangleF ClipRectanglePxf;
    uint32_t Custom0{0};

    constexpr EncodedCommand() noexcept = default;

    constexpr EncodedCommand(const DrawCommandType type, const MeshHandle mesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                             const UIRenderColor dstColor, const DrawClipContext& clipContext)
      : State(type, clipContext.Enabled)
      , Mesh(mesh)
      , DstPositionPxf(dstPositionPxf)
      , DstSizePx(dstSizePx)
      , DstColor(dstColor)
      , ClipRectanglePxf(clipContext.ClipRectanglePxf)
    {
    }

    constexpr EncodedCommand(const DrawCommandType type, const MeshHandle mesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                             const UIRenderColor dstColor, const DrawClipContext& clipContext, const uint32_t custom0)
      : State(type, clipContext.Enabled)
      , Mesh(mesh)
      , DstPositionPxf(dstPositionPxf)
      , DstSizePx(dstSizePx)
      , DstColor(dstColor)
      , ClipRectanglePxf(clipContext.ClipRectanglePxf)
      , Custom0(custom0)
    {
    }

    constexpr bool operator==(const EncodedCommand rhs) const noexcept
    {
      return State == rhs.State && Mesh == rhs.Mesh && DstPositionPxf == rhs.DstPositionPxf && DstSizePx == rhs.DstSizePx &&
             DstColor == rhs.DstColor && ClipRectanglePxf == rhs.ClipRectanglePxf && Custom0 == rhs.Custom0;
    }

    constexpr bool operator!=(const EncodedCommand rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };

}

#endif
