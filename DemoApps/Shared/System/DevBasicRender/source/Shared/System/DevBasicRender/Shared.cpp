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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/PxSize1DF.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/IBasicDynamicBuffer.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
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

      constexpr const Color ColQuad = Colors::White();
      // constexpr const Color ColQuadB = Colors::Blue();
      // constexpr const Color ColQuadR = Colors::Red();

      constexpr const uint32_t LineVertices = 64;
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
    constexpr const float CubeU0 = 1.0f;
    constexpr const float CubeU1 = 0.0f;
    constexpr const float CubeV0 = 0.0f;
    constexpr const float CubeV1 = 1.0f;


    const std::array<VertexPositionColorTexture, 6 * 6> g_vertices = {
      // Floor
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), LocalConfig::Col0, Vector2(CubeU1, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeFront), LocalConfig::Col0, Vector2(CubeU0, CubeV1)),     // LF
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), LocalConfig::Col0, Vector2(CubeU0, CubeV0)),    // RF

      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), LocalConfig::Col0, Vector2(CubeU1, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), LocalConfig::Col0, Vector2(CubeU0, CubeV0)),    // RF
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), LocalConfig::Col0, Vector2(CubeU1, CubeV0)),     // RB

      // Ceiling
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), LocalConfig::Col1, Vector2(CubeU1, CubeV1)),     // LF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), LocalConfig::Col1, Vector2(CubeU0, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), LocalConfig::Col1, Vector2(CubeU1, CubeV0)),    // RF

      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), LocalConfig::Col1, Vector2(CubeU1, CubeV0)),    // RF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), LocalConfig::Col1, Vector2(CubeU0, CubeV1)),      // LB
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeBack), LocalConfig::Col1, Vector2(CubeU0, CubeV0)),     // RB

      // Back wall
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), LocalConfig::Col2, Vector2(CubeU1, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), LocalConfig::Col2, Vector2(CubeU1, CubeV1)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), LocalConfig::Col2, Vector2(CubeU0, CubeV1)),

      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), LocalConfig::Col2, Vector2(CubeU1, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), LocalConfig::Col2, Vector2(CubeU0, CubeV1)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeBack), LocalConfig::Col2, Vector2(CubeU0, CubeV0)),

      // Front wall
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), LocalConfig::Col3, Vector2(CubeU0, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), LocalConfig::Col3, Vector2(CubeU1, CubeV1)),
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeFront), LocalConfig::Col3, Vector2(CubeU0, CubeV1)),

      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), LocalConfig::Col3, Vector2(CubeU0, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), LocalConfig::Col3, Vector2(CubeU1, CubeV0)),
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), LocalConfig::Col3, Vector2(CubeU1, CubeV1)),

      //// Right wall
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), LocalConfig::Col4, Vector2(CubeU1, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeFront), LocalConfig::Col4, Vector2(CubeU0, CubeV1)),      // FF
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), LocalConfig::Col4, Vector2(CubeU0, CubeV0)),    // CF

      VertexPositionColorTexture(Vector3(CubeRight, CubeFloor, CubeBack), LocalConfig::Col4, Vector2(CubeU1, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeFront), LocalConfig::Col4, Vector2(CubeU0, CubeV0)),    // CF
      VertexPositionColorTexture(Vector3(CubeRight, CubeCeiling, CubeBack), LocalConfig::Col4, Vector2(CubeU1, CubeV0)),     // CB

      // Left wall
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeFront), LocalConfig::Col5, Vector2(CubeU1, CubeV1)),      // FF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), LocalConfig::Col5, Vector2(CubeU0, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), LocalConfig::Col5, Vector2(CubeU1, CubeV0)),    // CF

      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeFront), LocalConfig::Col5, Vector2(CubeU1, CubeV0)),    // CF
      VertexPositionColorTexture(Vector3(CubeLeft, CubeFloor, CubeBack), LocalConfig::Col5, Vector2(CubeU0, CubeV1)),       // FB
      VertexPositionColorTexture(Vector3(CubeLeft, CubeCeiling, CubeBack), LocalConfig::Col5, Vector2(CubeU0, CubeV0)),     // CB
    };

    constexpr const int32_t QuadSizE1 = 64;
    constexpr const float QuadSize = static_cast<float>(QuadSizE1);
    constexpr const float QuadLeft = 0;
    constexpr const float QuadRight = QuadSize;
    constexpr const float QuadTop = 0;
    constexpr const float QuadZ = 1;
    constexpr const float QuadBottom = QuadSize;
    constexpr const float QuadU0 = 0.0f;
    constexpr const float QuadU1 = 1.0f;
    constexpr const float QuadV0 = 1.0f;
    constexpr const float QuadV1 = 0.0f;

    //  L  R
    // T*-*
    //  |\|
    // B*-*

    // CW:   LB, LT, RB + LT, RT, RB
    // CCW:  LB, RB, LT + LT, RB, RT

    const std::array<VertexPositionColorTexture, 3 * 2> g_quad0Vertices = {
      VertexPositionColorTexture(Vector3(QuadLeft, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV1)),     // LB
      VertexPositionColorTexture(Vector3(QuadLeft, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV0)),        // LT
      VertexPositionColorTexture(Vector3(QuadRight, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV1)),    // RB

      VertexPositionColorTexture(Vector3(QuadLeft, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV0)),        // LT
      VertexPositionColorTexture(Vector3(QuadRight, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV0)),       // RT
      VertexPositionColorTexture(Vector3(QuadRight, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV1)),    // RB
    };

    const std::array<VertexPositionColorTexture, 4> g_quad1Vertices = {
      VertexPositionColorTexture(Vector3(QuadLeft, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV1)),     // LB
      VertexPositionColorTexture(Vector3(QuadLeft, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV0)),        // LT
      VertexPositionColorTexture(Vector3(QuadRight, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV1)),    // RB
      VertexPositionColorTexture(Vector3(QuadRight, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV0)),       // RT
    };

    const std::array<uint16_t, 6> g_quad1Indices = {0, 1, 2, 1, 3, 2};
  }

  Shared::Shared(const DemoAppConfig& config, const std::shared_ptr<IContentManager>& theContentManager)
    : m_render(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasicRenderSystem())
  {
    const auto& contentManager = *theContentManager;

    constexpr IO::PathView PathLogo("Textures/GPUSdk/SquareLogo512x512.jpg");
    constexpr IO::PathView PathOpaqueR("Textures/TestText/Opaque/R.png");
    constexpr IO::PathView PathOpaqueG("Textures/TestText/Opaque/G.png");
    constexpr IO::PathView PathOpaqueB("Textures/TestText/Opaque/B.png");
    constexpr IO::PathView PathPreAlpha1("Textures/TestText/Premultiplied/1.png");
    constexpr IO::PathView PathNonPreAlpha("Textures/TestText/Alpha/1.png");

    constexpr BitmapOrigin BitmapOrigin = BitmapOrigin::LowerLeft;

    Texture texture;
    contentManager.Read(texture, PathLogo, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureLogo = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    contentManager.Read(texture, PathOpaqueR, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueR = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, PathOpaqueG, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueG = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, PathOpaqueB, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueB = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    contentManager.Read(texture, PathPreAlpha1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureAlpha = m_render->CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, PathNonPreAlpha, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
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

    m_resources.MatLineListOpaque =
      m_render->CreateMaterial(BasicMaterialCreateInfo(BasicMaterialInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise,
                                                                         depthInfo, BasicPrimitiveTopology::LineList),
                                                       VertexPositionColor::AsVertexDeclarationSpan()),
                               textureAlpha);
    m_resources.MatLineStripOpaque =
      m_render->CreateMaterial(BasicMaterialCreateInfo(BasicMaterialInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise,
                                                                         depthInfo, BasicPrimitiveTopology::LineStrip),
                                                       VertexPositionColor::AsVertexDeclarationSpan()),
                               textureAlpha);

    m_resources.MeshVertices = m_render->CreateDynamicBuffer(vertexSpan);
    m_resources.Quad0MeshVertices = m_render->CreateDynamicBuffer(quadVertexSpan);

    m_resources.Quad1MeshVertices = m_render->CreateDynamicBuffer(ReadOnlyFlexVertexSpanUtil::AsSpan(g_quad1Vertices));
    m_resources.Quad1MeshIndices = m_render->CreateDynamicBuffer(SpanUtil::AsReadOnlySpan(g_quad1Indices));

    m_resources.LineVertices = m_render->CreateDynamicBuffer(VertexPositionColor::AsVertexDeclarationSpan(), LocalConfig::LineVertices);

    assert(m_resources.MeshVertices->Capacity() == vertexSpan.size());
    assert(m_resources.Quad0MeshVertices->Capacity() == quadVertexSpan.size());
    assert(m_resources.Quad1MeshVertices->Capacity() == g_quad1Vertices.size());
    assert(m_resources.Quad1MeshIndices->Capacity() == g_quad1Indices.size());

    // Setup the various elements that depend on the window metrics
    ConfigurationChanged(config.WindowMetrics);
  }


  void Shared::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_windowMetrics = windowMetrics;
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

    {    // Generate a some vertices intended for line rendering
      std::array<VertexPositionColor, LocalConfig::LineVertices> tmpVertices;

      const PxSize2D windowSizePx = m_windowMetrics.GetSizePx();
      const int32_t windowWidth = windowSizePx.RawWidth();
      const int32_t windowHeight = windowSizePx.RawHeight();
      const int32_t border = TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(windowWidth) * 0.1f);
      const int32_t areaSizeWidth = windowWidth - (2 * border);
      const int32_t areaHeight = windowHeight / 4;

      const float addXPxf = static_cast<float>(areaSizeWidth) / static_cast<float>(tmpVertices.size());

      const auto yOffset = static_cast<float>(areaHeight);
      const float yDist0 = static_cast<float>(areaHeight) * 0.5f;
      const float yDist1 = static_cast<float>(areaHeight) * 0.4f;

      Vector3 position(static_cast<float>(border), 0, 0);
      Color color = Colors::White();
      Vector2 lineStateY = m_lineAnimationState;
      for (std::size_t i = 0; i < tmpVertices.size(); ++i)
      {
        position.Y = yOffset + ((std::sin(lineStateY.X) * yDist0) + (std::sin(lineStateY.Y) * yDist1));
        tmpVertices[i] = VertexPositionColor(position, color);
        position.X += addXPxf;
        lineStateY += m_lineAnimationAdd;
      }

      // Animate
      m_lineAnimationState += m_lineAnimationSpeed * time.DeltaTime;
      m_lineAnimationState.X = MathHelper::WrapAngle(m_lineAnimationState.X);
      m_lineAnimationState.Y = MathHelper::WrapAngle(m_lineAnimationState.Y);

      m_resources.LineVertices->SetData(ReadOnlyFlexVertexSpanUtil::AsSpan(tmpVertices));
    }
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
      {    // Render a line list
        const uint32_t vertexCount = m_resources.LineVertices->Capacity();
        render.CmdSetCamera(BasicCameraInfo(m_matrices.TestLines.Orthographic0));
        render.CmdBindVertexBuffer(m_resources.LineVertices);
        render.CmdBindMaterial(m_resources.MatLineListOpaque);
        render.CmdDraw(vertexCount, 0);
      }
      {    // Render a line strip
        const uint32_t vertexCount = m_resources.LineVertices->Capacity();
        render.CmdSetCamera(BasicCameraInfo(m_matrices.TestLines.Orthographic1));
        // render.CmdBindVertexBuffer(m_resources.LineVertices);
        render.CmdBindMaterial(m_resources.MatLineStripOpaque);
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

      const auto screenWidth = static_cast<float>(sizePx.RawWidth());
      const auto screenHeight = static_cast<float>(sizePx.RawHeight());
      const float screenOffsetX = screenWidth / 2.0f;
      const float screenOffsetY = screenHeight / 2.0f;

      matrices.Orthographic0 = Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 1.0f) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic1 = Matrix::CreateTranslation(-screenOffsetX + ((QuadSize + 10) * 1), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic2 = Matrix::CreateTranslation(-screenOffsetX + ((QuadSize + 10) * 2), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic3 = Matrix::CreateTranslation(-screenOffsetX + ((QuadSize + 10) * 3), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      matrices.Orthographic4 = Matrix::CreateTranslation(-screenOffsetX + ((QuadSize + 10) * 4), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

      matrices.Orthographic5 = Matrix::CreateTranslation(-screenOffsetX + ((QuadSize + 10) * 5), -screenOffsetY, 1.0f) *
                               Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                               Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

      matrices.Test0 = CreateTestMatrices(metrics, 0u, 1u);
      matrices.Test1 = CreateTestMatrices(metrics, 1u, 1u);
      matrices.Test2 = CreateTestMatrices(metrics, 2u, 1u);
      matrices.Test3 = CreateTestMatrices(metrics, 0u, 2u);

      matrices.TestLines = CreateLinesTestMatrices(metrics);
    }
    return matrices;
  }


  Shared::DepthTest Shared::CreateTestMatrices(const DemoWindowMetrics& metrics, const uint16_t offsetX, const uint16_t offsetY)
  {
    // Setup the orthographic matrix for 2d rendering
    const PxSize2D sizePx = metrics.GetSizePx();

    const auto screenWidth = static_cast<float>(sizePx.RawWidth());
    const auto screenHeight = static_cast<float>(sizePx.RawHeight());
    const float screenOffsetX = screenWidth / 2.0f;
    const float screenOffsetY = screenHeight / 2.0f;

    const float xPos0 = -screenOffsetX + static_cast<float>(offsetX * QuadSizE1 * 2);
    const float xPos1 = -screenOffsetX + static_cast<float>(offsetX * QuadSizE1 * 2) + static_cast<float>(QuadSizE1 * 0.25f);
    const float xPos2 = -screenOffsetX + static_cast<float>(offsetX * QuadSizE1 * 2) + static_cast<float>(QuadSizE1 * 0.25f * 2.0f);

    const float yPos0 = -screenOffsetY + static_cast<float>(offsetY * QuadSizE1 * 2);
    const float yPos1 = -screenOffsetY + static_cast<float>(offsetY * QuadSizE1 * 2) + static_cast<float>(QuadSizE1 * 0.25f);
    const float yPos2 = -screenOffsetY + static_cast<float>(offsetY * QuadSizE1 * 2) + static_cast<float>(QuadSizE1 * 0.25f * 2.0f);

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


  Shared::LinesTest Shared::CreateLinesTestMatrices(const DemoWindowMetrics& metrics)
  {
    // Setup the orthographic matrix for 2d rendering
    const PxSize2D sizePx = metrics.GetSizePx();

    const auto screenWidth = static_cast<float>(sizePx.RawWidth());
    const auto screenHeight = static_cast<float>(sizePx.RawHeight());
    const float screenOffsetX = screenWidth / 2.0f;
    const float screenOffsetY = screenHeight / 2.0f;

    const float xPos = -screenOffsetX;

    const float yPos0 = -screenOffsetY;
    const float yPos1 = 0.0f;

    const float zPos = 5.0f;

    const Matrix orthographic0 = Matrix::CreateTranslation(xPos, yPos0, zPos) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                                 Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
    const Matrix orthographic1 = Matrix::CreateTranslation(xPos, yPos1, zPos) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                                 Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

    return {orthographic0, orthographic1};
  }
}
