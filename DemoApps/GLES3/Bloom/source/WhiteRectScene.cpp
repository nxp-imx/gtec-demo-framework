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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include "WhiteRectScene.hpp"
#include <GLES3/gl3.h>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include "VBHelper.hpp"

namespace Fsl
{
  using namespace GLES3;
  using namespace Graphics3D;


  using BasicMesh = GenericMesh<VertexPositionNormalTexture, uint16_t>;
  using BasicScene = GenericScene<BasicMesh>;

  namespace
  {
    const auto MODELS_PATH = "Models";
  }


  WhiteRectScene::WhiteRectScene(const DemoAppConfig& config)

  {
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();
    m_program.Reset(contentManager->ReadAllText("Shaders/Pass.vert"), contentManager->ReadAllText("Shaders/CopyPass.frag"));


    {    // Prepare a white texture
      GLTextureParameters params(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      Bitmap b(1, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      b.SetNativePixel(0, 0, 0xffffffff);
      m_texWhite.Reset(b, params);
    }

    // build the rect VB
    VBHelper::BuildVB(m_vbRect, BoxF(-1 / 10.0f, -1 / 10.0f, 1 / 10.0f, 1 / 10.0f), BoxF(0.0f, 0.0f, 1.0f, 1.0f));
  }


  WhiteRectScene::~WhiteRectScene() = default;

  void WhiteRectScene::Update(const DemoTime& demoTime, const Matrix& cameraViewMatrix, const Matrix& cameraRotation, const Vector3& rotation,
                              const Point2& screenResolution)
  {
  }


  void WhiteRectScene::Draw()
  {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(m_program.Get());
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_texWhite.Get());

      glBindBuffer(m_vbRect.GetTarget(), m_vbRect.Get());
      m_vbRect.EnableAttribArrays();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      m_vbRect.DisableAttribArrays();
    }
  }
}
