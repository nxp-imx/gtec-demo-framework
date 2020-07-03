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

#include <FslDemoHost/Base/DemoHostConfig.hpp>
#include <utility>

namespace Fsl
{
  DemoHostConfig::DemoHostConfig(std::shared_ptr<ADemoHostOptionParser> optionParser, std::weak_ptr<INativeWindowEventQueue> eventQueue,
                                 DemoHostAppSetup demoHostAppSetup, const std::weak_ptr<IServiceProvider>& serviceProvider,
                                 const uint32_t verbosityLevel)
    : m_optionParser(std::move(optionParser))
    , m_eventQueue(std::move(eventQueue))
    , m_demoHostAppSetup(std::move(demoHostAppSetup))
    , m_serviceProvider(serviceProvider)
    , m_verbosityLevel(verbosityLevel)
  {
  }


  DemoHostConfig::~DemoHostConfig() = default;


  std::weak_ptr<INativeWindowEventQueue> DemoHostConfig::GetEventQueue() const
  {
    return m_eventQueue;
  }


  const DemoHostAppSetup& DemoHostConfig::GetDemoHostAppSetup() const
  {
    return m_demoHostAppSetup;
  }


  ServiceProvider DemoHostConfig::GetServiceProvider() const
  {
    return m_serviceProvider;
  }


  uint32_t DemoHostConfig::GetVerbosityLevel() const
  {
    return m_verbosityLevel;
  }
}
