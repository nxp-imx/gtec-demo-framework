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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTangentTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslGraphics3D/BasicScene/MeshConverter.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/TextureUtil.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>
#include <FslGraphics3D/Procedural/BoxGenerator.hpp>
#include <FslGraphics3D/Procedural/TorusGenerator.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include "TestScene.hpp"
// Because of inconsistency in khronos extension definition both the 31 and 2 headers are needed
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "../OptionParser.hpp"
#include <cmath>


namespace Fsl
{
  using namespace GLES3;
  using namespace Graphics3D;
  using namespace Procedural;

  using ModelMesh = GenericMesh<VertexPositionNormalTangentTexture, uint16_t>;
  using ModelScene = GenericScene<ModelMesh>;


  TestScene::TestScene(const DemoAppConfig& config, const std::shared_ptr<OptionParser>& options, const int32_t id)
    : AScene(config, options)
  {
    SetCullEnabled(true);

    m_material.Ambient = Vector3();
    m_material.Specular = Vector3(1.0f, 1.0f, 1.0f);
    m_material.Shininess = 100.0f;
    m_tessellationConfig.TessLevelInner = 6.0f;
    m_tessellationConfig.TessLevelOuter = 6.0f;
    m_tessellationConfig.DisplacementFactor = 20.0f;
    // m_tessellationConfig.DisplacementMod = -0.5f;
    // m_tessellationConfig.UseRotation = true;
    const int32_t sceneId = id;
    if (sceneId == 2)
    {
      m_tessellationConfig.DisplacementFactor = 180.0f;
    }
    else if (sceneId == 3)
    {
      m_tessellationConfig.DisplacementFactor = 180.0f;
      // m_tessellationConfig.DisplacementMod = 0;// -0.345f;
      m_tessellationConfig.TessLevelInner = 60;
      m_tessellationConfig.TessLevelOuter = 60;
    }
    else if (sceneId == 4)
    {
      m_tessellationConfig.DisplacementFactor = 1000.0f;
      m_tessellationConfig.DisplacementMod = 0.0f;
    }
    if (sceneId == 1 || sceneId == 4)
    {
      m_rotationSpeed = Vector3(0.0f, -0.2f, 0);
    }
    else
    {
      m_rotationSpeed = Vector3(0.1f, 0.2f, 0.3f);
    }
    m_rotation = Vector3(0, 100, 0);

    // m_lightDirection = Vector3(0.0f, -0.75f, -1.0f);
    m_lightDirection = Vector3(1, 1, 1);
    m_lightColor = Vector3(0.8f, 0.8f, 0.8f);

    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();

    bool embedNormalMap = false;
    ConstructMesh(contentManager, sceneId);
    ConstructTextures(contentManager, embedNormalMap, sceneId);

    FSLLOG3_INFO("Ready");
    GL_CHECK_FOR_ERROR();
  }


  TestScene::~TestScene() = default;


  void TestScene::ConstructMesh(const std::shared_ptr<IContentManager>& /*contentManager*/, const int32_t sceneId)
  {
    ModelMesh mesh;
    {
      BasicMesh tmpMesh;
      WindingOrder::Enum windingOrder = WindingOrder::CCW;
      PxSize2D tex1Size(128, 128);
      TextureRectangle texRect(PxRectangle(0, 0, tex1Size.Width(), tex1Size.Height()), tex1Size);
      const NativeTextureArea texArea(TextureUtil::CalcTextureArea(texRect));
      const std::array<NativeTextureArea, 6> texAreas = {texArea, texArea, texArea, texArea, texArea, texArea};
      // tmpMesh = BoxGenerator::GenerateList(Vector3::Zero(), 150, 150, 150, texAreas, 6, windingOrder);
      switch (sceneId)
      {
      case 1:
      case 2:
      case 4:
        tmpMesh = SegmentedQuadGenerator::GenerateList(Vector3::Zero(), 800, 800, 40, 40, texArea, windingOrder);
        break;
      case 3:
        tmpMesh =
          BoxGenerator::GenerateList(Vector3::Zero(), 150, 150, 150, texAreas.data(), UncheckedNumericCast<int32_t>(texAreas.size()), windingOrder);
        break;
      case 0:
      default:
        tmpMesh = TorusGenerator::GenerateList(32, 32, 120, 30, texAreas[0], windingOrder);
        break;
      }

      MeshConverter::Convert(mesh, tmpMesh);
      auto* vertex = mesh.DirectAccessVertices();
      float swap = 0.0;
      for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i)
      {
        auto tmp = vertex[i];
        if (sceneId == 1 || sceneId == 4)
        {
          swap = tmp.Position.Y;
          tmp.Position.Y = tmp.Position.Z - 100;
          tmp.Position.Z = -swap;
          swap = tmp.Normal.Y;
          tmp.Normal.Y = tmp.Normal.Z;
          tmp.Normal.Z = -swap;
        }

        tmp.Tangent = Vector3();
        vertex[i] = tmp;
      }
      {
        auto* indices = mesh.DirectAccessIndices();
        for (uint32_t i = 0; i < mesh.GetIndexCount(); i += 3)
        {
          const auto i0 = indices[i + 0];
          const auto i1 = indices[i + 1];
          const auto i2 = indices[i + 2];
          auto e1p = vertex[i1].Position - vertex[i0].Position;
          auto e1t = vertex[i1].TextureCoordinate - vertex[i0].TextureCoordinate;
          auto e2t = vertex[i2].TextureCoordinate - vertex[i0].TextureCoordinate;
          const float urate = std::abs(e1t.X) > std::abs(e2t.X) ? e1t.X : e2t.X;
          auto tangent = Vector3::Normalize(Vector3(e1p.X / urate, e1p.Y / urate, e1p.Z / urate));

          vertex[i0].Tangent += tangent;
          vertex[i1].Tangent += tangent;
          vertex[i2].Tangent += tangent;
        }
        for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i)
        {
          vertex[i].Tangent = Vector3::Normalize(vertex[i].Tangent);
        }
      }
    }

    m_indexBuffers.Resize(1, GL_UNSIGNED_SHORT);
    m_vertexBuffers.Resize(1, vertex_type::GetVertexDeclaration());
    m_vertexBuffers.Reset(0, mesh.GetVertexArray(), GL_STATIC_DRAW);
    m_indexBuffers.Reset(0, mesh.GetIndexArray().data(), mesh.GetIndexCount(), GL_STATIC_DRAW);
  }


  void TestScene::ConstructTextures(const std::shared_ptr<IContentManager>& contentManager, const bool embedNormalMap, const int32_t sceneId)
  {
    // Create the main texture (we use a scope here so we throw away the bitmap as soon as we don't need it)
    Bitmap bitmap;
    contentManager->Read(bitmap, "Textures/Rocks/TexColor.png", PixelFormat::R8G8B8_UNORM);
    GLTextureParameters texParams1(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    m_textureDiffuse.SetData(bitmap, texParams1);

    IO::Path strTexDisplace;
    IO::Path strTexNormal;
    bool isFloatTexture = false;

    switch (sceneId)
    {
    case 2:
    case 3:
      strTexDisplace = "Textures/Circles/TexDisplace.png";
      strTexNormal = "Textures/Circles/TexNormal.png";
      break;
    case 4:
      strTexDisplace = "Models/Face/TexDisplace.png";
      strTexNormal = "Modes/Face/TexNormal.png";
      isFloatTexture = true;
      break;
    case 0:
    case 1:
    default:
      strTexDisplace = "Textures/Rocks/TexDisplace.png";
      strTexNormal = "Textures/Rocks/TexNormal.png";
      break;
    }


    Bitmap bitmapNormals;
    contentManager->Read(bitmapNormals, strTexNormal, PixelFormat::R8G8B8_UNORM);
    m_textureNormals.SetData(bitmapNormals, texParams1);

    if (embedNormalMap)
    {
      contentManager->Read(bitmap, strTexDisplace, PixelFormat::R8G8B8A8_UNORM);
      // Encode normal as xyz and displacement as z
      for (uint32_t y = 0; y < bitmap.Height(); ++y)
      {
        for (uint32_t x = 0; x < bitmap.Width(); ++x)
        {
          uint32_t pixelNormal = bitmapNormals.GetNativePixel(x, y);
          uint32_t pixel = bitmap.GetNativePixel(x, y);
          pixel = ((pixel << 8) & 0xFF000000) | (pixelNormal & 0xFFFFFF);
          bitmap.SetNativePixel(x, y, pixel);
        }
      }
      m_textureDisplacement.SetData(bitmap, texParams1);
    }
    else
    {
      contentManager->Read(bitmap, strTexDisplace, !isFloatTexture ? PixelFormat::R8G8B8_UNORM : PixelFormat::R32G32B32A32_SFLOAT);
      m_textureDisplacement.SetData(bitmap, texParams1);
    }


    bitmap.Reset(1, 1, PixelFormat::R8G8B8_UNORM, BitmapOrigin::LowerLeft);
    bitmap.SetNativePixel(0, 0, 0x808080);
    m_textureNoDisplacement.SetData(bitmap, texParams1);
    bitmap.SetNativePixel(0, 0, 0xFFFFFFFF);
    m_textureDiffuseWhite.SetData(bitmap, texParams1);
    GL_CHECK_FOR_ERROR();
  }
}
