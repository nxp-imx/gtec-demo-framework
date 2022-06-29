/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <Shared/ModelViewer/ModelSceneUtil.hpp>
#include <algorithm>
#include <array>

namespace Fsl::ModelSceneUtil
{
  namespace
  {
    IO::Path TryDiscoverTextureName(const IContentManager& contentManager, const IO::Path& baseName, const std::string& postfix)
    {
      constexpr std::array<const char*, 2> extensions = {"png", "jpg"};

      FSLLOG3_VERBOSE("Trying to discover texture name")
      for (const auto& entry : extensions)
      {
        IO::Path filename(fmt::format("{}{}.{}", baseName, postfix, entry));
        if (contentManager.Exists(filename))
        {
          return filename;
        }


        FSLLOG3_VERBOSE("- Not found: '{}'", filename)
      }
      return "";
    }

    void ApplyBasePath(IO::Path& rPath, const IO::Path& basePath)
    {
      if (rPath.IsEmpty())
      {
        return;
      }
      rPath = IO::Path::Combine(basePath, rPath);
    }
  }

  ModelLoaderConfig PrepareCustomModel(RenderConfig& rRenderConfig, const IContentManager& contentManager, const IO::Path& customModelPath)
  {
    if (IO::Path::IsPathRooted(customModelPath))
    {
      throw NotSupportedException(fmt::format("The custom model path '{}' can not be absolute", customModelPath));
    }
    if (!contentManager.Exists(customModelPath))
    {
      throw NotFoundException(fmt::format("Could not find the custom model '{}' in the content directory", customModelPath));
    }
    auto path = IO::Path::GetDirectoryName(customModelPath);
    auto filenameWithoutExtension = IO::Path::GetFileNameWithoutExtension(customModelPath);
    auto customName = IO::Path::Combine(path, filenameWithoutExtension);

    ModelLoaderConfig loaderConfig{};
    loaderConfig.ModelFileName = customModelPath;
    loaderConfig.TextureFileName = TryDiscoverTextureName(contentManager, customName, "_C");
    if (!loaderConfig.TextureFileName.IsEmpty())
    {
      // We require a diffuse texture to use any of the others
      loaderConfig.TextureSpecularFileName = TryDiscoverTextureName(contentManager, customName, "_S");
      loaderConfig.TextureGloss = TryDiscoverTextureName(contentManager, customName, "_G");
      loaderConfig.TextureNormalFileName = TryDiscoverTextureName(contentManager, customName, "_N");
    }
    else
    {
      loaderConfig.TextureFileName.Clear();
      loaderConfig.TextureSpecularFileName.Clear();
      loaderConfig.TextureGloss.Clear();
      loaderConfig.TextureNormalFileName.Clear();
    }

    rRenderConfig.RotationSpeed.X = 0;
    rRenderConfig.RotationSpeed.Z = 0;
    rRenderConfig.AllowBackfaceCull = false;
    return loaderConfig;
  }

  ModelLoaderConfig PrepareSceneModel(RenderConfig& rRenderConfig, Graphics3D::ArcballCamera& rCamera, const IContentManager& /*contentManager*/,
                                      const int32_t scene, const IO::Path& defaultScenePath)
  {
    ModelLoaderConfig loaderConfig{};
    switch (scene)
    {
    case 0:
      // Using this as the default since the fbx files seems to have problems under ubuntu
      loaderConfig.ModelFileName = "Knight2/armor.obj";
      loaderConfig.TextureFileName = "Knight2/armor_default_color.jpg";
      loaderConfig.TextureSpecularFileName = "Knight2/armor_default_metalness.jpg";
      loaderConfig.TextureGloss = "Knight2/armor_default_rough.jpg";
      loaderConfig.TextureNormalFileName = "Knight2/armor_default_nmap.jpg";
      rRenderConfig.RotationSpeed.X = 0;
      rRenderConfig.RotationSpeed.Z = 0;
      break;
    case 1:
      loaderConfig.ModelFileName = "Dragon/dragon.3ds";
      loaderConfig.TextureFileName = "Dragon/color.jpg";
      loaderConfig.TextureSpecularFileName = "Dragon/spec.jpg";
      loaderConfig.TextureNormalFileName = "Dragon/normal.jpg";
      loaderConfig.ScaleMod = 2.0f;
      rRenderConfig.Rotation.Y = MathHelper::ToRadians(170.0f);
      rRenderConfig.RotationSpeed.X = 0;
      rRenderConfig.RotationSpeed.Z = 0;
      rRenderConfig.AllowBackfaceCull = false;
      break;
    case 2:
      loaderConfig.ModelFileName = "FuturisticCar/Futuristic_Car.3ds";
      loaderConfig.TextureFileName = "FuturisticCar/Futuristic_Car_C.jpg";
      loaderConfig.TextureNormalFileName = "FuturisticCar/Futuristic_Car_N.jpg";
      // strTextureSpecularFileName = "FuturisticCar/Futuristic_Car_S.jpg";
      // m_rotation.X = MathHelper::ToRadians(-90.0f);
      rRenderConfig.RotationSpeed.X = 0;
      rRenderConfig.RotationSpeed.Z = 0;
      loaderConfig.ScaleMod = 1.75f;
      rCamera.SetRotation(Matrix::CreateRotationX(MathHelper::ToRadians(15)));
      break;
    case 3:
      loaderConfig.ModelFileName = "Torus1/Scene_flat.fbx";
      break;
    case 4:
      loaderConfig.ModelFileName = "Torus1/Scene_smooth.fbx";
      break;
    case 5:
      loaderConfig.ModelFileName = "Torus2/Scene_flat.fbx";
      break;
    case 6:
      loaderConfig.ModelFileName = "Torus2/Scene_smooth.fbx";
      break;
    case 7:
      loaderConfig.ModelFileName = "Nff/Cube.nff";
      break;
    case 8:
      loaderConfig.ModelFileName = "Nff/Sphere.nff";
      rRenderConfig.RotationSpeed.X = 0;
      rRenderConfig.RotationSpeed.Z = 0;
      break;
    case 9:
      loaderConfig.ModelFileName = "Nff/Tetrahedron.nff";
      break;
    default:
      throw NotSupportedException(fmt::format("Scene not supported {}", scene));
    }

    if (!defaultScenePath.IsEmpty())
    {
      ApplyBasePath(loaderConfig.ModelFileName, defaultScenePath);
      ApplyBasePath(loaderConfig.TextureFileName, defaultScenePath);
      ApplyBasePath(loaderConfig.TextureSpecularFileName, defaultScenePath);
      ApplyBasePath(loaderConfig.TextureGloss, defaultScenePath);
      ApplyBasePath(loaderConfig.TextureNormalFileName, defaultScenePath);
    }
    return loaderConfig;
  }
}
