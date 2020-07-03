/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/String/StringViewLite.hpp>

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
    m_scalePxToDp = static_cast<float>(SpriteDpConfig::BaseDpi) / static_cast<float>(densityDpi);
    m_scaleDpToPx = static_cast<float>(densityDpi) / static_cast<float>(SpriteDpConfig::BaseDpi);
  }


  PxSize2D SpriteUnitConverter::CalcScaledPxSize2D(const PxExtent2D& extentPx, const uint32_t imageDpi) const
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


  DpExtent SpriteUnitConverter::CalcImageDpExtent(const PxExtent2D imageExtentPx, const uint32_t imageDpi)
  {
    if (imageDpi == 0u)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }
    if (imageDpi == SpriteDpConfig::BaseDpi)
    {
      return {imageExtentPx.Width, imageExtentPx.Height};
    }
    const float scalePxToDp = static_cast<float>(SpriteDpConfig::BaseDpi) / static_cast<float>(imageDpi);

    const float scaledX = std::round(float(imageExtentPx.Width) * scalePxToDp);
    const float scaledY = std::round(float(imageExtentPx.Height) * scalePxToDp);
    assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<DpExtent::value_type>::max()));
    assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<DpExtent::value_type>::max()));
    return {static_cast<DpExtent::value_type>(scaledX), static_cast<DpExtent::value_type>(scaledY)};
  }

  DpThicknessU SpriteUnitConverter::CalcDpThicknessU(const PxThicknessU& thicknessPx, const uint32_t imageDpi)
  {
    if (imageDpi == 0u)
    {
      throw std::invalid_argument("imageDpi can not be zero");
    }
    if (imageDpi == SpriteDpConfig::BaseDpi)
    {
      return {thicknessPx.Left, thicknessPx.Top, thicknessPx.Right, thicknessPx.Bottom};
    }
    const float scalePxToDp = static_cast<float>(SpriteDpConfig::BaseDpi) / static_cast<float>(imageDpi);

    const float scaledL = std::round(float(thicknessPx.Left) * scalePxToDp);
    const float scaledT = std::round(float(thicknessPx.Top) * scalePxToDp);
    const float scaledR = std::round(float(thicknessPx.Right) * scalePxToDp);
    const float scaledB = std::round(float(thicknessPx.Bottom) * scalePxToDp);
    assert(scaledL >= 0.0f && scaledL <= float(std::numeric_limits<DpThicknessU::value_type>::max()));
    assert(scaledT >= 0.0f && scaledT <= float(std::numeric_limits<DpThicknessU::value_type>::max()));
    assert(scaledR >= 0.0f && scaledR <= float(std::numeric_limits<DpThicknessU::value_type>::max()));
    assert(scaledB >= 0.0f && scaledB <= float(std::numeric_limits<DpThicknessU::value_type>::max()));
    return {static_cast<DpThicknessU::value_type>(scaledL), static_cast<DpThicknessU::value_type>(scaledT),
            static_cast<DpThicknessU::value_type>(scaledR), static_cast<DpThicknessU::value_type>(scaledB)};
  }

}
