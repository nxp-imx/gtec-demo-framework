#ifndef GLES3_FURSHELLRENDERING_FURSHELLRENDERING_HPP
#define GLES3_FURSHELLRENDERING_FURSHELLRENDERING_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <Shared/FurShellRendering/Config.hpp>
#include "MeshRenderBasic.hpp"
#include "MeshRenderNormals.hpp"
#include "MeshRenderVB.hpp"
#include "MeshRenderVBInstanced.hpp"
#include "Shader/FurShaderInstanced.hpp"
#include "Shader/FurShaderInstancedLayer0.hpp"
#include "Shader/FurShaderInstancedLayerN.hpp"
#include "Shader/FurShaderMultiPass.hpp"
#include "Shader/WhiteShader.hpp"

namespace Fsl
{
  class FurShellRendering final : public DemoAppGLES3
  {
    struct MeshStuffRecord
    {
      Procedural::BasicMesh Mesh;
      MeshRenderBasic Render;
      MeshRenderVB RenderVB;
      MeshRenderVBInstanced RenderInstanced;
      MeshRenderVBInstanced RenderInstancedLayerN;
      MeshRenderNormals RenderNormals;

      explicit MeshStuffRecord(const Procedural::BasicMesh& mesh)
        : Mesh(mesh)
        , Render(mesh)
        , RenderVB(mesh)
        , RenderInstanced(mesh)
        , RenderInstancedLayerN(mesh)
        , RenderNormals(mesh)
      {
      }
    };

    struct LightInfo
    {
      Vector3 Direction;
      Vector3 Color;
      Vector3 AmbientColor;
    };

    struct Resources
    {
      GLES3::GLProgram Program;
      std::unique_ptr<MeshStuffRecord> MeshStuff;
      GLES3::GLTexture Tex1;
      GLES3::GLTexture Tex2;
      GLES3::GLTexture TexDescriptionAtlas;
      AtlasTextureInfo TexDescription;
      GLES3::GLProgram BasicProgram;
      GLES3::GLVertexBuffer VBDescription;
    };

    Config m_config;
    Resources m_resources;
    FurShaderMultiPass m_shaderMultiPass;
    FurShaderInstanced m_shaderInstanced;
    FurShaderInstancedLayer0 m_shaderInstancedLayer0;
    FurShaderInstancedLayerN m_shaderInstancedLayerN;
    WhiteShader m_shader2;


    float m_perspectiveZ;
    float m_xAngle;
    float m_yAngle;
    float m_zAngle;
    float m_xSpeed;
    float m_ySpeed;
    float m_zSpeed;

    float m_cameraAngleX;
    float m_cameraPosY;

    Matrix m_rotate;
    Matrix m_view;
    Matrix m_perspective;
    Matrix m_mvp;
    Matrix m_world;

    Vector3 m_gravity;
    Vector3 m_displacement;
    Vector3 m_backgroundColor;
    float m_radians;
    bool m_enableDepthTest;
    bool m_enableForce;

  public:
    explicit FurShellRendering(const DemoAppConfig& config);
    ~FurShellRendering() override;

  protected:
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void Draw(const FrameInfo& frameInfo) final;

  private:
    static void DrawMultipass(FurShaderMultiPass& rShader, MeshRender& rRender, const Matrix& world, const Matrix& view, const Matrix& perspective,
                              const Vector3& displacement, const int layerCount);
    static void DrawInstanced(FurShaderBase& rShader, MeshRender& rRender, const Matrix& world, const Matrix& view, const Matrix& perspective,
                              const Vector3& displacement);
  };
}

#endif
