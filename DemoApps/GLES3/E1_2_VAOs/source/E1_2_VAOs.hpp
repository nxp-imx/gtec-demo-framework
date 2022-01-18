#ifndef GLES3_E1_2_VAOS_E1_2_VAOS_HPP
#define GLES3_E1_2_VAOS_E1_2_VAOS_HPP
/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <array>

namespace Fsl
{
  class E1_2_VAOs : public DemoAppGLES3
  {
    struct UserData
    {
      // VertexBufferObject Ids
      std::array<GLuint, 4> vboIds{};
      // VertexArrayObject Id
      std::array<GLuint, 2> vaoId{};

      UserData()
      {
        vboIds[0] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[1] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[2] = GLES3::GLValues::INVALID_HANDLE;
        vboIds[3] = GLES3::GLValues::INVALID_HANDLE;
        vaoId[0] = GLES3::GLValues::INVALID_HANDLE;
        vaoId[1] = GLES3::GLValues::INVALID_HANDLE;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E1_2_VAOs(const DemoAppConfig& config);
    ~E1_2_VAOs() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void Cleanup();
  };
}

#endif
