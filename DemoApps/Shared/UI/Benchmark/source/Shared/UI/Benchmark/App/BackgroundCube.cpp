/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Basic/IBasicStaticBuffer.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <Shared/UI/Benchmark/App/BackgroundCube.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr float BackgroundAngleX = MathHelper::ToRadians(10);
      constexpr float BackgroundAngleY = MathHelper::ToRadians(10);
      constexpr float BackgroundAngleZ = MathHelper::ToRadians(-12);

      constexpr IO::PathView BackgroundTexture("Textures/GPUSdk/SquareLogo512x512.jpg");
    }

    // B D
    // |\|
    // A C
    // A = 1.0
    constexpr const float CubeDimensions = 100.0f;

    constexpr const float CubeCeiling = CubeDimensions;
    constexpr const float CubeFloor = -CubeDimensions;

    constexpr const float CubeLeft = -CubeDimensions;
    constexpr const float CubeRight = CubeDimensions;
    constexpr const float CubeBack = CubeDimensions;      // zBack
    constexpr const float CubeFront = -CubeDimensions;    // zFront
    constexpr const float CubeU0 = 0.0f;
    constexpr const float CubeU1 = 1.0f;
    constexpr const float CubeV0 = 1.0f;
    constexpr const float CubeV1 = 0.0f;
    constexpr const Color CubeColor = Colors::White();


    const std::array<VertexPositionColorTexture, 6 * 6> g_vertices = {
      // Floor
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), CubeColor, Vector2(CubeU1, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeFront), CubeColor, Vector2(CubeU0, CubeV1)),     // LF
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), CubeColor, Vector2(CubeU0, CubeV0)),    // RF

      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), CubeColor, Vector2(CubeU1, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), CubeColor, Vector2(CubeU0, CubeV0)),    // RF
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), CubeColor, Vector2(CubeU1, CubeV0)),     // RB

      // Ceiling
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU1, CubeV1)),     // LF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU0, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU1, CubeV0)),    // RF

      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU1, CubeV0)),    // RF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU0, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU0, CubeV0)),     // RB

      // Back wall
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU1, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), CubeColor, Vector2(CubeU1, CubeV1)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), CubeColor, Vector2(CubeU0, CubeV1)),

      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU1, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), CubeColor, Vector2(CubeU0, CubeV1)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU0, CubeV0)),

      // Front wall
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU0, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), CubeColor, Vector2(CubeU1, CubeV1)),
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeFront), CubeColor, Vector2(CubeU0, CubeV1)),

      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU0, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU1, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), CubeColor, Vector2(CubeU1, CubeV1)),

      //// Right wall
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), CubeColor, Vector2(CubeU1, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), CubeColor, Vector2(CubeU0, CubeV1)),      // FF
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU0, CubeV0)),    // CF

      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), CubeColor, Vector2(CubeU1, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU0, CubeV0)),    // CF
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU1, CubeV0)),     // CB

      // Left wall
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeFront), CubeColor, Vector2(CubeU1, CubeV1)),      // FF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), CubeColor, Vector2(CubeU0, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU1, CubeV0)),    // CF

      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), CubeColor, Vector2(CubeU1, CubeV0)),    // CF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), CubeColor, Vector2(CubeU0, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), CubeColor, Vector2(CubeU0, CubeV0)),     // CB
    };
  }


  BackgroundCube::BackgroundCube(const DemoWindowMetrics& windowMetrics, const ServiceProvider& serviceProvider)
    : m_render(serviceProvider.Get<IGraphicsService>()->GetBasicRenderSystem())
    , m_background(CreateBackgroundResources(*m_render, *serviceProvider.Get<IContentManager>()))
  {
    ConfigurationChanged(windowMetrics);
  }

  void BackgroundCube::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_background.MatrixModel = Matrix::CreateRotationX(LocalConfig::BackgroundAngleX) * Matrix::CreateRotationY(LocalConfig::BackgroundAngleY) *
                               Matrix::CreateRotationZ(LocalConfig::BackgroundAngleZ);
    m_background.MatrixProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(75.0f), windowMetrics.AspectRatio(), 1.0f, 1000.0f);
  }

  void BackgroundCube::Draw()
  {
    Draw(*m_render, m_background);
  }


  BackgroundCube::BackgroundRecord BackgroundCube::CreateBackgroundResources(IBasicRenderSystem& render, IContentManager& contentManager)
  {
    constexpr BitmapOrigin BitmapOrigin = BitmapOrigin::LowerLeft;
    Texture texture =
      contentManager.ReadTexture(LocalConfig::BackgroundTexture, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureLogo = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    auto vertexSpan = ReadOnlyFlexVertexSpanUtil::AsSpan(g_vertices);
    const BasicMaterialDepthInfo depthInfo(false, false, BasicCompareOp::Less);
    auto matCubeOpaque = render.CreateMaterial(BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise,
                                                                       depthInfo, vertexSpan.AsVertexDeclarationSpan()),
                                               textureLogo, true);

    auto meshVertices = render.CreateStaticBuffer(vertexSpan);


    return {std::move(matCubeOpaque), std::move(meshVertices), Matrix(), Matrix()};
  }

  void BackgroundCube::Draw(IBasicRenderSystem& render, const BackgroundRecord& background)
  {
    render.BeginCmds();
    {
      {    // Render the background cube
        const uint32_t vertexCount = background.MeshVertices->Capacity();

        render.CmdSetCamera(BasicCameraInfo(background.MatrixModel, Matrix::GetIdentity(), background.MatrixProj));
        render.CmdBindVertexBuffer(background.MeshVertices);
        render.CmdBindMaterial(background.MatCubeOpaque);
        render.CmdDraw(vertexCount, 0);
      }
    }
    render.EndCmds();
  }

  void BackgroundCube::SYS_SetRenderSystemViewport(const PxViewport& viewportPx)
  {
    assert(m_render);
    IBasicRenderSystem& rRenderSystem = *m_render;

    const BasicMaterialInfo materialInfo = rRenderSystem.GetMaterialInfo(m_background.MatCubeOpaque);

    // Enable custom viewport on the material
    BasicMaterialInfo newMI(materialInfo.Blend, materialInfo.CullMode, materialInfo.CullFrontFace, materialInfo.Depth,
                            BasicViewportMode::CustomOriginTopLeft, viewportPx);
    rRenderSystem.SetMaterialInfo(m_background.MatCubeOpaque, newMI);
  }

}
