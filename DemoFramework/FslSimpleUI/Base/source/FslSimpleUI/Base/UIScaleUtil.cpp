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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslBase/Math/Pixel/EqualHelper_PxSize2DF.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl::UI
{
  namespace
  {
    [[maybe_unused]] constexpr float ConsiderPositiveInfinity = std::numeric_limits<float>::max();

    inline PxSize1DF CorrectFloatingPointErrors(const PxSize1DF value, const PxSize1DF correctMaxValue)
    {
      return !EqualHelper::IsAlmostEqual(value, correctMaxValue) ? value : correctMaxValue;
    }

    inline PxSize2DF CalcSizeKeepAspectRatioScaling(const PxSize2DF targetSize, const PxSize2DF srcSize)
    {
      assert(targetSize.RawWidth() >= 0.0f);
      assert(targetSize.RawHeight() >= 0.0f);
      assert(targetSize.RawWidth() < ConsiderPositiveInfinity);
      assert(targetSize.RawHeight() < ConsiderPositiveInfinity);
      assert(srcSize.RawWidth() >= 0.0f);
      assert(srcSize.RawHeight() >= 0.0f);
      assert(srcSize.RawWidth() < ConsiderPositiveInfinity);
      assert(srcSize.RawHeight() < ConsiderPositiveInfinity);

      if (srcSize.RawWidth() < 0.00001f || srcSize.RawHeight() < 0.00001f)
      {
        return {};
      }
      if (targetSize.RawWidth() <= 0.00001f || targetSize.RawHeight() <= 0.00001f)
      {
        return {};
      }

      // m = dy / dx
      // y = m * x
      // x = y / m
      PxSize1DF m = srcSize.Height() / srcSize.Width();

      PxSize1DF calcedX = targetSize.Height() / m;
      PxSize1DF calcedY = m * targetSize.Width();
      if (calcedX.RawValue() <= 0.0001f || calcedY.RawValue() <= 0.0001f)
      {
        return {};
      }

      PxSize2DF size1(calcedX, targetSize.Height());
      PxSize2DF size2(targetSize.Width(), calcedY);
      // since we keep the aspect ratio the scaling factors will always be uniform
      return (size1.LengthSquared() < size2.LengthSquared() ? size1 : size2);
    }


    inline PxSize2DF CalcSizeKeepAspectRatioDownScaling(const PxSize2DF targetSize, const PxSize2DF srcSize)
    {
      assert(targetSize.RawWidth() >= 0.0f);
      assert(targetSize.RawHeight() >= 0.0f);
      assert(targetSize.RawWidth() < ConsiderPositiveInfinity);
      assert(targetSize.RawHeight() < ConsiderPositiveInfinity);
      assert(srcSize.RawWidth() > 0.0f);
      assert(srcSize.RawHeight() > 0.0f);
      assert(srcSize.RawWidth() < ConsiderPositiveInfinity);
      assert(srcSize.RawHeight() < ConsiderPositiveInfinity);

      if (srcSize.Width() > targetSize.Width())
      {
        if (srcSize.Height() > targetSize.Height())
        {
          // Both X,Y is too large so make it fit
          return CalcSizeKeepAspectRatioScaling(targetSize, srcSize);
        }

        // Source Y fits but X is too large
        const PxSize1DF newScale = targetSize.Width() / srcSize.Width();
        PxSize1DF newY = srcSize.Height() * newScale;
        return {targetSize.Width(), CorrectFloatingPointErrors(newY, targetSize.Height())};
      }
      if (srcSize.Height() > targetSize.Height())
      {
        // Source X fits but Y is too large
        const PxSize1DF newScale = targetSize.Height() / srcSize.Height();
        PxSize1DF newX = srcSize.Width() * newScale;
        return {CorrectFloatingPointErrors(newX, targetSize.Width()), targetSize.Height()};
      }

      // Source X, Y fits so do nothing
      return srcSize;
    }
  }


  bool UIScaleUtil::TryCalcScaling(PxSize2DF& rScaling, const PxSize2DF targetSize, const PxSize2D srcSize, const ItemScalePolicy scalePolicy)
  {
    return TryCalcScaling(rScaling, targetSize, TypeConverter::UncheckedTo<PxSize2DF>(srcSize), scalePolicy);
  }


  bool UIScaleUtil::TryCalcScaling(PxSize2DF& rScaling, const PxSize2DF targetSize, const PxSize2DF srcSize, const ItemScalePolicy scalePolicy)
  {
    if (srcSize.RawWidth() <= 0.0f || srcSize.RawHeight() <= 0.0f)
    {
      rScaling = PxSize2DF();
      return false;
    }

    PxSize2DF newSize;
    if (!TryCalcSize(newSize, targetSize, srcSize, scalePolicy))
    {
      rScaling = PxSize2DF();
      return false;
    }

    rScaling = !EqualHelper::IsAlmostEqual(newSize, srcSize) ? PxSize2DF((newSize.Width() / srcSize.Width()), (newSize.Height() / srcSize.Height()))
                                                             : PxSize2DF::Create(1.0f, 1.0f);
    return true;
  }


  bool UIScaleUtil::TryCalcSize(PxSize2DF& rSize, const PxSize2DF targetSize, const PxSize2D srcSize, const ItemScalePolicy scalePolicy)
  {
    return TryCalcSize(rSize, targetSize, TypeConverter::UncheckedTo<PxSize2DF>(srcSize), scalePolicy);
  }


  bool UIScaleUtil::TryCalcSize(PxSize2DF& rSize, const PxSize2DF targetSize, const PxSize2DF srcSize, const ItemScalePolicy scalePolicy)
  {
    if (srcSize.RawWidth() <= 0.0f || srcSize.RawHeight() <= 0.0f)
    {
      rSize = PxSize2DF();
      return false;
    }

    PxSize2DF size = CalcSize(targetSize, srcSize, scalePolicy);
    if (size.RawWidth() < 0 || size.RawHeight() < 0)
    {
      rSize = PxSize2DF();
      return false;
    }

    rSize = size;
    return true;
  }

  bool UIScaleUtil::TryCalcSize(PxSize2D& rSize, const PxSize2D targetSize, const PxSize2D srcSize, const ItemScalePolicy scalePolicy)
  {
    return TryCalcSize(rSize, targetSize, TypeConverter::To<PxSize2DF>(srcSize), scalePolicy);
  }

  bool UIScaleUtil::TryCalcSize(PxSize2D& rSize, const PxSize2D targetSize, const PxSize2DF srcSize, const ItemScalePolicy scalePolicy)
  {
    PxSize2DF scaledSize;
    if (TryCalcSize(scaledSize, TypeConverter::To<PxSize2DF>(targetSize), srcSize, scalePolicy))
    {
      rSize = TypeConverter::UncheckedChangeTo<PxSize2D>(scaledSize);
      return true;
    }
    rSize = {};
    return false;
  }


  PxSize2DF UIScaleUtil::CalcScaling(const PxSize2DF targetSize, const PxSize2D srcSize, const ItemScalePolicy scalePolicy)
  {
    return CalcScaling(targetSize, TypeConverter::UncheckedTo<PxSize2DF>(srcSize), scalePolicy);
  }


  PxSize2DF UIScaleUtil::CalcScaling(const PxSize2DF targetSize, const PxSize2DF srcSize, const ItemScalePolicy scalePolicy)
  {
    // Early abort in case of crazy input
    if (srcSize.RawWidth() <= 0.0f || srcSize.RawHeight() <= 0.0f)
    {
      return {};
    }

    const PxSize2DF newSize = CalcSize(targetSize, srcSize, scalePolicy);

    return !EqualHelper::IsAlmostEqual(newSize, srcSize) ? PxSize2DF(newSize.Width() / srcSize.Width(), newSize.Height() / srcSize.Height())
                                                         : PxSize2DF::Create(1.0f, 1.0f);
  }


  PxSize2DF UIScaleUtil::CalcSize(const PxSize2DF targetSize, const PxSize2D srcSize, const ItemScalePolicy scalePolicy)
  {
    return CalcSize(targetSize, TypeConverter::UncheckedTo<PxSize2DF>(srcSize), scalePolicy);
  }


  PxSize2DF UIScaleUtil::CalcSize(const PxSize2DF targetSize, const PxSize2DF srcSize, const ItemScalePolicy scalePolicy)
  {
    assert(!isnan(targetSize.RawWidth()));
    assert(!isnan(targetSize.RawHeight()));
    assert(!isnan(srcSize.RawWidth()));
    assert(!isnan(srcSize.RawHeight()));
    assert(targetSize.RawWidth() >= 0.0f);
    assert(targetSize.RawHeight() >= 0.0f);
    assert(targetSize.RawWidth() < ConsiderPositiveInfinity);
    assert(targetSize.RawHeight() < ConsiderPositiveInfinity);
    assert(srcSize.RawWidth() < ConsiderPositiveInfinity);
    assert(srcSize.RawHeight() < ConsiderPositiveInfinity);

    // Early abort for no scaling
    if (targetSize == srcSize)
    {
      return targetSize;
    }

    switch (scalePolicy)
    {
    case ItemScalePolicy::NoScaling:
      return srcSize;
    case ItemScalePolicy::Fit:
      return targetSize;
    case ItemScalePolicy::Downscale:
      return {MathHelper::Min(targetSize.Width(), srcSize.Width()), MathHelper::Min(targetSize.Height(), srcSize.Height())};
    case ItemScalePolicy::FitKeepAR:
      return CalcSizeKeepAspectRatioScaling(targetSize, srcSize);
    case ItemScalePolicy::DownscaleKeepAR:
      return CalcSizeKeepAspectRatioDownScaling(targetSize, srcSize);
    default:
      FSLLOG3_WARNING("Unsupported scaling policy, using src size");
      return srcSize;
    }
  }
}
