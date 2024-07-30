#ifndef GLES3_E6_0_MULTIPLERENDERTARGETS_E6_0_MULTIPLERENDERTARGETS_HPP
#define GLES3_E6_0_MULTIPLERENDERTARGETS_E6_0_MULTIPLERENDERTARGETS_HPP
/*
 * OpenGL ES 3.0 Tutorial 6
 *
 * Outputs the results of the Fragment shader in Multiple Render Buffers
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <array>

namespace Fsl
{
  // NOLINTNEXTLINE(readability-identifier-naming)
  class E6_0_MultipleRenderTargets : public DemoAppGLES3
  {
    struct UserData
    {
      GLuint Fbo;

      // Texture handle
      std::array<GLuint, 4> ColorTexId{};

      // Texture size
      GLsizei TextureWidth{0};
      GLsizei TextureHeight{0};

      UserData()
        : Fbo(GLES3::GLValues::InvalidHandle)
      {
        ColorTexId[0] = GLES3::GLValues::InvalidHandle;
        ColorTexId[1] = GLES3::GLValues::InvalidHandle;
        ColorTexId[2] = GLES3::GLValues::InvalidHandle;
        ColorTexId[3] = GLES3::GLValues::InvalidHandle;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E6_0_MultipleRenderTargets(const DemoAppConfig& config);
    ~E6_0_MultipleRenderTargets() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    int InitFBO();
    void DrawGeometry(const int w, const int h);
    void BlitTextures(const int w, const int h);
    void Cleanup();
  };
}

#endif
