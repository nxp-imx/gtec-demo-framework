#ifndef GLES3_E4_0_PRESTART_E4_0_PRESTART_HPP
#define GLES3_E4_0_PRESTART_E4_0_PRESTART_HPP
/*
 * OpenGL ES 3.0 Tutorial 4
 *
 * Primitive restart operation
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>

namespace Fsl
{
  class E4_0_PRestart : public DemoAppGLES3
  {
    struct UserData
    {
      // VertexBufferObject Ids
      GLuint vboIds[2]{};
      // VertexArrayObject Id
      GLuint vaoId[1]{};

      UserData()
      {
        vboIds[0] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[1] = GLES3::GLValues::INVALID_HANDLE;
        vaoId[0] = GLES3::GLValues::INVALID_HANDLE;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    E4_0_PRestart(const DemoAppConfig& config);
    ~E4_0_PRestart() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void Cleanup();
  };
}

#endif
