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

#include "ServiceRegistryImpl.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslService/Impl/Registry/InterfaceCollisionChecker.hpp>
#include <FslService/Impl/Exceptions.hpp>
#include <FslService/Impl/Priority.hpp>
#include "ProviderIdGenerator.hpp"
#include "RegisteredServices.hpp"
#include <algorithm>
#include <cassert>

namespace Fsl
{
  ServiceRegistryImpl::ServiceRegistryImpl()
    : m_providerIdGenerator(std::make_shared<ProviderIdGenerator>())
    , m_interfaceCollisionChecker(std::make_shared<InterfaceCollisionChecker>())
    , m_threadGlobalServiceRegistry(m_providerIdGenerator, m_interfaceCollisionChecker, ServiceTypeEx::ThreadSafeSynchronous)

  {
    // End by creating the main thread group
    m_mainThreadGroup = DoCreateServiceGroup(ServiceGroupType::MainThread);
  }


  ServiceRegistryImpl::~ServiceRegistryImpl() = default;


  ServiceGroupId ServiceRegistryImpl::CreateServiceGroup()
  {
    auto serviceGroupRegistry = DoCreateServiceGroup(ServiceGroupType::Managed);
    return serviceGroupRegistry->Id;
  }


  ServiceGroupId ServiceRegistryImpl::CreateServiceGroup(const uint32_t groupNameUniqueId)
  {
    auto itrFind = std::find_if(m_nameToGroup.begin(), m_nameToGroup.end(),
                                [groupNameUniqueId](const ServiceGroupRecord& val) { return (val.UniqueName == groupNameUniqueId); });
    if (itrFind != m_nameToGroup.end())
    {
      throw UsageErrorException("The unique group name has already been registered");
    }

    auto serviceGroupRegistry = DoCreateServiceGroup(ServiceGroupType::Managed);

    m_nameToGroup.emplace_back(groupNameUniqueId, serviceGroupRegistry->Id);
    return serviceGroupRegistry->Id;
  }


  ServiceGroupId ServiceRegistryImpl::GetMainServiceGroup() const
  {
    return m_mainThreadGroup->Id;
  }


  ServiceGroupId ServiceRegistryImpl::GetServiceGroupByName(const uint32_t groupNameUniqueId) const
  {
    auto itrFind = std::find_if(m_nameToGroup.begin(), m_nameToGroup.end(),
                                [groupNameUniqueId](const ServiceGroupRecord& val) { return (val.UniqueName == groupNameUniqueId); });
    if (itrFind == m_nameToGroup.end())
    {
      throw NotFoundException("GetServiceGroupByName the name was not found");
    }

    return itrFind->GroupId;
  }


  void ServiceRegistryImpl::SetServiceGroupName(const ServiceGroupId& serviceGroupId, const uint32_t groupNameUniqueId)
  {
    if (!serviceGroupId.IsValid())
    {
      throw std::invalid_argument("ServiceGroupId must be valid");
    }

    auto itrFind = std::find_if(m_nameToGroup.begin(), m_nameToGroup.end(),
                                [groupNameUniqueId](const ServiceGroupRecord& val) { return (val.UniqueName == groupNameUniqueId); });
    if (itrFind != m_nameToGroup.end())
    {
      throw UsageErrorException("The unique group name has already been registered");
    }

    m_nameToGroup.emplace_back(groupNameUniqueId, serviceGroupId);
  }


  void ServiceRegistryImpl::LockAndExtractServices(RegisteredServices& rServices, ServiceOptionParserDeque& rServiceOptionParsers)
  {
    m_threadGlobalServiceRegistry.LockAndExtractServices(rServices.GlobalServices, rServiceOptionParsers);

    for (auto& serviceGroup : m_serviceGroups)
    {
      RegisteredServiceGroupRecord group(serviceGroup->Id, serviceGroup->Type);

      serviceGroup->AsyncServiceRegistry.LockAndExtractServices(group.AsyncServices, rServiceOptionParsers);
      serviceGroup->ThreadLocalServiceRegistry.LockAndExtractServices(group.ThreadLocalServices, rServiceOptionParsers);

      rServices.ServiceGroups.push_back(std::move(group));
    }
  }


  void ServiceRegistryImpl::Register(const std::shared_ptr<IThreadLocalSingletonServiceFactory>& factory, const Priority& priority,
                                     const ServiceGroupId& serviceGroupId)
  {
    const std::shared_ptr<ServiceGroupRegistry> finalServiceGroup = !serviceGroupId.IsValid() ? m_mainThreadGroup : GetServiceGroup(serviceGroupId);

    finalServiceGroup->ThreadLocalServiceRegistry.Register(factory, priority);
  }


  void ServiceRegistryImpl::Register(const std::shared_ptr<IThreadSafeSynchronousServiceFactory>& factory, const Priority& priority)
  {
    m_threadGlobalServiceRegistry.Register(factory, priority);
  }


  void ServiceRegistryImpl::Register(const AsynchronousServiceFactory& factory, const Priority& priority, const ServiceGroupId& serviceGroupId)
  {
    const std::shared_ptr<ServiceGroupRegistry> finalServiceGroup =
      serviceGroupId.IsValid() ? GetServiceGroup(serviceGroupId) : DoCreateServiceGroup(ServiceGroupType::Managed);

    finalServiceGroup->AsyncServiceRegistry.Register(factory, priority);
  }


  std::shared_ptr<ServiceGroupRegistry> ServiceRegistryImpl::DoCreateServiceGroup(const ServiceGroupType serviceGroupType)
  {
    // We utilize this very simple pattern as we require all services to be registered before launch and we don't support unregister (static pattern)
    ++m_serviceGroupId;
    if (m_serviceGroupId == ServiceGroupId::INVALID)
    {
      throw std::runtime_error("Internal error, the group id counter wrapped around (usage pattern different than expected)");
    }

    // TODO: m_collisionDict really needs to be 'global+async' and then one 'thread-local' for each service group
    //       This will allow us to have thread-local services that expose the same interface inside all serviceGroups
    ServiceGroupId serviceGroupId(m_serviceGroupId);
    auto serviceGroup = std::make_shared<ServiceGroupRegistry>(serviceGroupId, serviceGroupType, m_providerIdGenerator, m_interfaceCollisionChecker);

    m_serviceGroups.push_back(serviceGroup);
    return serviceGroup;
  }


  std::shared_ptr<ServiceGroupRegistry> ServiceRegistryImpl::GetServiceGroup(const ServiceGroupId& serviceGroupId)
  {
    const auto id = serviceGroupId.GetValue();
    auto compareFunc = [id](const std::shared_ptr<ServiceGroupRegistry>& val) {
      assert(val);
      return (val->Id.GetValue() == id);
    };

    const auto itrFind = std::find_if(m_serviceGroups.begin(), m_serviceGroups.end(), compareFunc);
    if (itrFind == m_serviceGroups.end())
    {
      throw std::runtime_error("Unknown serviceGroupId");
    }
    return *itrFind;
  }
}
