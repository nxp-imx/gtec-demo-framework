/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include "ObjectSelection.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Math/Viewport.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslGraphics/Vertices/VertexPositionColorF.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics3D/Procedural/BoxGenerator.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslGraphics3D/Procedural/TorusGenerator.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/TextureUtil.hpp>
#include <Shared/ObjectSelection/BoundingBoxUtil.hpp>
#include <Shared/ObjectSelection/OptionParser.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <cmath>
#include <random>


namespace Fsl
{
  using namespace GLES3;
  using namespace Procedural;

  namespace
  {
    const float DEFAULT_ZOOM = 10;
  }


  ObjectSelection::ObjectSelection(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_menuUI(config)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_resources(3, VertexDeclaration(VertexPositionColorF::AsVertexDeclarationSpan()))
    , m_hasSelectedObject(false)
    , m_selectedIndex(0)
  {
    m_lightFragUboData.LightDirection = {0.0f, 1.0f, 1.0};
    m_lightFragUboData.LightColor = {0.8f, 0.8f, 0.8f};
    m_lightFragUboData.AmbientColor = {0.2f, 0.2f, 0.2f};

    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    m_camera.SetPosition(Vector3(0, 0, DEFAULT_ZOOM), Vector3(), Vector3::Up());

    m_lightFragUboData.LightDirection.Normalize();

    const auto optionParser = config.GetOptions<OptionParser>();

    auto contentManager = GetContentManager();

    CreateTextures(contentManager);

    m_resources.MeshPlane = PreparePlaneMesh(m_resources.TextureChessboard);
    PrepareMeshes(m_resources.Meshes, m_resources.Texture);

    m_resources.ProgDirectionalLight =
      PrepareDirectionalLightProgram(contentManager, m_resources.AttribLink, VertexPositionNormalTexture::AsVertexDeclarationSpan());
    m_resources.ProgramSolidColor =
      PrepareSolidColorProgram(contentManager, m_resources.AttribLinkColoredToLine, m_resources.VertexDeclLine.AsSpan());
    m_resources.ProgramTextured =
      PrepareTexturedProgram(contentManager, m_resources.AttribLinkTextured, VertexPositionNormalTexture::AsVertexDeclarationSpan());

    GenerateObjects(m_resources.Objects, optionParser->GetObjectCount(), m_menuUI.IsRandomSeedEnabled());

    m_resources.PlaneObject.Rotation = Vector3(-MathHelper::ToRadians(90), 0, 0);
    m_resources.PlaneObject.Position = Vector3(0, -8, 0);
  }


  ObjectSelection::~ObjectSelection() = default;


  void ObjectSelection::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    if (event.IsPressed() && event.GetButton() == VirtualMouseButton::Left)
    {
      if (CheckCollision(event.GetPosition()))
      {
        event.Handled();
      }
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
      {
        m_rightMouseDown = event.IsPressed();
        if (m_demoAppControl->TryEnableMouseCaptureMode(m_rightMouseDown))
        {
          m_mouseCaptureEnabled = m_rightMouseDown;
        }
        else
        {
          m_mouseCaptureEnabled = false;
        }
        event.Handled();
        break;
      }
    case VirtualMouseButton::Middle:
      if (event.IsPressed())
      {
        m_camera.SetPosition(Vector3(0, 0, DEFAULT_ZOOM), Vector3(), Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void ObjectSelection::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);

    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
  }


  void ObjectSelection::Update(const DemoTime& demoTime)
  {
    const PxSize2D windowSizePx = GetWindowSizePx();

    m_viewPort = Viewport(Rectangle(0, 0, windowSizePx.Width(), windowSizePx.Height()));

    m_matrixView = m_camera.GetViewMatrix();
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), m_viewPort.GetAspectRatio(), 0.1f, 500.0f);
    m_matrixViewProjection = m_matrixView * m_matrixProjection;

    for (auto& rEntry : m_resources.Objects)
    {
      rEntry.Rotation.X += rEntry.RotationSpeed.X * demoTime.DeltaTime;
      rEntry.Rotation.Y += rEntry.RotationSpeed.Y * demoTime.DeltaTime;
      rEntry.Rotation.Z += rEntry.RotationSpeed.Z * demoTime.DeltaTime;
      rEntry.WorldMatrix = Matrix::CreateScale(rEntry.Scale) * Matrix::CreateRotationX(rEntry.Rotation.X) *
                           Matrix::CreateRotationY(rEntry.Rotation.Y) * Matrix::CreateRotationZ(rEntry.Rotation.Z) *
                           Matrix::CreateTranslation(rEntry.Position);
      rEntry.WorldViewMatrix = rEntry.WorldMatrix * m_matrixView;
      rEntry.WorldViewProjectionMatrix = rEntry.WorldViewMatrix * m_matrixProjection;

      rEntry.NormalMatrix = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(rEntry.WorldViewMatrix)));

      rEntry.MeshAABB = BoundingBoxUtil::CalculateAABB(rEntry.WorldMatrix, m_resources.Meshes[rEntry.MeshIndex].TheBoundingBox);
    }

    {
      m_resources.PlaneObject.WorldMatrix =
        Matrix::CreateRotationX(m_resources.PlaneObject.Rotation.X) * Matrix::CreateTranslation(m_resources.PlaneObject.Position);
      m_resources.PlaneObject.WorldViewMatrix = m_resources.PlaneObject.WorldMatrix * m_matrixView;
      m_resources.PlaneObject.WorldViewProjectionMatrix = m_resources.PlaneObject.WorldViewMatrix * m_matrixProjection;

      m_resources.PlaneObject.NormalMatrix = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_resources.PlaneObject.WorldViewMatrix)));

      m_resources.PlaneObject.MeshAABB = BoundingBoxUtil::CalculateAABB(m_resources.PlaneObject.WorldMatrix, m_resources.MeshPlane.TheBoundingBox);
    }

    auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
  }


  void ObjectSelection::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (m_menuUI.IsPlaneEnabled())
    {
      DrawPlaneMesh();
    }
    DrawMeshes();
    DrawDebugData();

    // GL_CHECK_FOR_ERROR();

    // Draw the UI overlay
    m_menuUI.Draw();

    // Disable everything
    glUseProgram(0);
  }


  void ObjectSelection::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      Vector3 sourcePos(static_cast<float>(mouseState.Position.X), static_cast<float>(mouseState.Position.Y), 0.0f);
      m_mousePositionNear = m_viewPort.Unproject(sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
      sourcePos.Z = 1.0f;
      m_mousePositionFar = m_viewPort.Unproject(sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
      // FSLLOG3_INFO("2D: X: " << mouseState.Position.X << " Y: " << mouseState.Position.Y  << " 3D: X: " << m_mousePositionFar.X << " Y: " <<
      // m_mousePositionFar.Y << " Z: " << m_mousePositionFar.Z);


      if (!m_rightMouseDown)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X, -mouseState.RawPosition.Y);
          m_camera.Rotate(rawPosition);
        }
      }
    }

    // Keyboard camera movement
    const float movementSpeed = 32.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::W))
    {
      m_camera.MoveForward(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::S))
    {
      m_camera.MoveBackwards(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::A))
    {
      m_camera.MoveLeft(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::D))
    {
      m_camera.MoveRight(movementSpeed);
    }
  }


  bool ObjectSelection::CheckCollision(const PxPoint2& screenSpacePosition)
  {
    Vector3 sourcePos(static_cast<float>(screenSpacePosition.X), static_cast<float>(screenSpacePosition.Y), 0.0f);

    // Unproject a point on the near and far plane
    const auto nearPoint = m_viewPort.Unproject(sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
    sourcePos.Z = 1.0f;
    const auto farPoint = m_viewPort.Unproject(sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());

    // Build a ray
    Vector3 direction = farPoint - nearPoint;
    direction.Normalize();
    Ray mouseRay(nearPoint, direction);

    // Check which AABB's the ray intersect and store the hits in m_pickScratchpad
    m_pickScratchpad.clear();
    for (std::size_t i = 0; i < m_resources.Objects.size(); ++i)
    {
      float distance = 0.0f;
      if (mouseRay.Intersects(m_resources.Objects[i].MeshAABB, distance))
      {
        m_pickScratchpad.emplace_back(i, distance);
      }
    }

    // Check if there is any stored hits
    m_hasSelectedObject = !m_pickScratchpad.empty();
    if (m_hasSelectedObject)
    {
      // Find the one with the shortest distance
      float distance = std::numeric_limits<float>::max();
      for (const auto& entry : m_pickScratchpad)
      {
        if (entry.Distance < distance)
        {
          distance = entry.Distance;
          m_selectedIndex = entry.ObjectIndex;
        }
      }
    }
    return m_hasSelectedObject;
  }


  void ObjectSelection::DrawMeshes()
  {
    // Instance based rendering would be simpler and faster, but this code now matches how it was done for the GLES3 sample

    glUseProgram(m_resources.ProgDirectionalLight.Program.Get());

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);

    glUniform3fv(m_resources.ProgDirectionalLight.LocLightDirection, 1, m_lightFragUboData.LightDirection.DirectAccess());
    glUniform3fv(m_resources.ProgDirectionalLight.LocLightColor, 1, m_lightFragUboData.LightColor.DirectAccess());
    glUniform3fv(m_resources.ProgDirectionalLight.LocAmbientColor, 1, m_lightFragUboData.AmbientColor.DirectAccess());

    // Enable the attribs the meshes use once (since we use the same mesh layout for everything)
    for (std::size_t i = 0; i < m_resources.AttribLink.size(); ++i)
    {
      if (m_resources.AttribLink[i].AttribIndex >= 0)
      {
        glEnableVertexAttribArray(m_resources.AttribLink[i].AttribIndex);
      }
    }

    // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
    if (m_menuUI.IsObjectsEnabled())
    {
      glBindTexture(GL_TEXTURE_2D, m_resources.Texture.Get());

      const bool forceBind = m_menuUI.IsForceBindEnabled();
      uint32_t oldMeshIndex = std::numeric_limits<uint32_t>::max();
      for (auto& rEntry : m_resources.Objects)
      {
        // Load the matrices
        if (m_resources.ProgDirectionalLight.LocWorldView != GLValues::INVALID_LOCATION)
        {
          glUniformMatrix4fv(m_resources.ProgDirectionalLight.LocWorldView, 1, 0, rEntry.WorldViewMatrix.DirectAccess());
        }
        glUniformMatrix4fv(m_resources.ProgDirectionalLight.LocWorldViewProjection, 1, 0, rEntry.WorldViewProjectionMatrix.DirectAccess());
        glUniformMatrix3fv(m_resources.ProgDirectionalLight.LocNormalMatrix, 1, 0, rEntry.NormalMatrix.DirectAccess());

        DrawMesh(m_resources.Meshes[rEntry.MeshIndex], (forceBind || oldMeshIndex != rEntry.MeshIndex));
        oldMeshIndex = rEntry.MeshIndex;
      }
    }

    // Disable stuff
    glBindBuffer(m_resources.Meshes[0].IndexBuffer.GetTarget(), 0);
    glBindBuffer(m_resources.Meshes[0].VertexBuffer.GetTarget(), 0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }


  void ObjectSelection::DrawPlaneMesh()
  {
    glUseProgram(m_resources.ProgramTextured.Program.Get());

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_resources.TextureChessboard.Get());

    // Load the matrix
    glUniformMatrix4fv(m_resources.ProgramTextured.LocWorldViewProjection, 1, 0, m_resources.PlaneObject.WorldViewProjectionMatrix.DirectAccess());

    // Bind and enable the vertex buffer
    glBindBuffer(m_resources.MeshPlane.VertexBuffer.GetTarget(), m_resources.MeshPlane.VertexBuffer.Get());
    glBindBuffer(m_resources.MeshPlane.IndexBuffer.GetTarget(), m_resources.MeshPlane.IndexBuffer.Get());

    m_resources.MeshPlane.VertexBuffer.EnableAttribArrays(m_resources.AttribLinkTextured);

    glDrawElements(GL_TRIANGLE_STRIP, m_resources.MeshPlane.IndexBuffer.GetGLCapacity(), m_resources.MeshPlane.IndexBuffer.GetType(), nullptr);

    glBindBuffer(m_resources.MeshPlane.IndexBuffer.GetTarget(), 0);
    glBindBuffer(m_resources.MeshPlane.VertexBuffer.GetTarget(), 0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }


  void ObjectSelection::DrawMesh(const Mesh& mesh, const bool bindMesh)
  {
    const auto indexBufferType = mesh.IndexBuffer.GetType();
    if (mesh.IndexBuffer.GetCapacity() <= 0)
    {
      return;
    }

    if (bindMesh)
    {
      // Bind and enable the vertex buffer
      glBindBuffer(mesh.VertexBuffer.GetTarget(), mesh.VertexBuffer.Get());
      glBindBuffer(mesh.IndexBuffer.GetTarget(), mesh.IndexBuffer.Get());

      // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
      mesh.VertexBuffer.SetVertexAttribPointers(m_resources.AttribLink);
    }

    glDrawElements(GL_TRIANGLE_STRIP, mesh.IndexBuffer.GetGLCapacity(), indexBufferType, nullptr);
  }


  void ObjectSelection::DrawDebugData()
  {
    m_resources.LineBuild.Clear();

    // Load the matrices
    if (m_menuUI.IsDrawOrientedBoundingBoxEnabled())
    {
      for (auto& rEntry : m_resources.Objects)
      {
        m_resources.LineBuild.Add(m_resources.Meshes[rEntry.MeshIndex].TheBoundingBox, Color::Red(), rEntry.WorldMatrix);
      }
    }

    if (m_menuUI.IsDrawAxisAlignedBoundingBoxEnabled())
    {
      for (std::size_t i = 0; i < m_resources.Objects.size(); ++i)
      {
        if (i != m_selectedIndex || !m_hasSelectedObject)
        {
          m_resources.LineBuild.Add(m_resources.Objects[i].MeshAABB, Color::Blue());
        }
      }
    }
    if (m_hasSelectedObject)
    {
      m_resources.LineBuild.Add(m_resources.Objects[m_selectedIndex].MeshAABB, Color::White());
    }

    // Draw bounding boxes

    {
      // DrawLine(m_mousePositionNear, m_mousePositionFar, Color::Pink());

      if (m_menuUI.IsDrawNearPlaneMouseEnabled())
      {
        const float magic = 0.001f;
        m_resources.LineBuild.Add(m_mousePositionNear + Vector3(-magic, -magic, 0.0f), m_mousePositionNear + Vector3(Vector3(magic, magic, 0.0f)),
                                  Color::Pink());
        m_resources.LineBuild.Add(m_mousePositionNear + Vector3(magic, -magic, 0.0f), m_mousePositionNear + Vector3(Vector3(-magic, magic, 0.0f)),
                                  Color::Pink());
      }
      if (m_menuUI.IsDrawFarPlaneMouseEnabled())
      {
        const float magic = 5.0f;
        m_resources.LineBuild.Add(m_mousePositionFar + Vector3(-magic, -magic, 1.0f), m_mousePositionFar + Vector3(Vector3(magic, magic, 1.0f)),
                                  Color::Cyan());
        m_resources.LineBuild.Add(m_mousePositionFar + Vector3(magic, -magic, 1.0f), m_mousePositionFar + Vector3(Vector3(-magic, magic, 1.0f)),
                                  Color::Cyan());
      }
    }

    if (!m_resources.LineBuild.IsEmpty())
    {
      glUseProgram(m_resources.ProgramSolidColor.Program.Get());
      glUniformMatrix4fv(m_resources.ProgramSolidColor.LocWorldViewProjection, 1, 0, m_matrixViewProjection.DirectAccess());

      m_resources.LineDraw.Draw(m_resources.LineBuild.GetVertexSpan(), m_resources.AttribLinkColoredToLine);
    }
  }


  void ObjectSelection::CreateTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    auto texture = contentManager->ReadTexture("SeamlessFur.png", PixelFormat::R8G8B8A8_UNORM);
    GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    m_resources.Texture.Reset(texture, texParams, TextureFlags::GenerateMipMaps);

    texture = contentManager->ReadTexture("MarbleChessboard.jpg", PixelFormat::R8G8B8A8_UNORM);
    m_resources.TextureChessboard.Reset(texture, texParams, TextureFlags::GenerateMipMaps);
  }


  ObjectSelection::Mesh ObjectSelection::PreparePlaneMesh(const GLES3::GLTexture& texture)
  {
    const auto tex1Size = texture.GetSize();
    TextureRectangle texRect(PxRectangle(0, 0, tex1Size.Width(), tex1Size.Height()), tex1Size);
    const NativeTextureArea texRepeatArea(TextureUtil::CalcTextureArea(texRect, 15 / 5, 15 / 5));
    const auto mesh = SegmentedQuadGenerator::GenerateStrip(Vector3(0, 0, 0), 1000 / 5.0f, 1000 / 5.0f, 1, 1, texRepeatArea, WindingOrder::CCW);
    return CreateMesh(mesh);
  }


  void ObjectSelection::PrepareMeshes(std::vector<Mesh>& rMeshes, const GLES3::GLTexture& texture)
  {
    const auto tex1Size = texture.GetSize();
    TextureRectangle texRect(PxRectangle(0, 0, tex1Size.Width(), tex1Size.Height()), tex1Size);

    const NativeTextureArea texArea(TextureUtil::CalcTextureArea(texRect));

    auto mesh = TorusGenerator::GenerateStrip(16, 16, 2, 0.5f, texArea, WindingOrder::CCW);
    rMeshes[0] = CreateMesh(mesh);

    mesh = TorusGenerator::GenerateStrip(3, 3, 2, 0.5f, texArea, WindingOrder::CCW);
    rMeshes[1] = CreateMesh(mesh);

    const std::array<NativeTextureArea, 6> texAreas = {texArea, texArea, texArea, texArea, texArea, texArea};
    mesh =
      BoxGenerator::GenerateStrip(Vector3(), 2.0f, 2.0f, 2.0f, texAreas.data(), UncheckedNumericCast<int32_t>(texAreas.size()), WindingOrder::CCW);
    rMeshes[2] = CreateMesh(mesh);
  }


  ObjectSelection::Mesh ObjectSelection::CreateMesh(const Procedural::BasicMesh& mesh)
  {
    Mesh newMesh;
    newMesh.IndexBuffer.Reset(mesh.GetIndexArray(), GL_STATIC_DRAW);
    newMesh.VertexBuffer.Reset(mesh.GetVertexArray(), GL_STATIC_DRAW);
    newMesh.TheBoundingBox = BoundingBoxUtil::CalculateBoundingBox(mesh.GetVertexArray());
    return newMesh;
  }


  ObjectSelection::ProgramDirectionalLight ObjectSelection::PrepareDirectionalLightProgram(const std::shared_ptr<IContentManager>& contentManager,
                                                                                           std::array<GLES3::GLVertexAttribLink, 3>& rAttribLink,
                                                                                           VertexDeclarationSpan vertexDecl)
  {
    ProgramDirectionalLight newProgram;
    newProgram.Program.Reset(contentManager->ReadAllText("BasicShaderDLight.vert"), contentManager->ReadAllText("BasicShaderDLightTextured.frag"));

    const auto& program = newProgram.Program;
    newProgram.LocWorldView = program.TryGetUniformLocation("WorldView");
    newProgram.LocWorldViewProjection = program.GetUniformLocation("WorldViewProjection");
    newProgram.LocNormalMatrix = program.GetUniformLocation("NormalMatrix");
    newProgram.LocTexture0 = program.GetUniformLocation("Texture0");
    newProgram.LocLightDirection = program.GetUniformLocation("LightDirection");
    newProgram.LocLightColor = program.GetUniformLocation("LightColor");
    newProgram.LocAmbientColor = program.GetUniformLocation("AmbientColor");

    // Attrib link between shader and vertex format
    rAttribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    rAttribLink[1] = GLVertexAttribLink(program.GetAttribLocation("VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    rAttribLink[2] =
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    return newProgram;
  }


  ObjectSelection::ProgramColor ObjectSelection::PrepareSolidColorProgram(const std::shared_ptr<IContentManager>& contentManager,
                                                                          std::array<GLES3::GLVertexAttribLink, 2>& rAttribLink,
                                                                          VertexDeclarationSpan vertexDecl)
  {
    ProgramColor newProgram;
    newProgram.Program.Reset(contentManager->ReadAllText("VertexColored.vert"), contentManager->ReadAllText("VertexColored.frag"));

    const auto& program = newProgram.Program;

    newProgram.LocWorldViewProjection = program.GetUniformLocation("WorldViewProjection");

    // Attrib link between shader and vertex format
    rAttribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    rAttribLink[1] = GLVertexAttribLink(program.GetAttribLocation("VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
    return newProgram;
  }


  ObjectSelection::ProgramColor ObjectSelection::PrepareTexturedProgram(const std::shared_ptr<IContentManager>& contentManager,
                                                                        std::array<GLES3::GLVertexAttribLink, 2>& rAttribLink,
                                                                        VertexDeclarationSpan vertexDecl)
  {
    ProgramColor newProgram;
    // Prepare the texture program
    newProgram.Program.Reset(contentManager->ReadAllText("Textured.vert"), contentManager->ReadAllText("Textured.frag"));
    const auto& program = newProgram.Program;

    newProgram.LocWorldViewProjection = program.GetUniformLocation("WorldViewProjection");

    // Attrib link between shader and vertex format
    rAttribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    rAttribLink[1] =
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    return newProgram;
  }


  void ObjectSelection::GenerateObjects(std::vector<MeshObject>& rObjects, const uint32_t objectCount, const bool useRandomSeed)
  {
    // Distribute the objects at random positions, scale and rotation
    rObjects.resize(objectCount);

    std::mt19937 randomFixedSeed(42);
    std::mt19937 randomSeed(std::random_device{}());

    std::mt19937& rRandomSeed = useRandomSeed ? randomSeed : randomFixedSeed;

    std::uniform_real_distribution<float> randomPositionX(-8, 8);
    std::uniform_real_distribution<float> randomPositionY(-5, 5);
    std::uniform_real_distribution<float> randomPositionZ(-50, -1);
    std::uniform_real_distribution<float> randomRotation(-1.0f, 1.0f);
    std::uniform_real_distribution<float> randomScale(0.15f, 0.4f);
    std::uniform_real_distribution<float> randomAngle(MathHelper::RADS0, MathHelper::RADS360);
    std::uniform_int_distribution<uint32_t> randomMesh(0, 2);


    for (auto& rEntry : rObjects)
    {
      rEntry.MeshIndex = randomMesh(rRandomSeed);
      rEntry.Position = Vector3(randomPositionX(rRandomSeed), randomPositionY(rRandomSeed), randomPositionZ(rRandomSeed));
      rEntry.RotationSpeed = Vector3(randomRotation(rRandomSeed), randomRotation(rRandomSeed), randomRotation(rRandomSeed));
      rEntry.Rotation = Vector3(randomAngle(rRandomSeed), randomAngle(rRandomSeed), randomAngle(rRandomSeed));
      rEntry.Scale = randomScale(rRandomSeed) * 2.0f;
    }

    // Sort the objects according to mesh index to make it simpler to render
    std::sort(rObjects.begin(), rObjects.end(), [](const MeshObject& lhs, const MeshObject& rhs) -> bool { return lhs.MeshIndex < rhs.MeshIndex; });
  }
}
