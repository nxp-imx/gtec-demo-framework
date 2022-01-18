#ifndef GLES3_OBJECTSELECTION_OBJECTSELECTION_HPP
#define GLES3_OBJECTSELECTION_OBJECTSELECTION_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Viewport.hpp>
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>

#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLIndexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3/Draw/GLLineDraw.hpp>
#include <Shared/ObjectSelection/MenuUI.hpp>
#include <array>
#include <deque>
#include <utility>

namespace Fsl
{
  class ObjectSelection : public DemoAppGLES3
  {
    struct LightUBOData
    {
      Vector3 LightDirection;
      Vector3 LightColor;
      Vector3 AmbientColor;
    };
    struct Mesh
    {
      GLES3::GLIndexBuffer IndexBuffer;
      GLES3::GLVertexBuffer VertexBuffer;
      BoundingBox TheBoundingBox;
    };

    struct ProgramDirectionalLight
    {
      GLES3::GLProgram Program;
      GLint LocWorldView = GLES3::GLValues::INVALID_LOCATION;
      GLint LocWorldViewProjection = GLES3::GLValues::INVALID_LOCATION;
      GLint LocNormalMatrix = GLES3::GLValues::INVALID_LOCATION;
      GLint LocTexture0 = GLES3::GLValues::INVALID_LOCATION;
      GLint LocLightDirection = GLES3::GLValues::INVALID_LOCATION;
      GLint LocLightColor = GLES3::GLValues::INVALID_LOCATION;
      GLint LocAmbientColor = GLES3::GLValues::INVALID_LOCATION;
    };

    struct ProgramColor
    {
      GLES3::GLProgram Program;
      GLint LocWorldViewProjection = GLES3::GLValues::INVALID_LOCATION;
    };

    struct MeshObject
    {
      uint32_t MeshIndex = 0;
      Matrix WorldMatrix;
      Matrix WorldViewMatrix;
      Matrix WorldViewProjectionMatrix;
      Matrix3 NormalMatrix;
      float Scale = 1.0f;
      Vector3 Position;
      Vector3 Rotation;
      Vector3 RotationSpeed;
      BoundingBox MeshAABB;
    };

    struct HitRecord
    {
      std::size_t ObjectIndex = 0;
      float Distance = 0.0f;
      HitRecord() = default;

      HitRecord(const std::size_t objectIndex, const float distance)
        : ObjectIndex(objectIndex)
        , Distance(distance)
      {
      }
    };

    struct Resources
    {
      GLES3::GLTexture Texture;
      GLES3::GLTexture TextureChessboard;

      std::vector<Mesh> Meshes;
      Mesh MeshPlane;
      MeshObject PlaneObject;

      ProgramDirectionalLight ProgDirectionalLight;
      std::array<GLES3::GLVertexAttribLink, 3> AttribLink;

      VertexDeclaration VertexDeclLine;
      ProgramColor ProgramSolidColor;
      std::array<GLES3::GLVertexAttribLink, 2> AttribLinkColoredToLine;

      ProgramColor ProgramTextured;
      std::array<GLES3::GLVertexAttribLink, 2> AttribLinkTextured;

      std::vector<MeshObject> Objects;

      Graphics3D::LineBuilder LineBuild;
      GLES3::GLLineDraw LineDraw;

      Resources(const std::size_t meshCapacity, VertexDeclaration vertexDeclaration)
        : Meshes(meshCapacity)
        , VertexDeclLine(std::move(vertexDeclaration))
      {
      }
    };

    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled;
    bool m_rightMouseDown = false;
    MenuUI m_menuUI;
    Graphics3D::FirstPersonCamera m_camera;
    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;

    Resources m_resources;

    Viewport m_viewPort;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Matrix m_matrixViewProjection;

    Vector3 m_mousePositionNear;
    Vector3 m_mousePositionFar;

    std::deque<HitRecord> m_pickScratchpad;

    bool m_hasSelectedObject;
    std::size_t m_selectedIndex;

    LightUBOData m_lightFragUboData;

  public:
    explicit ObjectSelection(const DemoAppConfig& config);
    ~ObjectSelection() override;

  protected:
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnKeyEvent(const KeyEvent& event) override;

    void Update(const DemoTime& demoTime) override;
    void Draw(const FrameInfo& frameInfo) override;

  private:
    void UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState);

    bool CheckCollision(const PxPoint2& screenSpacePosition);

    void DrawMeshes();
    void DrawPlaneMesh();
    void DrawMesh(const Mesh& mesh, const bool bindMesh);

    void DrawDebugData();

    void CreateTextures(const std::shared_ptr<IContentManager>& contentManager);

    static Mesh PreparePlaneMesh(const GLES3::GLTexture& texture);
    static void PrepareMeshes(std::vector<Mesh>& rMeshes, const GLES3::GLTexture& texture);
    static Mesh CreateMesh(const Procedural::BasicMesh& mesh);
    static ProgramDirectionalLight PrepareDirectionalLightProgram(const std::shared_ptr<IContentManager>& contentManager,
                                                                  std::array<GLES3::GLVertexAttribLink, 3>& rAttribLink,
                                                                  VertexDeclarationSpan vertexDecl);
    static ProgramColor PrepareSolidColorProgram(const std::shared_ptr<IContentManager>& contentManager,
                                                 std::array<GLES3::GLVertexAttribLink, 2>& rAttribLink, VertexDeclarationSpan vertexDecl);
    static ProgramColor PrepareTexturedProgram(const std::shared_ptr<IContentManager>& contentManager,
                                               std::array<GLES3::GLVertexAttribLink, 2>& rAttribLink, VertexDeclarationSpan vertexDecl);
    static void GenerateObjects(std::vector<MeshObject>& rObjects, const uint32_t objectCount, const bool useRandomSeed);
  };
}

#endif
