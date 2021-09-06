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

#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace UI
  {
    UniformStackLayout::UniformStackLayout(const std::shared_ptr<BaseWindowContext>& context)
      : SimpleLayout(context)
      , m_orientation(LayoutOrientation::Vertical)
      , m_spacingDp(0)
    {
    }


    void UniformStackLayout::SetLayoutOrientation(const LayoutOrientation& value)
    {
      if (value != m_orientation)
      {
        m_orientation = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void UniformStackLayout::SetSpacing(const float& value)
    {
      if (value != m_spacingDp)
      {
        m_spacingDp = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    PxSize2D UniformStackLayout::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
      const auto spacePx = unitConverter.DpToPxInt32(m_spacingDp);
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Run through each element and give it the space it desired in X, but only finalSize.Y in Y
        int32_t posPx = 0;
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Arrange(PxRectangle(posPx, 0, m_maxSizePx, finalSizePx.Height()));
          posPx += m_maxSizePx + spacePx;
        }
        if (!empty())
        {
          posPx -= spacePx;
        }
        return {posPx, finalSizePx.Height()};
      }


      // Run through each element and give it the space it desired in Y, but only finalSize.X in X
      int32_t posPx = 0;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Arrange(PxRectangle(0, posPx, finalSizePx.Width(), m_maxSizePx));
        posPx += m_maxSizePx + spacePx;
      }
      if (!empty())
      {
        posPx -= spacePx;
      }
      return {finalSizePx.Width(), posPx};
    }


    PxSize2D UniformStackLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

      const auto spacePx = unitConverter.DpToPxInt32(m_spacingDp);
      PxSize2D finalSizePx;
      PxSize2D maxSizePx;
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Fake that we have unlimited space in X and keep Y constrained.
        const PxAvailableSize fakeAvailableSizePx(PxAvailableSizeUtil::InfiniteSpacePx, availableSizePx.Height());
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          maxSizePx.SetMax(desiredSizePx);
        }
        if (!empty())
        {
          const auto childCount = UncheckedNumericCast<int32_t>(GetChildCount());
          finalSizePx = PxSize2D((maxSizePx.Width() * childCount) + (spacePx * (childCount - 1)), maxSizePx.Height());
        }
        m_maxSizePx = maxSizePx.Width();
      }
      else
      {
        // Fake that we have unlimited space in Y and keep X constrained.
        const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSizeUtil::InfiniteSpacePx);
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          maxSizePx.SetMax(desiredSizePx);
        }
        if (!empty())
        {
          const auto childCount = UncheckedNumericCast<int32_t>(GetChildCount());
          finalSizePx = PxSize2D(maxSizePx.Width(), (maxSizePx.Height() * childCount) + (spacePx * (childCount - 1)));
        }
        m_maxSizePx = maxSizePx.Height();
      }
      return finalSizePx;
    }
  }
}
