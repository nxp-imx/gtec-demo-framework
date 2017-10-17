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
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Log/Log.hpp>
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

      inline Vector2 CalcSizeKeepAspectRatioScaling(const Vector2& targetSize, const Vector2& srcSize)
      {
        assert(targetSize.X >= 0.0f);
        assert(targetSize.Y >= 0.0f);
        assert(targetSize.X < CONSIDER_POSITIVE_INFINITY);
        assert(targetSize.Y < CONSIDER_POSITIVE_INFINITY);
        assert(srcSize.X >= 0.0f);
        assert(srcSize.Y >= 0.0f);
        assert(srcSize.X < CONSIDER_POSITIVE_INFINITY);
        assert(srcSize.Y < CONSIDER_POSITIVE_INFINITY);

        if (srcSize.X < 0.00001f || srcSize.X < 0.00001f)
          return Vector2();
        if( targetSize.X <= 0.00001f || targetSize.Y <= 0.00001f )
          return Vector2();

        // m = dy / dx
        // y = m * x
        // x = y / m
        float m = srcSize.Y / srcSize.X;

        float calcedX = targetSize.Y / m;
        float calcedY = m * targetSize.X;
        if (calcedX <= 0.0001f || calcedY <= 0.000f )
          return Vector2();

        Vector2 size1(calcedX, targetSize.Y);
        Vector2 size2(targetSize.X, calcedY);
        // since we keep the aspect ratio the scaling factors will always be uniform
        return (size1.LengthSquared() < size2.LengthSquared() ? size1 : size2);
      }


      inline Vector2 CalcSizeKeepAspectRatioDownScaling(const Vector2& targetSize, const Vector2& srcSize)
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
          else
          {
            // Source Y fits but X is too large
            const float newScale = targetSize.X / srcSize.X;
            float newY = srcSize.Y * newScale;
            return Vector2(targetSize.X, CorrectFloatingPointErrors(newY, targetSize.Y));
          }
        }
        else if (srcSize.Y > targetSize.Y)
        {
          // Source X fits but Y is too large
          const float newScale = targetSize.Y / srcSize.Y;
          float newX = srcSize.X * newScale;
          return Vector2(CorrectFloatingPointErrors(newX, targetSize.X), targetSize.Y);
        }
        else
        {
          // Source X, Y fits so do nothing
          return srcSize;
        }
      }
    }


    bool UIScaleUtil::TryCalcScaling(Vector2& rScaling, const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return TryCalcScaling(rScaling, targetSize, Vector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    bool UIScaleUtil::TryCalcScaling(Vector2& rScaling, const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      rScaling = Vector2();
      if (srcSize.X <= 0.0f || srcSize.Y <= 0.0f)
        return false;

      Vector2 newSize;
      if (!TryCalcSize(newSize, targetSize, srcSize, scalePolicy))
        return false;

      rScaling = !EqualHelper::IsAlmostEqual(newSize, srcSize) ? Vector2((newSize.X / srcSize.X), (newSize.Y / srcSize.Y)) : Vector2(1.0f, 1.0f);
      return true;
    }


    bool UIScaleUtil::TryCalcSize(Vector2& rSize, const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return TryCalcSize(rSize, targetSize, Vector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    bool UIScaleUtil::TryCalcSize(Vector2& rSize, const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      rSize = Vector2();
      if (srcSize.X <= 0.0f || srcSize.Y <= 0.0f)
        return false;

      Vector2 size = CalcSize(targetSize, srcSize, scalePolicy);
      if (size.X < 0 || size.Y < 0)
        return false;

      rSize = size;
      return true;
    }



    Vector2 UIScaleUtil::CalcScaling(const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return CalcScaling(targetSize, Vector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    Vector2 UIScaleUtil::CalcScaling(const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy)
    {
      // Early abort in case of crazy input
      if (srcSize.X <= 0.0f || srcSize.Y <= 0.0f)
        return Vector2();

      const Vector2 newSize = CalcSize(targetSize, srcSize, scalePolicy);

      return !EqualHelper::IsAlmostEqual(newSize, srcSize) ? Vector2((newSize.X / srcSize.X), (newSize.Y / srcSize.Y)) : Vector2(1.0f, 1.0f);
    }


    Vector2 UIScaleUtil::CalcSize(const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy)
    {
      return CalcSize(targetSize, Vector2(static_cast<float>(srcSize.X), static_cast<float>(srcSize.Y)), scalePolicy);
    }


    Vector2 UIScaleUtil::CalcSize(const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy)
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

      const Vector2 clampedTarget(std::max(targetSize.X, 0.0f), std::max(targetSize.Y, 0.0f));
      const Vector2 clampedSrc(srcSize.X <= 0.0f ? 0.0f : srcSize.X, srcSize.Y <= 0.0f ? 0.0f : srcSize.Y);

      // Early abort for no scaling
      if (clampedTarget == clampedSrc)
        return clampedTarget;

      switch (scalePolicy)
      {
      case ItemScalePolicy::NoScaling:
        return srcSize;
      case ItemScalePolicy::Fit:
        return clampedTarget;
      case ItemScalePolicy::Downscale:
        return Vector2(std::min(clampedTarget.X, clampedSrc.X), std::min(clampedTarget.Y, clampedSrc.Y));
      case ItemScalePolicy::FitKeepAR:
        return CalcSizeKeepAspectRatioScaling(clampedTarget, srcSize);
      case ItemScalePolicy::DownscaleKeepAR:
        return CalcSizeKeepAspectRatioDownScaling(clampedTarget, srcSize);
      default:
        FSLLOG_WARNING("Unsupported scaling policy, using src size");
        return srcSize;
      }
    }
  }
}
