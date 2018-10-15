#ifndef GLES3_D1_2_VAOS_D1_2_VAOS_HPP
#define GLES3_D1_2_VAOS_D1_2_VAOS_HPP
/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLIndexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLVertexArray.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>

namespace Fsl
{
  class D1_2_VAOs : public DemoAppGLES3
  {
    GLES3::GLProgram m_program;
    GLES3::GLVertexBuffer m_vertexBuffer0;
    GLES3::GLIndexBuffer m_indexBuffer0;
    GLES3::GLVertexBuffer m_vertexBuffer1;
    GLES3::GLIndexBuffer m_indexBuffer1;
    GLES3::GLVertexArray m_vertexArray0;
    GLES3::GLVertexArray m_vertexArray1;

  public:
    D1_2_VAOs(const DemoAppConfig& config);
    ~D1_2_VAOs() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
  };
}

#endif
