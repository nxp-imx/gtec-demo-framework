/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include <FslBase/Math/Vector3.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include "D1_1_VBOs.hpp"
#include <GLES3/gl3.h>
#include <iostream>

namespace Fsl
{
  D1_1_VBOs::D1_1_VBOs(const DemoAppConfig& config)
    : DemoAppGLES3(config)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"));

    const GLuint hProgram = m_program.Get();

    GL_CHECK(glUseProgram(hProgram));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    {    // Initialization
      // 3 vertices, with (x,y,z), (r, g, b, a) per-vertex
      VertexPositionColor vertices[3] = {
        VertexPositionColor(Vector3(-0.5f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),     // v0, c0
        VertexPositionColor(Vector3(-1.0f, -0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),    // v1, c1
        VertexPositionColor(Vector3(0.0f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),     // v2, c2
      };
      // Index buffer data
      GLushort indices[3] = {0, 1, 2};

      // 4 vertices, with (x,y,z), (r, g, b, a) per-vertex
      VertexPositionColor vertices2[4] = {
        VertexPositionColor(Vector3(0.0f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),     // v0, c0
        VertexPositionColor(Vector3(0.0f, -0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),    // v1, c1
        VertexPositionColor(Vector3(1.0f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),    // v2, c2
        VertexPositionColor(Vector3(1.0f, 0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),     // v3, c3
      };

      // Index buffer data
      GLushort indices2[6] = {0, 2, 1, 0, 3, 2};

      // Prepare the vertex and index buffer
      m_vertexBuffer0.Reset(vertices, sizeof(vertices) / sizeof(VertexPositionColor), GL_STATIC_DRAW);
      m_indexBuffer0.Reset(indices, sizeof(indices) / sizeof(GLushort), GL_STATIC_DRAW);

      // Prepare the vertex and index buffer
      m_vertexBuffer1.Reset(vertices2, sizeof(vertices2) / sizeof(VertexPositionColor), GL_STATIC_DRAW);
      m_indexBuffer1.Reset(indices2, sizeof(indices2) / sizeof(GLushort), GL_STATIC_DRAW);
    }
  }


  D1_1_VBOs::~D1_1_VBOs() = default;


  void D1_1_VBOs::Update(const DemoTime& demoTime)
  {
  }


  void D1_1_VBOs::Draw(const DemoTime& demoTime)
  {
    const Point2 size = GetScreenResolution();

    glViewport(0, 0, size.X, size.Y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // OSTEP4 BIND your already filled buffers
    glBindBuffer(m_vertexBuffer0.GetTarget(), m_vertexBuffer0.Get());
    glBindBuffer(m_indexBuffer0.GetTarget(), m_indexBuffer0.Get());

    // OSTEP5 Enable the Vertex Attributes for position and color
    // - We utilize the fact that the vertex format is ordered in the same was as the shader requires them.
    m_vertexBuffer0.EnableAttribArrays();

    // OSTEP8 Draw the Triangle
    glDrawElements(GL_TRIANGLES, 3, m_indexBuffer0.GetType(), nullptr);

    // Disable the attribs arrays that we enabled above (using the corresponding disable method)
    m_vertexBuffer0.DisableAttribArrays();

    // OSTEP10 Unbind the Buffers
    glBindBuffer(m_vertexBuffer0.GetTarget(), 0);
    glBindBuffer(m_indexBuffer0.GetTarget(), 0);


    // CHALLENGE4 BIND your already filled buffers
    // - We assume that the vertex format is listed in the same order as the shader requires them.
    glBindBuffer(m_vertexBuffer1.GetTarget(), m_vertexBuffer1.Get());
    glBindBuffer(m_indexBuffer1.GetTarget(), m_indexBuffer1.Get());

    // CHALLENGE5 Enable the Vertex Attributes for position and color
    // We utilize the fact that the vertex format is ordered in the same was as the shader requires them.
    m_vertexBuffer1.EnableAttribArrays();

    // CHALLENGE8 Draw the Triangle
    glDrawElements(GL_TRIANGLES, 6, m_indexBuffer1.GetType(), nullptr);

    // Disable the attribs arrays that we enabled above (using the corresponding disable method)
    m_vertexBuffer1.DisableAttribArrays();

    // CHALLENGE10 Unbind the Buffers
    glBindBuffer(m_vertexBuffer1.GetTarget(), 0);
    glBindBuffer(m_indexBuffer1.GetTarget(), 0);
  }
}
