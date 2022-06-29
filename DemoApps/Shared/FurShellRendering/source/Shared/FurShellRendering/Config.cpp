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

#include <FslBase/Math/MathHelper.hpp>
#include <Shared/FurShellRendering/Config.hpp>
#include <algorithm>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const int MaxLights = 10;
      constexpr const int MinLight = 1;
    }
  }

  Config::Config(const int32_t defaultRenderMode)
    : m_renderMode(defaultRenderMode)
    , m_layerCount(10)
    , m_hairLength(2.0f)
    , m_hairDensity(0.8f)
    , m_furTextureDimensions(512, 512)    // WARNING: this must be a pow2 number (1,2,4,8....)
    , m_torusMajorSegments(32)
    , m_torusMinorSegments(32)
    , m_useTriangleStrip(true)
    , m_showNormals(false)
    , m_textureRepeatCountX(1)
    , m_textureRepeatCountY(1)
    , m_useHighShaderPrecision(false)
    , m_lightCount(LocalConfig::MinLight)
    , m_forceFinishEachFrame(false)
    , m_backgroundColor(0xFF808080)
    , m_demoId(0)
  {
  }


  int32_t Config::GetRenderMode() const
  {
    return m_renderMode;
  }


  void Config::SetRenderMode(const int32_t value)
  {
    m_renderMode = value;
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


  Point2 Config::GetFurTextureDimensions() const
  {
    return m_furTextureDimensions;
  }


  void Config::SetFurTextureDimensions(const int32_t value)
  {
    SetFurTextureDimensions(Point2(value, value));
  }


  void Config::SetFurTextureDimensions(const Point2 value)
  {
    m_furTextureDimensions.X = MathHelper::ToPowerOfTwo(std::max(std::min(value.X, 4096), 0));
    m_furTextureDimensions.Y = MathHelper::ToPowerOfTwo(std::max(std::min(value.Y, 4096), 0));
  }


  int Config::GetTorusMajorSegments() const
  {
    return m_torusMajorSegments;
  }


  void Config::SetTorusMajorSegments(const int value)
  {
    m_torusMajorSegments = std::max(value, 3);
  }


  int Config::GetTorusMinorSegments() const
  {
    return m_torusMinorSegments;
  }


  void Config::SetTorusMinorSegments(const int value)
  {
    m_torusMinorSegments = std::max(value, 3);
  }


  bool Config::GetUseTriangleStrip() const
  {
    return m_useTriangleStrip;
  }


  void Config::SetUseTriangleStrip(const bool value)
  {
    m_useTriangleStrip = value;
  }


  bool Config::GetShowNormals() const
  {
    return m_showNormals;
  }


  void Config::SetShowNormals(const bool value)
  {
    m_showNormals = value;
  }


  int Config::GetTextureRepeatCountX() const
  {
    return m_textureRepeatCountX;
  }


  void Config::SetTextureRepeatCountX(const int value)
  {
    m_textureRepeatCountX = std::max(value, 1);
  }


  int Config::GetTextureRepeatCountY() const
  {
    return m_textureRepeatCountY;
  }


  void Config::SetTextureRepeatCountY(const int value)
  {
    m_textureRepeatCountY = std::max(value, 1);
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
    m_lightCount = std::min(std::max(value, LocalConfig::MinLight), LocalConfig::MaxLights);
  }


  bool Config::GetForceFinishEachFrame() const
  {
    return m_forceFinishEachFrame;
  }


  void Config::SetForceFinishEachFrame(const bool value)
  {
    m_forceFinishEachFrame = value;
  }


  uint32_t Config::GetBackgroundColor() const
  {
    return m_backgroundColor;
  }


  void Config::SetBackgroundColor(const uint32_t value)
  {
    m_backgroundColor = value;
  }

  int Config::GetDemoId() const
  {
    return m_demoId;
  }


  void Config::SetDemoId(const int value)
  {
    m_demoId = value;
  }
}
