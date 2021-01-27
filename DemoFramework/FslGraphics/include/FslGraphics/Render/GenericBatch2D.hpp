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
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/VectorHelper.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/BitmapFontConfig.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/Render/Batch2DUtil.hpp>
#include <FslGraphics/Render/GenericBatch2D_fwd.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfoUtil.hpp>
#include <cassert>
#include <cstring>

namespace Fsl
{
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::GenericBatch2D(const native_batch_type& nativeBatchType, const PxExtent2D& currentExtent)
    : m_batchStrategy(GenericBatch2D_DEFAULT_CAPACITY)
    , m_native(nativeBatchType)
    , m_screenRect(0, 0, currentExtent.Width, currentExtent.Height)
    , m_inBegin(false)
    , m_restoreState(false)
    , m_posScratchpad(128)
    , m_glyphScratchpad(128)
  {
    m_batchStrategy.SetBlendState(BlendState::AlphaBlend);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::~GenericBatch2D() = default;


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::SetScreenExtent(const PxExtent2D& extentPx)
  {
    m_screenRect = PxRectangle(0, 0, extentPx.Width, extentPx.Height);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Begin()
  {
    if (m_inBegin)
    {
      throw UsageErrorException("Already inside a begin/end block");
    }

    m_inBegin = true;
    m_batchStrategy.SetBlendState(BlendState::AlphaBlend);
    m_restoreState = false;
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Begin(const BlendState blendState)
  {
    if (m_inBegin)
    {
      throw UsageErrorException("Already inside a begin/end block");
    }

    m_inBegin = true;
    m_batchStrategy.SetBlendState(blendState);
    m_restoreState = false;
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Begin(const BlendState blendState, const bool restoreState)
  {
    if (m_inBegin)
    {
      throw UsageErrorException("Already inside a begin/end block");
    }

    m_inBegin = true;
    m_batchStrategy.SetBlendState(blendState);
    m_restoreState = restoreState;
  }


  //! @brief If in a begin/end block this switches the blend state to the requested state
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::ChangeTo(const BlendState blendState)
  {
    if (!m_inBegin)
    {
      FSLLOG3_WARNING("ChangeTo called outside begin/end block, call ignored");
      return;
    }

    if (blendState != m_batchStrategy.GetActiveBlendState())
    {
      m_batchStrategy.SetBlendState(blendState);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::End()
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Not in a begin/end block");
    }

    FlushQuads();
    m_inBegin = false;
  }

  // ---------- 0

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);
    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, color.ToVector4());
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Vector4& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);
    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, color);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);
    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, color.ToVector4());
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Vector4& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);
    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, color);
  }

  // ---------- 0 with clip

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    PxAreaRectangleF clippedDstRectPxf(dstRectanglePxf);
    NativeTextureArea clippedSrcArea(srcArea);
    if (Batch2DUtil::Clip(clippedDstRectPxf, clippedSrcArea, clipRectPx))
    {
      m_batchStrategy.EnsureCapacityFor(1);
      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, color.ToVector4());
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeTextureArea& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Vector4& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    PxAreaRectangleF clippedDstRectPxf(dstRectanglePxf);
    NativeTextureArea clippedSrcArea(srcArea);
    if (Batch2DUtil::Clip(clippedDstRectPxf, clippedSrcArea, clipRectPx))
    {
      m_batchStrategy.EnsureCapacityFor(1);
      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, color);
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    PxAreaRectangleF clippedDstRectPxf(dstRectanglePxf);
    NativeQuadTextureCoords clippedSrcArea(srcArea);
    if (Batch2DUtil::Clip(clippedDstRectPxf, clippedSrcArea, clipRectPx))
    {
      m_batchStrategy.EnsureCapacityFor(1);
      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, color.ToVector4());
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const NativeQuadTextureCoords& srcArea,
                                                                 const PxAreaRectangleF& dstRectanglePxf, const Vector4& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid())
    {
      return;
    }

    PxAreaRectangleF clippedDstRectPxf(dstRectanglePxf);
    NativeQuadTextureCoords clippedSrcArea(srcArea);
    if (Batch2DUtil::Clip(clippedDstRectPxf, clippedSrcArea, clipRectPx))
    {
      m_batchStrategy.EnsureCapacityFor(1);
      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, color);
    }
  }

  // ---------- 1

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color)
  {
    Vector2 dst(dstPositionPxf.X + srcTexture.Info.TrimMarginPx.Left, dstPositionPxf.Y + srcTexture.Info.TrimMarginPx.Top);
    Draw(srcTexture.Texture, dst, srcTexture.Info.TrimmedRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const Color& color)
  {
    if (dstRectanglePx.Width() > 0 && dstRectanglePx.Height() > 0)
    {
      Vector2 scale(dstRectanglePx.Width() / float(srcTexture.Info.ExtentPx.Width), dstRectanglePx.Height() / float(srcTexture.Info.ExtentPx.Height));
      Draw(srcTexture, Vector2(dstRectanglePx.X(), dstRectanglePx.Y()), color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const Color& color)
  {
    Draw(srcTexture, dstRectanglePx, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color)
  {
    if (dstRectanglePxf.Width() > 0 && dstRectanglePxf.Height() > 0)
    {
      Vector2 scale(dstRectanglePxf.Width() / srcTexture.Info.ExtentPx.Width, dstRectanglePxf.Height() / srcTexture.Info.ExtentPx.Height);
      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color)
  {
    Draw(srcTexture, dstRectanglePxf, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color, const BatchEffect effect)
  {
    if (dstRectanglePxf.Width() > 0 && dstRectanglePxf.Height() > 0)
    {
      auto scale = (effect & (BatchEffect::Rotate90Clockwise | BatchEffect::Rotate270Clockwise)) == BatchEffect::NoEffect
                     ? Vector2(dstRectanglePxf.Width() / srcTexture.Info.ExtentPx.Width, dstRectanglePxf.Height() / srcTexture.Info.ExtentPx.Height)
                     : Vector2(dstRectanglePxf.Width() / srcTexture.Info.ExtentPx.Height, dstRectanglePxf.Height() / srcTexture.Info.ExtentPx.Width);

      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), color, Vector2(), scale, effect);
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color, const BatchEffect effect)
  {
    Draw(srcTexture, dstRectanglePxf, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color, effect);
  }

  // ---------- 2

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color)
  {
    Vector2 origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, origin))
    {
      return;
    }

    Vector2 dst(dstPositionPxf.X - origin.X, dstPositionPxf.Y - origin.Y);
    Draw(srcTexture.Texture, dst, srcRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (!srcTexture.IsValid() || clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRectPx.Left() / srcWidth;
    const float u2 = clippedSrcRectPx.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + clippedSrcRectPx.Width, dstPositionPxf.Y),
                            Vector2(dstPositionPxf.X, dstPositionPxf.Y + clippedSrcRectPx.Height),
                            Vector2(dstPositionPxf.X + clippedSrcRectPx.Width, dstPositionPxf.Y + clippedSrcRectPx.Height), Vector2(u1, v1),
                            Vector2(u2, v2), color.ToVector4());
  }

  // ---------- 2 with clip

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    Vector2 origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, origin))
    {
      return;
    }

    Vector2 dst(dstPositionPxf.X - origin.X, dstPositionPxf.Y - origin.Y);
    Draw(srcTexture.Texture, dst, srcRectPx, color, clipRectPx);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (!srcTexture.IsValid() || clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    PxAreaRectangleF dstRectPxf(dstPositionPxf.X, dstPositionPxf.Y, static_cast<float>(clippedSrcRectPx.Width),
                                static_cast<float>(clippedSrcRectPx.Height));
    auto finalClippedSrcRectPxf = TypeConverter::UncheckedTo<PxAreaRectangleF>(clippedSrcRectPx);
    if (Batch2DUtil::Clip(dstRectPxf, finalClippedSrcRectPxf, clipRectPx))
    {
      m_batchStrategy.EnsureCapacityFor(1);

      // Basic quad vertex format
      // 0-1
      // | |
      // 2-3
      const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
      const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
      const float u1 = finalClippedSrcRectPxf.Left() / srcWidth;
      const float u2 = finalClippedSrcRectPxf.Right() / srcWidth;
      const float v1 = TVFormatter::Format(finalClippedSrcRectPxf.Top() / srcHeight);
      const float v2 = TVFormatter::Format(finalClippedSrcRectPxf.Bottom() / srcHeight);

      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                              Vector2(u2, v2), color.ToVector4());
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const BatchEffect effect)
  {
    Vector2 origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, origin))
    {
      return;
    }

    Vector2 dst(dstPositionPxf.X - origin.X, dstPositionPxf.Y - origin.Y);
    Draw(srcTexture.Texture, dst, srcRectPx, color, effect);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const BatchEffect effect)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (!srcTexture.IsValid() || clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    //  u1 u2
    // v 0--1    None     = 0=u1,v1 1=u2,v1 2=u1,v2 3=u2,v2
    // 1 |  |    Rot90CW  = 0=u1,v2 1=u1,v1 2=u2,v2 3=u2,v1
    // v |  |    Rot180CW = 0=u2,v2 1=u1,v2 2=u2,v1 3=u1,v1
    // 2 2--3    Rot270CW = 0=u2,v1 1=u2,v2 2=u1,v1 3=u1,v2
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    float u1 = clippedSrcRectPx.Left() / srcWidth;
    float u2 = clippedSrcRectPx.Right() / srcWidth;
    float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);

    if (BatchEffectUtil::IsFlagged(effect, BatchEffect::FlipHorizontal))
    {
      std::swap(u1, u2);
    }
    if (BatchEffectUtil::IsFlagged(effect, BatchEffect::FlipVertical))
    {
      std::swap(v1, v2);
    }
    switch (effect & BatchEffect::RotateMask)
    {
    case BatchEffect::Rotate90Clockwise:
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + clippedSrcRectPx.Height, dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + clippedSrcRectPx.Width),
                              Vector2(dstPositionPxf.X + clippedSrcRectPx.Height, dstPositionPxf.Y + clippedSrcRectPx.Width),
                              NativeQuadTextureCoords(u1, v2, u1, v1, u2, v2, u2, v1), color.ToVector4());
      break;
    case BatchEffect::Rotate180Clockwise:
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + clippedSrcRectPx.Width, dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + clippedSrcRectPx.Height),
                              Vector2(dstPositionPxf.X + clippedSrcRectPx.Width, dstPositionPxf.Y + clippedSrcRectPx.Height), Vector2(u2, v2),
                              Vector2(u1, v1), color.ToVector4());
      break;
    case BatchEffect::Rotate270Clockwise:
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + clippedSrcRectPx.Height, dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + clippedSrcRectPx.Width),
                              Vector2(dstPositionPxf.X + clippedSrcRectPx.Height, dstPositionPxf.Y + clippedSrcRectPx.Width),
                              NativeQuadTextureCoords(u2, v1, u2, v2, u1, v1, u1, v2), color.ToVector4());
      break;
    case BatchEffect::NoEffect:
    default:
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + clippedSrcRectPx.Width, dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + clippedSrcRectPx.Height),
                              Vector2(dstPositionPxf.X + clippedSrcRectPx.Width, dstPositionPxf.Y + clippedSrcRectPx.Height), Vector2(u1, v1),
                              Vector2(u2, v2), color.ToVector4());
      break;
    }
  }


  // ---------- 3

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color)
  {
    if (dstRectanglePx.Width() > 0 && dstRectanglePx.Height() > 0)
    {
      Vector2 scale(dstRectanglePx.Width() / float(srcRectanglePx.Width), dstRectanglePx.Height() / float(srcRectanglePx.Height));
      Draw(srcTexture, Vector2(dstRectanglePx.X(), dstRectanglePx.Y()), srcRectanglePx, color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePx.Width() <= 0 || dstRectanglePx.Height() <= 0)
    {
      return;
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRectPx.Left() / srcWidth;
    const float u2 = clippedSrcRectPx.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePx.Left()), static_cast<float>(dstRectanglePx.Top())),
                            Vector2(static_cast<float>(dstRectanglePx.Right()), static_cast<float>(dstRectanglePx.Top())),
                            Vector2(static_cast<float>(dstRectanglePx.Left()), static_cast<float>(dstRectanglePx.Bottom())),
                            Vector2(static_cast<float>(dstRectanglePx.Right()), static_cast<float>(dstRectanglePx.Bottom())), Vector2(u1, v1),
                            Vector2(u2, v2), color.ToVector4());
  }

  // ---------- 4

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color)
  {
    if (dstRectanglePxf.Width() > 0 && dstRectanglePxf.Height() > 0)
    {
      Vector2 scale(dstRectanglePxf.Width() / srcRectanglePx.Width, dstRectanglePxf.Height() / srcRectanglePx.Height);
      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), srcRectanglePx, color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePxf.Width() <= 0 || dstRectanglePxf.Height() <= 0)
    {
      return;
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRectPx.Left() / srcWidth;
    const float u2 = clippedSrcRectPx.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Top())),
                            Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Top())),
                            Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Bottom())),
                            Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Bottom())), Vector2(u1, v1),
                            Vector2(u2, v2), color.ToVector4());
  }

  // ---------- 4 with clip


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (dstRectanglePxf.Width() > 0 && dstRectanglePxf.Height() > 0)
    {
      Vector2 scale(dstRectanglePxf.Width() / srcRectanglePx.Width, dstRectanglePxf.Height() / srcRectanglePx.Height);
      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), srcRectanglePx, color, Vector2(), scale, clipRectPx);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePxf.Width() <= 0 || dstRectanglePxf.Height() <= 0)
    {
      return;
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    PxAreaRectangleF dstRectPxf(dstRectanglePxf);
    auto finalClippedSrcRectPxf = TypeConverter::UncheckedTo<PxAreaRectangleF>(clippedSrcRectPx);
    if (Batch2DUtil::Clip(dstRectPxf, finalClippedSrcRectPxf, clipRectPx))
    {
      const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
      const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
      const float u1 = finalClippedSrcRectPxf.Left() / srcWidth;
      const float u2 = finalClippedSrcRectPxf.Right() / srcWidth;
      const float v1 = TVFormatter::Format(finalClippedSrcRectPxf.Top() / srcHeight);
      const float v2 = TVFormatter::Format(finalClippedSrcRectPxf.Bottom() / srcHeight);

      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                              Vector2(u2, v2), color.ToVector4());
    }
  }


  // ---------- 4A

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const BatchEffect effect)
  {
    if (dstRectanglePxf.Width() > 0 && dstRectanglePxf.Height() > 0)
    {
      auto scale = (effect & (BatchEffect::Rotate90Clockwise | BatchEffect::Rotate270Clockwise)) == BatchEffect::NoEffect
                     ? Vector2(dstRectanglePxf.Width() / srcRectanglePx.Width, dstRectanglePxf.Height() / srcRectanglePx.Height)
                     : Vector2(dstRectanglePxf.Width() / srcRectanglePx.Height, dstRectanglePxf.Height() / srcRectanglePx.Width);

      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), srcRectanglePx, color, Vector2(), scale, effect);
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const BatchEffect effect)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePxf.Width() <= 0 || dstRectanglePxf.Height() <= 0)
    {
      return;
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    float u1 = clippedSrcRectPx.Left() / srcWidth;
    float u2 = clippedSrcRectPx.Right() / srcWidth;
    float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);

    if (BatchEffectUtil::IsFlagged(effect, BatchEffect::FlipHorizontal))
    {
      std::swap(u1, u2);
    }
    if (BatchEffectUtil::IsFlagged(effect, BatchEffect::FlipVertical))
    {
      std::swap(v1, v2);
    }

    switch (effect & BatchEffect::RotateMask)
    {
    case BatchEffect::Rotate90Clockwise:
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Bottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Bottom())),
                              NativeQuadTextureCoords(u1, v2, u1, v1, u2, v2, u2, v1), color.ToVector4());
      break;
    case BatchEffect::Rotate180Clockwise:
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Bottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Bottom())), Vector2(u2, v2),
                              Vector2(u1, v1), color.ToVector4());
      break;
    case BatchEffect::Rotate270Clockwise:
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Bottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Bottom())),
                              NativeQuadTextureCoords(u2, v1, u2, v2, u1, v1, u1, v2), color.ToVector4());
      break;
    case BatchEffect::NoEffect:
    default:
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Top())),
                              Vector2(static_cast<float>(dstRectanglePxf.Left()), static_cast<float>(dstRectanglePxf.Bottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.Right()), static_cast<float>(dstRectanglePxf.Bottom())), Vector2(u1, v1),
                              Vector2(u2, v2), color.ToVector4());
      break;
    }
  }

  // ---------- 5

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color, const Vector2& origin, const Vector2& scale)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMarginPx.Left, origin.Y - srcTexture.Info.TrimMarginPx.Top);
    Draw(srcTexture.Texture, dstPositionPxf, srcTexture.Info.TrimmedRectPx, color, originMod, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color,
                                                                 const Vector2& origin, const Vector2& scale)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color, origin, scale);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color, const Vector2& origin, const Vector2& scale,
                                                                 const BatchEffect effect)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMarginPx.Left, origin.Y - srcTexture.Info.TrimMarginPx.Top);
    Draw(srcTexture.Texture, dstPositionPxf, srcTexture.Info.TrimmedRectPx, color, originMod, scale, effect);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color,
                                                                 const Vector2& origin, const Vector2& scale, const BatchEffect effect)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color, origin, scale, effect);
  }

  // ---------- 6


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color, const float rotation, const Vector2& origin,
                                                                 const Vector2& scale)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMarginPx.Left, origin.Y - srcTexture.Info.TrimMarginPx.Top);
    Draw(srcTexture.Texture, dstPositionPxf, srcTexture.Info.TrimmedRectPx, color, rotation, originMod, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color,
                                                                 const float rotation, const Vector2& origin, const Vector2& scale)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color, rotation, origin, scale);
  }

  // ---------- 7

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const Vector2& origin,
                                                                 const Vector2& scale)
  {
    Vector2 originMod = origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, originMod))
    {
      return;
    }

    Draw(srcTexture.Texture, dstPositionPxf, srcRectPx, color, originMod, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const Vector2& origin,
                                                                 const Vector2& scale)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || scale.X <= 0.0f || scale.Y <= 0.0f)
    {
      return;
    }

    const Vector2 dstPos(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);
    const auto clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const float scaledDstWidth = clippedSrcRectPx.Width * scale.X;
    const float scaledDstHeight = clippedSrcRectPx.Height * scale.Y;
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRectPx.Left() / srcWidth;
    const float u2 = clippedSrcRectPx.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                            Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u1, v1), Vector2(u2, v2), color.ToVector4());
  }

  // ---------- 7 with clip

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const Vector2& origin,
                                                                 const Vector2& scale, const PxClipRectangle& clipRectPx)
  {
    Vector2 originMod = origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, originMod))
    {
      return;
    }

    Draw(srcTexture.Texture, dstPositionPxf, srcRectPx, color, originMod, scale, clipRectPx);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const Vector2& origin,
                                                                 const Vector2& scale, const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || scale.X <= 0.0f || scale.Y <= 0.0f)
    {
      return;
    }

    const Vector2 dstPos(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);
    const auto clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    PxAreaRectangleF dstRectPxf(dstPos.X, dstPos.Y, clippedSrcRectPx.Width * scale.X, clippedSrcRectPx.Height * scale.Y);
    auto finalClippedSrcRectPxf = TypeConverter::UncheckedTo<PxAreaRectangleF>(clippedSrcRectPx);
    if (Batch2DUtil::Clip(dstRectPxf, finalClippedSrcRectPxf, clipRectPx))
    {
      // Ensure that we have enough room for the quad
      m_batchStrategy.EnsureCapacityFor(1);

      // Basic quad vertex format
      // 0-1
      // | |
      // 2-3

      const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
      const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
      const float u1 = finalClippedSrcRectPxf.Left() / srcWidth;
      const float u2 = finalClippedSrcRectPxf.Right() / srcWidth;
      const float v1 = TVFormatter::Format(finalClippedSrcRectPxf.Top() / srcHeight);
      const float v2 = TVFormatter::Format(finalClippedSrcRectPxf.Bottom() / srcHeight);

      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                              Vector2(u2, v2), color.ToVector4());
    }
  }

  // ---------- 7a

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const Vector2& origin,
                                                                 const Vector2& scale, const BatchEffect effect)
  {
    Vector2 originMod = origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, originMod))
    {
      return;
    }

    Draw(srcTexture.Texture, dstPositionPxf, srcRectPx, color, originMod, scale, effect);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const Vector2& origin,
                                                                 const Vector2& scale, const BatchEffect effect)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || scale.X <= 0.0f || scale.Y <= 0.0f)
    {
      return;
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);
    const auto clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    float u1 = clippedSrcRectPx.Left() / srcWidth;
    float u2 = clippedSrcRectPx.Right() / srcWidth;
    float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);

    if (BatchEffectUtil::IsFlagged(effect, BatchEffect::FlipHorizontal))
    {
      std::swap(u1, u2);
    }
    if (BatchEffectUtil::IsFlagged(effect, BatchEffect::FlipVertical))
    {
      std::swap(v1, v2);
    }
    switch (effect & BatchEffect::RotateMask)
    {
    case BatchEffect::Rotate90Clockwise:
    {
      const float scaledDstWidth = clippedSrcRectPx.Width * scale.Y;
      const float scaledDstHeight = clippedSrcRectPx.Height * scale.X;
      const Vector2 dstPos(dstPositionPxf.X - (origin.Y * scale.X), dstPositionPxf.Y - (origin.X * scale.Y));

      m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstHeight, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstWidth),
                              Vector2(dstPos.X + scaledDstHeight, dstPos.Y + scaledDstWidth), NativeQuadTextureCoords(u1, v2, u1, v1, u2, v2, u2, v1),
                              color.ToVector4());
      break;
    }
    case BatchEffect::Rotate180Clockwise:
    {
      const float scaledDstWidth = clippedSrcRectPx.Width * scale.X;
      const float scaledDstHeight = clippedSrcRectPx.Height * scale.Y;
      const Vector2 dstPos(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

      m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                              Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u2, v2), Vector2(u1, v1), color.ToVector4());
      break;
    }
    case BatchEffect::Rotate270Clockwise:
    {
      const float scaledDstWidth = clippedSrcRectPx.Width * scale.Y;
      const float scaledDstHeight = clippedSrcRectPx.Height * scale.X;
      const Vector2 dstPos(dstPositionPxf.X - (origin.Y * scale.X), dstPositionPxf.Y - (origin.X * scale.Y));

      m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstHeight, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstWidth),
                              Vector2(dstPos.X + scaledDstHeight, dstPos.Y + scaledDstWidth), NativeQuadTextureCoords(u2, v1, u2, v2, u1, v1, u1, v2),
                              color.ToVector4());
      break;
    }
    case BatchEffect::NoEffect:
    default:
    {
      const float scaledDstWidth = clippedSrcRectPx.Width * scale.X;
      const float scaledDstHeight = clippedSrcRectPx.Height * scale.Y;
      const Vector2 dstPos(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

      m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                              Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u1, v1), Vector2(u2, v2), color.ToVector4());
      break;
    }
    }
  }


  // ---------- 8

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const float rotation,
                                                                 const Vector2& origin, const Vector2& scale)
  {
    Vector2 originMod = origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, originMod))
    {
      return;
    }

    Draw(srcTexture.Texture, dstPositionPxf, srcRectPx, color, rotation, originMod, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU& srcRectanglePx, const Color& color, const float rotation,
                                                                 const Vector2& origin, const Vector2& scale)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || scale.X <= 0.0f || scale.Y <= 0.0f)
    {
      return;
    }

    if (rotation == 0.0f)
    {
      Draw(srcTexture, dstPositionPxf, srcRectanglePx, color, origin, scale);
      return;
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const Vector2 scaledOrigin(origin.X * scale.X, origin.Y * scale.Y);

    Vector2 vec0;
    Vector2 vec1;
    Vector2 vec2;
    Vector2 vec3;
    {    // rotate
      const float dx = dstPositionPxf.X;
      const float dy = dstPositionPxf.Y;
      const float left = -scaledOrigin.X;
      const float right = left + (clippedSrcRectPx.Width * scale.X);
      const float top = -scaledOrigin.Y;
      const float bottom = top + (clippedSrcRectPx.Height * scale.Y);
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

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRectPx.Left() / srcWidth;
    const float u2 = clippedSrcRectPx.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(vec0, vec1, vec2, vec3, Vector2(u1, v1), Vector2(u2, v2), color.ToVector4());
  }

  // ---------- 9

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2* const pDstPositions,
                                                                 const uint32_t dstPositionsLength, const Color& color)
  {
    if (pDstPositions == nullptr)
    {
      throw std::invalid_argument("pDstPositions is invalid");
    }
    EnsurePosScratchpadCapacity(dstPositionsLength);

    // adjust all the dst coordinates
    const auto offsetX = static_cast<float>(srcTexture.Info.TrimMarginPx.Left);
    const auto offsetY = static_cast<float>(srcTexture.Info.TrimMarginPx.Top);
    for (uint32_t i = 0; i < dstPositionsLength; ++i)
    {
      m_posScratchpad[i] = Vector2(pDstPositions[i].X + offsetX, pDstPositions[i].Y + offsetY);
    }

    Draw(srcTexture.Texture, m_posScratchpad.data(), dstPositionsLength, srcTexture.Info.TrimmedRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2* const pDstPositions,
                                                                 const uint32_t dstPositionsLength, const Color& color)
  {
    Draw(srcTexture, pDstPositions, dstPositionsLength, PxRectangleU(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }

  // ---------- 10

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2* const pDstPositions,
                                                                 const uint32_t dstPositionsLength, const PxRectangleU& srcRectanglePx,
                                                                 const Color& color)
  {
    if (pDstPositions == nullptr)
    {
      throw std::invalid_argument("pDstPositions or dstPositions length is invalid");
    }

    Vector2 origin;
    auto srcRectPx = srcRectanglePx;
    if (!AtlasTextureInfoUtil::AdjustSourceRect(srcRectPx, srcTexture.Info, origin))
    {
      return;
    }

    EnsurePosScratchpadCapacity(dstPositionsLength);

    // adjust all the dst coordinates
    for (uint32_t i = 0; i < dstPositionsLength; ++i)
    {
      m_posScratchpad[i] = Vector2(pDstPositions[i].X - origin.X, pDstPositions[i].Y - origin.Y);
    }

    Draw(srcTexture.Texture, m_posScratchpad.data(), dstPositionsLength, srcRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2* const pDstPositions,
                                                                 const uint32_t dstPositionsLength, const PxRectangleU& srcRectanglePx,
                                                                 const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }
    if (!srcTexture.IsValid() || pDstPositions == nullptr || dstPositionsLength <= 0)
    {
      FSLLOG3_WARNING_IF(pDstPositions == nullptr, "It's invalid to specify null for positions");
      FSLLOG3_WARNING_IF(dstPositionsLength < 0, "Its invalid to specify a negative length");
      return;
    }

    const PxRectangleU srcTextureRectanglePx(0, 0, srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU clippedSrcRectPx = PxRectangleU::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width <= 0 || clippedSrcRectPx.Height <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quads
    m_batchStrategy.EnsureCapacityFor(dstPositionsLength);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);
    const float u1 = clippedSrcRectPx.Left() / srcWidth;
    const float u2 = clippedSrcRectPx.Right() / srcWidth;
    const float v1 = TVFormatter::Format(clippedSrcRectPx.Top() / srcHeight);
    const float v2 = TVFormatter::Format(clippedSrcRectPx.Bottom() / srcHeight);

    const Vector2* pDstPosition = pDstPositions;
    const Vector2* const pDstPositionEnd = pDstPositions + dstPositionsLength;
    const Vector4 col = color.ToVector4();

    m_batchStrategy.SetTexture(srcTexture);
    while (pDstPosition < pDstPositionEnd)
    {
      m_batchStrategy.AddQuad(*pDstPosition, Vector2(pDstPosition->X + clippedSrcRectPx.Width, pDstPosition->Y),
                              Vector2(pDstPosition->X, pDstPosition->Y + clippedSrcRectPx.Height),
                              Vector2(pDstPosition->X + clippedSrcRectPx.Width, pDstPosition->Y + clippedSrcRectPx.Height), Vector2(u1, v1),
                              Vector2(u2, v2), col);
      ++pDstPosition;
    }
  }

  // ---------- 11

  // DrawString impl1
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const StringViewLite& strView, const Vector2& dstPositionPxf,
                                                                       const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }
    if (!srcTexture.IsValid() || strView.empty())
    {
      return;
    }

    // Ensure we have enough room for our quads
    m_batchStrategy.EnsureCapacityFor(strView.size());

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < strView.size())
    {
      m_glyphScratchpad.resize(strView.size());
    }

    // Extract the render rules
    if (!font.ExtractRenderRules(m_glyphScratchpad, strView))
    {
      return;
    }

    const Vector4 col = color.ToVector4();
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& glyph = m_glyphScratchpad[i];
      if (glyph.SrcRectPx.Width > 0u)
      {
        auto dstPos = TypeConverter::UncheckedTo<Vector2>(glyph.DstRectPx.Offset) + dstPositionPxf;

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3

        const float u1 = glyph.SrcRectPx.Left() / srcWidth;
        const float u2 = glyph.SrcRectPx.Right() / srcWidth;
        const float v1 = TVFormatter::Format(glyph.SrcRectPx.Top() / srcHeight);
        const float v2 = TVFormatter::Format(glyph.SrcRectPx.Bottom() / srcHeight);

        m_batchStrategy.AddQuad(
          dstPos, Vector2(dstPos.X + glyph.DstRectPx.Extent.Width, dstPos.Y), Vector2(dstPos.X, dstPos.Y + glyph.DstRectPx.Extent.Height),
          Vector2(dstPos.X + glyph.DstRectPx.Extent.Width, dstPos.Y + glyph.DstRectPx.Extent.Height), Vector2(u1, v1), Vector2(u2, v2), col);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const char* const psz, const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, StringViewLite(psz), dstPositionPxf, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const std::string& str, const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color);
  }

  // ---------- 12


  // DrawString impl2
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const StringViewLite& strView, const Vector2& dstPositionPxf,
                                                                       const Color& color, const Vector2& origin, const Vector2& scale)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || strView.empty() || scale.X <= 0.0f || scale.Y <= 0.0f)
    {
      return;
    }

    // Ensure we have enough room for our quads
    m_batchStrategy.EnsureCapacityFor(strView.size());

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < strView.size())
    {
      m_glyphScratchpad.resize(strView.size());
    }

    // Extract the render rules
    if (!font.ExtractRenderRules(m_glyphScratchpad, strView))
    {
      return;
    }

    const Vector2 dstPosModded(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));
    const Vector4 col = color.ToVector4();
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& glyph = m_glyphScratchpad[i];
      if (glyph.SrcRectPx.Width > 0)
      {
        const Vector2 dstPos((static_cast<float>(glyph.DstRectPx.Offset.X) * scale.X) + dstPosModded.X,
                             (static_cast<float>(glyph.DstRectPx.Offset.Y) * scale.Y) + dstPosModded.Y);

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3
        const float scaledDstWidth = static_cast<float>(glyph.DstRectPx.Extent.Width) * scale.X;
        const float scaledDstHeight = static_cast<float>(glyph.DstRectPx.Extent.Height) * scale.Y;
        const float u1 = glyph.SrcRectPx.Left() / srcWidth;
        const float u2 = glyph.SrcRectPx.Right() / srcWidth;
        const float v1 = TVFormatter::Format(glyph.SrcRectPx.Top() / srcHeight);
        const float v2 = TVFormatter::Format(glyph.SrcRectPx.Bottom() / srcHeight);

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                                Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u1, v1), Vector2(u2, v2), col);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const char* const psz, const Vector2& dstPositionPxf, const Color& color,
                                                                       const Vector2& origin, const Vector2& scale)
  {
    DrawString(srcTexture, font, StringViewLite(psz), dstPositionPxf, color, origin, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const std::string& str, const Vector2& dstPositionPxf, const Color& color,
                                                                       const Vector2& origin, const Vector2& scale)
  {
    DrawString(srcTexture, font, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color, origin, scale);
  }

  // ---------- 13

  // DrawString impl3
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const StringViewLite& strView,
                                                                       const Vector2& dstPositionPxf, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }
    if (!srcTexture.IsValid() || strView.empty())
    {
      return;
    }

    // Ensure we have enough room for our quads
    m_batchStrategy.EnsureCapacityFor(strView.size());

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < strView.size())
    {
      m_glyphScratchpad.resize(strView.size());
    }

    // Extract the render rules
    if (!font.ExtractRenderRules(m_glyphScratchpad, strView, fontConfig))
    {
      return;
    }

    const Vector4 col = color.ToVector4();
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);

    if (m_batchStrategy.GetActiveBlendState() == BlendState::Sdf)
    {
      m_batchStrategy.SetBatchSdfRenderConfig(ToBatchSdfRenderConfig(font, fontConfig));
    }

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const FontGlyphPosition& srcGlyph = m_glyphScratchpad[i];
      if (srcGlyph.SrcRectPx.Width > 0)
      {
        const Vector2 dstPosPxf = TypeConverter::UncheckedTo<Vector2>(srcGlyph.DstRectPx.Offset) + dstPositionPxf;

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3

        const auto scaledDstWidth = static_cast<float>(srcGlyph.DstRectPx.Extent.Width);
        const auto scaledDstHeight = static_cast<float>(srcGlyph.DstRectPx.Extent.Height);

        const float u1 = srcGlyph.SrcRectPx.Left() / srcWidth;
        const float u2 = srcGlyph.SrcRectPx.Right() / srcWidth;
        const float v1 = TVFormatter::Format(srcGlyph.SrcRectPx.Top() / srcHeight);
        const float v2 = TVFormatter::Format(srcGlyph.SrcRectPx.Bottom() / srcHeight);

        m_batchStrategy.AddQuad(dstPosPxf, Vector2(dstPosPxf.X + scaledDstWidth, dstPosPxf.Y), Vector2(dstPosPxf.X, dstPosPxf.Y + scaledDstHeight),
                                Vector2(dstPosPxf.X + scaledDstWidth, dstPosPxf.Y + scaledDstHeight), Vector2(u1, v1), Vector2(u2, v2), col);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const std::string& str,
                                                                       const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color);
  }

  // ---------- 13 with clip

  // DrawString impl4
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const StringViewLite& strView,
                                                                       const Vector2& dstPositionPxf, const Color& color,
                                                                       const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }
    if (!srcTexture.IsValid() || strView.empty())
    {
      return;
    }

    // Ensure we have enough room for our quads
    m_batchStrategy.EnsureCapacityFor(strView.size());

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < strView.size())
    {
      m_glyphScratchpad.resize(strView.size());
    }

    // Extract the render rules
    if (!font.ExtractRenderRules(m_glyphScratchpad, strView, fontConfig))
    {
      return;
    }

    const Vector4 col = color.ToVector4();
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);

    if (m_batchStrategy.GetActiveBlendState() == BlendState::Sdf)
    {
      m_batchStrategy.SetBatchSdfRenderConfig(ToBatchSdfRenderConfig(font, fontConfig));
    }

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const FontGlyphPosition& srcGlyph = m_glyphScratchpad[i];
      if (srcGlyph.SrcRectPx.Width > 0)
      {
        PxAreaRectangleF dstRectPxf(static_cast<float>(srcGlyph.DstRectPx.Offset.X) + dstPositionPxf.X,
                                    static_cast<float>(srcGlyph.DstRectPx.Offset.Y) + dstPositionPxf.Y,
                                    static_cast<float>(srcGlyph.DstRectPx.Extent.Width), static_cast<float>(srcGlyph.DstRectPx.Extent.Height));
        auto srcRectPxf = TypeConverter::UncheckedTo<PxAreaRectangleF>(srcGlyph.SrcRectPx);
        if (Batch2DUtil::Clip(dstRectPxf, srcRectPxf, clipRectPx))
        {
          const float u1 = srcRectPxf.Left() / srcWidth;
          const float u2 = srcRectPxf.Right() / srcWidth;
          const float v1 = TVFormatter::Format(srcRectPxf.Top() / srcHeight);
          const float v2 = TVFormatter::Format(srcRectPxf.Bottom() / srcHeight);

          // Basic quad vertex format
          // 0-1
          // | |
          // 2-3
          m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                                  Vector2(u2, v2), col);
        }
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color,
                                                                       const PxClipRectangle& clipRectPx)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color, clipRectPx);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const std::string& str,
                                                                       const Vector2& dstPositionPxf, const Color& color,
                                                                       const PxClipRectangle& clipRectPx)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color, clipRectPx);
  }


  // ---------- 14

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const StringViewLite& strView,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || strView.empty() || scale.X <= 0.0f || scale.Y <= 0.0f)
    {
      return;
    }

    // Ensure we have enough room for our quads
    m_batchStrategy.EnsureCapacityFor(strView.size());

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < strView.size())
    {
      m_glyphScratchpad.resize(strView.size());
    }

    // Extract the render rules
    if (!font.ExtractRenderRules(m_glyphScratchpad, strView, fontConfig))
    {
      return;
    }

    const Vector2 dstPosModded(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));
    const Vector4 col = color.ToVector4();
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& glyph = m_glyphScratchpad[i];
      if (glyph.SrcRectPx.Width > 0)
      {
        const Vector2 dstPos((static_cast<float>(glyph.DstRectPx.Offset.X) * scale.X) + dstPosModded.X,
                             (static_cast<float>(glyph.DstRectPx.Offset.Y) * scale.Y) + dstPosModded.Y);

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3
        const float scaledDstWidth = static_cast<float>(glyph.DstRectPx.Extent.Width) * scale.X;
        const float scaledDstHeight = static_cast<float>(glyph.DstRectPx.Extent.Height) * scale.Y;
        const float u1 = glyph.SrcRectPx.Left() / srcWidth;
        const float u2 = glyph.SrcRectPx.Right() / srcWidth;
        const float v1 = TVFormatter::Format(glyph.SrcRectPx.Top() / srcHeight);
        const float v2 = TVFormatter::Format(glyph.SrcRectPx.Bottom() / srcHeight);

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                                Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u1, v1), Vector2(u2, v2), col);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color, origin, scale);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const std::string& str,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color, origin, scale);
  }

  // ---------- 14 with clip

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const StringViewLite& strView,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale, const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || strView.empty() || scale.X <= 0.0f || scale.Y <= 0.0f)
    {
      return;
    }

    // Ensure we have enough room for our quads
    m_batchStrategy.EnsureCapacityFor(strView.size());

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < strView.size())
    {
      m_glyphScratchpad.resize(strView.size());
    }

    // Extract the render rules
    if (!font.ExtractRenderRules(m_glyphScratchpad, strView, fontConfig))
    {
      return;
    }

    const Vector2 dstPosModded(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));
    const Vector4 col = color.ToVector4();
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height);

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& srcGlyph = m_glyphScratchpad[i];
      if (srcGlyph.SrcRectPx.Width > 0)
      {
        PxAreaRectangleF dstRectPxf((static_cast<float>(srcGlyph.DstRectPx.Offset.X) * scale.X) + dstPosModded.X,
                                    (static_cast<float>(srcGlyph.DstRectPx.Offset.Y) * scale.Y) + dstPosModded.Y,
                                    static_cast<float>(srcGlyph.DstRectPx.Extent.Width) * scale.X,
                                    static_cast<float>(srcGlyph.DstRectPx.Extent.Height) * scale.Y);
        auto srcRectPxf = TypeConverter::UncheckedTo<PxAreaRectangleF>(srcGlyph.SrcRectPx);
        if (Batch2DUtil::Clip(dstRectPxf, srcRectPxf, clipRectPx))
        {
          const float u1 = srcRectPxf.Left() / srcWidth;
          const float u2 = srcRectPxf.Right() / srcWidth;
          const float v1 = TVFormatter::Format(srcRectPxf.Top() / srcHeight);
          const float v2 = TVFormatter::Format(srcRectPxf.Bottom() / srcHeight);

          // Basic quad vertex format
          // 0-1
          // | |
          // 2-3
          m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                                  Vector2(u2, v2), col);
        }
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale, const PxClipRectangle& clipRectPx)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color, origin, scale, clipRectPx);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasBitmapFont& font,
                                                                       const BitmapFontConfig& fontConfig, const std::string& str,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale, const PxClipRectangle& clipRectPx)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color, origin, scale, clipRectPx);
  }

  // ----------


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const atlas_texture_type& srcFillTexture,
                                                                               const PxRectangle& dstRectanglePx, const Color& color)
  {
    const auto texSize = srcFillTexture.Info.ExtentPx;
    const PxRectangleU srcRectPx(texSize.Width / 2, texSize.Height / 2, 1, 1);
    PxRectangle finalDstRectanglePx(dstRectanglePx.X(), dstRectanglePx.Y(), dstRectanglePx.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Bottom() - 1);
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Top());
    finalDstRectanglePx.SetWidth(1);
    finalDstRectanglePx.SetHeight(dstRectanglePx.Height());
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetX(dstRectanglePx.Right() - 1);
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const texture_type& srcFillTexture, const PxRectangle& dstRectanglePx,
                                                                               const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const PxRectangleU srcRectPx(texExtent.Width / 2, texExtent.Height / 2, 1, 1);
    PxRectangle finalDstRectanglePx(dstRectanglePx.X(), dstRectanglePx.Y(), dstRectanglePx.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Bottom() - 1);
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Top());
    finalDstRectanglePx.SetWidth(1);
    finalDstRectanglePx.SetHeight(dstRectanglePx.Height());
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetX(dstRectanglePx.Right() - 1);
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const atlas_texture_type& srcFillTexture,
                                                                               const PxAreaRectangleF& dstRectanglePxf, const Color& color)
  {
    const auto texSize = srcFillTexture.Info.ExtentPx;
    const PxRectangleU srcRectPx(texSize.Width / 2, texSize.Height / 2, 1, 1);
    PxAreaRectangleF finalDstRectanglePxf(dstRectanglePxf.Left(), dstRectanglePxf.Top(), dstRectanglePxf.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Bottom() - 1);
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Top());
    finalDstRectanglePxf.SetWidth(1);
    finalDstRectanglePxf.SetHeight(dstRectanglePxf.Height());
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveLeft(dstRectanglePxf.Right() - 1);
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const texture_type& srcFillTexture,
                                                                               const PxAreaRectangleF& dstRectanglePxf, const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const PxRectangleU srcRectPx(texExtent.Width / 2, texExtent.Height / 2, 1, 1);
    PxAreaRectangleF finalDstRectanglePxf(dstRectanglePxf.Left(), dstRectanglePxf.Top(), dstRectanglePxf.Width(), 1);

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Bottom() - 1);
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Top());
    finalDstRectanglePxf.SetWidth(1);
    finalDstRectanglePxf.SetHeight(dstRectanglePxf.Height());
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveLeft(dstRectanglePxf.Right() - 1);
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const atlas_texture_type& srcFillTexture, const Vector2& dstFromPxf,
                                                                          const Vector2& dstToPxf, const Color& color)
  {
    const auto texSize = srcFillTexture.Info.ExtentPx;
    const PxRectangleU srcRectPx(texSize.Width / 2, texSize.Height / 2, 1, 1);

    Vector2 delta(dstToPxf.X - dstFromPxf.X, dstToPxf.Y - dstFromPxf.Y);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, dstFromPxf, srcRectPx, color, rotation, Vector2(), scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const texture_type& srcFillTexture, const Vector2& dstFromPxf,
                                                                          const Vector2& dstToPxf, const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const PxRectangleU srcRectPx(texExtent.Width / 2, texExtent.Height / 2, 1, 1);

    Vector2 delta(dstToPxf.X - dstFromPxf.X, dstToPxf.Y - dstFromPxf.Y);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, dstFromPxf, srcRectPx, color, rotation, Vector2(), scale);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  Batch2DStats GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::GetStats() const
  {
    return {m_stats, m_native->GetStats()};
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::FlushQuads()
  {
    const PxSize2D sizePx(m_screenRect.Width(), m_screenRect.Height());

    auto vertexSpan = m_batchStrategy.GetSpan();
    const auto segmentCount = m_batchStrategy.GetSegmentCount();

    if (segmentCount > 0)
    {
      // With the new rendering strategy the Begin/End + 'DrawQuads' render method is not very effective
      const auto& firstSegment = m_batchStrategy.GetSegment(0);
      BlendState activeBlendState = firstSegment.ActiveBlendState;
      auto activeSdfConfig = firstSegment.SdfRenderConfig;
      m_native->Begin(sizePx, activeBlendState, activeSdfConfig, m_restoreState);

      auto* pSrcVertices = vertexSpan.pVertices;
      for (uint32_t i = 0; i < segmentCount; ++i)
      {
        const auto& segment = m_batchStrategy.GetSegment(i);

        if (segment.ActiveBlendState != activeBlendState ||
            (segment.ActiveBlendState == BlendState::Sdf && activeSdfConfig != segment.SdfRenderConfig))
        {
          activeBlendState = segment.ActiveBlendState;
          activeSdfConfig = segment.SdfRenderConfig;
          m_native->End();
          m_native->Begin(sizePx, activeBlendState, activeSdfConfig, m_restoreState);
        }

        m_native->DrawQuads(pSrcVertices, segment.VertexCount / stategy_type::VERTICES_PER_QUAD, segment.TextureInfo);
        pSrcVertices += segment.VertexCount;
      }
      m_native->End();
    }

    m_batchStrategy.Clear();
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::EnsurePosScratchpadCapacity(const uint32_t minCapacity)
  {
    const std::size_t newMinCapacity = minCapacity;
    if (newMinCapacity < m_posScratchpad.size())
    {
      return;
    }
    m_posScratchpad.resize(newMinCapacity + 32);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Rotate2D(Vector2& rPoint0, Vector2& rPoint1, Vector2& rPoint2, Vector2& rPoint3,
                                                                     const float rotation) const
  {
    const float cosR = std::cos(rotation);
    const float sinR = std::sin(rotation);

    rPoint0 = Vector2((rPoint0.X * cosR - rPoint0.Y * sinR), (rPoint0.Y * cosR + rPoint0.X * sinR));
    rPoint1 = Vector2((rPoint1.X * cosR - rPoint1.Y * sinR), (rPoint1.Y * cosR + rPoint1.X * sinR));
    rPoint2 = Vector2((rPoint2.X * cosR - rPoint2.Y * sinR), (rPoint2.Y * cosR + rPoint2.X * sinR));
    rPoint3 = Vector2((rPoint3.X * cosR - rPoint3.Y * sinR), (rPoint3.Y * cosR + rPoint3.X * sinR));
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  inline BatchSdfRenderConfig GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::ToBatchSdfRenderConfig(const TextureAtlasBitmapFont& /*font*/,
                                                                                                          const BitmapFontConfig& fontConfig)
  {
    return BatchSdfRenderConfig(fontConfig.Scale);
  }
}

#endif
