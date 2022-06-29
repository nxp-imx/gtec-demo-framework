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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>

namespace Fsl
{
  SpriteUnitConverter::SpriteUnitConverter(const uint32_t densityDpi)
    : m_densityDpi(densityDpi)
  {
    SetDensityDpi(densityDpi);
  }

  void SpriteUnitConverter::SetDensityDpi(const uint32_t densityDpi)
  {
    if (densityDpi <= 0u)
    {
      throw std::invalid_argument("densityDpi can not be zero");
    }
    m_densityDpi = densityDpi;
    m_scalePxToDpf = static_cast<float>(SpriteDpConfig::BaseDpi) / static_cast<float>(densityDpi);
    m_scaleDpToPxf = static_cast<float>(densityDpi) / static_cast<float>(SpriteDpConfig::BaseDpi);
  }


  PxSize2D SpriteUnitConverter::CalcScaledPxSize2D(const PxExtent2D extentPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }

    // if (imageDpi == m_densityDpi)
    //{
    //  return TypeConverter::To<PxSize2D>(extentPx);
    //}

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    const float widthPx = std::round(static_cast<float>(extentPx.Width) * scaleFactor);
    const float heightPx = std::round(static_cast<float>(extentPx.Height) * scaleFactor);
    assert(widthPx >= 0.0f && widthPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    assert(heightPx >= 0.0f && heightPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    return {static_cast<PxSize2D::value_type>(widthPx), static_cast<PxSize2D::value_type>(heightPx)};
  }


  PxSize2DF SpriteUnitConverter::CalcScaledPxSize2DF(const PxExtent2D extentPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }

    // if (imageDpi == m_densityDpi)
    //{
    //  return TypeConverter::To<PxSize2D>(extentPx);
    //}

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    const float widthPx = static_cast<float>(extentPx.Width) * scaleFactor;
    const float heightPx = static_cast<float>(extentPx.Height) * scaleFactor;
    assert(widthPx >= 0.0f && widthPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    assert(heightPx >= 0.0f && heightPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    return {widthPx, heightPx};
  }


  PxThickness SpriteUnitConverter::CalcScaledPxThickness(const PxThickness& thicknessPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }

    // if (imageDpi == m_densityDpi)
    //{
    //  return thicknessPx;
    //}

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    const float trimLPx = std::round(static_cast<float>(thicknessPx.Left()) * scaleFactor);
    const float trimTPx = std::round(static_cast<float>(thicknessPx.Top()) * scaleFactor);
    const float trimRPx = std::round(static_cast<float>(thicknessPx.Right()) * scaleFactor);
    const float trimBPx = std::round(static_cast<float>(thicknessPx.Bottom()) * scaleFactor);
    assert(trimLPx >= 0.0f && trimLPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimTPx >= 0.0f && trimTPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimRPx >= 0.0f && trimRPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimBPx >= 0.0f && trimBPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    return {static_cast<PxThickness::value_type>(trimLPx), static_cast<PxThickness::value_type>(trimTPx),
            static_cast<PxThickness::value_type>(trimRPx), static_cast<PxThickness::value_type>(trimBPx)};
  }


  PxThickness SpriteUnitConverter::CalcScaledPxThickness(const PxThicknessU& thicknessPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }

    // if (imageDpi == m_densityDpi)
    //{
    //  return TypeConverter::To<PxThickness>(thicknessPx);
    //}

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    const float trimLPx = std::round(static_cast<float>(thicknessPx.Left) * scaleFactor);
    const float trimTPx = std::round(static_cast<float>(thicknessPx.Top) * scaleFactor);
    const float trimRPx = std::round(static_cast<float>(thicknessPx.Right) * scaleFactor);
    const float trimBPx = std::round(static_cast<float>(thicknessPx.Bottom) * scaleFactor);
    assert(trimLPx >= 0.0f && trimLPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimTPx >= 0.0f && trimTPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimRPx >= 0.0f && trimRPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimBPx >= 0.0f && trimBPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    return {static_cast<PxThickness::value_type>(trimLPx), static_cast<PxThickness::value_type>(trimTPx),
            static_cast<PxThickness::value_type>(trimRPx), static_cast<PxThickness::value_type>(trimBPx)};
  }


  PxThicknessF SpriteUnitConverter::CalcScaledPxThicknessF(const PxThicknessU& thicknessPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }

    // if (imageDpi == m_densityDpi)
    //{
    //  return TypeConverter::To<PxThickness>(thicknessPx);
    //}

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    const float trimLPx = static_cast<float>(thicknessPx.Left) * scaleFactor;
    const float trimTPx = static_cast<float>(thicknessPx.Top) * scaleFactor;
    const float trimRPx = static_cast<float>(thicknessPx.Right) * scaleFactor;
    const float trimBPx = static_cast<float>(thicknessPx.Bottom) * scaleFactor;
    assert(trimLPx >= 0.0f && trimLPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimTPx >= 0.0f && trimTPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimRPx >= 0.0f && trimRPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(trimBPx >= 0.0f && trimBPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    return {trimLPx, trimTPx, trimRPx, trimBPx};
  }


  PxThicknessU SpriteUnitConverter::CalcScaledPxThicknessU(const PxThicknessU& thicknessPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }
    // if (imageDpi == m_densityDpi)
    //{
    //  return thicknessPx;
    //}

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    const float trimLPx = std::round(static_cast<float>(thicknessPx.Left) * scaleFactor);
    const float trimTPx = std::round(static_cast<float>(thicknessPx.Top) * scaleFactor);
    const float trimRPx = std::round(static_cast<float>(thicknessPx.Right) * scaleFactor);
    const float trimBPx = std::round(static_cast<float>(thicknessPx.Bottom) * scaleFactor);
    assert(trimLPx >= 0.0f && trimLPx <= float(std::numeric_limits<PxThicknessU::value_type>::max()));
    assert(trimTPx >= 0.0f && trimTPx <= float(std::numeric_limits<PxThicknessU::value_type>::max()));
    assert(trimRPx >= 0.0f && trimRPx <= float(std::numeric_limits<PxThicknessU::value_type>::max()));
    assert(trimBPx >= 0.0f && trimBPx <= float(std::numeric_limits<PxThicknessU::value_type>::max()));
    return {static_cast<PxThicknessU::value_type>(trimLPx), static_cast<PxThicknessU::value_type>(trimTPx),
            static_cast<PxThicknessU::value_type>(trimRPx), static_cast<PxThicknessU::value_type>(trimBPx)};
  }


  PxTrimmedImage SpriteUnitConverter::CalcScaledPxTrimmedImage(const PxExtent2D extentPx, const PxThicknessU& trimMarginPx,
                                                               const PxExtent2D trimmedExtentPx, const uint32_t imageDpi) const
  {
    assert(extentPx.Width >= trimmedExtentPx.Width);
    assert(extentPx.Height >= trimmedExtentPx.Height);
    assert(extentPx == (trimMarginPx.Sum() + trimmedExtentPx));
    if (imageDpi == 0u)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }
    float scaleFactor = CalcImageDensityScale(imageDpi);

    const float roundedNewWidthPx = std::round(static_cast<float>(extentPx.Width) * scaleFactor);
    const float roundedNewHeightPx = std::round(static_cast<float>(extentPx.Height) * scaleFactor);

    assert(roundedNewWidthPx >= 0.0f && roundedNewWidthPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    assert(roundedNewHeightPx >= 0.0f && roundedNewHeightPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    PxSize2D roundedScaledSizePx(static_cast<PxSize2D::value_type>(roundedNewWidthPx), static_cast<PxSize2D::value_type>(roundedNewHeightPx));
    if (extentPx == trimmedExtentPx)
    {
      // Quick exit for 'no trim'
      assert(trimMarginPx.Sum() == PxExtent2D());
      return {roundedScaledSizePx, PxThicknessF(), TypeConverter::To<PxSize2DF>(roundedScaledSizePx)};
    }

    // Figure out the actually used scale factor for each axis to ensure the trim and trimmed size are calculated to match this rounding
    const float finalScaleX = static_cast<float>(roundedScaledSizePx.Width()) / static_cast<float>(extentPx.Width);
    const float finalScaleY = static_cast<float>(roundedScaledSizePx.Height()) / static_cast<float>(extentPx.Height);

    // Scale the trim margin
    PxThicknessF scaledTrimMarginPxf(static_cast<float>(trimMarginPx.Left) * finalScaleX, static_cast<float>(trimMarginPx.Top) * finalScaleY,
                                     static_cast<float>(trimMarginPx.Right) * finalScaleX, static_cast<float>(trimMarginPx.Bottom) * finalScaleY);
    PxSize2DF scaledTrimmedSizePxf(static_cast<float>(trimmedExtentPx.Width) * finalScaleX, static_cast<float>(trimmedExtentPx.Height) * finalScaleY);
    return {roundedScaledSizePx, scaledTrimMarginPxf, scaledTrimmedSizePxf};
  }


  PxTrimmedNineSlice SpriteUnitConverter::CalcScaledPxTrimmedNineSlice(const PxExtent2D extentPx, const PxThicknessU& trimMarginPx,
                                                                       const PxThicknessU& trimmedNineSlicePx, const PxThicknessU& contentMarginPx,
                                                                       const uint32_t imageDpi) const
  {
    if (imageDpi == 0u)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }
    float scaleFactor = CalcImageDensityScale(imageDpi);

    const float roundedNewWidthPx = std::round(static_cast<float>(extentPx.Width) * scaleFactor);
    const float roundedNewHeightPx = std::round(static_cast<float>(extentPx.Height) * scaleFactor);

    assert(roundedNewWidthPx >= 0.0f && roundedNewWidthPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    assert(roundedNewHeightPx >= 0.0f && roundedNewHeightPx <= float(std::numeric_limits<PxSize2D::value_type>::max()));
    PxSize2D roundedScaledSizePx(static_cast<PxSize2D::value_type>(roundedNewWidthPx), static_cast<PxSize2D::value_type>(roundedNewHeightPx));

    // Figure out the actually used scale factor for each axis to ensure the trim and trimmed size are calculated to match this rounding
    const float finalScaleX = static_cast<float>(roundedScaledSizePx.Width()) / static_cast<float>(extentPx.Width);
    const float finalScaleY = static_cast<float>(roundedScaledSizePx.Height()) / static_cast<float>(extentPx.Height);

    // Scale the trim margin
    PxThicknessF scaledTrimMarginPxf(static_cast<float>(trimMarginPx.Left) * finalScaleX, static_cast<float>(trimMarginPx.Top) * finalScaleY,
                                     static_cast<float>(trimMarginPx.Right) * finalScaleX, static_cast<float>(trimMarginPx.Bottom) * finalScaleY);

    // Scale the trimmed nine slice
    PxThicknessF scaledTrimmedNineSlicePxf(
      static_cast<float>(trimmedNineSlicePx.Left) * finalScaleX, static_cast<float>(trimmedNineSlicePx.Top) * finalScaleY,
      static_cast<float>(trimmedNineSlicePx.Right) * finalScaleX, static_cast<float>(trimmedNineSlicePx.Bottom) * finalScaleY);


    // Scale the content margin
    const float scaledContentLPx = std::round(static_cast<float>(contentMarginPx.Left) * finalScaleX);
    const float scaledContentTPx = std::round(static_cast<float>(contentMarginPx.Top) * finalScaleY);
    const float scaledContentRPx = std::round(static_cast<float>(contentMarginPx.Right) * finalScaleX);
    const float scaledContentBPx = std::round(static_cast<float>(contentMarginPx.Bottom) * finalScaleY);

    assert(scaledContentLPx >= 0.0f && scaledContentLPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(scaledContentTPx >= 0.0f && scaledContentTPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(scaledContentRPx >= 0.0f && scaledContentRPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    assert(scaledContentBPx >= 0.0f && scaledContentBPx <= float(std::numeric_limits<PxThickness::value_type>::max()));
    PxThickness roundedScaledContentMarginPx(
      static_cast<PxThickness::value_type>(scaledContentLPx), static_cast<PxThickness::value_type>(scaledContentTPx),
      static_cast<PxThickness::value_type>(scaledContentRPx), static_cast<PxThickness::value_type>(scaledContentBPx));


    return {roundedScaledSizePx, scaledTrimMarginPxf, scaledTrimmedNineSlicePxf, roundedScaledContentMarginPx};
  }


  DpExtent2D SpriteUnitConverter::CalcImageDpExtent2D(const PxExtent2D imageExtentPx, const uint32_t imageDpi)
  {
    if (imageDpi == 0u)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }
    if (imageDpi == SpriteDpConfig::BaseDpi)
    {
      return DpExtent2D::Create(imageExtentPx.Width, imageExtentPx.Height);
    }
    const float scalePxToDp = static_cast<float>(SpriteDpConfig::BaseDpi) / static_cast<float>(imageDpi);
    return TypeConverter::UncheckedChangeTo<DpExtent2D>(
      DpSize2DF::Create(static_cast<float>(imageExtentPx.Width) * scalePxToDp, static_cast<float>(imageExtentPx.Height) * scalePxToDp));
  }

  DpThicknessU SpriteUnitConverter::CalcDpThicknessU(const PxThicknessU& thicknessPx, const uint32_t imageDpi)
  {
    if (imageDpi == 0u)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }
    if (imageDpi == SpriteDpConfig::BaseDpi)
    {
      return DpThicknessU::Create(thicknessPx.Left, thicknessPx.Top, thicknessPx.Right, thicknessPx.Bottom);
    }
    const float scalePxToDp = static_cast<float>(SpriteDpConfig::BaseDpi) / static_cast<float>(imageDpi);

    return TypeConverter::UncheckedChangeTo<DpThicknessU>(
      DpThicknessF::Create(static_cast<float>(thicknessPx.Left) * scalePxToDp, static_cast<float>(thicknessPx.Top) * scalePxToDp,
                           static_cast<float>(thicknessPx.Right) * scalePxToDp, static_cast<float>(thicknessPx.Bottom) * scalePxToDp));
  }


  PxVector2 SpriteUnitConverter::CalcScaledOffsetPxVector2(const PxPoint2 offsetPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }

    if (imageDpi == m_densityDpi)
    {
      return TypeConverter::To<PxVector2>(offsetPx);
    }

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    return {static_cast<float>(offsetPx.X) * scaleFactor, static_cast<float>(offsetPx.Y) * scaleFactor};
  }


  float SpriteUnitConverter::CalcScaledOffsetValuePxf(const int32_t offsetPx, const uint32_t imageDpi) const
  {
    if (imageDpi == 0)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }

    if (imageDpi == m_densityDpi)
    {
      return static_cast<float>(offsetPx);
    }

    const float scaleFactor = CalcImageDensityScale(imageDpi);
    return static_cast<float>(offsetPx) * scaleFactor;
  }
}
