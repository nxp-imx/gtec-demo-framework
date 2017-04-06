#ifndef FSLSIMPLEUI_UISCALEUTIL_HPP
#define FSLSIMPLEUI_UISCALEUTIL_HPP
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
#include <FslBase/Math/Vector2.hpp>
#include <FslSimpleUI/ItemScalePolicy.hpp>

namespace Fsl
{
  struct Point2;

  namespace UI
  {

    class UIScaleUtil
    {
    public:
      //! @brief Try to calc the scaling factor based on the requested targetSize, srcSize and scale policy
      //! @return true if ok, false if the inputSize becomes impossible to scale to the output size or if one of the components of the output size is zero
      static bool TryCalcScaling(Vector2& rScaling, const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy);
      static bool TryCalcScaling(Vector2& rScaling, const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy);

      //! @brief Try to calc the size based on the requested targetSize, srcSize and scale policy
      //! @return true if ok, false if one if one of the components of the output size is zero
      static bool TryCalcSize(Vector2& rSize, const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy);
      static bool TryCalcSize(Vector2& rSize, const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy);


      //! @brief Calc the scaling factor based on the requested targetSize, srcSize and scale policy
      static Vector2 CalcScaling(const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy);
      static Vector2 CalcScaling(const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy);

      //! @brief Calc the size based on the requested targetSize, srcSize and scale policy
      static Vector2 CalcSize(const Vector2& targetSize, const Point2& srcSize, const ItemScalePolicy scalePolicy);
      static Vector2 CalcSize(const Vector2& targetSize, const Vector2& srcSize, const ItemScalePolicy scalePolicy);
    };
  }
}

#endif
