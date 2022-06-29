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

#include <FslService/Impl/Exceptions.hpp>
#include <FslService/Impl/Registry/IServiceRegistry.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <utility>

namespace Fsl
{
  ServiceRegistry::ServiceRegistry() = default;


  ServiceRegistry::ServiceRegistry(std::weak_ptr<IServiceRegistry> registry)
    : m_registry(std::move(registry))
  {
  }


  ServiceRegistry::~ServiceRegistry() = default;


  ServiceGroupId ServiceRegistry::CreateServiceGroup()
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    return registry->CreateServiceGroup();
  }


  ServiceGroupId ServiceRegistry::CreateServiceGroup(const uint32_t groupNameUniqueId)
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    return registry->CreateServiceGroup(groupNameUniqueId);
  }


  ServiceGroupId ServiceRegistry::GetMainServiceGroup() const
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    return registry->GetMainServiceGroup();
  }


  ServiceGroupId ServiceRegistry::GetServiceGroupByName(const uint32_t groupNameUniqueId) const
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    return registry->GetServiceGroupByName(groupNameUniqueId);
  }


  void ServiceRegistry::SetServiceGroupName(const ServiceGroupId& serviceGroupId, const uint32_t groupNameUniqueId)
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    return registry->SetServiceGroupName(serviceGroupId, groupNameUniqueId);
  }


  void ServiceRegistry::Register(const std::shared_ptr<IThreadLocalSingletonServiceFactory>& factory, const Priority& priority,
                                 const ServiceGroupId& serviceGroup)
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    registry->Register(factory, priority, serviceGroup);
  }


  void ServiceRegistry::Register(const std::shared_ptr<IThreadSafeSynchronousServiceFactory>& factory, const Priority& priority)
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    registry->Register(factory, priority);
  }


  void ServiceRegistry::Register(const AsynchronousServiceFactory& factory, const Priority& priority, const ServiceGroupId& serviceGroup)
  {
    std::shared_ptr<IServiceRegistry> registry = m_registry.lock();
    if (!registry)
    {
      throw ServiceRegistryUnavailableException("");
    }
    registry->Register(factory, priority, serviceGroup);
  }
}
