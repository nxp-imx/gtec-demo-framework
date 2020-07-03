#ifndef FSLGRAPHICS_RENDER_ADAPTER_INATIVEBATCH2D_HPP
#define FSLGRAPHICS_RENDER_ADAPTER_INATIVEBATCH2D_HPP
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
#include <FslGraphics/Render/BatchEffect.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Render/Stats/Batch2DStats.hpp>
#include <string>

namespace Fsl
{
  class AtlasTexture2D;
  class BaseTexture2D;
  struct BitmapFontConfig;
  struct Color;
  class StringViewLite;
  class INativeTexture2D;
  struct NativeTextureArea;
  struct NativeQuadTextureCoords;
  struct Point2;
  struct PxAreaRectangleF;
  struct PxClipRectangle;
  struct PxExtent2D;
  struct PxRectangle;
  struct PxRectangleU;
  class SpriteFont;
  class TextureAtlasBitmapFont;
  struct Vector2;
  struct Vector4;

  //! @brief A really simple API independent way to draw some graphics
  //!        All methods operate in screen coordinate pixel mode where 0,0 is the top left corner and
  //!        the bottom right corner is equal to the display width-1,height-1
  //! @note  This API provides a form of batched immediate mode, so it will be slower than properly
  //         optimized graphics, but its faster to get something running and good for debugging.
  class INativeBatch2D
  {
  public:
    virtual ~INativeBatch2D() = default;

    //! @brief Set the screen resolution used by the batch2d instance
    virtual void SetScreenExtent(const PxExtent2D& extentPx) = 0;

    //! @brief Begin drawing. Defaults to blendState == BlendState::AlphaBlend, restoreState == false
    virtual void Begin() = 0;
    //! @brief Begin drawing
    //! @param blendState the BlendState to use
    virtual void Begin(const BlendState blendState) = 0;
    //! @brief Begin drawing
    //! @param blendState the BlendState to use
    //! @param restoreState if true all native state that is modified by Batch2D will be restored to their initial setting
    virtual void Begin(const BlendState blendState, const bool restoreState) = 0;

    //! @brief If in a begin/end block this switches the blend state to the requested state
    virtual void ChangeTo(const BlendState blendState) = 0;

    virtual void End() = 0;


    //! @brief Get basic drawing stats
    virtual Batch2DStats GetStats() const = 0;

    // ---------- 0

    virtual void Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Color& color) = 0;
    //! @note Low level draw access (almost a passthrough function)
    virtual void Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Vector4& color) = 0;
    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    virtual void Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Color& color) = 0;
    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    virtual void Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Vector4& color) = 0;

    // ---------- 0 with clip

    virtual void Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Color& color, const PxClipRectangle& clipRectPx) = 0;
    virtual void Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Vector4& color, const PxClipRectangle& clipRectPx) = 0;

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    virtual void Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Color& color, const PxClipRectangle& clipRectPx) = 0;
    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    virtual void Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                      const Vector4& color, const PxClipRectangle& clipRectPx) = 0;

    // ---------- 1

    //! @brief Draw the full texture at dstPosition
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color) = 0;
    //! @brief Draw the full texture at dstPosition
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color) = 0;
    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const Color& color) = 0;
    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const Color& color) = 0;
    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color) = 0;
    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color) = 0;
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color, const BatchEffect effect) = 0;
    virtual void Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color, const BatchEffect effect) = 0;

    // ---------- 2

    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color) = 0;
    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color) = 0;
    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color) = 0;
    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color) = 0;


    // ---------- 2 with clip

    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const PxClipRectangle& clipRectPx) = 0;
    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const PxClipRectangle& clipRectPx) = 0;
    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const PxClipRectangle& clipRectPx) = 0;
    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const PxClipRectangle& clipRectPx) = 0;

    // ---------- 2A
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const BatchEffect effect) = 0;
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const BatchEffect effect) = 0;
    // ---------- 3

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangleU& srcRectanglePx,
                      const Color& color) = 0;
    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangleU& srcRectanglePx, const Color& color) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangle& srcRectanglePx, const Color& color) = 0;
    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangle& srcRectanglePx, const Color& color) = 0;

    // ---------- 4

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx,
                      const Color& color) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx,
                      const Color& color) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx,
                      const Color& color) = 0;
    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx,
                      const Color& color) = 0;

    // ---------- 4 With clip

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx,
                      const Color& color, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx,
                      const Color& color, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx,
                      const Color& color, const PxClipRectangle& clipRectPx) = 0;
    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const PxClipRectangle& clipRectPx) = 0;

    // ---------- 4A

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx,
                      const Color& color, const BatchEffect effect) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx,
                      const Color& color, const BatchEffect effect) = 0;

    // ---------- 5

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                      const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                      const Vector2& scale) = 0;

    // ---------- 6

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const float rotation,
                      const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const float rotation, const Vector2& origin,
                      const Vector2& scale) = 0;

    // ---------- 7

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale) = 0;

    // ---------- 7 with clip

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    // ---------- 8

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const float rotation, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
                      const float rotation, const Vector2& origin, const Vector2& scale) = 0;
    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const float rotation, const Vector2& origin, const Vector2& scale) = 0;
    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
                      const float rotation, const Vector2& origin, const Vector2& scale) = 0;

    // ---------- 9

    //! @brief Draw the full texture at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositionsPxf, const int32_t dstPositionsLength,
                      const Color& color) = 0;
    //! @brief Draw the full texture at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2* const pDstPositionsPxf, const int32_t dstPositionsLength,
                      const Color& color) = 0;

    // ---------- 10

    //! @brief Draw the texture area at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositionsPxf, const int32_t dstPositionsLength,
                      const PxRectangleU& srcRectanglePx, const Color& color) = 0;
    //! @brief Draw the texture area at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2* const pDstPositionsPxf, const int32_t dstPositionsLength,
                      const PxRectangleU& srcRectanglePx, const Color& color) = 0;
    //! @brief Draw the texture area at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositionsPxf, const int32_t dstPositionsLength,
                      const PxRectangle& srcRectanglePx, const Color& color) = 0;
    //! @brief Draw the texture area at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const BaseTexture2D& srcTexture, const Vector2* const pDstPositionsPxf, const int32_t dstPositionsLength,
                      const PxRectangle& srcRectanglePx, const Color& color) = 0;

    // ---------- 11

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const StringViewLite& strView,
                            const Vector2& dstPositionPxf, const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPositionPxf,
                            const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str,
                            const Vector2& dstPositionPxf, const Color& color) = 0;

    // ---------- 12

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const StringViewLite& strView,
                            const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPositionPxf,
                            const Color& color, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str,
                            const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale) = 0;

    // ---------- 13

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param fontConfig the font configuration.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param fontConfig the font configuration.
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const char* const psz, const Vector2& dstPositionPxf, const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param fontConfig the font configuration.
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const std::string& str, const Vector2& dstPositionPxf, const Color& color) = 0;

    // ---------- 13 With clip

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param fontConfig the font configuration.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    //! @param clipRect the rendering will be clipped against this
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param fontConfig the font configuration.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    //! @param clipRect the rendering will be clipped against this
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const char* const psz, const Vector2& dstPositionPxf, const Color& color, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param fontConfig the font configuration.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    //! @param clipRect the rendering will be clipped against this
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const std::string& str, const Vector2& dstPositionPxf, const Color& color, const PxClipRectangle& clipRectPx) = 0;

    // ---------- 14

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz,
                            const BitmapFontConfig& fontConfig, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str,
                            const BitmapFontConfig& fontConfig, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale) = 0;

    // ---------- 14 with clip

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                            const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz,
                            const BitmapFontConfig& fontConfig, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const BaseTexture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str,
                            const BitmapFontConfig& fontConfig, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    // ---------- 15

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const char* const psz, const Vector2& dstPositionPxf, const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const std::string& str, const Vector2& dstPositionPxf, const Color& color) = 0;

    // ---------- 15 with clip

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color,
                            const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const char* const psz, const Vector2& dstPositionPxf, const Color& color,
                            const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const std::string& str, const Vector2& dstPositionPxf, const Color& color,
                            const PxClipRectangle& clipRectPx) = 0;

    // ---------- 16

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color,
                            const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const char* const psz, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const std::string& str, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale) = 0;

    // ---------- 16 with clip

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color,
                            const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const char* const psz, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const SpriteFont& font, const std::string& str, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                            const Vector2& scale, const PxClipRectangle& clipRectPx) = 0;

    // ----------

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const PxRectangle& dstRect, const Color& color) = 0;

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const BaseTexture2D& srcFillTexture, const PxRectangle& dstRect, const Color& color) = 0;

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color) = 0;

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const BaseTexture2D& srcFillTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color) = 0;

    // ----------

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawLine(const AtlasTexture2D& srcFillTexture, const Vector2& dstFromPxf, const Vector2& dstToPxf, const Color& color) = 0;

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawLine(const BaseTexture2D& srcFillTexture, const Vector2& dstFromPxf, const Vector2& dstToPxf, const Color& color) = 0;
  };
}

#endif
