#ifndef FSLDEMOAPP_OPENVG_SETUP_REGISTERDEMOAPP_HPP
#define FSLDEMOAPP_OPENVG_SETUP_REGISTERDEMOAPP_HPP
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

#ifndef FSL_ENABLE_GRAPHICS_OPENVG
#error Graphics.OpenVG support has not been enabled (define FSL_ENABLE_GRAPHICS_OPENVG) to enable it
#endif

#include <FslDemoApp/Base/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/RegisterDemoApp.hpp>
#include <FslDemoHost/EGL/Config/DemoAppHostConfigEGL.hpp>


namespace Fsl
{
  namespace DemoAppRegister
  {
    namespace OpenVG
    {
      extern void Register(HostDemoAppSetup& rSetup, const DemoAppSetup& demoAppSetup, const DemoAppHostConfigEGL& demoHostEGLConfig);

      //! Register a demo app without a option parser
      template<typename TAppClass, typename TDemoAppHostConfig>
      static void Register(HostDemoAppSetup& rSetup, const std::string& applicationName, const TDemoAppHostConfig& config,
                           const CustomDemoAppConfig& customDemoAppConfig = CustomDemoAppConfig())
      {
        auto appFactory = std::make_shared<DemoHost_Internal::DemoAppFactoryTemplate<TAppClass> >();
        const DemoAppSetup demoAppSetup(applicationName, customDemoAppConfig, appFactory);
        Register(rSetup, demoAppSetup, config);
      }

      //! Register a demo app with a option parser
      template<typename TAppClass, typename TOptionParser, typename TDemoAppHostConfig>
      static void Register(HostDemoAppSetup& rSetup, const std::string& applicationName, const TDemoAppHostConfig& config,
                           const CustomDemoAppConfig& customDemoAppConfig = CustomDemoAppConfig())
      {
        auto appFactory = std::make_shared<DemoHost_Internal::DemoAppFactoryTemplate<TAppClass> >();
        auto appOptionParser = std::make_shared<TOptionParser>();
        const DemoAppSetup demoAppSetup(applicationName, customDemoAppConfig, appFactory, appOptionParser);
        Register(rSetup, demoAppSetup, config);
      }
    }
  }
}


// Register a pure OpenVG demo
#define FSL_REGISTER_OPENVG_DEMO(tdEMOaPP, dEMOhOSTeglcONFIG)                          static_assert(false, "The FSL_REGISTER_OPENVG_DEMO macro is not supported anymore, please create a ConfigureDemoAppEnvironment function");
#define FSL_REGISTER_OPENVG_DEMO_EX(tdEMOaPP, dEMOhOSTeglcONFIG, tdEMOaPPoPTIONpARSER) static_assert(false, "The FSL_REGISTER_OPENVG_DEMO_EX macro is not supported anymore, please create a ConfigureDemoAppEnvironment function");

#endif
