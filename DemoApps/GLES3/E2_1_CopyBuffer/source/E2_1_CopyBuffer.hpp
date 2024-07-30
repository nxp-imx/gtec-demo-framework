#ifndef GLES3_E2_1_COPYBUFFER_E2_1_COPYBUFFER_HPP
#define GLES3_E2_1_COPYBUFFER_E2_1_COPYBUFFER_HPP
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
  class E2_1_CopyBuffer : public DemoAppGLES3
  {
    struct UserData
    {
      // VertexBufferObject Ids
      std::array<GLuint, 2> VboIds{};
      // VertexBufferObject Ids to copy
      std::array<GLuint, 2> CopyVboIds{};
      // x-offset uniform location
      GLuint OffsetLoc{0};

      UserData()
      {
        VboIds[0] = GLES3::GLValues::InvalidHandle;
        VboIds[1] = GLES3::GLValues::InvalidHandle;
        CopyVboIds[0] = GLES3::GLValues::InvalidHandle;
        CopyVboIds[1] = GLES3::GLValues::InvalidHandle;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E2_1_CopyBuffer(const DemoAppConfig& config);
    ~E2_1_CopyBuffer() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;
  };
}

#endif
