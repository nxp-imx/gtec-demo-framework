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

#include <FslSimpleUI/Base/Layout/CanvasLayout.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <algorithm>
#include <cmath>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {
    CanvasLayout::CanvasLayout(const std::shared_ptr<BaseWindowContext>& context)
      : ComplexLayout<CanvasLayoutWindowRecord>(context)
    {
    }


    void CanvasLayout::SetChildPosition(const std::shared_ptr<BaseWindow>& window, const DpPointF& positionDp)
    {
      auto itr = std::find_if(begin(), end(), [window](const collection_type::record_type& record) { return record.Window == window; });
      if (itr == end())
      {
        throw NotFoundException("The window is not part of this layout");
      }

      itr->PositionDp = positionDp;
    }


    PxSize2D CanvasLayout::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        auto positionPx = unitConverter.ToPxPoint2(itr->PositionDp);
        itr->Window->Arrange(PxRectangle(positionPx, finalSizePx));
      }

      return finalSizePx;
    }


    PxSize2D CanvasLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Measure(availableSizePx);
      }

      // The canvas content does not affect its measurements.
      return {};
    }
  }
}
