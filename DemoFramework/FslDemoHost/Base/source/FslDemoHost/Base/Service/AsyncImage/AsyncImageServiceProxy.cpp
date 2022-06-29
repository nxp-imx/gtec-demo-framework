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

#include <FslBase/Exceptions.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/AsyncImageServiceProxy.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/ReadBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/ReadTexturePromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/TryReadBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/TryWriteBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/TryWriteExactBitmapImagePromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/WriteBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/WriteExactBitmapImagePromiseMessage.hpp>

namespace Fsl
{
  namespace
  {
  }


  AsyncImageServiceProxy::AsyncImageServiceProxy(const AsynchronousServiceProxyCreateInfo& createInfo)
    : AsynchronousServiceProxy(createInfo)
  {
  }


  std::future<Bitmap> AsyncImageServiceProxy::ReadBitmap(const IO::Path& absolutePath, const PixelFormat desiredPixelFormat,
                                                         const BitmapOrigin desiredOrigin, const PixelChannelOrder preferredChannelOrder) const
  {
    return PostMessage(AsyncImageMessages::ReadBitmapPromiseMessage(absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder));
  }


  std::future<Texture> AsyncImageServiceProxy::ReadTexture(const IO::Path& absolutePath, const PixelFormat desiredPixelFormat,
                                                           const BitmapOrigin desiredOrigin, const PixelChannelOrder preferredChannelOrder) const
  {
    return PostMessage(AsyncImageMessages::ReadTexturePromiseMessage(absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder));
  }


  std::future<void> AsyncImageServiceProxy::Write(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                                  const PixelFormat desiredPixelFormat)
  {
    return PostMessage(AsyncImageMessages::WriteBitmapPromiseMessage(absolutePath, bitmap, imageFormat, desiredPixelFormat));
  }


  std::future<void> AsyncImageServiceProxy::WriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                                            const PixelFormat desiredPixelFormat)
  {
    return PostMessage(AsyncImageMessages::WriteExactBitmapImagePromiseMessage(absolutePath, bitmap, imageFormat, desiredPixelFormat));
  }


  std::future<std::pair<bool, Bitmap>> AsyncImageServiceProxy::TryRead(const IO::Path& absolutePath, const PixelFormat desiredPixelFormat,
                                                                       const BitmapOrigin desiredOrigin,
                                                                       const PixelChannelOrder preferredChannelOrder) const
  {
    return PostMessage(AsyncImageMessages::TryReadBitmapPromiseMessage(absolutePath, desiredPixelFormat, desiredOrigin, preferredChannelOrder));
  }


  std::future<bool> AsyncImageServiceProxy::TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                                     const PixelFormat desiredPixelFormat)
  {
    return PostMessage(AsyncImageMessages::TryWriteBitmapPromiseMessage(absolutePath, bitmap, imageFormat, desiredPixelFormat));
  }


  std::future<bool> AsyncImageServiceProxy::TryWriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                                               const PixelFormat desiredPixelFormat)
  {
    return PostMessage(AsyncImageMessages::TryWriteExactBitmapImagePromiseMessage(absolutePath, bitmap, imageFormat, desiredPixelFormat));
  }
}
