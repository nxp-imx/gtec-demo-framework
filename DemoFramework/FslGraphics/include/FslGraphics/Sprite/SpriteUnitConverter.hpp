#ifndef FSLGRAPHICS_SPRITE_SPRITEUNITCONVERTER_HPP
#define FSLGRAPHICS_SPRITE_SPRITEUNITCONVERTER_HPP
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

#include <FslBase/Math/Dp/DpExtent.hpp>
#include <FslBase/Math/Dp/DpPoint.hpp>
#include <FslBase/Math/Dp/DpPointF.hpp>
#include <FslBase/Math/Dp/DpSize.hpp>
#include <FslBase/Math/Dp/DpThickness.hpp>
#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslBase/Math/Dp/DpThicknessU.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxThickness.hpp>
#include <FslBase/Math/Pixel/PxThicknessF.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslBase/Math/Pixel/PxTrimmedImage.hpp>
#include <FslBase/Math/Pixel/PxTrimmedNineSlice.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Sprite/SpriteDpConfig.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace Fsl
{
  class SpriteUnitConverter
  {
    uint32_t m_densityDpi{};
    float m_scalePxToDp{};
    float m_scaleDpToPx{};

  public:
    explicit SpriteUnitConverter(const uint32_t densityDpi);

    void SetDensityDpi(const uint32_t densityDpi);

    uint32_t GetDensityDpi() const
    {
      return m_densityDpi;
    }

    inline int32_t DpToPxInt32(const int32_t valueDp) const
    {
      return static_cast<int32_t>(std::round(valueDp * m_scaleDpToPx));
    }

    inline int32_t DpToPxInt32(const float valueDpf) const
    {
      return static_cast<int32_t>(std::round(valueDpf * m_scaleDpToPx));
    }

    inline int32_t PxToDpInt32(const int32_t valuePx) const
    {
      return static_cast<int32_t>(std::round(float(valuePx) * m_scalePxToDp));
    }

    inline int32_t PxToDpInt32(const float valuePxf) const
    {
      return static_cast<int32_t>(std::round(valuePxf * m_scalePxToDp));
    }

    inline float PxToDpf(const int32_t valuePx) const
    {
      return float(valuePx) * m_scalePxToDp;
    }

    inline float PxToDpf(const float valuePxf) const
    {
      return valuePxf * m_scalePxToDp;
    }


    inline int32_t PxfToPxInt32(const float valuePxf) const
    {
      return static_cast<int32_t>(std::round(valuePxf));
    }

    inline PxExtent2D ToPxExtent2D(const DpExtent& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valueDp.Width, valueDp.Height};
      }

      const float scaledX = std::round(float(valueDp.Width) * m_scaleDpToPx);
      const float scaledY = std::round(float(valueDp.Height) * m_scaleDpToPx);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<PxExtent2D::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<PxExtent2D::value_type>::max()));
      return {static_cast<PxExtent2D::value_type>(scaledX), static_cast<PxExtent2D::value_type>(scaledY)};
    }


    DpPoint ToDpPoint(const PxPoint2& valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valuePx.X, valuePx.Y};
      }
      const float scaledX = std::round(float(valuePx.X) * m_scalePxToDp);
      const float scaledY = std::round(float(valuePx.Y) * m_scalePxToDp);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<DpPoint::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<DpPoint::value_type>::max()));
      return {static_cast<DpPoint::value_type>(scaledX), static_cast<DpPoint::value_type>(scaledY)};
    }

    DpSize ToDpSize(const PxPoint2& valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valuePx.X, valuePx.Y};
      }
      const float scaledX = std::round(float(valuePx.X) * m_scalePxToDp);
      const float scaledY = std::round(float(valuePx.Y) * m_scalePxToDp);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<DpSize::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<DpSize::value_type>::max()));
      return {static_cast<DpSize::value_type>(scaledX), static_cast<DpSize::value_type>(scaledY)};
    }


    DpExtent ToDpExtent(const PxExtent2D& valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valuePx.Width, valuePx.Height};
      }
      const float scaledX = std::round(float(valuePx.Width) * m_scalePxToDp);
      const float scaledY = std::round(float(valuePx.Height) * m_scalePxToDp);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<DpExtent::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<DpExtent::value_type>::max()));
      return {static_cast<DpExtent::value_type>(scaledX), static_cast<DpExtent::value_type>(scaledY)};
    }

    DpThicknessU ToDpThicknessU(const PxThicknessU& valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valuePx.Left, valuePx.Top, valuePx.Right, valuePx.Bottom};
      }
      const float scaledL = std::round(float(valuePx.Left) * m_scalePxToDp);
      const float scaledT = std::round(float(valuePx.Top) * m_scalePxToDp);
      const float scaledR = std::round(float(valuePx.Right) * m_scalePxToDp);
      const float scaledB = std::round(float(valuePx.Bottom) * m_scalePxToDp);
      assert(scaledL >= 0.0f && scaledL <= float(std::numeric_limits<DpExtent::value_type>::max()));
      assert(scaledT >= 0.0f && scaledT <= float(std::numeric_limits<DpExtent::value_type>::max()));
      assert(scaledR >= 0.0f && scaledR <= float(std::numeric_limits<DpExtent::value_type>::max()));
      assert(scaledB >= 0.0f && scaledB <= float(std::numeric_limits<DpExtent::value_type>::max()));
      return {static_cast<DpThicknessU::value_type>(scaledL), static_cast<DpThicknessU::value_type>(scaledT),
              static_cast<DpThicknessU::value_type>(scaledR), static_cast<DpThicknessU::value_type>(scaledB)};
    }


    PxPoint2 ToPxPoint2(const DpPoint& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valueDp.X, valueDp.Y};
      }

      const float scaledX = std::round(float(valueDp.X) * m_scaleDpToPx);
      const float scaledY = std::round(float(valueDp.Y) * m_scaleDpToPx);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      return {static_cast<PxPoint2::value_type>(scaledX), static_cast<PxPoint2::value_type>(scaledY)};
    }

    PxPoint2 ToPxPoint2(const DpPointF& valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {static_cast<PxPoint2::value_type>(std::round(valueDpf.X)), static_cast<PxPoint2::value_type>(std::round(valueDpf.Y))};
      }

      const float scaledX = std::round(float(valueDpf.X) * m_scaleDpToPx);
      const float scaledY = std::round(float(valueDpf.Y) * m_scaleDpToPx);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      return {static_cast<PxPoint2::value_type>(scaledX), static_cast<PxPoint2::value_type>(scaledY)};
    }

    PxPoint2 ToPxPoint2(const DpSize& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valueDp.Width(), valueDp.Height()};
      }

      const float scaledX = std::round(float(valueDp.Width()) * m_scaleDpToPx);
      const float scaledY = std::round(float(valueDp.Height()) * m_scaleDpToPx);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      return {static_cast<PxPoint2::value_type>(scaledX), static_cast<PxPoint2::value_type>(scaledY)};
    }

    PxSize2D ToPxSize2D(const DpExtent& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {UncheckedNumericCast<PxSize2D::value_type>(valueDp.Width), UncheckedNumericCast<PxSize2D::value_type>(valueDp.Height)};
      }

      const float scaledX = std::round(float(valueDp.Width) * m_scaleDpToPx);
      const float scaledY = std::round(float(valueDp.Height) * m_scaleDpToPx);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<PxSize2D::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<PxSize2D::value_type>::max()));
      return {static_cast<PxSize2D::value_type>(scaledX), static_cast<PxSize2D::value_type>(scaledY)};
    }

    PxThickness ToPxThickness(const DpThickness& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valueDp.Left(), valueDp.Top(), valueDp.Right(), valueDp.Bottom()};
      }

      const float scaledL = std::round(float(valueDp.Left()) * m_scaleDpToPx);
      const float scaledT = std::round(float(valueDp.Top()) * m_scaleDpToPx);
      const float scaledR = std::round(float(valueDp.Right()) * m_scaleDpToPx);
      const float scaledB = std::round(float(valueDp.Bottom()) * m_scaleDpToPx);
      assert(scaledL >= 0.0f && scaledL <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledT >= 0.0f && scaledT <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledR >= 0.0f && scaledR <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledB >= 0.0f && scaledB <= float(std::numeric_limits<PxThickness::value_type>::max()));
      return {static_cast<PxThickness::value_type>(scaledL), static_cast<PxThickness::value_type>(scaledT),
              static_cast<PxThickness::value_type>(scaledR), static_cast<PxThickness::value_type>(scaledB)};
    }


    PxThickness ToPxThickness(const DpThicknessF& valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        const float valueLDpf = std::round(valueDpf.Left());
        const float valueTDpf = std::round(valueDpf.Top());
        const float valueRDpf = std::round(valueDpf.Right());
        const float valueBDpf = std::round(valueDpf.Bottom());

        assert(valueLDpf >= 0.0f && valueLDpf <= float(std::numeric_limits<PxThickness::value_type>::max()));
        assert(valueTDpf >= 0.0f && valueTDpf <= float(std::numeric_limits<PxThickness::value_type>::max()));
        assert(valueRDpf >= 0.0f && valueRDpf <= float(std::numeric_limits<PxThickness::value_type>::max()));
        assert(valueBDpf >= 0.0f && valueBDpf <= float(std::numeric_limits<PxThickness::value_type>::max()));

        return {static_cast<PxThickness::value_type>(valueLDpf), static_cast<PxThickness::value_type>(valueTDpf),
                static_cast<PxThickness::value_type>(valueRDpf), static_cast<PxThickness::value_type>(valueBDpf)};
      }

      const float scaledL = std::round(valueDpf.Left() * m_scaleDpToPx);
      const float scaledT = std::round(valueDpf.Top() * m_scaleDpToPx);
      const float scaledR = std::round(valueDpf.Right() * m_scaleDpToPx);
      const float scaledB = std::round(valueDpf.Bottom() * m_scaleDpToPx);
      assert(scaledL >= 0.0f && scaledL <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledT >= 0.0f && scaledT <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledR >= 0.0f && scaledR <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledB >= 0.0f && scaledB <= float(std::numeric_limits<PxThickness::value_type>::max()));
      return {static_cast<PxThickness::value_type>(scaledL), static_cast<PxThickness::value_type>(scaledT),
              static_cast<PxThickness::value_type>(scaledR), static_cast<PxThickness::value_type>(scaledB)};
    }


    inline PxThickness ToPxThickness(const DpThicknessU& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {UncheckedNumericCast<PxThickness::value_type>(valueDp.Left), UncheckedNumericCast<PxThickness::value_type>(valueDp.Top),
                UncheckedNumericCast<PxThickness::value_type>(valueDp.Right), UncheckedNumericCast<PxThickness::value_type>(valueDp.Bottom)};
      }

      const float scaledL = std::round(float(valueDp.Left) * m_scaleDpToPx);
      const float scaledT = std::round(float(valueDp.Top) * m_scaleDpToPx);
      const float scaledR = std::round(float(valueDp.Right) * m_scaleDpToPx);
      const float scaledB = std::round(float(valueDp.Bottom) * m_scaleDpToPx);
      assert(scaledL >= 0.0f && scaledL <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledT >= 0.0f && scaledT <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledR >= 0.0f && scaledR <= float(std::numeric_limits<PxThickness::value_type>::max()));
      assert(scaledB >= 0.0f && scaledB <= float(std::numeric_limits<PxThickness::value_type>::max()));
      return {static_cast<PxThickness::value_type>(scaledL), static_cast<PxThickness::value_type>(scaledT),
              static_cast<PxThickness::value_type>(scaledR), static_cast<PxThickness::value_type>(scaledB)};
    }

    PxVector2 ToPxVector2(const DpPointF& valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return {valueDpf.X, valueDpf.Y};
      }

      const float scaledX = float(valueDpf.X) * m_scaleDpToPx;
      const float scaledY = float(valueDpf.Y) * m_scaleDpToPx;
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<PxPoint2::value_type>::max()));
      return {scaledX, scaledY};
    }


    constexpr float CalcImageDensityScale(const uint32_t imageDpi) const
    {
      if (imageDpi == 0u)
      {
        throw std::invalid_argument("imageDpi can not be zero");
      }

      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        // ok there is no density scaling, so we just need to figure out the image scaling
        // return (imageDpi != m_densityDpi ? 1.0f / (static_cast<float>(imageDpi) / static_cast<float>(SpriteDpConfig::BaseDpi)) : 1.0f);
        return (imageDpi != m_densityDpi ? static_cast<float>(SpriteDpConfig::BaseDpi) / static_cast<float>(imageDpi) : 1.0f);
      }

      // There is density scaling, so lets examine the image scaling
      if (imageDpi == SpriteDpConfig::BaseDpi)
      {
        // No image scaling, pure density scaling
        return static_cast<float>(m_densityDpi) / static_cast<float>(SpriteDpConfig::BaseDpi);
      }

      // both image scaling and density scaling
      // return (static_cast<float>(m_densityDpi) / static_cast<float>(SpriteDpConfig::BaseDpi)) /
      //       (static_cast<float>(imageDpi) / static_cast<float>(SpriteDpConfig::BaseDpi));
      return static_cast<float>(m_densityDpi) / static_cast<float>(imageDpi);
    }

    PxSize2D CalcScaledPxSize2D(const PxExtent2D& extentPx, const uint32_t imageDpi) const;
    PxSize2DF CalcScaledPxSize2DF(const PxExtent2D& extentPx, const uint32_t imageDpi) const;
    PxThickness CalcScaledPxThickness(const PxThickness& thicknessPx, const uint32_t imageDpi) const;
    PxThickness CalcScaledPxThickness(const PxThicknessU& thicknessPx, const uint32_t imageDpi) const;
    PxThicknessF CalcScaledPxThicknessF(const PxThicknessU& thicknessPx, const uint32_t imageDpi) const;
    PxThicknessU CalcScaledPxThicknessU(const PxThicknessU& thicknessPx, const uint32_t imageDpi) const;
    PxTrimmedImage CalcScaledPxTrimmedImage(const PxExtent2D extentPx, const PxThicknessU& trimMarginPx, const PxExtent2D trimmedExtentPx,
                                            const uint32_t imageDpi) const;
    PxTrimmedNineSlice CalcScaledPxTrimmedNineSlice(const PxExtent2D extentPx, const PxThicknessU& trimMarginPx,
                                                    const PxThicknessU& trimmedNineSlicePx, const PxThicknessU& contentMarginPx,
                                                    const uint32_t imageDpi) const;

    /// Calc how much a image of a given DPI should be scaled.
    // static constexpr float CalcImageScale(const uint32_t imageDpi)
    //{
    //  return imageDpi != SpriteDpConfig::BaseDpi ? static_cast<float>(imageDpi) / static_cast<float>(SpriteDpConfig::BaseDpi) : 1.0f;
    //}

    static DpExtent CalcImageDpExtent(const PxExtent2D imageExtentPx, const uint32_t imageDpi);
    static DpThicknessU CalcDpThicknessU(const PxThicknessU& thicknessPx, const uint32_t imageDpi);

  private:
  };
}

#endif
