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

#include <FslService/Impl/ServiceFramework.hpp>
#include <FslService/Impl/Foundation/Message/BasicMessageQueue.hpp>
#include <FslService/Impl/Foundation/Message/ThreadInitBasicMessage.hpp>
#include <FslService/Impl/Threading/IServiceHost.hpp>
#include <FslService/Impl/Threading/Launcher/ServiceLauncher.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <cassert>
#include "Registry/RegisteredServices.hpp"
#include "Registry/ServiceRegistryImpl.hpp"
#include "Threading/ServiceThreadManager.hpp"
#include "Threading/Launcher/RegisteredGlobalServiceInfo.hpp"

namespace Fsl
{
  ServiceFramework::ServiceFramework()
    : m_state(State::RegisterServices)
    , m_serviceRegistry(std::make_shared<ServiceRegistryImpl>())
    , m_threadManager(new ServiceThreadManager())
  {
  }


  ServiceFramework::~ServiceFramework() = default;


  std::weak_ptr<IServiceRegistry> ServiceFramework::GetServiceRegistry() const
  {
    return m_serviceRegistry;
  }


  void ServiceFramework::PrepareServices(ServiceOptionParserDeque& rServiceOptionParsers)
  {
    if (m_state != State::RegisterServices)
    {
      throw UsageErrorException("Expected to be in State::RegisterServices");
    }

    assert(m_serviceRegistry);
    assert(!m_registeredServices);
    assert(!m_mainHost);

    // Extract the registered services and their option parsers
    m_registeredServices = std::make_shared<RegisteredServices>();
    m_serviceRegistry->LockAndExtractServices(*m_registeredServices, rServiceOptionParsers);
    m_serviceRegistry.reset();

    m_state = State::ServicesPrepared;
  }


  void ServiceFramework::LaunchGlobalServices()
  {
    if (m_state != State::ServicesPrepared)
    {
      throw UsageErrorException("Expected to be in State::ServicesPrepared");
    }

    assert(!m_serviceRegistry);
    assert(m_registeredServices);
    assert(m_threadManager);
    assert(!m_mainHost);

    // Launch the global services
    m_registeredGlobalServiceInfo = ServiceLauncher::Launch(m_registeredServices->GlobalServices);
    assert(m_registeredGlobalServiceInfo);

    m_state = State::RunThreads;
  }


  void ServiceFramework::LaunchThreads()
  {
    if (m_state != State::RunThreads)
    {
      throw UsageErrorException("Expected to be in State::RunThreads");
    }

    assert(!m_serviceRegistry);
    assert(m_registeredServices);
    assert(m_threadManager);
    assert(!m_mainHost);
    assert(m_registeredGlobalServiceInfo);

    FSLLOG3_WARNING_IF(m_registeredServices->ServiceGroups.empty(), "No service groups exist, we would expect at least one for the main ");

    m_threadManager->PrepareServiceGroups(m_registeredServices->ServiceGroups);

    // Prepare the factories for the async proxy objects
    m_threadManager->PrepareAsyncProxyFactories(m_registeredGlobalServiceInfo->GlobalServiceTypeMaps);

    std::deque<CustomServiceHostRecord> customHosts;
    // Launch all managed 'service' threads
    m_threadManager->LaunchThreads(*m_registeredGlobalServiceInfo, customHosts);

    if (customHosts.empty() || customHosts.front().GroupType != ServiceGroupType::MainThread)
    {
      throw std::runtime_error("Did not find the main thread service custom host at the expected location");
    }

    m_mainHost = customHosts.front().ServiceHost;

    // Throw away all objects we don't need
    m_registeredServices.reset();
    m_registeredGlobalServiceInfo.reset();
    m_state = State::Running;
  }


  std::shared_ptr<IServiceProvider> ServiceFramework::GetServiceProvider() const
  {
    if (m_state != State::Running)
    {
      throw UsageErrorException("Expected to be in State::Running");
    }

    assert(m_mainHost);
    return m_mainHost->GetServiceProvider();
  }


  std::shared_ptr<IServiceHostLooper> ServiceFramework::GetServiceHostLooper() const
  {
    if (m_state != State::Running)
    {
      throw UsageErrorException("Expected to be in State::Running");
    }

    if (!m_mainHost)
    {
      throw std::runtime_error("internal error");
    }

    return m_mainHost;
  }
}
