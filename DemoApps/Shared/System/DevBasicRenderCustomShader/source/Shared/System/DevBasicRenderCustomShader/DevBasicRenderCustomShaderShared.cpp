/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/IBasicDynamicBuffer.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Shader/BasicShaderCreateInfo.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptionArray.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <Shared/System/DevBasicRenderCustomShader/DevBasicRenderCustomShaderShared.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      // constexpr Color Col0 = Color(0x7FFF0000);
      // constexpr Color Col1 = Color(0x7F00FF00);
      // constexpr Color Col2 = Color(0x7F0000FF);
      // constexpr Color Col3 = Color(0x7FFFFFFF);
      // constexpr Color Col4 = Color(0x7FFFFFFF);
      // constexpr Color Col5 = Color(0x7FFFFFFF);

      constexpr Color ColQuad = Colors::White();
      // constexpr const Color ColQuadB = Colors::Blue();
      // constexpr const Color ColQuadR = Colors::Red();
    }


    constexpr int32_t QuadSizE1 = 64;
    constexpr float QuadSize = static_cast<float>(QuadSizE1);
    constexpr float QuadLeft = 0;
    constexpr float QuadRight = QuadSize;
    constexpr float QuadTop = 0;
    constexpr float QuadZ = 1;
    constexpr float QuadBottom = QuadSize;
    constexpr float QuadU0 = 0.0f;
    constexpr float QuadU1 = 1.0f;
    constexpr float QuadV0 = 1.0f;
    constexpr float QuadV1 = 0.0f;

    //  L  R
    // T*-*
    //  |\|
    // B*-*

    // CW:   LB, LT, RB + LT, RT, RB
    // CCW:  LB, RB, LT + LT, RB, RT

    constexpr std::array<VertexPositionColorTexture, 3 * 2> GQuad0Vertices = {
      VertexPositionColorTexture(Vector3(QuadLeft, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV1)),     // LB
      VertexPositionColorTexture(Vector3(QuadLeft, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV0)),        // LT
      VertexPositionColorTexture(Vector3(QuadRight, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV1)),    // RB

      VertexPositionColorTexture(Vector3(QuadLeft, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU0, QuadV0)),        // LT
      VertexPositionColorTexture(Vector3(QuadRight, QuadTop, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV0)),       // RT
      VertexPositionColorTexture(Vector3(QuadRight, QuadBottom, QuadZ), LocalConfig::ColQuad, Vector2(QuadU1, QuadV1)),    // RB
    };

    // constexpr std::array<VertexPositionColorTexture, 4> g_quad1Vertices = {
    // VertexPositionColorTexture(Vector3(QUAD_LEFT, QUAD_BOTTOM, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U0, QUAD_V1)),     // LB
    // VertexPositionColorTexture(Vector3(QUAD_LEFT, QUAD_TOP, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U0, QUAD_V0)),        // LT
    // VertexPositionColorTexture(Vector3(QUAD_RIGHT, QUAD_BOTTOM, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U1, QUAD_V1)),    // RB
    // VertexPositionColorTexture(Vector3(QUAD_RIGHT, QUAD_TOP, QUAD_Z), LocalConfig::ColQuad, Vector2(QUAD_U1, QUAD_V0)),       // RT
    // };

    // constexpr std::array<uint16_t, 6> g_quad1Indices = {0, 1, 2, 1, 3, 2};
  }

  DevBasicRenderCustomShaderShared::DevBasicRenderCustomShaderShared(const DemoAppConfig& config, const DevCustomCreateInfo& createInfo)
    : m_render(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasicRenderSystem())
  {
    const auto& contentManager = *createInfo.ContentManager;

    auto quadVertexSpan = ReadOnlyFlexVertexSpanUtil::AsSpan(GQuad0Vertices);
    m_resources.QuadMeshVertices = m_render->CreateDynamicBuffer(quadVertexSpan);

    auto shadersRecord = CreateCustomShaders(*m_render, createInfo);

    const auto quadVertexDeclaration = quadVertexSpan.AsVertexDeclarationSpan();
    m_resources.Materials = CreateMaterials(contentManager, *m_render, shadersRecord, quadVertexDeclaration);


    assert(m_resources.QuadMeshVertices->Capacity() == quadVertexSpan.size());

    // Setup the various elements that depend on the window metrics
    ConfigurationChanged(config.WindowMetrics);
  }


  void DevBasicRenderCustomShaderShared::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_matrices = CalcMatrices(windowMetrics);
  }

  void DevBasicRenderCustomShaderShared::Update(const DemoTime& time)
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


  void DevBasicRenderCustomShaderShared::Draw()
  {
    // These material changes are only applied at bind time!
    // m_resources.MatCubeOpaque.SetModelViewProjection(m_matrices.Model * m_matrices.Proj);

    assert(m_render);
    IBasicRenderSystem& render = *m_render;

    const uint32_t vertexCount = m_resources.QuadMeshVertices->Capacity();

    render.BeginCmds();
    {
      render.CmdBindVertexBuffer(m_resources.QuadMeshVertices);
      {    // Render a basic quad using the predefined opaque material
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic0));
        render.CmdBindMaterial(m_resources.Materials.MatOpaque);
        render.CmdDraw(vertexCount, 0);
      }
      {    // Render a basic quad using the custom opaque material
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic1));
        render.CmdBindMaterial(m_resources.Materials.MatOpaqueBasicVertFixedColor);
        render.CmdDraw(vertexCount, 0);
      }
      {    // Render a basic quad using the custom opaque material
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic2));
        render.CmdBindMaterial(m_resources.Materials.MatOpaqueBasicFragFixedColor);
        render.CmdDraw(vertexCount, 0);
      }
      {    // Render a basic quad using the custom opaque material
        render.CmdSetCamera(BasicCameraInfo(m_matrices.Orthographic3));
        render.CmdBindMaterial(m_resources.Materials.MatOpaqueBasicFixedColor);
        render.CmdDraw(vertexCount, 0);
      }
    }
    render.EndCmds();
  }


  DevBasicRenderCustomShaderShared::CustomShadersRecord
    DevBasicRenderCustomShaderShared::CreateCustomShaders(IBasicRenderSystem& render, const DevCustomCreateInfo& devCustomCreateInfo)
  {
    constexpr VertexAttributeDescriptionArray<3> VertexAttrDesc = {
      VertexAttributeDescription(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0, "inVertexPosition"),
      VertexAttributeDescription(1, VertexElementFormat::Vector4, VertexElementUsage::Color, 0, "inVertexColor"),
      VertexAttributeDescription(2, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0, "inVertexTextureCoord")};

    BasicShaderCreateInfo basicVertFixedColorCreateInfo(
      BasicShaderStageFlag::Vertex, SpanUtil::AsReadOnlySpan(devCustomCreateInfo.ShaderBasicVertFixedColor), VertexAttrDesc.AsReadOnlySpan());
    BasicShaderCreateInfo basicFragFixedColorCreateInfo(SpanUtil::AsReadOnlySpan(devCustomCreateInfo.ShaderBasicFragFixedColor));

    BasicShader basicVertFixedColor = render.CreateShader(basicVertFixedColorCreateInfo);
    BasicShader basicFragFixedColor = render.CreateShader(basicFragFixedColorCreateInfo);

    return {std::move(basicVertFixedColor), std::move(basicFragFixedColor)};
  }


  DevBasicRenderCustomShaderShared::MaterialsRecord DevBasicRenderCustomShaderShared::CreateMaterials(const IContentManager& contentManager,
                                                                                                      IBasicRenderSystem& render,
                                                                                                      const CustomShadersRecord& shaders,
                                                                                                      const VertexDeclarationSpan& vertexDeclSpan)
  {
    constexpr IO::PathView PathLogo("Textures/GPUSdk/SquareLogo512x512.jpg");
    constexpr IO::PathView PathOpaqueR("Textures/TestText/Opaque/R.png");
    constexpr IO::PathView PathOpaqueG("Textures/TestText/Opaque/G.png");
    constexpr IO::PathView PathOpaqueB("Textures/TestText/Opaque/B.png");
    constexpr IO::PathView PathPreAlpha1("Textures/TestText/Premultiplied/1.png");
    constexpr IO::PathView PathNonPreAlpha("Textures/TestText/Alpha/1.png");

    constexpr BitmapOrigin BitmapOrigin = BitmapOrigin::LowerLeft;

    Texture texture;
    contentManager.Read(texture, PathLogo, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureLogo = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    contentManager.Read(texture, PathOpaqueR, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueR = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, PathOpaqueG, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueG = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, PathOpaqueB, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureOpaqueB = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);

    contentManager.Read(texture, PathPreAlpha1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureAlpha = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);
    contentManager.Read(texture, PathNonPreAlpha, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin, PixelChannelOrder::Undefined, true);
    auto textureNonPreAlpha = render.CreateTexture2D(texture, Texture2DFilterHint::Smooth);


    const BasicMaterialDepthInfo depthInfo(false, false, BasicCompareOp::Less);

    const BasicMaterialInfo basicOpaqueMaterialInfo(BlendState::Opaque, BasicCullMode::Front, BasicFrontFace::CounterClockwise, depthInfo);

    auto matOpaque = render.CreateMaterial(BasicMaterialCreateInfo(basicOpaqueMaterialInfo, vertexDeclSpan), textureAlpha);

    auto matOpaqueBasicVertFixedColor = render.CreateMaterial(
      BasicMaterialCreateInfo(basicOpaqueMaterialInfo, vertexDeclSpan, shaders.BasicVertFixedColor.TryGetHandle(), BasicShaderHandle::Invalid()),
      textureAlpha);

    auto matOpaqueBasicFragFixedColor = render.CreateMaterial(
      BasicMaterialCreateInfo(basicOpaqueMaterialInfo, vertexDeclSpan, BasicShaderHandle::Invalid(), shaders.BasicFragFixedColor.TryGetHandle()),
      textureAlpha);

    auto matOpaqueBasicFixedColor =
      render.CreateMaterial(BasicMaterialCreateInfo(basicOpaqueMaterialInfo, vertexDeclSpan, shaders.BasicVertFixedColor.TryGetHandle(),
                                                    shaders.BasicFragFixedColor.TryGetHandle()),
                            textureAlpha);

    return {std::move(matOpaque), std::move(matOpaqueBasicVertFixedColor), std::move(matOpaqueBasicFragFixedColor),
            std::move(matOpaqueBasicFixedColor)};
  }

  DevBasicRenderCustomShaderShared::Matrices DevBasicRenderCustomShaderShared::CalcMatrices(const DemoWindowMetrics& metrics)
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
    }
    return matrices;
  }
}
