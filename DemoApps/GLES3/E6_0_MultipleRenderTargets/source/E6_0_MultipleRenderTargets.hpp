#ifndef GLES3_E6_0_MULTIPLERENDERTARGETS_E6_0_MULTIPLERENDERTARGETS_HPP
#define GLES3_E6_0_MULTIPLERENDERTARGETS_E6_0_MULTIPLERENDERTARGETS_HPP
/*
* OpenGL ES 3.0 Tutorial 6
*
* Outputs the results of the Fragment shader in Multiple Render Buffers
*/

#include <FslDemoAppGLES3/DemoAppGLES3.hpp>
#include <FslGraphicsGLES3/GLProgram.hpp>
#include <FslGraphicsGLES3/GLTexture.hpp>

namespace Fsl
{
  class E6_0_MultipleRenderTargets : public DemoAppGLES3
  {
    struct UserData
    {
      GLuint fbo;

      // Texture handle
      GLuint colorTexId[4];

      // Texture size
      GLsizei textureWidth;
      GLsizei textureHeight;

      UserData()
        : fbo(GLES3::GLValues::INVALID_HANDLE)
        , textureWidth(0)
        , textureHeight(0)
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
    E6_0_MultipleRenderTargets(const DemoAppConfig& config);
    ~E6_0_MultipleRenderTargets();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    int InitFBO();
    void DrawGeometry(const int w, const int h);
    void BlitTextures(const int w, const int h);
    void Cleanup();
  };
}

#endif
