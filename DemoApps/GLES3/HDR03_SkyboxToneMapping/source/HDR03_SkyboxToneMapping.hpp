#ifndef GLES3_HDR03_SKYBOXTONEMAPPING_HDR03_SKYBOXTONEMAPPING_HPP
#define GLES3_HDR03_SKYBOXTONEMAPPING_HDR03_SKYBOXTONEMAPPING_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLVertexArray.hpp>
#include <FslUtil/OpenGLES3/GLFrameBuffer.hpp>
#include <Shared/CubeMapping/API/OpenGLES3/SkyboxMesh.hpp>
#include <Shared/CubeMapping/API/OpenGLES3/SkyboxProgram.hpp>
#include <vector>
#include <utility>
#include <Shared/HDR/SkyboxTonemapping/MenuUI.hpp>
#include <Shared/HDR/SkyboxTonemapping/OptionParser.hpp>
#include <Shared/HDR/SkyboxTonemapping/RenderRecord.hpp>

namespace Fsl
{
  class HDR03_SkyboxToneMapping : public DemoAppGLES3
  {
    struct VertexUBOData
    {
      Matrix MatModel;
      Matrix MatView;
      Matrix MatProj;
    };

    struct TonemapProgramLocations
    {
      GLint Exposure;
      TonemapProgramLocations()
        : Exposure(GLES3::GLValues::INVALID_LOCATION)
      {
      }
    };

    struct TonemapProgramInfo
    {
      GLES3::GLProgram Program;
      TonemapProgramLocations Location;
    };

    struct SimpleMesh
    {
      GLES3::GLVertexBuffer VertexBuffer;
      GLES3::GLVertexArray VertexArray;

      SimpleMesh() = default;

      SimpleMesh(GLES3::GLVertexBuffer&& vertexBuffer, GLES3::GLVertexArray&& vertexArray)
        : VertexBuffer(std::move(vertexBuffer))
        , VertexArray(std::move(vertexArray))
      {
      }
    };

    struct Scene
    {
      GLES3::GLTexture CubemapTexture;
      SkyboxProgram Program;
      SkyboxMesh Mesh;

      Scene() = default;
    };

    struct Resources
    {
      std::vector<TonemapProgramInfo> ProgramTonemap;

      Scene MainScene;

      GLES3::GLFrameBuffer HdrFrameBuffer;

      SimpleMesh MeshQuad;
    };

    MenuUI m_menuUI;

    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled;
    bool m_rightMouseDown = false;
    Graphics3D::FirstPersonCamera m_camera;

    Resources m_resources;
    VertexUBOData m_vertexUboData;

    Vector2 m_rotationSpeed;

  public:
    explicit HDR03_SkyboxToneMapping(const DemoAppConfig& config);
    ~HDR03_SkyboxToneMapping() override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void UpdateInput(const DemoTime& demoTime);
    void UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState);

    void DrawScene(const Scene& scene, const VertexUBOData& vertexUboData);
    void DrawTonemappedScene(const TonemapProgramInfo& programInfo, const GLES3::GLTextureInfo& hdrFramebufferTexInfo);
    void PrepareScene(const std::shared_ptr<IContentManager>& contentManager, Scene& rScene);

    static void CreateTonemappers(std::vector<TonemapProgramInfo>& rVector, const std::shared_ptr<IContentManager>& contentManager);
    static TonemapProgramInfo CreateTonemapShader(const std::shared_ptr<IContentManager>& contentManager, const Tonemapper::Enum tonemapper);
    SimpleMesh CreateQuadVertexArray(const GLES3::GLProgram& program);
  };
}

#endif
