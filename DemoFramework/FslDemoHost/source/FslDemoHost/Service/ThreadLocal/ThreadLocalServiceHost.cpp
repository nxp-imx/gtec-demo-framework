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

#include <FslDemoHost/Service/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Service/ServiceProvider.hpp>
#include <FslDemoHost/Service/ServiceProviderImpl.hpp>
#include <FslDemoHost/Service/ServiceSupportedInterfaceDeque.hpp>
#include <FslDemoHost/Service/ThreadLocal/IThreadLocalSingletonServiceFactory.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalServiceDeque.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalServiceHost.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalServiceRecord.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalSingletonServiceMap.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalSingletonServiceMapEx.hpp>
#include <algorithm>
#include <cassert>
#include <map>
#include <set>

namespace Fsl
{

  namespace
  {
    struct RecordComparator
    {
      bool operator()(const ThreadLocalServiceRecord& lhs, const ThreadLocalServiceRecord& rhs)
      {
        return (lhs.StartupPriority > rhs.StartupPriority);
      }
    };


    void CalcInterfaceHitCount(std::map<TypeInfo, int32_t>& rInterfaceMap, const ThreadLocalServiceDeque& services)
    {
      ThreadLocalServiceDeque::const_iterator itr = services.begin();
      const ThreadLocalServiceDeque::const_iterator itrEnd = services.end();
      ServiceSupportedInterfaceDeque interfaces;
      while (itr != itrEnd)
      {
        interfaces.clear();
        itr->Factory->FillInterfaceType(interfaces);
        assert(interfaces.size() > 0);

        // Increase the hit count for each interface provided by this service
        ServiceSupportedInterfaceDeque::const_iterator itrItf = interfaces.begin();
        const ServiceSupportedInterfaceDeque::const_iterator itrItfEnd = interfaces.end();
        while (itrItf != itrItfEnd)
        {
          std::map<TypeInfo, int32_t>::iterator itrFind = rInterfaceMap.find(*itrItf);
          if (itrFind != rInterfaceMap.end())
            ++itrFind->second;
          else
            rInterfaceMap[*itrItf] = 1;

          ++itrItf;
        }

        ++itr;
      }
    }


    void FindMultiProviderInterfaces(std::set<TypeInfo>& rMultiProviderInterfaces, const ThreadLocalServiceDeque& services)
    {
      std::map<TypeInfo, int32_t> interfaceMap;
      CalcInterfaceHitCount(interfaceMap, services);

      std::map<TypeInfo, int32_t>::const_iterator itr = interfaceMap.begin();
      const std::map<TypeInfo, int32_t>::const_iterator itrEnd = interfaceMap.end();
      while (itr != itrEnd)
      {
        if (itr->second > 1)
          rMultiProviderInterfaces.insert(itr->first);
        ++itr;
      }
    }


    std::shared_ptr<IService> StartService(std::set<TypeInfo>& multiProviderInterfaces, ServiceProvider& provider, ThreadLocalSingletonServiceMap& rSingletonServiceMap, ThreadLocalSingletonServiceMapEx& rSingletonServiceMapEx, const ThreadLocalServiceRecord& record)
    {
      ServiceSupportedInterfaceDeque deque;
      record.Factory->FillInterfaceType(deque);

      assert(deque.size() > 0);

      const std::shared_ptr<IService> service(record.Factory->Allocate(provider));
      if (!service)
      {
        if ((record.Factory->GetFlags() & ServiceCaps::AvailableOnDemand) != 0)
          return std::shared_ptr<IService>();

        const auto pFactory = record.Factory.get();
        const auto className = typeid(*pFactory).name();
        FSLLOG_ERROR("The service factory '" << className << "' returned a nullptr");
        throw InvalidServiceFactoryException(std::string("The service factory returned a null-ptr '") + className + "'");
      }

      // Give the object a chance to do something after construction (useful for shared_from_this)
      service->Link(provider);

      // Register the object on all interfaces
      ServiceSupportedInterfaceDeque::const_iterator itr = deque.begin();
      const ServiceSupportedInterfaceDeque::const_iterator itrEnd = deque.end();
      while (itr != itrEnd)
      {
        if (multiProviderInterfaces.find(itr->Get()) == multiProviderInterfaces.end())
        {
          assert(rSingletonServiceMap.find(itr->Get()) == rSingletonServiceMap.end());
          rSingletonServiceMap[itr->Get()] = service;
        }
        else
        {
          // Register a multiple provider interface
          rSingletonServiceMap[itr->Get()] = std::shared_ptr<IService>();

          std::shared_ptr<ServiceDeque> serviceDeque;
          ThreadLocalSingletonServiceMapEx::iterator itrFind = rSingletonServiceMapEx.find(itr->Get());
          if (itrFind == rSingletonServiceMapEx.end())
          {
            serviceDeque = std::make_shared<ServiceDeque>();
            rSingletonServiceMapEx[itr->Get()] = serviceDeque;
          }
          else
            serviceDeque = itrFind->second;

          serviceDeque->push_back(service);
        }

        ++itr;
      }
      return service;
    }

  }

  ThreadLocalServiceHost::ThreadLocalServiceHost(const ThreadLocalServiceDeque& services)
    : m_provider()
  {
    ThreadLocalSingletonServiceMap serviceMap;
    ThreadLocalSingletonServiceMapEx serviceMapEx;
    ServiceDeque uniqueServices;
    if (services.size() > 0)
    {
      // Find all interfaces that have multiple providers
      std::set<TypeInfo> multiProviderInterfaces;
      FindMultiProviderInterfaces(multiProviderInterfaces, services);

      // Sort the services according to priority
      ThreadLocalServiceDeque sortedServices(services);
      std::sort(sortedServices.begin(), sortedServices.end(), RecordComparator());

      auto provider = std::make_shared<ServiceProviderImpl>(serviceMap, serviceMapEx);
      Priority currentPriority = sortedServices.front().StartupPriority;

      ThreadLocalServiceDeque::const_iterator itr = sortedServices.begin();
      const ThreadLocalServiceDeque::const_iterator itrEnd = sortedServices.end();

      ServiceProvider theServiceProvider(provider);
      while (itr != itrEnd)
      {
        // Each time we change priority we recreate the service provider to enable access to all higher priority services
        if (itr->StartupPriority != currentPriority)
        {
          currentPriority = itr->StartupPriority;
          provider = std::make_shared<ServiceProviderImpl>(serviceMap, serviceMapEx);
          theServiceProvider = ServiceProvider(provider);
        }

        try
        {
          const std::shared_ptr<IService> service = StartService(multiProviderInterfaces, theServiceProvider, serviceMap, serviceMapEx, *itr);
          if (service)
            uniqueServices.push_back(service);
        }
        catch (const std::exception& ex)
        {
          if ((itr->Factory->GetFlags() & ServiceCaps::Optional) != 0)
          {
            const auto pFactory = itr->Factory.get();
            const auto className = typeid(*pFactory).name();
            FSLLOG_WARNING("Service provided by '" << className << "' failed to start: " << ex.what());
          }
          else
            throw;
        }
        ++itr;
      }
      provider.reset();
    }
    m_provider = std::make_shared<ServiceProviderImpl>(serviceMap, serviceMapEx, uniqueServices);
  }


  ThreadLocalServiceHost::~ThreadLocalServiceHost()
  {

  }


  const std::shared_ptr<ServiceProviderImpl> ThreadLocalServiceHost::GetProvider()
  {
    return m_provider;
  }
}
