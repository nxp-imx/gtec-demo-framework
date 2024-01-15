/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslDemoApp/OpenGLES3/Setup/RegisterDemoApp.hpp>
#include <Shared/FractalShader/BasicOptionParser.hpp>
#include <EGL/egl.h>
#include <array>
#include "MultipleViewportsFractalShader.hpp"

namespace Fsl
{
  namespace
  {
    // Custom EGL config (these will per default overwrite the custom settings. However a exact EGL config can be used)
    const std::array<EGLint, (7 * 2) + 1> g_eglConfigAttribs = {EGL_SAMPLES,      0,
                                                                EGL_RED_SIZE,     8,
                                                                EGL_GREEN_SIZE,   8,
                                                                EGL_BLUE_SIZE,    8,
                                                                EGL_ALPHA_SIZE,   0,    // chose the smallest possible
                                                                EGL_DEPTH_SIZE,   0,    // Prefer no depth buffer
                                                                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                                                EGL_NONE};
  }

  // Configure the demo environment to run this demo app in a OpenGLES3 host environment
  void ConfigureDemoAppEnvironment(HostDemoAppSetup& rSetup)
  {
    CustomDemoAppConfig customDemoAppConfig;
    customDemoAppConfig.RestartFlags = CustomDemoAppConfigRestartFlags::Never;

    DemoAppHostConfigEGL config(g_eglConfigAttribs.data());

    DemoAppRegister::GLES3::Register<MultipleViewportsFractalShader, BasicOptionParser>(rSetup, "GLES3.MultipleViewportsFractalShader", config,
                                                                                        customDemoAppConfig);
  }
}
