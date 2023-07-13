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
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/VectorHelper.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/BitmapFontConfig.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/Render/Batch2DUtil.hpp>
#include <FslGraphics/Render/GenericBatch2D_fwd.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfoUtil.hpp>
#include <cassert>
#include <cstring>

namespace Fsl
{
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::GenericBatch2D(const native_batch_type& nativeBatchType, const PxExtent2D& currentExtent)
    : m_batchStrategy(GenericBatch2D_DEFAULT_CAPACITY)
    , m_native(nativeBatchType)
    , m_screenRect(PxRectangle::Create(0, 0, UncheckedNumericCast<PxRectangle::raw_size_value_type>(currentExtent.Width.Value),
                                       UncheckedNumericCast<PxRectangle::raw_size_value_type>(currentExtent.Height.Value)))
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
    m_screenRect = PxRectangle::Create(0, 0, UncheckedNumericCast<PxRectangle::raw_size_value_type>(extentPx.Width.Value),
                                       UncheckedNumericCast<PxRectangle::raw_size_value_type>(extentPx.Height.Value));
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
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, color);
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
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, Color(color));
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
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, color);
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
    m_batchStrategy.AddQuad(dstRectanglePxf, srcArea, Color(color));
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
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, color);
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
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, Color(color));
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
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, color);
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
      m_batchStrategy.AddQuad(clippedDstRectPxf, clippedSrcArea, Color(color));
    }
  }

  // ---------- 1

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color)
  {
    Vector2 dst(dstPositionPxf.X + srcTexture.Info.TrimMarginPx.Left.Value, dstPositionPxf.Y + srcTexture.Info.TrimMarginPx.Top.Value);
    Draw(srcTexture.Texture, dst, srcTexture.Info.TrimmedRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU32(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const Color& color)
  {
    if (dstRectanglePx.RawWidth() > 0 && dstRectanglePx.RawHeight() > 0)
    {
      Vector2 scale(static_cast<float>(dstRectanglePx.RawWidth()) / float(srcTexture.Info.ExtentPx.Width.Value),
                    static_cast<float>(dstRectanglePx.RawHeight()) / float(srcTexture.Info.ExtentPx.Height.Value));
      Draw(srcTexture, Vector2(dstRectanglePx.X().Value, dstRectanglePx.Y().Value), color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const Color& color)
  {
    Draw(srcTexture, dstRectanglePx, PxRectangleU32(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color)
  {
    if (dstRectanglePxf.RawWidth() > 0 && dstRectanglePxf.RawHeight() > 0)
    {
      Vector2 scale(dstRectanglePxf.RawWidth() / srcTexture.Info.ExtentPx.Width.Value,
                    dstRectanglePxf.RawHeight() / srcTexture.Info.ExtentPx.Height.Value);
      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color)
  {
    Draw(srcTexture, dstRectanglePxf, PxRectangleU32(PxValueU(), PxValueU(), srcTexture.Extent.Width, srcTexture.Extent.Height), color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color, const BatchEffect effect)
  {
    if (dstRectanglePxf.RawWidth() > 0 && dstRectanglePxf.RawHeight() > 0)
    {
      auto scale = (effect & (BatchEffect::Rotate90Clockwise | BatchEffect::Rotate270Clockwise)) == BatchEffect::NoEffect
                     ? Vector2(dstRectanglePxf.RawWidth() / srcTexture.Info.ExtentPx.Width.Value,
                               dstRectanglePxf.RawHeight() / srcTexture.Info.ExtentPx.Height.Value)
                     : Vector2(dstRectanglePxf.RawWidth() / srcTexture.Info.ExtentPx.Height.Value,
                               dstRectanglePxf.RawHeight() / srcTexture.Info.ExtentPx.Width.Value);

      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), color, Vector2(), scale, effect);
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const Color& color, const BatchEffect effect)
  {
    Draw(srcTexture, dstRectanglePxf, PxRectangleU32(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height), color, effect);
  }

  // ---------- 2

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color)
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
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (!srcTexture.IsValid() || clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    const float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    const float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    const float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    const float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Width.Value), dstPositionPxf.Y),
                            Vector2(dstPositionPxf.X, dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                            Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Width.Value),
                                    dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                            Vector2(u1, v1), Vector2(u2, v2), color);
  }

  // ---------- 2 with clip

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color,
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
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (!srcTexture.IsValid() || clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    auto dstRectPxf = PxAreaRectangleF::Create(dstPositionPxf.X, dstPositionPxf.Y, static_cast<float>(clippedSrcRectPx.Width.Value),
                                               static_cast<float>(clippedSrcRectPx.Height.Value));
    auto finalClippedSrcRectPxf = TypeConverter::UncheckedTo<PxAreaRectangleF>(clippedSrcRectPx);
    if (Batch2DUtil::Clip(dstRectPxf, finalClippedSrcRectPxf, clipRectPx))
    {
      m_batchStrategy.EnsureCapacityFor(1);

      // Basic quad vertex format
      // 0-1
      // | |
      // 2-3
      const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
      const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
      const float u1 = finalClippedSrcRectPxf.RawLeft() / srcWidth;
      const float u2 = finalClippedSrcRectPxf.RawRight() / srcWidth;
      const float v1 = TVFormatter::Format(finalClippedSrcRectPxf.RawTop() / srcHeight);
      const float v2 = TVFormatter::Format(finalClippedSrcRectPxf.RawBottom() / srcHeight);

      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                              Vector2(u2, v2), color);
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const BatchEffect effect)
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
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const BatchEffect effect)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (!srcTexture.IsValid() || clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
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
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

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
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Height.Value), dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Width.Value)),
                              Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Height.Value),
                                      dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Width.Value)),
                              NativeQuadTextureCoords(u1, v2, u1, v1, u2, v2, u2, v1), color);
      break;
    case BatchEffect::Rotate180Clockwise:
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Width.Value), dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                              Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Width.Value),
                                      dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                              Vector2(u2, v2), Vector2(u1, v1), color);
      break;
    case BatchEffect::Rotate270Clockwise:
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Height.Value), dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Width.Value)),
                              Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Height.Value),
                                      dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Width.Value)),
                              NativeQuadTextureCoords(u2, v1, u2, v2, u1, v1, u1, v2), color);
      break;
    case BatchEffect::NoEffect:
    default:
      m_batchStrategy.AddQuad(dstPositionPxf, Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Width.Value), dstPositionPxf.Y),
                              Vector2(dstPositionPxf.X, dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                              Vector2(dstPositionPxf.X + static_cast<float>(clippedSrcRectPx.Width.Value),
                                      dstPositionPxf.Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                              Vector2(u1, v1), Vector2(u2, v2), color);
      break;
    }
  }


  // ---------- 3

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color)
  {
    if (dstRectanglePx.RawWidth() > 0 && dstRectanglePx.RawHeight() > 0)
    {
      Vector2 scale(static_cast<float>(dstRectanglePx.RawWidth()) / static_cast<float>(srcRectanglePx.Width.Value),
                    static_cast<float>(dstRectanglePx.RawHeight()) / static_cast<float>(srcRectanglePx.Height.Value));
      Draw(srcTexture, Vector2(dstRectanglePx.X().Value, dstRectanglePx.Y().Value), srcRectanglePx, color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxRectangle& dstRectanglePx,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePx.RawWidth() <= 0 || dstRectanglePx.RawHeight() <= 0)
    {
      return;
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    const float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    const float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    const float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    const float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePx.RawLeft()), static_cast<float>(dstRectanglePx.RawTop())),
                            Vector2(static_cast<float>(dstRectanglePx.RawRight()), static_cast<float>(dstRectanglePx.RawTop())),
                            Vector2(static_cast<float>(dstRectanglePx.RawLeft()), static_cast<float>(dstRectanglePx.RawBottom())),
                            Vector2(static_cast<float>(dstRectanglePx.RawRight()), static_cast<float>(dstRectanglePx.RawBottom())), Vector2(u1, v1),
                            Vector2(u2, v2), color);
  }

  // ---------- 4

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color)
  {
    if (dstRectanglePxf.RawWidth() > 0 && dstRectanglePxf.RawHeight() > 0)
    {
      Vector2 scale(dstRectanglePxf.RawWidth() / static_cast<float>(srcRectanglePx.Width.Value),
                    dstRectanglePxf.RawHeight() / static_cast<float>(srcRectanglePx.Height.Value));
      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), srcRectanglePx, color, Vector2(), scale);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePxf.RawWidth() <= 0 || dstRectanglePxf.RawHeight() <= 0)
    {
      return;
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(), PxValueU(), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    const float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    const float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    const float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    const float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawTop())),
                            Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawTop())),
                            Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawBottom())),
                            Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawBottom())), Vector2(u1, v1),
                            Vector2(u2, v2), color);
  }

  // ---------- 4 with clip


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (dstRectanglePxf.RawWidth() > 0 && dstRectanglePxf.RawHeight() > 0)
    {
      Vector2 scale(dstRectanglePxf.RawWidth() / static_cast<float>(srcRectanglePx.Width.Value),
                    dstRectanglePxf.RawHeight() / static_cast<float>(srcRectanglePx.Height.Value));
      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), srcRectanglePx, color, Vector2(), scale, clipRectPx);
    }
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color,
                                                                 const PxClipRectangle& clipRectPx)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePxf.RawWidth() <= 0 || dstRectanglePxf.RawHeight() <= 0)
    {
      return;
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
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
      const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
      const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
      const float u1 = finalClippedSrcRectPxf.RawLeft() / srcWidth;
      const float u2 = finalClippedSrcRectPxf.RawRight() / srcWidth;
      const float v1 = TVFormatter::Format(finalClippedSrcRectPxf.RawTop() / srcHeight);
      const float v2 = TVFormatter::Format(finalClippedSrcRectPxf.RawBottom() / srcHeight);

      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                              Vector2(u2, v2), color);
    }
  }


  // ---------- 4A

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const BatchEffect effect)
  {
    if (dstRectanglePxf.RawWidth() > 0 && dstRectanglePxf.RawHeight() > 0)
    {
      auto scale = (effect & (BatchEffect::Rotate90Clockwise | BatchEffect::Rotate270Clockwise)) == BatchEffect::NoEffect
                     ? Vector2(dstRectanglePxf.RawWidth() / static_cast<float>(srcRectanglePx.Width.Value),
                               dstRectanglePxf.RawHeight() / static_cast<float>(srcRectanglePx.Height.Value))
                     : Vector2(dstRectanglePxf.RawWidth() / static_cast<float>(srcRectanglePx.Height.Value),
                               dstRectanglePxf.RawHeight() / static_cast<float>(srcRectanglePx.Width.Value));

      Draw(srcTexture, TypeConverter::To<Vector2>(dstRectanglePxf.Location()), srcRectanglePx, color, Vector2(), scale, effect);
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const PxAreaRectangleF& dstRectanglePxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const BatchEffect effect)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }

    if (!srcTexture.IsValid() || dstRectanglePxf.RawWidth() <= 0 || dstRectanglePxf.RawHeight() <= 0)
    {
      return;
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

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
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              NativeQuadTextureCoords(u1, v2, u1, v1, u2, v2, u2, v1), color);
      break;
    case BatchEffect::Rotate180Clockwise:
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              Vector2(u2, v2), Vector2(u1, v1), color);
      break;
    case BatchEffect::Rotate270Clockwise:
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              NativeQuadTextureCoords(u2, v1, u2, v2, u1, v1, u1, v2), color);
      break;
    case BatchEffect::NoEffect:
    default:
      m_batchStrategy.AddQuad(Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawTop())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawLeft()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              Vector2(static_cast<float>(dstRectanglePxf.RawRight()), static_cast<float>(dstRectanglePxf.RawBottom())),
                              Vector2(u1, v1), Vector2(u2, v2), color);
      break;
    }
  }

  // ---------- 5

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color, const Vector2& origin, const Vector2& scale)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMarginPx.Left.Value, origin.Y - srcTexture.Info.TrimMarginPx.Top.Value);
    Draw(srcTexture.Texture, dstPositionPxf, srcTexture.Info.TrimmedRectPx, color, originMod, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color,
                                                                 const Vector2& origin, const Vector2& scale)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU32(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height), color, origin,
         scale);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color, const Vector2& origin, const Vector2& scale,
                                                                 const BatchEffect effect)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMarginPx.Left.Value, origin.Y - srcTexture.Info.TrimMarginPx.Top.Value);
    Draw(srcTexture.Texture, dstPositionPxf, srcTexture.Info.TrimmedRectPx, color, originMod, scale, effect);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color,
                                                                 const Vector2& origin, const Vector2& scale, const BatchEffect effect)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU32(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height), color, origin,
         scale, effect);
  }

  // ---------- 6


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const Color& color, const float rotation, const Vector2& origin,
                                                                 const Vector2& scale)
  {
    Vector2 originMod(origin.X - srcTexture.Info.TrimMarginPx.Left.Value, origin.Y - srcTexture.Info.TrimMarginPx.Top.Value);
    Draw(srcTexture.Texture, dstPositionPxf, srcTexture.Info.TrimmedRectPx, color, rotation, originMod, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const texture_type& srcTexture, const Vector2& dstPositionPxf, const Color& color,
                                                                 const float rotation, const Vector2& origin, const Vector2& scale)
  {
    Draw(srcTexture, dstPositionPxf, PxRectangleU32(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height), color, rotation,
         origin, scale);
  }

  // ---------- 7

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const Vector2& origin,
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
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const Vector2& origin,
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

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);
    const auto clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const float scaledDstWidth = static_cast<float>(clippedSrcRectPx.Width.Value) * scale.X;
    const float scaledDstHeight = static_cast<float>(clippedSrcRectPx.Height.Value) * scale.Y;
    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    const float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    const float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    const float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    const float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                            Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u1, v1), Vector2(u2, v2), color);
  }

  // ---------- 7 with clip

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const Vector2& origin,
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
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const Vector2& origin,
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

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);
    const auto clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    auto dstRectPxf = PxAreaRectangleF::Create(dstPos.X, dstPos.Y, static_cast<float>(clippedSrcRectPx.Width.Value) * scale.X,
                                               static_cast<float>(clippedSrcRectPx.Height.Value) * scale.Y);
    auto finalClippedSrcRectPxf = TypeConverter::UncheckedTo<PxAreaRectangleF>(clippedSrcRectPx);
    if (Batch2DUtil::Clip(dstRectPxf, finalClippedSrcRectPxf, clipRectPx))
    {
      // Ensure that we have enough room for the quad
      m_batchStrategy.EnsureCapacityFor(1);

      // Basic quad vertex format
      // 0-1
      // | |
      // 2-3

      const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
      const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
      const float u1 = finalClippedSrcRectPxf.RawLeft() / srcWidth;
      const float u2 = finalClippedSrcRectPxf.RawRight() / srcWidth;
      const float v1 = TVFormatter::Format(finalClippedSrcRectPxf.RawTop() / srcHeight);
      const float v2 = TVFormatter::Format(finalClippedSrcRectPxf.RawBottom() / srcHeight);

      m_batchStrategy.SetTexture(srcTexture);
      m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(), Vector2(u1, v1),
                              Vector2(u2, v2), color);
    }
  }

  // ---------- 7a

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const Vector2& origin,
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
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const Vector2& origin,
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

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);
    const auto clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quad
    m_batchStrategy.EnsureCapacityFor(1);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

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
        const float scaledDstWidth = static_cast<float>(clippedSrcRectPx.Width.Value) * scale.Y;
        const float scaledDstHeight = static_cast<float>(clippedSrcRectPx.Height.Value) * scale.X;
        const Vector2 dstPos(dstPositionPxf.X - (origin.Y * scale.X), dstPositionPxf.Y - (origin.X * scale.Y));

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstHeight, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstWidth),
                                Vector2(dstPos.X + scaledDstHeight, dstPos.Y + scaledDstWidth),
                                NativeQuadTextureCoords(u1, v2, u1, v1, u2, v2, u2, v1), color);
        break;
      }
    case BatchEffect::Rotate180Clockwise:
      {
        const float scaledDstWidth = static_cast<float>(clippedSrcRectPx.Width.Value) * scale.X;
        const float scaledDstHeight = static_cast<float>(clippedSrcRectPx.Height.Value) * scale.Y;
        const Vector2 dstPos(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                                Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u2, v2), Vector2(u1, v1), color);
        break;
      }
    case BatchEffect::Rotate270Clockwise:
      {
        const float scaledDstWidth = static_cast<float>(clippedSrcRectPx.Width.Value) * scale.Y;
        const float scaledDstHeight = static_cast<float>(clippedSrcRectPx.Height.Value) * scale.X;
        const Vector2 dstPos(dstPositionPxf.X - (origin.Y * scale.X), dstPositionPxf.Y - (origin.X * scale.Y));

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstHeight, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstWidth),
                                Vector2(dstPos.X + scaledDstHeight, dstPos.Y + scaledDstWidth),
                                NativeQuadTextureCoords(u2, v1, u2, v2, u1, v1, u1, v2), color);
        break;
      }
    case BatchEffect::NoEffect:
    default:
      {
        const float scaledDstWidth = static_cast<float>(clippedSrcRectPx.Width.Value) * scale.X;
        const float scaledDstHeight = static_cast<float>(clippedSrcRectPx.Height.Value) * scale.Y;
        const Vector2 dstPos(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                                Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(u1, v1), Vector2(u2, v2), color);
        break;
      }
    }
  }


  // ---------- 8

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2& dstPositionPxf,
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const float rotation,
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
                                                                 const PxRectangleU32& srcRectanglePx, const Color& color, const float rotation,
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

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
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
      const float right = left + (static_cast<float>(clippedSrcRectPx.Width.Value) * scale.X);
      const float top = -scaledOrigin.Y;
      const float bottom = top + (static_cast<float>(clippedSrcRectPx.Height.Value) * scale.Y);
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

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    const float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    const float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    const float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    const float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

    m_batchStrategy.SetTexture(srcTexture);
    m_batchStrategy.AddQuad(vec0, vec1, vec2, vec3, Vector2(u1, v1), Vector2(u2, v2), color);
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
    const auto offsetX = static_cast<float>(srcTexture.Info.TrimMarginPx.Left.Value);
    const auto offsetY = static_cast<float>(srcTexture.Info.TrimMarginPx.Top.Value);
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
    Draw(srcTexture, pDstPositions, dstPositionsLength, PxRectangleU32(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height),
         color);
  }

  // ---------- 10

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::Draw(const atlas_texture_type& srcTexture, const Vector2* const pDstPositions,
                                                                 const uint32_t dstPositionsLength, const PxRectangleU32& srcRectanglePx,
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
                                                                 const uint32_t dstPositionsLength, const PxRectangleU32& srcRectanglePx,
                                                                 const Color& color)
  {
    if (!m_inBegin)
    {
      throw UsageErrorException("Draw can only occur inside a begin/end block");
    }
    if (!srcTexture.IsValid() || pDstPositions == nullptr || dstPositionsLength <= 0)
    {
      FSLLOG3_WARNING_IF(pDstPositions == nullptr, "It's invalid to specify null for positions");
      // FSLLOG3_WARNING_IF(dstPositionsLength < 0, "Its invalid to specify a negative length");
      return;
    }

    const PxRectangleU32 srcTextureRectanglePx(PxValueU(0), PxValueU(0), srcTexture.Extent.Width, srcTexture.Extent.Height);

    const PxRectangleU32 clippedSrcRectPx = PxRectangleU32::Intersect(srcTextureRectanglePx, srcRectanglePx);
    if (clippedSrcRectPx.Width.Value <= 0 || clippedSrcRectPx.Height.Value <= 0)
    {
      return;
    }

    // Ensure that we have enough room for the quads
    m_batchStrategy.EnsureCapacityFor(dstPositionsLength);

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3

    const auto srcWidth = static_cast<float>(srcTexture.Extent.Width.Value);
    const auto srcHeight = static_cast<float>(srcTexture.Extent.Height.Value);
    const float u1 = static_cast<float>(clippedSrcRectPx.RawLeft()) / srcWidth;
    const float u2 = static_cast<float>(clippedSrcRectPx.RawRight()) / srcWidth;
    const float v1 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawTop()) / srcHeight);
    const float v2 = TVFormatter::Format(static_cast<float>(clippedSrcRectPx.RawBottom()) / srcHeight);

    const Vector2* pDstPosition = pDstPositions;
    const Vector2* const pDstPositionEnd = pDstPositions + dstPositionsLength;

    m_batchStrategy.SetTexture(srcTexture);
    while (pDstPosition < pDstPositionEnd)
    {
      m_batchStrategy.AddQuad(*pDstPosition, Vector2(pDstPosition->X + static_cast<float>(clippedSrcRectPx.Width.Value), pDstPosition->Y),
                              Vector2(pDstPosition->X, pDstPosition->Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                              Vector2(pDstPosition->X + static_cast<float>(clippedSrcRectPx.Width.Value),
                                      pDstPosition->Y + static_cast<float>(clippedSrcRectPx.Height.Value)),
                              Vector2(u1, v1), Vector2(u2, v2), color);
      ++pDstPosition;
    }
  }

  // ---------- 11

  // DrawString impl1
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
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
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!font.ExtractRenderRules(glyphScratchpadSpan, strView))
    {
      return;
    }

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& glyph = glyphScratchpadSpan[i];
      if (glyph.TextureArea.X1 > glyph.TextureArea.X0)
      {
        auto dstPos = TypeConverter::UncheckedTo<Vector2>(glyph.DstRectPxf.Location()) + dstPositionPxf;

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + glyph.DstRectPxf.RawWidth(), dstPos.Y),
                                Vector2(dstPos.X, dstPos.Y + glyph.DstRectPxf.RawHeight()),
                                Vector2(dstPos.X + glyph.DstRectPxf.RawWidth(), dstPos.Y + glyph.DstRectPxf.RawHeight()),
                                Vector2(glyph.TextureArea.X0, glyph.TextureArea.Y0), Vector2(glyph.TextureArea.X1, glyph.TextureArea.Y1), color);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const char* const psz, const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, StringViewLite(psz), dstPositionPxf, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const std::string& str, const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color);
  }

  // ---------- 12


  // DrawString impl2
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
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
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!font.ExtractRenderRules(glyphScratchpadSpan, strView))
    {
      return;
    }

    const Vector2 dstPosModded(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& glyph = glyphScratchpadSpan[i];
      if (glyph.TextureArea.X1 > glyph.TextureArea.X0)
      {
        const Vector2 dstPos((glyph.DstRectPxf.RawLeft() * scale.X) + dstPosModded.X, (glyph.DstRectPxf.RawTop() * scale.Y) + dstPosModded.Y);

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3
        const float scaledDstWidth = glyph.DstRectPxf.RawWidth() * scale.X;
        const float scaledDstHeight = glyph.DstRectPxf.RawHeight() * scale.Y;

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                                Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(glyph.TextureArea.X0, glyph.TextureArea.Y0),
                                Vector2(glyph.TextureArea.X1, glyph.TextureArea.Y1), color);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const char* const psz, const Vector2& dstPositionPxf, const Color& color,
                                                                       const Vector2& origin, const Vector2& scale)
  {
    DrawString(srcTexture, font, StringViewLite(psz), dstPositionPxf, color, origin, scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const std::string& str, const Vector2& dstPositionPxf, const Color& color,
                                                                       const Vector2& origin, const Vector2& scale)
  {
    DrawString(srcTexture, font, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color, origin, scale);
  }

  // ---------- 13

  // DrawString impl3
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
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
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!font.ExtractRenderRules(glyphScratchpadSpan, strView, fontConfig))
    {
      return;
    }

    if (m_batchStrategy.GetActiveBlendState() == BlendState::Sdf)
    {
      m_batchStrategy.SetBatchSdfRenderConfig(ToBatchSdfRenderConfig(font, fontConfig));
    }

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const SpriteFontGlyphPosition& srcGlyph = glyphScratchpadSpan[i];
      if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
      {
        const Vector2 dstPosPxf = TypeConverter::UncheckedTo<Vector2>(srcGlyph.DstRectPxf.Location()) + dstPositionPxf;

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3

        const float scaledDstWidth = srcGlyph.DstRectPxf.RawWidth();
        const float scaledDstHeight = srcGlyph.DstRectPxf.RawHeight();

        m_batchStrategy.AddQuad(dstPosPxf, Vector2(dstPosPxf.X + scaledDstWidth, dstPosPxf.Y), Vector2(dstPosPxf.X, dstPosPxf.Y + scaledDstHeight),
                                Vector2(dstPosPxf.X + scaledDstWidth, dstPosPxf.Y + scaledDstHeight),
                                Vector2(srcGlyph.TextureArea.X0, srcGlyph.TextureArea.Y0), Vector2(srcGlyph.TextureArea.X1, srcGlyph.TextureArea.Y1),
                                color);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const BitmapFontConfig& fontConfig, const std::string& str,
                                                                       const Vector2& dstPositionPxf, const Color& color)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color);
  }

  // ---------- 13 with clip

  // DrawString impl4
  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
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
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!font.ExtractRenderRules(glyphScratchpadSpan, strView, fontConfig))
    {
      return;
    }

    if (m_batchStrategy.GetActiveBlendState() == BlendState::Sdf)
    {
      m_batchStrategy.SetBatchSdfRenderConfig(ToBatchSdfRenderConfig(font, fontConfig));
    }

    m_batchStrategy.SetTexture(srcTexture);
    PxVector2 dstPositionPxf2 = TypeConverter::To<PxVector2>(dstPositionPxf);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const SpriteFontGlyphPosition& srcGlyph = glyphScratchpadSpan[i];
      if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
      {
        PxAreaRectangleF dstRectPxf(PxAreaRectangleF::AddLocation(dstPositionPxf2, srcGlyph.DstRectPxf));
        auto srcTextureArea = srcGlyph.TextureArea;
        if (Batch2DUtil::Clip(dstRectPxf, srcTextureArea, clipRectPx))
        {
          // Basic quad vertex format
          // 0-1
          // | |
          // 2-3
          m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(),
                                  Vector2(srcTextureArea.X0, srcTextureArea.Y0), Vector2(srcTextureArea.X1, srcTextureArea.Y1), color);
        }
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color,
                                                                       const PxClipRectangle& clipRectPx)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color, clipRectPx);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const BitmapFontConfig& fontConfig, const std::string& str,
                                                                       const Vector2& dstPositionPxf, const Color& color,
                                                                       const PxClipRectangle& clipRectPx)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color, clipRectPx);
  }


  // ---------- 14

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
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
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!font.ExtractRenderRules(glyphScratchpadSpan, strView, fontConfig))
    {
      return;
    }

    const Vector2 dstPosModded(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& glyph = glyphScratchpadSpan[i];
      if (glyph.TextureArea.X1 > glyph.TextureArea.X0)
      {
        const Vector2 dstPos((glyph.DstRectPxf.RawLeft() * scale.X) + dstPosModded.X, (glyph.DstRectPxf.RawTop() * scale.Y) + dstPosModded.Y);

        // Basic quad vertex format
        // 0-1
        // | |
        // 2-3
        const float scaledDstWidth = glyph.DstRectPxf.RawWidth() * scale.X;
        const float scaledDstHeight = glyph.DstRectPxf.RawHeight() * scale.Y;

        m_batchStrategy.AddQuad(dstPos, Vector2(dstPos.X + scaledDstWidth, dstPos.Y), Vector2(dstPos.X, dstPos.Y + scaledDstHeight),
                                Vector2(dstPos.X + scaledDstWidth, dstPos.Y + scaledDstHeight), Vector2(glyph.TextureArea.X0, glyph.TextureArea.Y0),
                                Vector2(glyph.TextureArea.X1, glyph.TextureArea.Y1), color);
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color, origin, scale);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const BitmapFontConfig& fontConfig, const std::string& str,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLiteUtil::AsStringViewLite(str), dstPositionPxf, color, origin, scale);
  }

  // ---------- 14 with clip

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
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
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!font.ExtractRenderRules(glyphScratchpadSpan, strView, fontConfig))
    {
      return;
    }

    const Vector2 dstPosModded(dstPositionPxf.X - (origin.X * scale.X), dstPositionPxf.Y - (origin.Y * scale.Y));

    m_batchStrategy.SetTexture(srcTexture);
    for (std::size_t i = 0; i < strView.size(); ++i)
    {
      const auto& srcGlyph = glyphScratchpadSpan[i];
      if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
      {
        auto dstRectPxf = PxAreaRectangleF::Create((srcGlyph.DstRectPxf.RawLeft() * scale.X) + dstPosModded.X,
                                                   (srcGlyph.DstRectPxf.RawTop() * scale.Y) + dstPosModded.Y,
                                                   srcGlyph.DstRectPxf.RawWidth() * scale.X, srcGlyph.DstRectPxf.RawHeight() * scale.Y);
        auto srcTextureArea = srcGlyph.TextureArea;
        if (Batch2DUtil::Clip(dstRectPxf, srcTextureArea, clipRectPx))
        {
          // Basic quad vertex format
          // 0-1
          // | |
          // 2-3
          m_batchStrategy.AddQuad(dstRectPxf.TopLeft(), dstRectPxf.TopRight(), dstRectPxf.BottomLeft(), dstRectPxf.BottomRight(),
                                  Vector2(srcTextureArea.X0, srcTextureArea.Y0), Vector2(srcTextureArea.X1, srcTextureArea.Y1), color);
        }
      }
    }
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
                                                                       const BitmapFontConfig& fontConfig, const char* const psz,
                                                                       const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                                                       const Vector2& scale, const PxClipRectangle& clipRectPx)
  {
    DrawString(srcTexture, font, fontConfig, StringViewLite(psz), dstPositionPxf, color, origin, scale, clipRectPx);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DrawString(const texture_type& srcTexture, const TextureAtlasSpriteFont& font,
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
    const PxRectangleU32 srcRectPx(texSize.Width / PxValueU(2), texSize.Height / PxValueU(2), PxValueU(1), PxValueU(1));
    PxRectangle finalDstRectanglePx(dstRectanglePx.X(), dstRectanglePx.Y(), dstRectanglePx.Width(), PxSize1D::Create(1));

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Bottom() - PxValue(1));
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Top());
    finalDstRectanglePx.SetWidth(PxSize1D::Create(1));
    finalDstRectanglePx.SetHeight(dstRectanglePx.Height());
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetX(dstRectanglePx.Right() - PxValue(1));
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const texture_type& srcFillTexture, const PxRectangle& dstRectanglePx,
                                                                               const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const PxRectangleU32 srcRectPx(texExtent.Width / PxValueU(2), texExtent.Height / PxValueU(2), PxValueU(1), PxValueU(1));
    PxRectangle finalDstRectanglePx(dstRectanglePx.X(), dstRectanglePx.Y(), dstRectanglePx.Width(), PxSize1D::Create(1));

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Bottom() - PxValue(1));
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetY(dstRectanglePx.Top());
    finalDstRectanglePx.SetWidth(PxSize1D::Create(1));
    finalDstRectanglePx.SetHeight(dstRectanglePx.Height());
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
    finalDstRectanglePx.SetX(dstRectanglePx.Right() - PxValue(1));
    Draw(srcFillTexture, finalDstRectanglePx, srcRectPx, color);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const atlas_texture_type& srcFillTexture,
                                                                               const PxAreaRectangleF& dstRectanglePxf, const Color& color)
  {
    const auto texSize = srcFillTexture.Info.ExtentPx;
    const PxRectangleU32 srcRectPx(texSize.Width / PxValueU(2), texSize.Height / PxValueU(2), PxValueU(1), PxValueU(1));
    PxAreaRectangleF finalDstRectanglePxf(dstRectanglePxf.Left(), dstRectanglePxf.Top(), dstRectanglePxf.Width(), PxSize1DF::Create(1));

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Bottom() - PxValueF(1));
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Top());
    finalDstRectanglePxf.SetWidth(PxSize1DF::Create(1));
    finalDstRectanglePxf.SetHeight(dstRectanglePxf.Height());
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveLeft(dstRectanglePxf.Right() - PxValueF(1));
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawRectangle(const texture_type& srcFillTexture,
                                                                               const PxAreaRectangleF& dstRectanglePxf, const Color& color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const PxRectangleU32 srcRectPx(texExtent.Width / PxValueU(2), texExtent.Height / PxValueU(2), PxValueU(1), PxValueU(1));
    PxAreaRectangleF finalDstRectanglePxf(dstRectanglePxf.Left(), dstRectanglePxf.Top(), dstRectanglePxf.Width(), PxSize1DF::Create(1));

    // Do the drawing
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Bottom() - PxValueF(1));
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveTop(dstRectanglePxf.Top());
    finalDstRectanglePxf.SetWidth(PxSize1DF::Create(1));
    finalDstRectanglePxf.SetHeight(dstRectanglePxf.Height());
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
    finalDstRectanglePxf.MoveLeft(dstRectanglePxf.Right() - PxValueF(1));
    Draw(srcFillTexture, finalDstRectanglePxf, srcRectPx, color);
  }

  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const atlas_texture_type& srcFillTexture, const PxPoint2 dstFromPx,
                                                                          const PxPoint2 dstToPx, const Color color)
  {
    const auto texSize = srcFillTexture.Info.ExtentPx;
    const PxRectangleU32 srcRectPx(texSize.Width / PxValueU(2), texSize.Height / PxValueU(2), PxValueU(1), PxValueU(1));
    if (dstFromPx.X == dstToPx.X)
    {
      // Vertical line
      if (dstToPx.Y > dstFromPx.Y)
      {
        Draw(srcFillTexture, PxRectangle(dstFromPx.X, dstFromPx.Y, PxSize1D::Create(1), dstToPx.Y - dstFromPx.Y), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxRectangle(dstFromPx.X, dstToPx.Y, PxSize1D::Create(1), dstFromPx.Y - dstToPx.Y), srcRectPx, color);
      }
      return;
    }
    if (dstFromPx.Y == dstToPx.Y)
    {
      // Horizontal line
      if (dstToPx.X > dstFromPx.X)
      {
        Draw(srcFillTexture, PxRectangle(dstFromPx.X, dstFromPx.Y, dstToPx.X - dstFromPx.X, PxSize1D::Create(1)), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxRectangle(dstToPx.X, dstFromPx.Y, dstFromPx.X - dstToPx.X, PxSize1D::Create(1)), srcRectPx, color);
      }
      return;
    }

    Vector2 delta(dstToPx.X.Value - dstFromPx.X.Value, dstToPx.Y.Value - dstFromPx.Y.Value);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, Vector2(dstFromPx.X.Value, dstFromPx.Y.Value), srcRectPx, color, rotation, Vector2(), scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const atlas_texture_type& srcFillTexture, const PxVector2 dstFromPxf,
                                                                          const PxVector2 dstToPxf, const Color color)
  {
    const auto texSize = srcFillTexture.Info.ExtentPx;
    const PxRectangleU32 srcRectPx(texSize.Width / PxValueU(2), texSize.Height / PxValueU(2), PxValueU(1), PxValueU(1));

    if (EqualHelper::IsAlmostEqual(dstFromPxf.X.Value, dstToPxf.X.Value))
    {
      // Vertical line
      if (dstToPxf.Y > dstFromPxf.Y)
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstFromPxf.X, dstFromPxf.Y, PxSize1DF::Create(1), dstToPxf.Y - dstFromPxf.Y), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstFromPxf.X, dstToPxf.Y, PxSize1DF::Create(1), dstFromPxf.Y - dstToPxf.Y), srcRectPx, color);
      }
      return;
    }
    if (EqualHelper::IsAlmostEqual(dstFromPxf.Y.Value, dstToPxf.Y.Value))
    {
      // Horizontal line
      if (dstToPxf.X > dstFromPxf.X)
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstFromPxf.X, dstFromPxf.Y, dstToPxf.X - dstFromPxf.X, PxSize1DF::Create(1)), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstToPxf.X, dstFromPxf.Y, dstFromPxf.X - dstToPxf.X, PxSize1DF::Create(1)), srcRectPx, color);
      }
      return;
    }


    Vector2 delta(dstToPxf.X.Value - dstFromPxf.X.Value, dstToPxf.Y.Value - dstFromPxf.Y.Value);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, TypeConverter::To<Vector2>(dstFromPxf), srcRectPx, color, rotation, Vector2(), scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const texture_type& srcFillTexture, const PxPoint2 dstFromPx,
                                                                          const PxPoint2 dstToPx, const Color color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const PxRectangleU32 srcRectPx(texExtent.Width / PxValueU(2), texExtent.Height / PxValueU(2), PxValueU(1), PxValueU(1));

    if (dstFromPx.X == dstToPx.X)
    {
      // Vertical line
      if (dstToPx.Y > dstFromPx.Y)
      {
        Draw(srcFillTexture, PxRectangle(dstFromPx.X, dstFromPx.Y, PxSize1D::Create(1), dstToPx.Y - dstFromPx.Y), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxRectangle(dstFromPx.X, dstToPx.Y, PxSize1D::Create(1), dstFromPx.Y - dstToPx.Y), srcRectPx, color);
      }
      return;
    }
    if (dstFromPx.Y == dstToPx.Y)
    {
      // Horizontal line
      if (dstToPx.X > dstFromPx.X)
      {
        Draw(srcFillTexture, PxRectangle(dstFromPx.X, dstFromPx.Y, dstToPx.X - dstFromPx.X, PxSize1D::Create(1)), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxRectangle(dstToPx.X, dstFromPx.Y, dstFromPx.X - dstToPx.X, PxSize1D::Create(1)), srcRectPx, color);
      }
      return;
    }

    Vector2 delta(dstToPx.X.Value - dstFromPx.X.Value, dstToPx.Y.Value - dstFromPx.Y.Value);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, Vector2(dstFromPx.X.Value, dstFromPx.Y.Value), srcRectPx, color, rotation, Vector2(), scale);
  }


  template <typename TNativeBatch, typename TTexture, typename TVFormatter>
  void GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::DebugDrawLine(const texture_type& srcFillTexture, const PxVector2 dstFromPxf,
                                                                          const PxVector2 dstToPxf, const Color color)
  {
    const auto texExtent = srcFillTexture.Extent;
    const PxRectangleU32 srcRectPx(texExtent.Width / PxValueU(2), texExtent.Height / PxValueU(2), PxValueU(1), PxValueU(1));

    if (EqualHelper::IsAlmostEqual(dstFromPxf.X.Value, dstToPxf.X.Value))
    {
      // Vertical line
      if (dstToPxf.Y > dstFromPxf.Y)
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstFromPxf.X, dstFromPxf.Y, PxSize1DF::Create(1), dstToPxf.Y - dstFromPxf.Y), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstFromPxf.X, dstToPxf.Y, PxSize1DF::Create(1), dstFromPxf.Y - dstToPxf.Y), srcRectPx, color);
      }
      return;
    }
    if (EqualHelper::IsAlmostEqual(dstFromPxf.Y.Value, dstToPxf.Y.Value))
    {
      // Horizontal line
      if (dstToPxf.X > dstFromPxf.X)
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstFromPxf.X, dstFromPxf.Y, dstToPxf.X - dstFromPxf.X, PxSize1DF::Create(1)), srcRectPx, color);
      }
      else
      {
        Draw(srcFillTexture, PxAreaRectangleF(dstToPxf.X, dstFromPxf.Y, dstFromPxf.X - dstToPxf.X, PxSize1DF::Create(1)), srcRectPx, color);
      }
      return;
    }

    Vector2 delta(dstToPxf.X.Value - dstFromPxf.X.Value, dstToPxf.Y.Value - dstFromPxf.Y.Value);
    auto len = delta.Length();
    Vector2 scale(len, 1.0f);
    const float rotation = VectorHelper::VectorToAngle(delta);

    Draw(srcFillTexture, TypeConverter::To<Vector2>(dstFromPxf), srcRectPx, color, rotation, Vector2(), scale);
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
  inline BatchSdfRenderConfig GenericBatch2D<TNativeBatch, TTexture, TVFormatter>::ToBatchSdfRenderConfig(const TextureAtlasSpriteFont& /*font*/,
                                                                                                          const BitmapFontConfig& fontConfig)
  {
    return BatchSdfRenderConfig(fontConfig.Scale);
  }
}

#endif
