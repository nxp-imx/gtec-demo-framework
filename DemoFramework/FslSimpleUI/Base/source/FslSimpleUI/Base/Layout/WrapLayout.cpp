/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslSimpleUI/Base/LayoutHelper.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <cassert>
#include <cmath>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {
    WrapLayout::WrapLayout(const std::shared_ptr<BaseWindowContext>& context)
      : Layout(context)
      , m_orientation(LayoutOrientation::Vertical)
    {
    }


    void WrapLayout::WinInit()
    {
      Layout::WinInit();
      auto uiContext = GetContext()->TheUIContext.Get();
      m_children.SYS_WinInit(this, uiContext->WindowManager);
    }


    void WrapLayout::SetLayoutOrientation(const LayoutOrientation& value)
    {
      if (value != m_orientation)
      {
        m_orientation = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }

    void WrapLayout::SetSpacing(const Vector2& value)
    {
      if (value != m_spacing)
      {
        m_spacing = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }

    Vector2 WrapLayout::ArrangeOverride(const Vector2& finalSize)
    {
      if (m_children.empty())
      {
        return Layout::ArrangeOverride(finalSize);
      }

      for (auto itr = m_children.begin(); itr != m_children.end(); ++itr)
      {
        const auto desired = itr->Window->DesiredSize();
        itr->Window->Arrange(Rect(itr->Position.X, itr->Position.Y, desired.X, desired.Y));
      }

      assert(!isinf(finalSize.X));
      assert(!isinf(finalSize.Y));
      assert(!isnan(finalSize.X));
      assert(!isnan(finalSize.Y));
      return finalSize;
    }


    Vector2 WrapLayout::MeasureOverride(const Vector2& availableSize)
    {
      if (m_children.empty())
      {
        return Layout::MeasureOverride(availableSize);
      }

      Vector2 minSize;
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // If we are supplied with infinity we behave like a stack panel
        if (!isinf(availableSize.X))
        {
          minSize = MeasureHorizontalWrapLayout(m_children, m_spacing, availableSize);
        }
        else
        {
          minSize = MeasureHorizontalStackLayout(m_children, m_spacing.X, availableSize);
        }
      }
      else
      {
        // If we are supplied with infinity we behave like a stack panel
        if (!isinf(availableSize.Y))
        {
          minSize = MeasureVerticalWrapLayout(m_children, m_spacing, availableSize);
        }
        else
        {
          minSize = MeasureVerticalStackLayout(m_children, m_spacing.Y, availableSize);
        }
      }

      assert(!isinf(minSize.X));
      assert(!isinf(minSize.Y));
      assert(!isnan(minSize.X));
      assert(!isnan(minSize.Y));
      return minSize;
    }


    Vector2 WrapLayout::MeasureHorizontalStackLayout(collection_type& rEntries, const float spacingX, const Vector2& availableSize)
    {
      assert(!rEntries.empty());
      Vector2 minSize;
      float pos = 0;
      for (auto itr = rEntries.begin(); itr != rEntries.end(); ++itr)
      {
        itr->Window->Measure(availableSize);
        auto desiredSize = itr->Window->DesiredSize();
        if (desiredSize.Y > minSize.Y)
        {
          minSize.Y = desiredSize.Y;
        }

        itr->Position = Vector2(pos, 0);
        pos += desiredSize.X + spacingX;
      }
      minSize.X = pos - spacingX;
      return minSize;
    }


    Vector2 WrapLayout::MeasureVerticalStackLayout(collection_type& rEntries, const float spacingY, const Vector2& availableSize)
    {
      assert(!rEntries.empty());

      Vector2 minSize;
      float pos = 0;
      for (auto itr = rEntries.begin(); itr != rEntries.end(); ++itr)
      {
        itr->Window->Measure(availableSize);
        auto desiredSize = itr->Window->DesiredSize();
        itr->Position = Vector2(0, pos);
        if (desiredSize.X > minSize.X)
        {
          minSize.X = desiredSize.X;
        }
        pos += desiredSize.Y + spacingY;
      }
      minSize.Y = pos - spacingY;
      return minSize;
    }


    Vector2 WrapLayout::MeasureHorizontalWrapLayout(collection_type& rEntries, const Vector2& spacing, const Vector2& availableSize)
    {
      assert(!rEntries.empty());
      // Process the first element to simplify the loop
      const auto itrEnd = rEntries.end();
      auto itr = rEntries.begin();
      float rowWidth;
      itr->Window->Measure(availableSize);
      auto desiredSize = itr->Window->DesiredSize();
      rowWidth = desiredSize.X;
      float rowHeight = desiredSize.Y;
      itr->Position = Vector2(0, 0);
      float posX = desiredSize.X;
      float maxRowWidth = rowWidth;

      float posY = 0;
      ++itr;
      while (itr != itrEnd)
      {
        itr->Window->Measure(availableSize);
        desiredSize = itr->Window->DesiredSize();

        // Check if we need to force a line switch
        rowWidth += spacing.X + desiredSize.X;
        if (rowWidth > availableSize.X)
        {
          // ok we exceeded the available space so we wrap and
          // insert the current item as the first item in the new row
          if (posX > maxRowWidth)
          {
            maxRowWidth = posX;
          }

          posY += spacing.Y + rowHeight;
          rowHeight = desiredSize.Y;
          rowWidth = desiredSize.X;
          itr->Position = Vector2(0.0f, posY);
        }
        else if (desiredSize.Y > rowHeight)
        {
          // Keep track of the largest desired height we encounter
          rowHeight = desiredSize.Y;
          itr->Position = Vector2(posX + spacing.X, posY);
        }
        else
        {
          itr->Position = Vector2(posX + spacing.X, posY);
        }
        posX = rowWidth;

        ++itr;
      }
      if (rowWidth > maxRowWidth)
      {
        maxRowWidth = rowWidth;
      }
      posY += rowHeight;

      return Vector2(maxRowWidth, posY);
    }


    Vector2 WrapLayout::MeasureVerticalWrapLayout(collection_type& rEntries, const Vector2& spacing, const Vector2& availableSize)
    {
      assert(!rEntries.empty());

      // Process the first element to simplify the loop
      const auto itrEnd = rEntries.end();
      auto itr = rEntries.begin();
      float rowHeight;
      itr->Window->Measure(availableSize);
      auto desiredSize = itr->Window->DesiredSize();
      rowHeight = desiredSize.Y;
      float rowWidth = desiredSize.X;
      itr->Position = Vector2(0, 0);
      float posY = desiredSize.Y;
      float maxRowHeight = rowHeight;

      float posX = 0;
      ++itr;
      while (itr != itrEnd)
      {
        itr->Window->Measure(availableSize);
        desiredSize = itr->Window->DesiredSize();

        // Check if we need to force a line switch
        rowHeight += spacing.Y + desiredSize.Y;
        if (rowHeight > availableSize.Y)
        {
          // ok we exceeded the available space so we wrap and
          // insert the current item as the first item in the new row
          if (posY > maxRowHeight)
          {
            maxRowHeight = posY;
          }

          posX += spacing.X + rowWidth;
          rowWidth = desiredSize.X;
          rowHeight = desiredSize.Y;
          itr->Position = Vector2(posX, 0.0f);
        }
        else if (desiredSize.X > rowWidth)
        {
          // Keep track of the largest desired width we encounter
          rowWidth = desiredSize.X;
          itr->Position = Vector2(posX, posY + spacing.Y);
        }
        else
        {
          itr->Position = Vector2(posX, posY + spacing.Y);
        }
        posY = rowHeight;

        ++itr;
      }
      if (rowHeight > maxRowHeight)
      {
        maxRowHeight = rowHeight;
      }
      posX += rowWidth;

      return Vector2(posX, maxRowHeight);
    }
  }
}
