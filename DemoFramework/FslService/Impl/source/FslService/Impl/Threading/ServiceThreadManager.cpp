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

#include "ServiceThreadManager.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslService/Impl/Foundation/Message/BasicMessageQueue.hpp>
#include <FslService/Impl/Registry/RegisteredServiceGroupDeque.hpp>
#include <FslService/Impl/ServiceSupportedInterfaceDeque.hpp>
#include <FslService/Impl/ServiceType/Async/IAsynchronousServiceProxyFactory.hpp>
// #include <experimental/future>
#include <cassert>
#include <exception>
#include <memory>
#include <thread>
#include "Launcher/AsynchronousServiceImplLaunchFactoryRecord.hpp"
#include "Launcher/AsynchronousServiceProxyLaunchFactory.hpp"
#include "Launcher/RegisteredGlobalServiceInfo.hpp"
#include "ServiceHost.hpp"
#include "ServiceHostCreateInfo.hpp"
#include "ServiceHostFactory.hpp"
#include "ThreadLocalServiceConfig.hpp"

namespace Fsl
{
  namespace
  {
    std::unique_ptr<ServiceThreadRecord> SpawnThread(const std::shared_ptr<IBasicMessageQueue>& ownerQueue,
                                                     const std::shared_ptr<BasicMessageQueue>& hostReceiveQueue,
                                                     const ThreadLocalServiceConfig& serviceConfig,
                                                     const std::shared_ptr<IServiceHostFactory>& serviceHostFactory)
    {
      return std::make_unique<ServiceThreadRecord>(ownerQueue, hostReceiveQueue, serviceConfig, serviceHostFactory);
    }


    std::deque<AsynchronousServiceImplLaunchFactoryRecord>
      BuildAsyncServiceImplLaunchFactoryRecordDeque(const RegisteredAsynchronousServiceDeque& asyncServices)
    {
      std::deque<AsynchronousServiceImplLaunchFactoryRecord> res;
      for (const auto& entry : asyncServices)
      {
        res.emplace_back(entry.Id, entry.Factory.GetImplFactory(), entry.StartupPriority);
      }
      return res;
    }


    CustomServiceHostRecord PrepareMainThread(const ServiceGroupId& id, const std::shared_ptr<BasicMessageQueue>& hostReceiveQueue,
                                              const RegisteredGlobalServiceInfo& globalServiceInfo, const RegisteredServiceGroupRecord& serviceGroup)
    {
      auto asyncLaunchRecords = BuildAsyncServiceImplLaunchFactoryRecordDeque(serviceGroup.AsyncServices);

      // Launch the local 'main thread' host instance
      ServiceHostContext hostContext(hostReceiveQueue);
      const ThreadLocalServiceConfig serviceConfig(id, globalServiceInfo.GlobalServiceTypeMaps, std::move(asyncLaunchRecords),
                                                   serviceGroup.ThreadLocalServices);
      ServiceHostCreateInfo createInfo(hostContext, serviceConfig);
      auto mainHost = std::make_shared<ServiceHost>(createInfo, false);
      return {serviceGroup.Type, mainHost, hostReceiveQueue};
    }
  }

  ServiceThreadManager::ServiceThreadManager() = default;


  ServiceThreadManager::~ServiceThreadManager() = default;


  void ServiceThreadManager::PrepareServiceGroups(const RegisteredServiceGroupDeque& serviceGroups)
  {
    if (m_state != State::Waiting)
    {
      throw UsageErrorException("Not in the right state");
    }

    m_hostRecords.clear();

    for (const auto& rEntry : serviceGroups)
    {
      if (rEntry.Type != ServiceGroupType::MainThread)
      {
        m_hostRecords.emplace_back(rEntry);
      }
      else
      {
        if (m_hostRecords.size() > 1 && m_hostRecords.front().Group.Type == ServiceGroupType::MainThread)
        {
          throw std::runtime_error("A group for the main thread has already been registered");
        }
        {
          m_hostRecords.emplace_back(rEntry);
        }
      }
    }

    if (m_hostRecords.empty() || m_hostRecords.front().Group.Type != ServiceGroupType::MainThread)
    {
      throw std::runtime_error("No group for the main thread was found");
    }

    m_state = State::PreparedServiceGroups;
  }


  void ServiceThreadManager::PrepareAsyncProxyFactories(TypeServiceMaps& rGlobalServiceTypeMaps)
  {
    if (m_state != State::PreparedServiceGroups)
    {
      throw UsageErrorException("Not in the right state");
    }

    ServiceSupportedInterfaceDeque serviceInterfaces;
    for (auto& rHostRecord : m_hostRecords)
    {
      rHostRecord.MessageQueue = std::make_shared<BasicMessageQueue>(rHostRecord.Group.Id);

      for (auto& rRecord : rHostRecord.Group.AsyncServices)
      {
        auto proxyFactory = rRecord.Factory.GetProxyFactory();

        serviceInterfaces.clear();
        proxyFactory->FillInterfaceType(serviceInterfaces);
        for (const auto& serviceInterfaceType : serviceInterfaces)
        {
          // Link the queue and the proxy factory so we can launch instances as needed
          auto launchFactory =
            std::make_shared<AsynchronousServiceProxyLaunchFactory>(rRecord.Id, rHostRecord.MessageQueue, rRecord.Factory.GetProxyFactory());

          rGlobalServiceTypeMaps.AddProvider(serviceInterfaceType,
                                             ServiceLaunchRecord(rRecord.Id, ServiceLaunchType::InstanceAllocator, launchFactory));
        }
      }
    }

    m_state = State::PreparedAsyncProxies;
  }


  void ServiceThreadManager::LaunchThreads(const RegisteredGlobalServiceInfo& globalServiceInfo, std::deque<CustomServiceHostRecord>& rCustomHosts)
  {
    if (m_state != State::PreparedAsyncProxies)
    {
      throw UsageErrorException("Not in the right state");
    }

    if (m_hostRecords.empty() || m_hostRecords.front().Group.Type != ServiceGroupType::MainThread)
    {
      throw UsageErrorException("the host records was invalid");
    }

    const auto serviceHostFactory = std::make_shared<ServiceHostFactory>();

    // pop the main thread record
    const auto mainThreadRecord = m_hostRecords.front();
    m_hostRecords.pop_front();

    auto mainHost = PrepareMainThread(mainThreadRecord.Group.Id, mainThreadRecord.MessageQueue, globalServiceInfo, mainThreadRecord.Group);
    rCustomHosts.push_back(mainHost);


    // Spawn all the required threads
    for (const auto& hostRecord : m_hostRecords)
    {
      auto asyncLaunchRecords = BuildAsyncServiceImplLaunchFactoryRecordDeque(hostRecord.Group.AsyncServices);

      const ThreadLocalServiceConfig serviceConfig(hostRecord.Group.Id, globalServiceInfo.GlobalServiceTypeMaps, std::move(asyncLaunchRecords),
                                                   hostRecord.Group.ThreadLocalServices);

      switch (hostRecord.Group.Type)
      {
      case ServiceGroupType::Managed:
        m_threadRecords.push_back(SpawnThread(mainHost.MessageQueue, hostRecord.MessageQueue, serviceConfig, serviceHostFactory));
        break;
      case ServiceGroupType::MainThread:
        throw std::runtime_error("we expected the main-thread type to have been removed");
      default:
        throw std::runtime_error("unknown group type");
      }
    }

    m_hostRecords.clear();
    m_state = State::Running;
  }
}
