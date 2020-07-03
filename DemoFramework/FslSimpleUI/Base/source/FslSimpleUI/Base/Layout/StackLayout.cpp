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

#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
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
    StackLayout::StackLayout(const std::shared_ptr<BaseWindowContext>& context)
      : SimpleLayout(context)
      , m_orientation(LayoutOrientation::Vertical)
      , m_spacingDp(0)
    {
    }


    void StackLayout::SetLayoutOrientation(const LayoutOrientation& value)
    {
      if (value != m_orientation)
      {
        m_orientation = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void StackLayout::SetSpacing(const float& value)
    {
      if (value != m_spacingDp)
      {
        m_spacingDp = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    PxSize2D StackLayout::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
      const auto spacePx = unitConverter.DpToPxInt32(m_spacingDp);
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Run through each element and give it the space it desired in X, but only finalSize.Y in Y
        int32_t posPx = 0;
        int32_t elementDesiredXPx = 0;
        for (auto itr = begin(); itr != end(); ++itr)
        {
          elementDesiredXPx = itr->Window->DesiredSizePx().Width();
          itr->Window->Arrange(PxRectangle(posPx, 0, elementDesiredXPx, finalSizePx.Height()));
          posPx += elementDesiredXPx + spacePx;
        }
        if (!empty())
        {
          posPx -= spacePx;
        }
        return {posPx, finalSizePx.Height()};
      }


      // Run through each element and give it the space it desired in Y, but only finalSize.X in X
      int32_t posPx = 0;
      int32_t elementDesiredYPx = 0;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        elementDesiredYPx = itr->Window->DesiredSizePx().Height();
        itr->Window->Arrange(PxRectangle(0, posPx, finalSizePx.Width(), elementDesiredYPx));
        posPx += elementDesiredYPx + spacePx;
      }
      if (!empty())
      {
        posPx -= spacePx;
      }
      return {finalSizePx.Width(), posPx};
    }


    PxSize2D StackLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      PxPoint2 minSizePx;

      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

      const auto spacePx = unitConverter.DpToPxInt32(m_spacingDp);
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Fake that we have unlimited space in X and keep Y constrained.
        const PxAvailableSize fakeAvailableSizePx(PxAvailableSizeUtil::InfiniteSpacePx, availableSizePx.Height());
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          minSizePx.X += desiredSizePx.Width() + spacePx;
          if (desiredSizePx.Height() > minSizePx.Y)
          {
            minSizePx.Y = desiredSizePx.Height();
          }
        }
        if (!empty())
        {
          minSizePx.X -= spacePx;
        }
      }
      else
      {
        // Fake that we have unlimited space in Y and keep X constrained.
        const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSizeUtil::InfiniteSpacePx);
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Measure(fakeAvailableSizePx);
          PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
          minSizePx.Y += desiredSizePx.Height() + spacePx;
          if (desiredSizePx.Width() > minSizePx.X)
          {
            minSizePx.X = desiredSizePx.Width();
          }
        }
        if (!empty())
        {
          minSizePx.Y -= spacePx;
        }
      }

      return TypeConverter::UncheckedTo<PxSize2D>(minSizePx);
    }
  }
}
