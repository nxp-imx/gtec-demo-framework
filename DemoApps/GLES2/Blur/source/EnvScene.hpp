#ifndef GLES2_BLUR_ENVSCENE_HPP
#define GLES2_BLUR_ENVSCENE_HPP

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


#include <FslUtil/OpenGLES2/GLIndexBuffer.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <FslUtil/OpenGLES2/GLTexture.hpp>
#include <FslUtil/OpenGLES2/GLVertexBuffer.hpp>
#include <Shared/EnvironmentMapping/EnvShaderInfo.hpp>
#include <Shared/EnvironmentMapping/EnvState.hpp>
#include <vector>
#include "AScene.hpp"

namespace Fsl
{
  class EnvScene : public AScene
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

  public:
    explicit EnvScene(const DemoAppConfig& config);
    ~EnvScene() override;

    void Update(const DemoTime& demoTime) override;
    void Draw() override;
  };
}

#endif
