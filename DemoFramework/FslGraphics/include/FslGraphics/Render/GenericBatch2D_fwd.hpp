#ifndef FSLGRAPHICS_RENDER_GENERICBATCH2D_FWD_HPP
#define FSLGRAPHICS_RENDER_GENERICBATCH2D_FWD_HPP
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
#include <FslGraphics/Font/FontGlyphPosition.hpp>
#include <FslGraphics/Render/BatchEffect.hpp>
#include <FslGraphics/Render/BatchSdfRenderConfig.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Render/Stats/Batch2DStats.hpp>
#include <FslGraphics/Render/Stats/GenericBatch2DStats.hpp>
#include <FslGraphics/Render/Strategy/StrategyBatchByState.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <string>
#include <vector>

namespace Fsl
{
  struct BitmapFontConfig;
  struct Color;
  struct Point2;
  struct NativeTextureArea;
  struct PxClipRectangle;
  struct PxAreaRectangleF;
  struct PxExtent2D;
  struct PxRectangle;
  struct PxRectangleU;
  class StringViewLite;
  struct Vector2;
  struct Vector4;
  class TextureAtlasBitmapFont;

  template <typename T>
  struct GenericBatch2DAtlasTexture
  {
    T Texture;
    AtlasTextureInfo Info;

    GenericBatch2DAtlasTexture(T texture, AtlasTextureInfo info)
      : Texture(texture)
      , Info(info)
    {
    }
  };

  namespace GenericBatch2DFormat
  {
    struct Normal
    {
      inline static constexpr float Format(const float v)
      {
        return v;
      }
    };

    struct Flipped
    {
      inline static constexpr float Format(const float v)
      {
        return 1.0f - v;
      }
    };
  }

  const uint32_t GenericBatch2D_DEFAULT_CAPACITY = 2048;

  //! @brief A really simple API independent way to draw some graphics
  //!        All methods operate in screen coordinate pixel mode where 0,0 is the top left corner and
  //!        the bottom right corner is equal to the display width-1,height-1
  //! @note  This API provides a form of batched immediate mode, so it will be slower than properly
  //         optimized graphics, but its faster to get something running and good for debugging.
  template <typename TNativeBatch, typename TTexture, typename TVFormatter = GenericBatch2DFormat::Flipped>
  class GenericBatch2D
  {
    static const uint32_t VERTICES_PER_QUAD = 4;
    static const uint32_t EXPAND_QUAD_GROWTH = 1024;

  public:
    using texture_type = TTexture;
    using atlas_texture_type = GenericBatch2DAtlasTexture<texture_type>;
    using native_batch_type = TNativeBatch;
    using stategy_type = StrategyBatchByState<texture_type>;

  private:
    stategy_type m_batchStrategy;
    native_batch_type m_native;
    PxRectangle m_screenRect;
    bool m_inBegin;
    bool m_restoreState;
    std::vector<Vector2> m_posScratchpad;
    std::vector<FontGlyphPosition> m_glyphScratchpad;
    GenericBatch2DStats m_stats;

  public:
    GenericBatch2D(const native_batch_type& nativeBatchType, const PxExtent2D& currentExtent);
    virtual ~GenericBatch2D();

    void SetScreenExtent(const PxExtent2D& extentPx);

    //! @brief Begin drawing. Defaults to blendState == BlendState::AlphaBlend, restoreState == false
    void Begin();

    //! @brief Begin drawing
    //! @param blendState the BlendState to use
    void Begin(const BlendState blendState);

    //! @brief Begin drawing
    //! @param blendState the BlendState to use
    //! @param restoreState if true all native state that is modified by Batch2D will be restored to their initial setting
    void Begin(const BlendState blendState, const bool restoreState);

    //! @brief If in a begin/end block this switches the blend state to the requested state
    void ChangeTo(const BlendState blendState);

    void End();

    // ---------- 0

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Color& color);

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Vector4& color);

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Color& color);

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Vector4& color);

    // ---------- 0 with clip

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Color& color,
              const PxClipRectangle& clipRectPx);

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Vector4& color,
              const PxClipRectangle& clipRectPx);

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Color& color,
              const PxClipRectangle& clipRectPx);

    //! @brief draw the texture at dstRect using the native texture area
    //! @note Low level draw access (almost a passthrough function)
    void Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf, const Vector4& color,
              const PxClipRectangle& clipRectPx);

    // ---------- 1

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxRectangle& dstRectanglePx, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxRectangle& dstRectanglePx, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color);

    void Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color, const BatchEffect effect);
    void Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color, const BatchEffect effect);

    // ---------- 2

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    // ---------- 2 with clip

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx);

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx);

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, clipRectPx);
    }

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, clipRectPx);
    }

    // ---------- 2A
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const BatchEffect effect);
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const BatchEffect effect);

    // ---------- 3

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, dstRectanglePx, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, dstRectanglePx, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    // ---------- 4

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, dstRectanglePxf, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, dstRectanglePxf, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    // ---------- 4 With clip

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx)
    {
      Draw(srcTexture, dstRectanglePxf, ClampConvertToPxRectangleU(srcRectanglePx), color, clipRectPx);
    }

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangle& srcRectanglePx, const Color& color,
              const PxClipRectangle& clipRectPx)
    {
      Draw(srcTexture, dstRectanglePxf, ClampConvertToPxRectangleU(srcRectanglePx), color, clipRectPx);
    }

    // ---------- 4A

    //! @brief Scale the texture area so it fits inside the dstRectangle
    void Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const BatchEffect effect);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    void Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const BatchEffect effect);

    // ---------- 5

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale,
              const BatchEffect effect);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale,
              const BatchEffect effect);

    // ---------- 6

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color, const float rotation, const Vector2& origin,
              const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color, const float rotation, const Vector2& origin,
              const Vector2& scale);

    // ---------- 7

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, origin, scale);
    }

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, origin, scale);
    }

    // ---------- 7 with clip

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, origin, scale, clipRectPx);
    }

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, origin, scale, clipRectPx);
    }

    // ---------- 7a

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale, const BatchEffect effect);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const Vector2& origin, const Vector2& scale, const BatchEffect effect);

    // ---------- 8


    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const float rotation, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU& srcRectanglePx, const Color& color,
              const float rotation, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const float rotation, const Vector2& origin, const Vector2& scale)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, rotation, origin, scale);
    }

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const PxRectangle& srcRectanglePx, const Color& color,
              const float rotation, const Vector2& origin, const Vector2& scale)
    {
      Draw(srcTexture, dstPositionPxf, ClampConvertToPxRectangleU(srcRectanglePx), color, rotation, origin, scale);
    }

    // ---------- 9

    //! @brief Draw the full texture at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const atlas_texture_type& srcTexture, const Vector2* const pDstPositionsPxf, const uint32_t dstPositionsLength, const Color& color);

    //! @brief Draw the full texture at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const texture_type& srcTexture, const Vector2* const pDstPositionsPxf, const uint32_t dstPositionsLength, const Color& color);

    // ---------- 10

    //! @brief Draw the texture area at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const atlas_texture_type& srcTexture, const Vector2* const pDstPositionsPxf, const uint32_t dstPositionsLength,
              const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Draw the texture area at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const texture_type& srcTexture, const Vector2* const pDstPositionsPxf, const uint32_t dstPositionsLength,
              const PxRectangleU& srcRectanglePx, const Color& color);

    //! @brief Draw the texture area at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const atlas_texture_type& srcTexture, const Vector2* const pDstPositionsPxf, const uint32_t dstPositionsLength,
              const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, pDstPositionsPxf, dstPositionsLength, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    //! @brief Draw the texture area at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const texture_type& srcTexture, const Vector2* const pDstPositionsPxf, const uint32_t dstPositionsLength,
              const PxRectangle& srcRectanglePx, const Color& color)
    {
      Draw(srcTexture, pDstPositionsPxf, dstPositionsLength, ClampConvertToPxRectangleU(srcRectanglePx), color);
    }

    // ---------- 11

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string view to render
    //! @param dstPosition to render the string at (top left corner) in pixels. As long as the dstPositionPxf is pixel aligned the rendered font
    //! will also be pixel perfectly aligned.
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf,
                    const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPositionPxf,
                    const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPositionPxf,
                    const Color& color);

    // ---------- 12

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string view to render
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf,
                    const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPositionPxf,
                    const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPositionPxf,
                    const Color& color, const Vector2& origin, const Vector2& scale);

    // ---------- 13

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param fontConfig the font configuration.
    //! @param strView the string view to render
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                    const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param fontConfig the font configuration.
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const char* const psz,
                    const Vector2& dstPositionPxf, const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param fontConfig the font configuration.
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const std::string& str,
                    const Vector2& dstPositionPxf, const Color& color);

    // ---------- 13 with clip

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param fontConfig the font configuration.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    //! @param clipRectPx the rendering will be clipped against this
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                    const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const PxClipRectangle& clipRectPx);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param fontConfig the font configuration.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    //! @param clipRectPx the rendering will be clipped against this
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const char* const psz,
                    const Vector2& dstPositionPxf, const Color& color, const PxClipRectangle& clipRectPx);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param fontConfig the font configuration.
    //! @param font the font to use for rendering the string
    //! @param strView the string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    //! @param clipRectPx the rendering will be clipped against this
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const std::string& str,
                    const Vector2& dstPositionPxf, const Color& color, const PxClipRectangle& clipRectPx);

    // ---------- 14

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string view to render
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                    const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const char* const psz,
                    const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const std::string& str,
                    const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale);

    // ---------- 14 with clip

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param strView the string view to render
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig,
                    const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale,
                    const PxClipRectangle& clipRectPx);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const char* const psz,
                    const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale,
                    const PxClipRectangle& clipRectPx);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig, const std::string& str,
                    const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale,
                    const PxClipRectangle& clipRectPx);

    // ----------

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const atlas_texture_type& srcFillTexture, const PxRectangle& dstRectanglePx, const Color& color);

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const texture_type& srcFillTexture, const PxRectangle& dstRectanglePx, const Color& color);

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const atlas_texture_type& srcFillTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color);

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const texture_type& srcFillTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color);

    // ----------

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawLine(const atlas_texture_type& srcFillTexture, const Vector2& dstFromPxf, const Vector2& dstToPxf, const Color& color);

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for
    //! rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawLine(const texture_type& srcFillTexture, const Vector2& dstFromPxf, const Vector2& dstToPxf, const Color& color);

    Batch2DStats GetStats() const;

  protected:
    inline PxRectangleU ClampConvertToPxRectangleU(const PxRectangle& value) const
    {
      // If left and right is below zero clipping will occur (we consider this a error, hence the reason for the assert)
      // width and height should always be >= 0 in a Rectangle
      assert(value.Width() >= 0);
      assert(value.Height() >= 0);
      auto clippedLeft = std::max(value.Left(), 0);
      auto clippedTop = std::max(value.Top(), 0);
      auto clippedRight = std::max(value.Right(), clippedLeft);
      auto clippedBottom = std::max(value.Bottom(), clippedTop);
      assert(clippedLeft >= 0 && clippedTop >= 0 && clippedLeft <= clippedRight && clippedTop <= clippedBottom);
      assert((clippedRight - clippedLeft) <= value.Width());
      assert((clippedBottom - clippedTop) <= value.Height());
      return PxRectangleU::FromLeftTopRightBottom(
        static_cast<PxRectangleU::value_type>(clippedLeft), static_cast<PxRectangleU::value_type>(clippedTop),
        static_cast<PxRectangleU::value_type>(clippedRight), static_cast<PxRectangleU::value_type>(clippedBottom), OptimizationCheckFlag::NoCheck);
    }


  private:
    void FlushQuads();
    inline void EnsurePosScratchpadCapacity(const uint32_t minCapacity);
    inline void Rotate2D(Vector2& rPoint0, Vector2& rPoint1, Vector2& rPoint2, Vector2& rPoint3, const float rotation) const;
    inline BatchSdfRenderConfig ToBatchSdfRenderConfig(const TextureAtlasBitmapFont& font, const BitmapFontConfig& fontConfig);
  };


  // TNativeBatch is expected to be a pointer type
  // The TNativeBatch must have the ConceptNativeBatchType methods
  // The TTexture must have the ConceptTextureType methods
  // The TextureHandle must have the ConceptTextureHandle methods
  //
  // class ConceptTextureHandle
  //{
  //  int32_t q;
  // public:
  //  bool operator==(const ConceptTextureHandle& rhs) const { return q == rhs.q; }
  //  bool operator!=(const ConceptTextureHandle& rhs) const { return !(*this == rhs); }
  //};

  // class ConceptNativeBatchType
  //{
  // public:
  //  void Begin(const Point2& screenResolution, const BlendState blendState) {}
  //  void DrawQuads(const VertexPositionColorTexture*const pVertices, const int32_t length, ConceptTextureHandle& texture) {}
  //  void End() {}
  //};

  // class ConceptTextureType
  //{
  // public:
  //  ConceptTextureHandle Handle;
  //  Point2 Size;
  //  void Reset() {}

  //  bool operator==(const ConceptTextureType& rhs) const { return Handle == rhs.Handle && Size == rhs.Size; }
  //  bool operator!=(const ConceptTextureType& rhs) const { return !(*this == rhs); }
  //};
}

#endif
