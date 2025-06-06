#ifndef FSLGRAPHICS_SPRITE_SPRITEUNITCONVERTER_HPP
#define FSLGRAPHICS_SPRITE_SPRITEUNITCONVERTER_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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

#include <FslBase/Math/Dp/DpExtent2D.hpp>
#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Math/Dp/DpSize1D.hpp>
#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslBase/Math/Dp/DpSize2D.hpp>
#include <FslBase/Math/Dp/DpThickness.hpp>
#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslBase/Math/Dp/DpThicknessU.hpp>
#include <FslBase/Math/Dp/TypeConverter.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxSize1DF.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxThickness.hpp>
#include <FslBase/Math/Pixel/PxThicknessF.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslBase/Math/Pixel/PxTrimmedImage.hpp>
#include <FslBase/Math/Pixel/PxTrimmedNineSlice.hpp>
#include <FslBase/Math/Pixel/PxValueF.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
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
    float m_scalePxToDpf{};
    float m_scaleDpToPxf{};

  public:
    SpriteUnitConverter(const SpriteUnitConverter&) = delete;
    SpriteUnitConverter& operator=(const SpriteUnitConverter&) = delete;

    explicit SpriteUnitConverter(const uint32_t densityDpi);

    void SetDensityDpi(const uint32_t densityDpi);

    uint32_t GetDensityDpi() const
    {
      return m_densityDpi;
    }

    inline int32_t DpToPxInt32(const int32_t valueDp) const
    {
      return ToPxInt32(DpValue(valueDp));
    }

    inline PxSize1D DpToPxSize1D(const int32_t valueDpf) const
    {
      return ToPxSize1D(DpSize1D::Create(valueDpf));
    }

    inline PxSize1D DpToPxSize1D(const float valueDpf) const
    {
      return ToPxSize1D(DpSize1DF::Create(valueDpf));
    }

    inline int32_t DpToPxInt32(const float valueDpf) const
    {
      return ToPxInt32(DpValueF(valueDpf));
    }

    inline int32_t PxToDpInt32(const int32_t valuePx) const
    {
      return ToDpInt32(PxValue(valuePx));
    }

    inline int32_t PxToDpInt32(const float valuePxf) const
    {
      return ToDpInt32(PxValueF(valuePxf));
    }

    inline float PxToDpf(const int32_t valuePx) const
    {
      return static_cast<float>(valuePx) * m_scalePxToDpf;
    }

    inline float PxToDpf(const float valuePxf) const
    {
      return valuePxf * m_scalePxToDpf;
    }


    inline int32_t PxfToPxInt32(const float valuePxf) const
    {
      return static_cast<int32_t>(std::round(valuePxf));
    }

    inline PxSize1D PxfToPxSize1D(const float valuePxf) const
    {
      return PxSize1D::Create(static_cast<int32_t>(std::round(valuePxf)));
    }


    inline PxExtent2D ToPxExtent2D(const DpExtent2D& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxExtent2D::Create(valueDp.Width.Value, valueDp.Height.Value);
      }

      return TypeConverter::UncheckedChangeTo<PxExtent2D>(
        PxVector2::Create(static_cast<float>(valueDp.Width.Value) * m_scaleDpToPxf, static_cast<float>(valueDp.Height.Value) * m_scaleDpToPxf));
    }

    inline int32_t ToDpInt32(const PxValue valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValue>(DpValueF(static_cast<float>(valuePx.Value) * m_scalePxToDpf)).Value;
    }

    inline int32_t ToDpInt32(const PxValueF valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValue>(DpValueF(valuePx.Value * m_scalePxToDpf)).Value;
    }


    inline DpValue ToDpValue(const PxValue valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValue>(DpValueF(static_cast<float>(valuePx.Value) * m_scalePxToDpf));
    }

    inline DpValue ToDpValue(const PxValueF valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValue>(DpValueF(valuePx.Value * m_scalePxToDpf));
    }

    inline DpValue ToDpValue(const PxValueU valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValue>(DpValueF(static_cast<float>(valuePx.Value) * m_scalePxToDpf));
    }

    inline DpValueF ToDpValueF(const PxValue valuePx) const
    {
      return DpValueF(static_cast<float>(valuePx.Value) * m_scalePxToDpf);
    }

    inline DpValueF ToDpValueF(const PxValueF valuePx) const
    {
      return DpValueF(valuePx.Value * m_scalePxToDpf);
    }

    inline DpValueF ToDpValueF(const PxValueU valuePx) const
    {
      return DpValueF(static_cast<float>(valuePx.Value) * m_scalePxToDpf);
    }

    inline DpValueU ToDpValueU(const PxValue valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValueU>(DpValueF(static_cast<float>(valuePx.Value) * m_scalePxToDpf));
    }

    inline DpValueU ToDpValueU(const PxValueF valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValueU>(DpValueF(valuePx.Value * m_scalePxToDpf));
    }

    inline DpValueU ToDpValueU(const PxValueU valuePx) const
    {
      return TypeConverter::UncheckedChangeTo<DpValueU>(DpValueF(static_cast<float>(valuePx.Value) * m_scalePxToDpf));
    }

    inline DpSize1DF ToDpSize1DF(const PxValue valuePx) const
    {
      return DpSize1DF(ToDpValueF(valuePx));
    }

    inline DpSize1DF ToDpSize1DF(const PxValueF valuePx) const
    {
      return DpSize1DF(ToDpValueF(valuePx));
    }

    inline DpSize1DF ToDpSize1DF(const PxValueU valuePx) const
    {
      return DpSize1DF(ToDpValueF(valuePx));
    }

    DpPoint2 ToDpPoint2(const PxPoint2 valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return DpPoint2::Create(valuePx.X.Value, valuePx.Y.Value);
      }
      const float scaledX = std::round(static_cast<float>(valuePx.X.Value) * m_scalePxToDpf);
      const float scaledY = std::round(static_cast<float>(valuePx.Y.Value) * m_scalePxToDpf);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<DpPoint2::raw_value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<DpPoint2::raw_value_type>::max()));
      return DpPoint2::Create(static_cast<DpPoint2::raw_value_type>(scaledX), static_cast<DpPoint2::raw_value_type>(scaledY));
    }


    DpPoint2F ToDpPoint2F(const PxPoint2 valuePx) const noexcept
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return DpPoint2F::Create(static_cast<float>(valuePx.X.Value), static_cast<float>(valuePx.Y.Value));
      }
      const float scaledX = static_cast<float>(valuePx.X.Value) * m_scalePxToDpf;
      const float scaledY = static_cast<float>(valuePx.Y.Value) * m_scalePxToDpf;
      return DpPoint2F::Create(scaledX, scaledY);
    }


    DpSize1D ToDpSize1D(const PxSize1D sizePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return DpSize1D::Create(sizePx.RawValue(), OptimizationCheckFlag::NoCheck);
      }
      const float scaled = std::round(static_cast<float>(sizePx.RawValue()) * m_scalePxToDpf);
      assert(scaled >= 0.0f && scaled <= float(std::numeric_limits<DpSize2D::raw_value_type>::max()));
      return DpSize1D::Create(static_cast<DpSize2D::raw_value_type>(scaled));
    }

    DpSize1DF ToDpSize1DF(const PxSize1D sizePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return DpSize1DF::Create(static_cast<DpSize1DF::raw_value_type>(sizePx.RawValue()), OptimizationCheckFlag::NoCheck);
      }
      const float scaled = static_cast<float>(sizePx.RawValue()) * m_scalePxToDpf;
      assert(scaled >= 0.0f && scaled <= float(std::numeric_limits<DpSize2D::raw_value_type>::max()));
      return DpSize1DF::Create(scaled);
    }

    DpSize2D ToDpSize2D(const PxPoint2 valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return DpSize2D::Create(valuePx.X.Value, valuePx.Y.Value);
      }
      const float scaledX = std::round(static_cast<float>(valuePx.X.Value) * m_scalePxToDpf);
      const float scaledY = std::round(static_cast<float>(valuePx.Y.Value) * m_scalePxToDpf);
      assert(scaledX >= 0.0f && scaledX <= float(std::numeric_limits<DpSize2D::raw_value_type>::max()));
      assert(scaledY >= 0.0f && scaledY <= float(std::numeric_limits<DpSize2D::raw_value_type>::max()));
      return DpSize2D::Create(static_cast<DpSize2D::raw_value_type>(scaledX), static_cast<DpSize2D::raw_value_type>(scaledY));
    }

    DpSize2D ToDpSize2D(const PxSize2D valuePx) const
    {
      return {ToDpSize1D(valuePx.Width()), ToDpSize1D(valuePx.Height())};
    }

    DpSize2DF ToDpSize2DF(const PxSize2D valuePx) const
    {
      return {ToDpSize1DF(valuePx.Width()), ToDpSize1DF(valuePx.Height())};
    }

    DpExtent2D ToDpExtent2D(const PxExtent2D valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return DpExtent2D::Create(valuePx.Width.Value, valuePx.Height.Value);
      }
      auto scaledDpf =
        DpPoint2F::Create(static_cast<float>(valuePx.Width.Value) * m_scalePxToDpf, static_cast<float>(valuePx.Height.Value) * m_scalePxToDpf);
      return TypeConverter::UncheckedChangeTo<DpExtent2D>(scaledDpf);
    }

    DpThicknessU ToDpThicknessU(const PxThicknessU& valuePx) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return DpThicknessU::Create(valuePx.Left.Value, valuePx.Top.Value, valuePx.Right.Value, valuePx.Bottom.Value);
      }
      auto thicknessDpf =
        DpThicknessF::Create(static_cast<float>(valuePx.Left.Value) * m_scalePxToDpf, static_cast<float>(valuePx.Top.Value) * m_scalePxToDpf,
                             static_cast<float>(valuePx.Right.Value) * m_scalePxToDpf, static_cast<float>(valuePx.Bottom.Value) * m_scalePxToDpf);
      return TypeConverter::UncheckedChangeTo<DpThicknessU>(thicknessDpf);
    }


    inline int32_t ToPxInt32(const DpValue valueDp) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf)).Value;
    }

    inline int32_t ToPxInt32(const DpValueF valueDpf) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(valueDpf.Value * m_scaleDpToPxf)).Value;
    }

    inline int32_t ToPxInt32(const DpValueU valueDp) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf)).Value;
    }

    inline int32_t ToPxInt32(const DpSize1D valueDp) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(static_cast<float>(valueDp.RawValue()) * m_scaleDpToPxf)).Value;
    }

    inline int32_t ToPxInt32(const DpSize1DF valueDpf) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(valueDpf.RawValue() * m_scaleDpToPxf)).Value;
    }

    inline float ToPxRawFloat(const DpValueF valueDpf) const
    {
      return valueDpf.Value * m_scaleDpToPxf;
    }

    inline PxValue ToPxValue(const DpValue valueDp) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf));
    }

    inline PxValue ToPxValue(const DpValueF valueDpf) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(valueDpf.Value * m_scaleDpToPxf));
    }

    inline PxValue ToPxValue(const DpValueU valueDp) const
    {
      return TypeConverter::UncheckedChangeTo<PxValue>(PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf));
    }


    inline PxValueF ToPxValueF(const DpValue valueDp) const
    {
      return PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf);
    }

    inline PxValueF ToPxValueF(const DpValueF valueDpf) const
    {
      return PxValueF(valueDpf.Value * m_scaleDpToPxf);
    }

    inline PxValueF ToPxValueF(const DpValueU valueDp) const
    {
      return PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf);
    }


    PxValueU ToPxValueU(const DpValue valueDp) const
    {
      return TypeConverter::UncheckedChangeTo<PxValueU>(PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf));
    }

    PxValueU ToPxValueU(const DpValueF valueDpf) const
    {
      return TypeConverter::UncheckedChangeTo<PxValueU>(PxValueF(valueDpf.Value * m_scaleDpToPxf));
    }

    PxValueU ToPxValueU(const DpValueU valueDp) const
    {
      return TypeConverter::UncheckedChangeTo<PxValueU>(PxValueF(static_cast<float>(valueDp.Value) * m_scaleDpToPxf));
    }


    PxPoint2 ToPxPoint2(const DpPoint2 valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxPoint2::Create(valueDp.X.Value, valueDp.Y.Value);
      }
      return TypeConverter::UncheckedChangeTo<PxPoint2>(
        PxVector2::Create(static_cast<float>(valueDp.X.Value) * m_scaleDpToPxf, static_cast<float>(valueDp.Y.Value) * m_scaleDpToPxf));
    }

    PxPoint2 ToPxPoint2(const DpPoint2F valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return TypeConverter::UncheckedChangeTo<PxPoint2>(PxVector2::Create(valueDpf.X.Value, valueDpf.Y.Value));
      }
      return TypeConverter::UncheckedChangeTo<PxPoint2>(PxVector2::Create(valueDpf.X.Value * m_scaleDpToPxf, valueDpf.Y.Value * m_scaleDpToPxf));
    }

    PxPoint2 ToPxPoint2(const DpSize2D valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxPoint2::Create(valueDp.Width().RawValue(), valueDp.Height().RawValue());
      }
      return TypeConverter::UncheckedChangeTo<PxPoint2>(PxVector2::Create(static_cast<float>(valueDp.Width().RawValue()) * m_scaleDpToPxf,
                                                                          static_cast<float>(valueDp.Height().RawValue()) * m_scaleDpToPxf));
    }

    PxSize2D ToPxSize2D(const DpSize2D valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxSize2D::UncheckedCreate(valueDp.Width().RawValue(), valueDp.Height().RawValue());
      }
      return TypeConverter::UncheckedChangeTo<PxSize2D>(PxSize2DF::UncheckedCreate(static_cast<float>(valueDp.Width().RawValue()) * m_scaleDpToPxf,
                                                                                   static_cast<float>(valueDp.Height().RawValue()) * m_scaleDpToPxf));
    }

    PxSize1D ToPxSize1D(const DpSize1D valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxSize1D::Create(valueDp.Value().Value, OptimizationCheckFlag::NoCheck);
      }
      return TypeConverter::UncheckedChangeTo<PxSize1D>(PxSize1DF::Create(static_cast<float>(valueDp.Value().Value) * m_scaleDpToPxf));
    }


    PxSize1D ToPxSize1D(const DpSize1DF valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return TypeConverter::UncheckedChangeTo<PxSize1D>(PxSize1DF::UncheckedCreate(valueDpf.Value().Value));
      }
      return TypeConverter::UncheckedChangeTo<PxSize1D>(PxSize1DF::Create(valueDpf.Value().Value * m_scaleDpToPxf));
    }

    PxSize2D ToPxSize2D(const DpExtent2D valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return TypeConverter::UncheckedTo<PxSize2D>(PxExtent2D::Create(valueDp.Width.Value, valueDp.Height.Value));
      }
      return TypeConverter::UncheckedChangeTo<PxSize2D>(
        PxVector2::Create(static_cast<float>(valueDp.Width.Value) * m_scaleDpToPxf, static_cast<float>(valueDp.Height.Value) * m_scaleDpToPxf));
    }

    PxSize2D ToPxSize2D(const DpSize2DF valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return TypeConverter::UncheckedChangeTo<PxSize2D>(PxSize2DF::UncheckedCreate(valueDpf.RawWidth(), valueDpf.RawHeight()));
      }
      return TypeConverter::UncheckedChangeTo<PxSize2D>(
        PxSize2DF::UncheckedCreate(valueDpf.RawWidth() * m_scaleDpToPxf, valueDpf.RawHeight() * m_scaleDpToPxf));
    }

    PxSize1DF ToPxSize1DF(const DpSize1DF valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxSize1DF::UncheckedCreate(valueDpf.Value().Value);
      }
      return PxSize1DF::Create(valueDpf.Value().Value * m_scaleDpToPxf);
    }

    PxThickness ToPxThickness(const DpThickness& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxThickness::UncheckedCreate(valueDp.RawLeft(), valueDp.RawTop(), valueDp.RawRight(), valueDp.RawBottom());
      }
      return TypeConverter::UncheckedChangeTo<PxThickness>(
        PxThicknessF::Create(static_cast<float>(valueDp.RawLeft()) * m_scaleDpToPxf, static_cast<float>(valueDp.RawTop()) * m_scaleDpToPxf,
                             static_cast<float>(valueDp.RawRight()) * m_scaleDpToPxf, static_cast<float>(valueDp.RawBottom()) * m_scaleDpToPxf));
    }


    PxThickness ToPxThickness(const DpThicknessF& valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return TypeConverter::UncheckedChangeTo<PxThickness>(
          PxThicknessF::Create(valueDpf.RawLeft(), valueDpf.RawTop(), valueDpf.RawRight(), valueDpf.RawBottom()));
      }
      return TypeConverter::UncheckedChangeTo<PxThickness>(
        PxThicknessF::Create(valueDpf.RawLeft() * m_scaleDpToPxf, valueDpf.RawTop() * m_scaleDpToPxf, valueDpf.RawRight() * m_scaleDpToPxf,
                             valueDpf.RawBottom() * m_scaleDpToPxf));
    }

    inline PxThickness ToPxThickness(const DpThicknessU& valueDp) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxThickness::Create(UncheckedNumericCast<PxThickness::raw_value_type>(valueDp.Left.Value),
                                   UncheckedNumericCast<PxThickness::raw_value_type>(valueDp.Top.Value),
                                   UncheckedNumericCast<PxThickness::raw_value_type>(valueDp.Right.Value),
                                   UncheckedNumericCast<PxThickness::raw_value_type>(valueDp.Bottom.Value));
      }

      return TypeConverter::UncheckedChangeTo<PxThickness>(
        PxThicknessF::Create(static_cast<float>(valueDp.Left.Value) * m_scaleDpToPxf, static_cast<float>(valueDp.Top.Value) * m_scaleDpToPxf,
                             static_cast<float>(valueDp.Right.Value) * m_scaleDpToPxf, static_cast<float>(valueDp.Bottom.Value) * m_scaleDpToPxf));
    }

    PxVector2 ToPxVector2(const DpPoint2F valueDpf) const
    {
      if (m_densityDpi == SpriteDpConfig::BaseDpi)
      {
        return PxVector2::Create(valueDpf.X.Value, valueDpf.Y.Value);
      }
      return PxVector2::Create(valueDpf.X.Value * m_scaleDpToPxf, valueDpf.Y.Value * m_scaleDpToPxf);
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

    PxSize2D CalcScaledPxSize2D(const PxExtent2D extentPx, const uint32_t imageDpi) const;
    PxSize2DF CalcScaledPxSize2DF(const PxExtent2D extentPx, const uint32_t imageDpi) const;
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

    static DpExtent2D CalcImageDpExtent2D(const PxExtent2D imageExtentPx, const uint32_t imageDpi);
    static DpThicknessU CalcDpThicknessU(const PxThicknessU& thicknessPx, const uint32_t imageDpi);

    PxVector2 CalcScaledOffsetPxVector2(const PxPoint2 offsetPx, const uint32_t imageDpi) const;
    float CalcScaledOffsetValuePxf(const int32_t offsetPx, const uint32_t imageDpi) const;

  private:
  };
}

#endif
