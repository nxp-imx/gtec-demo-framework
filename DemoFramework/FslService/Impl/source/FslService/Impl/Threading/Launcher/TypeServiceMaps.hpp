#ifndef FSLSERVICE_IMPL_THREADING_LAUNCHER_TYPESERVICEMAPS_HPP
#define FSLSERVICE_IMPL_THREADING_LAUNCHER_TYPESERVICEMAPS_HPP
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

#include <utility>
#include "TypeServiceMap.hpp"
#include "TypeServiceMapEx.hpp"
#include "ServiceDeque.hpp"

namespace Fsl
{
  struct TypeServiceMaps
  {
    //! Maps a type to the single service that implements it
    TypeServiceMap InterfaceToService;

    //! Maps a type to the multiple services that implement it
    TypeServiceMapEx InterfaceMultipleServices;

    //! A list of all unique services instances owned by this thread, this can be used to ensure that something is called
    //! on all running services. The main thread contains the global thread safe services.
    ServiceDeque OwnedUniqueServices;


    TypeServiceMaps() = default;


    TypeServiceMaps(TypeServiceMap interfaceOneProvider, TypeServiceMapEx interfaceMultipleProviders, ServiceDeque ownedUniqueServices)
      : InterfaceToService(std::move(interfaceOneProvider))
      , InterfaceMultipleServices(std::move(interfaceMultipleProviders))
      , OwnedUniqueServices(std::move(ownedUniqueServices))
    {
    }


    TypeServiceMaps(TypeServiceMap&& interfaceOneProvider, TypeServiceMapEx&& interfaceMultipleProviders, ServiceDeque&& globalUniqueServices)
      : InterfaceToService(std::move(interfaceOneProvider))
      , InterfaceMultipleServices(std::move(interfaceMultipleProviders))
      , OwnedUniqueServices(std::move(globalUniqueServices))
    {
    }


    void AddProvider(const TypeInfo& serviceInterfaceType, const ServiceLaunchRecord& launchRecord)
    {
      auto itrFind = InterfaceToService.find(serviceInterfaceType);
      if (itrFind != InterfaceToService.end())
      {
        // A existing provider of the interface was found
        if (itrFind->second.LaunchType == ServiceLaunchType::MultipleProviderTag)
        {
          // A interface with multiple providers, so we just extend it
          auto itrFindMulti = InterfaceMultipleServices.find(serviceInterfaceType);
          if (itrFindMulti == InterfaceMultipleServices.end() || !itrFindMulti->second)
          {
            throw std::runtime_error("Internal error, the multi provider entry was not found");
          }

          itrFindMulti->second->push_back(launchRecord);
        }
        else
        {
          // Not a multi provider, so we need to change it to one
          auto itrFindMulti = InterfaceMultipleServices.find(serviceInterfaceType);
          if (itrFindMulti != InterfaceMultipleServices.end())
          {
            throw std::runtime_error("Internal error, a existing multi provider entry was found");
          }

          // Create a new multi provider deque and register it, then register the existing entry and the new one in it
          // Then patch the lookup so its indicated there is multiple providers now.
          auto newDeque = std::make_shared<std::deque<ServiceLaunchRecord>>();
          InterfaceMultipleServices[serviceInterfaceType] = newDeque;
          newDeque->push_back(itrFind->second);
          newDeque->push_back(launchRecord);

          InterfaceToService[serviceInterfaceType] = ServiceLaunchRecord(ProviderId::Invalid(), ServiceLaunchType::MultipleProviderTag);
        }
      }
      else
      {
        // No provider
        InterfaceToService[serviceInterfaceType] = launchRecord;
      }
    }
  };
}

#endif
