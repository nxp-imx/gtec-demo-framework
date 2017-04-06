#ifndef FRACTALSHADERSHARED_JULIAHELPER_HPP
#define FRACTALSHADERSHARED_JULIAHELPER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDemoApp/DemoTime.hpp>
#include <FslDemoApp/Service/ServiceProvider.hpp>
#include <FslDemoApp/Service/Graphics/Basic2D.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FractalShaderShared/Config.hpp>

namespace Fsl
{
  struct Point2;

  class JuliaHelper
  {
    Config m_config;
    std::shared_ptr<Basic2D> m_basic2D;
    std::shared_ptr<INativeBatch2D> m_nativeBatch2D;
    AtlasTexture2D m_atlasTexBanner;
    Vector2 m_angle1;
    Vector2 m_angle2;
    Vector2 m_angle3;
    Vector2 m_location;

    float m_angle;
  public:
    JuliaHelper(const Config& config, const ServiceProvider& serviceProvider);
    Vector2 GetLocation() const { return m_location; }

    void Update(const DemoTime& demoTime);
    void UpdateConstantLoad(const DemoTime& demoTime);
    void UpdateConstantLoad2(const DemoTime& demoTime);
    void UpdateConstantLoad3(const DemoTime& demoTime);
    void UpdateComplexMove(const DemoTime& demoTime);
    void UpdateSlowAndComplex(const DemoTime& demoTime);
    void UpdateDSin(const DemoTime& demoTime);
    void Draw(const Point2& screenResolution);

  };
}

#endif
