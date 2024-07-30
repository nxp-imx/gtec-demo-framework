#ifndef FSLSIMPLEUI_RENDER_IMBATCH_RENDERDRAWCOMMANDTYPE_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_RENDERDRAWCOMMANDTYPE_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicImageSprite_Nop = CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::Nop),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BasicImageSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BasicImageSprite_DrawCustomBasicImageAtOffsetAndSize =
      CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BasicImageSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
      CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicImageSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicImageSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicImageSprite_DrawRot90CWAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicImageSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // BasicNineSliceSprite
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicNineSliceSprite_Nop = CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::Nop),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BasicNineSliceSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicNineSliceSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicNineSliceSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicNineSliceSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicNineSliceSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // BasicNineSliceSprite_DrawRot90CWAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::BasicNineSliceSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // ImageSprite
    // NOLINTNEXTLINE(readability-identifier-naming)
    // ImageSprite_Nop = CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::Nop),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ImageSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // ImageSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // ImageSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // ImageSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::ImageSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // ImageSprite_DrawCustomTextAtOffsetAndSize = CreateValue(RenderDrawSpriteType::ImageSprite,
    // DrawCommandType::DrawCustomTextAtOffsetAndSize), ImageSprite_DrawRot90CWAtOffsetAndSize = CreateValue(RenderDrawSpriteType::ImageSprite,
    // DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // NineSliceSprite
    // NOLINTNEXTLINE(readability-identifier-naming)
    // NineSliceSprite_Nop = CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::Nop),
    // NOLINTNEXTLINE(readability-identifier-naming)
    NineSliceSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // NineSliceSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // NineSliceSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // NOLINTNEXTLINE(readability-identifier-naming)
    NineSliceSprite_DrawCustomNineSliceAtOffsetAndSize =
      CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // NineSliceSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    NineSliceSprite_DrawRot90CWAtOffsetAndSize = CreateValue(RenderDrawSpriteType::NineSliceSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // OptimizedNineSliceSprite
    // NOLINTNEXTLINE(readability-identifier-naming)
    // OptimizedNineSliceSprite_Nop = CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::Nop),
    // NOLINTNEXTLINE(readability-identifier-naming)
    OptimizedNineSliceSprite_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // OptimizedNineSliceSprite_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // OptimizedNineSliceSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // OptimizedNineSliceSprite_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // OptimizedNineSliceSprite_DrawCustomTextAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    OptimizedNineSliceSprite_DrawRot90CWAtOffsetAndSize =
      CreateValue(RenderDrawSpriteType::OptimizedNineSliceSprite, DrawCommandType::DrawRot90CWAtOffsetAndSize),

    // SpriteFont
    // NOLINTNEXTLINE(readability-identifier-naming)
    // SpriteFont_Nop = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::Nop),
    // NOLINTNEXTLINE(readability-identifier-naming)
    SpriteFont_DrawAtOffsetAndSize = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // SpriteFont_DrawCustomBasicImageAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomBasicImageAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // SpriteFont_DrawCustomBasicImageAtOffsetAndSizeBasicMesh =
    //  CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomBasicImageAtOffsetAndSizeBasicMesh),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // SpriteFont_DrawCustomNineSliceAtOffsetAndSize =
    //  CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomNineSliceAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    SpriteFont_DrawCustomTextAtOffsetAndSize = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawCustomTextAtOffsetAndSize),
    // NOLINTNEXTLINE(readability-identifier-naming)
    // SpriteFont_DrawRot90CWAtOffsetAndSize = CreateValue(RenderDrawSpriteType::SpriteFont, DrawCommandType::DrawRot90CWAtOffsetAndSize),
  };

  inline constexpr RenderDrawCommandType ToRenderDrawCommandType(const RenderDrawSpriteType spriteType, const DrawCommandType commandType) noexcept
  {
    return static_cast<RenderDrawCommandType>(CreateValue(spriteType, commandType));
  }
}
#endif
