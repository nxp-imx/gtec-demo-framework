/*
* OpenGL ES 3.0 Tutorial 1
*
* Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
*/

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include "E1_1_VBOs.hpp"
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

  E1_1_VBOs::E1_1_VBOs(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_program()
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));


    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


    // vboIds[0] - used to store vertex attribute data
    // vboIds[l] - used to store element indices
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
        //OSTEP1 Create the Buffer Handles, one for Vertex data, and other for indices data
        GL_CHECK(glGenBuffers(2, m_userData.vboIds));

        //OSTEP2 Bind the buffer to be a Vertex Array and fill the data with it
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

        //OSTEP3 Bind the buffer to hold Indices, fill it with your index data
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]));
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        //CHALLENGE1 Create the Buffer Handles, one for Vertex data, and other for indices data
        GL_CHECK(glGenBuffers(2, &m_userData.vboIds[2]));

        //CHALLENGE2 Bind the buffer to be a Vertex Array and fill the data with it
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[2]));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW));

        //CHALLENGE3 Bind the buffer to hold Indices, fill it with your index data
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[3]));
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW));
      }
      catch (const std::exception&)
      {
        Cleanup();
        throw;
      }

    }
  }


  E1_1_VBOs::~E1_1_VBOs()
  {
    Cleanup();
  }


  void E1_1_VBOs::Update(const DemoTime& demoTime)
  {
  }


  void E1_1_VBOs::Draw(const DemoTime& demoTime)
  {
    Point2 size = GetScreenResolution();

    glViewport(0, 0, size.X, size.Y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    //OSTEP4 BIND your already filled buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[1]);

    //OSTEP5 Enable the Vertex Attributes for position and color
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);

    uintptr_t offset = 0;
    //OSTEP6 Set the pointer for Position
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          reinterpret_cast<const void*>(offset));

    offset += VERTEX_POS_SIZE * sizeof(GLfloat);

    //OSTEP7 Set the pointer for Color
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          reinterpret_cast<const void*>(offset));

    //OSTEP8 Draw the Triangle
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

    //OSTEP9 Disable the Vertex Attributes
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);

    //OSTEP10 Unbind the Buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    offset = 0;

    //BIND your already filled buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_userData.vboIds[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_userData.vboIds[3]);

    //Enable the Vertex Attributes for position and color
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);

    //Set the pointer for Position
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          reinterpret_cast<const void*>(offset));

    offset += VERTEX_POS_SIZE * sizeof(GLfloat);

    //Set the pointer for Color
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          reinterpret_cast<const void*>(offset));

    //Draw the Triangle
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    //Disable the Vertex Attributes
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);

    //Unbind the Buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }


  void E1_1_VBOs::Cleanup()
  {
    if (m_userData.vboIds[0] != GLES3::GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(2, m_userData.vboIds);
      m_userData.vboIds[0] = GLES3::GLValues::INVALID_HANDLE;
    }
    if (m_userData.vboIds[2] != GLES3::GLValues::INVALID_HANDLE)
    {
      glDeleteBuffers(2, &m_userData.vboIds[2]);
      m_userData.vboIds[2] = GLES3::GLValues::INVALID_HANDLE;
    }
  }
}
