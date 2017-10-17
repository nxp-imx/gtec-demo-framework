#ifndef FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEPROXYFACTORYTEMPLATE_HPP
#define FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEPROXYFACTORYTEMPLATE_HPP
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

#include <FslService/Impl/ServiceType/Async/AsynchronousServiceProxyFactoryBase.hpp>

namespace Fsl
{
  template<typename T>
  class AsynchronousServiceProxyFactoryTemplate0 : public AsynchronousServiceProxyFactoryBase
  {
  public:
    AsynchronousServiceProxyFactoryTemplate0()
      : AsynchronousServiceProxyFactoryBase(TypeInfo(typeid(T)))
    {
    }

    AsynchronousServiceProxyFactoryTemplate0(const ServiceCaps::Flags flags)
      : AsynchronousServiceProxyFactoryBase(TypeInfo(typeid(T)), flags)
    {
    }

    virtual std::shared_ptr<IBasicService> Allocate(const AsynchronousServiceProxyCreateInfo& createInfo) override
    {
      return std::shared_ptr<IBasicService>(new T(createInfo));
    }
  };


  template<typename T, typename TItf>
  class AsynchronousServiceProxyFactoryTemplate : public AsynchronousServiceProxyFactoryBase
  {
  public:
    AsynchronousServiceProxyFactoryTemplate()
      : AsynchronousServiceProxyFactoryBase(TypeInfo(typeid(TItf)))
    {
    }

    AsynchronousServiceProxyFactoryTemplate(const ServiceCaps::Flags flags)
      : AsynchronousServiceProxyFactoryBase(TypeInfo(typeid(TItf)), flags)
    {
    }

    virtual std::shared_ptr<IBasicService> Allocate(const AsynchronousServiceProxyCreateInfo& createInfo) override
    {
      return std::shared_ptr<IBasicService>(new T(createInfo));
    }
  };


  template<typename T, typename TItf1, typename TItf2>
  class AsynchronousServiceProxyFactoryTemplate2 : public IAsynchronousServiceProxyFactory
  {
    ServiceCaps::Flags m_flags;
  public:
    AsynchronousServiceProxyFactoryTemplate2()
      : m_flags(ServiceCaps::Default)
    {
    }

    AsynchronousServiceProxyFactoryTemplate2(const ServiceCaps::Flags flags)
      : m_flags(flags)
    {
    }


    virtual std::shared_ptr<AServiceOptionParser> GetOptionParser() const override
    {
      return std::shared_ptr<AServiceOptionParser>();
    }


    virtual ServiceCaps::Flags GetFlags() const override
    {
      return m_flags;
    }

    virtual void FillInterfaceType(ServiceSupportedInterfaceDeque& rServiceInterfaceTypeDeque) const override
    {
      rServiceInterfaceTypeDeque.push_back(TypeInfo(typeid(TItf1)));
      rServiceInterfaceTypeDeque.push_back(TypeInfo(typeid(TItf2)));
    }


    virtual std::shared_ptr<IBasicService> Allocate(const AsynchronousServiceProxyCreateInfo& createInfo) override
    {
      return std::shared_ptr<IBasicService>(new T(createInfo));
    }
  };


  template<typename T, typename TItf1, typename TItf2, typename TItf3>
  class AsynchronousServiceProxyFactoryTemplate3 : public IAsynchronousServiceProxyFactory
  {
    ServiceCaps::Flags m_flags;
  public:
    AsynchronousServiceProxyFactoryTemplate3()
      : m_flags(ServiceCaps::Default)
    {
    }

    AsynchronousServiceProxyFactoryTemplate3(const ServiceCaps::Flags flags)
      : m_flags(flags)
    {
    }

    virtual ServiceCaps::Flags GetFlags() const override
    {
      return m_flags;
    }

    virtual void FillInterfaceType(ServiceSupportedInterfaceDeque& rServiceInterfaceTypeDeque) const override
    {
      rServiceInterfaceTypeDeque.push_back(TypeInfo(typeid(TItf1)));
      rServiceInterfaceTypeDeque.push_back(TypeInfo(typeid(TItf2)));
      rServiceInterfaceTypeDeque.push_back(TypeInfo(typeid(TItf3)));
    }


    virtual std::shared_ptr<IBasicService> Allocate(const AsynchronousServiceProxyCreateInfo& createInfo) override
    {
      return std::shared_ptr<IBasicService>(new T(createInfo));
    }
  };
}

#endif
