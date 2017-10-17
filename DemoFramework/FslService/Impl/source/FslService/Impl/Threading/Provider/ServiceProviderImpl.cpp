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

#include "ServiceProviderImpl.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslService/Consumer/ServiceId.hpp>
#include <FslService/Impl/Exceptions.hpp>
#include <FslService/Impl/ServiceSupportedInterfaceDeque.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  ServiceProviderImpl::ServiceProviderImpl(const TypeServiceMaps& serviceProviderMaps)
    : m_serviceProviderMaps(serviceProviderMaps)
  {
  }


  ServiceProviderImpl::~ServiceProviderImpl()
  {
  }


  void ServiceProviderImpl::Update()
  {
    ServiceDeque::iterator itr = m_serviceProviderMaps.OwnedUniqueServices.begin();
    while (itr != m_serviceProviderMaps.OwnedUniqueServices.end())
    {
      (*itr)->Update();
      ++itr;
    }
  }


  std::shared_ptr<IBasicService> ServiceProviderImpl::TryGet(const ServiceId& serviceId) const
  {
    auto res = TryGetNow(serviceId);
    FSLLOG_DEBUG_WARNING_IF(res.LaunchType == ServiceLaunchType::MultipleProviderTag, "Service has multiple providers, please specify which one you want.");

    if (res.LaunchType == ServiceLaunchType::InstanceAllocator)
    {
      assert(res.InstanceFactory);
      return res.InstanceFactory->Allocate();
    }
    return res.Service;
  }


  std::shared_ptr<IBasicService> ServiceProviderImpl::Get(const ServiceId& serviceId) const
  {
    auto res = TryGetNow(serviceId);
    switch (res.LaunchType)
    {
    case ServiceLaunchType::MultipleProviderTag:
      throw ServiceInterfaceHasMultipleProvidersException(serviceId.Get().name());
    case ServiceLaunchType::Invalid:
      throw UnknownServiceException(serviceId.Get().name());
    case ServiceLaunchType::InstanceAllocator:
      assert(res.InstanceFactory);
      return res.InstanceFactory->Allocate();
    default:
      break;
    }

    if (! res.Service)
      throw std::runtime_error("internal error, service pointer can not be null");

    return res.Service;
  }


  std::shared_ptr<IBasicService> ServiceProviderImpl::TryGet(const ServiceId& serviceId, const ProviderId& providerId) const
  {
    auto res = TryGetNow(serviceId, providerId);
    FSLLOG_DEBUG_WARNING_IF(res.LaunchType == ServiceLaunchType::MultipleProviderTag, "Service has multiple providers, please specify which one you want.");

    if (res.LaunchType == ServiceLaunchType::InstanceAllocator)
    {
      assert(res.InstanceFactory);
      return res.InstanceFactory->Allocate();
    }
    return res.Service;
  }


  std::shared_ptr<IBasicService> ServiceProviderImpl::Get(const ServiceId& serviceId, const ProviderId& providerId) const
  {
    auto res = TryGetNow(serviceId, providerId);
    switch (res.LaunchType)
    {
    case ServiceLaunchType::MultipleProviderTag:
      throw ServiceInterfaceHasMultipleProvidersException(serviceId.Get().name());
    case ServiceLaunchType::Invalid:
      throw UnknownServiceException(serviceId.Get().name());
    case ServiceLaunchType::InstanceAllocator:
      assert(res.InstanceFactory);
      return res.InstanceFactory->Allocate();
    default:
      break;
    }

    if (!res.Service)
      throw std::runtime_error("internal error, service pointer can not be null");

    return res.Service;
  }


  void ServiceProviderImpl::Get(BasicServiceDeque& rServices, const ServiceId& serviceId) const
  {
    rServices.clear();
    if (serviceId == ServiceId::Invalid())
    {
      FSLLOG_ERROR("No service can be returned for a invalid id");
      return;
    }

    // See if we already have a instance of the service
    TypeServiceMap::const_iterator itrFind = m_serviceProviderMaps.InterfaceToService.find(serviceId.Get());
    if (itrFind == m_serviceProviderMaps.InterfaceToService.end())
    {
      return;
    }

    assert(itrFind->second.LaunchType != ServiceLaunchType::Invalid);

    // If 'second' is null its a interface with multiple providers
    if (itrFind->second.LaunchType != ServiceLaunchType::MultipleProviderTag)
    {
      assert(itrFind->second.Service);
      rServices.push_back(itrFind->second.Service);
    }
    else
    {
      TypeServiceMapEx::const_iterator itrServices = m_serviceProviderMaps.InterfaceMultipleServices.find(serviceId.Get());
      assert(itrServices != m_serviceProviderMaps.InterfaceMultipleServices.end());
      if (itrServices != m_serviceProviderMaps.InterfaceMultipleServices.end())
      {
        for (auto& entry : *itrServices->second)
        {
          rServices.push_back(entry.Service);
        }
      }
    }
  }


  ServiceLaunchRecord ServiceProviderImpl::TryGetNow(const ServiceId& serviceId) const
  {
    if (serviceId == ServiceId::Invalid())
    {
      FSLLOG_ERROR("No service can be returned for a invalid id");
      return ServiceLaunchRecord();
    }

    // See if we already have a instance of the service
    TypeServiceMap::const_iterator itrService = m_serviceProviderMaps.InterfaceToService.find(serviceId.Get());

    return (itrService != m_serviceProviderMaps.InterfaceToService.end() ? itrService->second : ServiceLaunchRecord());
  }


  ServiceLaunchRecord ServiceProviderImpl::TryGetNow(const ServiceId& serviceId, const ProviderId& providerId) const
  {
    if (serviceId == ServiceId::Invalid())
    {
      FSLLOG_ERROR("No service can be returned for a invalid id");
      return ServiceLaunchRecord();
    }

    // See if we already have a instance of the service
    TypeServiceMapEx::const_iterator itrService = m_serviceProviderMaps.InterfaceMultipleServices.find(serviceId.Get());
    if (itrService != m_serviceProviderMaps.InterfaceMultipleServices.end())
    {
      const auto itrFind = std::find_if(itrService->second->begin(), itrService->second->end(), [providerId](const ServiceLaunchRecord& record) { return record.TheProviderId == providerId; });
      if (itrFind != itrService->second->end())
        return *itrFind;
    }
    return ServiceLaunchRecord();
  }

}
