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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#ifdef FSL_ENABLE_ASSIMP
#include <FslAssimp/SceneImporter.hpp>
#endif
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTangentTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslGraphics3D/BasicScene/MeshConverter.hpp>
#include <FslGraphics3D/SceneFormat/BasicSceneFormat.hpp>
#include <FslBase/IO/File.hpp>
#include <algorithm>
#include "LoadedScene.hpp"
// Because of inconsistency in khronos extension definition both the 31 and 2 headers are needed
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "OptionParser.hpp"

namespace Fsl
{
  using namespace GLES3;
  using namespace Graphics3D;


  using TestMesh = GenericMesh<VertexPositionNormalTangentTexture, uint16_t>;
  using TestScene = GenericScene<TestMesh>;

  namespace
  {
    const float DEFAULT_ZOOM = 350;
    const float DEFAULT_MODEL_SCALE = 5;

    std::shared_ptr<TestScene> LoadSceneBSF(const std::shared_ptr<IContentManager>& contentManager, const IO::Path& path)
    {
      SceneFormat::BasicSceneFormat loader;
      return loader.Load<TestScene>(path);
    }

    std::shared_ptr<TestScene> LoadSceneAssImp(const std::shared_ptr<IContentManager>& contentManager, const IO::Path& path){
#ifdef FSL_ENABLE_ASSIMP
      {SceneImporter sceneImporter;
    return sceneImporter.Load<TestScene>(IO::Path::Combine(path, "Scene.obj"), 200.0f, true, aiProcessPreset_TargetRealtime_MaxQuality);
    // return sceneImporter.Load<TestScene>(DetectSceneFileName(contentPath), aiProcessPreset_TargetRealtime_Quality);
  }
#else
      {throw NotSupportedException("Scene not supported on this platform");
  }
#endif
}

std::shared_ptr<TestScene> LoadScene(const std::shared_ptr<IContentManager>& contentManager, const IO::Path& path)
{
  auto contentPath = IO::Path::Combine(contentManager->GetContentPath(), path);
  auto preprocessedScene = IO::Path::Combine(contentPath, "Scene.fsf");
  if (IO::File::Exists(preprocessedScene))
  {
    return LoadSceneBSF(contentManager, preprocessedScene);
  }

  return LoadSceneAssImp(contentManager, contentPath);
}
}


LoadedScene::LoadedScene(const DemoAppConfig& config, const std::shared_ptr<OptionParser>& options, const int32_t id)
  : AScene(config, options)
{
  SetCullEnabled(false);
  m_material.Specular = options->GetMatSpecular();
  m_material.Shininess = options->GetMatShininess();
  m_material.Ambient = Vector3();

  m_tessellationConfig.TessLevelInner = 3.0f;
  m_tessellationConfig.TessLevelOuter = 3.0f;
  m_tessellationConfig.DisplacementFactor = options->GetDisplacementFactor();
  m_tessellationConfig.DisplacementMod = options->GetDisplacementMod();


  if (!GLUtil::HasExtension("GL_EXT_tessellation_shader"))
  {
    throw NotSupportedException("GL_EXT_tessellation_shader extension not supported");
  }
  if (!GLUtil::HasExtension("GL_EXT_geometry_shader"))
  {
    throw NotSupportedException("GL_EXT_geometry_shader extension not supported");
  }

  const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();


  IO::Path rootDir("Face");
  rootDir = IO::Path::Combine("Models", rootDir);


  FSLLOG("Loading scene...");
  std::shared_ptr<TestScene> scene = LoadScene(contentManager, rootDir);

  {    // Create the main texture (we use a scope here so we throw away the bitmap as soon as we don't need it)
    Bitmap bitmap;
    GLTextureParameters texParams(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

    FSLLOG("Loading color texture...");
    contentManager->Read(bitmap, IO::Path::Combine(rootDir, "TexColor.png"), PixelFormat::R8G8B8_UNORM);
    m_textureDiffuse.SetData(bitmap, texParams);

    FSLLOG("Loading normal texture...");
    contentManager->Read(bitmap, IO::Path::Combine(rootDir, "TexNormal.png"), PixelFormat::R8G8B8_UNORM);
    m_textureNormals.SetData(bitmap, texParams);

    FSLLOG("Loading displace texture...");
    contentManager->Read(bitmap, IO::Path::Combine(rootDir, "TexDisplace.png"), PixelFormat::R8G8B8_UNORM);
    m_textureDisplacement.SetData(bitmap, texParams);

    // Generate some 1pixel textures for specific colors
    bitmap.Reset(1, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
    bitmap.SetNativePixel(0, 0, 128);
    m_textureNoDisplacement.SetData(bitmap, texParams);
    bitmap.SetNativePixel(0, 0, 0xFFFFFFFF);
    m_textureDiffuseWhite.SetData(bitmap, texParams);
  }

  FSLLOG("Preparing meshes...");

  // Save the scene in bsf format
  // SceneFormat::BasicSceneFormat sceneFormat;
  // sceneFormat.Save("Scene.fsf", *scene);


  // Create index and vertex buffers for all the meshes.
  m_indexBuffers.Resize(scene->Meshes.size(), GL_UNSIGNED_SHORT);
  m_vertexBuffers.Resize(scene->Meshes.size(), TestMesh::vertex_type::GetVertexDeclaration());
  std::size_t vertexCount = 0;
  std::size_t indexCount = 0;
  for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
  {
    auto mesh = scene->Meshes[i];
    m_indexBuffers.Reset(i, mesh->GetIndexArray(), GL_STATIC_DRAW);
    m_vertexBuffers.Reset(i, mesh->GetVertexArray(), GL_STATIC_DRAW);

    vertexCount += mesh->GetVertexCount();
    indexCount += mesh->GetIndexCount();
    FSLLOG("Mesh #" << i << " vertex count: " << mesh->GetVertexCount() << ", Total index count : " << mesh->GetIndexCount());
  }
  FSLLOG("Total vertex count: " << vertexCount << ", Total index count : " << indexCount);

  GL_CHECK_FOR_ERROR();
}


LoadedScene::~LoadedScene() = default;
}
