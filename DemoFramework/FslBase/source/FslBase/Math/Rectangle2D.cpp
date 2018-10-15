/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Rectangle2D.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  Rectangle2D::Rectangle2D(const int32_t left, const int32_t top, const int32_t right, const int32_t bottom, const bool reserved)
    : Offset(left, top)
    , Extent(static_cast<Extent2D::element_type>(right - left), static_cast<Extent2D::element_type>(bottom - top))
  {
    static_assert(sizeof(Extent2D::element_type) >= sizeof(int32_t), "for the below check to work this has to be true");

    if (left > right || top > bottom)
    {
      throw std::invalid_argument("arguments out of range");
    }
  }


  void Rectangle2D::Inflate(const int32_t horizontalValue, const int32_t verticalValue)
  {
    if (horizontalValue <= 0 || verticalValue <= 0)
    {
      throw std::invalid_argument("");
    }

    Offset.X -= horizontalValue;
    Offset.Y -= verticalValue;
    Extent.Width += horizontalValue * 2;
    Extent.Height += verticalValue * 2;
  }


  Rectangle2D Rectangle2D::Intersect(const Rectangle2D& rect1, const Rectangle2D& rect2)
  {
    if (rect1.Intersects(rect2))
    {
      const int32_t rightSide = std::min(rect1.Right(), rect2.Right());
      const int32_t leftSide = std::max(rect1.Left(), rect2.Left());
      const int32_t topSide = std::max(rect1.Top(), rect2.Top());
      const int32_t bottomSide = std::min(rect1.Bottom(), rect2.Bottom());
      return Rectangle2D(leftSide, topSide, rightSide, bottomSide, true);
    }
    return Rectangle2D();
  }


  Rectangle2D Rectangle2D::Union(const Rectangle2D& rect1, const Rectangle2D& rect2)
  {
    const int32_t x = std::min(rect1.Offset.X, rect2.Offset.X);
    const int32_t y = std::min(rect1.Offset.Y, rect2.Offset.Y);
    const int32_t x2 = std::max(rect1.Right(), rect2.Right());
    const int32_t y2 = std::max(rect1.Bottom(), rect2.Bottom());
    return Rectangle2D(x, y, x2, y2, true);
  }
}
