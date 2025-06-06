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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/FrameInfo.hpp>
#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslDemoService/Graphics/Control/GraphicsBeginFrameInfo.hpp>
#include <FslDemoService/Graphics/Control/GraphicsDependentCreateInfo.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceHost.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  namespace
  {
    const char* SafeToCString(const GLubyte* psz)
    {
      return psz == nullptr ? "nullptr" : reinterpret_cast<const char*>(psz);
    }
  }

  DemoAppGLES2::DemoAppGLES2(const DemoAppConfig& demoAppConfig)
    : ADemoApp(demoAppConfig)
    , m_graphicsServiceHost(demoAppConfig.DemoServiceProvider.Get<IGraphicsServiceHost>())
  {
    const GLubyte* vendor = glGetString(GL_VENDOR);                           // GPU vendor
    const GLubyte* renderer = glGetString(GL_RENDERER);                       // GPU model
    const GLubyte* version = glGetString(GL_VERSION);                         // OpenGL ES version
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);    // GLSL version

    FSLLOG3_INFO("Vendor: {}", SafeToCString(vendor));
    FSLLOG3_INFO("Renderer: {}", SafeToCString(renderer));
    FSLLOG3_INFO("Version: {}", SafeToCString(version));
    FSLLOG3_INFO("GLSL Version: {}", SafeToCString(glslVersion));

    {    // Adjust the default viewport to fix apps that forget to handle it
      const PxSize2D sizePx = demoAppConfig.WindowMetrics.GetSizePx();
      glViewport(0, 0, sizePx.RawWidth(), sizePx.RawHeight());
    }
  }

  DemoAppGLES2::~DemoAppGLES2()
  {
    // Many DemoApps forget this and it can get problems on restart, so doing it here to be nice (the app doesn't care as its being killed).
    glUseProgram(0);
  }

  void DemoAppGLES2::_PostConstruct()
  {
    ADemoApp::_PostConstruct();
    assert(m_graphicsServiceHost);

    GraphicsDependentCreateInfo createInfo(GetScreenExtent(), nullptr);
    m_graphicsServiceHost->CreateDependentResources(createInfo);
  }

  void DemoAppGLES2::_PreDestruct()
  {
    assert(m_graphicsServiceHost);
    m_graphicsServiceHost->DestroyDependentResources();

    ADemoApp::_PreDestruct();
  }

  void DemoAppGLES2::_BeginDraw(const FrameInfo& frameInfo)
  {
    ADemoApp::_BeginDraw(frameInfo);
    GraphicsBeginFrameInfo graphicsFrameInfo(frameInfo.FrameIndex);
    m_graphicsServiceHost->BeginFrame(graphicsFrameInfo);
  }

  void DemoAppGLES2::_EndDraw(const FrameInfo& frameInfo)
  {
    ADemoApp::_EndDraw(frameInfo);
    m_graphicsServiceHost->EndFrame();
  }

  void DemoAppGLES2::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    ADemoApp::ConfigurationChanged(windowMetrics);

    m_graphicsServiceHost->DestroyDependentResources();
    {    // Adjust the default viewport to fix apps that forget to handle it
      const PxSize2D sizePx = windowMetrics.GetSizePx();
      glViewport(0, 0, sizePx.RawWidth(), sizePx.RawHeight());
    }
    GraphicsDependentCreateInfo createInfo(windowMetrics.ExtentPx, nullptr);
    m_graphicsServiceHost->CreateDependentResources(createInfo);
  }
}
