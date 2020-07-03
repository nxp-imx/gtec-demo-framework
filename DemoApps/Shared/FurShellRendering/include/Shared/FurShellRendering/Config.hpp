#ifndef SHARED_FURSHELLRENDERING_CONFIG_HPP
#define SHARED_FURSHELLRENDERING_CONFIG_HPP
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

#include <FslBase/Math/Point2.hpp>


namespace Fsl
{
  class Config
  {
    int32_t m_renderMode;
    // The number of layers to use for rendering the fur
    int m_layerCount;
    // The length of the hairs
    float m_hairLength;
    // The hair density
    float m_hairDensity;
    // The size of the fur texture (pow 2 value, if too large it will crash the app)
    Point2 m_furTextureDimensions;

    int m_torusMajorSegments;
    int m_torusMinorSegments;

    // Use triangle strips if true, triangle lists if false
    bool m_useTriangleStrip;
    // Render the normals
    bool m_showNormals;
    // The number of times the texture should be repeated
    int m_textureRepeatCountX;
    // The number of times the texture should be repeated
    int m_textureRepeatCountY;
    // Enables/disables the use of high precision shaders
    bool m_useHighShaderPrecision;
    // The desired number of lights
    int m_lightCount;
    // If true each frame will execute a glFinish call
    bool m_forceFinishEachFrame;

    uint32_t m_backgroundColor;
    //
    int m_demoId;

  public:
    explicit Config(const int32_t defaultRenderMode);
    int32_t GetRenderMode() const;
    void SetRenderMode(const int32_t value);
    int GetLayerCount() const;
    void SetLayerCount(const int value);
    float GetHairLength() const;
    void SetHairLength(const float value);
    float GetHairDensity() const;
    void SetHairDensity(const float value);
    Point2 GetFurTextureDimensions() const;
    void SetFurTextureDimensions(const Point2 value);
    void SetFurTextureDimensions(const int32_t value);
    int GetTorusMajorSegments() const;
    void SetTorusMajorSegments(const int value);
    int GetTorusMinorSegments() const;
    void SetTorusMinorSegments(const int value);

    bool GetUseTriangleStrip() const;
    void SetUseTriangleStrip(const bool value);

    bool GetShowNormals() const;
    void SetShowNormals(const bool value);

    int GetTextureRepeatCountX() const;
    void SetTextureRepeatCountX(const int value);

    int GetTextureRepeatCountY() const;
    void SetTextureRepeatCountY(const int value);

    bool GetUseHighShaderPrecision() const;
    void SetUseHighShaderPrecision(const bool bEnabled);

    int GetLightCount() const;
    void SetLightCount(const int value);

    bool GetForceFinishEachFrame() const;
    void SetForceFinishEachFrame(const bool value);

    uint32_t GetBackgroundColor() const;
    void SetBackgroundColor(const uint32_t value);

    int GetDemoId() const;
    void SetDemoId(const int value);
  };
}

#endif
