#ifndef GLES3_D1_1_VBOS_D1_1_VBOS_HPP
#define GLES3_D1_1_VBOS_D1_1_VBOS_HPP
/*
* OpenGL ES 3.0 Tutorial 1
*
* Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
*/

#include <FslDemoAppGLES3/DemoAppGLES3.hpp>
#include <FslGraphicsGLES3/GLIndexBuffer.hpp>
#include <FslGraphicsGLES3/GLProgram.hpp>
#include <FslGraphicsGLES3/GLTexture.hpp>
#include <FslGraphicsGLES3/GLVertexBuffer.hpp>

namespace Fsl
{
  class D1_1_VBOs : public DemoAppGLES3
  {
    GLES3::GLProgram m_program;
    GLES3::GLVertexBuffer m_vertexBuffer0;
    GLES3::GLIndexBuffer m_indexBuffer0;
    GLES3::GLVertexBuffer m_vertexBuffer1;
    GLES3::GLIndexBuffer m_indexBuffer1;

  public:
    D1_1_VBOs(const DemoAppConfig& config);
    ~D1_1_VBOs();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
