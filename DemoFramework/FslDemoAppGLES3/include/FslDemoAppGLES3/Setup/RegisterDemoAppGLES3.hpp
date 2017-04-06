#ifndef FSLDEMOAPPGLES3_SETUP_REGISTERDEMOAPPGLES3_HPP
#define FSLDEMOAPPGLES3_SETUP_REGISTERDEMOAPPGLES3_HPP
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

#ifndef FSL_ENABLE_GRAPHICS_ES3
#error Graphics.ES3 support has not been enabled (define FSL_ENABLE_GRAPHICS_ES3) to enable it
#endif

#include <FslDemoApp/Host/DemoAppHostConfigEGL.hpp>
#include <FslDemoApp/Setup/RegisterDemoApp.hpp>


namespace Fsl
{
  struct OpenGLESMinorVersion
  {
    uint16_t MinorVersion;
    uint16_t MinimumMinorVersion;

    OpenGLESMinorVersion(const uint16_t minorVersion)
      : MinorVersion(minorVersion)
      , MinimumMinorVersion(minorVersion)
    {
    }

    OpenGLESMinorVersion(const uint16_t preferredMinorVersion, const uint16_t minimumMinorVersion)
      : MinorVersion(preferredMinorVersion)
      , MinimumMinorVersion(minimumMinorVersion)
    {
    }
  };

  class RegisterDemoAppGLES3
  {
  public:
    static void Register(const HostDemoAppSetup& setup, const OpenGLESMinorVersion minorVersion, const DemoAppHostConfigEGL& demoHostEGLConfig, const std::shared_ptr<IDemoAppFactory>& appFactory);
    static void Register(const HostDemoAppSetup& setup, const OpenGLESMinorVersion minorVersion, const DemoAppHostConfigEGL& demoHostEGLConfig, const std::shared_ptr<IDemoAppFactory>& appFactory, const std::shared_ptr<Fsl::ADemoOptionParser> appOptionParser);
  };
}


// Register a pure OpenGL ES 3 demo
#define FSL_REGISTER_OPENGLES3_DEMO(tdEMOaPP, dEMOhOSTeglcONFIG) \
namespace Fsl \
{ \
  void HOST_DemoAppSetup(const HostDemoAppSetup*const pSetup) \
  { \
    if(pSetup == nullptr) \
      return; \
    const std::shared_ptr<IDemoAppFactory> appFactory(new DemoHost_Internal::DemoAppFactoryTemplate<tdEMOaPP>()); \
    return RegisterDemoAppGLES3::Register(*pSetup, 0, dEMOhOSTeglcONFIG, appFactory); \
  } \
}

#define FSL_REGISTER_OPENGLES3_DEMO_EX(tdEMOaPP, dEMOhOSTeglcONFIG, tdEMOaPPoPTIONpARSER) \
namespace Fsl \
{ \
  void HOST_DemoAppSetup(const HostDemoAppSetup*const pSetup) \
  { \
    if(pSetup == nullptr) \
      return; \
    const std::shared_ptr<IDemoAppFactory> appFactory(new DemoHost_Internal::DemoAppFactoryTemplate<tdEMOaPP>()); \
    const std::shared_ptr<Fsl::ADemoOptionParser> appOptionParser(new tdEMOaPPoPTIONpARSER()); \
    return RegisterDemoAppGLES3::Register(*pSetup, 0, dEMOhOSTeglcONFIG, appFactory, appOptionParser); \
  } \
}

#define FSL_REGISTER_OPENGLES3_X_DEMO(tdEMOaPP, mINORvERSION, dEMOhOSTeglcONFIG) \
namespace Fsl \
{ \
  void HOST_DemoAppSetup(const HostDemoAppSetup*const pSetup) \
  { \
    if(pSetup == nullptr) \
      return; \
    const std::shared_ptr<IDemoAppFactory> appFactory(new DemoHost_Internal::DemoAppFactoryTemplate<tdEMOaPP>()); \
    return RegisterDemoAppGLES3::Register(*pSetup, mINORvERSION, dEMOhOSTeglcONFIG, appFactory); \
  } \
}

#define FSL_REGISTER_OPENGLES3_X_DEMO_EX(tdEMOaPP, mINORvERSION, dEMOhOSTeglcONFIG, tdEMOaPPoPTIONpARSER) \
namespace Fsl \
{ \
  void HOST_DemoAppSetup(const HostDemoAppSetup*const pSetup) \
  { \
    if(pSetup == nullptr) \
      return; \
    const std::shared_ptr<IDemoAppFactory> appFactory(new DemoHost_Internal::DemoAppFactoryTemplate<tdEMOaPP>()); \
    const std::shared_ptr<Fsl::ADemoOptionParser> appOptionParser(new tdEMOaPPoPTIONpARSER()); \
    return RegisterDemoAppGLES3::Register(*pSetup, mINORvERSION, dEMOhOSTeglcONFIG, appFactory, appOptionParser); \
  } \
}

#endif
