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

#include "S04_Projection.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>

namespace Fsl
{
  namespace
  {
    // Define vertices for a 4-sided pyramid

    const std::array<float, 12 * 3> g_vertexPositions = {
      // Top Of Triangle (Front)
      0.0f,
      1.0f,
      0.0f,
      // Left Of Triangle (Front)
      -1.0f,
      -1.0f,
      1.0f,
      // Right Of Triangle (Front)
      1.0f,
      -1.0f,
      1.0f,
      // Top Of Triangle (Right)
      0.0f,
      1.0f,
      0.0f,
      // Left Of Triangle (Right)
      1.0f,
      -1.0f,
      1.0f,
      // Right Of Triangle (Right)
      1.0f,
      -1.0f,
      -1.0f,
      // Top Of Triangle (Back)
      0.0f,
      1.0f,
      0.0f,
      // Left Of Triangle (Back)
      1.0f,
      -1.0f,
      -1.0f,
      // Right Of Triangle (Back)
      -1.0f,
      -1.0f,
      -1.0f,
      // Top Of Triangle (Left)
      0.0f,
      1.0f,
      0.0f,
      // Left Of Triangle (Left)
      -1.0f,
      -1.0f,
      -1.0f,
      // Right Of Triangle (Left)
      -1.0f,
      -1.0f,
      1.0f,
    };

    const std::array<float, 24 * 3> g_vertexPositions2 = {
      /* Draw A Quad */

      // Top Right Of The Quad (Top)
      1.0f, 1.0f, -1.0f,
      // Top Left Of The Quad (Top)
      -1.0f, 1.0f, -1.0f,
      // Bottom Right Of The Quad (Top)
      1.0f, 1.0f, 1.0f,
      // Bottom Left Of The Quad (Top)
      -1.0f, 1.0f, 1.0f,
      // Top Right Of The Quad (Bottom)
      1.0f, -1.0f, 1.0f,
      // Top Left Of The Quad (Bottom)
      -1.0f, -1.0f, 1.0f,
      // Bottom Right Of The Quad (Bottom)
      1.0f, -1.0f, -1.0f,
      // Bottom Left Of The Quad (Bottom)
      -1.0f, -1.0f, -1.0f,
      // Top Right Of The Quad (Front)
      1.0f, 1.0f, 1.0f,
      // Top Left Of The Quad (Front)
      -1.0f, 1.0f, 1.0f,
      // Bottom Right Of The Quad (Front)
      1.0f, -1.0f, 1.0f,
      // Bottom Left Of The Quad (Front)
      -1.0f, -1.0f, 1.0f,

      // Top Right Of The Quad (Back)
      1.0f, -1.0f, -1.0f,
      // Top Left Of The Quad (Back)
      -1.0f, -1.0f, -1.0f,
      // Bottom Right Of The Quad (Back)
      1.0f, 1.0f, -1.0f,
      // Bottom Left Of The Quad (Back)
      -1.0f, 1.0f, -1.0f,

      // Top Right Of The Quad (Left)
      -1.0f, 1.0f, 1.0f,
      // Top Left Of The Quad (Left)
      -1.0f, 1.0f, -1.0f,
      // Bottom Right Of The Quad (Left)
      -1.0f, -1.0f, 1.0f,
      // Bottom Left Of The Quad (Left)
      -1.0f, -1.0f, -1.0f,
      // Top Right Of The Quad (Right)
      1.0f, 1.0f, -1.0f,
      // Top Left Of The Quad (Right)
      1.0f, 1.0f, 1.0f,
      // Bottom Right Of The Quad (Right)
      1.0f, -1.0f, -1.0f,
      // Bottom Left Of The Quad (Right)
      1.0f, -1.0f, 1.0f

    };

    const std::array<float, 12 * 4> g_vertexColors = {
      // Red
      1.0f,
      0.0f,
      0.0f,
      1.0f,
      // Green
      0.0f,
      1.0f,
      0.0f,
      1.0f,
      // Blue
      0.0f,
      0.0f,
      1.0f,
      1.0f,
      // Red
      1.0f,
      0.0,
      0.0f,
      1.0f,
      // Blue
      0.0f,
      0.0f,
      1.0f,
      1.0f,
      // Green
      0.0f,
      1.0f,
      0.0f,
      1.0f,
      // Red
      1.0f,
      0.0f,
      0.0f,
      1.0f,
      // Green
      0.0f,
      1.0f,
      0.0f,
      1.0f,
      // Blue
      0.0f,
      0.0f,
      1.0f,
      1.0f,
      // Red
      1.0f,
      0.0f,
      0.0f,
      1.0f,
      // Blue
      0.0f,
      0.0f,
      1.0f,
      1.0f,
      // Green
      0.0f,
      1.0f,
      0.0f,
      1.0f,

    };

    const std::array<float, 24 * 4> g_vertexColors2 = {
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,

      // Green
      0.0f, 1.0f, 0.0f, 1.0f,

      // Green
      0.0f, 1.0f, 0.0f, 1.0f,

      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,

      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,

      // Red
      1.0f, 0.0, 0.0f, 1.0f,
      // Red
      1.0f, 0.0, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,

      // Green
      0.0f, 1.0f, 0.0f, 1.0f};


    // The index in these variables should match the g_pszShaderAttributeArray ordering
    constexpr GLuint VertexLoc = 0;
    constexpr GLuint ColorLoc = 1;
    constexpr std::array<GLES3::GLBindAttribLocation, 2> ShaderAttributeArray = {GLES3::GLBindAttribLocation(VertexLoc, "g_vPosition"),
                                                                                 GLES3::GLBindAttribLocation(ColorLoc, "g_vColor")};
  }


  S04_Projection::S04_Projection(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_hModelViewMatrixLoc(0)
    , m_hProjMatrixLoc(0)
    , m_angle(0)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"), SpanUtil::AsReadOnlySpan(ShaderAttributeArray));

    const GLuint hProgram = m_program.Get();

    // Get uniform locations
    GL_CHECK(m_hModelViewMatrixLoc = glGetUniformLocation(hProgram, "g_matModelView"));
    GL_CHECK(m_hProjMatrixLoc = glGetUniformLocation(hProgram, "g_matProj"));


    const float aspectRatio = GetWindowAspectRatio();
    // Build a perspective projection matrix
    m_matProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(57.0f), aspectRatio, 1.0f, 1000.0f);
    m_matTranslate1 = Matrix::CreateTranslation(1.5f, 0.0f, -6.0f);
    m_matTranslate2 = Matrix::CreateTranslation(-1.5f, 0.0f, -6.0f);
  }


  S04_Projection::~S04_Projection() = default;


  void S04_Projection::Update(const DemoTime& demoTime)
  {
    m_angle -= 0.4f * demoTime.DeltaTime;
  }


  void S04_Projection::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    // Rotate and translate the model view matrices
    const Matrix matModelView1 = Matrix::CreateRotationY(m_angle) * m_matTranslate1;
    const Matrix matModelView2 = Matrix::CreateRotationY(m_angle) * m_matTranslate2;

    // Clear the color-buffer and depth-buffer
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If enabled, do depth comparisons and update the depth buffer
    glEnable(GL_DEPTH_TEST);
    // If enabled, cull polygons based on their winding in window coordinates
    glEnable(GL_CULL_FACE);
    // Culls back face
    glCullFace(GL_BACK);

    // Set the shader program
    glUseProgram(m_program.Get());
    // configure the shader
    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, matModelView1.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, m_matProj.DirectAccess());

    // Bind the vertex attributes
    glVertexAttribPointer(VertexLoc, 3, GL_FLOAT, 0, 0, g_vertexPositions.data());
    glEnableVertexAttribArray(VertexLoc);

    glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, 0, 0, g_vertexColors.data());
    glEnableVertexAttribArray(ColorLoc);


    glDrawArrays(GL_TRIANGLES, 0, 12);

    // Update the shaders model view matrix
    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, matModelView2.DirectAccess());

    // Bind the vertex attributes
    glVertexAttribPointer(VertexLoc, 3, GL_FLOAT, 0, 0, g_vertexPositions2.data());
    glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, 0, 0, g_vertexColors2.data());

    /* Drawing Using Triangle strips, draw triangle strips using 4 vertices */
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

    // Cleanup
    glDisableVertexAttribArray(VertexLoc);
    glDisableVertexAttribArray(ColorLoc);
  }
}
