#ifndef GLES3_S04_PROJECTION_S04_PROJECTION_HPP
#define GLES3_S04_PROJECTION_S04_PROJECTION_HPP

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

#include <FslBase/Math/Matrix.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>

namespace Fsl
{
  class S04_Projection : public DemoAppGLES3
  {
    GLES3::GLProgram m_program;
    GLuint m_hModelViewMatrixLoc;
    GLuint m_hProjMatrixLoc;
    Matrix m_matProj;
    Matrix m_matTranslate1;
    Matrix m_matTranslate2;
    float m_angle;

  public:
    explicit S04_Projection(const DemoAppConfig& config);
    ~S04_Projection() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
  };
}

#endif
