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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
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
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/TextureUtil.hpp>
#include <Shared/FurShellRendering/OptionParser.hpp>
#include <Shared/FurShellRendering/FurTexture.hpp>
#include <array>
#include <memory>
#include <GLES3/gl3.h>
#include "FurShellRendering.hpp"
#include "RenderMode.hpp"
#include "Shader/ShaderBase.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    enum class ProceduralPrimitive
    {
      Torus,
      Box
    };

    struct ProceduralConfig
    {
      ProceduralPrimitive Primitive{ProceduralPrimitive::Torus};
      WindingOrder::Enum Winding{WindingOrder::CW};
      float Radius{};
      float RingRadius{};
    };

    void BuildVB(GLVertexBuffer& rVB, const BoxF& coords, const BoxF& uv)
    {
      const std::array<VertexPositionTexture, 4> vertices = {
        VertexPositionTexture(Vector3(coords.X1, coords.Y2, 0.0f), Vector2(uv.X1, uv.Y2)),
        VertexPositionTexture(Vector3(coords.X1, coords.Y1, 0.0f), Vector2(uv.X1, uv.Y1)),
        VertexPositionTexture(Vector3(coords.X2, coords.Y2, 0.0f), Vector2(uv.X2, uv.Y2)),
        VertexPositionTexture(Vector3(coords.X2, coords.Y1, 0.0f), Vector2(uv.X2, uv.Y1)),
      };

      rVB.Reset(vertices, GL_STATIC_DRAW);
    }

    IO::Path GetDemoIdTextureName(const int demoId)
    {
      switch (demoId)
      {
      case 0:
      case 2:
      case 3:
        return "Textures/Seamless/Fur/SeamlessFur.png";
      default:
        return "Textures/Seamless/GrassPattern/Seamless.jpg";
      }
    }

    //! Create the main texture
    GLES3::GLTexture CreateMainTexture(const std::shared_ptr<IContentManager>& contentManager, const int demoId)
    {
      auto strPath = GetDemoIdTextureName(demoId);
      auto bitmap = contentManager->ReadBitmap(strPath, PixelFormat::R8G8B8_UNORM);

      GLTextureParameters texParams1(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      return GLES3::GLTexture(bitmap, texParams1);
    }

    //! Create the fur 'density' bitmap
    GLES3::GLTexture CreateFurDensityTexture(const int demoId, const Point2& furTextureDim, const float hairDensity, const int layerCount)
    {
      // if (furTextureDim.X != 1024 || furTextureDim.Y != 512)
      if (demoId != 2)
      {
        const std::vector<uint8_t> furBitmapContent = FurTexture::GenerateSmooth(furTextureDim.X, furTextureDim.Y, hairDensity, layerCount);
        const RawBitmap furBitmap(&furBitmapContent[0], furTextureDim.X, furTextureDim.Y, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
        GLTextureParameters texParams(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
        return GLES3::GLTexture(furBitmap, texParams);
      }
      const std::vector<uint8_t> furBitmapContent = FurTexture::GenerateWave(furTextureDim.X, furTextureDim.Y, hairDensity, layerCount);
      const RawBitmap furBitmap(&furBitmapContent[0], furTextureDim.X, furTextureDim.Y, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      GLTextureParameters texParams(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
      return GLES3::GLTexture(furBitmap, texParams);

      // std::string strPath("Density2.png");
      // auto bitmap = contentManager->ReadBitmap(strPath, PixelFormat::R8G8B8A8_UNORM);
      // GLTextureParameters texParams(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
      // return GLES3::GLTexture(furBitmap, texParams);
    }

    GLES3::GLTexture CreateMainAtlasTexture(const std::shared_ptr<IContentManager>& contentManager)
    {
      auto bitmap = contentManager->ReadBitmap("TextureAtlas/MainAtlas.png", PixelFormat::R8G8B8A8_UNORM);
      GLTextureParameters texParams(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
      return GLES3::GLTexture(bitmap, texParams);
    }

    AtlasTextureInfo CreateMainAtlasTextureInfo(const std::shared_ptr<IContentManager>& contentManager)
    {
      BasicTextureAtlas atlas;
      contentManager->Read(atlas, "TextureAtlas/MainAtlas.bta");
      return TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Banners");
    }


    Procedural::BasicMesh CreateMesh(const ProceduralConfig& proceduralConfig, const PxSize2D& tex1Size, const Point2& textureRepeatCount,
                                     const int torusMajorSegments, const int torusMinorSegments, const bool useTriangleStrip)
    {
      TextureRectangle texRect(PxRectangle(0, 0, tex1Size.Width(), tex1Size.Height()), tex1Size);
      const NativeTextureArea texArea(TextureUtil::CalcTextureArea(texRect, textureRepeatCount.X, textureRepeatCount.Y));
      if (proceduralConfig.Primitive == ProceduralPrimitive::Box)
      {
        const std::array<NativeTextureArea, 6> texAreas = {texArea, texArea, texArea, texArea, texArea, texArea};
        if (useTriangleStrip)
        {
          return Procedural::BoxGenerator::GenerateStrip(Vector3::Zero(), 30, 30, 30, texAreas.data(), UncheckedNumericCast<int32_t>(texAreas.size()),
                                                         proceduralConfig.Winding);
        }
        return Procedural::BoxGenerator::GenerateList(Vector3::Zero(), 30, 30, 30, texAreas.data(), UncheckedNumericCast<int32_t>(texAreas.size()),
                                                      proceduralConfig.Winding);
      }


      if (useTriangleStrip)
      {
        return Procedural::TorusGenerator::GenerateStrip(torusMajorSegments, torusMinorSegments, proceduralConfig.Radius, proceduralConfig.RingRadius,
                                                         texArea, proceduralConfig.Winding);
      }
      return Procedural::TorusGenerator::GenerateList(torusMajorSegments, torusMinorSegments, proceduralConfig.Radius, proceduralConfig.RingRadius,
                                                      texArea, proceduralConfig.Winding);
    }

  }

  FurShellRendering::FurShellRendering(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_config(config.GetOptions<OptionParser>()->GetConfig())
    , m_shaderMultiPass(*GetContentManager(), "MultiPass", false, m_config.GetLightCount())
    , m_shaderInstanced(*GetContentManager(), "Instanced", false, m_config.GetLightCount())
    , m_shaderInstancedLayer0(*GetContentManager(), "Instanced2/Fur_Layer0.vert", "Instanced2/Fur_Layer0.frag", 1)
    , m_shaderInstancedLayerN(*GetContentManager(), "Instanced2/Fur_LayerN.vert", "Instanced2/Fur_LayerN.frag", 1)
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
    , m_radians(0.0f)
    , m_enableDepthTest(true)
    , m_enableForce(true)
  {
    using namespace GLES3;

    const Point2 furTextureDim = m_config.GetFurTextureDimensions();
    const float hairDensity = m_config.GetHairDensity();
    float hairLength = m_config.GetHairLength();
    const int layerCount = m_config.GetLayerCount();

    const Vector4 color(Color(m_config.GetBackgroundColor()).ToVector4());
    m_backgroundColor = Vector3(color.X, color.Y, color.Z);

    auto contentManager = GetContentManager();

    m_resources.Tex1 = CreateMainTexture(contentManager, m_config.GetDemoId());
    m_resources.Tex2 = CreateFurDensityTexture(m_config.GetDemoId(), furTextureDim, hairDensity, layerCount);

    LightInfo lightInfo;
    ProceduralConfig proceduralConfig;
    {
      switch (m_config.GetDemoId())
      {
      case 1:
        proceduralConfig.Winding = WindingOrder::CW;
        proceduralConfig.Radius = 60;
        proceduralConfig.RingRadius = 40;
        lightInfo.Direction = Vector3(0.0f, 0.0f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.2f, 0.2f, 0.2f);
        m_xSpeed = 0.0f;
        m_ySpeed = 0.12f;
        m_zSpeed = 0.14f;
        m_gravity = Vector3(0.0f, -1.0f, 0.0f);
        break;
      case 2:
        proceduralConfig.Primitive = ProceduralPrimitive::Box;
        proceduralConfig.Winding = WindingOrder::CCW;
        proceduralConfig.Radius = 30;
        proceduralConfig.RingRadius = 4;
        lightInfo.Direction = Vector3(0.0f, 0.2f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.4f, 0.4f, 0.4f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.32f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.44f;
        m_gravity = Vector3(0.0f, -0.7f, 0.0f);
        hairLength *= 0.50f;
        break;
      case 3:
        proceduralConfig.Winding = WindingOrder::CCW;
        proceduralConfig.Radius = 30;
        proceduralConfig.RingRadius = 4;
        lightInfo.Direction = Vector3(0.0f, 0.2f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.4f, 0.4f, 0.4f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.32f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.44f;
        m_gravity = Vector3(0.0f, -0.4f, 0.0f);
        hairLength *= 0.50f;
        break;
      case 0:
      default:
        proceduralConfig.Winding = WindingOrder::CCW;
        proceduralConfig.Radius = 30;
        proceduralConfig.RingRadius = 4;
        lightInfo.Direction = Vector3(0.0f, 0.2f, -1.0f);
        lightInfo.Color = Vector3(0.9f, 0.9f, 0.9f);
        lightInfo.AmbientColor = Vector3(0.4f, 0.4f, 0.4f);
        // Modify the perspective matrix to get max resolution on the depth buffer
        m_perspectiveZ = 150.0f;
        m_xSpeed = 0.0f;
        m_ySpeed = 0.28f;
        m_zSpeed = 0.0f;
        m_xAngle = 0.0f;
        m_cameraAngleX = 20 * MathHelper::TO_RADS;
        m_cameraPosY = -0.5f;
        m_gravity = Vector3(0.0f, 0.0f, 0.0f);
        m_enableForce = false;
        hairLength *= 0.50f;
        break;
      }

      Point2 textureRepeatCount(m_config.GetTextureRepeatCountX(), m_config.GetTextureRepeatCountY());
      auto mesh = CreateMesh(proceduralConfig, m_resources.Tex1.GetSize(), textureRepeatCount, m_config.GetTorusMajorSegments(),
                             m_config.GetTorusMinorSegments(), m_config.GetUseTriangleStrip());

      // OpenGL ES expects that the index count is <= 0xFFFF
      if (mesh.GetIndexCount() > 0xFFFF)
      {
        throw NotSupportedException("Maximum IndexCount exceeded");
      }

      m_resources.MeshStuff = std::make_unique<MeshStuffRecord>(mesh);
      m_resources.MeshStuff->RenderInstanced.SetInstanceCount(layerCount);
      m_resources.MeshStuff->RenderInstancedLayerN.SetInstanceCount(layerCount - 1);
    }


    lightInfo.Direction.Normalize();

    {    // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderMultiPass);
      m_shaderMultiPass.SetTexture0(0);
      m_shaderMultiPass.SetTexture1(1);
      m_shaderMultiPass.SetMaxHairLength(hairLength);
      for (int i = 0; i < m_config.GetLightCount(); ++i)
      {
        m_shaderMultiPass.SetLightDirection(i, lightInfo.Direction);
        m_shaderMultiPass.SetLightColor(i, lightInfo.Color);
      }
      m_shaderMultiPass.SetLightAmbientColor(lightInfo.AmbientColor);
    }
    {    // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderInstanced);
      m_shaderInstanced.SetTexture0(0);
      m_shaderInstanced.SetTexture1(1);
      m_shaderInstanced.SetMaxHairLength(hairLength);
      for (int i = 0; i < m_config.GetLightCount(); ++i)
      {
        m_shaderInstanced.SetLightDirection(i, lightInfo.Direction);
        m_shaderInstanced.SetLightColor(i, lightInfo.Color);
      }
      m_shaderInstanced.SetLightAmbientColor(lightInfo.AmbientColor);
      m_shaderInstanced.SetInstanceCount(layerCount);
    }
    {    // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderInstancedLayer0);
      m_shaderInstancedLayer0.SetTexture0(0);
      m_shaderInstancedLayer0.SetTexture1(1);
      m_shaderInstancedLayer0.SetMaxHairLength(hairLength);
      m_shaderInstancedLayer0.SetLightDirection(0, lightInfo.Direction);
      m_shaderInstancedLayer0.SetLightColor(0, lightInfo.Color);
      m_shaderInstancedLayer0.SetLightAmbientColor(lightInfo.AmbientColor);
    }
    {    // Prepare the shader
      ShaderBase::ScopedUse shaderScope(m_shaderInstancedLayerN);
      m_shaderInstancedLayerN.SetTexture0(0);
      m_shaderInstancedLayerN.SetTexture1(1);
      m_shaderInstancedLayerN.SetMaxHairLength(hairLength);
      m_shaderInstancedLayerN.SetLightDirection(0, lightInfo.Direction);
      m_shaderInstancedLayerN.SetLightColor(0, lightInfo.Color);
      m_shaderInstancedLayerN.SetLightAmbientColor(lightInfo.AmbientColor);
      m_shaderInstancedLayerN.SetInstanceCount(layerCount);
    }

    // Build the description VB
    {
      m_resources.TexDescriptionAtlas = CreateMainAtlasTexture(contentManager);
      m_resources.TexDescription = CreateMainAtlasTextureInfo(contentManager);

      const Vector2 res(config.ScreenResolution.X / 2, config.ScreenResolution.Y / 2);
      const Vector2 atlasSize(TypeConverter::UncheckedTo<Vector2>(m_resources.TexDescriptionAtlas.GetSize()));

      // texSize.X / tex
      float x1 = -1.0f - (m_resources.TexDescription.OffsetPx.X / res.X);
      float x2 = x1 + (m_resources.TexDescription.TrimmedRectPx.Width / res.X);
      float y1 = -1.0f - (m_resources.TexDescription.OffsetPx.Y / res.Y);
      float y2 = y1 + (m_resources.TexDescription.TrimmedRectPx.Height / res.Y);

      float u1 = m_resources.TexDescription.TrimmedRectPx.Left() / atlasSize.X;
      float v1 = 1.0f - (m_resources.TexDescription.TrimmedRectPx.Top() / atlasSize.Y);
      float u2 = m_resources.TexDescription.TrimmedRectPx.Right() / atlasSize.X;
      float v2 = 1.0f - (m_resources.TexDescription.TrimmedRectPx.Bottom() / atlasSize.Y);

      BuildVB(m_resources.VBDescription, BoxF(x1, -y2, x2, -y1), BoxF(u1, v2, u2, v1));

      m_resources.BasicProgram.Reset(contentManager->ReadAllText("BasicShader.vert"), contentManager->ReadAllText("BasicShader.frag"));
    }
  }


  FurShellRendering::~FurShellRendering() = default;


  void FurShellRendering::Update(const DemoTime& demoTime)
  {
    if (m_enableForce)
    {
      Vector3 forceDirection(std::sin(m_radians), 0, 0);
      m_displacement = m_gravity + forceDirection;
    }
    else
    {
      m_displacement = m_gravity;
    }

    // m_xAngle = 0;
    // m_yAngle = 0;
    // m_zAngle = 0;
    m_world = Matrix::CreateRotationX(-m_xAngle);
    m_world *= Matrix::CreateRotationY(m_yAngle);
    m_world *= Matrix::CreateRotationZ(m_zAngle);

    const float cameraDistance = 100.0f;
    m_view = Matrix::CreateRotationX(m_cameraAngleX);
    m_view *= Matrix::CreateTranslation(0.0f, m_cameraPosY, -cameraDistance);

    m_perspective = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), GetWindowAspectRatio(), 1, m_perspectiveZ);
    m_MVP = m_world * m_view * m_perspective;

    m_radians += 1.00f * demoTime.DeltaTime;
    m_xAngle += m_xSpeed * demoTime.DeltaTime;
    m_yAngle += m_ySpeed * demoTime.DeltaTime;
    m_zAngle += m_xSpeed * demoTime.DeltaTime;
  }


  void FurShellRendering::Draw(const DemoTime& /*demoTime*/)
  {
    glEnable(GL_CULL_FACE);
    // glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(m_backgroundColor.X, m_backgroundColor.Y, m_backgroundColor.Z, 1.0f);

    if (m_enableDepthTest)
    {
      glEnable(GL_DEPTH_TEST);
    }
    else
    {
      glDisable(GL_DEPTH_TEST);
    }

    // Setup the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_resources.Tex1.Get());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_resources.Tex2.Get());

    const int layerCount = m_config.GetLayerCount();

    bool bypassRender = false;
    if (!bypassRender)
    {
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      switch (static_cast<RenderMode>(m_config.GetRenderMode()))
      {
      case RenderMode::MultiPass:
        DrawMultipass(m_shaderMultiPass, m_resources.MeshStuff->Render, m_world, m_view, m_perspective, m_displacement, layerCount);
        break;
      case RenderMode::MultiPassVB:
        DrawMultipass(m_shaderMultiPass, m_resources.MeshStuff->RenderVB, m_world, m_view, m_perspective, m_displacement, layerCount);
        break;
      case RenderMode::Instanced:
        DrawInstanced(m_shaderInstanced, m_resources.MeshStuff->RenderInstanced, m_world, m_view, m_perspective, m_displacement);
        break;
      case RenderMode::Instanced2:
        glDisable(GL_BLEND);
        DrawInstanced(m_shaderInstancedLayer0, m_resources.MeshStuff->RenderVB, m_world, m_view, m_perspective, m_displacement);
        if (layerCount > 1)
        {
          glEnable(GL_BLEND);
          DrawInstanced(m_shaderInstancedLayerN, m_resources.MeshStuff->RenderInstancedLayerN, m_world, m_view, m_perspective, m_displacement);
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

        MeshRender& render = m_resources.MeshStuff->RenderNormals;

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
        glBindTexture(GL_TEXTURE_2D, m_resources.TexDescriptionAtlas.Get());
        glUseProgram(m_resources.BasicProgram.Get());
        glBindBuffer(m_resources.VBDescription.GetTarget(), m_resources.VBDescription.Get());
        m_resources.VBDescription.EnableAttribArrays();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_resources.VBDescription.DisableAttribArrays();
        glBindBuffer(m_resources.VBDescription.GetTarget(), 0);
      }
    }

    if (m_config.GetForceFinishEachFrame())
    {
      // execute all in pipe before presentation - workaround for eglSwapBuffer not flushing pipe
      glFinish();
    }
  }

  void FurShellRendering::DrawMultipass(FurShaderMultiPass& rShader, MeshRender& rRender, const Matrix& world, const Matrix& view,
                                        const Matrix& perspective, const Vector3& displacement, const int layerCount)
  {
    ShaderBase::ScopedUse shaderScope(rShader);

    rShader.SetWorld(world);
    rShader.SetView(view);
    rShader.SetProjection(perspective);
    rShader.SetDisplacement(displacement);

    float layerAdd = (layerCount > 1 ? 1.0f / float(layerCount - 1) : 1);
    float layer = 0.0f;

    rRender.Bind(rShader);

    glDisable(GL_BLEND);

    rShader.SetCurrentLayer(0);
    rRender.Draw();
    layer += layerAdd;

    glEnable(GL_BLEND);

    for (int i = 1; i < layerCount; ++i)
    {
      rShader.SetCurrentLayer(layer);

      rRender.Draw();
      layer += layerAdd;
    }
    rRender.Unbind();
  }


  void FurShellRendering::DrawInstanced(FurShaderBase& rShader, MeshRender& rRender, const Matrix& world, const Matrix& view,
                                        const Matrix& perspective, const Vector3& displacement)
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
