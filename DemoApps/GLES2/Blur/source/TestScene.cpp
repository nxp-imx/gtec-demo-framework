/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include "TestScene.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include "GausianHelper.hpp"


namespace Fsl
{
  TestScene::TestScene(const DemoAppConfig& config)
  {
    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();
    const Point2 screenResolution = config.ScreenResolution;

    std::vector<double> kernel;
    // const int length = 3;
    // GausianHelper::CalculateGausianKernel(kernel, length, 1.0);
    const int length = 9;
    GausianHelper::CalculateGausianKernel(kernel, length, 1.83);
    // const int length = 41;
    // GausianHelper::CalculateGausianKernel(kernel, length, 9.83);
    // const int length = 21;
    // GausianHelper::CalculateGausianKernel(kernel, length, 9.5);

    // GausianHelper::DebugDumpKernel2D(kernel, length);

    {
      Bitmap bitmap;
      contentManager->Read(bitmap, "Test.jpg", PixelFormat::R8G8B8_UNORM);
      GLES2::GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      m_texture.SetData(bitmap, params);
    }

    const std::string fragTemplate = contentManager->ReadAllText("GaussianTemplate.frag");
    const std::string gaussianFrag = GausianHelper::GenerateGausianFragmentShader(fragTemplate, kernel, length, m_texture.GetSize());
    m_program.Reset(contentManager->ReadAllText("BasicShader.vert"), gaussianFrag);
    // m_program.Reset(contentManager->ReadAllText("BasicShader.vert"), contentManager->ReadAllText("BasicShader.frag"));

    // FSLLOG3_INFO(gaussianFrag);

    // const GLuint hProgram = m_program.Get();


    {    // prepare the vertex buffer
      // We scale the UV coordinates so that we get a 1-1 pixel mapping on the screen
      const float scaleX = static_cast<float>(screenResolution.X) / static_cast<float>(m_texture.GetSize().Width());
      const float aspect = (static_cast<float>(screenResolution.Y) / static_cast<float>(screenResolution.X));
      const float u1 = 0.0f;
      const float u2 = scaleX;
      const float v1 = scaleX * aspect;
      const float v2 = 0.0f * aspect;

      const std::array<VertexPositionTexture, 4> vertices = {
        VertexPositionTexture(Vector3(-1.0f, 1.0f, 0.0f), Vector2(u1, v2)),
        VertexPositionTexture(Vector3(-1.0f, -1.0f, 0.0f), Vector2(u1, v1)),
        VertexPositionTexture(Vector3(1.0f, 1.0f, 0.0f), Vector2(u2, v2)),
        VertexPositionTexture(Vector3(1.0f, -1.0f, 0.0f), Vector2(u2, v1)),
      };

      m_vertexBuffer.Reset(vertices, GL_STATIC_DRAW);
    }
    glViewport(0, 0, screenResolution.X, screenResolution.Y);
  }


  void TestScene::Update(const DemoTime& /*demoTime*/)
  {
  }


  void TestScene::Draw()
  {
    const GLuint hProgram = m_program.Get();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(hProgram);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get());
    m_vertexBuffer.EnableAttribArrays();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_vertexBuffer.DisableAttribArrays();

    glBindBuffer(m_vertexBuffer.GetTarget(), 0);
  }
}
