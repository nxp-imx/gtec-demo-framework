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

#include "S02_ColoredTriangle.hpp"
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
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

    const std::array<float, 6 * 3> g_vertexColors = {
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,    // Red
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f     // Green
    };

    // The index in these variables should match the g_pszShaderAttributeArray ordering
    constexpr GLuint VertexLoc = 0;
    constexpr GLuint ColorLoc = 1;

    constexpr std::array<GLES3::GLBindAttribLocation, 2> ShaderAttributeArray = {GLES3::GLBindAttribLocation(VertexLoc, "g_vPosition"),
                                                                                 GLES3::GLBindAttribLocation(ColorLoc, "g_vColor")};
  }

  S02_ColoredTriangle::S02_ColoredTriangle(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_hModelViewMatrixLoc(0)
    , m_hProjMatrixLoc(0)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"), SpanUtil::AsReadOnlySpan(ShaderAttributeArray));

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


  S02_ColoredTriangle::~S02_ColoredTriangle() = default;


  void S02_ColoredTriangle::Update(const DemoTime& /*demoTime*/)
  {
  }


  void S02_ColoredTriangle::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    // Clear the color-buffer and depth-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Matrix matProj =
      Matrix::CreatePerspective(static_cast<float>(currentSizePx.RawWidth()), static_cast<float>(currentSizePx.RawHeight()), 1.0f, 1000.0f);
    const Matrix matModelView = Matrix::CreateTranslation(0, 0, -1);

    // Set the shader program
    glUseProgram(m_program.Get());
    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, matModelView.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, matProj.DirectAccess());

    // Bind the vertex attributes
    glVertexAttribPointer(VertexLoc, 3, GL_FLOAT, 0, 0, g_vertexPositions.data());
    glEnableVertexAttribArray(VertexLoc);

    glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, 0, 0, g_vertexColors.data());
    glEnableVertexAttribArray(ColorLoc);


    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Cleanup
    glDisableVertexAttribArray(VertexLoc);
    glDisableVertexAttribArray(ColorLoc);
  }
}
