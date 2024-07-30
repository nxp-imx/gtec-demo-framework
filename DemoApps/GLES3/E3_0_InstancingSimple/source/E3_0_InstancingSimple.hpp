#ifndef GLES3_E3_0_INSTANCINGSIMPLE_E3_0_INSTANCINGSIMPLE_HPP
#define GLES3_E3_0_INSTANCINGSIMPLE_E3_0_INSTANCINGSIMPLE_HPP
/*
 * OpenGL ES 3.0 Tutorial 3
 *
 * Draws n number of cubes using instanced draw calls.
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NUM_INSTANCES 100

namespace Fsl
{
  // NOLINTNEXTLINE(readability-identifier-naming)
  class E3_0_InstancingSimple : public DemoAppGLES3
  {
    struct UserData
    {
      GLuint PositionVbo;
      GLuint ColorVbo;
      GLuint MvpVbo;
      GLuint IndicesIbo;

      // Number of indices
      int NumIndices{0};

      // Rotation angle
      GLfloat Angle[NUM_INSTANCES]{};    // NOLINT(modernize-avoid-c-arrays)

      UserData()
        : PositionVbo(GLES3::GLValues::InvalidHandle)
        , ColorVbo(GLES3::GLValues::InvalidHandle)
        , MvpVbo(GLES3::GLValues::InvalidHandle)
        , IndicesIbo(GLES3::GLValues::InvalidHandle)
      {
        for (int i = 0; i < NUM_INSTANCES; ++i)
        {
          Angle[i] = 0;
        }
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E3_0_InstancingSimple(const DemoAppConfig& config);
    ~E3_0_InstancingSimple() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void Cleanup();
  };
}

#endif
