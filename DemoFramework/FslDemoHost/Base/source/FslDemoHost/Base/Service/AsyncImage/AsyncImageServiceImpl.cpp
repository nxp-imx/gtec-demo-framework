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

#include <FslDemoHost/Base/Service/AsyncImage/AsyncImageServiceImpl.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImplCreateInfo.hpp>
#include <FslBase/Exceptions.hpp>
#include <functional>
#include <utility>

namespace Fsl
{
  AsyncImageServiceImpl::AsyncImageServiceImpl(const AsynchronousServiceImplCreateInfo& createInfo, const ServiceProvider& serviceProvider)
    : AsynchronousServiceImpl(createInfo, serviceProvider)
    , m_image(serviceProvider.Get<IImageBasicService>())
  {
    createInfo.MessageHandlerRegistry.Register<AsyncImageMessages::ReadBitmapPromiseMessage>(
      std::bind(&AsyncImageServiceImpl::ReadBitmap, this, std::placeholders::_1));
    createInfo.MessageHandlerRegistry.Register<AsyncImageMessages::ReadTexturePromiseMessage>(
      std::bind(&AsyncImageServiceImpl::ReadTexture, this, std::placeholders::_1));
    createInfo.MessageHandlerRegistry.Register<AsyncImageMessages::WriteBitmapPromiseMessage>(
      std::bind(&AsyncImageServiceImpl::Write, this, std::placeholders::_1));
    createInfo.MessageHandlerRegistry.Register<AsyncImageMessages::WriteExactBitmapImagePromiseMessage>(
      std::bind(&AsyncImageServiceImpl::WriteExactImage, this, std::placeholders::_1));
    createInfo.MessageHandlerRegistry.Register<AsyncImageMessages::TryReadBitmapPromiseMessage>(
      std::bind(&AsyncImageServiceImpl::TryRead, this, std::placeholders::_1));
    createInfo.MessageHandlerRegistry.Register<AsyncImageMessages::TryWriteBitmapPromiseMessage>(
      std::bind(&AsyncImageServiceImpl::TryWrite, this, std::placeholders::_1));
    createInfo.MessageHandlerRegistry.Register<AsyncImageMessages::TryWriteExactBitmapImagePromiseMessage>(
      std::bind(&AsyncImageServiceImpl::TryWriteExactImage, this, std::placeholders::_1));
  }


  void AsyncImageServiceImpl::ReadBitmap(AsyncImageMessages::ReadBitmapPromiseMessage& message) const
  {
    try
    {
      Bitmap result;
      m_image->Read(result, message.AbsolutePath, message.DesiredPixelFormat, message.DesiredOrigin, message.PreferredChannelOrderHint);
      message.Promise.set_value(std::move(result));
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }


  void AsyncImageServiceImpl::ReadTexture(AsyncImageMessages::ReadTexturePromiseMessage& message) const
  {
    try
    {
      Texture result;
      m_image->Read(result, message.AbsolutePath, message.DesiredPixelFormat, message.DesiredOrigin, message.PreferredChannelOrderHint);
      message.Promise.set_value(std::move(result));
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }


  void AsyncImageServiceImpl::Write(AsyncImageMessages::WriteBitmapPromiseMessage& message)
  {
    try
    {
      m_image->Write(message.AbsolutePath, message.TheBitmap, message.TheImageFormat, message.DesiredPixelFormat);
      message.Promise.set_value();
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }


  void AsyncImageServiceImpl::WriteExactImage(AsyncImageMessages::WriteExactBitmapImagePromiseMessage& message)
  {
    try
    {
      m_image->WriteExactImage(message.AbsolutePath, message.TheBitmap, message.TheImageFormat, message.DesiredPixelFormat);
      message.Promise.set_value();
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }


  void AsyncImageServiceImpl::TryRead(AsyncImageMessages::TryReadBitmapPromiseMessage& message) const
  {
    try
    {
      Bitmap bitmap;
      const bool result =
        m_image->TryRead(bitmap, message.AbsolutePath, message.DesiredPixelFormat, message.DesiredOrigin, message.PreferredChannelOrderHint);
      message.Promise.set_value(std::make_pair(result, std::move(bitmap)));
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }


  void AsyncImageServiceImpl::TryWrite(AsyncImageMessages::TryWriteBitmapPromiseMessage& message)
  {
    try
    {
      const bool result = m_image->TryWrite(message.AbsolutePath, message.TheBitmap, message.TheImageFormat, message.DesiredPixelFormat);
      message.Promise.set_value(result);
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }


  void AsyncImageServiceImpl::TryWriteExactImage(AsyncImageMessages::TryWriteExactBitmapImagePromiseMessage& message)
  {
    try
    {
      const bool result = m_image->TryWriteExactImage(message.AbsolutePath, message.TheBitmap, message.TheImageFormat, message.DesiredPixelFormat);
      message.Promise.set_value(result);
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }
}
