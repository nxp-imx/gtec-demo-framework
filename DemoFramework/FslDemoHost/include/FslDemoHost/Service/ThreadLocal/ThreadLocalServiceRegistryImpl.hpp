#ifndef FSLDEMOHOST_SERVICE_THREADLOCAL_THREADLOCALSERVICEREGISTRYIMPL_HPP
#define FSLDEMOHOST_SERVICE_THREADLOCAL_THREADLOCALSERVICEREGISTRYIMPL_HPP
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

#include <FslBase/Noncopyable.hpp>
#include <FslBase/RTTI/TypeInfo.hpp>
#include <FslDemoHost/Service/Priority.hpp>
#include <FslDemoHost/Service/ServiceOptionParserDeque.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalServiceDeque.hpp>
#include <deque>
#include <map>
#include <memory>

namespace Fsl
{
  class IThreadLocalSingletonServiceFactory;
  class Priority;
  class ServiceOptionParserDeque;
  class ThreadLocalSingletonServiceMap;

  //! @brief A simple registry for registering thread local service singleton factories
  class ThreadLocalServiceRegistryImpl : Noncopyable
  {
    bool m_isLocked;
    ThreadLocalServiceDeque m_services;
    ServiceOptionParserDeque m_serviceOptionParsers;
  public:
    ThreadLocalServiceRegistryImpl();
    ~ThreadLocalServiceRegistryImpl();

    void LockAndExtractServices(ThreadLocalServiceDeque& rServices, ServiceOptionParserDeque& rServiceOptionParsers);

    //! @brief Register a service with the given priority
    void Register(const std::shared_ptr<IThreadLocalSingletonServiceFactory>& factory, const Priority& priority);
  };
}

#endif
