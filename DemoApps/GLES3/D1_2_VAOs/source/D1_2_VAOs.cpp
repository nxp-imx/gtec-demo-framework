/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include "D1_2_VAOs.hpp"
#include <FslGraphics/Vertices/VertexPositionColorF.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>

namespace Fsl
{
  D1_2_VAOs::D1_2_VAOs(const DemoAppConfig& config)
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
      constexpr std::array<VertexPositionColorF, 3> vertices = {
        VertexPositionColorF(Vector3(-0.5f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),     // v0, c0
        VertexPositionColorF(Vector3(-1.0f, -0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),    // v1, c1
        VertexPositionColorF(Vector3(0.0f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),     // v2, c2
      };
      // Index buffer data
      constexpr std::array<GLushort, 3> indices = {0, 1, 2};

      // 4 vertices, with (x,y,z), (r, g, b, a) per-vertex
      constexpr std::array<VertexPositionColorF, 4> vertices2 = {
        VertexPositionColorF(Vector3(0.0f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),     // v0, c0
        VertexPositionColorF(Vector3(0.0f, -0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),    // v1, c1
        VertexPositionColorF(Vector3(1.0f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),    // v2, c2
        VertexPositionColorF(Vector3(1.0f, 0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),     // v3, c3
      };

      // Index buffer data
      constexpr std::array<GLushort, 6> indices2 = {0, 2, 1, 0, 3, 2};


      // Prepare the vertex and index buffer
      m_vertexBuffer0.Reset(vertices.data(), vertices.size(), GL_STATIC_DRAW);
      m_indexBuffer0.Reset(indices.data(), indices.size(), GL_STATIC_DRAW);

      // Prepare the vertex and index buffer
      m_vertexBuffer1.Reset(vertices2.data(), vertices2.size(), GL_STATIC_DRAW);
      m_indexBuffer1.Reset(indices2.data(), indices2.size(), GL_STATIC_DRAW);

      // Prepare the vertex arrays
      m_vertexArray0.Reset(true);
      m_vertexArray0.Bind();

      // Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(m_vertexBuffer0.GetTarget(), m_vertexBuffer0.Get()));
      GL_CHECK(glBindBuffer(m_indexBuffer0.GetTarget(), m_indexBuffer0.Get()));

      // - We assume that the vertex format is listed in the same order as the shader requires them.
      m_vertexBuffer0.EnableAttribArrays();

      m_vertexArray1.Reset(true);
      m_vertexArray1.Bind();

      // Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(m_vertexBuffer1.GetTarget(), m_vertexBuffer1.Get()));
      GL_CHECK(glBindBuffer(m_indexBuffer1.GetTarget(), m_indexBuffer1.Get()));
      // - We assume that the vertex format is listed in the same order as the shader requires them.
      m_vertexBuffer1.EnableAttribArrays();

      m_vertexArray1.Unbind();
    }
  }


  D1_2_VAOs::~D1_2_VAOs() = default;


  void D1_2_VAOs::Update(const DemoTime& /*demoTime*/)
  {
  }


  void D1_2_VAOs::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    PxSize2D sizePx = GetWindowSizePx();

    glViewport(0, 0, sizePx.Width(), sizePx.Height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // OSTEP5 Bind the TRIANGLE VAO
    m_vertexArray0.Bind();

    // Draw with the VAO settings
    glDrawElements(GL_TRIANGLES, 3, m_indexBuffer0.GetType(), nullptr);

    // OSTEP6 Bind the SQUARE VAO
    m_vertexArray1.Bind();

    // Draw with the VAO settings
    glDrawElements(GL_TRIANGLES, 6, m_indexBuffer1.GetType(), nullptr);

    // Return to the default VAO
    m_vertexArray1.Unbind();
  }
}
