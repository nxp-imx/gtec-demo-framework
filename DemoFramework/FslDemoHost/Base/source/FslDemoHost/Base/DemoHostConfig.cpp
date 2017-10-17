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

namespace Fsl
{

  DemoHostConfig::DemoHostConfig(const std::shared_ptr<ADemoHostOptionParser>& optionParser, const std::weak_ptr<INativeWindowEventQueue>& eventQueue,
                                 const DemoHostAppSetup& demoHostAppSetup, const std::weak_ptr<IServiceProvider>& serviceProvider)
    : m_optionParser(optionParser)
    , m_eventQueue(eventQueue)
    , m_demoHostAppSetup(demoHostAppSetup)
    , m_serviceProvider(serviceProvider)
  {
  }


  DemoHostConfig::~DemoHostConfig()
  {
  }


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

}
