#ifndef SHARED_MODELVIEWER_MODELSCENEUTIL_HPP
#define SHARED_MODELVIEWER_MODELSCENEUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics3D/Camera/ArcballCamera.hpp>

namespace Fsl
{
  namespace ModelSceneUtil
  {
    struct ModelLoaderConfig
    {
      IO::Path ModelFileName;
      IO::Path TextureFileName;
      IO::Path TextureSpecularFileName;
      IO::Path TextureGloss;
      IO::Path TextureNormalFileName;
      float ScaleMod{1.0f};
    };

    struct RenderConfig
    {
      Vector3 Rotation;
      Vector3 RotationSpeed;
      bool AllowBackfaceCull{false};

      constexpr RenderConfig() = default;

      constexpr RenderConfig(const Vector3& rotation, const Vector3& rotationSpeed, const bool allowBackfaceCull)
        : Rotation(rotation)
        , RotationSpeed(rotationSpeed)
        , AllowBackfaceCull(allowBackfaceCull)
      {
      }
    };


    ModelLoaderConfig PrepareCustomModel(RenderConfig& rRenderConfig, const IContentManager& contentManager, const IO::Path& customModelPath);
    ModelLoaderConfig PrepareSceneModel(RenderConfig& rRenderConfig, Graphics3D::ArcballCamera& rCamera, const IContentManager& contentManager,
                                        const int32_t scene, const IO::Path& defaultScenePath);
  }
}

#endif
