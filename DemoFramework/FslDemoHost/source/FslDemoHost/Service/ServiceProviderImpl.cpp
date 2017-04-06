/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Service/ServiceId.hpp>
#include <FslDemoHost/Service/Exceptions.hpp>
#include <FslDemoHost/Service/ServiceProviderImpl.hpp>
#include <FslDemoHost/Service/ServiceSupportedInterfaceDeque.hpp>
#include <cassert>

namespace Fsl
{

  ServiceProviderImpl::ServiceProviderImpl(const ThreadLocalSingletonServiceMap& map, const ThreadLocalSingletonServiceMapEx& mapEx)
    : m_map(map)
    , m_mapEx(mapEx)
    , m_uniqueServices()
  {
  }


  ServiceProviderImpl::ServiceProviderImpl(const ThreadLocalSingletonServiceMap& map, const ThreadLocalSingletonServiceMapEx& mapEx, const ServiceDeque& uniqueServices)
    : m_map(map)
    , m_mapEx(mapEx)
    , m_uniqueServices(uniqueServices)
  {

  }


  ServiceProviderImpl::~ServiceProviderImpl()
  {
  }


  void ServiceProviderImpl::Update()
  {
    ServiceDeque::iterator itr = m_uniqueServices.begin();
    while (itr != m_uniqueServices.end())
    {
      (*itr)->Update();
      ++itr;
    }
  }


  std::shared_ptr<IService> ServiceProviderImpl::TryGet(const ServiceId& serviceId) const
  {
    if (serviceId == ServiceId::Invalid())
    {
      FSLLOG_ERROR("No service can be returned for a invalid id");
      return std::shared_ptr<IService>();
    }

    // See if we already have a instance of the service
    ThreadLocalSingletonServiceMap::const_iterator itrService = m_map.find(serviceId.Get());
    if (itrService != m_map.end())
    {
      return itrService->second;
      //if (itrService->second)
      //else
        //throw ServiceInterfaceHasMultipleProvidersException(serviceId.Get().name());
    }
    else
      return std::shared_ptr<IService>();
  }


  std::shared_ptr<IService> ServiceProviderImpl::Get(const ServiceId& serviceId) const
  {
    const std::shared_ptr<IService> service = TryGet(serviceId);
    if (service)
      return service;
    else
      throw UnknownServiceException(serviceId.Get().name());
  }


  std::shared_ptr<IService> ServiceProviderImpl::TryGet(const ServiceId& serviceId, const ProviderId& providerId) const
  {
    if (serviceId == ServiceId::Invalid())
    {
      FSLLOG_ERROR("No service can be returned for a invalid id");
      return std::shared_ptr<IService>();
    }

    // See if we already have a instance of the service
    ThreadLocalSingletonServiceMapEx::const_iterator itrService = m_mapEx.find(serviceId.Get());
    if (itrService != m_mapEx.end())
    {
      throw NotImplementedException();
    }
    else
      return std::shared_ptr<IService>();
  }


  std::shared_ptr<IService> ServiceProviderImpl::Get(const ServiceId& serviceId, const ProviderId& providerId) const
  {
    const std::shared_ptr<IService> service = TryGet(serviceId, providerId);
    if (service)
      return service;
    else
      throw UnknownServiceException(serviceId.Get().name());
  }


  void ServiceProviderImpl::Get(ServiceDeque& rServices, const ServiceId& serviceId) const
  {
    rServices.clear();
    if (serviceId == ServiceId::Invalid())
    {
      FSLLOG_ERROR("No service can be returned for a invalid id");
      return;
    }

    // See if we already have a instance of the service
    ThreadLocalSingletonServiceMap::const_iterator itrFind = m_map.find(serviceId.Get());
    if (itrFind == m_map.end())
    {
      return;
    }

    // If 'second' is null its a interface with multiple providers
    if (itrFind->second)
    {
      rServices.push_back(itrFind->second);
    }
    else
    {
      ThreadLocalSingletonServiceMapEx::const_iterator itrServices = m_mapEx.find(serviceId.Get());
      assert(itrServices != m_mapEx.end());
      if (itrServices != m_mapEx.end())
      {
        ServiceDeque::const_iterator itr = itrServices->second->begin();
        const ServiceDeque::const_iterator itrEnd = itrServices->second->end();
        while (itr != itrEnd)
        {
          rServices.push_back(*itr);
          ++itr;
        }
      }
    }
  }
}
