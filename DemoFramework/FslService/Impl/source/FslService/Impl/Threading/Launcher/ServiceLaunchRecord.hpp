#ifndef FSLSERVICE_IMPL_THREADING_LAUNCHER_SERVICELAUNCHRECORD_HPP
#define FSLSERVICE_IMPL_THREADING_LAUNCHER_SERVICELAUNCHRECORD_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslService/Consumer/IService.hpp>
#include <FslService/Consumer/ProviderId.hpp>
#include <FslService/Impl/Threading/Launcher/ServiceLaunchType.hpp>
#include <memory>
#include <utility>
#include "IServiceLaunchInstanceFactory.hpp"

namespace Fsl
{
  struct ServiceLaunchRecord
  {
    ProviderId TheProviderId;
    ServiceLaunchType LaunchType{ServiceLaunchType::Invalid};
    std::shared_ptr<IService> Service;
    std::shared_ptr<IServiceLaunchInstanceFactory> InstanceFactory;

    ServiceLaunchRecord() = default;


    ServiceLaunchRecord(const ProviderId& providerId, const ServiceLaunchType launchType)
      : TheProviderId(providerId)
      , LaunchType(launchType)

    {
      if (!IsValid())
      {
        throw UsageErrorException();
      }
    }


    ServiceLaunchRecord(const ProviderId& providerId, const ServiceLaunchType launchType, std::shared_ptr<IService> service)
      : TheProviderId(providerId)
      , LaunchType(launchType)
      , Service(std::move(service))
    {
      if (!IsValid())
      {
        throw UsageErrorException();
      }
    }


    ServiceLaunchRecord(const ProviderId& providerId, const ServiceLaunchType launchType,
                        std::shared_ptr<IServiceLaunchInstanceFactory> instanceFactory)
      : TheProviderId(providerId)
      , LaunchType(launchType)
      , InstanceFactory(std::move(instanceFactory))
    {
      if (!IsValid())
      {
        throw UsageErrorException();
      }
    }


    bool IsValid() const
    {
      if (!TheProviderId.IsValid() && LaunchType != ServiceLaunchType::MultipleProviderTag)
      {
        return false;
      }
      if (LaunchType == ServiceLaunchType::Invalid || LaunchType == ServiceLaunchType::MultipleProviderTag)
      {
        return Service == nullptr;
      }
      if (LaunchType == ServiceLaunchType::Instance)
      {
        return Service != nullptr;
      }
      if (LaunchType == ServiceLaunchType::InstanceAllocator)
      {
        return InstanceFactory != nullptr;
      }
      return false;
    }
  };
}

#endif
