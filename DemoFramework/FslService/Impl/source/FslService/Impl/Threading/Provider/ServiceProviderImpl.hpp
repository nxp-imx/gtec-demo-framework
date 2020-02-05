#ifndef FSLSERVICE_IMPL_THREADING_PROVIDER_SERVICEPROVIDERIMPL_HPP
#define FSLSERVICE_IMPL_THREADING_PROVIDER_SERVICEPROVIDERIMPL_HPP
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

#include <FslService/Consumer/IServiceProvider.hpp>
#include <map>
#include "../Launcher/TypeServiceMaps.hpp"

namespace Fsl
{
  class ServiceProviderImpl : public IServiceProvider
  {
    TypeServiceMaps m_serviceProviderMaps;

  public:
    ServiceProviderImpl(TypeServiceMaps serviceProviderMaps);
    ~ServiceProviderImpl() override;

    void Update();

    // From IServiceProvider
    std::shared_ptr<IBasicService> TryGet(const ServiceId& serviceId) const override;
    std::shared_ptr<IBasicService> Get(const ServiceId& serviceId) const override;
    std::shared_ptr<IBasicService> TryGet(const ServiceId& serviceId, const ProviderId& providerId) const override;
    std::shared_ptr<IBasicService> Get(const ServiceId& serviceId, const ProviderId& providerId) const override;
    void Get(BasicServiceDeque& rServices, const ServiceId& serviceId) const override;

  private:
    ServiceLaunchRecord TryGetNow(const ServiceId& serviceId) const;
    ServiceLaunchRecord TryGetNow(const ServiceId& serviceId, const ProviderId& providerId) const;
  };
}

#endif
