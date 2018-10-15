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

#include <FslService/Impl/ServiceType/Async/AsynchronousServiceFactory.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslService/Impl/ServiceType/Async/IAsynchronousServiceProxyFactory.hpp>
#include <FslService/Impl/ServiceType/Async/IAsynchronousServiceImplFactory.hpp>

namespace Fsl
{
  AsynchronousServiceFactory::AsynchronousServiceFactory() = default;


  AsynchronousServiceFactory::AsynchronousServiceFactory(const std::shared_ptr<IAsynchronousServiceProxyFactory>& proxyFactory,
                                                         const std::shared_ptr<IAsynchronousServiceImplFactory>& implFactory)
    : m_proxyFactory(proxyFactory)
    , m_implFactory(implFactory)
  {
    if (!proxyFactory || !implFactory)
    {
      throw std::invalid_argument("can not be null");
    }
  }


  AsynchronousServiceFactory::~AsynchronousServiceFactory() = default;


  std::shared_ptr<IAsynchronousServiceProxyFactory> AsynchronousServiceFactory::GetProxyFactory() const
  {
    return m_proxyFactory;
  }


  std::shared_ptr<IAsynchronousServiceImplFactory> AsynchronousServiceFactory::GetImplFactory() const
  {
    return m_implFactory;
  }


  std::shared_ptr<AServiceOptionParser> AsynchronousServiceFactory::GetOptionParser() const
  {
    return m_proxyFactory->GetOptionParser();
  }


  ServiceCaps::Flags AsynchronousServiceFactory::GetFlags() const
  {
    return m_proxyFactory->GetFlags();
  }

  void AsynchronousServiceFactory::FillInterfaceType(ServiceSupportedInterfaceDeque& rServiceInterfaceTypeDeque) const
  {
    return m_proxyFactory->FillInterfaceType(rServiceInterfaceTypeDeque);
  }
}
