#ifndef GLES3_SDFFONTS_SDFFONTS_HPP
#define GLES3_SDFFONTS_SDFFONTS_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2025 NXP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES3/GLIndexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <Shared/SdfFonts/Shared.hpp>
#include <array>
#include <utility>
#include <vector>

namespace Fsl
{
  class SdfFonts : public DemoAppGLES3
  {
    struct LocationRecord
    {
      GLint OutlineDistance{GLES3::GLValues::InvalidLocation};
      GLint ProjMatrix{GLES3::GLValues::InvalidLocation};
      GLint Smoothing{GLES3::GLValues::InvalidLocation};
      GLint ShadowOffset{GLES3::GLValues::InvalidLocation};
      GLint ShadowSmoothing{GLES3::GLValues::InvalidLocation};
      GLint ContourScale{GLES3::GLValues::InvalidLocation};
      GLint Texture{GLES3::GLValues::InvalidLocation};
    };


    struct MeshRecord
    {
      GLuint CachedShader{GLES3::GLValues::InvalidHandle};

      PxPoint2 Offset;
      BitmapFontConfig FontConfig;

      std::vector<VertexPositionTexture> Vertices;
      GLES3::GLVertexBuffer VB;
      GLES3::GLIndexBuffer IB;

      std::array<GLES3::GLVertexAttribLink, 2> AttribLink;

      MeshRecord() = default;
      MeshRecord(const PxPoint2& offset, const BitmapFontConfig& fontConfig, std::vector<VertexPositionTexture> vertices, GLES3::GLVertexBuffer vb,
                 GLES3::GLIndexBuffer ib, const std::array<GLES3::GLVertexAttribLink, 2>& attribLink)
        : Offset(offset)
        , FontConfig(fontConfig)
        , Vertices(std::move(vertices))
        , VB(std::move(vb))
        , IB(std::move(ib))
        , AttribLink(attribLink)
      {
      }
    };

    struct ShaderRecord
    {
      GLES3::GLProgram Program;
      LocationRecord Location;
    };

    struct FontRecord
    {
      GLES3::GLTexture Texture;
      TextureAtlasSpriteFont Font;
    };

    struct ExampleRecord
    {
      FontRecord Font;
      MeshRecord Mesh;
      MeshRecord ScaledMesh;
    };

    struct FontShaderRecord
    {
      ShaderRecord Normal;
      ShaderRecord Outline;
      ShaderRecord Shadow;
      ShaderRecord ShadowAndOutline;
      ShaderRecord Contours;
    };

    struct Resources
    {
      ExampleRecord Normal;
      ExampleRecord Sdf;
      ExampleRecord Mtsdf;

      ShaderRecord ShaderNormal;
      FontShaderRecord ShadersSdf;
      FontShaderRecord ShadersMtsdf;

      AtlasTexture2D FillTexture;
      Matrix Projection;
    };

    Shared m_shared;

    std::shared_ptr<INativeBatch2D> m_nativeBatch;

    Resources m_resources;
    std::vector<SpriteFontGlyphPosition> m_positionsScratchpad;

  public:
    explicit SdfFonts(const DemoAppConfig& config);
    ~SdfFonts() override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void DrawMeshes(const FontDrawConfig& fontDrawConfig, const SdfFonts::ShaderRecord& fontSdfShader);

    static void DrawTextMesh(const MeshRecord& mesh, const FontRecord& fontRecord, const ShaderRecord& shader, const Matrix& projection,
                             const FontDrawConfig& fontDrawConfig);

    static FontRecord PrepareFont(const IContentManager& contentManager, const IO::Path& bitmapFontPath, const IO::Path& fontAtlasTexturePath,
                                  const SpriteNativeAreaCalc& spriteNativeAreaCalc, const uint32_t densityDpi);
    static ExampleRecord PrepareExample(const IContentManager& contentManager, const PxSize1D lineYPx, const ShaderRecord& shaderNormal,
                                        const IO::Path& bitmapFontPath, const IO::Path& fontAtlasTexturePath, const StringViewLite& strView,
                                        const SpriteNativeAreaCalc& spriteNativeAreaCalc, const uint32_t densityDpi,
                                        std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad);

    static ShaderRecord GenerateShaderRecord(const IContentManager& contentManager, const IO::Path& vertShaderPath, const IO::Path& fragShaderPath);


    static MeshRecord GenerateMesh(const PxPoint2& dstPositionPx, const FontRecord& fontRecord, const BitmapFontConfig& fontConfig,
                                   const StringViewLite& strView, std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad);
    static void RegenerateMeshOnDemand(MeshRecord& rMeshRecord, const PxPoint2& dstPositionPx, const ShaderRecord& shader,
                                       const FontRecord& fontRecord, const BitmapFontConfig fontConfig, const StringViewLite& strView,
                                       std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad);

    const ShaderRecord& SelectShaderRecord(const SdfFontMode fontSdfMode, const SdfType fontSdfType);
  };
}

#endif
