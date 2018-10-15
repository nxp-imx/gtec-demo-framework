#ifndef FSLSERVICE_IMPL_REGISTRY_GENERICSERVICEREGISTRYIMPL_HPP
#define FSLSERVICE_IMPL_REGISTRY_GENERICSERVICEREGISTRYIMPL_HPP
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

#include <FslBase/RTTI/TypeInfo.hpp>
#include <FslService/Impl/Exceptions.hpp>
#include <FslService/Impl/Registry/InterfaceCollisionChecker.hpp>
#include <FslService/Impl/Registry/RegisteredServiceDeque.hpp>
#include <FslService/Impl/Registry/ServiceTypeEx.hpp>
#include <FslService/Impl/ServiceOptionParserDeque.hpp>
#include <FslService/Impl/ServiceSupportedInterfaceDeque.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <map>
#include <iterator>
#include <utility>
#include "ProviderIdGenerator.hpp"

namespace Fsl
{
  template <typename TFactory>
  class GenericServiceRegistryImpl
  {
    std::shared_ptr<ProviderIdGenerator> m_providerIdGenerator;
    std::shared_ptr<InterfaceCollisionChecker> m_interfaceCollisionChecker;
    const ServiceTypeEx TheServiceType;
    bool m_isLocked;
    RegisteredServiceDeque m_services;
    ServiceOptionParserDeque m_serviceOptionParsers;

  public:
    GenericServiceRegistryImpl(std::shared_ptr<ProviderIdGenerator> providerIdGenerator,
                               std::shared_ptr<InterfaceCollisionChecker> interfaceCollisionChecker, const ServiceTypeEx serviceType)
      : m_providerIdGenerator(std::move(providerIdGenerator))
      , m_interfaceCollisionChecker(std::move(interfaceCollisionChecker))
      , TheServiceType(serviceType)
      , m_isLocked(false)

    {
    }

    void LockAndExtractServices(RegisteredServiceDeque& rServices, ServiceOptionParserDeque& rServiceOptionParsers)
    {
      m_isLocked = true;
      std::copy(m_services.begin(), m_services.end(), std::back_inserter(rServices));
      std::copy(m_serviceOptionParsers.begin(), m_serviceOptionParsers.end(), std::back_inserter(rServiceOptionParsers));
    }

    //! @brief Register a service with the given priority
    void Register(const std::shared_ptr<TFactory>& factory, const Priority& priority)
    {
      if (m_isLocked)
      {
        throw UsageErrorException("The services have already been started, so its too late to register new ones");
      }

      if (!factory)
      {
        throw std::invalid_argument("factory can not be null");
      }

      ServiceSupportedInterfaceDeque deque;
      factory->FillInterfaceType(deque);
      if (deque.empty())
      {
        throw std::invalid_argument("The factory must support at least one interface");
      }

      m_interfaceCollisionChecker->AddInterfaces(deque, TheServiceType);

      // Retrieve any option parser associated with the service
      auto optionParser = factory->GetOptionParser();
      if (optionParser)
      {
        m_serviceOptionParsers.push_back(optionParser);
      }

      m_services.push_back(RegisteredServiceDeque::value_type(m_providerIdGenerator->NewId(), factory, priority));
    }
  };
}

#endif
