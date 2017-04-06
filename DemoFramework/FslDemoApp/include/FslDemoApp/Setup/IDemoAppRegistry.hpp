#ifndef FSLDEMOAPP_SETUP_IDEMOAPPREGISTRY_HPP
#define FSLDEMOAPP_SETUP_IDEMOAPPREGISTRY_HPP
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

#include <FslDemoApp/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Host/DemoHostFeature.hpp>
#include <deque>

namespace Fsl
{
  class DemoAppHostConfigEGL;
  class DemoAppHostConfigVulkan;
  class DemoAppHostConfigWindow;

  // Can be used to
  // 1. supply the various configuration data
  // 2. runtime check for availability before choosing host capabilities (for runtime switching between es2 and es3 for example)
  class IDemoAppRegistry
  {
  public:
    virtual ~IDemoAppRegistry() {}

    //! @brief Register a app
    virtual void Register(const DemoAppSetup& demoAppSetup, const DemoHostFeature& demoHostFeature) = 0;
    virtual void Register(const DemoAppSetup& demoAppSetup, const std::deque<DemoHostFeature>& demoHostFeatures) = 0;
    //! @brief Register a EGL based app
    virtual void Register(const DemoAppSetup& demoAppSetup, const DemoHostFeature& demoHostFeature, const DemoAppHostConfigEGL& demoHostConfig) = 0;
    virtual void Register(const DemoAppSetup& demoAppSetup, const std::deque<DemoHostFeature>& demoHostFeatures, const DemoAppHostConfigEGL& demoHostConfig) = 0;
    //! @brief Register a Vulkan based app
    virtual void Register(const DemoAppSetup& demoAppSetup, const DemoHostFeature& demoHostFeature, const DemoAppHostConfigVulkan& demoHostConfig) = 0;
    virtual void Register(const DemoAppSetup& demoAppSetup, const std::deque<DemoHostFeature>& demoHostFeatures, const DemoAppHostConfigVulkan& demoHostConfig) = 0;
    //! @brief Register a Window based app
    virtual void Register(const DemoAppSetup& demoAppSetup, const DemoHostFeature& demoHostFeature, const DemoAppHostConfigWindow& demoHostConfig) = 0;
    virtual void Register(const DemoAppSetup& demoAppSetup, const std::deque<DemoHostFeature>& demoHostFeatures, const DemoAppHostConfigWindow& demoHostConfig) = 0;
  };
}

#endif
