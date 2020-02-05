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

#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Consumer/Exceptions.hpp>
#include <FslService/Consumer/IServiceProvider.hpp>
#include <FslBase/Log/Log3Core.hpp>

namespace Fsl
{
  ServiceProvider::ServiceProvider(const std::weak_ptr<IServiceProvider>& serviceProvider)
    : m_serviceProvider(serviceProvider)
  {
  }


  ServiceProvider::~ServiceProvider() = default;


  std::shared_ptr<IBasicService> ServiceProvider::TryGet(const ServiceId& serviceId) const
  {
    const std::shared_ptr<IServiceProvider> serviceProvider = m_serviceProvider.lock();
    if (!serviceProvider)
    {
      FSLLOG3_WARNING("Service provider unavailable, therefore the service is unavailable");
      return std::shared_ptr<IService>();
    }

    return serviceProvider->TryGet(serviceId);
  }


  std::shared_ptr<IBasicService> ServiceProvider::Get(const ServiceId& serviceId) const
  {
    const std::shared_ptr<IServiceProvider> serviceProvider = m_serviceProvider.lock();
    if (!serviceProvider)
    {
      throw ServiceProviderUnavailableException("The service provider is unavailable");
    }
    return serviceProvider->Get(serviceId);
  }


  void ServiceProvider::Get(BasicServiceDeque& rServices, const ServiceId& serviceId) const
  {
    const std::shared_ptr<IServiceProvider> serviceProvider = m_serviceProvider.lock();
    if (!serviceProvider)
    {
      throw ServiceProviderUnavailableException("The service provider is unavailable");
    }
    return serviceProvider->Get(rServices, serviceId);
  }
}
