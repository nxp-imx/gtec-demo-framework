#ifndef OPENVG_VGSTRESSTEST_VGSTRESSTEST_HPP
#define OPENVG_VGSTRESSTEST_VGSTRESSTEST_HPP
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

#include <FslDemoAppVG/DemoAppVG.hpp>
#include <FslGraphicsVG/VGPathBuffer.hpp>
#include "ConfigSpiral.hpp"

namespace Fsl
{
  typedef std::shared_ptr<OpenVG::VGPathBuffer> VGPathBufferPtr;

  class VGStressTest : public DemoAppVG
  {
    ConfigSpiral m_config;
    VGPaint m_stroke;
    VGPaint m_stroke2;
    VGPaint m_fill;
    VGPaint m_imgPaint;
    VGImage m_imgPattern;
    std::vector<VGPathBufferPtr> m_paths;
    std::vector<VGPathBufferPtr> m_paths2;
    uint16_t m_angle;

    OpenVG::VGPathBuffer m_test;

    //bool mVGS_test_pass;
    //float mVGS_framerate_avg;
    //uint32_t mVGS_error_count;
    bool m_toggleMinMax;
  public:
    VGStressTest(const DemoAppConfig& config);
    ~VGStressTest();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
