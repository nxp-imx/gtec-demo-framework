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

#include <FslSimpleUI/Base/LayoutHelper.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>
#include <cmath>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {
    StackLayout::StackLayout(const std::shared_ptr<BaseWindowContext>& context)
      : SimpleLayout(context)
      , m_orientation(LayoutOrientation::Vertical)
      , m_spacing(0)
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
      if (value != m_spacing)
      {
        m_spacing = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    Vector2 StackLayout::ArrangeOverride(const Vector2& finalSize)
    {
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Run through each element and give it the space it desired in X, but only finalSize.Y in Y
        float pos = 0;
        float elementDesiredX;
        for (auto itr = begin(); itr != end(); ++itr)
        {
          elementDesiredX = itr->Window->DesiredSize().X;
          itr->Window->Arrange(Rect(pos, 0, elementDesiredX, finalSize.Y));
          pos += elementDesiredX + m_spacing;
        }
        if (!empty())
        {
          pos -= m_spacing;
        }
        return Vector2(pos, finalSize.Y);
      }


      // Run through each element and give it the space it desired in Y, but only finalSize.X in X
      float pos = 0;
      float elementDesiredY;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        elementDesiredY = itr->Window->DesiredSize().Y;
        itr->Window->Arrange(Rect(0, pos, finalSize.X, elementDesiredY));
        pos += elementDesiredY + m_spacing;
      }
      if (!empty())
      {
        pos -= m_spacing;
      }
      return Vector2(finalSize.X, pos);
    }


    Vector2 StackLayout::MeasureOverride(const Vector2& availableSize)
    {
      Vector2 desiredSize;
      Vector2 minSize;

      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Fake that we have unlimited space in X and keep Y constrained.
        const Vector2 fakeAvailableSize(LayoutHelper::InfiniteSpace, availableSize.Y);
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Measure(fakeAvailableSize);
          desiredSize = itr->Window->DesiredSize();
          minSize.X += desiredSize.X + m_spacing;
          if (desiredSize.Y > minSize.Y)
          {
            minSize.Y = desiredSize.Y;
          }
        }
        if (!empty())
        {
          minSize.X -= m_spacing;
        }
      }
      else
      {
        // Fake that we have unlimited space in Y and keep X constrained.
        const Vector2 fakeAvailableSize(availableSize.X, LayoutHelper::InfiniteSpace);
        for (auto itr = begin(); itr != end(); ++itr)
        {
          itr->Window->Measure(fakeAvailableSize);
          desiredSize = itr->Window->DesiredSize();
          minSize.Y += desiredSize.Y + m_spacing;
          if (desiredSize.X > minSize.X)
          {
            minSize.X = desiredSize.X;
          }
        }
        if (!empty())
        {
          minSize.Y -= m_spacing;
        }
      }

      assert(!isinf(minSize.X));
      assert(!isinf(minSize.Y));
      assert(!isnan(minSize.X));
      assert(!isnan(minSize.Y));
      return minSize;
    }
  }
}
