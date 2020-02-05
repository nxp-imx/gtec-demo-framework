#ifndef FSLSERVICE_CONSUMER_ISERVICEPROVIDER_HPP
#define FSLSERVICE_CONSUMER_ISERVICEPROVIDER_HPP
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

#include <memory>
#include <FslService/Consumer/IBasicService.hpp>
#include <FslService/Consumer/BasicServiceDeque.hpp>

namespace Fsl
{
  class ProviderId;
  class ServiceId;

  class IServiceProvider
  {
  public:
    virtual ~IServiceProvider() = default;

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service if its available or null if not
    virtual std::shared_ptr<IBasicService> TryGet(const ServiceId& serviceId) const = 0;

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service or throws a exception if its not
    virtual std::shared_ptr<IBasicService> Get(const ServiceId& serviceId) const = 0;

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service if its available or null if not
    virtual std::shared_ptr<IBasicService> TryGet(const ServiceId& serviceId, const ProviderId& providerId) const = 0;

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service or throws a exception if its not
    virtual std::shared_ptr<IBasicService> Get(const ServiceId& serviceId, const ProviderId& providerId) const = 0;

    //! @brief Get all services that supports the given service id
    virtual void Get(BasicServiceDeque& rServices, const ServiceId& serviceId) const = 0;
  };
}

#endif
