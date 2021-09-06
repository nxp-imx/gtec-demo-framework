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

#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/EqualHelper_PxVector2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      const float CONSIDER_POSITIVE_INFINITY = std::numeric_limits<float>::max();

      inline float CorrectFloatingPointErrors(const float value, const float correctMaxValue)
      {
        return !EqualHelper::IsAlmostEqual(value, correctMaxValue) ? value : correctMaxValue;
      }

      inline PxVector2 CalcSizeKeepAspectRatioScaling(const PxVector2& targetSize, const PxVector2& srcSize)
      {
        assert(targetSize.X >= 0.0f);
        assert(targetSize.Y >= 0.0f);
        assert(targetSize.X < CONSIDER_POSITIVE_INFINITY);
        assert(targetSize.Y < CONSIDER_POSITIVE_INFINITY);
        assert(srcSize.X >= 0.0f);
        assert(srcSize.Y >= 0.0f);
        assert(srcSize.X < CONSIDER_POSITIVE_INFINITY);
        assert(srcSize.Y < CONSIDER_POSITIVE_INFINITY);

        if (srcSize.X < 0.00001f || srcSize.Y < 0.00001f)
        {
          return {};
        }
        if (targetSize.X <= 0.00001f || targetSize.Y <= 0.00001f)
        {
          return {};
        }

        // m = dy / dx
        // y = m * x
        // x = y / m
        float m = srcSize.Y / srcSize.X;

        float calcedX = targetSize.Y / m;
        float calcedY = m * targetSize.X;
        if (calcedX <= 0.0001f || calcedY <= 0.000f)
        {
          return {};
        }

        PxVector2 size1(calcedX, targetSize.Y);
        PxVector2 size2(targetSize.X, calcedY);
        // since we keep the aspect ratio the scaling factors will always be uniform
        return (size1.LengthSquared() < size2.LengthSquared() ? size1 : size2);
      }


      inline PxVector2 CalcSizeKeepAspectRatioDownScaling(const PxVector2& targetSize, const PxVector2& srcSize)
      {
        assert(targetSize.X >= 0.0f);
        assert(targetSize.Y >= 0.0f);
        assert(targetSize.X < CONSIDER_POSITIVE_INFINITY);
        assert(targetSize.Y < CONSIDER_POSITIVE_INFINITY);
        assert(srcSize.X > 0.0f);
        assert(srcSize.Y > 0.0f);
        assert(srcSize.X < CONSIDER_POSITIVE_INFINITY);
        assert(srcSize.Y < CONSIDER_POSITIVE_INFINITY);

        if (srcSize.X > targetSize.X)
        {
          if (srcSize.Y > targetSize.Y)
          {
            // Both X,Y is too large so make it fit
            return CalcSizeKeepAspectRatioScaling(targetSize, srcSize);
          }

          // Source Y fits but X is too large
          const float newScale = targetSize.X / srcSize.X;
          float newY = srcSize.Y * newScale;
          return {targetSize.X, CorrectFloatingPointErrors(newY, targetSize.Y)};
        }
        if (srcSize.Y > targetSize.Y)
        {
          // Source X fits but Y is too large
          const float newScale = targetSize.Y / srcSize.Y;
          float newX = srcSize.X * newScale;
          return {CorrectFloatingPointErrors(newX, targetSize.X), targetSize.Y};
        }

        // Source X, Y fits so do nothing
        return srcSize;
      }
    }


    bool UIScaleUtil::TryCalcScaling(PxVector2& rScaling, const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return TryCalcScaling(rScaling, targetSize, PxVector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    bool UIScaleUtil::TryCalcScaling(PxVector2& rScaling, const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      rScaling = PxVector2();
      if (srcSize.X <= 0.0f || srcSize.Y <= 0.0f)
      {
        return false;
      }

      PxVector2 newSize;
      if (!TryCalcSize(newSize, targetSize, srcSize, scalePolicy))
      {
        return false;
      }

      rScaling = !EqualHelper::IsAlmostEqual(newSize, srcSize) ? PxVector2((newSize.X / srcSize.X), (newSize.Y / srcSize.Y)) : PxVector2(1.0f, 1.0f);
      return true;
    }


    bool UIScaleUtil::TryCalcSize(PxVector2& rSize, const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return TryCalcSize(rSize, targetSize, PxVector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    bool UIScaleUtil::TryCalcSize(PxVector2& rSize, const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      rSize = PxVector2();
      if (srcSize.X <= 0.0f || srcSize.Y <= 0.0f)
      {
        return false;
      }

      PxVector2 size = CalcSize(targetSize, srcSize, scalePolicy);
      if (size.X < 0 || size.Y < 0)
      {
        return false;
      }

      rSize = size;
      return true;
    }

    bool UIScaleUtil::TryCalcSize(PxPoint2& rSize, const PxPoint2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return TryCalcSize(rSize, targetSize, TypeConverter::To<PxVector2>(srcSize), scalePolicy);
    }

    bool UIScaleUtil::TryCalcSize(PxPoint2& rSize, const PxPoint2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      PxVector2 scaledSize;
      if (TryCalcSize(scaledSize, TypeConverter::To<PxVector2>(targetSize), srcSize, scalePolicy))
      {
        rSize = TypeConverter::UncheckedChangeTo<PxPoint2>(scaledSize);
        return true;
      }
      rSize = {};
      return false;
    }

    bool UIScaleUtil::TryCalcSize(PxSize2D& rSize, const PxSize2D& targetSize, const PxSize2D& srcSize, const ItemScalePolicy scalePolicy)
    {
      PxPoint2 scaledSize;
      if (TryCalcSize(scaledSize, TypeConverter::To<PxPoint2>(targetSize), TypeConverter::To<PxPoint2>(srcSize), scalePolicy))
      {
        rSize = TypeConverter::To<PxSize2D>(scaledSize);
        return true;
      }
      rSize = {};
      return false;
    }


    PxVector2 UIScaleUtil::CalcScaling(const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return CalcScaling(targetSize, PxVector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    PxVector2 UIScaleUtil::CalcScaling(const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      // Early abort in case of crazy input
      if (srcSize.X <= 0.0f || srcSize.Y <= 0.0f)
      {
        return {};
      }

      const PxVector2 newSize = CalcSize(targetSize, srcSize, scalePolicy);

      return !EqualHelper::IsAlmostEqual(newSize, srcSize) ? PxVector2((newSize.X / srcSize.X), (newSize.Y / srcSize.Y)) : PxVector2(1.0f, 1.0f);
    }


    PxVector2 UIScaleUtil::CalcSize(const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return CalcSize(targetSize, PxVector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    PxVector2 UIScaleUtil::CalcSize(const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      assert(!isnan(targetSize.X));
      assert(!isnan(targetSize.Y));
      assert(!isnan(srcSize.X));
      assert(!isnan(srcSize.Y));
      assert(targetSize.X >= 0.0f);
      assert(targetSize.Y >= 0.0f);
      assert(targetSize.X < CONSIDER_POSITIVE_INFINITY);
      assert(targetSize.Y < CONSIDER_POSITIVE_INFINITY);
      assert(srcSize.X < CONSIDER_POSITIVE_INFINITY);
      assert(srcSize.Y < CONSIDER_POSITIVE_INFINITY);

      const PxVector2 clampedTarget(std::max(targetSize.X, 0.0f), std::max(targetSize.Y, 0.0f));
      const PxVector2 clampedSrc(srcSize.X <= 0.0f ? 0.0f : srcSize.X, srcSize.Y <= 0.0f ? 0.0f : srcSize.Y);

      // Early abort for no scaling
      if (clampedTarget == clampedSrc)
      {
        return clampedTarget;
      }

      switch (scalePolicy)
      {
      case ItemScalePolicy::NoScaling:
        return srcSize;
      case ItemScalePolicy::Fit:
        return clampedTarget;
      case ItemScalePolicy::Downscale:
        return {std::min(clampedTarget.X, clampedSrc.X), std::min(clampedTarget.Y, clampedSrc.Y)};
      case ItemScalePolicy::FitKeepAR:
        return CalcSizeKeepAspectRatioScaling(clampedTarget, srcSize);
      case ItemScalePolicy::DownscaleKeepAR:
        return CalcSizeKeepAspectRatioDownScaling(clampedTarget, srcSize);
      default:
        FSLLOG3_WARNING("Unsupported scaling policy, using src size");
        return srcSize;
      }
    }
  }
}
