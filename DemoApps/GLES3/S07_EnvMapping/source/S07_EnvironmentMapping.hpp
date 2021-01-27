#ifndef GLES3_S07_ENVMAPPING_S07_ENVIRONMENTMAPPING_HPP
#define GLES3_S07_ENVMAPPING_S07_ENVIRONMENTMAPPING_HPP

/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * (ported to the Freescale demo framework by Freescale '14)
 * (improved port to the Freescale demo framework by Freescale '15)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLIndexBuffer.hpp>

#include <Shared/EnvironmentMapping/EnvState.hpp>
#include <Shared/EnvironmentMapping/EnvShaderInfo.hpp>
#include <vector>

namespace Fsl
{
  class S07_EnvironmentMapping : public DemoAppGLES3
  {
    EnvState m_renderState;
    GLES3::GLProgram m_programGlass;
    GLES3::GLProgram m_programBgrnd;
    GLES3::GLTexture m_cubeTexture;
    GLES3::GLVertexBuffer m_vertexBuffer;
    GLES3::GLIndexBuffer m_indexBuffer;

    EnvShaderInfo m_glass;
    EnvShaderInfo m_background;

    Matrix m_transformMatrix;
    bool m_fast;

  public:
    explicit S07_EnvironmentMapping(const DemoAppConfig& config);
    ~S07_EnvironmentMapping() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
  };
}

#endif
