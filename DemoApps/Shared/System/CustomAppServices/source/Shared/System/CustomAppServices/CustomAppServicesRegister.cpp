/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceFactory.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImplFactoryTemplate.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceProxyFactoryTemplate.hpp>
#include <FslService/Impl/ServiceType/Global/ThreadSafeSynchronousServiceTemplate.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <Shared/System/CustomAppServices/Async/AsyncExampleServiceImpl.hpp>
#include <Shared/System/CustomAppServices/Async/AsyncExampleServiceProxy.hpp>
#include <Shared/System/CustomAppServices/CustomAppServicesRegister.hpp>
#include <Shared/System/CustomAppServices/Global/GlobalExampleService.hpp>
#include <Shared/System/CustomAppServices/Local/LocalExampleService.hpp>

namespace Fsl
{
  using AsyncExampleServiceProxyFactory = AsynchronousServiceProxyFactoryTemplate<AsyncExampleServiceProxy, IAsyncExampleService>;
  using AsyncExampleServiceImplFactory = AsynchronousServiceImplFactoryTemplate<AsyncExampleServiceImpl>;
  using GlobalExampleServiceImplFactory = ThreadSafeSynchronousServiceTemplate<GlobalExampleService, IGlobalExampleService>;
  using LocalExampleServiceImplFactory = ThreadLocalSingletonServiceFactoryTemplate<LocalExampleService, ILocalExampleService>;

  void CustomAppServicesRegisterServices(ServiceRegistry& serviceRegistry)
  {
    const Priority globalServicePriority(0);
    const Priority asyncServicePriority(0);
    const Priority localServicePriority(0);

    // Register a async example service that runs on its own thread
    {
      // Creating a service group to ensures it runs on its own thread
      auto exampleAsyncServiceGroup = serviceRegistry.CreateServiceGroup();
      serviceRegistry.Register(
        AsynchronousServiceFactory(std::make_shared<AsyncExampleServiceProxyFactory>(), std::make_shared<AsyncExampleServiceImplFactory>()),
        asyncServicePriority, exampleAsyncServiceGroup);
    }

    // Register a global example service
    {
      serviceRegistry.Register<GlobalExampleServiceImplFactory>(globalServicePriority);
    }

    // Register a local example service
    {
      serviceRegistry.Register<LocalExampleServiceImplFactory>(localServicePriority);
    }
  }
}
