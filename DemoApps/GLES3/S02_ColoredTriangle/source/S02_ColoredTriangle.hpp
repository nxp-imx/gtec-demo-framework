#ifndef GLES3_S02_COLOREDTRIANGLE_S02_COLOREDTRIANGLE_HPP
#define GLES3_S02_COLOREDTRIANGLE_S02_COLOREDTRIANGLE_HPP

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

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>

namespace Fsl
{
  class S02_ColoredTriangle : public DemoAppGLES3
  {
    GLES3::GLProgram m_program;
    GLint m_hModelViewMatrixLoc;
    GLint m_hProjMatrixLoc;

  public:
    explicit S02_ColoredTriangle(const DemoAppConfig& config);
    ~S02_ColoredTriangle() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;
  };
}

#endif
