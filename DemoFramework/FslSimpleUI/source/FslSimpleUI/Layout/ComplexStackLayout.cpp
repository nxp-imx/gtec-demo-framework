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

#include <FslSimpleUI/LayoutHelper.hpp>
#include <FslSimpleUI/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/PropertyTypeFlags.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>
#include <cmath>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {
    ComplexStackLayout::ComplexStackLayout(const std::shared_ptr<WindowContext>& context)
      : Layout(context)
      , m_orientation(LayoutOrientation::Vertical)
      , m_spacing(0)
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
      if (value != m_spacing)
      {
        m_spacing = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void ComplexStackLayout::ClearLayoutLengths()
    {
      if (m_layoutLength.size() > 0)
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
      if (m_layoutLength.size() <= 0)
      {
        FSLLOG_WARNING("There are not layouts to pop, request ignored");
        return;
      }

      m_layoutLength.pop_back();
      PropertyUpdated(PropertyType::Layout);
    }


    Vector2 ComplexStackLayout::ArrangeOverride(const Vector2& finalSize)
    {
      Vector2 resultSize;
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        resultSize = CalcFixedStarSizeHorizontal(m_finalLayout, finalSize);
        ArrangeHorizontal(m_finalLayout, finalSize.Y);
      }
      else
      {
        resultSize = CalcFixedStarSizeVertical(m_finalLayout, finalSize);
        ArrangeVertical(m_finalLayout, finalSize.X);
      }
      return resultSize;
    }


    Vector2 ComplexStackLayout::MeasureOverride(const Vector2& availableSize)
    {
      Vector2 desiredSize;
      Vector2 minSize;

      auto layoutLengthItr = m_layoutLength.begin();
      if (m_orientation == LayoutOrientation::Horizontal)
      {
        // Fake that we have unlimited space in X and keep Y constrained.
        const Vector2 fakeAvailableSize(LayoutHelper::InfiniteSpace, availableSize.Y);
        auto entries = DirectChildAccess();
        for (auto itr = entries.begin(); itr != entries.end(); ++itr)
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
          case LayoutUnitType::Star:
            itr->Window->Measure(fakeAvailableSize);
            desiredSize = itr->Window->DesiredSize();
            break;
          case LayoutUnitType::Fixed:
          {
            const Vector2 fixedAvailableSize(layoutLength.Value(), availableSize.Y);
            itr->Window->Measure(fixedAvailableSize);
            desiredSize = itr->Window->DesiredSize();
            desiredSize.X = layoutLength.Value();
            break;
          }
          default:
            FSLLOG_WARNING("Unsupported LayoutUnitType: " << (int32_t)layoutLength.UnitType());
            itr->Window->Measure(fakeAvailableSize);
            desiredSize = itr->Window->DesiredSize();
            break;
          }

          minSize.X += desiredSize.X + m_spacing;
          if (desiredSize.Y > minSize.Y)
            minSize.Y = desiredSize.Y;
        }
        if (entries.size() >= 1)
          minSize.X -= m_spacing;
      }
      else
      {

        // Fake that we have unlimited space in Y and keep X constrained.
        const Vector2 fakeAvailableSize(availableSize.X, LayoutHelper::InfiniteSpace);
        auto entries = DirectChildAccess();
        for (auto itr = entries.begin(); itr != entries.end(); ++itr)
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
          case LayoutUnitType::Star:
            itr->Window->Measure(fakeAvailableSize);
            desiredSize = itr->Window->DesiredSize();
            break;
          case LayoutUnitType::Fixed:
          {
            const Vector2 fixedAvailableSize(availableSize.X, layoutLength.Value());
            itr->Window->Measure(fixedAvailableSize);
            desiredSize = itr->Window->DesiredSize();
            desiredSize.Y = layoutLength.Value();
            break;
          }
          default:
            FSLLOG_WARNING("Unsupported LayoutUnitType: " << (int32_t)layoutLength.UnitType());
            itr->Window->Measure(fakeAvailableSize);
            desiredSize = itr->Window->DesiredSize();
            break;
          }
          desiredSize = itr->Window->DesiredSize();
          minSize.Y += desiredSize.Y + m_spacing;
          if (desiredSize.X > minSize.X)
            minSize.X = desiredSize.X;
        }
        if (entries.size() >= 1)
          minSize.Y -= m_spacing;
      }

      assert(!isinf(minSize.X));
      assert(!isinf(minSize.Y));
      assert(!isnan(minSize.X));
      assert(!isnan(minSize.Y));
      return minSize;
    }


    Vector2 ComplexStackLayout::CalcFixedStarSizeHorizontal(std::deque<FinalLayout>& rLayoutDefinition, const Vector2& finalSize)
    {
      auto layoutLengthItr = m_layoutLength.begin();
      rLayoutDefinition.clear();

      // Run through each element and give it the space it desired in Y, but only finalSize.X in X
      auto entries = DirectChildAccess();
      FinalLayout finalLayout;
      float totalStars = 0;
      float totalSize = 0;
      for (auto itr = entries.begin(); itr != entries.end(); ++itr)
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
          finalLayout.Size = itr->Window->DesiredSize().X;
          totalSize += finalLayout.Size;
          break;
        case LayoutUnitType::Fixed:
          finalLayout.Size = layoutLength.Value();
          totalSize += finalLayout.Size;
          break;
        case LayoutUnitType::Star:
        {
          totalStars += layoutLength.Value();
          finalLayout.Size = layoutLength.Value();
          break;
        }
        default:
          FSLLOG_WARNING("Unsupported LayoutUnitType: " << (int32_t)layoutLength.UnitType());
          break;
        }
        finalLayout.UnitType = layoutLength.UnitType();
        rLayoutDefinition.push_back(finalLayout);
        totalSize += m_spacing;
      }
      if (entries.size() >= 1)
        totalSize -= m_spacing;

      // We now know the total size and total stars
      const float sizeLeft = std::max(finalSize.X - totalSize, 0.0f);
      FinalizeStarSizes(rLayoutDefinition, sizeLeft, totalStars);
      if (finalSize.X >= totalSize && totalStars > 0)
        totalSize = finalSize.X;
      return Vector2(totalSize, finalSize.Y);
    }



    Vector2 ComplexStackLayout::CalcFixedStarSizeVertical(std::deque<FinalLayout>& rLayoutDefinition, const Vector2& finalSize)
    {
      auto layoutLengthItr = m_layoutLength.begin();
      rLayoutDefinition.clear();

      // Run through each element and give it the space it desired in Y, but only finalSize.X in X
      auto entries = DirectChildAccess();
      FinalLayout finalLayout;
      float totalStars = 0;
      float totalSize = 0;
      for (auto itr = entries.begin(); itr != entries.end(); ++itr)
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
          finalLayout.Size = itr->Window->DesiredSize().Y;
          break;
        case LayoutUnitType::Fixed:
          finalLayout.Size = layoutLength.Value();
          break;
        case LayoutUnitType::Star:
        {
          totalStars += layoutLength.Value();
          finalLayout.Size = layoutLength.Value();
          break;
        }
        default:
          FSLLOG_WARNING("Unsupported LayoutUnitType: " << (int32_t)layoutLength.UnitType());
          break;
        }
        finalLayout.UnitType = layoutLength.UnitType();
        rLayoutDefinition.push_back(finalLayout);
        totalSize += m_spacing;
      }
      if (entries.size() >= 1)
        totalSize -= m_spacing;

      // We now know the total size and total stars
      const float sizeLeft = std::max(finalSize.Y - totalSize, 0.0f);
      FinalizeStarSizes(rLayoutDefinition, sizeLeft, totalStars);
      if (finalSize.X >= totalSize && totalStars > 0)
        totalSize = finalSize.Y;
      return Vector2(finalSize.X, totalSize);
    }


    void ComplexStackLayout::FinalizeStarSizes(std::deque<FinalLayout>& rLayoutDefinition, const float spaceLeft, const float totalStars)
    {
      // Assign size to the star areas
      float position = 0;
      for (auto itr = rLayoutDefinition.begin(); itr != rLayoutDefinition.end(); ++itr)
      {
        if (itr->UnitType == LayoutUnitType::Star)
          itr->Size = (totalStars / itr->Size) * spaceLeft;
        itr->Position = position;
        position += itr->Size + m_spacing;
      }
    }


    void ComplexStackLayout::ArrangeHorizontal(const std::deque<FinalLayout>& layoutDefinition, const float finalSizeY)
    {
      // Run through each element and give it the space it desired in X, but only finalSize.Y in Y
      auto entries = DirectChildAccess();
      assert(entries.size() == layoutDefinition.size());
      auto itrLayout = layoutDefinition.begin();
      for (auto itr = entries.begin(); itr != entries.end(); ++itr)
      {
        itr->Window->Arrange(Rect(itrLayout->Position, 0, itrLayout->Size, finalSizeY));
        ++itrLayout;
      }
    }



    void ComplexStackLayout::ArrangeVertical(const std::deque<FinalLayout>& layoutDefinition, const float finalSizeX)
    {
      // Run through each element and give it the space it desired in X, but only finalSize.Y in Y
      auto entries = DirectChildAccess();
      assert(entries.size() == layoutDefinition.size());
      auto itrLayout = layoutDefinition.begin();
      for (auto itr = entries.begin(); itr != entries.end(); ++itr)
      {
        itr->Window->Arrange(Rect(0, itrLayout->Position, finalSizeX, itrLayout->Size));
        ++itrLayout;
      }
    }
  }

}
