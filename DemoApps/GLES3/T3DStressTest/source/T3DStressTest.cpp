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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/System/Threading/Thread.hpp>
#include <FslGraphics3D/Procedural/MeshBuilder.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/TextureUtil.hpp>
#include <Shared/T3DStressTest/FurTexture.hpp>
#include <Shared/T3DStressTest/OptionParser.hpp>
#include <GLES3/gl3.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include "Shader/ShaderBase.hpp"

namespace Fsl
{
  namespace
  {
    using namespace Procedural;
    using namespace GLES3;

    void CreateInstancedMesh(BasicMesh& rMesh, const std::size_t desiredInstanceCount, const bool shareInstanceVertices)
    {
      using BasicMeshBuilder = MeshBuilder<BasicMesh::vertex_type, BasicMesh::index_type>;

      BasicMeshBuilder meshBuilder(rMesh.GetPrimitiveType());
      const auto instances = meshBuilder.TryAppendInstances(rMesh, desiredInstanceCount, shareInstanceVertices);
      meshBuilder.ExtractTo(rMesh);

      if (instances != desiredInstanceCount)
      {
        FSLLOG3_WARNING("Created: {} instead of the requested: {}", instances, desiredInstanceCount);
      }
    }

    //! Create the main texture
    GLTexture CreateMainTexture(const std::shared_ptr<IContentManager>& contentManager)
    {
      Bitmap bitmap;
      contentManager->Read(bitmap, "Seamless.jpg", PixelFormat::R8G8B8A8_UNORM);
      GLTextureParameters texParams1(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      return GLTexture(bitmap, texParams1);
    }

    //! Create the fur 'density' bitmap
    GLTexture CreateFurDensityTexture(const int furTextureDim, const float hairDensity, const int layerCount)
    {
      // Create the fur 'density' bitmap
      const std::vector<uint8_t> furBitmapContent = FurTexture::Generate(furTextureDim, furTextureDim, hairDensity, layerCount);
      const RawBitmap furBitmap(furBitmapContent.data(), furTextureDim, furTextureDim, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      GLTextureParameters texParams(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
      return GLTexture(furBitmap, texParams);
    }

    Procedural::BasicMesh CreateMesh(const PxSize2D& tex1Size, const int textureRepeatCount, const Point2& vertexCount, int instanceCount,
                                     const bool shareInstanceVertices, const bool useTriangleStrip)
    {
      TextureRectangle texRect(PxRectangle(0, 0, tex1Size.Width(), tex1Size.Height()), tex1Size);
      const NativeTextureArea texArea(TextureUtil::CalcTextureArea(texRect, textureRepeatCount, textureRepeatCount));

      BasicMesh mesh;
      if (useTriangleStrip)
      {
        mesh = SegmentedQuadGenerator::GenerateStrip(Vector3::Zero(), 100, 100, vertexCount.X - 1, vertexCount.Y - 1, texArea, WindingOrder::CCW);
      }
      else
      {
        mesh = SegmentedQuadGenerator::GenerateList(Vector3::Zero(), 100, 100, vertexCount.X - 1, vertexCount.Y - 1, texArea, WindingOrder::CCW);
      }

      // OpenGL ES expects that the index count is <= 0xFFFF
      if (mesh.GetIndexCount() > 0xFFFF)
      {
        throw NotSupportedException("Maximum IndexCount exceeded");
      }

      // Create instances if so desired
      if (instanceCount > 1)
      {
        CreateInstancedMesh(mesh, instanceCount, shareInstanceVertices);
      }
      return mesh;
    }
  }


  T3DStressTest::T3DStressTest(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_config(config.GetOptions<OptionParser>()->GetConfig())
    , m_shaderMultiPass(*GetContentManager(), "MultiPass", m_config.GetUseHighShaderPrecision(), m_config.GetLightCount())
    , m_shaderInstanced(*GetContentManager(), "Instanced", m_config.GetUseHighShaderPrecision(), m_config.GetLightCount())
    , m_shaderWhite(*GetContentManager())
    , m_xAngle(0)
    , m_yAngle(0)
    , m_zAngle(0)
    , m_gravity(0, -1.0f, 0)
    , m_radians(0.0f)
  {
    switch (m_config.GetRenderMode())
    {
    case RenderMode::MultiPass:
      FSLLOG3_INFO("RenderMode: MultiPass");
      break;
    case RenderMode::Instanced:
      FSLLOG3_INFO("RenderMode: Instanced");
      break;
    default:
      break;
    }

    auto contentManager = GetContentManager();
    const int furTextureDim = m_config.GetFurTextureDimensions();

    m_resources.Tex1 = CreateMainTexture(contentManager);
    m_resources.Tex2 = CreateFurDensityTexture(furTextureDim, m_config.GetHairDensity(), m_config.GetLayerCount());

    {
      Point2 vertexCount(m_config.GetVertexCountX(), m_config.GetVertexCountY());
      auto mesh = CreateMesh(m_resources.Tex1.GetSize(), m_config.GetTextureRepeatCount(), vertexCount, m_config.GetInstanceCount(),
                             m_config.GetShareInstanceVertices(), m_config.GetUseTriangleStrip());
      m_resources.MeshStuff = std::make_unique<MeshStuffRecord>(mesh);
    }

    {
      Vector3 lightDirection(-0.0f, -0.0f, -1.0f);
      lightDirection.Normalize();
      Vector3 lightColor(0.9f, 0.9f, 0.9f);
      Vector3 ambientColor(0.2f, 0.2f, 0.2f);
      // Vector3 ambientColor(0.5f, 0.5f, 0.5f);

      {    // Prepare the shader
        ShaderBase::ScopedUse shaderScope(m_shaderMultiPass);
        m_shaderMultiPass.SetTexture0(0);
        m_shaderMultiPass.SetTexture1(1);
        m_shaderMultiPass.SetMaxHairLength(m_config.GetHairLength());
        for (int i = 0; i < m_config.GetLightCount(); ++i)
        {
          m_shaderMultiPass.SetLightDirection(i, lightDirection);
          m_shaderMultiPass.SetLightColor(i, lightColor);
        }
        m_shaderMultiPass.SetLightAmbientColor(ambientColor);
      }
      {    // Prepare the shader
        ShaderBase::ScopedUse shaderScope(m_shaderInstanced);
        m_shaderInstanced.SetTexture0(0);
        m_shaderInstanced.SetTexture1(1);
        m_shaderInstanced.SetMaxHairLength(m_config.GetHairLength());
        for (int i = 0; i < m_config.GetLightCount(); ++i)
        {
          m_shaderInstanced.SetLightDirection(i, lightDirection);
          m_shaderInstanced.SetLightColor(i, lightColor);
        }
        m_shaderInstanced.SetLightAmbientColor(ambientColor);
      }
    }
  }


  T3DStressTest::~T3DStressTest() = default;


  void T3DStressTest::FixedUpdate(const DemoTime& /*demoTime*/)
  {
    Vector3 forceDirection(std::sin(m_radians), 0, 0);
    m_displacement = m_gravity + forceDirection;

    m_radians += 0.01f;

    m_world = Matrix::CreateRotationX(MathHelper::TO_RADS * (float(m_xAngle) / 100.0f));
    m_world *= Matrix::CreateRotationY(MathHelper::TO_RADS * (float(m_yAngle) / 100.0f));
    m_world *= Matrix::CreateRotationZ(MathHelper::TO_RADS * (float(m_zAngle) / 100.0f));

    // Pull the camera back from the cube
    m_view = Matrix::CreateTranslation(0.0f, 0.0f, -m_config.GetCameraDistance());

    m_perspective = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), GetWindowAspectRatio(), 1, 100.0f);
    m_MVP = m_world * m_view * m_perspective;

    // m_xAngle += 10;
    // m_yAngle += 5;
    // m_zAngle += 6;

    if (m_xAngle >= 36000)
    {
      m_xAngle -= 36000;
    }
    if (m_xAngle < 0)
    {
      m_xAngle += 36000;
    }
    if (m_yAngle >= 36000)
    {
      m_yAngle -= 30600;
    }
    if (m_yAngle < 0)
    {
      m_yAngle += 36000;
    }
    if (m_zAngle >= 36000)
    {
      m_zAngle -= 36000;
    }
    if (m_zAngle < 0)
    {
      m_zAngle += 36000;
    }
  }


  void T3DStressTest::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    //  glEnable(GL_FRONT_AND_BACK);
    // glDisable(GL_CULL_FACE);

    if (m_config.GetEnableDepthTest())
    {
      glEnable(GL_DEPTH_TEST);
    }
    else
    {
      glDisable(GL_DEPTH_TEST);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    // Setup the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_resources.Tex1.Get());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_resources.Tex2.Get());
    bool bypassRender = false;
    if (m_config.GetToggleMinMax())
    {
      std::time_t result = std::time(nullptr);
      if ((result % 10) < 5)
      {
        bypassRender = true;
        Thread::SleepMilliseconds(16);
      }
    }
    if (!bypassRender)
    {
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      // Draw the fur mesh
      if (m_config.GetRenderMode() == RenderMode::MultiPass)
      {
        ShaderBase::ScopedUse shaderScope(m_shaderMultiPass);

        m_shaderMultiPass.SetWorld(m_world);
        m_shaderMultiPass.SetView(m_view);
        m_shaderMultiPass.SetProjection(m_perspective);
        m_shaderMultiPass.SetDisplacement(m_displacement);

        float layerAdd = (m_config.GetLayerCount() > 1 ? 1.0f / float(m_config.GetLayerCount() - 1) : 1);
        float layer = 0.0f;

        MeshRender& render = m_resources.MeshStuff->Render;
        render.Bind(m_shaderMultiPass);
        const int layerCount = m_config.GetLayerCount();
        for (int i = 0; i < layerCount; ++i)
        {
          m_shaderMultiPass.SetCurrentLayer(layer);

          render.Draw();
          layer += layerAdd;
        }
        render.Unbind();
      }
      else
      {
        assert(m_config.GetRenderMode() == RenderMode::Instanced);
        ShaderBase::ScopedUse shaderScope(m_shaderInstanced);

        m_shaderInstanced.SetWorld(m_world);
        m_shaderInstanced.SetView(m_view);
        m_shaderInstanced.SetProjection(m_perspective);
        m_shaderInstanced.SetDisplacement(m_displacement);

        float layerAdd = (m_config.GetLayerCount() > 1 ? 1.0f / float(m_config.GetLayerCount() - 1) : 1);
        m_shaderInstanced.SetInstanceDistance(layerAdd);

        MeshRender& render = m_resources.MeshStuff->Render;
        render.Bind(m_shaderInstanced);
        render.DrawInstanced(m_config.GetLayerCount());
        render.Unbind();
      }

      // Draw normals
      if (m_config.GetShowNormals())
      {
        ShaderBase::ScopedUse shaderScope(m_shaderWhite);

        m_shaderWhite.SetWorldViewProjection(m_MVP);

        MeshRender& render = m_resources.MeshStuff->RenderNormals;

        render.Bind(m_shaderWhite);
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
