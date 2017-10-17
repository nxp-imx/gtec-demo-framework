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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include "DFSimpleUI101.hpp"
#include <GLES3/gl3.h>

namespace Fsl
{

  //! All the actual UI example code can be found in the Shared class since its reused for all DFSimpleUI101 samples.
  DFSimpleUI101::DFSimpleUI101(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_shared(config)
  {
  }


  DFSimpleUI101::~DFSimpleUI101()
  {

  }


  void DFSimpleUI101::OnKeyEvent(const KeyEvent& event)
  {
    m_shared.OnKeyEvent(event);
  }


  void DFSimpleUI101::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    m_shared.OnMouseButtonEvent(event);
  }


  void DFSimpleUI101::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    m_shared.OnMouseMoveEvent(event);
  }


  void DFSimpleUI101::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    m_shared.OnMouseWheelEvent(event);
  }


  void DFSimpleUI101::Resized(const Point2& size)
  {
    m_shared.Resized(size);
  }


  void DFSimpleUI101::FixedUpdate(const DemoTime& demoTime)
  {
    m_shared.FixedUpdate(demoTime);
  }


  void DFSimpleUI101::Update(const DemoTime& demoTime)
  {
    m_shared.Update(demoTime);
  }


  void DFSimpleUI101::Draw(const DemoTime& demoTime)
  {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_shared.Draw();
  }

}
