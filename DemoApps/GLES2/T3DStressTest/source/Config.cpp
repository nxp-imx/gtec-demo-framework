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

#include "Config.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <algorithm>


#define MAX_LIGHTS 10
#define MIN_LIGHTS 1

namespace Fsl
{

  Config::Config()
    : m_layerCount(150)
    , m_hairLength(4.0f)
    , m_hairDensity(0.8f)
    , m_furTextureDimensions(512)  // WARNING: this must be a pow2 number (1,2,4,8....)
    , m_instanceCount(2)
    , m_vertexCountX(20)
    , m_vertexCountY(20)
    , m_useTriangleStrip(true)
    , m_cameraDistance(10.0f)
    , m_showNormals(false)
    , m_enableDepthTest(false)
    , m_shareInstanceVertices(false)
    , m_textureRepeatCount(1)
    , m_useHighShaderPrecision(false)
    , m_lightCount(MIN_LIGHTS)
    , m_toggleMinMax(false)
    , m_forceFinishEachFrame(true)
  {
  }


  int Config::GetLayerCount() const
  {
    return m_layerCount;
  }


  void Config::SetLayerCount(const int value)
  {
    m_layerCount = std::max(1, value);
  }


  float Config::GetHairLength() const
  {
    return m_hairLength;
  }


  void Config::SetHairLength(const float value)
  {
    m_hairLength = value;
  }


  float Config::GetHairDensity() const
  {
    return m_hairDensity;
  }


  void Config::SetHairDensity(const float value)
  {
    m_hairDensity = std::max(std::min(value, 1.0f), 0.0f);
  }


  int Config::GetFurTextureDimensions() const
  {
    return m_furTextureDimensions;
  }


  void Config::SetFurTextureDimensions(const int value)
  {
    m_furTextureDimensions = MathHelper::ToPowerOfTwo(std::max(std::min(value, 4096), 0));
  }


  int Config::GetInstanceCount() const
  {
    return m_instanceCount;
  }


  void Config::SetInstanceCount(const int value)
  {
    m_instanceCount = std::max(value, 1);
  }


  int Config::GetVertexCountX() const
  {
    return m_vertexCountX;
  }


  void Config::SetVertexCountX(const int value)
  {
    m_vertexCountX = std::max(value, 2);
  }


  int Config::GetVertexCountY() const
  {
    return m_vertexCountY;
  }


  void Config::SetVertexCountY(const int value)
  {
    m_vertexCountY = std::max(value, 2);
  }


  bool Config::GetUseTriangleStrip() const
  {
    return m_useTriangleStrip;
  }


  void Config::SetUseTriangleStrip(const bool value)
  {
    m_useTriangleStrip = value;
  }


  float Config::GetCameraDistance() const
  {
    return m_cameraDistance;
  }


  void Config::SetCameraDistance(const float value)
  {
    m_cameraDistance = value;
  }


  bool Config::GetShowNormals() const
  {
    return m_showNormals;
  }


  void Config::SetShowNormals(const bool value)
  {
    m_showNormals = value;
  }


  bool Config::GetEnableDepthTest() const
  {
    return m_enableDepthTest;
  }


  void Config::SetEnableDepthTest(const bool value)
  {
    m_enableDepthTest = value;
  }


  bool Config::GetShareInstanceVertices() const
  {
    return m_shareInstanceVertices;
  }


  void Config::SetShareInstanceVertices(const bool value)
  {
    m_shareInstanceVertices = value;
  }


  int Config::GetTextureRepeatCount() const
  {
    return m_textureRepeatCount;
  }


  void Config::SetTextureRepeatCount(const int value)
  {
    m_textureRepeatCount = std::max(value, 1);
  }


  bool Config::GetUseHighShaderPrecision() const
  {
    return m_useHighShaderPrecision;
  }


  void Config::SetUseHighShaderPrecision(const bool bEnabled)
  {
    m_useHighShaderPrecision = bEnabled;
  }


  int Config::GetLightCount() const
  {
    return m_lightCount;
  }


  void Config::SetLightCount(const int value)
  {
    m_lightCount = std::min(std::max(value, MIN_LIGHTS), MAX_LIGHTS);
  }


  bool Config::GetToggleMinMax() const
  {
    return m_toggleMinMax;
  }


  void Config::SetToggleMinMax(const bool bEnabled)
  {
    m_toggleMinMax = bEnabled;
  }


  bool Config::GetForceFinishEachFrame() const
  {
    return m_forceFinishEachFrame;
  }


  void Config::SetForceFinishEachFrame(const bool value)
  {
    m_forceFinishEachFrame = value;
  }

}
