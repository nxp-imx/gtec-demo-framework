/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include "E2_1_CopyBuffer.hpp"
#include <GLES3/gl3.h>
#include <iostream>
#include <FslUtil/OpenGLES3/GLCheck.hpp>


namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    // Attribute Arrays Indexes and Sizes
    constexpr GLint VERTEX_POS_SIZE = 3;        // x, y and z
    constexpr GLsizei VERTEX_COLOR_SIZE = 4;    // r, g, b, and a

    constexpr GLuint VERTEX_POS_INDX = 0;
    constexpr GLuint VERTEX_COLOR_INDX = 1;
  }


  E2_1_CopyBuffer::E2_1_CopyBuffer(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  }


  E2_1_CopyBuffer::~E2_1_CopyBuffer()
  {
    if (m_userData.copyVboIds[0] == GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(UncheckedNumericCast<GLsizei>(m_userData.copyVboIds.size()), m_userData.copyVboIds.data());
      m_userData.copyVboIds[0] = GLValues::INVALID_HANDLE;
    }
    if (m_userData.vboIds[0] == GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(UncheckedNumericCast<GLsizei>(m_userData.vboIds.size()), m_userData.vboIds.data());
      m_userData.vboIds[0] = GLValues::INVALID_HANDLE;
    }
  }


  void E2_1_CopyBuffer::Update(const DemoTime& /*demoTime*/)
  {
  }


  void E2_1_CopyBuffer::Draw(const DemoTime& /*demoTime*/)
  {
    PxSize2D sizePx = GetWindowSizePx();

    glViewport(0, 0, sizePx.Width(), sizePx.Height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
    const std::array<GLfloat, 3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)> vertices = {
      -0.5f, 0.5f,  0.0f,          // v0
      .4f,   0.2f,  0.4f, 1.0f,    // c0
      -1.0f, -0.5f, 0.0f,          // v1
      0.0f,  1.0f,  0.0f, 1.0f,    // c1
      0.0f,  -0.5f, 0.0f,          // v2
      0.0f,  0.0f,  1.0f, 1.0f,    // c2
    };

    // Index buffer data
    std::array<GLushort, 3> indices = {0, 1, 2};

    // vboIds[0] - used to store vertex attribute data
    // vboIds[l] - used to store element indices
    if (m_userData.vboIds[0] == GLValues::INVALID_HANDLE && m_userData.vboIds[1] == GLValues::INVALID_HANDLE)
    {
      // OSTEP1 GENERATE SOURCE BUFFERS
      glGenBuffers(UncheckedNumericCast<GLsizei>(m_userData.vboIds.size()), m_userData.vboIds.data());
      // OSTEP2 GENERATE DESTINATION BUFFERS
      glGenBuffers(UncheckedNumericCast<GLsizei>(m_userData.copyVboIds.size()), m_userData.copyVboIds.data());

      // OSTEP3 BIND THE ORIGINAL BUFFERS AND FILL THEM WITH DATA
      constexpr auto cbVertices = sizeof(GLfloat) * vertices.size();
      constexpr auto cbIndices = sizeof(GLushort) * indices.size();
      glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]);
      glBufferData(GL_ARRAY_BUFFER, cbVertices, vertices.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, cbIndices, indices.data(), GL_STATIC_DRAW);

      // OSTEP4 BIND THE DESTINATION BUFFERS AND ALLOCATE SPACE FOR THEM
      glBindBuffer(GL_ARRAY_BUFFER, m_userData.copyVboIds[0]);
      glBufferData(GL_ARRAY_BUFFER, cbVertices, nullptr, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.copyVboIds[1]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, cbIndices, nullptr, GL_STATIC_DRAW);

      // OSTEP5 Unbind Buffers, needed to be able to execute operations on the buffers!
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      // OSTEP6 BIND TARGET AND DESTINATION BUFFERS, here we will copy Vertex Data
      glBindBuffer(GL_COPY_READ_BUFFER, m_userData.vboIds[0]);
      glBindBuffer(GL_COPY_WRITE_BUFFER, m_userData.copyVboIds[0]);

      // OSTEP7 Copy the actual Vertex Data!
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, cbVertices);

      // OSTEP8 BIND TARGET AND DESTINATION BUFFERS, here we will copy Index Data
      glBindBuffer(GL_COPY_READ_BUFFER, m_userData.vboIds[1]);
      glBindBuffer(GL_COPY_WRITE_BUFFER, m_userData.copyVboIds[1]);

      // OSTEP9 Copy the actual Index Data!
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, cbIndices);
    }

    // USE THE COPIED BUFFERS TO DRAW
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.copyVboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.copyVboIds[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);

    intptr_t offset = 0;
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          reinterpret_cast<const void*>(offset));

    offset += VERTEX_POS_SIZE * sizeof(GLfloat);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          reinterpret_cast<const void*>(offset));

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}
