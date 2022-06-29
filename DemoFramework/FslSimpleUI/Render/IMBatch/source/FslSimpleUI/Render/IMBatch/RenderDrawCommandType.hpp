#ifndef FSLSIMPLEUI_RENDER_IMBATCH_RENDERDRAWCOMMANDTYPE_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_RENDERDRAWCOMMANDTYPE_HPP
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
#include <FslSimpleUI/Render/Base/Command/DrawCommandType.hpp>
#include <cassert>
#include "RenderDrawSpriteType.hpp"

namespace Fsl::UI::RenderIMBatch
{
  inline constexpr uint32_t CreateValue(const RenderDrawSpriteType spriteType, const DrawCommandType commandType) noexcept
  {
    assert((static_cast<uint32_t>(commandType) & 0xFF000000) == 0);
    return (static_cast<uint32_t>(spriteType) << 24u) | static_cast<uint32_t>(commandType);
  }

  enum class RenderDrawCommandType : uint32_t
  {
    // BasicImageSprite
    // BasicImageSprite_Nop = CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::Nop),
    BasicImageSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawAtOffsetAndSize),
    BasicImageSprite_DrawCustomBasicImageAtOffsetAndSize =
      CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    BasicImageSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
      CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // BasicImageSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // BasicImageSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // BasicImageSprite_DrawRot90CWAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // BasicNineSliceSprite
    // BasicNineSliceSprite_Nop = CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::Nop),
    BasicNineSliceSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawAtOffsetAndSize),
    // BasicNineSliceSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // BasicNineSliceSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // BasicNineSliceSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // BasicNineSliceSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // BasicNineSliceSprite_DrawRot90CWAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // ImageSprite
    // ImageSprite_Nop = CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::Nop),
    ImageSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawAtOffsetAndSize),
    // ImageSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // ImageSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // ImageSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // ImageSprite_DrawCustomTextAtOffsetAndSize = CreateValue(RenderDrawSpriteType::ImageSprite,
    // DrawCommandType::DrawCustomTextAtOffsetAndSize), ImageSprite_DrawRot90CWAtOffsetAndSize = CreateValue(RenderDrawSpriteType::ImageSprite,
    // DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // NineSliceSprite
    // NineSliceSprite_Nop = CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::Nop),
    NineSliceSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawAtOffsetAndSize),
    // NineSliceSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // NineSliceSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    NineSliceSprite_DrawCustomNineSliceAtOffsetAndSize =
      CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // NineSliceSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    NineSliceSprite_DrawRot90CWAtOffsetAndSize = CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // OptimizedNineSliceSprite
    // OptimizedNineSliceSprite_Nop = CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::Nop),
    OptimizedNineSliceSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawAtOffsetAndSize),
    // OptimizedNineSliceSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // OptimizedNineSliceSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // OptimizedNineSliceSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // OptimizedNineSliceSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    OptimizedNineSliceSprite_DrawRot90CWAtOffsetAndSize =
      CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // SpriteFont
    // SpriteFont_Nop = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::Nop),
    SpriteFont_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawAtOffsetAndSize),
    // SpriteFont_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // SpriteFont_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // SpriteFont_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    SpriteFont_DrawCustomTextAtOffsetAndSize = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // SpriteFont_DrawRot90CWAtOffsetAndSize = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawRot90CWAtOffsetAndSize),
  };

  inline constexpr RenderDrawCommandType ToRenderDrawCommandType(const RenderDrawSpriteType spriteType, const DrawCommandType commandType) noexcept
  {
    return static_cast<RenderDrawCommandType>(CreateValue(spriteType, commandType));
  }
}
#endif
