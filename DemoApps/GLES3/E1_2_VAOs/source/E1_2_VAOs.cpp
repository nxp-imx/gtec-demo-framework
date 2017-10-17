/*
* OpenGL ES 3.0 Tutorial 1
*
* Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
*/

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include "E1_2_VAOs.hpp"
#include <GLES3/gl3.h>
#include <iostream>
#include <FslUtil/OpenGLES3/GLCheck.hpp>

//Attribute Arrays Indexes and Sizes
#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_STRIDE         ( sizeof(GLfloat) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ) )

namespace Fsl
{
  using namespace GLES3;

  E1_2_VAOs::E1_2_VAOs(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_program()
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));


    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));

    // vboIds[0] - used to store vertex attribute data
    // vboIds[l] - used to store element indices
    // vboIds[2] - used to store vertex attribute data
    // vboIds[3] - used to store element indices
    { // Initialization
      // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
      GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
      {
        -0.5f, 0.5f, 0.0f,        // v0
        1.0f, 0.0f, 0.0f, 1.0f,  // c0
        -1.0f, -0.5f, 0.0f,        // v1
        0.0f, 1.0f, 0.0f, 1.0f,  // c1
        0.0f, -0.5f, 0.0f,        // v2
        0.0f, 0.0f, 1.0f, 1.0f,  // c2
      };
      // Index buffer data
      GLushort indices[3] = { 0, 1, 2 };

      // 4 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
      GLfloat vertices2[4 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
      {
        0.0f, 0.5f, 0.0f,        // v0
        1.0f, 0.0f, 0.0f, 1.0f,  // c0
        0.0f, -0.5f, 0.0f,        // v1
        0.0f, 1.0f, 0.0f, 1.0f,  // c1
        1.0f, -0.5f, 0.0f,        // v2
        0.0f, 0.0f, 1.0f, 1.0f,  // c2
        1.0f, 0.5f, 0.0f,        // v3
        0.0f, 0.0f, 1.0f, 1.0f,  // c3
      };
      // Index buffer data
      GLushort indices2[6] = { 0, 2, 1, 0, 3, 2 };


      try
      {
        //Create the Buffer Handles, one for Vertex data, and other for indices data
        GL_CHECK(glGenBuffers(2, m_userData.vboIds));

        //Bind the buffer to be a Vertex Array and fill the data with it
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

        //Bind the buffer to hold Indices, fill it with your index data
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]));
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        //Create the Buffer Handles, one for Vertex data, and other for indices data
        GL_CHECK(glGenBuffers(2, &m_userData.vboIds[2]));

        //Bind the buffer to be a Vertex Array and fill the data with it
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[2]));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW));

        //Bind the buffer to hold Indices, fill it with your index data
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[3]));
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW));

        //OSTEP1 Generate VAO Id
        GL_CHECK(glGenVertexArrays(1, &m_userData.vaoId[0]));

        //OSTEP2 Bind the VAO
        GL_CHECK(glBindVertexArray(m_userData.vaoId[0]));

        //Set up VBO Vertex Attribute information
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]));
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]));

        GL_CHECK(glEnableVertexAttribArray(VERTEX_POS_INDX));
        GL_CHECK(glEnableVertexAttribArray(VERTEX_COLOR_INDX));

        GL_CHECK(glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)0));
        GL_CHECK(glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
          (const void *)(VERTEX_POS_SIZE * sizeof(GLfloat))));

        //OSTEP3 Generate VAO Id
        GL_CHECK(glGenVertexArrays(1, &m_userData.vaoId[1]));

        //OSTEP4 Bind the VAO
        GL_CHECK(glBindVertexArray(m_userData.vaoId[1]));

        //Set up VBO Vertex Attribute information
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[2]));
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[3]));

        GL_CHECK(glEnableVertexAttribArray(VERTEX_POS_INDX));
        GL_CHECK(glEnableVertexAttribArray(VERTEX_COLOR_INDX));

        GL_CHECK(glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)0));
        GL_CHECK(glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)(VERTEX_POS_SIZE * sizeof(GLfloat))));

        // Restore the VAO Id to the default
        GL_CHECK(glBindVertexArray(0));

      }
      catch (const std::exception&)
      {
        Cleanup();
        throw;
      }
    }
  }


  E1_2_VAOs::~E1_2_VAOs()
  {
    Cleanup();
  }


  void E1_2_VAOs::Update(const DemoTime& demoTime)
  {
  }


  void E1_2_VAOs::Draw(const DemoTime& demoTime)
  {
    Point2 size = GetScreenResolution();

    glViewport(0, 0, size.X, size.Y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //OSTEP5 Bind the TRIANGLE VAO
    glBindVertexArray(m_userData.vaoId[0]);
    // Draw with the VAO settings
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

    //OSTEP6 Bind the SQUARE VAO
    glBindVertexArray(m_userData.vaoId[1]);

    // Draw with the VAO settings
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    // Return to the default VAO
    glBindVertexArray(0);
  }


  void E1_2_VAOs::Cleanup()
  {
    if (m_userData.vaoId[0] != GLValues::INVALID_HANDLE)
    {
      glDeleteVertexArrays(1, &m_userData.vaoId[0]);
      m_userData.vaoId[0] = GLValues::INVALID_HANDLE;
    }

    if (m_userData.vaoId[1] != GLValues::INVALID_HANDLE)
    {
      glDeleteVertexArrays(1, &m_userData.vaoId[1]);
      m_userData.vaoId[1] = GLValues::INVALID_HANDLE;
    }

    if (m_userData.vboIds[0] != GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(2, m_userData.vboIds);
      m_userData.vboIds[0] = GLValues::INVALID_HANDLE;
    }
    if (m_userData.vboIds[2] != GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(2, &m_userData.vboIds[2]);
      m_userData.vboIds[2] = GLValues::INVALID_HANDLE;
    }
  }

}
