#ifndef GLES3_E7_0_PARTICLESYSTEM_E7_0_PARTICLESYSTEM_HPP
#define GLES3_E7_0_PARTICLESYSTEM_E7_0_PARTICLESYSTEM_HPP
/*
* OpenGL ES 3.0 Tutorial 7
*
* Draws a Particle system
*/


#include <FslDemoAppGLES3/DemoAppGLES3.hpp>
#include <FslGraphicsGLES3/GLProgram.hpp>
#include <FslGraphicsGLES3/GLTexture.hpp>

#define NUM_PARTICLES   2000
#define PARTICLE_SIZE   7

namespace Fsl
{
  class E7_0_ParticleSystem : public DemoAppGLES3
  {
    struct UserData
    {
      //
      GLint lifetimeLoc;
      GLint startPositionLoc;
      GLint endPositionLoc;
      GLint timeLoc;
      GLint colorLoc;
      GLint centerPositionLoc;
      GLint samplerLoc;

      // Texture handle
      GLES3::GLTexture Texture;

      // Particle vertex data
      float particleData[NUM_PARTICLES * PARTICLE_SIZE];

      // Current time
      float time;

      UserData()
        : lifetimeLoc(0)
        , startPositionLoc(0)
        , endPositionLoc(0)
        , timeLoc(0)
        , colorLoc(0)
        , centerPositionLoc(0)
        , samplerLoc(0)
        , Texture()
        , time(1.0f) // 1.0f to reset the system
      {
        for (int i = 0; i < NUM_PARTICLES * PARTICLE_SIZE; ++i)
          particleData[i] = 0.0f;
      }
    };

    GLES3::GLProgram m_program;
    UserData m_userData;
    bool m_bIsInitialized;

  public:
    E7_0_ParticleSystem(const DemoAppConfig& config);
    ~E7_0_ParticleSystem();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
