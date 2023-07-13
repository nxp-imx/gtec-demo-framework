/*
 * OpenGL ES 3.0 Tutorial 6
 *
 * Outputs the results of the Fragment shader in Multiple Render Buffers
 */

#include "E6_0_MultipleRenderTargets.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;

  E6_0_MultipleRenderTargets::E6_0_MultipleRenderTargets(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();
    GL_CHECK(glUseProgram(hProgram));

    try
    {
      // Creates Frame Buffer Object and texture to render
      InitFBO();

      GL_CHECK(glEnable(GL_DEPTH_TEST));
      GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
    }
    catch (const std::exception&)
    {
      Cleanup();
      throw;
    }
  }


  E6_0_MultipleRenderTargets::~E6_0_MultipleRenderTargets()
  {
    Cleanup();
  }


  void E6_0_MultipleRenderTargets::Update(const DemoTime& /*demoTime*/)
  {
  }


  void E6_0_MultipleRenderTargets::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    PxSize2D sizePx = GetWindowSizePx();

    GLint defaultFramebuffer = 0;
    const std::array<GLenum, 4> attachments = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);

    // OSTEP5 FIRST: use MRTs to output four colors to four buffers
    glBindFramebuffer(GL_FRAMEBUFFER, m_userData.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffers(UncheckedNumericCast<GLsizei>(attachments.size()), attachments.data());
    DrawGeometry(sizePx.RawWidth(), sizePx.RawHeight());

    // OSTEP6 SECOND: copy the four output buffers into four window quadrants
    // with framebuffer blits

    // Restore the default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebuffer);
    BlitTextures(sizePx.RawWidth(), sizePx.RawHeight());
  }


  int E6_0_MultipleRenderTargets::InitFBO()
  {
    int i = 0;
    GLint defaultFramebuffer = 0;
    const std::array<GLenum, 4> attachments = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

    GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer));

    // OSTEP1 Setup fbo
    GL_CHECK(glGenFramebuffers(1, &m_userData.fbo));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_userData.fbo));

    // OSTEP3 Setup four output buffers and attach to fbo
    m_userData.textureHeight = m_userData.textureWidth = 400;
    GL_CHECK(glGenTextures(4, &m_userData.colorTexId[0]));
    for (i = 0; i < 4; ++i)
    {
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_userData.colorTexId[i]));

      GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_userData.textureWidth, m_userData.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

      // Set the filtering mode
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

      GL_CHECK(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, m_userData.colorTexId[i], 0));
    }

    // OSTEP4 Select the FBO as rendering target
    GL_CHECK(glDrawBuffers(UncheckedNumericCast<GLsizei>(attachments.size()), attachments.data()));

    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
    {
      return 0;
    }

    // Restore the original framebuffer
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));

    return 1;
  }

  ///
  // Draw a quad and output four colors per pixel
  //
  void E6_0_MultipleRenderTargets::DrawGeometry(const int w, const int h)
  {
    const std::array<GLfloat, 4 * 3> vVertices = {
      -0.5f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    };
    const std::array<GLushort, 2 * 3> indices = {0, 1, 2, 0, 2, 3};

    // Set the viewport
    glViewport(0, 0, w, h);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(m_program.Get());

    // Load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), vVertices.data());
    glEnableVertexAttribArray(0);

    // Draw a quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices.data());
  }

  //
  // Copy MRT output buffers to screen
  //
  void E6_0_MultipleRenderTargets::BlitTextures(const int w, const int h)
  {
    // set the fbo for reading
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_userData.fbo);

    // Copy the output red buffer to lower left quadrant
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, m_userData.textureWidth, m_userData.textureHeight, 0, 0, w / 3, h / 3, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Copy the output green buffer to lower right quadrant
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, m_userData.textureWidth, m_userData.textureHeight, w / 2, 0, w, h / 2, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Copy the output blue buffer to upper left quadrant
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBlitFramebuffer(0, 0, m_userData.textureWidth, m_userData.textureHeight, 0, h / 2, w / 2, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Copy the output gray buffer to upper right quadrant
    glReadBuffer(GL_COLOR_ATTACHMENT3);
    glBlitFramebuffer(0, 0, m_userData.textureWidth, m_userData.textureHeight, w / 2, h / 2, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  }


  void E6_0_MultipleRenderTargets::Cleanup()
  {
    if (m_userData.colorTexId[0] != GLValues::INVALID_HANDLE)
    {
      glDeleteTextures(4, &m_userData.colorTexId[0]);
      m_userData.colorTexId[0] = GLValues::INVALID_HANDLE;
    }

    if (m_userData.fbo != GLValues::INVALID_HANDLE)
    {
      glDeleteFramebuffers(1, &m_userData.fbo);
      m_userData.fbo = GLValues::INVALID_HANDLE;
    }
  }
}
