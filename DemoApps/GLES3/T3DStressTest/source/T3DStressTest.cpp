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

#include "T3DStressTest.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslGraphics3D/Procedural/MeshBuilder.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <GLES3/gl3.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include "FurTexture.hpp"
#include "Shader/ShaderBase.hpp"
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
    using namespace Procedural;

    void CreateInstancedMesh(BasicMesh& rMesh, const std::size_t desiredInstanceCount, const bool shareInstanceVertices)
    {
      typedef MeshBuilder<BasicMesh::vertex_type, BasicMesh::index_type> BasicMeshBuilder;

      BasicMeshBuilder meshBuilder(rMesh.GetPrimitiveType());
      const auto instances = meshBuilder.TryAppendInstances(rMesh, desiredInstanceCount, shareInstanceVertices);
      meshBuilder.ExtractTo(rMesh);

      if (instances != desiredInstanceCount)
      {
        FSLLOG_WARNING("Created: " << instances << " instead of the requested: " << desiredInstanceCount);
      }
    }
  }


  T3DStressTest::T3DStressTest(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_program()
    , m_config()
    , m_meshStuff()
    , m_tex1()
    , m_tex2()
    , m_shader1(*GetContentManager(), "", m_config.GetUseHighShaderPrecision(), m_config.GetLightCount())
    , m_shader2(*GetContentManager())
    , m_xAngle(0)
    , m_yAngle(0)
    , m_zAngle(0)
    , m_gravity(0, -1.0f, 0)
    , m_displacement()
    , m_radians(0.0f)
  {
    using namespace GLES3;

    std::shared_ptr<OptionParser> options = config.GetOptions<OptionParser>();
    m_config = options->GetConfig();

    const int furTextureDim = m_config.GetFurTextureDimensions();


    { // Create the main texture (we use a scope here so we throw away the bitmap as soon as we don't need it)
      Bitmap bitmap;
      GetContentManager()->Read(bitmap, "Seamless.jpg");
      GLTextureParameters texParams1(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_tex1.SetData(bitmap, texParams1);
    }

    // Create the fur 'density' bitmap
    const std::vector<uint8_t> furBitmapContent = FurTexture::Generate(furTextureDim, furTextureDim, m_config.GetHairDensity(), m_config.GetLayerCount());
    const RawBitmap furBitmap(&furBitmapContent[0], furTextureDim, furTextureDim, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
    GLTextureParameters texParams2(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    m_tex2.SetData(furBitmap, texParams2);


    {
      using namespace Procedural;
      const Point2 tex1Size = m_tex1.GetSize();
      TextureRectangle texRect(Rectangle(0, 0, tex1Size.X, tex1Size.Y), tex1Size);
      const NativeTextureArea texArea(GLTexture::CalcTextureArea(texRect, m_config.GetTextureRepeatCount(), m_config.GetTextureRepeatCount()));

      BasicMesh mesh;
      if (m_config.GetUseTriangleStrip())
        mesh = SegmentedQuadGenerator::GenerateStrip(Vector3::Zero(), 100, 100, m_config.GetVertexCountX() - 1, m_config.GetVertexCountY() - 1, texArea, WindingOrder::CCW);
      else
        mesh = SegmentedQuadGenerator::GenerateList(Vector3::Zero(), 100, 100, m_config.GetVertexCountX() - 1, m_config.GetVertexCountY() - 1, texArea, WindingOrder::CCW);

      // OpenGL ES expects that the index count is <= 0xFFFF
      assert(mesh.GetIndexCount() <= 0xFFFF);

      // Create instances if so desired
      if (m_config.GetInstanceCount() > 1)
        CreateInstancedMesh(mesh, m_config.GetInstanceCount(), m_config.GetShareInstanceVertices());

      // OpenGL ES expects that the index count is <= 0xFFFF
      assert(mesh.GetIndexCount() <= 0xFFFF);

      m_meshStuff.reset(new MeshStuff(mesh));
    }

    // Setup the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex1.Get());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_tex2.Get());

    Vector3 lightDirection(-0.0f, -0.0f, -1.0f);
    lightDirection.Normalize();
    Vector3 lightColor(0.9f, 0.9f, 0.9f);
    Vector3 ambientColor(0.2f, 0.2f, 0.2f);
    //Vector3 ambientColor(0.5f, 0.5f, 0.5f);

    { // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shader1);
      m_shader1.SetTexture0(0);
      m_shader1.SetTexture1(1);
      m_shader1.SetMaxHairLength(m_config.GetHairLength());
      m_shader1.SetLightDirection(0, lightDirection);
      m_shader1.SetLightColor(0, lightColor);
      m_shader1.SetLightAmbientColor(ambientColor);
    }

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    //  glEnable(GL_FRONT_AND_BACK);
    //glDisable(GL_CULL_FACE);

    if (m_config.GetEnableDepthTest())
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  }


  T3DStressTest::~T3DStressTest()
  {
  }


  void T3DStressTest::FixedUpdate(const DemoTime& demoTime)
  {
    const Point2 screenResolution = GetScreenResolution();

    Vector3 forceDirection(std::sin(m_radians), 0, 0);
    m_displacement = m_gravity + forceDirection;

    m_radians += 0.01f;

    m_world = Matrix::CreateRotationX(MathHelper::TO_RADS * (m_xAngle / 100.0f));
    m_world *= Matrix::CreateRotationY(MathHelper::TO_RADS * (m_yAngle / 100.0f));
    m_world *= Matrix::CreateRotationZ(MathHelper::TO_RADS * (m_zAngle / 100.0f));

    // Pull the camera back from the cube
    m_view = Matrix::CreateTranslation(0.0f, 0.0f, -m_config.GetCameraDistance());

    m_perspective = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), screenResolution.X / (float)screenResolution.Y, 1, 100.0f);
    m_MVP = m_world * m_view * m_perspective;

    //m_xAngle += 10;
    //m_yAngle += 5;
    //m_zAngle += 6;

    if (m_xAngle >= 36000) m_xAngle -= 36000;
    if (m_xAngle < 0) m_xAngle += 36000;
    if (m_yAngle >= 36000) m_yAngle -= 30600;
    if (m_yAngle < 0) m_yAngle += 36000;
    if (m_zAngle >= 36000) m_zAngle -= 36000;
    if (m_zAngle < 0) m_zAngle += 36000;
  }


  void T3DStressTest::Draw(const DemoTime& demoTime)
  {
    bool bypassRender = false;
    if (m_config.GetToggleMinMax() == true)
    {
      std::time_t result = std::time(NULL);
      if ((result % 10) < 5)
      {
        bypassRender = true;
        FSLLOG_WARNING("Thread sleep not implemented yet");
        //    //FIX: Thread::Sleep(16);
      }
    }
    if (bypassRender == false)
    {
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      // Draw the fur mesh
      {
        ShaderBase::ScopedUse shaderScope(m_shader1);

        m_shader1.SetWorld(m_world);
        m_shader1.SetView(m_view);
        m_shader1.SetProjection(m_perspective);
        m_shader1.SetDisplacement(m_displacement);

        float layerAdd = (m_config.GetLayerCount() > 1 ? 1.0f / (m_config.GetLayerCount() - 1) : 1);
        float layer = 0.0f;

        MeshRender& render = m_meshStuff->Render;
        render.Bind(m_shader1);
        const int layerCount = m_config.GetLayerCount();
        for (int i = 0; i < layerCount; ++i)
        {
          m_shader1.SetCurrentLayer(layer);

          render.Draw();
          layer += layerAdd;
        }
        render.Unbind();
      }

      // Draw normals
      if (m_config.GetShowNormals())
      {
        ShaderBase::ScopedUse shaderScope(m_shader2);

        m_shader2.SetWorldViewProjection(m_MVP);

        MeshRender& render = m_meshStuff->RenderNormals;

        render.Bind(m_shader2);
        render.Draw();
        render.Unbind();
      }
    }

    if (m_config.GetForceFinishEachFrame())
    {
      // execute all in pipe before presentation - workaround for eglSwapBuffer not flushing pipe
      glFinish();
    }
  }

}
