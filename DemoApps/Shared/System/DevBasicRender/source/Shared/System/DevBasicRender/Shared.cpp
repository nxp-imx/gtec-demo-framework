/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/IBasicDynamicBuffer.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <Shared/System/DevBasicRender/Shared.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const Color Col0 = Color(0x7FFF0000);
      constexpr const Color Col1 = Color(0x7F00FF00);
      constexpr const Color Col2 = Color(0x7F0000FF);
      constexpr const Color Col3 = Color(0x7FFFFFFF);
      constexpr const Color Col4 = Color(0x7FFFFFFF);
      constexpr const Color Col5 = Color(0x7FFFFFFF);

      constexpr const Color ColQuad = Color::White();
      // constexpr const Color ColQuadB = Color::Blue();
      // constexpr const Color ColQuadR = Color::Red();
    }

    // B D
    // |\|
    // A C
    // A = 1.0
    constexpr const float CUBE_DIMENSIONS = 100.0f;

    constexpr const float CUBE_CEILING = CUBE_DIMENSIONS;
    constexpr const float CUBE_FLOOR = -CUBE_DIMENSIONS;

    constexpr const float CUBE_LEFT = -CUBE_DIMENSIONS;
    constexpr const float CUBE_RIGHT = CUBE_DIMENSIONS;
    constexpr const float CUBE_BACK = CUBE_DIMENSIONS;      // zBack
    constexpr const float CUBE_FRONT = -CUBE_DIMENSIONS;    // zFront
    constexpr const float CUBE_U0 = 1.0f;
    constexpr const float CUBE_U1 = 0.0f;
    constexpr const float CUBE_V0 = 0.0f;
    constexpr const float CUBE_V1 = 1.0f;


    const std::array<VertexPositionColorTexture, 6 * 6> g_vertices = {
      // Floor
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col0, Vector2(CUBE_U1, CUBE_V1)),      // LB
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col0, Vector2(CUBE_U0, CUBE_V1)),     // LF
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col0, Vector2(CUBE_U0, CUBE_V0)),    // RF

      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col0, Vector2(CUBE_U1, CUBE_V1)),      // LB
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col0, Vector2(CUBE_U0, CUBE_V0)),    // RF
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col0, Vector2(CUBE_U1, CUBE_V0)),     // RB

      // Ceiling
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col1, Vector2(CUBE_U1, CUBE_V1)),     // LF
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col1, Vector2(CUBE_U0, CUBE_V1)),      // LB
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col1, Vector2(CUBE_U1, CUBE_V0)),    // RF

      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col1, Vector2(CUBE_U1, CUBE_V0)),    // RF
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col1, Vector2(CUBE_U0, CUBE_V1)),      // LB
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col1, Vector2(CUBE_U0, CUBE_V0)),     // RB

      // Back wall
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col2, Vector2(CUBE_U1, CUBE_V0)),
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col2, Vector2(CUBE_U1, CUBE_V1)),
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col2, Vector2(CUBE_U0, CUBE_V1)),

      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col2, Vector2(CUBE_U1, CUBE_V0)),
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col2, Vector2(CUBE_U0, CUBE_V1)),
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col2, Vector2(CUBE_U0, CUBE_V0)),

      // Front wall
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col3, Vector2(CUBE_U0, CUBE_V0)),
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col3, Vector2(CUBE_U1, CUBE_V1)),
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col3, Vector2(CUBE_U0, CUBE_V1)),

      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col3, Vector2(CUBE_U0, CUBE_V0)),
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col3, Vector2(CUBE_U1, CUBE_V0)),
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col3, Vector2(CUBE_U1, CUBE_V1)),

      //// Right wall
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col4, Vector2(CUBE_U1, CUBE_V1)),       // FB
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col4, Vector2(CUBE_U0, CUBE_V1)),      // FF
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col4, Vector2(CUBE_U0, CUBE_V0)),    // CF

      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col4, Vector2(CUBE_U1, CUBE_V1)),       // FB
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col4, Vector2(CUBE_U0, CUBE_V0)),    // CF
      VertexPositionColorTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col4, Vector2(CUBE_U1, CUBE_V0)),     // CB

      // Left wall
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), LocalConfig::Col5, Vector2(CUBE_U1, CUBE_V1)),      // FF
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col5, Vector2(CUBE_U0, CUBE_V1)),       // FB
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col5, Vector2(CUBE_U1, CUBE_V0)),    // CF

      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), LocalConfig::Col5, Vector2(CUBE_U1, CUBE_V0)),    // CF
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), LocalConfig::Col5, Vector2(CUBE_U0, CUBE_V1)),       // FB
      VertexPositionColorTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), LocalConfig::Col5, Vector2(CUBE_U0, CUBE_V0)),     // CB
    };

    constexpr const int32_t QUAD_SIZE1 = 64;
    constexpr const float QUAD_SIZE = static_cast<float>(QUAD_SIZE1);
    constexpr const float QUAD_LEFT = 0;
    constexpr const float QUAD_RIGHT = QUAD_SIZE;
    constexpr const float QUAD_TOP = 0;
    constexpr const float QUAD_Z = 1;
    constexpr const float QUAD_BOTTOM = QUAD_SIZE;
    constexpr const float QUAD_U0 = 0.0f;
    constexpr const float QUAD_U1 = 1.0f;
    constexpr const float QUAD_V0 = 1.0f;
    constexpr const float QUAD_V1 = 0.0f;

    //  L  R
    // T*-*
    //  |\|
    // B*-*

    // CW:   LB, LT, RB + LT, RT, RB
    // CCW:  LB, RB, LT + LT, RB, RT

    const std::array<VertexPositionColorTexture, 3 * 2> g_quad0Vertices = {
      VertexPositionColorTexture(Vector3(QUAD_LEFT, QUAD_BOTTOM, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U0, QUAD_V1)),     // LB
      VertexPositionColorTexture(Vector3(QUAD_LEFT, QUAD_TOP, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U0, QUAD_V0)),        // LT
      VertexPositionColorTexture(Vector3(QUAD_RIGHT, QUAD_BOTTOM, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U1, QUAD_V1)),    // RB

      VertexPositionColorTexture(Vector3(QUAD_LEFT, QUAD_TOP, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U0, QUAD_V0)),        // LT
      VertexPositionColorTexture(Vector3(QUAD_RIGHT, QUAD_TOP, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U1, QUAD_V0)),       // RT
      VertexPositionColorTexture(Vector3(QUAD_RIGHT, QUAD_BOTTOM, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U1, QUAD_V1)),    // RB
    };

    const std::array<VertexPositionColorTexture, 4> g_quad1Vertices = {
      VertexPositionColorTexture(Vector3(QUAD_LEFT, QUAD_BOTTOM, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U0, QUAD_V1)),     // LB
      VertexPositionColorTexture(Vector3(QUAD_LEFT, QUAD_TOP, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U0, QUAD_V0)),        // LT
      VertexPositionColorTexture(Vector3(QUAD_RIGHT, QUAD_BOTTOM, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U1, QUAD_V1)),    // RB
      VertexPositionColorTexture(Vector3(QUAD_RIGHT, QUAD_TOP, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U1, QUAD_V0)),       // RT
    };

    const std::array<uint16_t, 6> g_quad1Indices = {0, 1, 2, 1, 3, 2};
  }

  Shared::Shared(const DemoAppConfig& config, const std::shared_ptr<IContentManager>& theContentManager)
    : m_render(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasicRenderSystem())
  {
    const auto& contentManager = *theContentManager;

    constexpr IO::PathView pathLogo("Textures/GPUSdk/SquareLogo512x512.jpg");
    constexpr IO::PathView pathOpaqueR("Textures/TestText/Opaque/R.png");
    constexpr IO::PathView pathOpaqueG("Textures/TestText/Opaque/G.png");
    constexpr IO::PathView pathOpaqueB("Textures/TestText/Opaque/B.png");
    constexpr IO::PathView pathPreAlpha1("Textures/TestText/Premultiplied/1.png");
    constexpr IO::PathView pathNonPreAlpha("Textures/TestText/Alpha/1.png");

    constexpr BitmapOrigin bitmapOrigin = BitmapOrigin::LowerLeft;

    Texture texture;
    contentManager.Read(texture, pathLogo, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureLogo = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    contentManager.Read(texture, pathOpaqueR, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueR = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, pathOpaqueG, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueG = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, pathOpaqueB, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueB = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    contentManager.Read(texture, pathPreAlpha1, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureAlpha = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, pathNonPreAlpha, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureNonPreAlpha = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    auto vertexSpan = ReadOnlyFlexVertexSpanUtil::AsSpan(g_vertices);
    auto quadVertexSpan = ReadOnlyFlexVertexSpanUtil::AsSpan(g_quad0Vertices);

    const BasicMaterialDepthInfo depthInfo(false, false, BasicCompareOp::Less);
    const BasicMaterialDepthInfo depthInfoR(true, false, BasicCompareOp::Less);
    const BasicMaterialDepthInfo depthInfoRW(true, true, BasicCompareOp::Less);
    const BasicMaterialDepthInfo depthInfoRGreater(true, false, BasicCompareOp::Greater);

    m_resources.MatCubeOpaque =
      m_render->CreateMaterial(BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo,
                                                       vertexSpan.AsVertexDeclarationSpan()),
                               textureLogo);

    const auto quadVertexDeclaration = quadVertexSpan.AsVertexDeclarationSpan();

    m_resources.MatOpaque = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo, quadVertexDeclaration),
      textureAlpha);

    m_resources.MatOpaqueRDepthDisabled = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo, quadVertexDeclaration),
      textureOpaqueR);
    m_resources.MatOpaqueGDepthDisabled = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo, quadVertexDeclaration),
      textureOpaqueG);
    m_resources.MatOpaqueBDepthDisabled = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo, quadVertexDeclaration),
      textureOpaqueB);

    m_resources.MatOpaqueRDepthR = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoR, quadVertexDeclaration),
      textureOpaqueR);
    m_resources.MatOpaqueGDepthR = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoR, quadVertexDeclaration),
      textureOpaqueG);
    m_resources.MatOpaqueBDepthR = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoR, quadVertexDeclaration),
      textureOpaqueB);

    m_resources.MatOpaqueRDepthRW = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoRW, quadVertexDeclaration),
      textureOpaqueR);
    m_resources.MatOpaqueGDepthRW = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoRW, quadVertexDeclaration),
      textureOpaqueG);
    m_resources.MatOpaqueBDepthRW = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoRW, quadVertexDeclaration),
      textureOpaqueB);

    m_resources.MatOpaqueRDepthRGreater = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoRGreater, quadVertexDeclaration),
      textureOpaqueR);
    m_resources.MatOpaqueGDepthRGreater = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoRGreater, quadVertexDeclaration),
      textureOpaqueG);
    m_resources.MatOpaqueBDepthRGreater = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfoRGreater, quadVertexDeclaration),
      textureOpaqueB);


    m_resources.MatAlpha = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::AlphaBlend, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo, quadVertexDeclaration),
      textureAlpha);
    m_resources.MatNonPreAlpha = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::NonPremultiplied, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo, quadVertexDeclaration),
      textureNonPreAlpha);
    m_resources.MatSdf = m_render->CreateMaterial(
      BasicMaterialCreateInfo(BlendState::Sdf, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo, quadVertexDeclaration),
      textureNonPreAlpha);

    m_resources.MeshVertices = m_render->CreateDynamicBuffer(vertexSpan);
    m_resources.Quad0MeshVertices = m_render->CreateDynamicBuffer(quadVertexSpan);

    m_resources.Quad1MeshVertices = m_render->CreateDynamicBuffer(ReadOnlyFlexVertexSpanUtil::AsSpan(g_quad1Vertices));
    m_resources.Quad1MeshIndices = m_render->CreateDynamicBuffer(ReadOnlySpanUtil::AsSpan(g_quad1Indices));

    assert(m_resources.MeshVertices->Capacity() == vertexSpan.size());
    assert(m_resources.Quad0MeshVertices->Capacity() == quadVertexSpan.size());
    assert(m_resources.Quad1MeshVertices->Capacity() == g_quad1Vertices.size());
    assert(m_resources.Quad1MeshIndices->Capacity() == g_quad1Indices.size());

    // Setup the various elements that depend on the window metrics
    ConfigurationChanged(config.WindowMetrics);
  }


  void Shared::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_matrices = CalcMatrices(windowMetrics);
  }

  void Shared::Update(const DemoTime& time)
  {
    m_angle.X -= 0.60f * time.DeltaTime;
    m_angle.Y -= 0.50f * time.DeltaTime;
    m_angle.Z -= 0.40f * time.DeltaTime;
    m_angle.X = MathHelper::WrapAngle(m_angle.X);
    m_angle.Y = MathHelper::WrapAngle(m_angle.Y);
    m_angle.Z = MathHelper::WrapAngle(m_angle.Z);

    // Rotate and translate the model view matrix
    m_matrices.Model =
      Matrix::CreateRotationX(m_angle.X) * Matrix::CreateRotationY(m_angle.Y) * Matrix::CreateRotationZ(m_angle.Z) * m_matrices.Translate;
  }


  void Shared::Draw()
  {
    // These material changes are only applied at bind time!
    // m_resources.MatCubeOpaque.SetModelViewProjection(m_matrices.Model * m_matrices.Proj);

    assert(m_render);
    IBasicRenderSystem& render = *m_render;

    render.BeginCmds();
    {
      {    // Render the background cube
        const uint32_t vertexCount = m_resources.MeshVertices->Capacity();

        render.CmdSetCamera(BasicCameraInfo(m_matrices.Model, Matrix::GetIdentity(), m_matrices.Proj));
        render.CmdBindVertexBuffer(m_resources.MeshVertices);
        render.CmdBindMaterial(m_resources.MatCubeOpaque);
        render.CmdDraw(vertexCount, 0);
      }
      {    // Render the overlay 2d image
        const uint32_t vertexCount = m_resources.Quad0MeshVertices->Capacity();


        render.CmdBindVertexBuffer(m_resources.Quad0MeshVertices);

        render.CmdBindMaterial(m_resources.MatOpaque);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic0));
        render.CmdDraw(vertexCount, 0);

        render.CmdBindMaterial(m_resources.MatAlpha);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic1));
        render.CmdDraw(vertexCount, 0);

        render.CmdBindMaterial(m_resources.MatNonPreAlpha);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic2));
        render.CmdDraw(vertexCount, 0);

        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic3));
        render.CmdDraw(vertexCount / 2, 0);
        render.CmdDraw(vertexCount / 2, vertexCount / 2);

        render.CmdBindMaterial(m_resources.MatSdf);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic4));
        render.CmdDraw(vertexCount, 0);

        render.CmdBindMaterial(m_resources.MatOpaque);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic5));
        render.CmdBindVertexBuffer(m_resources.Quad1MeshVertices);
        render.CmdBindIndexBuffer(m_resources.Quad1MeshIndices);
        render.CmdDrawIndexed(m_resources.Quad1MeshIndices->Capacity(), 0);
      }

      render.CmdBindVertexBuffer(m_resources.Quad0MeshVertices);
      {    // Disabled
        const uint32_t vertexCount = m_resources.Quad0MeshVertices->Capacity();

        render.CmdBindMaterial(m_resources.MatOpaqueRDepthDisabled);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test0.Orthographic0));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueGDepthDisabled);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test0.Orthographic1));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueBDepthDisabled);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test0.Orthographic2));
        render.CmdDraw(vertexCount, 0);
      }
      {
        const uint32_t vertexCount = m_resources.Quad0MeshVertices->Capacity();

        render.CmdBindMaterial(m_resources.MatOpaqueRDepthRW);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test1.Orthographic0));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueGDepthRW);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test1.Orthographic1));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueBDepthRW);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test1.Orthographic2));
        render.CmdDraw(vertexCount, 0);
      }
      {
        const uint32_t vertexCount = m_resources.Quad0MeshVertices->Capacity();

        render.CmdBindMaterial(m_resources.MatOpaqueRDepthRW);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test2.Orthographic0));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueGDepthR);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test2.Orthographic1));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueBDepthR);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test2.Orthographic2));
        render.CmdDraw(vertexCount, 0);
      }
      {
        const uint32_t vertexCount = m_resources.Quad0MeshVertices->Capacity();

        render.CmdBindMaterial(m_resources.MatOpaqueRDepthRW);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test3.Orthographic0));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueGDepthRGreater);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test3.Orthographic1));
        render.CmdDraw(vertexCount, 0);
        render.CmdBindMaterial(m_resources.MatOpaqueBDepthRGreater);
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Test3.Orthographic2));
        render.CmdDraw(vertexCount, 0);
      }
    }
    render.EndCmds();
  }

  Shared::Matrices Shared::CalcMatrices(const DemoWindowMetrics& metrics)
  {
    Matrices matrices;
    {
      // Setup the big background cube matrix
      matrices.Proj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(75.0f), metrics.AspectRatio(), 1.0f, 1000.0f);
      matrices.Translate = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
    }
    {    // Setup the orthographic matrix for 2d rendering
      const PxSize2D sizePx = metrics.GetSizePx();

      const auto screenWidth = static_cast<float>(sizePx.Width());
      const auto screenHeight = static_cast<float>(sizePx.Height());
      const float screenOffsetX = screenWidth / 2.0f;
      const float screenOffsetY = screenHeight / 2.0f;

      matrices.Orthographic0 = Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 1.0f) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic1 = Matrix::CreateTranslation(-screenOffsetX + ((QUAD_SIZE + 10) * 1), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic2 = Matrix::CreateTranslation(-screenOffsetX + ((QUAD_SIZE + 10) * 2), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic3 = Matrix::CreateTranslation(-screenOffsetX + ((QUAD_SIZE + 10) * 3), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic4 = Matrix::CreateTranslation(-screenOffsetX + ((QUAD_SIZE + 10) * 4), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

      matrices.Orthographic5 = Matrix::CreateTranslation(-screenOffsetX + ((QUAD_SIZE + 10) * 5), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

      matrices.Test0 = CreateTestMatrices(metrics, 0u, 1u);
      matrices.Test1 = CreateTestMatrices(metrics, 1u, 1u);
      matrices.Test2 = CreateTestMatrices(metrics, 2u, 1u);
      matrices.Test3 = CreateTestMatrices(metrics, 0u, 2u);
    }
    return matrices;
  }


  Shared::DepthTest Shared::CreateTestMatrices(const DemoWindowMetrics& metrics, const uint16_t offsetX, const uint16_t offsetY)
  {
    // Setup the orthographic matrix for 2d rendering
    const PxSize2D sizePx = metrics.GetSizePx();

    const auto screenWidth = static_cast<float>(sizePx.Width());
    const auto screenHeight = static_cast<float>(sizePx.Height());
    const float screenOffsetX = screenWidth / 2.0f;
    const float screenOffsetY = screenHeight / 2.0f;

    const float xPos0 = -screenOffsetX + static_cast<float>(offsetX * QUAD_SIZE1 * 2);
    const float xPos1 = -screenOffsetX + static_cast<float>(offsetX * QUAD_SIZE1 * 2) + static_cast<float>(QUAD_SIZE1 * 0.25f);
    const float xPos2 = -screenOffsetX + static_cast<float>(offsetX * QUAD_SIZE1 * 2) + static_cast<float>(QUAD_SIZE1 * 0.25f * 2.0f);

    const float yPos0 = -screenOffsetY + static_cast<float>(offsetY * QUAD_SIZE1 * 2);
    const float yPos1 = -screenOffsetY + static_cast<float>(offsetY * QUAD_SIZE1 * 2) + static_cast<float>(QUAD_SIZE1 * 0.25f);
    const float yPos2 = -screenOffsetY + static_cast<float>(offsetY * QUAD_SIZE1 * 2) + static_cast<float>(QUAD_SIZE1 * 0.25f * 2.0f);

    const float zPos0 = 7.0f;
    const float zPos1 = 8.0f;
    const float zPos2 = 9.0f;

    const Matrix orthographic0 = Matrix::CreateTranslation(xPos0, yPos0, zPos0) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                                 Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
    const Matrix orthographic1 = Matrix::CreateTranslation(xPos1, yPos1, zPos1) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                                 Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
    const Matrix orthographic2 = Matrix::CreateTranslation(xPos2, yPos2, zPos2) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                                 Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

    return {orthographic0, orthographic1, orthographic2};
  }


}
