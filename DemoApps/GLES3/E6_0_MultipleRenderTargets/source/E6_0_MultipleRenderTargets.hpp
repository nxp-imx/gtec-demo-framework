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
  class E6_0_MultipleRenderTargets : public DemoAppGLES3
  {
    struct UserData
    {
      GLuint fbo;

      // Texture handle
      std::array<GLuint, 4> colorTexId{};

      // Texture size
      GLsizei textureWidth{0};
      GLsizei textureHeight{0};

      UserData()
        : fbo(GLES3::GLValues::INVALID_HANDLE)
      {
        colorTexId[0] = GLES3::GLValues::INVALID_HANDLE;
        colorTexId[1] = GLES3::GLValues::INVALID_HANDLE;
        colorTexId[2] = GLES3::GLValues::INVALID_HANDLE;
        colorTexId[3] = GLES3::GLValues::INVALID_HANDLE;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    explicit E6_0_MultipleRenderTargets(const DemoAppConfig& config);
    ~E6_0_MultipleRenderTargets() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    int InitFBO();
    void DrawGeometry(const int w, const int h);
    void BlitTextures(const int w, const int h);
    void Cleanup();
  };
}

#endif
