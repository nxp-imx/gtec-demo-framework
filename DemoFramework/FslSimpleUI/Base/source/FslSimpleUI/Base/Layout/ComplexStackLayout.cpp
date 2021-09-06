/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/PxAvailableSizeUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace UI
  {
    ComplexStackLayout::ComplexStackLayout(const std::shared_ptr<BaseWindowContext>& context)
      : ComplexLayout<ComplexStackLayoutWindowRecord>(context)
      , m_orientation(LayoutOrientation::Vertical)
    {
    }

    void ComplexStackLayout::SetLayoutOrientation(const LayoutOrientation& value)
    {
      if (value != m_orientation)
      {
        m_orientation = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void ComplexStackLayout::SetSpacing(const float& value)
    {
      if (value != m_spacingDp)
      {
        m_spacingDp = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void ComplexStackLayout::ClearLayoutLengths()
    {
      if (!m_layoutLength.empty())
      {
        m_layoutLength.clear();
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void ComplexStackLayout::PushLayoutLength(const LayoutLength& layoutLength)
    {
      m_layoutLength.push_back(layoutLength);
      PropertyUpdated(PropertyType::Layout);
    }


    void ComplexStackLayout::PopLayoutLength()
    {
      if (m_layoutLength.empty())
      {
        FSLLOG3_WARNING("There are not layouts to pop, request ignored");
        return;
      }

      m_layoutLength.pop_back();
      PropertyUpdated(PropertyType::Layout);
    }


    PxSize2D ComplexStackLayout::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      PxSize2D resultSizePx;
      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        resultSizePx = CalcFixedStarSizeHorizontal(unitConverter, finalSizePx);
        ArrangeHorizontal(finalSizePx.Height());
      }
      else
      {
        resultSizePx = CalcFixedStarSizeVertical(unitConverter, finalSizePx);
        ArrangeVertical(finalSizePx.Width());
      }
      return resultSizePx;
    }


    PxSize2D ComplexStackLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      PxSize2D desiredSizePx;
      PxPoint2 minSizePx;

      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

      auto layoutLengthItr = m_layoutLength.begin();
      const int32_t spacingPx = unitConverter.DpToPxInt32(m_spacingDp);
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Fake that we have unlimited space in X and keep Y constrained.
        const PxAvailableSize fakeAvailableSizePx(PxAvailableSizeUtil::InfiniteSpacePx, availableSizePx.Height());
        LayoutLength layoutLength;
        for (auto itr = begin(); itr != end(); ++itr)
        {
          if (layoutLengthItr != m_layoutLength.end())
          {
            layoutLength = *layoutLengthItr;
            ++layoutLengthItr;
          }

          switch (layoutLength.UnitType())
          {
          case LayoutUnitType::Auto:
          case LayoutUnitType::Star:
            itr->Window->Measure(fakeAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            break;
          case LayoutUnitType::Fixed:
          {
            const int32_t fixedLayoutLengthPx = unitConverter.DpToPxInt32(layoutLength.Value());
            const PxAvailableSize fixedAvailableSizePx(fixedLayoutLengthPx, availableSizePx.Height());
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetWidth(fixedLayoutLengthPx);
            break;
          }
          case LayoutUnitType::Px:
          {
            const int32_t fixedLayoutLengthPx = unitConverter.PxfToPxInt32(layoutLength.Value());
            const PxAvailableSize fixedAvailableSizePx(fixedLayoutLengthPx, availableSizePx.Height());
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetWidth(fixedLayoutLengthPx);
            break;
          }
          default:
            FSLLOG3_WARNING("Unsupported LayoutUnitType: {}", static_cast<int32_t>(layoutLength.UnitType()));
            itr->Window->Measure(fakeAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            break;
          }
          minSizePx.X += desiredSizePx.Width() + spacingPx;
          if (desiredSizePx.Height() > minSizePx.Y)
          {
            minSizePx.Y = desiredSizePx.Height();
          }
        }
        if (!empty())
        {
          minSizePx.X -= spacingPx;
        }
      }
      else
      {
        // Fake that we have unlimited space in Y and keep X constrained.
        const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSizeUtil::InfiniteSpacePx);
        LayoutLength layoutLength;
        for (auto itr = begin(); itr != end(); ++itr)
        {
          if (layoutLengthItr != m_layoutLength.end())
          {
            layoutLength = *layoutLengthItr;
            ++layoutLengthItr;
          }

          switch (layoutLength.UnitType())
          {
          case LayoutUnitType::Auto:
          case LayoutUnitType::Star:
            itr->Window->Measure(fakeAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            break;
          case LayoutUnitType::Fixed:
          {
            const int32_t fixedLayoutLengthPx = unitConverter.DpToPxInt32(layoutLength.Value());
            const PxAvailableSize fixedAvailableSizePx(availableSizePx.Width(), fixedLayoutLengthPx);
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetHeight(fixedLayoutLengthPx);
            break;
          }
          case LayoutUnitType::Px:
          {
            const int32_t fixedLayoutLengthPx = unitConverter.PxfToPxInt32(layoutLength.Value());
            const PxAvailableSize fixedAvailableSizePx(availableSizePx.Width(), fixedLayoutLengthPx);
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetHeight(fixedLayoutLengthPx);
            break;
          }
          default:
            FSLLOG3_WARNING("Unsupported LayoutUnitType: {0}", static_cast<int32_t>(layoutLength.UnitType()));
            itr->Window->Measure(fakeAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            break;
          }
          minSizePx.Y += desiredSizePx.Height() + spacingPx;
          if (desiredSizePx.Width() > minSizePx.X)
          {
            minSizePx.X = desiredSizePx.Width();
          }
        }
        if (!empty())
        {
          minSizePx.Y -= spacingPx;
        }
      }

      assert(PxAvailableSizeUtil::IsNormalValue(minSizePx.X));
      assert(PxAvailableSizeUtil::IsNormalValue(minSizePx.Y));
      return TypeConverter::UncheckedTo<PxSize2D>(minSizePx);
    }


    PxSize2D ComplexStackLayout::CalcFixedStarSizeHorizontal(const SpriteUnitConverter& unitConverter, const PxSize2D& finalSizePx)
    {
      auto layoutLengthItr = m_layoutLength.begin();

      const auto spacingPx = unitConverter.DpToPxInt32(m_spacingDp);

      // Run through each element and give it the space it desired in Y, but only finalSize.X in X
      float totalStars = 0;
      int32_t totalSizePx = 0;
      LayoutLength layoutLength{};
      for (auto itr = begin(); itr != end(); ++itr)
      {
        if (layoutLengthItr != m_layoutLength.end())
        {
          layoutLength = *layoutLengthItr;
          ++layoutLengthItr;
        }

        switch (layoutLength.UnitType())
        {
        case LayoutUnitType::Auto:
          itr->SizePx = itr->Window->DesiredSizePx().Width();
          itr->LayoutSizeMagic = 0;
          totalSizePx += itr->SizePx;
          break;
        case LayoutUnitType::Fixed:
          itr->SizePx = unitConverter.DpToPxInt32(layoutLength.Value());
          itr->LayoutSizeMagic = 0;
          totalSizePx += itr->SizePx;
          break;
        case LayoutUnitType::Px:
          itr->SizePx = unitConverter.PxfToPxInt32(layoutLength.Value());
          itr->LayoutSizeMagic = 0;
          totalSizePx += itr->SizePx;
          break;
        case LayoutUnitType::Star:
        {
          totalStars += layoutLength.Value();
          itr->SizePx = 0;    // this will be filled later by FinalizePositionAndStarSizes
          itr->LayoutSizeMagic = layoutLength.Value();
          break;
        }
        default:
          FSLLOG3_WARNING("Unsupported LayoutUnitType: {}", static_cast<int32_t>(layoutLength.UnitType()));
          break;
        }
        itr->UnitType = layoutLength.UnitType();
        totalSizePx += spacingPx;
      }
      if (!empty())
      {
        totalSizePx -= spacingPx;
      }

      // We now know the total size and total stars
      const int32_t sizeLeftPx = std::max(finalSizePx.Width() - totalSizePx, 0);
      FinalizePositionAndStarSizes(unitConverter, sizeLeftPx, totalStars);
      if (finalSizePx.Width() >= totalSizePx && totalStars > 0)
      {
        totalSizePx = finalSizePx.Width();
      }
      return {totalSizePx, finalSizePx.Height()};
    }


    PxSize2D ComplexStackLayout::CalcFixedStarSizeVertical(const SpriteUnitConverter& unitConverter, const PxSize2D& finalSizePx)
    {
      auto layoutLengthItr = m_layoutLength.begin();

      const auto spacingPx = unitConverter.DpToPxInt32(m_spacingDp);

      // Run through each element and give it the space it desired in Y, but only finalSize.X in X
      float totalStars = 0;
      int32_t totalSizePx = 0;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        LayoutLength layoutLength;
        if (layoutLengthItr != m_layoutLength.end())
        {
          layoutLength = *layoutLengthItr;
          ++layoutLengthItr;
        }

        switch (layoutLength.UnitType())
        {
        case LayoutUnitType::Auto:
          itr->SizePx = itr->Window->DesiredSizePx().Height();
          itr->LayoutSizeMagic = 0.0f;
          totalSizePx += itr->SizePx;
          break;
        case LayoutUnitType::Fixed:
          itr->SizePx = unitConverter.DpToPxInt32(layoutLength.Value());
          itr->LayoutSizeMagic = 0.0f;
          totalSizePx += itr->SizePx;
          break;
        case LayoutUnitType::Px:
          itr->SizePx = unitConverter.PxfToPxInt32(layoutLength.Value());
          itr->LayoutSizeMagic = 0.0f;
          totalSizePx += itr->SizePx;
          break;
        case LayoutUnitType::Star:
        {
          totalStars += layoutLength.Value();
          itr->SizePx = 0;    // this will be filled later by FinalizePositionAndStarSizes
          itr->LayoutSizeMagic = layoutLength.Value();
          break;
        }
        default:
          FSLLOG3_WARNING("Unsupported LayoutUnitType: {}", static_cast<int32_t>(layoutLength.UnitType()));
          break;
        }
        itr->UnitType = layoutLength.UnitType();
        totalSizePx += spacingPx;
      }
      if (!empty())
      {
        totalSizePx -= spacingPx;
      }

      // We now know the total size and total stars
      const int32_t sizeLeftPx = std::max(finalSizePx.Height() - totalSizePx, 0);
      FinalizePositionAndStarSizes(unitConverter, sizeLeftPx, totalStars);
      if (finalSizePx.Width() >= totalSizePx && totalStars > 0)
      {
        totalSizePx = finalSizePx.Height();
      }
      return {finalSizePx.Width(), totalSizePx};
    }


    void ComplexStackLayout::FinalizePositionAndStarSizes(const SpriteUnitConverter& unitConverter, const int32_t spaceLeftPx, const float totalStars)
    {
      // Assign size to the star areas
      const auto spacingPx = unitConverter.DpToPxInt32(m_spacingDp);
      int32_t positionPx = 0;
      int32_t maxSpaceLeftPx = spaceLeftPx;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        if (itr->UnitType == LayoutUnitType::Star)
        {
          assert(totalStars > 0.0f);
          const auto finalSizePxf = std::round((itr->LayoutSizeMagic / totalStars) * static_cast<float>(spaceLeftPx));
          const auto finalSizePx = std::min(static_cast<int32_t>(finalSizePxf), maxSpaceLeftPx);
          itr->SizePx = finalSizePx;
          assert(maxSpaceLeftPx >= finalSizePx);
          maxSpaceLeftPx -= finalSizePx;
        }
        itr->PositionPx = positionPx;
        positionPx += itr->SizePx + spacingPx;
      }
    }


    void ComplexStackLayout::ArrangeHorizontal(const int32_t finalSizeYPx)
    {
      // Run through each element and give it the space it desired in X, but only finalSizeY in Y
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Arrange(PxRectangle(itr->PositionPx, 0, itr->SizePx, finalSizeYPx));
      }
    }


    void ComplexStackLayout::ArrangeVertical(const int32_t finalSizeXPx)
    {
      // Run through each element and give it the space it desired in Y, but only finalSizeX in X
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Arrange(PxRectangle(0, itr->PositionPx, finalSizeXPx, itr->SizePx));
      }
    }
  }
}
