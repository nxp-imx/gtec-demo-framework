#ifndef FSLSERVICE_IMPL_SERVICEFRAMEWORK_HPP
#define FSLSERVICE_IMPL_SERVICEFRAMEWORK_HPP
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

#include <memory>

namespace Fsl
{
  class IServiceHost;
  class IServiceHostLooper;
  class IServiceProvider;
  class IServiceRegistry;
  struct RegisteredGlobalServiceInfo;
  struct RegisteredServices;
  class ServiceOptionParserDeque;
  class ServiceRegistryImpl;
  class ServiceThreadManager;

  class ServiceFramework
  {
    enum class State
    {
      RegisterServices,
      ServicesPrepared,
      RunThreads,
      Running,
    };

    State m_state;
    std::shared_ptr<ServiceRegistryImpl> m_serviceRegistry;
    std::shared_ptr<RegisteredServices> m_registeredServices;

    std::shared_ptr<RegisteredGlobalServiceInfo> m_registeredGlobalServiceInfo;

    std::unique_ptr<ServiceThreadManager> m_threadManager;
    std::shared_ptr<IServiceHost> m_mainHost;

  public:
    ServiceFramework();
    ~ServiceFramework();

    std::weak_ptr<IServiceRegistry> GetServiceRegistry() const;

    void PrepareServices(ServiceOptionParserDeque& rServiceOptionParsers);
    void LaunchGlobalServices();
    void LaunchThreads();

    std::shared_ptr<IServiceProvider> GetServiceProvider() const;

    //! @brief Get the service host looper (can be called when the framework is running)
    std::shared_ptr<IServiceHostLooper> GetServiceHostLooper() const;
  };
}

#endif
