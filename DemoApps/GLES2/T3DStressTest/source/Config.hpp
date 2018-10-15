#ifndef GLES2_T3DSTRESSTEST_CONFIG_HPP
#define GLES2_T3DSTRESSTEST_CONFIG_HPP
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

namespace Fsl
{
  class Config
  {
    // The number of layers to use for rendering the fur
    int m_layerCount;
    // The length of the hairs
    float m_hairLength;
    // The hair density
    float m_hairDensity;
    // The size of the fur texture (pow 2 value, if too large it will crash the app)
    int m_furTextureDimensions;
    // The number of instances that should be generated of the mesh
    int m_instanceCount;
    // The number of vertices that the quad consists of in X (>= 2, if too large it will crash the app)
    int m_vertexCountX;
    // The number of vertices that the quad consists of in y (>= 2, if too large it will crash the app)
    int m_vertexCountY;
    // Use triangle strips if true, triangle lists if false
    bool m_useTriangleStrip;
    // Set the distance of the camera
    float m_cameraDistance;
    // Render the normals
    bool m_showNormals;
    // Enable / disable zbuffer checks
    bool m_enableDepthTest;
    // Controls if instances shares the same vertices
    bool m_shareInstanceVertices;
    // The number of times the texture should be repeated
    int m_textureRepeatCount;
    // Enables/disables the use of high precision shaders
    bool m_useHighShaderPrecision;
    // The desired number of lights
    int m_lightCount;

    bool m_toggleMinMax;
    // If true each frame will execute a glFinish call
    bool m_forceFinishEachFrame;

  public:
    Config();
    int GetLayerCount() const;
    void SetLayerCount(const int value);
    float GetHairLength() const;
    void SetHairLength(const float value);
    float GetHairDensity() const;
    void SetHairDensity(const float value);
    int GetFurTextureDimensions() const;
    void SetFurTextureDimensions(const int value);
    int GetInstanceCount() const;
    void SetInstanceCount(const int value);
    int GetVertexCountX() const;
    void SetVertexCountX(const int value);
    int GetVertexCountY() const;
    void SetVertexCountY(const int value);

    bool GetUseTriangleStrip() const;
    void SetUseTriangleStrip(const bool value);

    float GetCameraDistance() const;
    void SetCameraDistance(const float value);

    bool GetShowNormals() const;
    void SetShowNormals(const bool value);

    bool GetEnableDepthTest() const;
    void SetEnableDepthTest(const bool value);

    bool GetShareInstanceVertices() const;
    void SetShareInstanceVertices(const bool value);

    int GetTextureRepeatCount() const;
    void SetTextureRepeatCount(const int value);

    bool GetUseHighShaderPrecision() const;
    void SetUseHighShaderPrecision(const bool bEnabled);

    int GetLightCount() const;
    void SetLightCount(const int value);

    bool GetToggleMinMax() const;
    void SetToggleMinMax(const bool bEnabled);

    bool GetForceFinishEachFrame() const;
    void SetForceFinishEachFrame(const bool value);
  };
}

#endif
