#ifndef GLES3_DFSIMPLEUI101_DFSIMPLEUI101_HPP
#define GLES3_DFSIMPLEUI101_DFSIMPLEUI101_HPP
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

#include <FslDemoAppGLES3/DemoAppGLES3.hpp>
#include <DFSimpleUI101Shared/Shared.hpp>

namespace Fsl
{
  class DFSimpleUI101 : public DemoAppGLES3
  {
    //! All the actual UI example code can be found in the Shared class since its reused for all DFSimpleUI101 samples.
    Shared m_shared;
  public:
    DFSimpleUI101(const DemoAppConfig& config);
    ~DFSimpleUI101();
  protected:
    virtual void OnKeyEvent(const KeyEvent& event) override;
    virtual void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    virtual void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    virtual void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    virtual void Resized(const Point2& size) override;
    virtual void FixedUpdate(const DemoTime& demoTime) override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
