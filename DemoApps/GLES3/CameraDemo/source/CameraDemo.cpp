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

#include "CameraDemo.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/Camera/Platform/PlatformCameraSystem.hpp>
#include <GLES3/gl3.h>
#include <limits>
#include "CameraRender.hpp"

// Includes for glm and matrix manipulation
#include <glm/glm.hpp>
// translate, rotate, scale, perspective
#include <glm/gtc/matrix_transform.hpp>
// value_ptr
#include <glm/gtc/type_ptr.hpp>

#include <memory>

namespace Fsl
{
  using namespace GLES3;


  CameraDemo::CameraDemo(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_basic2D(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasic2D())
  {
    try
    {
      m_cameraRender = std::make_shared<CameraRender>(GetContentManager(), GetScreenResolution());
    }
    catch (const std::exception& ex)
    {
      FSLLOG_WARNING("Failed to create camera render: " << ex.what());
    }
  }


  CameraDemo::~CameraDemo() = default;


  void CameraDemo::Update(const DemoTime& demoTime)
  {
  }


  void CameraDemo::Draw(const DemoTime& demoTime)
  {
    auto currentSize = GetScreenResolution();

    glViewport(0, 0, currentSize.X, currentSize.Y);

    // Clear the color-buffer and depth-buffer
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_cameraRender)
    {
      m_cameraRender->Draw(demoTime);
    }
    else if (m_basic2D)
    {
      m_basic2D->Begin();
      m_basic2D->DrawString("Camera not available", Vector2());
      m_basic2D->End();
    }
  }
}
