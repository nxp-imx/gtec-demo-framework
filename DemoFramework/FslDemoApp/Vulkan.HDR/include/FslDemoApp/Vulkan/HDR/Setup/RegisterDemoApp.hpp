#ifndef FSLDEMOAPP_VULKAN_HDR_SETUP_REGISTERDEMOAPP_HPP
#define FSLDEMOAPP_VULKAN_HDR_SETUP_REGISTERDEMOAPP_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#ifndef FSL_FEATURE_VULKAN1_0
#error Graphics.Vulkan support has not been enabled (define FSL_FEATURE_VULKAN1_0) to enable it
#endif

#include <FslBase/Log/Log3Core.hpp>
#include <FslDemoApp/Vulkan/HDR/Setup/Configure.hpp>
#include <FslDemoApp/Vulkan/Setup/RegisterDemoApp.hpp>

namespace Fsl::DemoAppRegister::Vulkan::HDR
{
  //! Register a demo app without a option parser
  template <typename TAppClass>
  void Register(HostDemoAppSetup& rSetup, const std::string& applicationName, const DemoAppHostConfigVulkan& config,
                const CustomDemoAppConfig& customDemoAppConfig = CustomDemoAppConfig())
  {
    FSLLOG3_VERBOSE2("Setting color space to linear");
    CustomDemoAppConfig patchedCustomDemoAppConfig = customDemoAppConfig;
    patchedCustomDemoAppConfig.AppColorSpaceType = ColorSpaceType::Linear;
    patchedCustomDemoAppConfig.HDREnabled = true;

    // Do the normal Vulkan register operations
    Fsl::DemoAppRegister::Vulkan::Register<TAppClass>(rSetup, applicationName, config, patchedCustomDemoAppConfig);

    FSLLOG3_VERBOSE2("Registering HDR services");
    Fsl::DemoAppRegister::Vulkan::HDR::Internal::RegisterServices(rSetup);
  }

  //! Register a demo app with a option parser
  template <typename TAppClass, typename TOptionParser>
  void Register(HostDemoAppSetup& rSetup, const std::string& applicationName, const DemoAppHostConfigVulkan& config,
                const CustomDemoAppConfig& customDemoAppConfig = CustomDemoAppConfig())
  {
    FSLLOG3_VERBOSE2("Setting color space to linear");
    CustomDemoAppConfig patchedCustomDemoAppConfig = customDemoAppConfig;
    patchedCustomDemoAppConfig.AppColorSpaceType = ColorSpaceType::Linear;
    patchedCustomDemoAppConfig.HDREnabled = true;

    // Do the normal Vulkan register operations
    Fsl::DemoAppRegister::Vulkan::Register<TAppClass, TOptionParser>(rSetup, applicationName, config, patchedCustomDemoAppConfig);

    FSLLOG3_VERBOSE2("Registering HDR services");
    Fsl::DemoAppRegister::Vulkan::HDR::Internal::RegisterServices(rSetup);
  }
}

#endif
