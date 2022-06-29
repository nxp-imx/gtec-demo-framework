/*
 * OpenGL ES 3.0 Tutorial 4
 *
 * Primitive restart operation
 */

#include "E4_0_PRestart.hpp"
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    // Attribute Arrays Indexes and Sizes
    constexpr GLint VERTEX_POS_SIZE = 3;      // x, y and z
    constexpr GLint VERTEX_COLOR_SIZE = 4;    // r, g, b, and a

    constexpr GLuint VERTEX_POS_INDX = 0;
    constexpr GLuint VERTEX_COLOR_INDX = 1;

    constexpr GLsizei VERTEX_STRIDE = (sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE));

  }

  E4_0_PRestart::E4_0_PRestart(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    try
    {
      // VERTEX DATA
      // OSTEP1 12 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
      const std::array<GLfloat, 12 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)> vertices0 = {
        -1.0f, -1.0f, 0.0f,        // v0
        1.0f, 0.0f, 0.0f, 1.0f,    // c0
        -1.0f, -0.5f, 0.0f,        // v1
        0.0f, 1.0f, 0.0f, 1.0f,    // c1
        -0.5f, -1.0f, 0.0f,        // v2
        0.0f, 0.0f, 1.0f, 1.0f,    // c2
        -0.5f, -0.5f, 0.0f,        // v3
        1.0f, 0.0f, 1.0f, 1.0f,    // c3
        0.0f, -1.0f, 0.0f,         // v4
        0.3f, 0.2f, 1.0f, 1.0f,    // c4
        0.0f, -0.5f, 0.0f,         // v5
        1.0f, 0.0f, 1.0f, 1.0f,    // c5
        // NEW VERTICES ARE LOADED
        -1.0f, 0.0f, 0.0f,         // v0
        1.0f, 0.0f, 0.0f, 1.0f,    // c0
        -1.0f, 0.5f, 0.0f,         // v1
        0.0f, 1.0f, 0.0f, 1.0f,    // c1
        -0.5f, 0.0f, 0.0f,         // v2
        0.0f, 0.0f, 1.0f, 1.0f,    // c2
        -0.5f, 0.5f, 0.0f,         // v3
        1.0f, 0.0f, 1.0f, 1.0f,    // c3
        0.0f, 0.0f, 0.0f,          // v4
        0.3f, 0.2f, 1.0f, 1.0f,    // c4
        0.0f, 0.5f, 0.0f,          // v5
        1.0f, 0.0f, 1.0f, 1.0f,    // c5
      };

      // OSTEP2 Define the Indices, when you want to render a new primitive use the PRIMITIVE RESTART INDEX
      const std::array<GLushort, 13> indices = {0, 1, 2, 3, 4, 5, 0xFFFF, 6, 7, 8, 9, 10, 11};

      // Generate VBO Ids and load the VBOs with data
      GL_CHECK(glGenBuffers(2, m_userData.vboIds));

      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]));
      GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices0.size(), vertices0.data(), GL_STATIC_DRAW));
      GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]));
      GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), indices.data(), GL_STATIC_DRAW));

      // Generate VAO Id
      GL_CHECK(glGenVertexArrays(1, &m_userData.vaoId[0]));

      // Bind the VAO and then setup the vertex
      // attributes
      GL_CHECK(glBindVertexArray(m_userData.vaoId[0]));

      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]));
      GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]));

      GL_CHECK(glEnableVertexAttribArray(VERTEX_POS_INDX));
      GL_CHECK(glEnableVertexAttribArray(VERTEX_COLOR_INDX));

      GL_CHECK(glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, nullptr));

      GL_CHECK(glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
                                     reinterpret_cast<const void*>(VERTEX_POS_SIZE * sizeof(GLfloat))));

      // Restore the VAO Id to the default
      GL_CHECK(glBindVertexArray(0));

      GL_CHECK(glEnable(GL_DEPTH_TEST));
      GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
    }
    catch (const std::exception&)
    {
      Cleanup();
      throw;
    }
  }


  E4_0_PRestart::~E4_0_PRestart()
  {
    Cleanup();
  }


  void E4_0_PRestart::Update(const DemoTime& /*demoTime*/)
  {
  }


  void E4_0_PRestart::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    PxSize2D sizePx = GetWindowSizePx();
    glViewport(0, 0, sizePx.Width(), sizePx.Height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the VAO
    glBindVertexArray(m_userData.vaoId[0]);

    // OSTEP3 Enable the Primitive Restart Functionality
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

    // OSTEP4 Draw the triangles
    glDrawElements(GL_TRIANGLE_STRIP, 13, GL_UNSIGNED_SHORT, nullptr);

    glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    // Return to the default VAO
    glBindVertexArray(0);
  }


  void E4_0_PRestart::Cleanup()
  {
    // Return to the default VAO
    glBindVertexArray(0);

    if (m_userData.vboIds[0] == GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(2, m_userData.vboIds);
      m_userData.vboIds[0] = GLValues::INVALID_HANDLE;
    }
    if (m_userData.vaoId[0] == GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(1, m_userData.vaoId);
      m_userData.vaoId[0] = GLValues::INVALID_HANDLE;
    }
  }
}
