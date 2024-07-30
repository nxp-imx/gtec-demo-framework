/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include "E2_1_CopyBuffer.hpp"
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <iostream>


namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    // Attribute Arrays Indexes and Sizes
    constexpr GLint VertexPosSize = 3;        // x, y and z
    constexpr GLsizei VertexColorSize = 4;    // r, g, b, and a

    constexpr GLuint VertexPosIndx = 0;
    constexpr GLuint VertexColorIndx = 1;
  }


  E2_1_CopyBuffer::E2_1_CopyBuffer(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  }


  E2_1_CopyBuffer::~E2_1_CopyBuffer()
  {
    if (m_userData.CopyVboIds[0] == GLValues::InvalidHandle)
    {
      glDeleteBuffers(UncheckedNumericCast<GLsizei>(m_userData.CopyVboIds.size()), m_userData.CopyVboIds.data());
      m_userData.CopyVboIds[0] = GLValues::InvalidHandle;
    }
    if (m_userData.VboIds[0] == GLValues::InvalidHandle)
    {
      glDeleteBuffers(UncheckedNumericCast<GLsizei>(m_userData.VboIds.size()), m_userData.VboIds.data());
      m_userData.VboIds[0] = GLValues::InvalidHandle;
    }
  }


  void E2_1_CopyBuffer::Update(const DemoTime& /*demoTime*/)
  {
  }


  void E2_1_CopyBuffer::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    PxSize2D sizePx = GetWindowSizePx();

    glViewport(0, 0, sizePx.RawWidth(), sizePx.RawHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
    const std::array<GLfloat, 3 * (VertexPosSize + VertexColorSize)> vertices = {
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
    if (m_userData.VboIds[0] == GLValues::InvalidHandle && m_userData.VboIds[1] == GLValues::InvalidHandle)
    {
      // OSTEP1 GENERATE SOURCE BUFFERS
      glGenBuffers(UncheckedNumericCast<GLsizei>(m_userData.VboIds.size()), m_userData.VboIds.data());
      // OSTEP2 GENERATE DESTINATION BUFFERS
      glGenBuffers(UncheckedNumericCast<GLsizei>(m_userData.CopyVboIds.size()), m_userData.CopyVboIds.data());

      // OSTEP3 BIND THE ORIGINAL BUFFERS AND FILL THEM WITH DATA
      constexpr auto CbVertices = sizeof(GLfloat) * vertices.size();
      constexpr auto CbIndices = sizeof(GLushort) * indices.size();
      glBindBuffer(GL_ARRAY_BUFFER, m_userData.VboIds[0]);
      glBufferData(GL_ARRAY_BUFFER, CbVertices, vertices.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.VboIds[1]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, CbIndices, indices.data(), GL_STATIC_DRAW);

      // OSTEP4 BIND THE DESTINATION BUFFERS AND ALLOCATE SPACE FOR THEM
      glBindBuffer(GL_ARRAY_BUFFER, m_userData.CopyVboIds[0]);
      glBufferData(GL_ARRAY_BUFFER, CbVertices, nullptr, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.CopyVboIds[1]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, CbIndices, nullptr, GL_STATIC_DRAW);

      // OSTEP5 Unbind Buffers, needed to be able to execute operations on the buffers!
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      // OSTEP6 BIND TARGET AND DESTINATION BUFFERS, here we will copy Vertex Data
      glBindBuffer(GL_COPY_READ_BUFFER, m_userData.VboIds[0]);
      glBindBuffer(GL_COPY_WRITE_BUFFER, m_userData.CopyVboIds[0]);

      // OSTEP7 Copy the actual Vertex Data!
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, CbVertices);

      // OSTEP8 BIND TARGET AND DESTINATION BUFFERS, here we will copy Index Data
      glBindBuffer(GL_COPY_READ_BUFFER, m_userData.VboIds[1]);
      glBindBuffer(GL_COPY_WRITE_BUFFER, m_userData.CopyVboIds[1]);

      // OSTEP9 Copy the actual Index Data!
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, CbIndices);
    }

    // USE THE COPIED BUFFERS TO DRAW
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.CopyVboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.CopyVboIds[1]);

    glEnableVertexAttribArray(VertexPosIndx);
    glEnableVertexAttribArray(VertexColorIndx);

    intptr_t offset = 0;
    glVertexAttribPointer(VertexPosIndx, VertexPosSize, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (VertexPosSize + VertexColorSize),
                          reinterpret_cast<const void*>(offset));

    offset += VertexPosSize * sizeof(GLfloat);
    glVertexAttribPointer(VertexColorIndx, VertexColorSize, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (VertexPosSize + VertexColorSize),
                          reinterpret_cast<const void*>(offset));

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(VertexPosIndx);
    glDisableVertexAttribArray(VertexColorIndx);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}
