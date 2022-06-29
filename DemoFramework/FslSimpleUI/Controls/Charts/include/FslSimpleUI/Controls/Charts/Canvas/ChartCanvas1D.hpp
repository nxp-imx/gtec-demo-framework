#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_CANVAS_CHARTCANVAS1D_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_CANVAS_CHARTCANVAS1D_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Math/MinMax.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <memory>

namespace Fsl
{
  struct PxRectangle;

  namespace UI
  {
    class ChartDataView;
    struct PxAvailableSize;

    // FIX: this should not deal with anything 2d, so things like orientation and 2d scaling+scaling policy should not be done here

    //! Maps a canvas coordinate set to DP coordinates which can then be mapped to the pixel grid (PX).
    class ChartCanvas1D
    {
      struct ArrangeCache
      {
        Vector2 LayoutScale{1, 1};

        constexpr ArrangeCache() noexcept = default;
        constexpr explicit ArrangeCache(const Vector2 layoutScale) noexcept
          : LayoutScale(layoutScale)
        {
        }
      };

      struct LayoutCache
      {
        // Local instance that need to be kept in sync
        SpriteUnitConverter UnitConverter;
        PxSize1D SizeOfOneDpInPixels;

        ArrangeCache Arrange;

        explicit LayoutCache(const uint32_t densityDpi)
          : UnitConverter(densityDpi)
          , SizeOfOneDpInPixels(UnitConverter.ToPxSize1D(DpSize1D::Create(1)))
          , Arrange(Vector2(1.0f, 1.0f))
        {
        }

        bool SetDensityDpi(const uint32_t densityDpi);
      };

      MinMax<uint32_t> m_axisRange;
      LayoutOrientation m_orientation{LayoutOrientation::Vertical};
      LayoutCache m_layoutCache;


    public:
      explicit ChartCanvas1D(const SpriteUnitConverter& unitConverter);

      MinMax<uint32_t> GetAxisRange() const noexcept
      {
        return m_axisRange;
      }

      bool SetAxisRange(const MinMax<uint32_t> value) noexcept
      {
        const bool changed = value != m_axisRange;
        if (changed)
        {
          m_axisRange = value;
        }
        return changed;
      }


      LayoutOrientation GetLayoutOrientation() const
      {
        return m_orientation;
      }

      bool SetOrientation(const LayoutOrientation value);


      //! @brief  Transforms a value on the canvas x axis to its pixel coordinate
      //! @note   Cant be noexcept because std::round is not noexcept
      inline PxValue CanvasAxisToPx(const int32_t value) const
      {
        return m_layoutCache.UnitConverter.ToPxValue(CanvasAxisToDp(value));
      }

      //! @brief  Transforms a value on the canvas x axis to its pixel coordinate
      //! @note   Cant be noexcept because std::round is not noexcept
      inline PxValue CanvasAxisToPx(const uint32_t value) const
      {
        return m_layoutCache.UnitConverter.ToPxValue(CanvasAxisToDp(value));
      }

      //! @brief  Transforms a value on the canvas x axis to its pixel coordinate
      //! @note   Cant be noexcept because std::round is not noexcept
      inline PxValue CanvasAxisToPx(const float value) const
      {
        return m_layoutCache.UnitConverter.ToPxValue(CanvasAxisToDp(value));
      }

      //! @brief  Transforms a value on the canvas x axis to its pixel coordinate
      //! @note   Cant be noexcept because std::round is not noexcept
      inline PxValue CanvasAxisToPx(const double value) const
      {
        return m_layoutCache.UnitConverter.ToPxValue(CanvasAxisToDp(value));
      }

      inline DpValueF CanvasAxisToDp(const int32_t value) const noexcept
      {
        return DpValueF(static_cast<float>(value) * m_layoutCache.Arrange.LayoutScale.X);
      }


      inline DpValueF CanvasAxisToDp(const uint32_t value) const noexcept
      {
        return DpValueF(static_cast<float>(value) * m_layoutCache.Arrange.LayoutScale.X);
      }


      inline DpValueF CanvasAxisToDp(const float value) const noexcept
      {
        return DpValueF(value * m_layoutCache.Arrange.LayoutScale.X);
      }

      inline DpValueF CanvasAxisToDp(const double value) const noexcept
      {
        return DpValueF(static_cast<float>(value * m_layoutCache.Arrange.LayoutScale.X));
      }

      inline PxSize1D SizeOfOneDpInPixels() const noexcept
      {
        return m_layoutCache.SizeOfOneDpInPixels;
      }

      PxSize2D Arrange(const PxSize2D finalSizePx, const PxSize1D itemHeightPx);
      PxSize2D Arrange(const PxSize2D finalSizePx, const PxSize1D itemHeightPx, const PxSize1D spacingPx, const uint8_t channels);
      PxSize2D Measure(const PxAvailableSize availableSizePx, const PxSize1D itemHeightPx);
      PxSize2D Measure(const PxAvailableSize availableSizePx, const PxSize1D itemHeightPx, const PxSize1D spacingPx, const uint16_t entries);

      void OnResolutionChanged(const SpriteUnitConverter& unitConverter);

    private:
      PxSize1D CalculateAxisMinimumPixelSize() const;
    };
  }
}

#endif
