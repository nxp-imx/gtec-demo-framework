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

#include "AsynchronousServiceRegistry.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslService/Impl/Registry/InterfaceCollisionChecker.hpp>
#include <FslService/Impl/ServiceSupportedInterfaceDeque.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceFactory.hpp>
#include <algorithm>
#include <iterator>
#include <utility>
#include "ProviderIdGenerator.hpp"

namespace Fsl
{
  AsynchronousServiceRegistry::AsynchronousServiceRegistry(std::shared_ptr<ProviderIdGenerator> providerIdGenerator,
                                                           std::shared_ptr<InterfaceCollisionChecker> interfaceCollisionChecker,
                                                           const ServiceTypeEx serviceType)
    : m_providerIdGenerator(std::move(providerIdGenerator))
    , m_interfaceCollisionChecker(std::move(interfaceCollisionChecker))
    , TheServiceType(serviceType)
    , m_isLocked(false)
  {
  }


  AsynchronousServiceRegistry::~AsynchronousServiceRegistry() = default;


  void AsynchronousServiceRegistry::LockAndExtractServices(RegisteredAsynchronousServiceDeque& rServices,
                                                           ServiceOptionParserDeque& rServiceOptionParsers)
  {
    m_isLocked = true;
    std::copy(m_services.begin(), m_services.end(), std::back_inserter(rServices));
    std::copy(m_serviceOptionParsers.begin(), m_serviceOptionParsers.end(), std::back_inserter(rServiceOptionParsers));
  }


  void AsynchronousServiceRegistry::Register(const AsynchronousServiceFactory& factory, const Priority& priority)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("The services have already been started, so its too late to register new ones");
    }

    if (!factory.IsValid())
    {
      throw std::invalid_argument("factory must be valid");
    }

    ServiceSupportedInterfaceDeque deque;
    factory.FillInterfaceType(deque);
    if (deque.empty())
    {
      throw std::invalid_argument("The factory must support at least one interface");
    }

    m_interfaceCollisionChecker->AddInterfaces(deque, TheServiceType);

    // Retrieve any option parser associated with the service
    auto optionParser = factory.GetOptionParser();
    if (optionParser)
    {
      m_serviceOptionParsers.push_back(optionParser);
    }

    m_services.push_back(RegisteredAsynchronousServiceDeque::value_type(m_providerIdGenerator->NewId(), factory, priority));
  }
}
