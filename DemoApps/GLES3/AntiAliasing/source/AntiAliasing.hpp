#ifndef GLES3_ANTIALIASING_ANTIALIASING_HPP
#define GLES3_ANTIALIASING_ANTIALIASING_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/Draw/GLLineDraw.hpp>
#include <FslUtil/OpenGLES3/GLFrameBuffer.hpp>
#include <FslUtil/OpenGLES3/GLMultisampleFrameBuffer.hpp>
#include <FslUtil/OpenGLES3/GLMultisampleFrameBufferTextureExt.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <Shared/AntiAliasing/AntiAliasingShared.hpp>
#include "RenderModel.hpp"

namespace Fsl
{
  class AntiAliasing final : public DemoAppGLES3
  {
    using base_type = DemoAppGLES3;

    struct VertexUBOData
    {
      Matrix MatViewProjection;
    };

    struct ZoomVertexUBOData
    {
      Matrix MatWorldViewProjection;
    };

    struct LineProgramInfo
    {
      GLES3::GLProgram Program;
      GLint LocWorldViewProjection{GLES3::GLValues::InvalidLocation};
      std::array<GLES3::GLVertexAttribLink, 2> Links;
    };

    struct PostProcessProgramInfo
    {
      GLES3::GLProgram Program;
      GLint LocTexture{GLES3::GLValues::InvalidLocation};
      std::array<GLES3::GLVertexAttribLink, 1> Links;
    };

    struct LineDrawResources
    {
      LineProgramInfo ProgramInfo;
      GLES3::GLLineDraw Render;
    };

    struct PostProcessResources
    {
      PostProcessProgramInfo ProgramInfo;
      GLES3::GLVertexBuffer VertexBuffer;

      PostProcessResources() = default;
      PostProcessResources(PostProcessProgramInfo programInfo, GLES3::GLVertexBuffer vertexBuffer)
        : ProgramInfo(std::move(programInfo))
        , VertexBuffer(std::move(vertexBuffer))
      {
      }
    };

    struct ZoomProgramInfo
    {
      GLES3::GLProgram Program;
      GLint LocWorldViewProjection{GLES3::GLValues::InvalidLocation};
      GLint LocTexture{GLES3::GLValues::InvalidLocation};
      std::array<GLES3::GLVertexAttribLink, 2> Links;
    };

    struct ZoomResources
    {
      ZoomProgramInfo ProgramInfo;
      GLES3::GLVertexBuffer VertexBuffer;
      GLES3::GLVertexBuffer VertexBufferCustomZoom;

      ZoomResources() = default;
      ZoomResources(ZoomProgramInfo programInfo, GLES3::GLVertexBuffer vertexBuffer, GLES3::GLVertexBuffer vertexBufferCustomZoom)
        : ProgramInfo(std::move(programInfo))
        , VertexBuffer(std::move(vertexBuffer))
        , VertexBufferCustomZoom(std::move(vertexBufferCustomZoom))
      {
      }
    };

    struct FrameBufferResources
    {
      AntiAliasingMethod ActiveAntiAliasingMethod{AntiAliasingMethod::Disabled};

    private:
      PxSize2D m_sizePx;
      GLES3::GLFrameBuffer m_framebuffer;
      GLES3::GLMultisampleFrameBuffer m_multisampleFramebuffer;
      GLES3::GLMultisampleFrameBufferTextureExt m_multisampleFramebufferTextureExt;

    public:
      // Needed to do a accurate zoom on the SSAA content
      GLES3::GLFrameBuffer ZoomFramebuffer;

      PxSize1D ScalePx;
      PxSize2D ScaledSizePx;

      FrameBufferResources() = default;
      FrameBufferResources(AntiAliasingMethod activeAntiAliasingMethod, const PxSize2D sizePx, GLES3::GLFrameBuffer framebuffer)
        : FrameBufferResources(activeAntiAliasingMethod, sizePx, std::move(framebuffer), {}, PxSize1D::Create(1))
      {
      }

      FrameBufferResources(AntiAliasingMethod activeAntiAliasingMethod, const PxSize2D sizePx, GLES3::GLFrameBuffer framebuffer,
                           const PxSize1D scalePx)
        : FrameBufferResources(activeAntiAliasingMethod, sizePx, std::move(framebuffer), {}, scalePx)
      {
      }

      FrameBufferResources(AntiAliasingMethod activeAntiAliasingMethod, const PxSize2D sizePx, GLES3::GLFrameBuffer framebuffer,
                           GLES3::GLMultisampleFrameBuffer multisampleFramebuffer)
        : FrameBufferResources(activeAntiAliasingMethod, sizePx, std::move(framebuffer), std::move(multisampleFramebuffer), PxSize1D::Create(1))
      {
      }

      FrameBufferResources(AntiAliasingMethod activeAntiAliasingMethod, const PxSize2D sizePx, GLES3::GLFrameBuffer framebuffer,
                           GLES3::GLMultisampleFrameBuffer multisampleFramebuffer, const PxSize1D scalePx)
        : ActiveAntiAliasingMethod(activeAntiAliasingMethod)
        , m_sizePx(sizePx)
        , m_framebuffer(std::move(framebuffer))
        , m_multisampleFramebuffer(std::move(multisampleFramebuffer))
        , ScalePx(scalePx)
        , ScaledSizePx(m_multisampleFramebuffer.IsValid() ? m_multisampleFramebuffer.GetSize() : m_framebuffer.GetSize())
      {
      }

      FrameBufferResources(AntiAliasingMethod activeAntiAliasingMethod, const PxSize2D sizePx, GLES3::GLMultisampleFrameBufferTextureExt framebuffer)
        : ActiveAntiAliasingMethod(activeAntiAliasingMethod)
        , m_sizePx(sizePx)
        , m_multisampleFramebufferTextureExt(std::move(framebuffer))
        , ScalePx(PxSize1D::Create(1))
        , ScaledSizePx(m_multisampleFramebufferTextureExt.GetSize())
      {
      }

      GLuint GetFrameBufferHandle() const noexcept
      {
        if (m_multisampleFramebufferTextureExt.IsValid())
        {
          return m_multisampleFramebufferTextureExt.Get();
        }
        if (m_multisampleFramebuffer.IsValid())
        {
          return m_multisampleFramebuffer.Get();
        }
        return m_framebuffer.Get();
      }

      PxSize2D GetFrameBufferSize() const noexcept
      {
        if (m_multisampleFramebufferTextureExt.IsValid())
        {
          return m_multisampleFramebufferTextureExt.GetSize();
        }
        if (m_multisampleFramebuffer.IsValid())
        {
          return m_multisampleFramebuffer.GetSize();
        }
        return m_framebuffer.GetSize();
      }

      bool ResolvedRequired() const noexcept
      {
        return m_multisampleFramebuffer.IsValid();
      }

      const GLES3::GLFrameBuffer& GetCustomZoomFB() const noexcept
      {
        return m_framebuffer;
      }

      GLuint GetResolveFBHandle() const noexcept
      {
        return m_framebuffer.Get();
      }

      PxSize2D GetResolveFBSize() const noexcept
      {
        return m_framebuffer.GetSize();
      }

      GLES3::GLTextureInfo GetTextureInfo() const
      {
        if (m_multisampleFramebufferTextureExt.IsValid())
        {
          return m_multisampleFramebufferTextureExt.GetTextureInfo();
        }
        return m_framebuffer.GetTextureInfo();
      }

      PxSize2D GetTextureSize() const
      {
        if (m_multisampleFramebufferTextureExt.IsValid())
        {
          return m_multisampleFramebufferTextureExt.GetSize();
        }
        return m_framebuffer.GetSize();
      }

      void Reset()
      {
        m_framebuffer.Reset();
        m_multisampleFramebuffer.Reset();
        m_multisampleFramebufferTextureExt.Reset();
      }

      PxSize2D GetSize() const noexcept
      {
        return m_sizePx;
      }

      bool IsUsingFastResolve() const noexcept
      {
        return m_multisampleFramebufferTextureExt.IsValid();
      }
    };


    struct Resources
    {
      FrameBufferResources Offscreen;
      LineDrawResources Line;
      LineDrawResources DividerLine;
      PostProcessResources PostProcess;
      ZoomResources Zoom;

      Resources() noexcept = default;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;
    };

    AntiAliasingShared m_shared;
    RenderModel m_renderModelCar;
    RenderModel m_renderModelKnight;

    Resources m_resources;
    VertexUBOData m_linesVertexUboData;
    ZoomVertexUBOData m_zoomVertexUboData;

  public:
    explicit AntiAliasing(const DemoAppConfig& config);
    ~AntiAliasing() final;

  protected:
    void OnKeyEvent(const KeyEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void Resolve(const DemoTime& demoTime) final;
    void Draw(const FrameInfo& frameInfo) final;

  private:
    static GLES3::GLFrameBuffer CreateNoAAFramebuffer(const PxSize2D sizePx);
    static GLES3::GLMultisampleFrameBuffer CreateMSAAFramebuffer(const PxSize2D sizePx, const GLsizei maxSamples);
    static GLES3::GLFrameBuffer CreateMSAAResolveFramebuffer(const PxSize2D sizePx);
    static GLES3::GLMultisampleFrameBufferTextureExt CreateMSAAFastResolveFramebuffer(const PxSize2D sizePx, const int32_t maxSamples);
    static GLES3::GLFrameBuffer CreateSSAAFramebuffer(const PxSize2D sizePx, const int32_t maxSamples);
    static FrameBufferResources CreateFramebufferResources(const PxSize2D sizePx, const AntiAliasingMethod aaMethod, const bool useFastResolve);

    LineProgramInfo CreateLineProgram(const std::shared_ptr<IContentManager>& contentManager);
    PostProcessProgramInfo CreatePostProcessProgram(const std::shared_ptr<IContentManager>& contentManager);
    PostProcessResources CreatePostProcessResources(const std::shared_ptr<IContentManager>& contentManager);
    ZoomProgramInfo CreateZoomProgram(const std::shared_ptr<IContentManager>& contentManager);
    ZoomResources CreateZoomResources(const std::shared_ptr<IContentManager>& contentManager, const DemoWindowMetrics& windowMetrics);

    void RecreateFrameBufferIfNecessary();

    PxSize2D ApplyViewport(const ContentWindowId contentWindowId);
    static void RenderLines(LineDrawResources& rLine, const PxSize1D scalePx, const Fsl::Graphics3D::LineBuilder& lineBuilder,
                            const Matrix& matrixWorldViewProjection);
    static void GenerateCustomZoomVertices(GLES3::GLVertexBuffer& rVertexBuffer, const PxSize2D textureSizePx, const PxSize1D texScalePx,
                                           const UI::Custom::ZoomInfo& zoomInfo);
    static void GenerateCustomZoomVertices2(GLES3::GLVertexBuffer& rVertexBuffer, const PxSize2D sizePx, const UI::Custom::ZoomInfo& zoomInfo);
    static void GenerateZoomVertices(GLES3::GLVertexBuffer& rVertexBuffer, const PxSize2D sizePx, const PxSize1D texScalePx,
                                     const UI::Custom::ZoomInfo& zoomInfo);

    void DrawToMultisampleFB(const DemoTime& demoTime);
    static void DrawToZoomFB(const UI::Custom::ZoomInfo& zoomInfo, GLES3::GLFrameBuffer& rZoomFramebuffer, ZoomResources& rZoomResources,
                             const GLES3::GLFrameBuffer& srcFramebuffer, const PxSize1D srcTexScalePx);
    void DrawFinalComposite(const UI::Custom::ZoomInfo& zoomInfo, const PxSize2D windowSizePx, const bool useCustomZoom);

    static void ResolveFrameBuffer(const GLuint hDst, const PxSize2D dstSizePx, const GLuint hSrc, const PxSize2D srcSizePx);
    static void DrawFrameBuffer(const PostProcessResources& resources, const GLuint hTexture);
    static void DrawZoomArea(const GLuint hZoomTexture, const ZoomProgramInfo& programInfo, const GLES3::GLVertexBuffer& vertexBuffer,
                             const Matrix& matWorldViewProjection, const bool useLinearFiltering = false);
  };
}

#endif
