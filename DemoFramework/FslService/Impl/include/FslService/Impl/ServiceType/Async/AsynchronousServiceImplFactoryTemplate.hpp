#ifndef FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEIMPLFACTORYTEMPLATE_HPP
#define FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEIMPLFACTORYTEMPLATE_HPP
/****************************************************************************************************************************************************
 * Copyright 2017, 2023 NXP
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

#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImplFactoryBase.hpp>

namespace Fsl
{
  template <typename T>
  class AsynchronousServiceImplFactoryTemplate : public AsynchronousServiceImplFactoryBase
  {
  public:
    AsynchronousServiceImplFactoryTemplate()
      : AsynchronousServiceImplFactoryBase(typeid(T))
    {
    }

    std::shared_ptr<IService> Allocate(const AsynchronousServiceImplCreateInfo& createInfo, ServiceProvider& provider) override
    {
      return std::shared_ptr<IService>(new T(createInfo, provider));
    }
  };

  template <typename T, typename TCustomArg0>
  class AsynchronousServiceImplFactoryCustomArgTemplate : public AsynchronousServiceImplFactoryBase
  {
    using custom_arg0_type = TCustomArg0;

    custom_arg0_type m_customArg0;

  public:
    explicit AsynchronousServiceImplFactoryCustomArgTemplate(const custom_arg0_type& customArg0)
      : AsynchronousServiceImplFactoryBase(typeid(T))
      , m_customArg0(customArg0)
    {
    }

    std::shared_ptr<IService> Allocate(const AsynchronousServiceImplCreateInfo& createInfo, ServiceProvider& provider) override
    {
      return std::shared_ptr<IService>(new T(createInfo, provider, m_customArg0));
    }
  };

}

#endif
