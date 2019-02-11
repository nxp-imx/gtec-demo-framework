/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace MathHelper
  {
    namespace
    {
      inline Point2 CalcOptimalSizePow2(const int32_t totalArea, const Point2& unitSize, const int32_t unitCount)
      {
        int32_t newSize = MathHelper::ToPowerOfTwo(static_cast<int32_t>(std::ceil(::sqrt(totalArea))));

        const int32_t unitsX = newSize / unitSize.X;
        const int32_t unitsY = newSize / unitSize.Y;

        int32_t newSizeY = newSize;
        if ((unitsX * unitsY) < unitCount)
        {
          newSize = MathHelper::ToPowerOfTwo((unitsX + 1) * unitSize.X);
        }
        else if (((unitsX * unitsY) / 2) >= unitCount)
        {
          newSizeY /= 2;
        }
        return Point2(newSize, newSizeY);
      }


      inline Point2 CalcOptimalSizeSquare(const int32_t totalArea, const Point2& unitSize, const int32_t unitCount)
      {
        auto newSize = static_cast<int32_t>(std::ceil(std::sqrt(totalArea)));

        const int32_t unitsX = newSize / unitSize.X;
        const int32_t unitsY = newSize / unitSize.Y;
        if ((unitsX * unitsY) < unitCount)
        {
          int32_t result1 = (unitsX + 1) * unitSize.X;
          int32_t result2 = (unitsY + 1) * unitSize.Y;
          int32_t result = std::min(result1, result2);
          return Point2(result, result);
        }

        int32_t result = unitsX * unitSize.X;
        return Point2(result, result);
      }


      inline Point2 CalcOptimalSizeSquarePow2(const int32_t totalArea, const Point2& unitSize, const int32_t unitCount)
      {
        int32_t newSize = MathHelper::ToPowerOfTwo(static_cast<int32_t>(std::ceil(std::sqrt(totalArea))));

        const int32_t unitsX = newSize / unitSize.X;
        const int32_t unitsY = newSize / unitSize.Y;
        if ((unitsX * unitsY) < unitCount)
        {
          newSize = MathHelper::ToPowerOfTwo((unitsX + 1) * unitSize.X);
        }
        return Point2(newSize, newSize);
      }
    }

    float DistBetweenAngles(const float from, const float to)
    {
      float delta = to - from;
      const float deltaMod = (std::floor((std::abs(delta) / RADS360))) * RADS360;
      delta += (delta >= 0.0f) ? -deltaMod : deltaMod;
      if (delta > 0)
      {
        return delta <= RADS180 ? delta : -(RADS360 - delta);
      }
      return delta >= -RADS180 ? delta : (RADS360 + delta);
    }


    int ToPowerOfTwo(const int value)
    {
      assert(value >= 0);
      if (value > 0)
      {
        int tmpValue = value;
        --tmpValue;
        tmpValue |= (tmpValue >> 1);
        tmpValue |= (tmpValue >> 2);
        tmpValue |= (tmpValue >> 4);
        tmpValue |= (tmpValue >> 8);
        tmpValue |= (tmpValue >> 16);
        ++tmpValue;    // Val is now the next highest power of 2.
        return tmpValue;
      }
      return 1;
    }


    float WrapAngle(const float angle)
    {
      if ((angle > -MathHelper::PI) && (angle <= MathHelper::PI))
      {
        return angle;
      }

      using namespace std;
      auto result = static_cast<float>(remainder(static_cast<double>(angle), 6.2831854820251465));
      if (result <= -3.14159274f)
      {
        return result + 6.28318548f;
      }
      if (result > 3.14159274f)
      {
        return result - 6.28318548f;
      }
      return result;
    }


    Point2 CalcOptimalSize(const Point2& unitSize, const int32_t unitCount, const RectangleSizeRestrictionFlag::Enum restrictionFlags)
    {
      assert(unitSize.X > 0);
      assert(unitSize.Y > 0);
      assert(unitCount > 0);

      const int areaOfChar = unitSize.X * unitSize.Y;
      const int totalArea = areaOfChar * unitCount;

      Point2 result;
      if ((restrictionFlags & RectangleSizeRestrictionFlag::Power2) == 0)    // Not pow 2
      {
        if ((restrictionFlags & RectangleSizeRestrictionFlag::Square) == 0)    // not square
        {
          result = Point2(unitSize.X * unitCount, unitSize.Y);
        }
        else
        {
          result = CalcOptimalSizeSquare(totalArea, unitSize, unitCount);
        }
      }
      else
      {
        if ((restrictionFlags & RectangleSizeRestrictionFlag::Square) == 0)    // not square
        {
          result = CalcOptimalSizePow2(totalArea, unitSize, unitCount);
        }
        else
        {
          result = CalcOptimalSizeSquarePow2(totalArea, unitSize, unitCount);
        }
      }
      assert(((result.X / unitSize.X) * (result.Y / unitSize.Y)) >= unitCount);
      return result;
    }
  }
}
