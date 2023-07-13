/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslSimpleUI/App/UISpriteToTextureUtil.hpp>

namespace Fsl::UISpriteToTextureUtil
{
  AtlasTexture2D ExtractFillTexture(const IBasicRenderSystem& renderSystem, const std::shared_ptr<BasicImageSprite>& fillSprite)
  {
    if (!fillSprite)
    {
      throw std::invalid_argument("fillSprite must be valid");
    }

    const BasicImageSpriteInfo& info = fillSprite->GetInfo();

    const PxRectangleU32 fillRect(info.ImageInfo.RectanglePx.GetCenter().X, info.ImageInfo.RectanglePx.GetCenter().Y, PxValueU(1), PxValueU(1));

    const auto* pMaterial = dynamic_cast<const BasicSpriteMaterial*>(info.MaterialInfo.Material.get());
    if (pMaterial == nullptr)
    {
      throw NotSupportedException("Material not of the expected type");
    }

    return {Texture2D(renderSystem.GetMaterialTexture(pMaterial->Material), info.MaterialInfo.ExtentPx, PixelFormat::R8G8B8A8_UNORM),
            AtlasTextureInfo(fillRect, PxThicknessU(), info.ImageDpi)};
  }

  AtlasTexture2D ExtractFillTexture(const IBasicRenderSystem& renderSystem, const std::shared_ptr<ImageSprite>& fillSprite)
  {
    if (!fillSprite)
    {
      throw std::invalid_argument("fillSprite must be valid");
    }

    const ImageSpriteInfo& info = fillSprite->GetInfo();

    const PxRectangleU32 fillRect(info.ImageInfo.TrimmedRectanglePx.GetCenter().X, info.ImageInfo.TrimmedRectanglePx.GetCenter().Y, PxValueU(1),
                                  PxValueU(1));

    const auto* pMaterial = dynamic_cast<const BasicSpriteMaterial*>(info.MaterialInfo.Material.get());
    if (pMaterial == nullptr)
    {
      throw NotSupportedException("Material not of the expected type");
    }

    return {Texture2D(renderSystem.GetMaterialTexture(pMaterial->Material), info.MaterialInfo.ExtentPx, PixelFormat::R8G8B8A8_UNORM),
            AtlasTextureInfo(fillRect, PxThicknessU(), info.ImageDpi)};
  }
}
