#ifndef FSLGRAPHICS_BITMAP_RAWBITMAPAREAEX_HPP
#define FSLGRAPHICS_BITMAP_RAWBITMAPAREAEX_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslGraphics/Bitmap/RawBitmapArea.hpp>

namespace Fsl
{
  //! @brief Very basic structure intended to contain low level information about a bitmap area
  //! @note  It is expected that all other necessary data about the bitmap has been acquired elsewhere
  struct RawBitmapAreaEx : public RawBitmapArea
  {
    void* ContentEx{nullptr};

    RawBitmapAreaEx() = default;


    RawBitmapAreaEx(void* pContent, const Extent2D& extent, const uint32_t stride)
      : RawBitmapArea(pContent, extent, stride)
      , ContentEx(pContent)
    {
    }

    //! @brief The main point of this object is to give write access to the object
    void* GetContent()
    {
      assert(Content == ContentEx);
      // Since you have a object of this type it means that the pointer was 'non' const to begin with.
      // So this should be entirely safe
      return ContentEx;
    }
  };
}

#endif
