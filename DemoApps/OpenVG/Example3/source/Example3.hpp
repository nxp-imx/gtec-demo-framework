#ifndef OPENVG_EXAMPLE3_EXAMPLE3_HPP
#define OPENVG_EXAMPLE3_EXAMPLE3_HPP
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

#include <FslDemoApp/OpenVG/DemoAppVG.hpp>
#include <FslUtil/OpenVG/VGPathBuffer.hpp>
#include "VG/openvg.h"
#include "VG/vgu.h"

namespace Fsl
{
  class Example3 : public DemoAppVG
  {
    OpenVG::VGPathBuffer m_vgTrianglePath;
    OpenVG::VGPathBuffer m_vgQuadPath;
    OpenVG::VGPathBuffer m_vgCubicPath;
    OpenVG::VGPathBuffer m_vgArcPath;
    VGPaint m_vgTrianglePaint;
    VGPaint m_vgQuadPaint;
    VGPaint m_vgCubicPaint;
    VGPaint m_vgArcPaint;
    VGPaint m_vgStrokePaint;

    VGfloat m_angle;
    VGfloat m_scale;
    VGfloat m_shear;
    VGfloat m_translate;
    uint8_t m_scaleDirection;
    uint8_t m_shearDirection;
    uint8_t m_translateDirection;
    uint8_t m_scissorsDirection;
    VGfloat m_scissorRectangle;

  public:
    explicit Example3(const DemoAppConfig& config);
    ~Example3() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;
  };
}

#endif
