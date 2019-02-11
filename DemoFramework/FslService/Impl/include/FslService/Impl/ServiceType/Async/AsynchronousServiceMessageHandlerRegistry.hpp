#ifndef FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEMESSAGEHANDLERREGISTRY_HPP
#define FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEMESSAGEHANDLERREGISTRY_HPP
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

#include <FslBase/Noncopyable.hpp>
#include <FslService/Impl/Foundation/Message/Message.hpp>
#include <functional>
#include <memory>
#include <typeindex>

namespace Fsl
{
  class IAsynchronousServiceMessageHandlerRegistry;

  class AsynchronousServiceMessageHandlerRegistry : private Noncopyable
  {
    std::shared_ptr<IAsynchronousServiceMessageHandlerRegistry> m_registry;

  public:
    using type_message_function = std::function<void(Message&)>;

    AsynchronousServiceMessageHandlerRegistry(const std::shared_ptr<IAsynchronousServiceMessageHandlerRegistry>& registry);
    ~AsynchronousServiceMessageHandlerRegistry();


    void Register(const std::type_index& messageType, const type_message_function& handler) const;


    template <typename TMessage>
    void Register(const std::function<void(TMessage& message)>& handler) const
    {
      // Little lambda function that converts from the generic Message to a TMessage
      type_message_function typeConversionFunc = [handler](Message& message) { handler(dynamic_cast<TMessage&>(message)); };

      Register(std::type_index(typeid(TMessage)), typeConversionFunc);
    }
  };
}

#endif
