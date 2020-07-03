#ifndef GLES3_RENDERTOTEXTURE_RENDERTOTEXTURE_HPP
#define GLES3_RENDERTOTEXTURE_RENDERTOTEXTURE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>

#include <array>
#ifdef FSL_PLATFORM_YOCTO
#include <g2d.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#include <GLES3/gl3.h>
#include "GLES2/gl2ext.h"
#endif
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Fsl
{
  enum class TextureBacking
  {
    g2d = 0,
    openGL
  };

#ifdef FSL_PLATFORM_YOCTO
  struct MappedBuffer
  {
    struct g2d_buf* g2dBuffer{nullptr};
    void* g2dHandle{nullptr};
    void* glBuffer{nullptr};
  };
#endif

  class RenderToTexture
    : public DemoAppGLES3
    , public UI::EventListener
  {
    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<UI::Switch> m_usingExtensions;
    std::shared_ptr<UI::Switch> m_usingG2D;

    GLES3::GLProgram m_mainProgram;
    GLES3::GLProgram m_renderPassProgram;

#ifdef FSL_PLATFORM_YOCTO
    MappedBuffer m_mappedBuffer{};
#endif

    std::array<GLuint, 2> m_mappedTargetTexture{};
    GLuint m_notMappedTargetTexture{};

    std::array<GLuint, 2> m_mappedFBO{};
    GLuint m_notMappedFBO{};

    GLuint m_fboDepth{};
    std::array<GLenum, 1> m_fboOutputBuffers{GL_COLOR_ATTACHMENT0};

    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::mat4 m_MVP;
    GLint m_MVPLoc{};
    GLint m_samplerLoc{};

    glm::mat4 m_rpModel;
    glm::mat4 m_rpView;
    glm::mat4 m_rpProj;
    glm::mat4 m_rpMVP;
    GLint m_rpMVPLoc{};

    float m_angle;

  public:
    explicit RenderToTexture(const DemoAppConfig& config);
    ~RenderToTexture() override;

  protected:
    void Draw(const DemoTime& demoTime) override;
    void OnKeyEvent(const KeyEvent& event) override;

    void PrepareCubeShader();
    void DrawMainPass(GLuint& texture);

    void PrepareTargetTexture(GLuint& texture);
    void PrepareTargetTexture(GLuint& texture, const Fsl::TextureBacking& textureBackingScheme);
    void MapBufferToTexture(const Fsl::TextureBacking& textureBackingScheme);

    void Prepare3DFBO(GLuint& fbo, GLuint& targetTexture);
    void Prepare3DRenderPassShader();
    void Draw3DRenderPass(GLuint fbo);


#ifdef FSL_PLATFORM_YOCTO
    void CreateContiguousBuffer();
    void DestroyContiguousBuffer();
#endif
  };
}

#endif
