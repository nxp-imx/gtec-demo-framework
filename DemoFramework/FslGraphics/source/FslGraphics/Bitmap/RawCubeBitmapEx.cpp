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

#include <FslGraphics/Bitmap/RawCubeBitmapEx.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  RawCubeBitmapEx::RawCubeBitmapEx()
    : m_bitmapPosX()
    , m_bitmapNegX()
    , m_bitmapPosY()
    , m_bitmapNegY()
    , m_bitmapPosZ()
    , m_bitmapNegZ()
  {
  }

  RawCubeBitmapEx::RawCubeBitmapEx(const RawBitmapEx& bitmapPosX, const RawBitmapEx& bitmapNegX, const RawBitmapEx& bitmapPosY, const RawBitmapEx& bitmapNegY, const RawBitmapEx& bitmapPosZ, const RawBitmapEx& bitmapNegZ)
    : m_bitmapPosX(bitmapPosX)
    , m_bitmapNegX(bitmapNegX)
    , m_bitmapPosY(bitmapPosY)
    , m_bitmapNegY(bitmapNegY)
    , m_bitmapPosZ(bitmapPosZ)
    , m_bitmapNegZ(bitmapNegZ)
  {
    if (!bitmapPosX.IsValid() || !bitmapNegX.IsValid() || !bitmapPosY.IsValid() || !bitmapNegY.IsValid() || !bitmapPosZ.IsValid() || !bitmapNegZ.IsValid())
      throw std::invalid_argument("The bitmaps must be valid");

    const auto texSize = bitmapPosX.Width();
    if (bitmapPosX.Height() != texSize ||
        bitmapNegX.Width() != texSize || bitmapNegX.Height() != texSize ||
        bitmapPosY.Width() != texSize || bitmapPosY.Height() != texSize ||
        bitmapNegY.Width() != texSize || bitmapNegY.Height() != texSize ||
        bitmapPosZ.Width() != texSize || bitmapPosZ.Height() != texSize ||
        bitmapNegZ.Width() != texSize || bitmapNegZ.Height() != texSize)
    {
      throw std::invalid_argument("The cube map textures must be of the same width and height");
    }

    const PixelFormat pixelFormat = bitmapPosX.GetPixelFormat();
    if (bitmapNegX.GetPixelFormat() != pixelFormat ||
        bitmapPosY.GetPixelFormat() != pixelFormat ||
        bitmapNegY.GetPixelFormat() != pixelFormat ||
        bitmapPosZ.GetPixelFormat() != pixelFormat ||
        bitmapNegZ.GetPixelFormat() != pixelFormat)
    {
      throw std::invalid_argument("The cube map textures must be of the same format");
    }
  }
}
