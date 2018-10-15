#ifndef FSLDEMOHOST_BASE_SERVICE_ASYNCIMAGE_MESSAGE_TRYWRITEBITMAPPROMISEMESSAGE_HPP
#define FSLDEMOHOST_BASE_SERVICE_ASYNCIMAGE_MESSAGE_TRYWRITEBITMAPPROMISEMESSAGE_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslService/Impl/ServiceType/Async/Message/AsyncPromiseMessage.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/ImageFormat.hpp>
#include <FslGraphics/PixelFormat.hpp>

namespace Fsl
{
  namespace AsyncImageMessages
  {
    struct TryWriteBitmapPromiseMessage : public AsyncPromiseMessage<bool>
    {
      IO::Path AbsolutePath;
      Bitmap TheBitmap;
      ImageFormat TheImageFormat{ImageFormat::Undefined};
      PixelFormat DesiredPixelFormat{PixelFormat::Undefined};

      TryWriteBitmapPromiseMessage() = default;

      TryWriteBitmapPromiseMessage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                   const PixelFormat desiredPixelFormat)
        : AbsolutePath(absolutePath)
        , TheBitmap(bitmap)
        , TheImageFormat(imageFormat)
        , DesiredPixelFormat(desiredPixelFormat)
      {
      }
    };
  }
}

#endif
