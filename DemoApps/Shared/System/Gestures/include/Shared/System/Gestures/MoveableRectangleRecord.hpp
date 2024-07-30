#ifndef SHARED_SYSTEM_GESTURES_MOVEABLERECTANGLERECORD_HPP
#define SHARED_SYSTEM_GESTURES_MOVEABLERECTANGLERECORD_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Transition/TransitionPxVector2.hpp>
#include <FslGraphics/Color.hpp>

namespace Fsl
{

  struct MoveableRectangleRecord
  {
    PxPoint2 LocationPx;
    PxSize2D SizePx;
    Color RenderColor;
    TransitionPxVector2 FlickAnim;
    bool Flicking{false};

    MoveableRectangleRecord() = default;
    MoveableRectangleRecord(const PxPoint2 locationPx, const PxSize2D sizePx, const Color renderColor) noexcept
      : LocationPx(locationPx)
      , SizePx(sizePx)
      , RenderColor(renderColor)
      , FlickAnim(TimeSpan::FromSeconds(1), TransitionType::EaseOutSine)
    {
    }
  };

}


#endif
