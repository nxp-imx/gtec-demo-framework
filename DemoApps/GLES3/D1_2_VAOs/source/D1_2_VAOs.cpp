/*
* OpenGL ES 3.0 Tutorial 1
*
* Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
*/

#include <FslGraphicsGLES3/Exceptions.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include "D1_2_VAOs.hpp"
#include <GLES3/gl3.h>
#include <iostream>
#include <FslGraphicsGLES3/GLCheck.hpp>

namespace Fsl
{

  D1_2_VAOs::D1_2_VAOs(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_program()
    , m_vertexBuffer0()
    , m_indexBuffer0()
    , m_vertexBuffer1()
    , m_indexBuffer1()
    , m_vertexArray0()
    , m_vertexArray1()
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    { // Initialization
      // 3 vertices, with (x,y,z), (r, g, b, a) per-vertex
      VertexPositionColor vertices[3] =
      {
        VertexPositionColor(Vector3(-0.5f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),  // v0, c0
        VertexPositionColor(Vector3(-1.0f, -0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),  // v1, c1
        VertexPositionColor(Vector3(0.0f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),  // v2, c2
      };
      // Index buffer data
      GLushort indices[3] = { 0, 1, 2 };

      // 4 vertices, with (x,y,z), (r, g, b, a) per-vertex
      VertexPositionColor vertices2[4] =
      {
        VertexPositionColor(Vector3(0.0f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)), // v0, c0
        VertexPositionColor(Vector3(0.0f, -0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)), // v1, c1
        VertexPositionColor(Vector3(1.0f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)), // v2, c2
        VertexPositionColor(Vector3(1.0f, 0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)), // v3, c3
      };

      // Index buffer data
      GLushort indices2[6] = { 0, 2, 1, 0, 3, 2 };



      // Prepare the vertex and index buffer
      m_vertexBuffer0.Reset(vertices, sizeof(vertices) / sizeof(VertexPositionColor), GL_STATIC_DRAW);
      m_indexBuffer0.Reset(indices, sizeof(indices) / sizeof(GLushort), GL_STATIC_DRAW);

      // Prepare the vertex and index buffer
      m_vertexBuffer1.Reset(vertices2, sizeof(vertices2) / sizeof(VertexPositionColor), GL_STATIC_DRAW);
      m_indexBuffer1.Reset(indices2, sizeof(indices2) / sizeof(GLushort), GL_STATIC_DRAW);

      // Prepare the vertex arrays
      m_vertexArray0.Reset(true);
      m_vertexArray0.Bind();

      //Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(m_vertexBuffer0.GetTarget(), m_vertexBuffer0.Get()));
      GL_CHECK(glBindBuffer(m_indexBuffer0.GetTarget(), m_indexBuffer0.Get()));

      // - We assume that the vertex format is listed in the same order as the shader requires them.
      m_vertexBuffer0.EnableAttribArrays();

      m_vertexArray1.Reset(true);
      m_vertexArray1.Bind();

      //Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(m_vertexBuffer1.GetTarget(), m_vertexBuffer1.Get()));
      GL_CHECK(glBindBuffer(m_indexBuffer1.GetTarget(), m_indexBuffer1.Get()));
      // - We assume that the vertex format is listed in the same order as the shader requires them.
      m_vertexBuffer1.EnableAttribArrays();

      m_vertexArray1.Unbind();
    }
  }


  D1_2_VAOs::~D1_2_VAOs()
  {

  }


  void D1_2_VAOs::Update(const DemoTime& demoTime)
  {
  }


  void D1_2_VAOs::Draw(const DemoTime& demoTime)
  {
    Point2 size = GetScreenResolution();

    glViewport(0, 0, size.X, size.Y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //OSTEP5 Bind the TRIANGLE VAO
    m_vertexArray0.Bind();

    // Draw with the VAO settings
    glDrawElements(GL_TRIANGLES, 3, m_indexBuffer0.GetType(), nullptr);

    //OSTEP6 Bind the SQUARE VAO
    m_vertexArray1.Bind();

    // Draw with the VAO settings
    glDrawElements(GL_TRIANGLES, 6, m_indexBuffer1.GetType(), nullptr);

    // Return to the default VAO
    m_vertexArray1.Unbind();
  }

}
