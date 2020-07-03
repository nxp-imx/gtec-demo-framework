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

#include <FslService/Impl/Threading/Launcher/ServiceLauncher.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/Exceptions.hpp>
#include <FslService/Impl/Registry/RegisteredServiceDeque.hpp>
#include <FslService/Impl/ServiceSupportedInterfaceDeque.hpp>
#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include "../Provider/ServiceProviderImpl.hpp"
#include "RegisteredGlobalServiceInfo.hpp"
#include "TypeServiceMap.hpp"
#include "TypeServiceMapEx.hpp"

namespace Fsl
{
  namespace
  {
    struct RecordComparator
    {
      bool operator()(const RegisteredServiceRecord& lhs, const RegisteredServiceRecord& rhs)
      {
        return (lhs.StartupPriority > rhs.StartupPriority);
      }
    };


    std::string SafeGetTypeName(const std::shared_ptr<IServiceFactory>& factory)
    {
      auto* pFactory = factory.get();

      return (pFactory != nullptr ? std::string(typeid(*pFactory).name()) : std::string());
    }


    void CalcInterfaceHitCount(std::map<std::type_index, int32_t>& rInterfaceMap, const RegisteredServiceDeque& services)
    {
      auto itr = services.begin();
      const RegisteredServiceDeque::const_iterator itrEnd = services.end();
      ServiceSupportedInterfaceDeque interfaces;
      while (itr != itrEnd)
      {
        interfaces.clear();
        itr->Factory->FillInterfaceType(interfaces);
        assert(!interfaces.empty());

        // Increase the hit count for each interface provided by this service
        ServiceSupportedInterfaceDeque::const_iterator itrItf = interfaces.begin();
        const ServiceSupportedInterfaceDeque::const_iterator itrItfEnd = interfaces.end();
        while (itrItf != itrItfEnd)
        {
          auto itrFind = rInterfaceMap.find(*itrItf);
          if (itrFind != rInterfaceMap.end())
          {
            ++itrFind->second;
          }
          else
          {
            rInterfaceMap[*itrItf] = 1;
          }

          ++itrItf;
        }

        ++itr;
      }
    }


    void FindMultiProviderInterfaces(std::set<std::type_index>& rMultiProviderInterfaces, const RegisteredServiceDeque& services)
    {
      std::map<std::type_index, int32_t> interfaceMap;
      CalcInterfaceHitCount(interfaceMap, services);

      std::map<std::type_index, int32_t>::const_iterator itr = interfaceMap.begin();
      const std::map<std::type_index, int32_t>::const_iterator itrEnd = interfaceMap.end();
      while (itr != itrEnd)
      {
        if (itr->second > 1)
        {
          rMultiProviderInterfaces.insert(itr->first);
        }
        ++itr;
      }
    }


    std::shared_ptr<IService> StartService(std::set<std::type_index>& multiProviderInterfaces, ServiceProvider& provider,
                                           TypeServiceMaps& rServiceProviderMaps, const RegisteredServiceRecord& record)
    {
      ServiceSupportedInterfaceDeque deque;
      record.Factory->FillInterfaceType(deque);

      assert(!deque.empty());

      std::shared_ptr<IService> service(record.Factory->Allocate(provider));
      if (!service)
      {
        if ((record.Factory->GetFlags() & ServiceCaps::AvailableOnDemand) != 0)
        {
          return std::shared_ptr<IService>();
        }

        FSLLOG3_ERROR("The service factory '{}' returned a nullptr", SafeGetTypeName(record.Factory));
        throw InvalidServiceFactoryException("The service factory returned a null-ptr");
      }

      // Give the object a chance to do something after construction (useful for shared_from_this)
      service->Link(provider);

      // Register the object on all interfaces
      ServiceSupportedInterfaceDeque::const_iterator itr = deque.begin();
      const ServiceSupportedInterfaceDeque::const_iterator itrEnd = deque.end();
      while (itr != itrEnd)
      {
        if (multiProviderInterfaces.find(*itr) == multiProviderInterfaces.end())
        {
          assert(rServiceProviderMaps.InterfaceToService.find(*itr) == rServiceProviderMaps.InterfaceToService.end());
          rServiceProviderMaps.InterfaceToService[*itr] = ServiceLaunchRecord(record.Id, ServiceLaunchType::Instance, service);
        }
        else
        {
          // Register a multiple provider interface
          rServiceProviderMaps.InterfaceToService[*itr] = ServiceLaunchRecord(ProviderId::Invalid(), ServiceLaunchType::MultipleProviderTag);

          std::shared_ptr<std::deque<ServiceLaunchRecord>> serviceDeque;
          auto itrFind = rServiceProviderMaps.InterfaceMultipleServices.find(*itr);
          if (itrFind == rServiceProviderMaps.InterfaceMultipleServices.end())
          {
            serviceDeque = std::make_shared<std::deque<ServiceLaunchRecord>>();
            rServiceProviderMaps.InterfaceMultipleServices[*itr] = serviceDeque;
          }
          else
          {
            serviceDeque = itrFind->second;
          }

          serviceDeque->push_back(ServiceLaunchRecord(record.Id, ServiceLaunchType::Instance, service));
        }

        ++itr;
      }
      return service;
    }


    void StartService(const RegisteredServiceDeque& services, TypeServiceMaps& rServiceProviderMaps,
                      std::set<std::type_index>& rMultiProviderInterfaces)
    {
      // Sort the services according to priority
      RegisteredServiceDeque sortedServices(services);
      std::sort(sortedServices.begin(), sortedServices.end(), RecordComparator());

      auto provider = std::make_shared<ServiceProviderImpl>(rServiceProviderMaps);
      Priority currentPriority = sortedServices.front().StartupPriority;

      RegisteredServiceDeque::const_iterator itr = sortedServices.begin();
      const RegisteredServiceDeque::const_iterator itrEnd = sortedServices.end();

      ServiceProvider theServiceProvider(provider);
      while (itr != itrEnd)
      {
        // Each time we change priority we recreate the service provider to enable access to all higher priority services
        if (itr->StartupPriority != currentPriority)
        {
          currentPriority = itr->StartupPriority;
          provider = std::make_shared<ServiceProviderImpl>(rServiceProviderMaps);
          theServiceProvider = ServiceProvider(provider);
        }

        try
        {
          const std::shared_ptr<IService> service = StartService(rMultiProviderInterfaces, theServiceProvider, rServiceProviderMaps, *itr);
          if (service)
          {
            rServiceProviderMaps.OwnedUniqueServices.push_back(service);
          }
        }
        catch (const std::exception& ex)
        {
          if ((itr->Factory->GetFlags() & ServiceCaps::Optional) != 0)
          {
            FSLLOG3_WARNING("Service provided by '{}' failed to start: {}", SafeGetTypeName(itr->Factory), ex.what());
          }
          else
          {
            throw;
          }
        }
        ++itr;
      }
      provider.reset();
    }
  }


  std::shared_ptr<RegisteredGlobalServiceInfo> ServiceLauncher::Launch(const RegisteredServiceDeque& services)
  {
    TypeServiceMaps serviceProviderMaps;
    if (!services.empty())
    {
      // Find all interfaces that have multiple providers
      std::set<std::type_index> multiProviderInterfaces;
      FindMultiProviderInterfaces(multiProviderInterfaces, services);

      StartService(services, serviceProviderMaps, multiProviderInterfaces);
    }
    return std::make_shared<RegisteredGlobalServiceInfo>(serviceProviderMaps);
  }


  std::shared_ptr<ServiceProviderImpl> ServiceLauncher::Launch(const TypeServiceMaps& globalServiceTypeMaps, const RegisteredServiceDeque& services,
                                                               const bool clearOwnedUniqueServices)
  {
    TypeServiceMaps serviceProviderMaps(globalServiceTypeMaps);
    if (clearOwnedUniqueServices)
    {
      serviceProviderMaps.OwnedUniqueServices.clear();
    }

    if (!services.empty())
    {
      // Find all interfaces that have multiple providers
      std::set<std::type_index> multiProviderInterfaces;
      FindMultiProviderInterfaces(multiProviderInterfaces, services);

      StartService(services, serviceProviderMaps, multiProviderInterfaces);
    }
    return std::make_shared<ServiceProviderImpl>(serviceProviderMaps);
  }
}
