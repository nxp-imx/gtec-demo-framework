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

#include <FslGraphicsVG/VGUtil.hpp>
#include <FslGraphicsVG/VGCheck.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <cassert>
#include <VG/openvg.h>

namespace Fsl
{
  namespace OpenVG
  {

    void VGUtil::Capture(Bitmap& rBitmap, const PixelFormat pixelFormat, const Rectangle& srcRectangle)
    {
      // We don't need to clear as we are going to overwrite everything anyway
      // We utilize PixelFormat::R8G8B8A8_UINT here since that is what vgReadPixels is filling it with
      // that also allows the convert method to detect if the the supplied pixelFormat is different and then
      // convert as necessary
      rBitmap.Reset(Extent2D(srcRectangle.Width(), srcRectangle.Height()), PixelFormat::R8G8B8A8_UINT, rBitmap.GetOrigin(), BitmapClearMethod::DontClear);

      {
        RawBitmapEx rawBitmap;
        Bitmap::ScopedDirectAccess scopedAccess(rBitmap, rawBitmap);
        vgReadPixels(rawBitmap.Content(), rawBitmap.Stride(), VG_sABGR_8888, srcRectangle.X(), srcRectangle.Y(), srcRectangle.Width(), srcRectangle.Height());
        FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
        RawBitmapUtil::FlipHorizontal(rawBitmap);
      }

      // Convert if necessary (convert will do nothing if the format is already correct)
      BitmapUtil::Convert(rBitmap, pixelFormat);
    }
  }
}
