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


//Attribute Arrays Indexes and Sizes
#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

namespace Fsl
{
  using namespace GLES3;

  E2_1_CopyBuffer::E2_1_CopyBuffer(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_program()
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
      glDeleteBuffers(2, m_userData.copyVboIds);
      m_userData.copyVboIds[0] = GLValues::INVALID_HANDLE;
    }
    if (m_userData.vboIds[0] == GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(2, m_userData.vboIds);
      m_userData.vboIds[0] = GLValues::INVALID_HANDLE;
    }
  }


  void E2_1_CopyBuffer::Update(const DemoTime& demoTime)
  {
  }


  void E2_1_CopyBuffer::Draw(const DemoTime& demoTime)
  {
    Point2 size = GetScreenResolution();

    glViewport(0, 0, size.X, size.Y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
    GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
    {
      -0.5f, 0.5f, 0.0f,        // v0
      .4f, 0.2f, 0.4f, 1.0f,  // c0
      -1.0f, -0.5f, 0.0f,        // v1
      0.0f, 1.0f, 0.0f, 1.0f,  // c1
      0.0f, -0.5f, 0.0f,        // v2
      0.0f, 0.0f, 1.0f, 1.0f,  // c2
    };

    // Index buffer data
    GLushort indices[3] = { 0, 1, 2 };

    // vboIds[0] - used to store vertex attribute data
    // vboIds[l] - used to store element indices
    if (m_userData.vboIds[0] == GLValues::INVALID_HANDLE && m_userData.vboIds[1] == GLValues::INVALID_HANDLE)
    {
      //OSTEP1 GENERATE SOURCE BUFFERS
      glGenBuffers(2, m_userData.vboIds);
      //OSTEP2 GENERATE DESTINATION BUFFERS
      glGenBuffers(2, m_userData.copyVboIds);

      //OSTEP3 BIND THE ORIGINAL BUFFERS AND FILL THEM WITH DATA
      glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

      //OSTEP4 BIND THE DESTINATION BUFFERS AND ALLOCATE SPACE FOR THEM
      glBindBuffer(GL_ARRAY_BUFFER, m_userData.copyVboIds[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), nullptr, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.copyVboIds[1]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), nullptr, GL_STATIC_DRAW);

      //OSTEP5 Unbind Buffers, needed to be able to execute operations on the buffers!
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      //OSTEP6 BIND TARGET AND DESTINATION BUFFERS, here we will copy Vertex Data
      glBindBuffer(GL_COPY_READ_BUFFER, m_userData.vboIds[0]);
      glBindBuffer(GL_COPY_WRITE_BUFFER, m_userData.copyVboIds[0]);

      //OSTEP7 Copy the actual Vertex Data!
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertices));

      //OSTEP8 BIND TARGET AND DESTINATION BUFFERS, here we will copy Index Data
      glBindBuffer(GL_COPY_READ_BUFFER, m_userData.vboIds[1]);
      glBindBuffer(GL_COPY_WRITE_BUFFER, m_userData.copyVboIds[1]);

      //OSTEP9 Copy the actual Index Data!
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(indices));

    }

    //USE THE COPIED BUFFERS TO DRAW
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.copyVboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.copyVboIds[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);

    intptr_t offset = 0;
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
      GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
      reinterpret_cast<const void*>(offset));

    offset += VERTEX_POS_SIZE * sizeof(GLfloat);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
      GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
      reinterpret_cast<const void*>(offset));

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

}
