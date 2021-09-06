/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslSimpleUI/Base/Layout/UniformWrapLayout.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace UI
  {
    UniformWrapLayout::UniformWrapLayout(const std::shared_ptr<BaseWindowContext>& context)
      : SimpleLayout(context)
      , m_orientation(LayoutOrientation::Vertical)
    {
    }

    void UniformWrapLayout::SetLayoutOrientation(const LayoutOrientation& value)
    {
      if (value != m_orientation)
      {
        m_orientation = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }

    void UniformWrapLayout::SetSpacing(const DpPointF& valueDp)
    {
      if (valueDp != m_spacingDp)
      {
        m_spacingDp = valueDp;
        PropertyUpdated(PropertyType::Layout);
      }
    }

    PxSize2D UniformWrapLayout::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      if (empty())
      {
        return Layout::ArrangeOverride(finalSizePx);
      }
      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
      auto spacingPx = unitConverter.ToPxPoint2(m_spacingDp);


      if (m_orientation == LayoutOrientation::Horizontal)
      {
        const PxPoint2 addPx(m_elementUniformSizePx + spacingPx);
        PxPoint2 positionPx;
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Arrange(PxRectangle(positionPx, m_elementUniformSizePx));
          positionPx.X += addPx.X;
          if ((positionPx.X + m_elementUniformSizePx.Width()) > finalSizePx.Width())
          {
            positionPx.X = 0;
            positionPx.Y += addPx.Y;
          }
        }
      }
      else
      {
        const PxPoint2 addPx(m_elementUniformSizePx + spacingPx);
        PxPoint2 positionPx;
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Arrange(PxRectangle(positionPx, m_elementUniformSizePx));
          positionPx.Y += addPx.Y;
          if ((positionPx.Y + m_elementUniformSizePx.Height()) > finalSizePx.Height())
          {
            positionPx.X += addPx.X;
            positionPx.Y = 0;
          }
        }
      }
      return finalSizePx;
    }


    PxSize2D UniformWrapLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      if (empty())
      {
        return Layout::MeasureOverride(availableSizePx);
      }

      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

      MeasureResult res;
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // If we are supplied with infinity we behave like a stack panel
        if (!availableSizePx.IsInfinityWidth())
        {
          auto spacingPx = unitConverter.ToPxPoint2(m_spacingDp);
          res = MeasureHorizontalWrapLayout(begin(), end(), spacingPx, availableSizePx);
        }
        else
        {
          auto spacingPx = unitConverter.DpToPxInt32(m_spacingDp.X);
          res = MeasureHorizontalStackLayout(begin(), end(), spacingPx, availableSizePx);
        }
      }
      else
      {
        // If we are supplied with infinity we behave like a stack panel
        if (!availableSizePx.IsInfinityHeight())
        {
          auto spacingPx = unitConverter.ToPxPoint2(m_spacingDp);
          res = MeasureVerticalWrapLayout(begin(), end(), spacingPx, availableSizePx);
        }
        else
        {
          auto spacingPx = unitConverter.DpToPxInt32(m_spacingDp.Y);
          res = MeasureVerticalStackLayout(begin(), end(), spacingPx, availableSizePx);
        }
      }

      m_elementUniformSizePx = res.ElementUniformSizePx;
      return res.MeasuredSizePx;
    }


    UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureHorizontalStackLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                     const collection_type::queue_type::const_iterator& itrEnd,
                                                                                     const int32_t spacingXPx, const PxAvailableSize& availableSizePx)
    {
      assert(itrBegin != itrEnd);

      PxSize2D finalSizePx;
      PxSize2D maxElementSizePx;
      if (itrBegin != itrEnd)    // if not empty
      {
        // Fake that we have unlimited space in X and keep Y constrained.
        const PxAvailableSize fakeAvailableSizePx(PxAvailableSizeUtil::InfiniteSpacePx, availableSizePx.Height());

        // During this pass we measure all elements to find the max size.
        for (auto itr = itrBegin; itr != itrEnd; ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          maxElementSizePx.SetMax(desiredSizePx);
        }

        const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));
        finalSizePx = PxSize2D((maxElementSizePx.Width() * childCount) + (spacingXPx * (childCount - 1)), maxElementSizePx.Height());
      }
      return {finalSizePx, maxElementSizePx};
    }


    UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureVerticalStackLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                   const collection_type::queue_type::const_iterator& itrEnd,
                                                                                   const int32_t spacingYPx, const PxAvailableSize& availableSizePx)
    {
      assert(itrBegin != itrEnd);

      PxSize2D finalSizePx;
      PxSize2D maxElementSizePx;
      if (itrBegin != itrEnd)    // if not empty
      {
        // Fake that we have unlimited space in Y and keep X constrained.
        const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSizeUtil::InfiniteSpacePx);

        // During this pass we measure all elements to find the max size.
        for (auto itr = itrBegin; itr != itrEnd; ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          maxElementSizePx.SetMax(desiredSizePx);
        }

        const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));
        finalSizePx = PxSize2D(maxElementSizePx.Width(), (maxElementSizePx.Height() * childCount) + (spacingYPx * (childCount - 1)));
      }
      return {finalSizePx, maxElementSizePx};
    }


    UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureHorizontalWrapLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                    const collection_type::queue_type::const_iterator& itrEnd,
                                                                                    const PxPoint2& spacingPx, const PxAvailableSize& availableSizePx)
    {
      assert(itrBegin != itrEnd);

      PxSize2D finalSizePx;
      PxSize2D maxElementSizePx;
      if (itrBegin != itrEnd)    // if not empty
      {
        // Fake that we have unlimited space in X and keep Y constrained.
        const PxAvailableSize fakeAvailableSizePx(PxAvailableSizeUtil::InfiniteSpacePx, availableSizePx.Height());

        // During this pass we measure all elements to find the max size.
        for (auto itr = itrBegin; itr != itrEnd; ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          maxElementSizePx.SetMax(desiredSizePx);
        }
        const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));

        const int32_t maxWidthForElementsPx = availableSizePx.Width() + spacingPx.X;

        // Figure out how many elements that can fit per line with the given spacing
        const int32_t maxElementsPerLine =
          maxElementSizePx.Width() > 0 ? std ::max(maxWidthForElementsPx / (maxElementSizePx.Width() + spacingPx.X), 1) : 1;
        int32_t columns = 0;
        int32_t rows = 0;
        if (childCount <= maxElementsPerLine)
        {
          columns = childCount;
          rows = 1;
        }
        else
        {
          columns = maxElementsPerLine;
          rows = (childCount / maxElementsPerLine) + ((childCount % maxElementsPerLine) > 0 ? 1 : 0);
        }

        assert(columns >= 1);
        assert(rows >= 1);
        const int32_t finalWidth = (maxElementSizePx.Width() * columns) + (spacingPx.X * (columns - 1));
        const int32_t finalHeight = (maxElementSizePx.Height() * rows) + (spacingPx.Y * (rows - 1));
        assert(finalWidth >= maxElementSizePx.Width());
        assert(finalHeight >= maxElementSizePx.Height());
        finalSizePx = PxSize2D(finalWidth, finalHeight);
      }
      return {finalSizePx, maxElementSizePx};
    }


    UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureVerticalWrapLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                  const collection_type::queue_type::const_iterator& itrEnd,
                                                                                  const PxPoint2& spacingPx, const PxAvailableSize& availableSizePx)
    {
      assert(itrBegin != itrEnd);

      PxSize2D finalSizePx;
      PxSize2D maxElementSizePx;
      if (itrBegin != itrEnd)    // if not empty
      {
        // Fake that we have unlimited space in Y and keep X constrained.
        const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSizeUtil::InfiniteSpacePx);

        // During this pass we measure all elements to find the max size.
        for (auto itr = itrBegin; itr != itrEnd; ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          maxElementSizePx.SetMax(desiredSizePx);
        }
        const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));

        const int32_t maxHeightPxForElements = availableSizePx.Height() + spacingPx.Y;

        // Figure out how many elements that can fit per line with the given spacing
        const int32_t maxElementsPerLine =
          maxElementSizePx.Height() > 0 ? std ::max(maxHeightPxForElements / (maxElementSizePx.Height() + spacingPx.Y), 1) : 1;
        int32_t columns = 0;
        int32_t rows = 0;
        if (childCount <= maxElementsPerLine)
        {
          columns = 1;
          rows = childCount;
        }
        else
        {
          columns = (childCount / maxElementsPerLine) + ((childCount % maxElementsPerLine) > 0 ? 1 : 0);
          rows = maxElementsPerLine;
        }

        assert(columns >= 1);
        assert(rows >= 1);
        const int32_t finalWidth = (maxElementSizePx.Width() * columns) + (spacingPx.X * (columns - 1));
        const int32_t finalHeight = (maxElementSizePx.Height() * rows) + (spacingPx.Y * (rows - 1));
        assert(finalWidth >= maxElementSizePx.Width());
        assert(finalHeight >= maxElementSizePx.Height());
        finalSizePx = PxSize2D(finalWidth, finalHeight);
      }
      return {finalSizePx, maxElementSizePx};
    }
  }
}
