#ifndef GLES2_S07_ENVIRONMENTMAPPING_S07_ENVIRONMENTMAPPING_HPP
#define GLES2_S07_ENVIRONMENTMAPPING_S07_ENVIRONMENTMAPPING_HPP

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

#include <FslDemoAppGLES2/DemoAppGLES2.hpp>
#include <FslGraphicsGLES2/GLProgram.hpp>
#include <FslGraphicsGLES2/GLTexture.hpp>
#include <FslGraphicsGLES2/GLVertexBuffer.hpp>
#include <FslGraphicsGLES2/GLIndexBuffer.hpp>

#include <EnvironmentMappingShared/EnvState.hpp>
#include <EnvironmentMappingShared/EnvShaderInfo.hpp>
#include <vector>

namespace Fsl
{
  class S07_EnvironmentMapping : public DemoAppGLES2
  {
    EnvState m_renderState;
    GLES2::GLProgram m_programGlass;
    GLES2::GLProgram m_programBgrnd;
    GLES2::GLTexture m_cubeTexture;
    GLES2::GLVertexBuffer m_vertexBuffer;
    GLES2::GLIndexBuffer m_indexBuffer;

    EnvShaderInfo m_glass;
    EnvShaderInfo m_background;

    Matrix m_transformMatrix;
    bool m_fast;
  public:
    S07_EnvironmentMapping(const DemoAppConfig& config);
    ~S07_EnvironmentMapping();

  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
