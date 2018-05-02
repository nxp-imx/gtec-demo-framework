#ifndef FSLSIMPLEUI_BASE_LAYOUT_COMPLEXSTACKLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_COMPLEXSTACKLAYOUT_HPP
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

#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <FslSimpleUI/Base/Layout/LayoutLength.hpp>
#include <FslSimpleUI/Base/WindowCollection/GenericWindowCollection.hpp>
#include <deque>

namespace Fsl
{
  namespace UI
  {
    class ComplexStackLayout : public Layout
    {
      struct FinalLayout : GenericWindowCollectionRecordBase
      {
        LayoutUnitType UnitType;
        float Position;
        float Size;

        FinalLayout(const std::shared_ptr<BaseWindow>& window)
          : GenericWindowCollectionRecordBase(window)
          , UnitType(LayoutUnitType::Auto)
          , Position(0)
          , Size()
        {
        }
      };

      using collection_type = GenericWindowCollection<FinalLayout>;
      collection_type m_children;

      LayoutOrientation m_orientation;
      std::deque<LayoutLength> m_layoutLength;
      float m_spacing;

    public:
      ComplexStackLayout(const std::shared_ptr<WindowContext>& context);
      virtual void WinInit() override;

      virtual void ClearChildren() override
      {
        m_children.Clear();
      }

      virtual void AddChild(const std::shared_ptr<BaseWindow>& window) override
      {
        m_children.Add(window);
      }

      virtual void RemoveChild(const std::shared_ptr<BaseWindow>& window) override
      {
        m_children.Remove(window);
      }

      LayoutOrientation GetLayoutOrientation() const
      {
        return m_orientation;
      }
      void SetLayoutOrientation(const LayoutOrientation& value);

      float GetSpacing() const
      {
        return m_spacing;
      }
      void SetSpacing(const float& value);

      void ClearLayoutLengths();
      void PushLayoutLength(const LayoutLength& layoutLength);
      void PopLayoutLength();
    protected:
      virtual Vector2 ArrangeOverride(const Vector2& finalSize) override;
      virtual Vector2 MeasureOverride(const Vector2& availableSize) override;
    private:
      Vector2 CalcFixedStarSizeHorizontal(const Vector2& finalSize);
      Vector2 CalcFixedStarSizeVertical(const Vector2& finalSize);
      void FinalizeStarSizes(const float spaceLeft, const float totalStars);
      void ArrangeHorizontal(const float finalSizeY);
      void ArrangeVertical(const float finalSizeX);
    };
  }
}

#endif
