#ifndef GLES3_T3DSTRESSTEST_T3DSTRESSTEST_HPP
#define GLES3_T3DSTRESSTEST_T3DSTRESSTEST_HPP
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
#include <FslGraphics3D/Procedural/BasicMesh.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <Shared/T3DStressTest/Config.hpp>
#include "MeshRenderBasic.hpp"
#include "MeshRenderNormals.hpp"
#include "MeshRenderVB.hpp"
#include "Shader/FurShaderMultiPass.hpp"
#include "Shader/FurShaderInstanced.hpp"
#include "Shader/WhiteShader.hpp"

namespace Fsl
{
  class T3DStressTest : public DemoAppGLES3
  {
    struct MeshStuffRecord
    {
      Procedural::BasicMesh Mesh;
      MeshRenderBasic MeshRender1;
      MeshRenderVB MeshRender2;
      MeshRenderNormals RenderNormals;
      MeshRender& Render;

      explicit MeshStuffRecord(const Procedural::BasicMesh& mesh)
        : Mesh(mesh)
        , MeshRender1(mesh)
        , MeshRender2(mesh)
        , RenderNormals(mesh)
        , Render(MeshRender2)
      {
      }
    };

    struct Resources
    {
      GLES3::GLProgram Program;
      std::unique_ptr<MeshStuffRecord> MeshStuff;
      GLES3::GLTexture Tex1;
      GLES3::GLTexture Tex2;
    };

    Config m_config;
    Resources m_resources;
    FurShaderMultiPass m_shaderMultiPass;
    FurShaderInstanced m_shaderInstanced;
    WhiteShader m_shaderWhite;

    int m_xAngle;
    int m_yAngle;
    int m_zAngle;

    Matrix m_rotate;
    Matrix m_view;
    Matrix m_perspective;
    Matrix m_MVP;
    Matrix m_world;

    Vector3 m_gravity;
    Vector3 m_displacement;
    float m_radians;

  public:
    explicit T3DStressTest(const DemoAppConfig& config);
    ~T3DStressTest() override;

  protected:
    void FixedUpdate(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
  };
}

#endif
