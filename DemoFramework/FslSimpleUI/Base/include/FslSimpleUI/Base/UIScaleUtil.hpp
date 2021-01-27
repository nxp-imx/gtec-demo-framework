#ifndef FSLSIMPLEUI_BASE_UISCALEUTIL_HPP
#define FSLSIMPLEUI_BASE_UISCALEUTIL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
//#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Base/ItemScalePolicy.hpp>

namespace Fsl
{
  struct PxPoint2;
  struct PxSize2D;

  namespace UI
  {
    class UIScaleUtil
    {
    public:
      //! @brief Try to calc the scaling factor based on the requested targetSize, srcSize and scale policy
      //! @return true if ok, false if the inputSize becomes impossible to scale to the output size or if one of the components of the output size is
      //! zero
      static bool TryCalcScaling(PxVector2& rScaling, const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy);
      static bool TryCalcScaling(PxVector2& rScaling, const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy);

      //! @brief Try to calc the size based on the requested targetSize, srcSize and scale policy
      //! @return true if ok, false if one if one of the components of the output size is zero
      static bool TryCalcSize(PxVector2& rSize, const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy);
      static bool TryCalcSize(PxVector2& rSize, const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy);

      //! @brief Try to calc the size based on the requested targetSize, srcSize and scale policy
      //! @return true if ok, false if one if one of the components of the output size is zero
      static bool TryCalcSize(PxPoint2& rSize, const PxPoint2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy);
      static bool TryCalcSize(PxPoint2& rSize, const PxPoint2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy);

      /*    static bool TryCalcSize(PxAvailableSize& rSize, const PxAvailableSize& targetSize, const PxAvailableSize& srcSize,
                                  const ItemScalePolicy scalePolicy);
      */
      static bool TryCalcSize(PxSize2D& rSize, const PxSize2D& targetSize, const PxSize2D& srcSize, const ItemScalePolicy scalePolicy);

      //! @brief Calc the scaling factor based on the requested targetSize, srcSize and scale policy
      static PxVector2 CalcScaling(const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy);
      static PxVector2 CalcScaling(const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy);

      //! @brief Calc the size based on the requested targetSize, srcSize and scale policy
      static PxVector2 CalcSize(const PxVector2& targetSize, const PxPoint2& srcSize, const ItemScalePolicy scalePolicy);
      static PxVector2 CalcSize(const PxVector2& targetSize, const PxVector2& srcSize, const ItemScalePolicy scalePolicy);
    };
  }
}

#endif
