#ifndef FSLDEMOHOST_BASE_DEMOHOSTCONFIG_HPP
#define FSLDEMOHOST_BASE_DEMOHOSTCONFIG_HPP
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
#include <FslBase/Exceptions.hpp>
#include <FslDemoHost/Base/IDemoHost.hpp>
#include <FslDemoHost/Base/Setup/DemoHostAppSetup.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>

namespace Fsl
{
  class ADemoHostOptionParser;
  class INativeWindowEventQueue;

  class DemoHostConfig
  {
    std::shared_ptr<ADemoHostOptionParser> m_optionParser;
    std::weak_ptr<INativeWindowEventQueue> m_eventQueue;
    DemoHostAppSetup m_demoHostAppSetup;
    ServiceProvider m_serviceProvider;
    uint32_t m_verbosityLevel;

  public:
    DemoHostConfig(std::shared_ptr<ADemoHostOptionParser> optionParser, std::weak_ptr<INativeWindowEventQueue> eventQueue,
                   DemoHostAppSetup demoHostAppSetup, const std::weak_ptr<IServiceProvider>& serviceProvider, const uint32_t verbosityLevel);
    ~DemoHostConfig();


    template <typename T>
    std::shared_ptr<T> GetOptions() const
    {
      const std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(m_optionParser);
      if (!ptr)
      {
        throw UnknownTypeException(typeid(T).name());
      }
      return ptr;
    }

    std::weak_ptr<INativeWindowEventQueue> GetEventQueue() const;

    const DemoHostAppSetup& GetDemoHostAppSetup() const;
    ServiceProvider GetServiceProvider() const;
    uint32_t GetVerbosityLevel() const;
  };
}

#endif
