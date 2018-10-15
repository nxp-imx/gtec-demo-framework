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

#include "ObjectSelection.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Math/Viewport.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics3D/Procedural/BoxGenerator.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslGraphics3D/Procedural/TorusGenerator.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <Shared/ObjectSelection/OptionParser.hpp>
#include <GLES3/gl3.h>
#include <random>

namespace Fsl
{
  using namespace GLES3;
  using namespace Procedural;

  namespace
  {
    const float DEFAULT_ZOOM = 10;

    template <typename TVertex>
    BoundingBox CalculateBoundingBox(const std::vector<TVertex>& vertices)
    {
      Vector3 min = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
      Vector3 max = Vector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

      for (auto entry : vertices)
      {
        min.X = (entry.Position.X < min.X ? entry.Position.X : min.X);
        min.Y = (entry.Position.Y < min.Y ? entry.Position.Y : min.Y);
        min.Z = (entry.Position.Z < min.Z ? entry.Position.Z : min.Z);
        max.X = (entry.Position.X > max.X ? entry.Position.X : max.X);
        max.Y = (entry.Position.Y > max.Y ? entry.Position.Y : max.Y);
        max.Z = (entry.Position.Z > max.Z ? entry.Position.Z : max.Z);
      }
      return BoundingBox(min, max);
    }


    BoundingBox CalculateAABB(const Matrix& matrix, const BoundingBox& box)
    {
      Vector3 vertices[] = {Vector3(box.Min.X, box.Min.Y, box.Min.Z), Vector3(box.Max.X, box.Min.Y, box.Min.Z),
                            Vector3(box.Min.X, box.Max.Y, box.Min.Z), Vector3(box.Max.X, box.Max.Y, box.Min.Z),

                            Vector3(box.Min.X, box.Min.Y, box.Max.Z), Vector3(box.Max.X, box.Min.Y, box.Max.Z),
                            Vector3(box.Min.X, box.Max.Y, box.Max.Z), Vector3(box.Max.X, box.Max.Y, box.Max.Z)};

      Vector3 min = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
      Vector3 max = Vector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

      for (uint32_t i = 0; i < 8; ++i)
      {
        const auto entry = Vector4::Transform(vertices[i], matrix);
        min.X = (entry.X < min.X ? entry.X : min.X);
        min.Y = (entry.Y < min.Y ? entry.Y : min.Y);
        min.Z = (entry.Z < min.Z ? entry.Z : min.Z);

        max.X = (entry.X > max.X ? entry.X : max.X);
        max.Y = (entry.Y > max.Y ? entry.Y : max.Y);
        max.Z = (entry.Z > max.Z ? entry.Z : max.Z);
      }
      return BoundingBox(min, max);
    }
  }


  ObjectSelection::ObjectSelection(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_menuUI(config)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_meshes(3)
    , m_attribLink(3)
    , m_vertexDeclLine(VertexPositionColor::GetVertexDeclaration())
    , m_attribLinkColoredToLine(2)
    , m_attribLinkTextured(2)
    , m_lightDirection(0.0f, 1.0f, 1.0f)
    , m_lightColor(0.8f, 0.8f, 0.8f)
    , m_ambientColor(0.2f, 0.2f, 0.2f)
    , m_hasSelectedObject(false)
    , m_selectedIndex(0)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    m_camera.SetPosition(Vector3(0, 0, DEFAULT_ZOOM), Vector3(), Vector3::Up());

    m_lightDirection.Normalize();

    const auto optionParser = config.GetOptions<OptionParser>();

    auto contentManager = GetContentManager();

    {
      auto texture = contentManager->ReadTexture("SeamlessFur.png", PixelFormat::R8G8B8A8_UNORM);
      GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_texture.Reset(texture, texParams, TextureFlags::GenerateMipMaps);

      texture = contentManager->ReadTexture("MarbleChessboard.jpg", PixelFormat::R8G8B8A8_UNORM);
      m_textureChessboard.Reset(texture, texParams, TextureFlags::GenerateMipMaps);
    }

    PreparePlaneMesh(m_meshPlane, m_textureChessboard);
    PrepareMeshes(m_meshes, m_texture);

    PrepareDirectionalLightProgram(contentManager, m_programDirectionalLight, m_attribLink, VertexPositionNormalTexture::GetVertexDeclaration());
    PrepareSolidColorProgram(contentManager, m_programSolidColor, m_attribLinkColoredToLine, m_vertexDeclLine);
    PrepareTexturedProgram(contentManager, m_programTextured, m_attribLinkTextured, VertexPositionNormalTexture::GetVertexDeclaration());

    GenerateObjects(m_objects, optionParser->GetObjectCount(), m_menuUI.IsRandomSeedEnabled());

    m_planeObject.Rotation = Vector3(-MathHelper::ToRadians(90), 0, 0);
    m_planeObject.Position = Vector3(0, -8, 0);
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
      // if (event.IsPressed())
      //  m_camera.BeginDrag(event.GetPosition());
      // else if (m_camera.IsDragging())
      //  m_camera.EndDrag(event.GetPosition());
      const bool mouseCapture = event.IsPressed();
      if (m_demoAppControl->TryEnableMouseCaptureMode(mouseCapture))
      {
        m_mouseCaptureEnabled = mouseCapture;
      }
      else
      {
        m_mouseCaptureEnabled = false;
      }
      // FSLLOG("Captured: " << m_mouseCaptureEnabled);
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
    const Point2 screenResolution = GetScreenResolution();

    m_viewPort = Viewport(Rectangle(0, 0, screenResolution.X, screenResolution.Y));

    m_matrixView = m_camera.GetViewMatrix();
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), m_viewPort.GetAspectRatio(), 0.1f, 500.0f);
    m_matrixViewProjection = m_matrixView * m_matrixProjection;

    for (auto& rEntry : m_objects)
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

      rEntry.MeshAABB = CalculateAABB(rEntry.WorldMatrix, m_meshes[rEntry.MeshIndex].TheBoundingBox);
    }

    {
      m_planeObject.WorldMatrix = Matrix::CreateRotationX(m_planeObject.Rotation.X) * Matrix::CreateTranslation(m_planeObject.Position);
      m_planeObject.WorldViewMatrix = m_planeObject.WorldMatrix * m_matrixView;
      m_planeObject.WorldViewProjectionMatrix = m_planeObject.WorldViewMatrix * m_matrixProjection;

      m_planeObject.NormalMatrix = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_planeObject.WorldViewMatrix)));

      m_planeObject.MeshAABB = CalculateAABB(m_planeObject.WorldMatrix, m_meshPlane.TheBoundingBox);
    }


    {
      const auto mouseState = m_mouse->GetState();
      Vector3 sourcePos(static_cast<float>(mouseState.Position.X), static_cast<float>(mouseState.Position.Y), 0.0f);
      m_mousePositionNear = m_viewPort.Unproject(sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
      sourcePos.Z = 1.0f;
      m_mousePositionFar = m_viewPort.Unproject(sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
      // FSLLOG("2D: X: " << mouseState.Position.X << " Y: " << mouseState.Position.Y  << " 3D: X: " << m_mousePositionFar.X << " Y: " <<
      // m_mousePositionFar.Y << " Z: " << m_mousePositionFar.Z);


      if (!m_mouseCaptureEnabled)
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

    {
      const float movementSpeed = 32.0f * demoTime.DeltaTime;
      auto keyboardState = m_keyboard->GetState();

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
  }


  void ObjectSelection::Draw(const DemoTime& demoTime)
  {
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


  bool ObjectSelection::CheckCollision(const Point2& screenSpacePosition)
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
    for (std::size_t i = 0; i < m_objects.size(); ++i)
    {
      float distance;
      if (mouseRay.Intersects(m_objects[i].MeshAABB, distance))
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
    glUseProgram(m_programDirectionalLight.Program.Get());

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureChessboard.Get());

    glUniform3fv(m_programDirectionalLight.LocLightDirection, 1, m_lightDirection.DirectAccess());
    glUniform3fv(m_programDirectionalLight.LocLightColor, 1, m_lightColor.DirectAccess());
    glUniform3fv(m_programDirectionalLight.LocAmbientColor, 1, m_ambientColor.DirectAccess());

    // Enable the attribs the meshes use once (since we use the same mesh layout for everything)
    for (std::size_t i = 0; i < m_attribLink.size(); ++i)
    {
      if (m_attribLink[i].AttribIndex >= 0)
      {
        glEnableVertexAttribArray(m_attribLink[i].AttribIndex);
      }
    }

    // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
    if (m_menuUI.IsObjectsEnabled())
    {
      glBindTexture(GL_TEXTURE_2D, m_texture.Get());

      const bool forceBind = m_menuUI.IsForceBindEnabled();
      uint32_t oldMeshIndex = std::numeric_limits<uint32_t>::max();
      for (auto& rEntry : m_objects)
      {
        // Load the matrices
        glUniformMatrix4fv(m_programDirectionalLight.LocWorldView, 1, 0, rEntry.WorldViewMatrix.DirectAccess());
        glUniformMatrix4fv(m_programDirectionalLight.LocWorldViewProjection, 1, 0, rEntry.WorldViewProjectionMatrix.DirectAccess());
        glUniformMatrix3fv(m_programDirectionalLight.LocNormalMatrix, 1, 0, rEntry.NormalMatrix.DirectAccess());

        DrawMesh(m_meshes[rEntry.MeshIndex], (forceBind || oldMeshIndex != rEntry.MeshIndex));
        oldMeshIndex = rEntry.MeshIndex;
      }
    }

    // Disable stuff
    glBindBuffer(m_meshes[0].IndexBuffer.GetTarget(), 0);
    glBindBuffer(m_meshes[0].VertexBuffer.GetTarget(), 0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }


  void ObjectSelection::DrawPlaneMesh()
  {
    glUseProgram(m_programTextured.Program.Get());

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureChessboard.Get());

    // Load the matrix
    glUniformMatrix4fv(m_programTextured.LocWorldViewProjection, 1, 0, m_planeObject.WorldViewProjectionMatrix.DirectAccess());

    // Bind and enable the vertex buffer
    glBindBuffer(m_meshPlane.VertexBuffer.GetTarget(), m_meshPlane.VertexBuffer.Get());
    glBindBuffer(m_meshPlane.IndexBuffer.GetTarget(), m_meshPlane.IndexBuffer.Get());

    m_meshPlane.VertexBuffer.EnableAttribArrays(m_attribLinkTextured);

    glDrawElements(GL_TRIANGLE_STRIP, m_meshPlane.IndexBuffer.GetCapacity(), m_meshPlane.IndexBuffer.GetType(), nullptr);

    glBindBuffer(m_meshPlane.IndexBuffer.GetTarget(), 0);
    glBindBuffer(m_meshPlane.VertexBuffer.GetTarget(), 0);
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
      mesh.VertexBuffer.SetVertexAttribPointers(m_attribLink);
    }

    glDrawElements(GL_TRIANGLE_STRIP, mesh.IndexBuffer.GetCapacity(), indexBufferType, nullptr);
  }


  void ObjectSelection::DrawDebugData()
  {
    // Draw bounding boxes
    glUseProgram(m_programSolidColor.Program.Get());

    glEnableVertexAttribArray(m_attribLinkColoredToLine[0].AttribIndex);
    glEnableVertexAttribArray(m_attribLinkColoredToLine[1].AttribIndex);

    // Load the matrices
    if (m_menuUI.IsDrawOrientedBoundingBoxEnabled())
    {
      for (auto& rEntry : m_objects)
      {
        glUniformMatrix4fv(m_programSolidColor.LocWorldViewProjection, 1, 0, rEntry.WorldViewProjectionMatrix.DirectAccess());
        DrawBoundingBox(m_meshes[rEntry.MeshIndex].TheBoundingBox, Color::Red());
      }
    }

    glUniformMatrix4fv(m_programSolidColor.LocWorldViewProjection, 1, 0, m_matrixViewProjection.DirectAccess());
    if (m_menuUI.IsDrawAxisAlignedBoundingBoxEnabled())
    {
      for (std::size_t i = 0; i < m_objects.size(); ++i)
      {
        if (i != m_selectedIndex || !m_hasSelectedObject)
        {
          DrawBoundingBox(m_objects[i].MeshAABB, Color::Blue());
        }
      }
    }
    if (m_hasSelectedObject)
    {
      DrawBoundingBox(m_objects[m_selectedIndex].MeshAABB, Color::White());
    }


    {
      glUniformMatrix4fv(m_programSolidColor.LocWorldViewProjection, 1, 0, m_matrixViewProjection.DirectAccess());

      DrawLine(m_mousePositionNear, m_mousePositionFar, Color::Pink());

      if (m_menuUI.IsDrawNearPlaneMouseEnabled())
      {
        const float magic = 0.001f;
        DrawLine(m_mousePositionNear + Vector3(-magic, -magic, 0.0f), m_mousePositionNear + Vector3(Vector3(magic, magic, 0.0f)), Color::Pink());
        DrawLine(m_mousePositionNear + Vector3(magic, -magic, 0.0f), m_mousePositionNear + Vector3(Vector3(-magic, magic, 0.0f)), Color::Pink());
      }
      if (m_menuUI.IsDrawFarPlaneMouseEnabled())
      {
        const float magic = 5.0f;
        DrawLine(m_mousePositionFar + Vector3(-magic, -magic, 1.0f), m_mousePositionFar + Vector3(Vector3(magic, magic, 1.0f)), Color::Cyan());
        DrawLine(m_mousePositionFar + Vector3(magic, -magic, 1.0f), m_mousePositionFar + Vector3(Vector3(-magic, magic, 1.0f)), Color::Cyan());
      }
    }
  }


  void ObjectSelection::DrawBoundingBox(const BoundingBox& box, const Color& color)
  {
    const Vector4 realColor = color.ToVector4();
    VertexPositionColor vertices[] = {
      VertexPositionColor(Vector3(box.Min.X, box.Min.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Min.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Max.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Max.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Min.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Min.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Max.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Max.Y, box.Max.Z), realColor),

      VertexPositionColor(Vector3(box.Min.X, box.Min.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Min.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Min.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Min.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Max.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Max.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Max.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Max.Y, box.Max.Z), realColor),

      VertexPositionColor(Vector3(box.Min.X, box.Min.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Max.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Min.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Min.X, box.Max.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Min.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Max.Y, box.Min.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Min.Y, box.Max.Z), realColor),
      VertexPositionColor(Vector3(box.Max.X, box.Max.Y, box.Max.Z), realColor),
    };

    const auto e0 = m_vertexDeclLine.At(static_cast<std::size_t>(m_attribLinkColoredToLine[0].VertexElementIndex));
    const auto e1 = m_vertexDeclLine.At(static_cast<std::size_t>(m_attribLinkColoredToLine[1].VertexElementIndex));

    glVertexAttribPointer(m_attribLinkColoredToLine[0].AttribIndex, 3, GL_FLOAT, 0, sizeof(VertexPositionColor),
                          reinterpret_cast<const uint8_t*>(vertices) + e0.Offset);
    glVertexAttribPointer(m_attribLinkColoredToLine[1].AttribIndex, 4, GL_FLOAT, 0, sizeof(VertexPositionColor),
                          reinterpret_cast<const uint8_t*>(vertices) + e1.Offset);

    glDrawArrays(GL_LINES, 0, 8 + 8 + 8);
  }


  void ObjectSelection::DrawLine(const Vector3& p1, const Vector3& p2, const Color& color)
  {
    const Vector4 realColor = color.ToVector4();
    VertexPositionColor vertices[] = {
      VertexPositionColor(p1, realColor),
      VertexPositionColor(p2, realColor),
    };

    const auto e0 = m_vertexDeclLine.At(static_cast<std::size_t>(m_attribLinkColoredToLine[0].VertexElementIndex));
    const auto e1 = m_vertexDeclLine.At(static_cast<std::size_t>(m_attribLinkColoredToLine[1].VertexElementIndex));

    glVertexAttribPointer(m_attribLinkColoredToLine[0].AttribIndex, 3, GL_FLOAT, 0, sizeof(VertexPositionColor),
                          reinterpret_cast<const uint8_t*>(vertices) + e0.Offset);
    glVertexAttribPointer(m_attribLinkColoredToLine[1].AttribIndex, 4, GL_FLOAT, 0, sizeof(VertexPositionColor),
                          reinterpret_cast<const uint8_t*>(vertices) + e1.Offset);

    glDrawArrays(GL_LINES, 0, 2);
  }


  void ObjectSelection::PreparePlaneMesh(Mesh& rMeshPlane, const GLES3::GLTexture& texture)
  {
    const Point2 tex1Size = texture.GetSize();
    TextureRectangle texRect(Rectangle(0, 0, tex1Size.X, tex1Size.Y), tex1Size);
    const NativeTextureArea texRepeatArea(GLTexture::CalcTextureArea(texRect, 15 / 5, 15 / 5));
    const auto mesh = SegmentedQuadGenerator::GenerateStrip(Vector3(0, 0, 0), 1000 / 5.0f, 1000 / 5.0f, 1, 1, texRepeatArea, WindingOrder::CCW);

    rMeshPlane.IndexBuffer.Reset(mesh.GetIndexArray(), GL_STATIC_DRAW);
    rMeshPlane.VertexBuffer.Reset(mesh.GetVertexArray(), GL_STATIC_DRAW);
    rMeshPlane.TheBoundingBox = CalculateBoundingBox(mesh.GetVertexArray());
  }


  void ObjectSelection::PrepareMeshes(std::vector<Mesh>& rMeshes, const GLES3::GLTexture& texture)
  {
    const Point2 tex1Size = texture.GetSize();
    TextureRectangle texRect(Rectangle(0, 0, tex1Size.X, tex1Size.Y), tex1Size);

    const NativeTextureArea texArea(GLTexture::CalcTextureArea(texRect, 1, 1));

    auto mesh = TorusGenerator::GenerateStrip(16, 16, 2, 0.5f, texArea, WindingOrder::CCW);

    rMeshes[0].IndexBuffer.Reset(mesh.GetIndexArray(), GL_STATIC_DRAW);
    rMeshes[0].VertexBuffer.Reset(mesh.GetVertexArray(), GL_STATIC_DRAW);
    rMeshes[0].TheBoundingBox = CalculateBoundingBox(mesh.GetVertexArray());

    mesh = TorusGenerator::GenerateStrip(3, 3, 2, 0.5f, texArea, WindingOrder::CCW);

    rMeshes[1].IndexBuffer.Reset(mesh.GetIndexArray(), GL_STATIC_DRAW);
    rMeshes[1].VertexBuffer.Reset(mesh.GetVertexArray(), GL_STATIC_DRAW);
    rMeshes[1].TheBoundingBox = CalculateBoundingBox(mesh.GetVertexArray());

    NativeTextureArea texAreas[] = {texArea, texArea, texArea, texArea, texArea, texArea};
    mesh = BoxGenerator::GenerateStrip(Vector3(), 2.0f, 2.0f, 2.0f, texAreas, 6, WindingOrder::CCW);
    rMeshes[2].IndexBuffer.Reset(mesh.GetIndexArray(), GL_STATIC_DRAW);
    rMeshes[2].VertexBuffer.Reset(mesh.GetVertexArray(), GL_STATIC_DRAW);
    rMeshes[2].TheBoundingBox = CalculateBoundingBox(mesh.GetVertexArray());
  }


  void ObjectSelection::PrepareDirectionalLightProgram(const std::shared_ptr<IContentManager>& contentManager, ProgramDirectionalLight& rProgram,
                                                       std::vector<GLES3::GLVertexAttribLink>& rAttribLink, const VertexDeclaration& vertexDecl)
  {
    rProgram.Program.Reset(contentManager->ReadAllText("BasicShaderDLight.vert"), contentManager->ReadAllText("BasicShaderDLightTextured.frag"));

    const auto program = rProgram.Program.Get();
    rProgram.LocWorldView = glGetUniformLocation(program, "WorldView");
    rProgram.LocWorldViewProjection = glGetUniformLocation(program, "WorldViewProjection");
    rProgram.LocNormalMatrix = glGetUniformLocation(program, "NormalMatrix");
    rProgram.LocTexture0 = glGetUniformLocation(program, "Texture0");
    rProgram.LocLightDirection = glGetUniformLocation(program, "LightDirection");
    rProgram.LocLightColor = glGetUniformLocation(program, "LightColor");
    rProgram.LocAmbientColor = glGetUniformLocation(program, "AmbientColor");

    // Attrib link between shader and vertex format
    rAttribLink[0] =
      GLVertexAttribLink(glGetAttribLocation(program, "VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    rAttribLink[1] =
      GLVertexAttribLink(glGetAttribLocation(program, "VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    rAttribLink[2] = GLVertexAttribLink(glGetAttribLocation(program, "VertexTexCoord"),
                                        vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
  }


  void ObjectSelection::PrepareSolidColorProgram(const std::shared_ptr<IContentManager>& contentManager, ProgramColor& rProgram,
                                                 std::vector<GLES3::GLVertexAttribLink>& rAttribLink, const VertexDeclaration& vertexDecl)
  {
    rProgram.Program.Reset(contentManager->ReadAllText("VertexColored.vert"), contentManager->ReadAllText("VertexColored.frag"));
    const auto program = rProgram.Program.Get();

    rProgram.LocWorldViewProjection = glGetUniformLocation(program, "WorldViewProjection");

    // Attrib link between shader and vertex format
    rAttribLink[0] =
      GLVertexAttribLink(glGetAttribLocation(program, "VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    rAttribLink[1] =
      GLVertexAttribLink(glGetAttribLocation(program, "VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
  }


  void ObjectSelection::PrepareTexturedProgram(const std::shared_ptr<IContentManager>& contentManager, ProgramColor& rProgram,
                                               std::vector<GLES3::GLVertexAttribLink>& rAttribLink, const VertexDeclaration& vertexDecl)
  {
    // Prepare the texture program
    rProgram.Program.Reset(contentManager->ReadAllText("Textured.vert"), contentManager->ReadAllText("Textured.frag"));
    const auto program = rProgram.Program.Get();

    rProgram.LocWorldViewProjection = glGetUniformLocation(program, "WorldViewProjection");

    // Attrib link between shader and vertex format
    rAttribLink[0] =
      GLVertexAttribLink(glGetAttribLocation(program, "VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    rAttribLink[1] = GLVertexAttribLink(glGetAttribLocation(program, "VertexTexCoord"),
                                        vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
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
