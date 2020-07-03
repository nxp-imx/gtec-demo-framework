#ifndef FSLBASE_MATH_MATHHELPER_CALCOPTIMALSIZE_HPP
#define FSLBASE_MATH_MATHHELPER_CALCOPTIMALSIZE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/RectangleSizeRestrictionFlag.hpp>

namespace Fsl
{
  namespace MathHelper
  {
    //! @brief Find the optimal rectangle size for packing a given amount of uniform sized units under the supplied constraints
    //! @param unitSize the size of the unit (x > 0 && y > 0)
    //! @param unitCount the total number of units > 0
    Point2 CalcOptimalSize(const Point2& unitSize, const int32_t unitCount, const RectangleSizeRestrictionFlag restrictionFlags);

    //! @brief Find the optimal rectangle size for packing a given amount of uniform sized units under the supplied constraints
    //! @param unitSize the size of the unit (x > 0 && y > 0)
    //! @param unitCount the total number of units > 0
    PxSize2D CalcOptimalSize(const PxSize2D& unitSize, const int32_t unitCount, const RectangleSizeRestrictionFlag restrictionFlags);

    //! @brief Find the optimal rectangle size for packing a given amount of uniform sized units under the supplied constraints
    //! @param unitSize the size of the unit (x > 0 && y > 0)
    //! @param unitCount the total number of units > 0
    PxExtent2D CalcOptimalSize(const PxExtent2D& unitSize, const uint32_t unitCount, const RectangleSizeRestrictionFlag restrictionFlags);
  }
}


#endif
