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
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics/Font/FontGlyphPosition.hpp>
#include <string>
#include <vector>

namespace Fsl
{
  struct Color;
  struct Point2;
  struct Vector2;
  class TextureAtlasBitmapFont;

  template<typename T>
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
      inline static float Format(const float v)
      {
        return v;
      }
    };

    struct Flipped
    {
      inline static float Format(const float v)
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
  template<typename TNativeBatch, typename TTexture, typename TVFormatter=GenericBatch2DFormat::Flipped>
  class GenericBatch2D
  {
    static const uint32_t VERTICES_PER_QUAD = 4;
    static const uint32_t EXPAND_QUAD_GROWTH = 1024;
  public:



    typedef TTexture texture_type;
    typedef GenericBatch2DAtlasTexture<texture_type> atlas_texture_type;
    typedef TNativeBatch native_batch_type;
  private:

    struct DrawRecord
    {
      texture_type NativeTexture;
      uint32_t QuadCount;

      DrawRecord()
        : NativeTexture()
        , QuadCount(0)
      {
      }
      DrawRecord(const texture_type& nativeTexture)
        : NativeTexture(nativeTexture)
        , QuadCount(1)
      {
      }
      DrawRecord(const texture_type& nativeTexture, const uint32_t quadCount)
        : NativeTexture(nativeTexture)
        , QuadCount(quadCount)
      {
      }
    };

    native_batch_type m_native;
    std::vector<VertexPositionColorTexture> m_vertices;
    std::vector<DrawRecord> m_drawRecords;
    int32_t m_drawRecordIndex;
    int32_t m_quadCount;
    Rectangle m_screenRect;
    bool m_inBegin;
    BlendState m_blendState;
    bool m_restoreState;
    std::vector<Vector2> m_posScratchpad;
    std::vector<FontGlyphPosition> m_glyphScratchpad;
  public:
    GenericBatch2D(const native_batch_type& nativeGraphics, const Point2& screenResolution);
    virtual ~GenericBatch2D();

    void SetScreenResolution(const Point2& resolution);

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


    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Color& color);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Rectangle& dstRectangle, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Rectangle& dstRectangle, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Rect& dstRectangle, const Color& color);

    //! @brief Scale the full texture to fit inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Rect& dstRectangle, const Color& color);

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color);

    //! @brief Draw the texture area at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Rect& dstRect, const Rectangle& srcRectangle, const Color& color);

    //! @brief Scale the texture area so it fits inside the dstRectangle
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Rect& dstRect, const Rectangle& srcRectangle, const Color& color);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at dstPosition
    //! @note Do not invalidate the srcTexture before End() is called.
    void Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale);

    //! @brief Draw the full texture at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const atlas_texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Color& color);

    //! @brief Draw the full texture at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Color& color);

    //! @brief Draw the texture area at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const atlas_texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Rectangle& srcRectangle, const Color& color);

    //! @brief Draw the texture area at the given dst positions
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void Draw(const texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Rectangle& srcRectangle, const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const psz, const Vector2& dstPosition, const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition, const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param pStr a string that should be rendered
    //! @param startIndex of the first character to render from pStr
    //! @param length the number of characters to render from startIndex
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const pStr, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param startIndex of the first character to render from str
    //! @param length the number of characters to render from startIndex
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param psz a zero terminated string that should be rendered (!= nullptr)
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const psz, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param pStr a string that should be rendered
    //! @param startIndex of the first character to render from pStr
    //! @param length the number of characters to render from startIndex
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const pStr, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale);

    //! @brief Draw a ASCII string using the supplied TextureAtlasBitmapFont.
    //! @param srcTexture the texture atlas that contains the font.
    //! @param font the font to use for rendering the string
    //! @param str a string that should be rendered
    //! @param startIndex of the first character to render from str
    //! @param length the number of characters to render from startIndex
    //! @param the dstPosition to render the string at (top left corner)
    //! @param color the color to use.
    void DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale);


    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const atlas_texture_type& srcFillTexture, const Rectangle& dstRect, const Color& color);

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const texture_type& srcFillTexture, const Rectangle& dstRect, const Color& color);

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const atlas_texture_type& srcFillTexture, const Rect& dstRect, const Color& color);

    //! @brief Draw a rectangle at the given location using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawRectangle(const texture_type& srcFillTexture, const Rect& dstRect, const Color& color);

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawLine(const atlas_texture_type& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color);

    //! @brief Draw a line using a fill texture
    //! @param srcFillTexture a fill texture is texture containing a white rectangle, we will select the middle pixel of the texture and use it for rendering lines.
    //! @param color the color to use.
    //! @note Do not invalidate the srcTexture before End() is called.
    //! @note If you use this to draw a lot of instances consider using a more optimal way of rendering it.
    void DebugDrawLine(const texture_type& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color);

  private:
    void GrowCapacity();
    void GrowCapacity(const std::size_t newMinimumCapacity);
    void FlushQuads();
    inline void AddToDrawRecords(const texture_type& srcNativeTexture);
    inline void AddToDrawRecords(const texture_type& srcNativeTexture, const uint32_t quadCount);
    inline bool AdjustSourceRect(Rectangle& rSrcRect, const AtlasTextureInfo& texInfo, Vector2& rOrigin);
    inline void EnsurePosScratchpadCapacity(const uint32_t minCapacity);
    inline void Rotate2D(Vector2& rPoint0, Vector2& rPoint1, Vector2& rPoint2, Vector2& rPoint3, const float rotation) const;
  };


  // TNativeBatch is expected to be a pointer type
  // The TNativeBatch must have the ConceptNativeBatchType methods
  // The TTexture must have the ConceptTextureType methods
  // The TextureHandle must have the ConceptTextureHandle methods
  //
  //class ConceptTextureHandle
  //{
  //  int32_t q;
  //public:
  //  bool operator==(const ConceptTextureHandle& rhs) const { return q == rhs.q; }
  //  bool operator!=(const ConceptTextureHandle& rhs) const { return !(*this == rhs); }
  //};

  //class ConceptNativeBatchType
  //{
  //public:
  //  void Begin(const Point2& screenResolution, const BlendState blendState) {}
  //  void DrawQuads(const VertexPositionColorTexture*const pVertices, const int32_t length, ConceptTextureHandle& texture) {}
  //  void End() {}
  //};

  //class ConceptTextureType
  //{
  //public:
  //  ConceptTextureHandle Handle;
  //  Point2 Size;
  //  void Reset() {}

  //  bool operator==(const ConceptTextureType& rhs) const { return Handle == rhs.Handle && Size == rhs.Size; }
  //  bool operator!=(const ConceptTextureType& rhs) const { return !(*this == rhs); }
  //};
}

#endif
