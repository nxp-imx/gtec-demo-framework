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
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <string>

namespace Fsl
{
  class AtlasTexture2D;
  struct Color;
  struct Point2;
  struct Rect;
  struct Rectangle;
  struct Vector2;
  class Texture2D;
  class TextureAtlasBitmapFont;

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
    virtual void SetScreenResolution(const Point2& resolution) = 0;

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

    //! @brief Draw the full texture at dstPosition
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color) = 0;
    //! @brief Draw the full texture at dstPosition
    virtual void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color) = 0;

    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const Rectangle& dstRectangle, const Color& color) = 0;
    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const Texture2D& srcTexture, const Rectangle& dstRectangle, const Color& color) = 0;

    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const Rect& dstRectangle, const Color& color) = 0;
    //! @brief Scale the full texture to fit inside the dstRectangle
    virtual void Draw(const Texture2D& srcTexture, const Rect& dstRectangle, const Color& color) = 0;

    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color) = 0;
    //! @brief Draw the texture area at dstPosition
    virtual void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color) = 0;
    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const Texture2D& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color) = 0;

    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const AtlasTexture2D& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color) = 0;
    //! @brief Scale the texture area so it fits inside the dstRectangle
    virtual void Draw(const Texture2D& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin,
                      const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin,
                      const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin,
                      const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color,
                      const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color,
                      const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color,
                      const float rotation, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    virtual void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color,
                      const float rotation, const Vector2& origin, const Vector2& scale) = 0;


    //! @brief Draw the full texture at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength, const Color& color) = 0;
    //! @brief Draw the full texture at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const Texture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength, const Color& color) = 0;

    //! @brief Draw the texture area at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength,
                      const Rectangle& srcRectangle, const Color& color) = 0;
    //! @brief Draw the texture area at the given dst positions
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void Draw(const Texture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength,
                      const Rectangle& srcRectangle, const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPosition,
                            const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition,
                            const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const StringViewLite& strView,
                            const Vector2& dstPosition, const Color& color) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPosition,
                            const Color& color, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition,
                            const Color& color, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    virtual void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const StringViewLite& strView,
                            const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale) = 0;

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const Rectangle& dstRect, const Color& color) = 0;

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const Texture2D& srcFillTexture, const Rectangle& dstRect, const Color& color) = 0;

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const Rect& dstRect, const Color& color) = 0;

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawRectangle(const Texture2D& srcFillTexture, const Rect& dstRect, const Color& color) = 0;

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawLine(const AtlasTexture2D& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color) = 0;

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    virtual void DebugDrawLine(const Texture2D& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color) = 0;
  };
}

#endif
