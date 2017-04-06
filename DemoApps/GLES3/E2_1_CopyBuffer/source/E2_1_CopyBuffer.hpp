#ifndef GLES3_E2_1_COPYBUFFER_E2_1_COPYBUFFER_HPP
#define GLES3_E2_1_COPYBUFFER_E2_1_COPYBUFFER_HPP
/*
* OpenGL ES 3.0 Tutorial 1
*
* Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
*/

#include <FslDemoAppGLES3/DemoAppGLES3.hpp>
#include <FslGraphicsGLES3/GLProgram.hpp>

namespace Fsl
{
  class E2_1_CopyBuffer : public DemoAppGLES3
  {
    struct UserData
    {
      // VertexBufferObject Ids
      GLuint vboIds[2];
      // VertexBufferObject Ids to copy
      GLuint copyVboIds[2];
      // x-offset uniform location
      GLuint offsetLoc;

      UserData()
        : offsetLoc(0)
      {
        vboIds[0] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[1] = GLES3::GLValues::INVALID_HANDLE;
        copyVboIds[0] = GLES3::GLValues::INVALID_HANDLE;
        copyVboIds[1] = GLES3::GLValues::INVALID_HANDLE;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    E2_1_CopyBuffer(const DemoAppConfig& config);
    ~E2_1_CopyBuffer();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
