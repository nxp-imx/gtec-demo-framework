#ifndef FSLSERVICE_IMPL_FOUNDATION_MESSAGE_FIREANDFORGETBASICMESSAGE_HPP
#define FSLSERVICE_IMPL_FOUNDATION_MESSAGE_FIREANDFORGETBASICMESSAGE_HPP
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

#include <FslService/Consumer/ProviderId.hpp>
#include <FslService/Impl/Foundation/Message/BasicMessage.hpp>
#include <FslBase/Exceptions.hpp>
#include <utility>

namespace Fsl
{
  struct FireAndForgetBasicMessage
  {
    ProviderId TargetId;
    std::shared_ptr<Message> Content;
    std::shared_ptr<IMessagePool> MessagePool;


    FireAndForgetBasicMessage() = default;


    FireAndForgetBasicMessage(const ProviderId& targetId, std::shared_ptr<Message> content)
      : TargetId(targetId)
      , Content(std::move(content))
    {
    }


    FireAndForgetBasicMessage(const ProviderId& targetId, std::shared_ptr<Message> content, std::shared_ptr<IMessagePool> messagePool)
      : TargetId(targetId)
      , Content(std::move(content))
      , MessagePool(std::move(messagePool))
    {
    }


    operator BasicMessage() const    // NOLINT(google-explicit-constructor);
    {
      return BasicMessage(BasicMessageType::FireAndForgetMessage, static_cast<uint32_t>(TargetId.Get()), Content, MessagePool);
    }


    static FireAndForgetBasicMessage Decode(const BasicMessage& message)
    {
      if (message.Type != BasicMessageType::FireAndForgetMessage)
      {
        throw std::invalid_argument("message was not of the expected type");
      }

      return FireAndForgetBasicMessage(ProviderId(static_cast<uint32_t>(message.Param1)), message.Content, message.MessagePool);
    }
  };
}

#endif
