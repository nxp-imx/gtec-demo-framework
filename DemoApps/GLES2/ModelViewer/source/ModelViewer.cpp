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
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTangentTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>
#include "OptionParser.hpp"

namespace Fsl
{
  using namespace GLES2;
  using namespace Graphics3D;

  using TestMesh = GenericMesh<VertexPositionColorNormalTangentTexture, uint16_t>;
  using TestScene = GenericScene<TestMesh>;

  const int32_t GRAPH_RES = 20000;

  namespace
  {
    const float DEFAULT_ZOOM = 10;
    const float DEFAULT_MODEL_SCALE = 5;
    const auto SCENE_PATH = "Models";
  }

  ModelViewer::ModelViewer(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_profilerService(config.DemoServiceProvider.Get<IProfilerService>())
    , m_hCounterBind(m_profilerService, m_profilerService->CreateCustomCounter("bind", 0, GRAPH_RES, Color(0xFF, 0xFF, 0x80, 0xFF)))
    , m_hCounterEnable(m_profilerService, m_profilerService->CreateCustomCounter("enable", 0, GRAPH_RES, Color(0xFF, 0x80, 0xFF, 0xFF)))
    , m_hCounterDraw(m_profilerService, m_profilerService->CreateCustomCounter("draw", 0, GRAPH_RES, Color(0x80, 0xFF, 0xFF, 0xFF)))
    , m_hCounterTotal(m_profilerService, m_profilerService->CreateCustomCounter("total", 0, GRAPH_RES, Color(0x80, 0x80, 0xFF, 0xFF)))
    , m_camera(config.ScreenResolution)
    , m_rotationSpeed(0.5f, -0.6f, 0.7f)
    , m_lightDirection(1.0f, 1.0f, 1.0f)
    , m_lightColor(0.8f, 0.8f, 0.8f)
    , m_matAmbient(0.2f, 0.2f, 0.2f, 0.2f)
    , m_matSpecular(1, 1, 1, 1)
    , m_matShininess(100.0f)
    , m_allowBackfaceCull(true)
  {
    m_camera.SetZoom(DEFAULT_ZOOM);

    auto options = config.GetOptions<OptionParser>();

    auto contentManager = GetContentManager();
    auto contentPath = contentManager->GetContentPath();

    std::string strFileName, strTextureFileName, strTextureGloss, strTextureSpecularFileName, strTextureNormalFileName;
    float scaleMod = 1.0f;
    switch (options->GetScene())
    {
    case 0:
      // Using this as the default since the fbx files seems to have problems under ubuntu
      strFileName = "Knight2/armor.obj";
      strTextureFileName = "Knight2/armor_default_color.jpg";
      strTextureSpecularFileName = "Knight2/armor_default_metalness.jpg";
      strTextureGloss = "Knight2/armor_default_rough.jpg";
      strTextureNormalFileName = "Knight2/armor_default_nmap.jpg";
      m_rotationSpeed.X = 0;
      m_rotationSpeed.Z = 0;
      break;
    case 1:
      strFileName = "Dragon/dragon.3ds";
      strTextureFileName = "Dragon/color.jpg";
      strTextureSpecularFileName = "Dragon/spec.jpg";
      strTextureNormalFileName = "Dragon/normal.jpg";
      scaleMod = 2.0f;
      m_rotation.Y = MathHelper::ToRadians(170.0f);
      m_rotationSpeed.X = 0;
      m_rotationSpeed.Z = 0;
      m_allowBackfaceCull = false;
      break;
    case 2:
      strFileName = "FuturisticCar/Futuristic_Car.3ds";
      strTextureFileName = "FuturisticCar/Futuristic_Car_C.jpg";
      strTextureNormalFileName = "FuturisticCar/Futuristic_Car_N.jpg";
      // strTextureSpecularFileName = "FuturisticCar/Futuristic_Car_S.jpg";
      // m_rotation.X = MathHelper::ToRadians(-90.0f);
      m_rotationSpeed.X = 0;
      m_rotationSpeed.Z = 0;
      scaleMod = 1.75f;
      m_camera.SetRotation(Matrix::CreateRotationX(MathHelper::ToRadians(15)));
      break;
    case 3:
      strFileName = "Torus1/Scene_flat.fbx";
      break;
    case 4:
      strFileName = "Torus1/Scene_smooth.fbx";
      break;
    case 5:
      strFileName = "Torus2/Scene_flat.fbx";
      break;
    case 6:
      strFileName = "Torus2/Scene_smooth.fbx";
      break;
    case 7:
      strFileName = "Nff/Cube.nff";
      break;
    case 8:
      strFileName = "Nff/Sphere.nff";
      m_rotationSpeed.X = 0;
      m_rotationSpeed.Z = 0;
      break;
    case 9:
      strFileName = "Nff/Tetrahedron.nff";
      break;
    default:
      throw NotSupportedException("Scene not supported");
    }


    FSLLOG("Loading textures");
    if (!strTextureFileName.empty())
    {
      Bitmap bitmap;
      auto texturePath = IO::Path::Combine(SCENE_PATH, strTextureFileName);

      if (strTextureGloss.empty())
      {
        FSLLOG("- Diffuse");
        contentManager->Read(bitmap, texturePath, PixelFormat::R8G8B8_UNORM);
      }
      else
      {
        Bitmap bitmapGloss;
        auto glossTexturePath = IO::Path::Combine(SCENE_PATH, strTextureGloss);
        FSLLOG("- Diffuse");
        contentManager->Read(bitmap, texturePath, PixelFormat::R8G8B8A8_UNORM);
        FSLLOG("- Gloss");
        contentManager->Read(bitmapGloss, glossTexturePath, PixelFormat::R8G8B8A8_UNORM);
        FSLLOG("Combining diffuse and gloss texture");
        // This is a slow and brute force way of combining the textures
        for (uint32_t y = 0; y < bitmap.Height(); ++y)
        {
          for (uint32_t x = 0; x < bitmap.Width(); ++x)
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

      if (!strTextureSpecularFileName.empty())
      {
        auto specTexturePath = IO::Path::Combine(SCENE_PATH, strTextureSpecularFileName);
        contentManager->Read(bitmap, specTexturePath, PixelFormat::R8G8B8A8_UNORM);
        m_resources.TextureSpecular.Reset(bitmap, texParams, TextureFlags::GenerateMipMaps);
      }
      if (!strTextureNormalFileName.empty())
      {
        auto normTexturePath = IO::Path::Combine(SCENE_PATH, strTextureNormalFileName);
        contentManager->Read(bitmap, normTexturePath, PixelFormat::R8G8B8A8_UNORM);
        m_resources.TextureNormal.Reset(bitmap, texParams, TextureFlags::GenerateMipMaps);
      }
    }

    FSLLOG("Loading scene");
    // Default is Fast
    // aiProcessPreset_TargetRealtime_Fast
    // aiProcessPreset_TargetRealtime_Quality
    // aiProcessPreset_TargetRealtime_MaxQuality
    // | aiProcess_TransformUVCoords
    auto scenePath = IO::Path::Combine(contentPath, SCENE_PATH);
    auto modelPath = IO::Path::Combine(scenePath, strFileName);
    SceneImporter sceneImporter;
    const std::shared_ptr<TestScene> scene = sceneImporter.Load<TestScene>(modelPath, DEFAULT_MODEL_SCALE * scaleMod, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException(std::string("Scene did not contain any meshes: ") + strFileName);
    }

    m_rootNode = scene->GetRootNode();
    if (!m_rootNode)
    {
      throw NotSupportedException(std::string("Scene did not contain a root node: ") + strFileName);
    }

    PrepareShader(contentManager, !strTextureGloss.empty(), m_resources.TextureSpecular.IsValid(), m_resources.TextureNormal.IsValid());

    // Create index and vertex buffers for all the meshes.
    m_resources.IndexBuffers.Resize(scene->Meshes.size(), GL_UNSIGNED_SHORT);
    m_resources.VertexBuffers.Resize(scene->Meshes.size(), TestMesh::vertex_type::GetVertexDeclaration());
    std::size_t vertexCount = 0;
    std::size_t indexCount = 0;
    for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
    {
      auto mesh = scene->Meshes[i];
      m_resources.IndexBuffers.Reset(i, mesh->GetIndexArray(), GL_STATIC_DRAW);
      m_resources.VertexBuffers.Reset(i, mesh->GetVertexArray(), GL_STATIC_DRAW);

      vertexCount += mesh->GetVertexCount();
      indexCount += mesh->GetIndexCount();
    }
    FSLLOG("Total vertex count: " << vertexCount << ", Total index count : " << indexCount);

    m_storedStartRotation = m_rotation;
    GL_CHECK_FOR_ERROR();
  }


  ModelViewer::~ModelViewer() = default;


  void ModelViewer::OnKeyEvent(const KeyEvent& event)
  {
    switch (event.GetKey())
    {
    case VirtualKey::R:
      if (event.IsPressed())
      {
        std::swap(m_rotationSpeed, m_rotationSpeedOld);
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
        m_camera.SetZoom(DEFAULT_ZOOM);
        m_rotation = m_storedStartRotation;
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
    m_camera.AddZoom(event.GetDelta() * -0.001f);
  }


  void ModelViewer::Update(const DemoTime& demoTime)
  {
    const Point2 screenResolution = GetScreenResolution();

    m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
    m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
    m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    m_matrixWorld = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
    m_matrixView = m_camera.GetViewMatrix();
    m_matrixProjection =
      Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), screenResolution.X / static_cast<float>(screenResolution.Y), 1, 1000.0f);
    m_matrixWorldView = m_matrixWorld * m_matrixView;
    m_matrixWorldViewProjection = m_matrixWorldView * m_matrixProjection;

    Vector4 cameraSpaceLightDirection = Vector4::Transform(m_lightDirection, m_camera.GetRotationMatrix());
    m_cameraSpaceLightDirection = Vector3(cameraSpaceLightDirection.X, cameraSpaceLightDirection.Y, cameraSpaceLightDirection.Z);
    m_cameraSpaceLightDirection.Normalize();
    m_matrixNormal = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_matrixWorldView)));
  }


  void ModelViewer::Draw(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    if (m_allowBackfaceCull)
    {
      glEnable(GL_CULL_FACE);
    }
    else
    {
      glDisable(GL_CULL_FACE);
    }

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(m_resources.Program.Get());

      // Select Our Texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_resources.Texture.Get());
      if (m_resources.TextureSpecular.IsValid() && m_resources.LocTextureSpecular != GLValues::INVALID_LOCATION)
      {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_resources.TextureSpecular.Get());

        glUniform1i(m_resources.LocTextureSpecular, 1);
      }
      if (m_resources.TextureNormal.IsValid() && m_resources.LocTextureNormal != GLValues::INVALID_LOCATION)
      {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_resources.TextureNormal.Get());

        glUniform1i(m_resources.LocTextureNormal, 2);
      }

      if (m_resources.LocLightDirection != GLValues::INVALID_LOCATION)
      {
        glUniform3fv(m_resources.LocLightDirection, 1, m_cameraSpaceLightDirection.DirectAccess());
      }
      if (m_resources.LocLightColor != GLValues::INVALID_LOCATION)
      {
        glUniform3fv(m_resources.LocLightColor, 1, m_lightColor.DirectAccess());
      }
      if (m_resources.LocMatAmbient != GLValues::INVALID_LOCATION)
      {
        glUniform4fv(m_resources.LocMatAmbient, 1, m_matAmbient.DirectAccess());
      }
      if (m_resources.LocMatSpecular != GLValues::INVALID_LOCATION)
      {
        glUniform4fv(m_resources.LocMatSpecular, 1, m_matSpecular.DirectAccess());
      }
      if (m_resources.LocMatShininess != GLValues::INVALID_LOCATION)
      {
        glUniform1f(m_resources.LocMatShininess, m_matShininess);
      }
      // Load the matrices
      if (m_resources.LocWorld != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix4fv(m_resources.LocWorld, 1, 0, m_matrixWorld.DirectAccess());
      }
      if (m_resources.LocWorldView != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix4fv(m_resources.LocWorldView, 1, 0, m_matrixWorldView.DirectAccess());
      }
      if (m_resources.LocWorldViewProjection != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix4fv(m_resources.LocWorldViewProjection, 1, 0, m_matrixWorldViewProjection.DirectAccess());
      }
      if (m_resources.LocNormalMatrix != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix3fv(m_resources.LocNormalMatrix, 1, 0, m_matrixNormal.DirectAccess());
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

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCapacity(), indexBufferType, nullptr);
      }
    }
  }


  void ModelViewer::DrawMeshesAndProfile()
  {
    HighResolutionTimer timer;
    auto startTime = timer.GetTime();
    uint64_t totalTimeBind = 0;
    uint64_t totalTimeEnable = 0;
    uint64_t totalTimeDraw = 0;

    const auto indexBufferType = m_resources.IndexBuffers.GetType();

    uint64_t sequenceTimestampStart, sequenceTimestampEnd;
    for (int32_t i = 0; i < m_resources.IndexBuffers.Length(); ++i)
    {
      auto indexBuffer = m_resources.IndexBuffers.Get(i);
      auto vertexBuffer = m_resources.VertexBuffers.Get(i);
      if (indexBuffer.GetCapacity() > 0)
      {
        // Bind and enable the vertex buffer
        sequenceTimestampStart = timer.GetTime();
        glBindBuffer(m_resources.VertexBuffers.GetTarget(), vertexBuffer.Get());
        glBindBuffer(m_resources.IndexBuffers.GetTarget(), indexBuffer.Get());
        sequenceTimestampEnd = timer.GetTime();
        totalTimeBind += sequenceTimestampEnd - sequenceTimestampStart;
        sequenceTimestampStart = sequenceTimestampEnd;

        // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
        m_resources.VertexBuffers.SetVertexAttribPointers(m_resources.AttribLink);
        // m_vertexBuffers.EnableAttribArrays(m_attribLink.data(), m_attribLink.size());
        sequenceTimestampEnd = timer.GetTime();
        totalTimeEnable += sequenceTimestampEnd - sequenceTimestampStart;
        sequenceTimestampStart = sequenceTimestampEnd;

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCapacity(), indexBufferType, nullptr);

        sequenceTimestampEnd = timer.GetTime();
        totalTimeDraw += sequenceTimestampEnd - sequenceTimestampStart;
        sequenceTimestampStart = sequenceTimestampEnd;
      }
    }
    auto endTime = timer.GetTime();
    auto totalTime = endTime - startTime;
    // FSLLOG("DrawCalls: " << m_indexBuffers.Length() << " Time: " << totalTime << " bind: " << totalTimeBind << " enable: " << totalTimeEnable << "
    // draw: " << totalTimeDraw);

    {
      const auto val1 = static_cast<int32_t>(totalTimeBind);
      const auto val2 = static_cast<int32_t>(totalTimeEnable);
      const auto val3 = static_cast<int32_t>(totalTimeDraw);
      const auto val4 = static_cast<int32_t>(totalTime);
      m_profilerService->Set(m_hCounterBind, val1);
      m_profilerService->Set(m_hCounterEnable, val2);
      m_profilerService->Set(m_hCounterDraw, val3);
      m_profilerService->Set(m_hCounterTotal, val4);
    }
  }


  void ModelViewer::DrawMeshesUsingNodes(const SceneNode* pNode, const Matrix& parentMatrix)
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
      if (m_resources.LocWorld != GLValues::INVALID_LOCATION)
      {
        glUniformMatrix4fv(m_resources.LocWorld, 1, 0, currentMatrix.DirectAccess());
      }
      // if (m_locWorldView != GLValues::INVALID_LOCATION)
      //  glUniformMatrix4fv(m_locWorldView, 1, 0, currentMatrix.DirectAccess());
      if (m_resources.LocWorldViewProjection != GLValues::INVALID_LOCATION)
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

          glDrawElements(GL_TRIANGLES, indexBuffer.GetCapacity(), indexBufferType, nullptr);
        }
      }
    }
  }


  void ModelViewer::PrepareShader(const std::shared_ptr<IContentManager>& contentManager, const bool useGlossMap, const bool useSpecularMap,
                                  const bool useNormalMap)
  {
    std::string shaderPath = "Shaders";

    std::string baseShaderName;
    // baseShaderName = "LightPerVertexDirectional";
    // baseShaderName = "LightPerVertexDirectionalTextured";
    // baseShaderName = "LightPerVertexDirectionalSpecular";
    // baseShaderName = "LightPerVertexDirectionalSpecularTextured";
    // baseShaderName = "PerPixelDirectional";
    // baseShaderName = "PerPixelDirectionalTextured";
    // baseShaderName = "PerPixelDirectionalSpecular";
    baseShaderName = "PerPixelDirectionalSpecular";
    if (useSpecularMap)
    {
      baseShaderName += "Map";
    }
    if (useGlossMap)
    {
      baseShaderName += "GlossMap";
    }
    if (useNormalMap)
    {
      baseShaderName += "NormalMap";
    }
    baseShaderName += "Textured";

    // auto strFragShaderName = useTexture ? "BasicShaderDLightTextured.frag" : "BasicShaderDLight.frag";
    const IO::Path strVertShaderFilename(baseShaderName + ".vert");
    const IO::Path strFragShaderFilename(baseShaderName + ".frag");
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

    auto vertexDecl = TestMesh::vertex_type::GetVertexDeclaration();
    m_resources.AttribLink[0] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexPosition"),
                                                   vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    m_resources.AttribLink[1] =
      GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
    m_resources.AttribLink[2] =
      GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    m_resources.AttribLink[3] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexTexCoord"),
                                                   vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    if (useNormalMap)
    {
      m_resources.AttribLink[4] = GLVertexAttribLink(m_resources.Program.GetAttribLocation("VertexTangent"),
                                                     vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Tangent, 0));
    }
    else
    {
      m_resources.AttribLink[4] = GLVertexAttribLink(-1, 0);
    }
  }
}
