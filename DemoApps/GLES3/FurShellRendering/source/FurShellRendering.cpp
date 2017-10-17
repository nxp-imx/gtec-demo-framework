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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslGraphics3D/Procedural/MeshBuilder.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslGraphics3D/Procedural/BoxGenerator.hpp>
#include <FslGraphics3D/Procedural/TorusGenerator.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include "FurShellRendering.hpp"
#include "FurTexture.hpp"
#include "OptionParser.hpp"
#include "Shader/ShaderBase.hpp"
#include <GLES3/gl3.h>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;

  struct ProceduralPrimitive
  {
    enum Enum
    {
      Torus,
      Box
    };
  };

  namespace
  {
    void BuildVB(GLVertexBuffer& rVB, const BoxF& coords, const BoxF& uv)
    {
      VertexPositionTexture vertices[] =
      {
        VertexPositionTexture(Vector3(coords.X1, coords.Y2, 0.0f), Vector2(uv.X1, uv.Y2)),
        VertexPositionTexture(Vector3(coords.X1, coords.Y1, 0.0f), Vector2(uv.X1, uv.Y1)),
        VertexPositionTexture(Vector3(coords.X2, coords.Y2, 0.0f), Vector2(uv.X2, uv.Y2)),
        VertexPositionTexture(Vector3(coords.X2, coords.Y1, 0.0f), Vector2(uv.X2, uv.Y1)),
      };

      rVB.Reset(vertices, 4, GL_STATIC_DRAW);
    }
  }



  FurShellRendering::FurShellRendering(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_config(config.GetOptions<OptionParser>()->GetConfig())
    , m_program()
    , m_meshStuff()
    , m_tex1()
    , m_tex2()
    , m_shaderES3MultiPass(*GetContentManager(), "ES3MultiPass", false, m_config.GetLightCount())
    , m_shaderES3Instanced(*GetContentManager(), "ES3Instanced", false, m_config.GetLightCount())
    , m_shaderES3InstancedLayer0(*GetContentManager(), "ES3Instanced2/Fur_Layer0.vert", "ES3Instanced2/Fur_Layer0.frag", 1)
    , m_shaderES3InstancedLayerN(*GetContentManager(), "ES3Instanced2/Fur_LayerN.vert", "ES3Instanced2/Fur_LayerN.frag", 1)
    , m_shader2(*GetContentManager())
    , m_perspectiveZ(400.0f)
    , m_xAngle(0)
    , m_yAngle(MathHelper::ToRadians(270))
    , m_zAngle(0)
    , m_xSpeed(0)
    , m_ySpeed(0)
    , m_zSpeed(0)
    , m_cameraAngleX(0)
    , m_cameraPosY(0)
    , m_gravity(0, -1.0f, 0)
    , m_displacement()
    , m_backgroundColor()
    , m_radians(0.0f)
    , m_enableDepthTest(true)
    , m_enableForce(true)
  {
    using namespace GLES3;

    const Point2 furTextureDim = m_config.GetFurTextureDimensions();
    const float hairDensity =m_config.GetHairDensity();
    float hairLength = m_config.GetHairLength();
    const int layerCount = m_config.GetLayerCount();

    const Vector4 color(Color(m_config.GetBackgroundColor()).ToVector4());
    m_backgroundColor = Vector3(color.X, color.Y, color.Z);


    { // Create the main texture (we use a scope here so we throw away the bitmap as soon as we don't need it)
      Bitmap bitmap;

      std::string strPath("Seamless.jpg");
      switch (m_config.GetDemoId())
      {
      case 0:
      case 2:
      case 3:
        strPath = "SeamlessFur.png";
        break;
      default:
        break;
      }

      GetContentManager()->Read(bitmap, strPath, PixelFormat::R8G8B8_UNORM);
      GLTextureParameters texParams1(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_tex1.SetData(bitmap, texParams1);
    }

    // Create the fur 'density' bitmap
    //if (furTextureDim.X != 1024 || furTextureDim.Y != 512)
    if (m_config.GetDemoId() != 2)
    {
      const std::vector<uint8_t> furBitmapContent = FurTexture::GenerateSmooth(furTextureDim.X, furTextureDim.Y, hairDensity, layerCount);
      const RawBitmap furBitmap(&furBitmapContent[0], furTextureDim.X, furTextureDim.Y, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      GLTextureParameters texParams2(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
      m_tex2.SetData(furBitmap, texParams2);
    }
    else
    {
      const std::vector<uint8_t> furBitmapContent = FurTexture::GenerateWave(furTextureDim.X, furTextureDim.Y, hairDensity, layerCount);
      const RawBitmap furBitmap(&furBitmapContent[0], furTextureDim.X, furTextureDim.Y, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      GLTextureParameters texParams2(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
      m_tex2.SetData(furBitmap, texParams2);
    }
    //else
    //{
    //  Bitmap bitmap;

    //  std::string strPath("Density2.png");
    //  GetContentManager()->Read(bitmap, strPath, PixelFormat::R8G8B8A8_UNORM);
    //  GLTextureParameters texParams1(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    //  m_tex2.SetData(bitmap, texParams1);
    //}


    Vector3 lightDirection;
    Vector3 lightColor;
    Vector3 ambientColor;
    ProceduralPrimitive::Enum primitive = ProceduralPrimitive::Torus;
    {
      using namespace Procedural;
      const Point2 tex1Size = m_tex1.GetSize();
      TextureRectangle texRect(Rectangle(0, 0, tex1Size.X, tex1Size.Y), tex1Size);
      const NativeTextureArea texArea(GLTexture::CalcTextureArea(texRect, m_config.GetTextureRepeatCountX(), m_config.GetTextureRepeatCountY()));
      WindingOrder::Enum windingOrder;
      float radius;
      float ringRadius;
      switch (m_config.GetDemoId())
      {
      case 1:
        windingOrder = WindingOrder::CW;
        radius = 60;
        ringRadius = 40;
        lightDirection = Vector3(0.0f, 0.0f, -1.0f);
        m_gravity = Vector3(0.0f, -1.0f, 0.0f);
        lightColor = Vector3(0.9f, 0.9f, 0.9f);
        ambientColor = Vector3(0.2f, 0.2f, 0.2f);
        m_xSpeed = 0.0f;
        m_ySpeed = 0.12f;
        m_zSpeed = 0.14f;
        break;
      case 2:
        primitive = ProceduralPrimitive::Box;
        windingOrder = WindingOrder::CCW;
        radius = 30;
        ringRadius = 4;
        lightDirection = Vector3(0.0f, 0.2f, -1.0f);
        lightColor = Vector3(0.9f, 0.9f, 0.9f);
        ambientColor = Vector3(0.4f, 0.4f, 0.4f);
        m_gravity = Vector3(0.0f, -0.7f, 0.0f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.32f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.44f;
        hairLength *= 0.50f;
        break;
      case 3:
        windingOrder = WindingOrder::CCW;
        radius = 30;
        ringRadius = 4;
        lightDirection = Vector3(0.0f, 0.2f, -1.0f);
        lightColor = Vector3(0.9f, 0.9f, 0.9f);
        ambientColor = Vector3(0.4f, 0.4f, 0.4f);
        m_gravity = Vector3(0.0f, -0.4f, 0.0f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.32f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.44f;
        hairLength *= 0.50f;
        break;
      case 0:
      default:
        windingOrder = WindingOrder::CCW;
        radius = 30;
        ringRadius = 4;
        lightDirection = Vector3(0.0f, 0.2f, -1.0f);
        lightColor = Vector3(0.9f, 0.9f, 0.9f);
        ambientColor = Vector3(0.4f, 0.4f, 0.4f);
        m_gravity = Vector3(0.0f, 0.0f, 0.0f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.0f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.0f;
        m_xAngle = 0.0f;
        m_cameraAngleX = 20 * MathHelper::TO_RADS;
        m_cameraPosY = -0.5f;
        hairLength *= 0.50f;
        m_enableForce = false;
        break;
      }


      BasicMesh mesh;
      if (primitive == ProceduralPrimitive::Box)
      {
        NativeTextureArea texAreas[6] = { texArea, texArea, texArea, texArea, texArea, texArea };
        if (m_config.GetUseTriangleStrip())
          mesh = BoxGenerator::GenerateStrip(Vector3::Zero(), 30, 30, 30, texAreas, 6, windingOrder);
        else
          mesh = BoxGenerator::GenerateList(Vector3::Zero(), 30, 30, 30, texAreas, 6, windingOrder);
      }
      else
      {
        if (m_config.GetUseTriangleStrip())
          mesh = TorusGenerator::GenerateStrip(m_config.GetTorusMajorSegments(), m_config.GetTorusMinorSegments(), radius, ringRadius, texArea, windingOrder);
        else
          mesh = TorusGenerator::GenerateList(m_config.GetTorusMajorSegments(), m_config.GetTorusMinorSegments(), radius, ringRadius, texArea, windingOrder);
      }

      // OpenGL ES expects that the index count is <= 0xFFFF
      assert(mesh.GetIndexCount() <= 0xFFFF);

      m_meshStuff.reset(new MeshStuff(mesh));
      m_meshStuff->RenderES3Instanced.SetInstanceCount(layerCount);
      m_meshStuff->RenderES3InstancedLayerN.SetInstanceCount(layerCount-1);
    }


    lightDirection.Normalize();

    { // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderES3MultiPass);
      m_shaderES3MultiPass.SetTexture0(0);
      m_shaderES3MultiPass.SetTexture1(1);
      m_shaderES3MultiPass.SetMaxHairLength(hairLength);
      m_shaderES3MultiPass.SetLightDirection(0, lightDirection);
      m_shaderES3MultiPass.SetLightColor(0, lightColor);
      m_shaderES3MultiPass.SetLightAmbientColor(ambientColor);
    }
    { // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderES3Instanced);
      m_shaderES3Instanced.SetTexture0(0);
      m_shaderES3Instanced.SetTexture1(1);
      m_shaderES3Instanced.SetMaxHairLength(hairLength);
      m_shaderES3Instanced.SetLightDirection(0, lightDirection);
      m_shaderES3Instanced.SetLightColor(0, lightColor);
      m_shaderES3Instanced.SetLightAmbientColor(ambientColor);
      m_shaderES3Instanced.SetInstanceCount(layerCount);
    }
    { // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderES3InstancedLayer0);
      m_shaderES3InstancedLayer0.SetTexture0(0);
      m_shaderES3InstancedLayer0.SetTexture1(1);
      m_shaderES3InstancedLayer0.SetMaxHairLength(hairLength);
      m_shaderES3InstancedLayer0.SetLightDirection(0, lightDirection);
      m_shaderES3InstancedLayer0.SetLightColor(0, lightColor);
      m_shaderES3InstancedLayer0.SetLightAmbientColor(ambientColor);
    }
    { // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderES3InstancedLayerN);
      m_shaderES3InstancedLayerN.SetTexture0(0);
      m_shaderES3InstancedLayerN.SetTexture1(1);
      m_shaderES3InstancedLayerN.SetMaxHairLength(hairLength);
      m_shaderES3InstancedLayerN.SetLightDirection(0, lightDirection);
      m_shaderES3InstancedLayerN.SetLightColor(0, lightColor);
      m_shaderES3InstancedLayerN.SetLightAmbientColor(ambientColor);
      m_shaderES3InstancedLayerN.SetInstanceCount(layerCount);
    }

    // Build the description VB
    {
      {
        BasicTextureAtlas atlas;
        GetContentManager()->Read(atlas, "MainAtlas.bta");
        Fsl::Bitmap bitmap;
        GetContentManager()->Read(bitmap, "MainAtlas.png", PixelFormat::R8G8B8A8_UNORM);
        GLTextureParameters texParams(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
        m_texDescriptionAtlas.Reset(bitmap, texParams);
        m_texDescription = TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Banners");
      }

      const Vector2 res(config.ScreenResolution.X / 2, config.ScreenResolution.Y / 2);
      const Vector2 atlasSize(m_texDescriptionAtlas.GetSize().X, m_texDescriptionAtlas.GetSize().Y);

      //texSize.X / tex
      float x1 = -1.0f - (m_texDescription.Offset.X / res.X);
      float x2 = x1 + (m_texDescription.TrimmedRect.Width() / res.X);
      float y1 = -1.0f - (m_texDescription.Offset.Y / res.Y);
      float y2 = y1 + (m_texDescription.TrimmedRect.Height() / res.Y);

      float u1 = m_texDescription.TrimmedRect.Left() / atlasSize.X;
      float v1 = m_texDescription.TrimmedRect.Top() / atlasSize.Y;
      float u2 = m_texDescription.TrimmedRect.Right() / atlasSize.X;
      float v2 = m_texDescription.TrimmedRect.Bottom() / atlasSize.Y;

      BuildVB(m_vbDescription, BoxF(x1, -y2, x2, -y1), BoxF(u1, v1, u2, v2));

      m_basicProgram.Reset(GetContentManager()->ReadAllText("BasicShader.vert"), GetContentManager()->ReadAllText("BasicShader.frag"));
    }
  }


  FurShellRendering::~FurShellRendering()
  {

  }


  void FurShellRendering::Update(const DemoTime& demoTime)
  {
    const Point2 screenResolution = GetScreenResolution();

    if (m_enableForce)
    {
      Vector3 forceDirection(std::sin(m_radians), 0, 0);
      m_displacement = m_gravity + forceDirection;
    }
    else
      m_displacement = m_gravity;

    //m_xAngle = 0;
    //m_yAngle = 0;
    //m_zAngle = 0;
    m_world = Matrix::CreateRotationX(-m_xAngle);
    m_world *= Matrix::CreateRotationY(m_yAngle);
    m_world *= Matrix::CreateRotationZ(m_zAngle);

    const float cameraDistance = 100.0f;
    m_view = Matrix::CreateRotationX(m_cameraAngleX);
    m_view *= Matrix::CreateTranslation(0.0f, m_cameraPosY, -cameraDistance);

    m_perspective = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), screenResolution.X / (float)screenResolution.Y, 1, m_perspectiveZ);
    m_MVP = m_world * m_view * m_perspective;

    m_radians += 1.00f * demoTime.DeltaTime;
    m_xAngle += m_xSpeed * demoTime.DeltaTime;
    m_yAngle += m_ySpeed * demoTime.DeltaTime;
    m_zAngle += m_xSpeed * demoTime.DeltaTime;
  }


  void FurShellRendering::Draw(const DemoTime& demoTime)
  {
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(m_backgroundColor.X, m_backgroundColor.Y, m_backgroundColor.Z, 1.0f);

    if (m_enableDepthTest)
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);

    // Setup the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex1.Get());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_tex2.Get());

    const int layerCount = m_config.GetLayerCount();

    bool bypassRender = false;
    if (bypassRender == false)
    {
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      switch (m_config.GetRenderMode())
      {
      case RenderMode::ES3MultiPass:
        DrawES3Multipass(m_shaderES3MultiPass, m_meshStuff->Render, m_world, m_view, m_perspective, m_displacement, layerCount);
        break;
      case RenderMode::ES3MultiPassVB:
        DrawES3Multipass(m_shaderES3MultiPass, m_meshStuff->RenderVB, m_world, m_view, m_perspective, m_displacement, layerCount);
        break;
      case RenderMode::ES3Instanced:
        DrawES3Instanced(m_shaderES3Instanced, m_meshStuff->RenderES3Instanced, m_world, m_view, m_perspective, m_displacement);
        break;
      case RenderMode::ES3Instanced2:
        glDisable(GL_BLEND);
        DrawES3Instanced(m_shaderES3InstancedLayer0, m_meshStuff->RenderVB, m_world, m_view, m_perspective, m_displacement);
        if (layerCount > 1)
        {
          glEnable(GL_BLEND);
          DrawES3Instanced(m_shaderES3InstancedLayerN, m_meshStuff->RenderES3InstancedLayerN, m_world, m_view, m_perspective, m_displacement);
        }
        break;
      default:
        break;
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

      {
        // Draw the description using the static vertex buffer
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texDescriptionAtlas.Get());
        glUseProgram(m_basicProgram.Get());
        glBindBuffer(m_vbDescription.GetTarget(), m_vbDescription.Get());
        m_vbDescription.EnableAttribArrays();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      }
    }

    if (m_config.GetForceFinishEachFrame())
    {
      // execute all in pipe before presentation - workaround for eglSwapBuffer not flushing pipe
      glFinish();
    }
  }


  void FurShellRendering::DrawES3Multipass(FurShaderMultiPass& rShader, MeshRender& rRender, const Matrix& world, const Matrix& view, const Matrix& perspective, const Vector3& displacement, const int layerCount)
  {
    ShaderBase::ScopedUse shaderScope(rShader);

    rShader.SetWorld(world);
    rShader.SetView(view);
    rShader.SetProjection(perspective);
    rShader.SetDisplacement(displacement);

    float layerAdd = (layerCount > 1 ? 1.0f / (layerCount - 1) : 1);
    float layer = 0.0f;

    rRender.Bind(rShader);
    for (int i = 0; i < layerCount; ++i)
    {
      rShader.SetCurrentLayer(layer);

      rRender.Draw();
      layer += layerAdd;
    }
    rRender.Unbind();

  }


  void FurShellRendering::DrawES3Instanced(FurShaderBase& rShader, MeshRender& rRender, const Matrix& world, const Matrix& view, const Matrix& perspective, const Vector3& displacement)
  {
    ShaderBase::ScopedUse shaderScope(rShader);

    rShader.SetWorld(world);
    rShader.SetView(view);
    rShader.SetProjection(perspective);
    rShader.SetDisplacement(displacement);

    rRender.Bind(rShader);
    rRender.Draw();
    rRender.Unbind();
  }
}
