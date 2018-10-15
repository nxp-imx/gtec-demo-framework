#ifndef FSLDEMOHOST_BASE_SERVICE_ASYNCIMAGE_ASYNCIMAGESERVICEIMPL_HPP
#define FSLDEMOHOST_BASE_SERVICE_ASYNCIMAGE_ASYNCIMAGESERVICEIMPL_HPP
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

#include <FslDemoApp/Base/Service/AsyncImage/IAsyncImageService.hpp>
#include <FslDemoApp/Base/Service/ImageBasic/IImageBasicService.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/ReadBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/ReadTexturePromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/TryReadBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/TryWriteBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/TryWriteExactBitmapImagePromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/WriteBitmapPromiseMessage.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/Message/WriteExactBitmapImagePromiseMessage.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImpl.hpp>

namespace Fsl
{
  class AsyncImageServiceImpl : public AsynchronousServiceImpl
  {
    std::shared_ptr<IImageBasicService> m_image;

  public:
    AsyncImageServiceImpl(const AsynchronousServiceImplCreateInfo& createInfo, const ServiceProvider& serviceProvider);

  private:
    // IAsyncImageService
    void ReadBitmap(AsyncImageMessages::ReadBitmapPromiseMessage& message) const;
    void ReadTexture(AsyncImageMessages::ReadTexturePromiseMessage& message) const;
    void Write(AsyncImageMessages::WriteBitmapPromiseMessage& message);
    void WriteExactImage(AsyncImageMessages::WriteExactBitmapImagePromiseMessage& message);
    void TryRead(AsyncImageMessages::TryReadBitmapPromiseMessage& message) const;
    void TryWrite(AsyncImageMessages::TryWriteBitmapPromiseMessage& message);
    void TryWriteExactImage(AsyncImageMessages::TryWriteExactBitmapImagePromiseMessage& message);
  };
}

#endif
