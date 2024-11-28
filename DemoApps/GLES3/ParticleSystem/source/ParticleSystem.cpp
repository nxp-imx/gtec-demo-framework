/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include "ParticleSystem.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <GLES3/gl3.h>
#include <iostream>
#include "ParticleSystemBasicScene.hpp"
#include "ParticleSystemScene.hpp"

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;


  ParticleSystem::ParticleSystem(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
  {
    RegisterExtension(m_uiExtension);

    auto optionParser = config.GetOptions<OptionParser>();


    switch (optionParser->GetScene())
    {
    case DemoScene::Basic:
      m_scene = std::make_shared<ParticleSystemBasicScene>(config, m_uiExtension);
      break;
    case DemoScene::Default:
    default:
      m_scene = std::make_shared<ParticleSystemScene>(config, m_uiExtension);
      break;
    }
  }


  ParticleSystem::~ParticleSystem() = default;


  void ParticleSystem::OnSelect(const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (m_scene)
    {
      m_scene->OnSelect(theEvent);
    }
  }


  void ParticleSystem::OnContentChanged(const std::shared_ptr<WindowContentChangedEvent>& theEvent)
  {
    if (m_scene)
    {
      m_scene->OnContentChanged(theEvent);
    }
  }


  void ParticleSystem::OnKeyEvent(const KeyEvent& event)
  {
    base_type::OnKeyEvent(event);
    if (m_scene)
    {
      m_scene->OnKeyEvent(event);
    }
  }


  void ParticleSystem::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    base_type::OnMouseButtonEvent(event);
    if (m_scene)
    {
      m_scene->OnMouseButtonEvent(event);
    }
  }


  void ParticleSystem::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    base_type::OnMouseMoveEvent(event);
    if (m_scene)
    {
      m_scene->OnMouseMoveEvent(event);
    }
  }


  void ParticleSystem::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    base_type::OnMouseWheelEvent(event);
    if (m_scene)
    {
      m_scene->OnMouseWheelEvent(event);
    }
  }


  void ParticleSystem::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    base_type::ConfigurationChanged(windowMetrics);
    if (m_scene)
    {
      m_scene->ConfigurationChanged(windowMetrics);
    }
  }


  void ParticleSystem::Update(const DemoTime& demoTime)
  {
    base_type::Update(demoTime);
    if (m_scene)
    {
      m_scene->Update(demoTime);
    }
  }


  void ParticleSystem::Draw(const FrameInfo& frameInfo)
  {
    base_type::Draw(frameInfo);

    if (!m_scene)
    {
      glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
    {
      m_scene->Draw();
    }

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }
}
