#ifndef FSLGRAPHICS_RENDER_GENERICBATCH2D_HPP
#define FSLGRAPHICS_RENDER_GENERICBATCH2D_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/VectorHelper.hpp>
#include <FslGraphics/Render/GenericBatch2D_fwd.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <cassert>
#include <cstring>

namespace Fsl
{

  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::GenericBatch2D(const native_batch_type& nativeBatchType, const Point2& screenResolution)
    : m_native(nativeBatchType)
    , m_vertices(GenericBatch2D_DEFAULT_CAPACITY * VERTICES_PER_QUAD)
    , m_drawRecords(GenericBatch2D_DEFAULT_CAPACITY)
    , m_drawRecordIndex(0)
    , m_quadCount(0)
    , m_screenRect(0, 0, screenResolution.X, screenResolution.Y)
    , m_inBegin(false)
    , m_blendState(BlendState::AlphaBlend)
    , m_restoreState(false)
    , m_posScratchpad(128)
    , m_glyphScratchpad(128)
  {
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::~GenericBatch2D()
  {
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::SetScreenResolution(const Point2& resolution)
  {
    m_screenRect = Rectangle(0, 0, resolution.X, resolution.Y);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Begin()
  {
    if (m_inBegin)
      throw UsageErrorException("Already inside a begin/end block");

    m_inBegin = true;
    m_blendState = BlendState::AlphaBlend;
    m_restoreState = false;
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Begin(const BlendState blendState)
  {
    if (m_inBegin)
      throw UsageErrorException("Already inside a begin/end block");

    m_inBegin = true;
    m_blendState = blendState;
    m_restoreState = false;
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Begin(const BlendState blendState, const bool restoreState)
  {
    if (m_inBegin)
      throw UsageErrorException("Already inside a begin/end block");

    m_inBegin = true;
    m_blendState = blendState;
    m_restoreState = restoreState;
  }


  //! @brief If in a begin/end block this switches the blend state to the requested state
  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::ChangeTo(const BlendState blendState)
  {
    if (!m_inBegin)
    {
      FSLLOG_WARNING("ChangeTo called outside begin/end block, call ignored");
      return;
    }

    if (m_blendState != blendState)
    {
      // If the blend state changes we need to flush our current queue :(
      FlushQuads();
      m_blendState = blendState;
    }
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::End()
  {
    if (! m_inBegin)
      throw UsageErrorException("Not in a begin/end block");

    FlushQuads();
    m_inBegin = false;
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Color& color)
  {
    Vector2 dst(dstPosition.X + srcTexture.Info.TrimMargin.Left(), dstPosition.Y + srcTexture.Info.TrimMargin.Top());
    Draw(srcTexture.Texture, dst, srcTexture.Info.TrimmedRect, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Color& color)
  {
    Draw(srcTexture, dstPosition, Rectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Rectangle& dstRectangle, const Color& color)
  {
    if (dstRectangle.Width() > 0 && dstRectangle.Height() > 0)
    {
      Vector2 scale(dstRectangle.Width() / float(srcTexture.Info.OriginalSize.X), dstRectangle.Height() / float(srcTexture.Info.OriginalSize.Y));
      Draw(srcTexture, Vector2(dstRectangle.X(), dstRectangle.Y()), color, Vector2(), scale);
    }
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Rectangle& dstRectangle, const Color& color)
  {
    Draw(srcTexture, dstRectangle, Rectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Rect& dstRectangle, const Color& color)
  {
    if (dstRectangle.Width() > 0 && dstRectangle.Height() > 0)
    {
      Vector2 scale(dstRectangle.Width() / srcTexture.Info.OriginalSize.X, dstRectangle.Height() / srcTexture.Info.OriginalSize.Y);
      Draw(srcTexture, Vector2(dstRectangle.X(), dstRectangle.Y()), color, Vector2(), scale);
    }
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Rect& dstRectangle, const Color& color)
  {
    Draw(srcTexture, dstRectangle, Rectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color)
  {
    Vector2 origin;
    Rectangle srcRect = srcRectangle;
    if (!AdjustSourceRect(srcRect, srcTexture.Info, origin))
      return;

    Vector2 dst(dstPosition.X - origin.X, dstPosition.Y - origin.Y);
    Draw(srcTexture.Texture, dst, srcRect, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");

    const Rectangle srcTextureRectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const Rectangle clippedSrcRect = Rectangle::Intersect(srcTextureRectangle, srcRectangle);
    if (clippedSrcRect.Width() <= 0 || clippedSrcRect.Height() <= 0)
      return;

    const int32_t vertexIndex = m_quadCount * VERTICES_PER_QUAD;
    // Ensure that we have enough room for the quad
    if (static_cast<size_t>(vertexIndex + VERTICES_PER_QUAD) > m_vertices.size())
      GrowCapacity();

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3


    m_vertices[vertexIndex + 0].Position = Vector3(dstPosition.X, dstPosition.Y, 0.0f);
    m_vertices[vertexIndex + 1].Position = Vector3(dstPosition.X + clippedSrcRect.Width(), dstPosition.Y, 0.0f);
    m_vertices[vertexIndex + 2].Position = Vector3(dstPosition.X, dstPosition.Y + clippedSrcRect.Height(), 0.0f);
    m_vertices[vertexIndex + 3].Position = Vector3(dstPosition.X + clippedSrcRect.Width(), dstPosition.Y + clippedSrcRect.Height(), 0.0f);

    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRect.Left() / srcWidth;
    const float u2 = clippedSrcRect.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRect.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRect.Bottom() / srcHeight);

    m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
    m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
    m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
    m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

    const Vector4 col = color.ToVector4();
    m_vertices[vertexIndex + 0].Color = col;
    m_vertices[vertexIndex + 1].Color = col;
    m_vertices[vertexIndex + 2].Color = col;
    m_vertices[vertexIndex + 3].Color = col;
    AddToDrawRecords(srcTexture);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color)
  {
    if (dstRectangle.Width() > 0 && dstRectangle.Height() > 0)
    {
      Vector2 scale(dstRectangle.Width() / float(srcRectangle.Width()), dstRectangle.Height() / float(srcRectangle.Height()));
      Draw(srcTexture, Vector2(dstRectangle.X(), dstRectangle.Y()), srcRectangle, color, Vector2(), scale);
    }
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");

    if (dstRectangle.Width() <= 0 || dstRectangle.Height() <= 0)
      return;

    const Rectangle srcTextureRectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const Rectangle clippedSrcRect = Rectangle::Intersect(srcTextureRectangle, srcRectangle);
    if (clippedSrcRect.Width() <= 0 || clippedSrcRect.Height() <= 0)
      return;

    const int32_t vertexIndex = m_quadCount * VERTICES_PER_QUAD;
    // Ensure that we have enough room for the quad
    if (static_cast<size_t>(vertexIndex + VERTICES_PER_QUAD) > m_vertices.size())
      GrowCapacity();

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    m_vertices[vertexIndex + 0].Position = Vector3(static_cast<float>(dstRectangle.Left()), static_cast<float>(dstRectangle.Top()), 0.0f);
    m_vertices[vertexIndex + 1].Position = Vector3(static_cast<float>(dstRectangle.Right()), static_cast<float>(dstRectangle.Top()), 0.0f);
    m_vertices[vertexIndex + 2].Position = Vector3(static_cast<float>(dstRectangle.Left()), static_cast<float>(dstRectangle.Bottom()), 0.0f);
    m_vertices[vertexIndex + 3].Position = Vector3(static_cast<float>(dstRectangle.Right()), static_cast<float>(dstRectangle.Bottom()), 0.0f);

    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRect.Left() / srcWidth;
    const float u2 = clippedSrcRect.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRect.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRect.Bottom() / srcHeight);

    m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
    m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
    m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
    m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

    const Vector4 col = color.ToVector4();
    m_vertices[vertexIndex + 0].Color = col;
    m_vertices[vertexIndex + 1].Color = col;
    m_vertices[vertexIndex + 2].Color = col;
    m_vertices[vertexIndex + 3].Color = col;

    AddToDrawRecords(srcTexture);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color)
  {
    if (dstRectangle.Width() > 0 && dstRectangle.Height() > 0)
    {
      Vector2 scale(dstRectangle.Width() / srcRectangle.Width(), dstRectangle.Height() / srcRectangle.Height());
      Draw(srcTexture, Vector2(dstRectangle.X(), dstRectangle.Y()), srcRectangle, color, Vector2(), scale);
    }
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");

    if (dstRectangle.Width() <= 0 || dstRectangle.Height() <= 0)
      return;

    const Rectangle srcTextureRectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const Rectangle clippedSrcRect = Rectangle::Intersect(srcTextureRectangle, srcRectangle);
    if (clippedSrcRect.Width() <= 0 || clippedSrcRect.Height() <= 0)
      return;

    const int32_t vertexIndex = m_quadCount * VERTICES_PER_QUAD;
    // Ensure that we have enough room for the quad
    if (static_cast<size_t>(vertexIndex + VERTICES_PER_QUAD) > m_vertices.size())
      GrowCapacity();

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    m_vertices[vertexIndex + 0].Position = Vector3(static_cast<float>(dstRectangle.Left()), static_cast<float>(dstRectangle.Top()), 0.0f);
    m_vertices[vertexIndex + 1].Position = Vector3(static_cast<float>(dstRectangle.Right()), static_cast<float>(dstRectangle.Top()), 0.0f);
    m_vertices[vertexIndex + 2].Position = Vector3(static_cast<float>(dstRectangle.Left()), static_cast<float>(dstRectangle.Bottom()), 0.0f);
    m_vertices[vertexIndex + 3].Position = Vector3(static_cast<float>(dstRectangle.Right()), static_cast<float>(dstRectangle.Bottom()), 0.0f);

    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRect.Left() / srcWidth;
    const float u2 = clippedSrcRect.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRect.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRect.Bottom() / srcHeight);

    m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
    m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
    m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
    m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

    const Vector4 col = color.ToVector4();
    m_vertices[vertexIndex + 0].Color = col;
    m_vertices[vertexIndex + 1].Color = col;
    m_vertices[vertexIndex + 2].Color = col;
    m_vertices[vertexIndex + 3].Color = col;

    AddToDrawRecords(srcTexture);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMargin.Left(), origin.Y - srcTexture.Info.TrimMargin.Top());
    Draw(srcTexture.Texture, dstPosition, srcTexture.Info.TrimmedRect, color, originMod, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    Draw(srcTexture, dstPosition, Rectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color, origin, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMargin.Left(), origin.Y - srcTexture.Info.TrimMargin.Top());
    Draw(srcTexture.Texture, dstPosition, srcTexture.Info.TrimmedRect, color, rotation, originMod, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
  {
    Draw(srcTexture, dstPosition, Rectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color, rotation, origin, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    Vector2 originMod = origin;
    Rectangle srcRect = srcRectangle;
    if (!AdjustSourceRect(srcRect, srcTexture.Info, originMod))
      return;

    Draw(srcTexture.Texture, dstPosition, srcRect, color, originMod, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");

    if (scale.X <= 0.0f || scale.Y <= 0.0f)
      return;

    const Vector2 dstPos(dstPosition.X - (origin.X * scale.X), dstPosition.Y - (origin.Y * scale.Y));

    const Rectangle srcTextureRectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const Rectangle clippedSrcRect = Rectangle::Intersect(srcTextureRectangle, srcRectangle);
    if (clippedSrcRect.Width() <= 0 || clippedSrcRect.Height() <= 0)
      return;

    const int32_t vertexIndex = m_quadCount * VERTICES_PER_QUAD;
    // Ensure that we have enough room for the quad
    if (static_cast<size_t>(vertexIndex + VERTICES_PER_QUAD) > m_vertices.size())
      GrowCapacity();

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const float scaledSrcWidth = clippedSrcRect.Width() * scale.X;
    const float scaledSrcHeight = clippedSrcRect.Height() * scale.Y;

    m_vertices[vertexIndex + 0].Position = Vector3(dstPos.X, dstPos.Y, 0.0f);
    m_vertices[vertexIndex + 1].Position = Vector3(dstPos.X + scaledSrcWidth, dstPos.Y, 0.0f);
    m_vertices[vertexIndex + 2].Position = Vector3(dstPos.X, dstPos.Y + scaledSrcHeight, 0.0f);
    m_vertices[vertexIndex + 3].Position = Vector3(dstPos.X + scaledSrcWidth, dstPos.Y + scaledSrcHeight, 0.0f);

    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRect.Left() / srcWidth;
    const float u2 = clippedSrcRect.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRect.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRect.Bottom() / srcHeight);

    m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
    m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
    m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
    m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

    const Vector4 col = color.ToVector4();
    m_vertices[vertexIndex + 0].Color = col;
    m_vertices[vertexIndex + 1].Color = col;
    m_vertices[vertexIndex + 2].Color = col;
    m_vertices[vertexIndex + 3].Color = col;
    AddToDrawRecords(srcTexture);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
  {
    Vector2 originMod = origin;
    Rectangle srcRect = srcRectangle;
    if (!AdjustSourceRect(srcRect, srcTexture.Info, originMod))
      return;

    Draw(srcTexture.Texture, dstPosition, srcRect, color, rotation, originMod, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");

    if (scale.X <= 0.0f || scale.Y <= 0.0f)
      return;

    if (rotation == 0.0f)
    {
      Draw(srcTexture, dstPosition, srcRectangle, color, origin, scale);
      return;
    }

    const Rectangle srcTextureRectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const Rectangle clippedSrcRect = Rectangle::Intersect(srcTextureRectangle, srcRectangle);
    if (clippedSrcRect.Width() <= 0 || clippedSrcRect.Height() <= 0)
      return;

    const int32_t vertexIndex = m_quadCount * VERTICES_PER_QUAD;
    // Ensure that we have enough room for the quad
    if (static_cast<size_t>(vertexIndex + VERTICES_PER_QUAD) > m_vertices.size())
      GrowCapacity();

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const Vector2 scaledOrigin(origin.X * scale.X, origin.Y * scale.Y);

    Vector2 vec0(OptimizationFlag::NoInitialization);
    Vector2 vec1(OptimizationFlag::NoInitialization);
    Vector2 vec2(OptimizationFlag::NoInitialization);
    Vector2 vec3(OptimizationFlag::NoInitialization);
    { // rotate
      const float dx = dstPosition.X;
      const float dy = dstPosition.Y;
      const float left = -scaledOrigin.X;
      const float right = left + (clippedSrcRect.Width() * scale.X);
      const float top = -scaledOrigin.Y;
      const float bottom = top + (clippedSrcRect.Height() * scale.Y);
      vec0 = Vector2(left, top);
      vec1 = Vector2(right, top);
      vec2 = Vector2(left, bottom);
      vec3 = Vector2(right, bottom);

      Rotate2D(vec0, vec1, vec2, vec3, rotation);
      vec0.X += dx;
      vec1.X += dx;
      vec2.X += dx;
      vec3.X += dx;
      vec0.Y += dy;
      vec1.Y += dy;
      vec2.Y += dy;
      vec3.Y += dy;
    }

    m_vertices[vertexIndex + 0].Position = Vector3(vec0.X, vec0.Y, 0.0f);
    m_vertices[vertexIndex + 1].Position = Vector3(vec1.X, vec1.Y, 0.0f);
    m_vertices[vertexIndex + 2].Position = Vector3(vec2.X, vec2.Y, 0.0f);
    m_vertices[vertexIndex + 3].Position = Vector3(vec3.X, vec3.Y, 0.0f);

    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRect.Left() / srcWidth;
    const float u2 = clippedSrcRect.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRect.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRect.Bottom() / srcHeight);

    m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
    m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
    m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
    m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

    const Vector4 col = color.ToVector4();
    m_vertices[vertexIndex + 0].Color = col;
    m_vertices[vertexIndex + 1].Color = col;
    m_vertices[vertexIndex + 2].Color = col;
    m_vertices[vertexIndex + 3].Color = col;
    AddToDrawRecords(srcTexture);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Color& color)
  {
    if (pDstPositions == nullptr)
      throw std::invalid_argument("pDstPositions is invalid");
    EnsurePosScratchpadCapacity(dstPositionsLength);

    // adjust all the dst coordinates
    const float offsetX = static_cast<float>(srcTexture.Info.TrimMargin.Left());
    const float offsetY = static_cast<float>(srcTexture.Info.TrimMargin.Top());
    for (uint32_t i = 0; i < dstPositionsLength; ++i)
    {
      m_posScratchpad[i] = Vector2(pDstPositions[i].X + offsetX, pDstPositions[i].Y + offsetY);
    }

    Draw(srcTexture.Texture, m_posScratchpad.data(), dstPositionsLength, srcTexture.Info.TrimmedRect, color);
  }




  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Color& color)
  {
    Draw(srcTexture, pDstPositions, dstPositionsLength, Rectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Rectangle& srcRectangle, const Color& color)
  {
    if (pDstPositions == nullptr)
      throw std::invalid_argument("pDstPositions or dstPositions length is invalid");

    Vector2 origin;
    Rectangle srcRect = srcRectangle;
    if (!AdjustSourceRect(srcRect, srcTexture.Info, origin))
      return;

    EnsurePosScratchpadCapacity(dstPositionsLength);

    // adjust all the dst coordinates
    for (uint32_t i = 0; i < dstPositionsLength; ++i)
    {
      m_posScratchpad[i] = Vector2(pDstPositions[i].X - origin.X, pDstPositions[i].Y - origin.Y);
    }

    Draw(srcTexture.Texture, m_posScratchpad.data(), dstPositionsLength, srcRect, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2*const pDstPositions, const uint32_t dstPositionsLength, const Rectangle& srcRectangle, const Color& color)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    if (pDstPositions == nullptr || dstPositionsLength <= 0)
    {
      FSLLOG_WARNING_IF(pDstPositions == nullptr , "It's invalid to specify null for positions");
      FSLLOG_WARNING_IF(dstPositionsLength < 0, "Its invalid to specify a negative length");
      return;
    }

    const Rectangle srcTextureRectangle(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const Rectangle clippedSrcRect = Rectangle::Intersect(srcTextureRectangle, srcRectangle);
    if (clippedSrcRect.Width() <= 0 || clippedSrcRect.Height() <= 0)
      return;

    int32_t vertexIndex = m_quadCount * VERTICES_PER_QUAD;
    const std::size_t minimumCapacity = static_cast<size_t>(vertexIndex + (dstPositionsLength * VERTICES_PER_QUAD));
    // Ensure that we have enough room for the quad
    if (minimumCapacity > m_vertices.size())
      GrowCapacity(minimumCapacity);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRect.Left() / srcWidth;
    const float u2 = clippedSrcRect.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRect.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRect.Bottom() / srcHeight);

    const Vector2* pDstPosition = pDstPositions;
    const Vector2*const pDstPositionEnd = pDstPositions + dstPositionsLength;
    const Vector4 col = color.ToVector4();

    while (pDstPosition < pDstPositionEnd)
    {
      m_vertices[vertexIndex + 0].Position = Vector3(pDstPosition->X, pDstPosition->Y, 0.0f);
      m_vertices[vertexIndex + 1].Position = Vector3(pDstPosition->X + clippedSrcRect.Width(), pDstPosition->Y, 0.0f);
      m_vertices[vertexIndex + 2].Position = Vector3(pDstPosition->X, pDstPosition->Y + clippedSrcRect.Height(), 0.0f);
      m_vertices[vertexIndex + 3].Position = Vector3(pDstPosition->X + clippedSrcRect.Width(), pDstPosition->Y + clippedSrcRect.Height(), 0.0f);

      m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
      m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
      m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
      m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

      m_vertices[vertexIndex + 0].Color = col;
      m_vertices[vertexIndex + 1].Color = col;
      m_vertices[vertexIndex + 2].Color = col;
      m_vertices[vertexIndex + 3].Color = col;
      ++pDstPosition;
      vertexIndex += VERTICES_PER_QUAD;
    }
    AddToDrawRecords(srcTexture, dstPositionsLength);
  }

  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const psz, const Vector2& dstPosition, const Color& color)
  {
    if (psz == nullptr)
      throw std::invalid_argument("psz can not be null");

    DrawString(srcTexture, font, psz, 0, static_cast<int32_t>(std::strlen(psz)), dstPosition, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition, const Color& color)
  {
    DrawString(srcTexture, font, str.c_str(), 0, static_cast<int32_t>(str.size()), dstPosition, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const pStr, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");

    if (pStr == nullptr)
      throw std::invalid_argument("psz can not be null");
    if (startIndex < 0 || length < 0)
      throw std::invalid_argument("startIndex or length is invalid");

    // Ensure we have enough room for our quads
    int32_t vertexIndex = (m_quadCount * VERTICES_PER_QUAD);
    const std::size_t minQuadCapacity = std::size_t(m_quadCount + length);
    if (minQuadCapacity > m_vertices.size())
      GrowCapacity(minQuadCapacity);

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < std::size_t(length))
      m_glyphScratchpad.resize(length);

    // Extract the render rules
    font.ExtractRenderRules(m_glyphScratchpad, pStr, 0, length);

    const Vector4 col = color.ToVector4();
    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);

    for (int32_t i = 0; i < length; ++i)
    {
      Vector2 dstPos = m_glyphScratchpad[i].DstOffset;
      dstPos.X += dstPosition.X;
      dstPos.Y += dstPosition.Y;
      const Rectangle& srcRect = m_glyphScratchpad[i].SrcRect;
      if (srcRect.Width() > 0)
      {
        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3

        m_vertices[vertexIndex + 0].Position = Vector3(dstPos.X, dstPos.Y, 0.0f);
        m_vertices[vertexIndex + 1].Position = Vector3(dstPos.X + srcRect.Width(), dstPos.Y, 0.0f);
        m_vertices[vertexIndex + 2].Position = Vector3(dstPos.X, dstPos.Y + srcRect.Height(), 0.0f);
        m_vertices[vertexIndex + 3].Position = Vector3(dstPos.X + srcRect.Width(), dstPos.Y + srcRect.Height(), 0.0f);

        const float u1 = srcRect.Left() / srcWidth;
        const float u2 = srcRect.Right() / srcWidth;
        const float v1 = TVFormatter::Format(srcRect.Top() / srcHeight);
        const float v2 = TVFormatter::Format(srcRect.Bottom() / srcHeight);

        m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
        m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
        m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
        m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

        m_vertices[vertexIndex + 0].Color = col;
        m_vertices[vertexIndex + 1].Color = col;
        m_vertices[vertexIndex + 2].Color = col;
        m_vertices[vertexIndex + 3].Color = col;
        vertexIndex += 4;

        AddToDrawRecords(srcTexture);
      }
    }
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color)
  {
    if (startIndex < 0 || length < 0 || std::size_t(startIndex + length) > str.size())
      throw std::invalid_argument("startIndex or length is invalid");

    DrawString(srcTexture, font, str.c_str(), startIndex, length, dstPosition, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const psz, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    if (psz == nullptr)
      throw std::invalid_argument("psz can not be null");

    DrawString(srcTexture, font, psz, 0, static_cast<int32_t>(std::strlen(psz)), dstPosition, color, origin, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    DrawString(srcTexture, font, str.c_str(), 0, static_cast<int32_t>(str.size()), dstPosition, color, origin, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const char*const pStr, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    if (!m_inBegin)
      throw UsageErrorException("Draw can only occur inside a begin/end block");

    if (pStr == nullptr)
      throw std::invalid_argument("psz can not be null");
    if (startIndex < 0 || length < 0)
      throw std::invalid_argument("startIndex or length is invalid");

    if (scale.X <= 0.0f || scale.Y <= 0.0f)
      return;

    // Ensure we have enough room for our quads
    int32_t vertexIndex = (m_quadCount * VERTICES_PER_QUAD);
    const std::size_t minQuadCapacity = std::size_t(m_quadCount + length);
    if (minQuadCapacity > m_vertices.size())
      GrowCapacity(minQuadCapacity);

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < std::size_t(length))
      m_glyphScratchpad.resize(length);

    // Extract the render rules
    font.ExtractRenderRules(m_glyphScratchpad, pStr, 0, length);

    const Vector2 dstPosModded(dstPosition.X - (origin.X * scale.X), dstPosition.Y - (origin.Y * scale.Y));
    const Vector4 col = color.ToVector4();
    const float srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const float srcHeight = static_cast<float>(srcTexture.Extent.Height);

    for (int32_t i = 0; i < length; ++i)
    {
      const Vector2 dstPos((m_glyphScratchpad[i].DstOffset.X * scale.X) + dstPosModded.X,
                           (m_glyphScratchpad[i].DstOffset.Y * scale.Y) + dstPosModded.Y);

      const Rectangle& srcRect = m_glyphScratchpad[i].SrcRect;
      if (srcRect.Width() > 0)
      {
        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3
        const float scaledSrcWidth = srcRect.Width() * scale.X;
        const float scaledSrcHeight = srcRect.Height() * scale.Y;

        m_vertices[vertexIndex + 0].Position = Vector3(dstPos.X, dstPos.Y, 0.0f);
        m_vertices[vertexIndex + 1].Position = Vector3(dstPos.X + scaledSrcWidth, dstPos.Y, 0.0f);
        m_vertices[vertexIndex + 2].Position = Vector3(dstPos.X, dstPos.Y + scaledSrcHeight, 0.0f);
        m_vertices[vertexIndex + 3].Position = Vector3(dstPos.X + scaledSrcWidth, dstPos.Y + scaledSrcHeight, 0.0f);

        const float u1 = srcRect.Left() / srcWidth;
        const float u2 = srcRect.Right() / srcWidth;
        const float v1 = TVFormatter::Format(srcRect.Top() / srcHeight);
        const float v2 = TVFormatter::Format(srcRect.Bottom() / srcHeight);

        m_vertices[vertexIndex + 0].TextureCoordinate = Vector2(u1, v1);
        m_vertices[vertexIndex + 1].TextureCoordinate = Vector2(u2, v1);
        m_vertices[vertexIndex + 2].TextureCoordinate = Vector2(u1, v2);
        m_vertices[vertexIndex + 3].TextureCoordinate = Vector2(u2, v2);

        m_vertices[vertexIndex + 0].Color = col;
        m_vertices[vertexIndex + 1].Color = col;
        m_vertices[vertexIndex + 2].Color = col;
        m_vertices[vertexIndex + 3].Color = col;
        vertexIndex += 4;

        AddToDrawRecords(srcTexture);
      }
    }
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
  {
    if (startIndex < 0 || length < 0 || std::size_t(startIndex + length) > str.size())
      throw std::invalid_argument("startIndex or length is invalid");

    DrawString(srcTexture, font, str.c_str(), startIndex, length, dstPosition, color, origin, scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const atlas_texture_type& srcFillTexture, const Rectangle& dstRect, const Color& color)
  {
    const auto texSize = srcFillTexture.Info.OriginalSize;
    const Rectangle srcRect(texSize.X / 2, texSize.Y / 2, 1, 1);
    Rectangle dstRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Bottom() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Top());
    dstRectangle.SetWidth(1);
    dstRectangle.SetHeight(dstRect.Height());
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetX(dstRect.Right() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const texture_type& srcFillTexture, const Rectangle& dstRect, const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const Rectangle srcRect(texExtent.Width / 2, texExtent.Height / 2, 1, 1);
    Rectangle dstRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Bottom() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Top());
    dstRectangle.SetWidth(1);
    dstRectangle.SetHeight(dstRect.Height());
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetX(dstRect.Right() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const atlas_texture_type& srcFillTexture, const Rect& dstRect, const Color& color)
  {
    const auto texSize = srcFillTexture.Info.OriginalSize;
    const Rectangle srcRect(texSize.X / 2, texSize.Y / 2, 1, 1);
    Rect dstRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Bottom() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Top());
    dstRectangle.SetWidth(1);
    dstRectangle.SetHeight(dstRect.Height());
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetX(dstRect.Right() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const texture_type& srcFillTexture, const Rect& dstRect, const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const Rectangle srcRect(texExtent.Width / 2, texExtent.Height / 2, 1, 1);
    Rect dstRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Bottom() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetY(dstRect.Top());
    dstRectangle.SetWidth(1);
    dstRectangle.SetHeight(dstRect.Height());
    Draw(srcFillTexture, dstRectangle, srcRect, color);
    dstRectangle.SetX(dstRect.Right() - 1);
    Draw(srcFillTexture, dstRectangle, srcRect, color);
  }

  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const atlas_texture_type& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color)
  {
    const auto texSize = srcFillTexture.Info.OriginalSize;
    const Rectangle srcRect(texSize.X / 2, texSize.Y / 2, 1, 1);

    Vector2 delta(dstTo.X - dstFrom.X, dstTo.Y - dstFrom.Y);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, dstFrom, srcRect, color, rotation, Vector2(), scale);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const texture_type& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const Rectangle srcRect(texExtent.Width / 2, texExtent.Height / 2, 1, 1);

    Vector2 delta(dstTo.X - dstFrom.X, dstTo.Y - dstFrom.Y);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, dstFrom, srcRect, color, rotation, Vector2(), scale);
  }



  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::GrowCapacity()
  {
    const std::size_t growBy = static_cast<std::size_t>(EXPAND_QUAD_GROWTH);
    const std::size_t growQuadBy = growBy * static_cast<std::size_t>(VERTICES_PER_QUAD);
    m_vertices.resize(m_vertices.size() + growQuadBy);
    m_drawRecords.resize(m_drawRecords.size() + growBy);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::GrowCapacity(const std::size_t newMinimumCapacity)
  {
    assert(newMinimumCapacity > m_drawRecords.size());

    const std::size_t newCapacity = newMinimumCapacity + EXPAND_QUAD_GROWTH;
    const std::size_t newVertexCapacity = newCapacity * static_cast<std::size_t>(VERTICES_PER_QUAD);
    m_vertices.resize(newVertexCapacity);
    m_drawRecords.resize(newCapacity);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::FlushQuads()
  {
    const Point2 screenResolution(m_screenRect.Width(), m_screenRect.Height());
    m_native->Begin(screenResolution, m_blendState, m_restoreState);
    std::size_t vertexStartIndex = 0;
    for (int32_t i = 0; i < m_drawRecordIndex; ++i)
    {
      m_native->DrawQuads(m_vertices.data() + vertexStartIndex, m_drawRecords[i].QuadCount, m_drawRecords[i].NativeTexture);
      // release the texture pointer so we don't keep it alive by accident
      m_drawRecords[i].NativeTexture.Reset();
      vertexStartIndex += m_drawRecords[i].QuadCount * VERTICES_PER_QUAD;
    }
    m_native->End();

    m_drawRecordIndex = 0;
    m_quadCount = 0;
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::AddToDrawRecords(const texture_type& srcNativeTexture)
  {
    // Only add a new draw record if the 'draw-state' changed
    if (m_drawRecordIndex > 0 && srcNativeTexture == m_drawRecords[m_drawRecordIndex - 1].NativeTexture)
    {
      ++m_drawRecords[m_drawRecordIndex - 1].QuadCount;
    }
    else
    {
      m_drawRecords[m_drawRecordIndex] = DrawRecord(srcNativeTexture);
      ++m_drawRecordIndex;
    }
    ++m_quadCount;
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::AddToDrawRecords(const texture_type& srcNativeTexture, const uint32_t quadCount)
  {
    // Only add a new draw record if the 'draw-state' changed
    if (m_drawRecordIndex > 0 && srcNativeTexture == m_drawRecords[m_drawRecordIndex - 1].NativeTexture)
    {
      m_drawRecords[m_drawRecordIndex - 1].QuadCount += quadCount;
    }
    else
    {
      m_drawRecords[m_drawRecordIndex] = DrawRecord(srcNativeTexture, quadCount);
      ++m_drawRecordIndex;
    }
    m_quadCount += quadCount;
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  bool GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::AdjustSourceRect(Rectangle& rSrcRect, const AtlasTextureInfo& texInfo, Vector2& rOrigin)
  {
    rSrcRect.Add(texInfo.Offset);
    const int32_t rectRight = rSrcRect.Right();
    const int32_t rectBottom = rSrcRect.Bottom();

    // Early abort
    if (rSrcRect.X() >= texInfo.TrimmedRect.Right() || rSrcRect.Y() >= texInfo.TrimmedRect.Bottom() || rectRight <= texInfo.TrimmedRect.Left() || rectBottom <= texInfo.TrimmedRect.Top())
    {
      rSrcRect = Rectangle();
      return false;
    }

    // clip left
    if (rSrcRect.X() < texInfo.TrimmedRect.Left())
    {
      int dist = texInfo.TrimmedRect.Left() - rSrcRect.X();
      rSrcRect.SetX(texInfo.TrimmedRect.Left());
      rSrcRect.SetWidth(rSrcRect.Width() - dist);
      rOrigin.X -= dist;
    }

    // clip top
    if (rSrcRect.Y() < texInfo.TrimmedRect.Top())
    {
      int dist = texInfo.TrimmedRect.Top() - rSrcRect.Y();
      rSrcRect.SetY(texInfo.TrimmedRect.Top());
      rSrcRect.SetHeight(rSrcRect.Height() - dist);
      rOrigin.Y -= dist;
    }

    // clip right
    if (rectRight > texInfo.TrimmedRect.Right())
      rSrcRect.SetWidth(rSrcRect.Width() - (rectRight - texInfo.TrimmedRect.Right()));

    // clip bottom
    if (rectBottom > texInfo.TrimmedRect.Bottom())
      rSrcRect.SetHeight(rSrcRect.Height() - (rectBottom - texInfo.TrimmedRect.Bottom()));
    return true;
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::EnsurePosScratchpadCapacity(const uint32_t minCapacity)
  {
    const std::size_t newMinCapacity = minCapacity;
    if (newMinCapacity < m_posScratchpad.size())
      return;
    m_posScratchpad.resize(newMinCapacity + 32);
  }


  template<typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Rotate2D(Vector2& rPoint0, Vector2& rPoint1, Vector2& rPoint2, Vector2& rPoint3, const float rotation) const
  {
    const float cosR = std::cos(rotation);
    const float sinR = std::sin(rotation);

    rPoint0 = Vector2((rPoint0.X * cosR - rPoint0.Y * sinR), (rPoint0.Y * cosR + rPoint0.X * sinR));
    rPoint1 = Vector2((rPoint1.X * cosR - rPoint1.Y * sinR), (rPoint1.Y * cosR + rPoint1.X * sinR));
    rPoint2 = Vector2((rPoint2.X * cosR - rPoint2.Y * sinR), (rPoint2.Y * cosR + rPoint2.X * sinR));
    rPoint3 = Vector2((rPoint3.X * cosR - rPoint3.Y * sinR), (rPoint3.Y * cosR + rPoint3.X * sinR));
  }


}

#endif
