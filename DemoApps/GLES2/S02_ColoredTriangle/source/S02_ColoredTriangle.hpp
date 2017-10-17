#ifndef GLES2_S02_COLOREDTRIANGLE_S02_COLOREDTRIANGLE_HPP
#define GLES2_S02_COLOREDTRIANGLE_S02_COLOREDTRIANGLE_HPP

/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * (ported to the Freescale demo framework by Freescale '14)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */

#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>

namespace Fsl
{
  class S02_ColoredTriangle : public DemoAppGLES2
  {
    GLES2::GLProgram m_program;
    GLuint m_hModelViewMatrixLoc;
    GLuint m_hProjMatrixLoc;
  public:
    S02_ColoredTriangle(const DemoAppConfig& config);
    ~S02_ColoredTriangle();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
