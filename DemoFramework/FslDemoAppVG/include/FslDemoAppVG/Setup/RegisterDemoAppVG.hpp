#ifndef FSLDEMOAPPVG_SETUP_REGISTERDEMOAPPVG_HPP
#define FSLDEMOAPPVG_SETUP_REGISTERDEMOAPPVG_HPP
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

#include <FslDemoApp/Host/DemoAppHostConfigEGL.hpp>
#include <FslDemoApp/Setup/RegisterDemoApp.hpp>


namespace Fsl
{
  class RegisterDemoAppVG
  {
  public:
    static void Register(const HostDemoAppSetup& setup, const DemoAppHostConfigEGL& demoHostEGLConfig, const std::shared_ptr<IDemoAppFactory>& appFactory);
    static void Register(const HostDemoAppSetup& setup, const DemoAppHostConfigEGL& demoHostEGLConfig, const std::shared_ptr<IDemoAppFactory>& appFactory, const std::shared_ptr<Fsl::ADemoOptionParser> appOptionParser);
  };
}


// Register a pure OpenGL ES 3 demo
#define FSL_REGISTER_OPENVG_DEMO(tdEMOaPP, dEMOhOSTeglcONFIG) \
namespace Fsl \
{ \
  void HOST_DemoAppSetup(const HostDemoAppSetup*const pSetup) \
  { \
    if(pSetup == nullptr) \
      return; \
    const std::shared_ptr<IDemoAppFactory> appFactory(new DemoHost_Internal::DemoAppFactoryTemplate<tdEMOaPP>()); \
    return RegisterDemoAppVG::Register(*pSetup, dEMOhOSTeglcONFIG, appFactory); \
  } \
}

#define FSL_REGISTER_OPENVG_DEMO_EX(tdEMOaPP, dEMOhOSTeglcONFIG, tdEMOaPPoPTIONpARSER) \
namespace Fsl \
{ \
  void HOST_DemoAppSetup(const HostDemoAppSetup*const pSetup) \
  { \
    if(pSetup == nullptr) \
      return; \
    const std::shared_ptr<IDemoAppFactory> appFactory(new DemoHost_Internal::DemoAppFactoryTemplate<tdEMOaPP>()); \
    const std::shared_ptr<Fsl::ADemoOptionParser> appOptionParser(new tdEMOaPPoPTIONpARSER()); \
    return RegisterDemoAppVG::Register(*pSetup, dEMOhOSTeglcONFIG, appFactory, appOptionParser); \
  } \
}

#endif
