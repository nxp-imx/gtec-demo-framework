#ifndef SHARED_SYSTEM_DFNATIVEBATCH2D_TEXTUREATLASSCENE_HPP
#define SHARED_SYSTEM_DFNATIVEBATCH2D_TEXTUREATLASSCENE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>

namespace Fsl
{
  class IGraphicsService;
  struct Point2;
  class SpriteNativeAreaCalc;

  class TextureAtlasScene
  {
    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<INativeBatch2D> m_batch;
    int32_t m_blendStateIndex;
    BlendState m_blendState;
    Texture2D m_texSdf;
    TextureAtlasSpriteFont m_font;
    TextureAtlasSpriteFont m_fontSmall;
    TextureAtlasSpriteFont m_fontSdf;
    AtlasTexture2D m_atlasTexture1;
    AtlasTexture2D m_atlasTexture2;
    AtlasTexture2D m_atlasTexture3;
    AtlasTexture2D m_atlasTexture4;
    AtlasTexture2D m_atlasTextureFill;
    PxRectangle m_fillCenter;
    float m_angle;
    float m_zoomAngle1;
    float m_zoomAngle2;
    float m_zoomFactorAngle{};
    int m_sceneId;

  public:
    explicit TextureAtlasScene(const ServiceProvider& serviceProvider, const SpriteNativeAreaCalc& spriteNativeAreaCalc, const uint32_t densityDpi);
    int32_t GetGridOffsetY() const;
    void OnKeyEvent(const KeyEvent& event);
    void Update(const DemoTime& demoTime);
    void Draw(const PxSize2D& windowSizePx);

  private:
    void DrawStrings(const BaseTexture2D& atlasTexture, const TextureAtlasSpriteFont& font, const char* const psz, const int32_t areaWidth);
    void DrawAtlasTextureSimpleRotationUsingDstPos(const Point2& dstOffset);
    void DrawAtlasTextureSimpleUsingDstPos(const Point2& dstOffset);
    void DrawAtlasTextureSimpleUsingDstRectangle(const Point2& dstOffset);
    void DrawGrid(const int32_t cellWidth, const int32_t cellHeight, const int32_t areaWidth, const int32_t areaHeight, const Color& colorBorder,
                  const Color& colorCenter, const Point2& offset);
    void DrawTargetingRect(const PxRectangle& dstRect, const Color& colorBorder, const Color& colorCenter);
    void DrawRect(const PxRectangle& dstRect, const Color& color);
  };
}

#endif
