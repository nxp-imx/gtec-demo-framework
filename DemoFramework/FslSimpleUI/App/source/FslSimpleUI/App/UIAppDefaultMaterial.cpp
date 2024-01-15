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
#include <FslBase/Exceptions.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Basic/BasicMaterial.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslSimpleUI/App/UIAppConfig.hpp>
#include <FslSimpleUI/App/UIAppDefaultMaterial.hpp>

namespace Fsl::UIAppDefaultMaterial
{
  SpriteMaterialInfo CreateDefaultMaterial(const ServiceProvider& serviceProvider, const VertexDeclarationSpan& vertexDeclaration,
                                           const PixelFormat pixelFormat, const bool isDynamic, const bool allowDepthBuffer)
  {
    auto graphicsService = serviceProvider.Get<IGraphicsService>();
    std::shared_ptr<IBasicRenderSystem> renderSystem = graphicsService->GetBasicRenderSystem();
    const bool yFlipped = graphicsService->GetNativeBatch2D()->SYS_IsTextureCoordinateYFlipped();

    Bitmap defaultBitmap(16, 16, pixelFormat, yFlipped ? BitmapOrigin::LowerLeft : BitmapOrigin::UpperLeft);
    auto basicTexture = renderSystem->CreateTexture2D(defaultBitmap, Texture2DFilterHint::Nearest, TextureFlags::NotDefined);

    BasicMaterialDepthInfo depthInfo(allowDepthBuffer, allowDepthBuffer, BasicCompareOp::Less);

    const BasicMaterialCreateInfo createInfo(BlendState::Opaque, BasicCullMode::Disabled, BasicFrontFace::CounterClockwise, depthInfo,
                                             vertexDeclaration);

    auto defaultMaterial = std::make_shared<BasicSpriteMaterial>(renderSystem->CreateMaterial(createInfo, basicTexture, isDynamic));

    // const BasicMaterialHandle hMat = m_defaultMaterial->Material.GetHandle();
    return {UIAppConfig::MaterialId::Default, defaultBitmap.GetExtent(), true, defaultMaterial};
  }
}
