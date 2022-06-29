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

#include <FslBase/Exceptions.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
//#include <FslBase/Log/Log3Fmt.hpp>
//#include <FslBase/Log/Math/FmtVector2.hpp>
#include <cmath>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl::UI
{
  FillLayout::FillLayout(const std::shared_ptr<BaseWindowContext>& context)
    : SimpleLayout(context)
  {
    SetAlignmentX(ItemAlignment::Stretch);
    SetAlignmentY(ItemAlignment::Stretch);
  }


  PxSize2D FillLayout::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    // FSLLOG3_INFO("Arrange: finalSize: {}", finalSize);
    for (auto itr = begin(); itr != end(); ++itr)
    {
      itr->Window->Arrange(PxRectangle(0, 0, finalSizePx.Width(), finalSizePx.Height()));
      // FSLLOG3_INFO("Arrange: RenderSize: {}", itr->Window->RenderSize());
    }
    return finalSizePx;
  }


  PxSize2D FillLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    if (availableSizePx.ContainsInfinity())
    {
      PxSize2D maxSize;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Measure(availableSizePx);
        maxSize = PxSize2D::Max(maxSize, itr->Window->DesiredSizePx());
      }
      if (availableSizePx.IsInfinityWidth())
      {
        if (availableSizePx.IsInfinityHeight())
        {
          return maxSize;
        }
        return {maxSize.Width(), availableSizePx.Height()};
      }
      // height must be infinity since the width isnt
      assert(availableSizePx.IsInfinityHeight());
      return {availableSizePx.Width(), maxSize.Height()};
    }

    if (!availableSizePx.IsNormal())
    {
      throw UsageErrorException("FillLayout got invalid availableSize");
    }

    // FSLLOG3_INFO("Measure: availableSize: {}", availableSize);
    for (auto itr = begin(); itr != end(); ++itr)
    {
      itr->Window->Measure(availableSizePx);
      // FSLLOG3_INFO("Measure: DesiredSize: {}", itr->Window->DesiredSize());
    }
    return availableSizePx.ToPxSize2D();
  }
}
