#ifndef GLES3_E1_1_VBOS_E1_1_VBOS_HPP
#define GLES3_E1_1_VBOS_E1_1_VBOS_HPP
/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>

namespace Fsl
{
  class E1_1_VBOs : public DemoAppGLES3
  {
    struct UserData
    {
      // VertexBufferObject Ids
      GLuint vboIds[4]{};
      // x-offset uniform location
      GLuint offsetLoc{0};

      UserData()
      {
        vboIds[0] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[1] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[2] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[3] = GLES3::GLValues::INVALID_HANDLE;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    E1_1_VBOs(const DemoAppConfig& config);
    ~E1_1_VBOs() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void Cleanup();
  };
}

#endif
