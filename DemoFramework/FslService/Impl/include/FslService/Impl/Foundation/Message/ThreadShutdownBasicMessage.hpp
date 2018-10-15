#ifndef FSLSERVICE_IMPL_FOUNDATION_MESSAGE_THREADSHUTDOWNBASICMESSAGE_HPP
#define FSLSERVICE_IMPL_FOUNDATION_MESSAGE_THREADSHUTDOWNBASICMESSAGE_HPP
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

#include <FslService/Impl/Foundation/Message/BasicMessage.hpp>
#include <FslService/Impl/Registry/ServiceGroupId.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  struct ThreadShutdownBasicMessage
  {
    ServiceGroupId Id;
    std::exception_ptr Exception;


    ThreadShutdownBasicMessage(const ServiceGroupId& id)
      : Id(id)
    {
    }


    ThreadShutdownBasicMessage(const ServiceGroupId& id, const std::exception_ptr& exception)
      : Id(id)
      , Exception(exception)
    {
    }


    operator BasicMessage() const
    {
      return BasicMessage(BasicMessageType::ThreadShutdown, static_cast<int32_t>(Id.GetValue()), Exception);
    }


    static ThreadShutdownBasicMessage Decode(const BasicMessage& message)
    {
      if (message.Type != BasicMessageType::ThreadShutdown)
      {
        throw std::invalid_argument("message was not of the expected type");
      }

      return ThreadShutdownBasicMessage(ServiceGroupId(static_cast<uint32_t>(message.Param1)), message.Exception);
    }
  };
}

#endif
