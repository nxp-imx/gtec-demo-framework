#ifndef GLES3_E1_1_VBOS_E1_1_VBOS_HPP
#define GLES3_E1_1_VBOS_E1_1_VBOS_HPP
/*
 * OpenGL ES 3.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders. Using Vertex Buffer Objects
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <array>

namespace Fsl
{
  // NOLINTNEXTLINE(readability-identifier-naming)
  class E1_1_VBOs : public DemoAppGLES3
  {
    struct UserData
    {
      // VertexBufferObject Ids
      std::array<GLuint, 4> VboIds{};
      // x-offset uniform location
      GLuint OffsetLoc{0};

      UserData()
      {
        VboIds[0] = GLES3::GLValues::InvalidHandle;
        VboIds[1] = GLES3::GLValues::InvalidHandle;
        VboIds[2] = GLES3::GLValues::InvalidHandle;
        VboIds[3] = GLES3::GLValues::InvalidHandle;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E1_1_VBOs(const DemoAppConfig& config);
    ~E1_1_VBOs() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void Cleanup();
  };
}

#endif
