#ifndef FSLSERVICE_IMPL_REGISTRY_ISERVICEREGISTRY_HPP
#define FSLSERVICE_IMPL_REGISTRY_ISERVICEREGISTRY_HPP
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

#include <FslBase/Noncopyable.hpp>
#include <FslService/Impl/Priority.hpp>
#include <FslService/Impl/Registry/ServiceGroupId.hpp>
#include <memory>

namespace Fsl
{
  class IThreadLocalSingletonServiceFactory;
  class IThreadSafeSynchronousServiceFactory;
  class AsynchronousServiceFactory;
  class Priority;

  class IServiceRegistry : private Noncopyable
  {
  public:
    virtual ~IServiceRegistry() = default;

    //! @brief Create a service group that can be used to group services
    virtual ServiceGroupId CreateServiceGroup() = 0;

    //! @brief Create a service group that can be used to group services and give it a unique name that can be used to query the ServiceGroupId.
    virtual ServiceGroupId CreateServiceGroup(const uint32_t groupNameUniqueId) = 0;

    //! @brief Get the service group belonging to the main thread
    virtual ServiceGroupId GetMainServiceGroup() const = 0;

    //! @brief Associate a name with a existing unnamed service group
    virtual ServiceGroupId GetServiceGroupByName(const uint32_t groupNameUniqueId) const = 0;

    //! @brief Create a service group that can be used to group services and give it a unique name that can be used to query the ServiceGroupId.
    virtual void SetServiceGroupName(const ServiceGroupId& serviceGroupId, const uint32_t groupNameUniqueId) = 0;

    //! @brief Register a service with a specific priority
    virtual void Register(const std::shared_ptr<IThreadLocalSingletonServiceFactory>& factory, const Priority& priority = Priority(),
                          const ServiceGroupId& serviceGroupId = ServiceGroupId()) = 0;

    //! @brief Register a global thread-safe service with a specific priority
    virtual void Register(const std::shared_ptr<IThreadSafeSynchronousServiceFactory>& factory, const Priority& priority = Priority()) = 0;

    //! @brief Register a global thread-safe service with a a specific priority in a specific thread group
    virtual void Register(const AsynchronousServiceFactory& factory, const Priority& priority = Priority(),
                          const ServiceGroupId& serviceGroupId = ServiceGroupId()) = 0;
  };
}

#endif
