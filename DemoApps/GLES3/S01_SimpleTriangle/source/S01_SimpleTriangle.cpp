/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * (ported to the Freescale demo framework by Freescale '14)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */

#include "S01_SimpleTriangle.hpp"
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>

namespace Fsl
{
  namespace
  {
    // Define vertice for a triangle
    const std::array<float, 6 * 3> g_vertexPositions = {0.0f,   100.0f, 0.0f, -100.0f, -100.0f, 0.0f, +100.0f, -100.0f, 0.0f,
                                                        100.0f, 100.0f, 0.0f, -100.0f, 100.0f,  0.0f, 100.0f,  -100.0,  0.0f};

    // The index in these variables should match the g_pszShaderAttributeArray ordering
    constexpr GLuint g_hVertexLoc = 0;
    constexpr std::array<GLES3::GLBindAttribLocation, 1> g_shaderAttributeArray = {GLES3::GLBindAttribLocation(g_hVertexLoc, "g_vPosition")};
  }

  S01_SimpleTriangle::S01_SimpleTriangle(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_hModelViewMatrixLoc(0)
    , m_hProjMatrixLoc(0)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"), ReadOnlySpanUtil::AsSpan(g_shaderAttributeArray));

    const GLuint hProgram = m_program.Get();

    // Get uniform locations
    GL_CHECK(m_hModelViewMatrixLoc = glGetUniformLocation(hProgram, "g_matModelView"));
    GL_CHECK(m_hProjMatrixLoc = glGetUniformLocation(hProgram, "g_matProj"));

    // Set the shader program
    GL_CHECK(glUseProgram(hProgram));

    // Set the Clear Color Value
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.5f, 1.0f));

    // If enabled, do depth comparisons and update the depth buffer
    GL_CHECK(glEnable(GL_DEPTH_TEST));
  }


  S01_SimpleTriangle::~S01_SimpleTriangle() = default;


  void S01_SimpleTriangle::Update(const DemoTime& /*demoTime*/)
  {
  }


  void S01_SimpleTriangle::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.Width(), currentSizePx.Height());

    // Clear the color-buffer and depth-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Matrix matProj =
      Matrix::CreatePerspective(static_cast<float>(currentSizePx.Width()), static_cast<float>(currentSizePx.Height()), 1.0f, 1000.0f);
    const Matrix matModelView = Matrix::CreateTranslation(0, 0, -1);

    // Set the shader program
    glUseProgram(m_program.Get());
    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, matModelView.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, matProj.DirectAccess());

    // Bind the vertex attributes
    glVertexAttribPointer(g_hVertexLoc, 3, GL_FLOAT, 0, 0, g_vertexPositions.data());
    glEnableVertexAttribArray(g_hVertexLoc);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Cleanup
    glDisableVertexAttribArray(g_hVertexLoc);
  }
}
