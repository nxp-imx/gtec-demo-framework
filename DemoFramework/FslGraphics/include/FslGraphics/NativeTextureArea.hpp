#ifndef FSLGRAPHICS_NATIVETEXTUREAREA_HPP
#define FSLGRAPHICS_NATIVETEXTUREAREA_HPP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  //! Describes a area on the texture in the native texture coordinate system
  struct NativeTextureArea
  {
    float X0{0.0f};
    float Y0{0.0f};
    float X1{0.0f};
    float Y1{0.0f};

    constexpr NativeTextureArea() noexcept = default;

    constexpr NativeTextureArea(const float x0, const float y0, const float x2, const float y2) noexcept
      : X0(x0)
      , Y0(y0)
      , X1(x2)
      , Y1(y2)
    {
    }


    constexpr bool operator==(const NativeTextureArea& rhs) const noexcept
    {
      return ((X0 == rhs.X0) && (Y0 == rhs.Y0) && (X1 == rhs.X1) && (Y1 == rhs.Y1));
    }


    constexpr bool operator!=(const NativeTextureArea& rhs) const noexcept
    {
      return !((*this) == rhs);
    }
  };
}

#endif
