#ifndef GLES3_S01_SIMPLETRIANGLE_S01_SIMPLETRIANGLE_HPP
#define GLES3_S01_SIMPLETRIANGLE_S01_SIMPLETRIANGLE_HPP

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

#include <FslDemoAppGLES3/DemoAppGLES3.hpp>
#include <FslGraphicsGLES3/GLProgram.hpp>

namespace Fsl
{
  class S01_SimpleTriangle : public DemoAppGLES3
  {
    GLES3::GLProgram m_program;
    GLuint m_hModelViewMatrixLoc;
    GLuint m_hProjMatrixLoc;
  public:
    S01_SimpleTriangle(const DemoAppConfig& config);
    ~S01_SimpleTriangle();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
