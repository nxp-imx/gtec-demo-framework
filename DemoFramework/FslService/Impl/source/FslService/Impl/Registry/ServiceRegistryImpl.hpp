#ifndef FSLSERVICE_IMPL_REGISTRY_SERVICEREGISTRYIMPL_HPP
#define FSLSERVICE_IMPL_REGISTRY_SERVICEREGISTRYIMPL_HPP
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

#include <FslService/Impl/Priority.hpp>
#include <FslService/Impl/Registry/IServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Global/IThreadSafeSynchronousServiceFactory.hpp>
#include <deque>
#include "GenericServiceRegistryImpl.hpp"
#include "ServiceGroupRegistry.hpp"

namespace Fsl
{
  class InterfaceCollisionChecker;
  class ProviderIdGenerator;
  class ServiceOptionParserDeque;
  struct RegisteredServices;

  class ServiceRegistryImpl : public IServiceRegistry
  {
    struct ServiceGroupRecord
    {
      uint32_t UniqueName{0};
      ServiceGroupId GroupId;

      ServiceGroupRecord() = default;

      ServiceGroupRecord(const uint32_t uniqueName, const ServiceGroupId& groupId)
        : UniqueName(uniqueName)
        , GroupId(groupId)
      {
      }
    };

    std::shared_ptr<ProviderIdGenerator> m_providerIdGenerator;
    std::shared_ptr<InterfaceCollisionChecker> m_interfaceCollisionChecker;
    GenericServiceRegistryImpl<IThreadSafeSynchronousServiceFactory> m_threadGlobalServiceRegistry;
    std::deque<std::shared_ptr<ServiceGroupRegistry>> m_serviceGroups;
    uint32_t m_serviceGroupId{0};

    std::shared_ptr<ServiceGroupRegistry> m_mainThreadGroup;


    std::deque<ServiceGroupRecord> m_nameToGroup;

  public:
    ServiceRegistryImpl();
    ~ServiceRegistryImpl() override;

    void LockAndExtractServices(RegisteredServices& rServices, ServiceOptionParserDeque& rServiceOptionParsers);

    // From IServiceRegistry
    ServiceGroupId CreateServiceGroup() override;
    ServiceGroupId CreateServiceGroup(const uint32_t groupNameUniqueId) override;
    ServiceGroupId GetMainServiceGroup() const override;
    ServiceGroupId GetServiceGroupByName(const uint32_t groupNameUniqueId) const override;
    void SetServiceGroupName(const ServiceGroupId& serviceGroupId, const uint32_t groupNameUniqueId) override;
    void Register(const std::shared_ptr<IThreadLocalSingletonServiceFactory>& factory, const Priority& priority = Priority(),
                  const ServiceGroupId& serviceGroupId = ServiceGroupId()) override;
    void Register(const std::shared_ptr<IThreadSafeSynchronousServiceFactory>& factory, const Priority& priority = Priority()) override;
    void Register(const AsynchronousServiceFactory& factory, const Priority& priority = Priority(),
                  const ServiceGroupId& serviceGroupId = ServiceGroupId()) override;

  private:
    std::shared_ptr<ServiceGroupRegistry> DoCreateServiceGroup(const ServiceGroupType serviceGroupType);
    std::shared_ptr<ServiceGroupRegistry> GetServiceGroup(const ServiceGroupId& serviceGroupId);
  };
}

#endif
