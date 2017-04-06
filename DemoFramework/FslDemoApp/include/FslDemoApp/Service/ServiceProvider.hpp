#ifndef FSLDEMOAPP_SERVICE_SERVICEPROVIDER_HPP
#define FSLDEMOAPP_SERVICE_SERVICEPROVIDER_HPP
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
#include <FslDemoApp/Service/IService.hpp>
#include <FslDemoApp/Service/ServiceDeque.hpp>
#include <FslDemoApp/Service/ServiceId.hpp>

namespace Fsl
{
  class IServiceProvider;

  class ServiceProvider
  {
    std::weak_ptr<IServiceProvider> m_serviceProvider;
  public:
    ServiceProvider(const std::weak_ptr<IServiceProvider>& serviceProvider);
    ~ServiceProvider();

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service if its available or null if not
    std::shared_ptr<IService> TryGet(const ServiceId& serviceId) const;

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service or throws a exception if its not
    std::shared_ptr<IService> Get(const ServiceId& serviceId) const;

    //! @brief Get all services that supports the given service id
    void Get(ServiceDeque& rServices, const ServiceId& serviceId) const;

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service if its available or null if not
    template<typename T>
    std::shared_ptr<T> TryGet() const
    {
      return std::dynamic_pointer_cast<T>(TryGet(ServiceId(typeid(T))));
    }

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service or throws a exception if its not
    template<typename T>
    std::shared_ptr<T> Get() const
    {
      return std::dynamic_pointer_cast<T>(Get(ServiceId(typeid(T))));
    }

    //! @brief Retrieve a service of the given type (if available)
    //! @return the service or throws a exception if its not
    template<typename T>
    void Get(std::deque<std::shared_ptr<T> >& rServices) const
    {
      rServices.clear();
      ServiceDeque services;
      Get(services, ServiceId(typeid(T)));

      // Dynamic cast all the returned services to the expected type
      ServiceDeque::const_iterator itr = services.begin();
      while (itr != services.end())
      {
        rServices.push_back(std::dynamic_pointer_cast<T>(*itr));
        ++itr;
      }
    }

  };
}

#endif
