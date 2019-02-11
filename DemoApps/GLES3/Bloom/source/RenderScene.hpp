#ifndef GLES3_BLOOM_RENDERSCENE_HPP
#define GLES3_BLOOM_RENDERSCENE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLIndexBufferArray.hpp>
#include <FslUtil/OpenGLES3/GLVertexBufferArray.hpp>
#include <vector>
#include <Shared/Bloom/IScene.hpp>

namespace Fsl
{
  class RenderScene : public IScene
  {
    GLES3::GLProgram m_program;
    GLES3::GLTexture m_texture;
    GLES3::GLTexture m_textureSpecular;
    GLES3::GLTexture m_textureNormal;
    GLES3::GLIndexBufferArray m_indexBuffers;
    GLES3::GLVertexBufferArray m_vertexBuffers;
    GLint m_locWorld;
    GLint m_locWorldView;
    GLint m_locWorldViewProjection;
    GLint m_locNormalMatrix;
    GLint m_locTexture0;
    GLint m_locTextureSpecular;
    GLint m_locTextureNormal;
    GLint m_locLightDirection;
    GLint m_locLightColor;
    GLint m_locMatAmbient;
    GLint m_locMatSpecular;
    GLint m_locMatShininess;
    std::vector<GLES3::GLVertexAttribLink> m_attribLink;

    Vector3 m_lightDirection;
    Vector3 m_lightColor;
    Vector4 m_matAmbient;
    Vector4 m_matSpecular;
    float m_matShininess;

    Matrix m_matrixWorld;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Matrix m_matrixWorldView;
    Matrix m_matrixWorldViewProjection;
    Vector3 m_cameraSpaceLightDirection;
    Matrix3 m_matrixNormal;

  public:
    RenderScene(const DemoAppConfig& config, const int32_t sceneId);
    ~RenderScene() override;
    void Update(const DemoTime& demoTime, const Matrix& cameraViewMatrix, const Matrix& cameraRotation, const Vector3& rotation,
                const Point2& screenResolution) override;
    void Draw() override;

  private:
    void DrawMeshes();
    void PrepareShader(const std::shared_ptr<IContentManager>& contentManager, const bool useSpecMap, const bool useGlossMap,
                       const bool useNormalMap);
  };
}

#endif
