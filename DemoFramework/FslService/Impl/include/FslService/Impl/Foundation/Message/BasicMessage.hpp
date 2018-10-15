#ifndef FSLSERVICE_IMPL_FOUNDATION_MESSAGE_BASICMESSAGE_HPP
#define FSLSERVICE_IMPL_FOUNDATION_MESSAGE_BASICMESSAGE_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslService/Impl/Foundation/Message/BasicMessageType.hpp>
#include <FslService/Impl/Foundation/Message/IMessagePool.hpp>
#include <FslService/Impl/Foundation/Message/Message.hpp>
#include <exception>
#include <memory>

namespace Fsl
{
  class Message;

  struct BasicMessage
  {
    BasicMessageType Type{BasicMessageType::Invalid};
    int32_t Param1{0};
    int32_t Param2{0};
    std::exception_ptr Exception;
    std::shared_ptr<Message> Content;
    std::shared_ptr<IMessagePool> MessagePool;

    BasicMessage() = default;


    BasicMessage(const BasicMessageType type)
      : Type(type)
    {
    }


    BasicMessage(const BasicMessageType type, const int32_t param1)
      : Type(type)
      , Param1(param1)
    {
    }


    BasicMessage(const BasicMessageType type, const std::shared_ptr<Message>& content)
      : Type(type)
      , Content(content)
    {
    }


    BasicMessage(const BasicMessageType type, const int32_t param1, const std::shared_ptr<Message>& content,
                 const std::shared_ptr<IMessagePool>& messagePool)
      : Type(type)
      , Param1(param1)
      , Content(content)
      , MessagePool(messagePool)
    {
    }

    BasicMessage(const BasicMessageType type, const std::shared_ptr<Message>& content, const std::shared_ptr<IMessagePool>& messagePool)
      : Type(type)
      , Content(content)
      , MessagePool(messagePool)
    {
    }


    BasicMessage(const BasicMessageType type, const int32_t param1, const std::exception_ptr& exception)
      : Type(type)
      , Param1(param1)
      , Exception(exception)
    {
    }

    BasicMessage(const BasicMessageType type, const std::exception_ptr& exception)
      : Type(type)
      , Exception(exception)
    {
    }


    BasicMessage(const BasicMessageType type, const int32_t param1, const int32_t param2, const std::exception_ptr& exception,
                 const std::shared_ptr<Message>& content, const std::shared_ptr<IMessagePool>& messagePool)
      : Type(type)
      , Param1(param1)
      , Param2(param2)
      , Exception(exception)
      , Content(content)
      , MessagePool(messagePool)
    {
    }
  };
}

#endif
