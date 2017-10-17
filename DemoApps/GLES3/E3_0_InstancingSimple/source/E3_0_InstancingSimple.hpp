#ifndef GLES3_E3_0_INSTANCINGSIMPLE_E3_0_INSTANCINGSIMPLE_HPP
#define GLES3_E3_0_INSTANCINGSIMPLE_E3_0_INSTANCINGSIMPLE_HPP
/*
* OpenGL ES 3.0 Tutorial 3
*
* Draws n number of cubes using instanced draw calls.
*/

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>

#define NUM_INSTANCES   100

namespace Fsl
{
  class E3_0_InstancingSimple : public DemoAppGLES3
  {
    struct UserData
    {
      GLuint positionVBO;
      GLuint colorVBO;
      GLuint mvpVBO;
      GLuint indicesIBO;

      // Number of indices
      int numIndices;

      // Rotation angle
      GLfloat   angle[NUM_INSTANCES];
      UserData()
        : positionVBO(GLES3::GLValues::INVALID_HANDLE)
        , colorVBO(GLES3::GLValues::INVALID_HANDLE)
        , mvpVBO(GLES3::GLValues::INVALID_HANDLE)
        , indicesIBO(GLES3::GLValues::INVALID_HANDLE)
        , numIndices(0)
      {
        for (int i = 0; i < NUM_INSTANCES; ++i)
          angle[i] = 0;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;

  public:
    E3_0_InstancingSimple(const DemoAppConfig& config);
    ~E3_0_InstancingSimple();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void Cleanup();
  };
}

#endif
