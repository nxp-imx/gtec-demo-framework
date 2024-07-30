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
  // NOLINTNEXTLINE(readability-identifier-naming)
  class E4_0_PRestart : public DemoAppGLES3
  {
    struct UserData
    {
      // VertexBufferObject Ids
      GLuint VboIds[2]{};    // NOLINT(modernize-avoid-c-arrays)
      // VertexArrayObject Id
      GLuint VaoId[1]{};    // NOLINT(modernize-avoid-c-arrays)

      UserData()
      {
        VboIds[0] = GLES3::GLValues::InvalidHandle;
        VboIds[1] = GLES3::GLValues::InvalidHandle;
        VaoId[0] = GLES3::GLValues::InvalidHandle;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E4_0_PRestart(const DemoAppConfig& config);
    ~E4_0_PRestart() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void Cleanup();
  };
}

#endif
