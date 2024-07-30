/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include "ModelViewer.hpp"
#include <FslAssimp/SceneImporter.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTangentTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <Shared/ModelViewer/MeshUtil.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  using namespace GLES2;


  namespace
  {
    constexpr int32_t GraphRes = 20000;
    constexpr float DefaultZoom = 10;
    constexpr float DefaultModelScale = 5;
    constexpr auto ScenePath = "Models";

    void ExtractMeshes(GLVertexBufferArray& rVertexBuffers, GLIndexBufferArray& rIndexBuffers, const MeshUtil::TestScene& scene)
    {
      rVertexBuffers.Resize(scene.Meshes.size(), MeshUtil::TestMesh::vertex_type::AsVertexDeclarationSpan());
      rIndexBuffers.Resize(scene.Meshes.size(), GL_UNSIGNED_SHORT);
      std::size_t vertexCount = 0;
      std::size_t indexCount = 0;
      for (std::size_t i = 0; i < scene.Meshes.size(); ++i)
      {
        auto mesh = scene.Meshes[i];
        rIndexBuffers.Reset(i, mesh->GetIndexArray(), GL_STATIC_DRAW);
        rVertexBuffers.Reset(i, mesh->GetVertexArray(), GL_STATIC_DRAW);

        vertexCount += mesh->GetVertexCount();
        indexCount += mesh->GetIndexCount();
      }
      FSLLOG3_INFO("Total vertex count: {}, Total index count: {}", vertexCount, indexCount);
    }

    void ExtractToSingleMesh(GLVertexBufferArray& rVertexBuffers, GLIndexBufferArray& rIndexBuffers, const MeshUtil::TestScene& scene)
    {
      auto mesh = MeshUtil::ExtractToSingleMesh(scene);

      rVertexBuffers.Resize(1, MeshUtil::TestMesh::vertex_type::AsVertexDeclarationSpan());
      rIndexBuffers.Resize(1, GL_UNSIGNED_SHORT);
      rVertexBuffers.Reset(0, mesh.Vertices, GL_STATIC_DRAW);
      rIndexBuffers.Reset(0, mesh.Indices, GL_STATIC_DRAW);
    }

    void ExtractMeshEdges(GLVertexBufferArray& rVertexBuffers, GLIndexBufferArray& rIndexBuffers, const MeshUtil::TestScene& scene)
    {
      auto mesh = MeshUtil::ExtractMeshEdges(scene);

      FSLLOG3_INFO("Building mesh");
      rVertexBuffers.Resize(1, MeshUtil::TestMesh::vertex_type::AsVertexDeclarationSpan());
      rIndexBuffers.Resize(1, GL_UNSIGNED_SHORT);

      rVertexBuffers.Reset(0, mesh.Vertices, GL_STATIC_DRAW);
      rIndexBuffers.Reset(0, mesh.Indices, GL_STATIC_DRAW);
    }


    void PrepareMeshes(GLVertexBufferArray& rVertexBuffers, GLIndexBufferArray& rIndexBuffers, const MeshUtil::TestScene& scene, MeshMode meshMode)
    {
      switch (meshMode)
      {
      case MeshMode::SingleMesh:
        FSLLOG3_INFO("MeshMode: SingleMesh");
        ExtractToSingleMesh(rVertexBuffers, rIndexBuffers, scene);
        break;
      case MeshMode::MultipleMeshes:
        FSLLOG3_INFO("MeshMode: MultipleMeshes");
        ExtractMeshes(rVertexBuffers, rIndexBuffers, scene);
        break;
      case MeshMode::Wireframe:
        FSLLOG3_INFO("MeshMode: Wireframe");
        ExtractMeshEdges(rVertexBuffers, rIndexBuffers, scene);
        break;
      default:
        throw NotSupportedException(fmt::format("Unknown MeshMode: {}", static_cast<int32_t>(meshMode)));
      }
    }


  }

  ModelViewer::ModelViewer(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_profilerService(config.DemoServiceProvider.Get<IProfilerService>())
    , m_hCounterBind(m_profilerService, m_profilerService->CreateCustomCounter("bind", 0, GraphRes, Color(0xFF, 0xFF, 0x80, 0xFF)))
    , m_hCounterEnable(m_profilerService, m_profilerService->CreateCustomCounter("enable", 0, GraphRes, Color(0xFF, 0x80, 0xFF, 0xFF)))
    , m_hCounterDraw(m_profilerService, m_profilerService->CreateCustomCounter("draw", 0, GraphRes, Color(0x80, 0xFF, 0xFF, 0xFF)))
    , m_hCounterTotal(m_profilerService, m_profilerService->CreateCustomCounter("total", 0, GraphRes, Color(0x80, 0x80, 0xFF, 0xFF)))
    , m_camera(config.WindowMetrics.GetSizePx())
    , m_renderConfig(Vector3(), Vector3(0.5f, -0.6f, 0.7f), true)
    , m_lightDirection(1.0f, 1.0f, 1.0f)
    , m_fragUboData(Vector3(0.8f, 0.8f, 0.8f), Vector4(0.2f, 0.2f, 0.2f, 0.2f), Vector4(1, 1, 1, 1), 100.0f)
  {
    m_lightDirection.Normalize();

    m_camera.SetZoom(DefaultZoom);

    auto options = config.GetOptions<OptionParser>();

    auto contentManager = GetContentManager();
    auto contentPath = contentManager->GetContentPath();

    auto customModelPath = options->GetCustomModelPath();
    ModelSceneUtil::ModelLoaderConfig loaderConfig;
    if (customModelPath.IsEmpty())
    {
      loaderConfig = PrepareSceneModel(m_renderConfig, m_camera, *contentManager, options->GetScene(), ScenePath);
    }
    else
    {
      loaderConfig = PrepareCustomModel(m_renderConfig, *contentManager, customModelPath);
    }

    FSLLOG3_INFO("Loading textures");
    const bool useDiffuseTexture = PrepareTextures(*contentManager, loaderConfig);

    // Default is Fast
    // aiProcessPreset_TargetRealtime_Fast
    // aiProcessPreset_TargetRealtime_Quality
    // aiProcessPreset_TargetRealtime_MaxQuality
    // | aiProcess_TransformUVCoords
    auto modelPath = IO::Path::Combine(contentPath, loaderConfig.ModelFileName);
    FSLLOG3_INFO("Loading scene '{}'", loaderConfig.ModelFileName);
    SceneImporter sceneImporter;
    const auto scene = sceneImporter.Load<MeshUtil::TestScene>(modelPath, DefaultModelScale * loaderConfig.ScaleMod, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException(fmt::format("Scene did not contain any meshes: '{}'", loaderConfig.ModelFileName));
    }

    m_rootNode = scene->GetRootNode();
    if (!m_rootNode)
    {
      throw NotSupportedException(fmt::format("Scene did not contain a root node: '{}'", loaderConfig.ModelFileName));
    }

    m_wireframe = (options->GetMeshMode() == MeshMode::Wireframe);

    // std::string baseShaderName = "LightPerVertexDirectional";
    // std::string baseShaderName = "LightPerVertexDirectionalTextured";
    // std::string baseShaderName = "LightPerVertexDirectionalSpecular";
    // std::string baseShaderName = "LightPerVertexDirectionalSpecularTextured";
    // std::string baseShaderName = "PerPixelDirectional";
    // std::string baseShaderName = "PerPixelDirectionalTextured";

    switch (options->GetShaderMode())
    {
    case ShaderMode::PerPixelNoTextures:
      FSLLOG3_INFO("ShaderMode: PerPixelNoTextures");
      PrepareShader(contentManager, false, false, false, false, "PerPixelDirectionalSpecular");
      break;
    case ShaderMode::NoLightVertexColor:
      FSLLOG3_INFO("ShaderMode: NoLightVertexColor");
      PrepareShader(contentManager, false, false, false, false, "VertexColor", false);
      break;
    case ShaderMode::PerPixelTextured:
    default:
      FSLLOG3_INFO("ShaderMode: PerPixelTextured");
      PrepareShader(contentManager, useDiffuseTexture, !loaderConfig.TextureGloss.IsEmpty(), m_resources.TextureSpecular.IsValid(),
                    m_resources.TextureNormal.IsValid(), "PerPixelDirectionalSpecular");
      break;
    }


    // Create index and vertex buffers for all the meshes.
    PrepareMeshes(m_resources.VertexBuffers, m_resources.IndexBuffers, *scene, options->GetMeshMode());

    m_storedStartRotation = m_renderConfig.Rotation;
    GL_CHECK_FOR_ERROR();

    FSLLOG3_INFO("Ready");
  }


  ModelViewer::~ModelViewer() = default;


  void ModelViewer::OnKeyEvent(const KeyEvent& event)
  {
    switch (event.GetKey())
    {
    case VirtualKey::R:
      if (event.IsPressed())
      {
        std::swap(m_renderConfig.RotationSpeed, m_rotationSpeedOld);
      }
      event.Handled();
      break;
    default:
      DemoAppGLES2::OnKeyEvent(event);
      break;
    }
  }


  void ModelViewer::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    switch (event.GetButton())
    {
    case VirtualMouseButton::Left:
      {
        if (event.IsPressed())
        {
          m_camera.BeginDrag(event.GetPosition());
        }
        else if (m_camera.IsDragging())
        {
          m_camera.EndDrag(event.GetPosition());
        }
        event.Handled();
      }
      break;
    case VirtualMouseButton::Right:
      if (event.IsPressed())
      {
        m_camera.ResetRotation();
        m_camera.SetZoom(DefaultZoom);
        m_renderConfig.Rotation = m_storedStartRotation;
        event.Handled();
      }
      break;
    default:
      DemoAppGLES2::OnMouseButtonEvent(event);
      break;
    }
  }


  void ModelViewer::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    DemoAppGLES2::OnMouseMoveEvent(event);

    if (m_camera.IsDragging())
    {
      m_camera.Drag(event.GetPosition());
      event.Handled();
    }
  }


  void ModelViewer::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    DemoAppGLES2::OnMouseWheelEvent(event);
    m_camera.AddZoom(static_cast<float>(event.GetDelta()) * -0.001f);
  }


  void ModelViewer::Update(const DemoTime& demoTime)
  {
    m_renderConfig.Rotation.X += m_renderConfig.RotationSpeed.X * demoTime.DeltaTime;
    m_renderConfig.Rotation.Y += m_renderConfig.RotationSpeed.Y * demoTime.DeltaTime;
    m_renderConfig.Rotation.Z += m_renderConfig.RotationSpeed.Z * demoTime.DeltaTime;
    m_matrixWorld = Matrix::CreateRotationX(m_renderConfig.Rotation.X) * Matrix::CreateRotationY(m_renderConfig.Rotation.Y) *
                    Matrix::CreateRotationZ(m_renderConfig.Rotation.Z);
    m_matrixView = m_camera.GetViewMatrix();
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), GetWindowAspectRatio(), 1, 1000.0f);
    m_vertexUboData.MatWorldView = m_matrixWorld * m_matrixView;
    m_vertexUboData.MatWorldViewProjection = m_vertexUboData.MatWorldView * m_matrixProjection;

    Vector4 cameraSpaceLightDirection = Vector4::Transform(m_lightDirection, m_camera.GetRotationMatrix());
    m_fragUboData.LightDirection = Vector3(cameraSpaceLightDirection.X, cameraSpaceLightDirection.Y, cameraSpaceLightDirection.Z);
    m_fragUboData.LightDirection.Normalize();
    m_vertexUboData.MatNormal = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_vertexUboData.MatWorldView)));
  }


  void ModelViewer::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    if (m_renderConfig.AllowBackfaceCull)
    {
      glEnable(GL_CULL_FACE);
    }
    else
    {
      glDisable(GL_CULL_FACE);
    }

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(m_resources.Program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_resources.Texture.Get());
      if (m_resources.TextureSpecular.IsValid() && m_resources.LocTextureSpecular != GLValues::InvalidLocation)
      {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_resources.TextureSpecular.Get());

        glUniform1i(m_resources.LocTextureSpecular, 1);
      }
      if (m_resources.TextureNormal.IsValid() && m_resources.LocTextureNormal != GLValues::InvalidLocation)
      {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_resources.TextureNormal.Get());

        glUniform1i(m_resources.LocTextureNormal, 2);
      }

      if (m_resources.LocLightDirection != GLValues::InvalidLocation)
      {
        glUniform3fv(m_resources.LocLightDirection, 1, m_fragUboData.LightDirection.DirectAccess());
      }
      if (m_resources.LocLightColor != GLValues::InvalidLocation)
      {
        glUniform3fv(m_resources.LocLightColor, 1, m_fragUboData.LightColor.DirectAccess());
      }
      if (m_resources.LocMatAmbient != GLValues::InvalidLocation)
      {
        glUniform4fv(m_resources.LocMatAmbient, 1, m_fragUboData.MatAmbient.DirectAccess());
      }
      if (m_resources.LocMatSpecular != GLValues::InvalidLocation)
      {
        glUniform4fv(m_resources.LocMatSpecular, 1, m_fragUboData.MatSpecular.DirectAccess());
      }
      if (m_resources.LocMatShininess != GLValues::InvalidLocation)
      {
        glUniform1f(m_resources.LocMatShininess, m_fragUboData.MatShininess);
      }
      // Load the matrices
      if (m_resources.LocWorld != GLValues::InvalidLocation)
      {
        glUniformMatrix4fv(m_resources.LocWorld, 1, 0, m_matrixWorld.DirectAccess());
      }
      if (m_resources.LocWorldView != GLValues::InvalidLocation)
      {
        glUniformMatrix4fv(m_resources.LocWorldView, 1, 0, m_vertexUboData.MatWorldView.DirectAccess());
      }
      if (m_resources.LocWorldViewProjection != GLValues::InvalidLocation)
      {
        glUniformMatrix4fv(m_resources.LocWorldViewProjection, 1, 0, m_vertexUboData.MatWorldViewProjection.DirectAccess());
      }
      if (m_resources.LocNormalMatrix != GLValues::InvalidLocation)
      {
        glUniformMatrix3fv(m_resources.LocNormalMatrix, 1, 0, m_vertexUboData.MatNormal.DirectAccess());
      }


      // Enable the attribs the meshes use once (since we use the same mesh layout for everything)
      for (const auto& attribLink : m_resources.AttribLink)
      {
        if (attribLink.AttribIndex >= 0)
        {
          glEnableVertexAttribArray(attribLink.AttribIndex);
        }
      }

      DrawMeshes();
      // DrawMeshesAndProfile();
      // DrawMeshesUsingNodes(m_rootNode.get(), Matrix::GetIdentity());

      // GL_CHECK_FOR_ERROR();

      // Disable everything
      glBindBuffer(m_resources.IndexBuffers.GetTarget(), 0);
      glBindBuffer(m_resources.VertexBuffers.GetTarget(), 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glUseProgram(0);
    }
  }


  void ModelViewer::DrawMeshes()
  {
    const GLenum drawMode = !m_wireframe ? GL_TRIANGLES : GL_LINES;
    const auto indexBufferType = m_resources.IndexBuffers.GetType();
    for (int32_t i = 0; i < m_resources.IndexBuffers.Length(); ++i)
    {
      auto indexBuffer = m_resources.IndexBuffers.Get(i);
      auto vertexBuffer = m_resources.VertexBuffers.Get(i);
      if (indexBuffer.GetCapacity() > 0)
      {
        // Bind and enable the vertex buffer
        glBindBuffer(m_resources.VertexBuffers.GetTarget(), vertexBuffer.Get());
        glBindBuffer(m_resources.IndexBuffers.GetTarget(), indexBuffer.Get());

        // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
        m_resources.VertexBuffers.SetVertexAttribPointers(m_resources.AttribLink);

        glDrawElements(drawMode, indexBuffer.GetGLCapacity(), indexBufferType, nullptr);
      }
    }
  }


  void ModelViewer::DrawMeshesAndProfile()
  {
    HighResolutionTimer timer;
    auto startTime = timer.GetTimestamp();
    TimeSpan totalTimeBind;
    TimeSpan totalTimeEnable;
    TimeSpan totalTimeDraw;

    const auto indexBufferType = m_resources.IndexBuffers.GetType();

    TickCount sequenceTimestampStart;
    TickCount sequenceTimestampEnd;
    const GLenum drawMode = !m_wireframe ? GL_TRIANGLES : GL_LINES;
    for (int32_t i = 0; i < m_resources.IndexBuffers.Length(); ++i)
    {
      auto indexBuffer = m_resources.IndexBuffers.Get(i);
      auto vertexBuffer = m_resources.VertexBuffers.Get(i);
      if (indexBuffer.GetCapacity() > 0)
      {
        // Bind and enable the vertex buffer
        sequenceTimestampStart = timer.GetTimestamp();
        glBindBuffer(m_resources.VertexBuffers.GetTarget(), vertexBuffer.Get());
        glBindBuffer(m_resources.IndexBuffers.GetTarget(), indexBuffer.Get());
        sequenceTimestampEnd = timer.GetTimestamp();
        totalTimeBind += sequenceTimestampEnd - sequenceTimestampStart;
        sequenceTimestampStart = sequenceTimestampEnd;

        // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
        m_resources.VertexBuffers.SetVertexAttribPointers(m_resources.AttribLink);
        // m_vertexBuffers.EnableAttribArrays(m_attribLink.data(), m_attribLink.size());
        sequenceTimestampEnd = timer.GetTimestamp();
        totalTimeEnable += sequenceTimestampEnd - sequenceTimestampStart;
        sequenceTimestampStart = sequenceTimestampEnd;

        glDrawElements(drawMode, indexBuffer.GetGLCapacity(), indexBufferType, nullptr);

        sequenceTimestampEnd = timer.GetTimestamp();
        totalTimeDraw += sequenceTimestampEnd - sequenceTimestampStart;
        sequenceTimestampStart = sequenceTimestampEnd;
      }
    }
    auto endTime = timer.GetTimestamp();
    auto totalTime = endTime - startTime;
    // FSLLOG3_INFO("DrawCalls: " << m_indexBuffers.Length() << " Time: " << totalTime << " bind: " << totalTimeBind << " enable: " << totalTimeEnable
    // << " draw: " << totalTimeDraw);

    {
      const auto val1 = TimeSpanUtil::ToClampedMicrosecondsInt32(totalTimeBind);
      const auto val2 = TimeSpanUtil::ToClampedMicrosecondsInt32(totalTimeEnable);
      const auto val3 = TimeSpanUtil::ToClampedMicrosecondsInt32(totalTimeDraw);
      const auto val4 = TimeSpanUtil::ToClampedMicrosecondsInt32(totalTime);
      m_profilerService->Set(m_hCounterBind, val1);
      m_profilerService->Set(m_hCounterEnable, val2);
      m_profilerService->Set(m_hCounterDraw, val3);
      m_profilerService->Set(m_hCounterTotal, val4);
    }
  }


  void ModelViewer::DrawMeshesUsingNodes(const Graphics3D::SceneNode* pNode, const Matrix& parentMatrix)
  {
    // This is probably the slowest and most basic way of rendering a node based scene
    const Matrix currentMatrix = parentMatrix * pNode->GetTransformation();
    const int32_t childCount = pNode->GetChildCount();
    for (int32_t i = 0; i < childCount; ++i)
    {
      DrawMeshesUsingNodes(pNode->GetChildAt(i).get(), currentMatrix);
    }

    const auto indexBufferType = m_resources.IndexBuffers.GetType();

    const int32_t meshCount = pNode->GetMeshCount();
    if (meshCount > 0)
    {
      Matrix matrix = currentMatrix * m_matrixView * m_matrixProjection;

      // Load the matrices
      if (m_resources.LocWorld != GLValues::InvalidLocation)
      {
        glUniformMatrix4fv(m_resources.LocWorld, 1, 0, currentMatrix.DirectAccess());
      }
      // if (m_locWorldView != GLValues::InvalidLocation)
      //  glUniformMatrix4fv(m_locWorldView, 1, 0, currentMatrix.DirectAccess());
      if (m_resources.LocWorldViewProjection != GLValues::InvalidLocation)
      {
        glUniformMatrix4fv(m_resources.LocWorldViewProjection, 1, 0, matrix.DirectAccess());
      }

      for (int32_t i = 0; i < meshCount; ++i)
      {
        int32_t meshIndex = pNode->GetMeshAt(i);

        auto indexBuffer = m_resources.IndexBuffers.Get(meshIndex);
        auto vertexBuffer = m_resources.VertexBuffers.Get(meshIndex);
        if (indexBuffer.GetCapacity() > 0)
        {
          // Bind and enable the vertex buffer
          glBindBuffer(m_resources.VertexBuffers.GetTarget(), vertexBuffer.Get());
          glBindBuffer(m_resources.IndexBuffers.GetTarget(), indexBuffer.Get());

          // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
          m_resources.VertexBuffers.SetVertexAttribPointers(m_resources.AttribLink);

          glDrawElements(GL_TRIANGLES, indexBuffer.GetGLCapacity(), indexBufferType, nullptr);
        }
      }
    }
  }

  void ModelViewer::PrepareShader(const std::shared_ptr<IContentManager>& contentManager, const bool useDiffuse, const bool useGlossMap,
                                  const bool useSpecularMap, const bool useNormalMap, const std::string& baseShaderName,
                                  const bool requireVertexNormal)
  {
    IO::Path shaderPath("Shaders");

    auto shaderName = baseShaderName;

    if (useSpecularMap)
    {
      shaderName += "Map";
    }
    if (useGlossMap)
    {
      shaderName += "GlossMap";
    }
    if (useNormalMap)
    {
      shaderName += "NormalMap";
    }
    if (useDiffuse || useSpecularMap || useGlossMap || useNormalMap)
    {
      shaderName += "Textured";
    }

    // auto strFragShaderName = useTexture ? "BasicShaderDLightTextured.frag" : "BasicShaderDLight.frag";
    const IO::Path strVertShaderFilename(shaderName + ".vert");
    const IO::Path strFragShaderFilename(shaderName + ".frag");
    // std::string strVertShaderFilename("BasicShaderDLight.vert");
    // std::string strFragShaderFilename("BasicShaderDLight.frag");
    // auto strFragShaderName = useTexture ? "BasicShaderDLightTextured.frag" : "BasicShaderDLight.frag";

    const IO::Path strVertShaderPath = IO::Path::Combine(shaderPath, strVertShaderFilename);
    const IO::Path strFragShaderPath = IO::Path::Combine(shaderPath, strFragShaderFilename);
    m_resources.Program.Reset(contentManager->ReadAllText(strVertShaderPath), contentManager->ReadAllText(strFragShaderPath));

    m_resources.LocWorld = m_resources.Program.TryGetUniformLocation("World");
    m_resources.LocWorldView = m_resources.Program.TryGetUniformLocation("WorldView");
    m_resources.LocWorldViewProjection = m_resources.Program.TryGetUniformLocation("WorldViewProjection");
    m_resources.LocNormalMatrix = m_resources.Program.TryGetUniformLocation("NormalMatrix");
    m_resources.LocTexture0 = m_resources.Program.TryGetUniformLocation("Texture0");
    m_resources.LocTextureSpecular = m_resources.Program.TryGetUniformLocation("TextureSpecular");
    m_resources.LocTextureNormal = m_resources.Program.TryGetUniformLocation("TextureNormal");
    m_resources.LocLightDirection = m_resources.Program.TryGetUniformLocation("LightDirection");
    m_resources.LocLightColor = m_resources.Program.TryGetUniformLocation("LightColor");
    m_resources.LocMatAmbient = m_resources.Program.TryGetUniformLocation("MatAmbient");
    m_resources.LocMatSpecular = m_resources.Program.TryGetUniformLocation("MatSpecular");
    m_resources.LocMatShininess = m_resources.Program.TryGetUniformLocation("MatShininess");

    constexpr auto VertexDecl = MeshUtil::TestMesh::vertex_type::GetVertexDeclarationArray();
    m_resources.AttribLink[0] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexPosition"),
                                                   VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    m_resources.AttribLink[1] =
      GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexColor"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
    if (requireVertexNormal)
    {
      m_resources.AttribLink[2] =
        GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexNormal"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    }
    else
    {
      m_resources.AttribLink[2] = GLVertexAttribLink(m_resources.Program.TryGetAttribLocation("VertexNormal"),
                                                     VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    }

    m_resources.AttribLink[3] = GLVertexAttribLink(m_resources.Program.TryGetAttribLocation("VertexTexCoord"),
                                                   VertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    if (useNormalMap)
    {
      m_resources.AttribLink[4] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexTangent"),
                                                     VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Tangent, 0));
    }
    else
    {
      m_resources.AttribLink[4] = GLVertexAttribLink(-1, 0);
    }
  }

  bool ModelViewer::PrepareTextures(const IContentManager& contentManager, const ModelSceneUtil::ModelLoaderConfig& config)
  {
    if (config.TextureFileName.IsEmpty())
    {
      return false;
    }

    Bitmap bitmap;

    if (config.TextureGloss.IsEmpty())
    {
      FSLLOG3_INFO("- Diffuse");
      contentManager.Read(bitmap, config.TextureFileName, PixelFormat::R8G8B8_UNORM);
    }
    else
    {
      Bitmap bitmapGloss;
      FSLLOG3_INFO("- Diffuse");
      contentManager.Read(bitmap, config.TextureFileName, PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("- Gloss");
      contentManager.Read(bitmapGloss, config.TextureGloss, PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Combining diffuse and gloss texture");
      // This is a slow and brute force way of combining the textures
      for (uint32_t y = 0; y < bitmap.RawUnsignedHeight(); ++y)
      {
        for (uint32_t x = 0; x < bitmap.RawUnsignedWidth(); ++x)
        {
          auto col1 = bitmap.GetNativePixel(x, y);
          auto col2 = bitmapGloss.GetNativePixel(x, y);
          const uint32_t color = (col1 & 0xFFFFFF) | ((col2 & 0xFF) << 24);
          bitmap.SetNativePixel(x, y, color);
        }
      }
    }

    GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    m_resources.Texture.Reset(bitmap, texParams, TextureFlags::GenerateMipMaps);

    if (!config.TextureSpecularFileName.IsEmpty())
    {
      contentManager.Read(bitmap, config.TextureSpecularFileName, PixelFormat::R8G8B8A8_UNORM);
      m_resources.TextureSpecular.Reset(bitmap, texParams, TextureFlags::GenerateMipMaps);
    }
    if (!config.TextureNormalFileName.IsEmpty())
    {
      contentManager.Read(bitmap, config.TextureNormalFileName, PixelFormat::R8G8B8A8_UNORM);
      m_resources.TextureNormal.Reset(bitmap, texParams, TextureFlags::GenerateMipMaps);
    }
    return true;
  }
}
