#ifndef GLES3_MODELVIEWER_MODELVIEWER_HPP
#define GLES3_MODELVIEWER_MODELVIEWER_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslDemoService/Profiler/ScopedProfilerCustomCounterHandle.hpp>
#include <FslGraphics3D/Camera/ArcballCamera.hpp>
#include <FslUtil/OpenGLES3/GLIndexBufferArray.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBufferArray.hpp>
#include <Shared/ModelViewer/ModelSceneUtil.hpp>
#include <Shared/ModelViewer/OptionParser.hpp>
#include <array>

namespace Fsl
{
  namespace Graphics3D
  {
    class SceneNode;
  }

  class ModelViewer final : public DemoAppGLES3
  {
    using base_type = DemoAppGLES3;

    struct Resources
    {
      GLES3::GLProgram Program;
      GLES3::GLTexture Texture;
      GLES3::GLTexture TextureSpecular;
      GLES3::GLTexture TextureNormal;
      GLES3::GLIndexBufferArray IndexBuffers;
      GLES3::GLVertexBufferArray VertexBuffers;

      GLint LocWorld = GLES3::GLValues::InvalidLocation;
      GLint LocWorldView = GLES3::GLValues::InvalidLocation;
      GLint LocWorldViewProjection = GLES3::GLValues::InvalidLocation;
      GLint LocNormalMatrix = GLES3::GLValues::InvalidLocation;
      GLint LocTexture0 = GLES3::GLValues::InvalidLocation;
      GLint LocTextureSpecular = GLES3::GLValues::InvalidLocation;
      GLint LocTextureNormal = GLES3::GLValues::InvalidLocation;
      GLint LocLightDirection = GLES3::GLValues::InvalidLocation;
      GLint LocLightColor = GLES3::GLValues::InvalidLocation;
      GLint LocMatAmbient = GLES3::GLValues::InvalidLocation;
      GLint LocMatSpecular = GLES3::GLValues::InvalidLocation;
      GLint LocMatShininess = GLES3::GLValues::InvalidLocation;

      std::array<GLES3::GLVertexAttribLink, 5> AttribLink;
    };

    struct VertexUBOData
    {
      Matrix MatWorldView;
      Matrix MatWorldViewProjection;
      Matrix3 MatNormal;
    };

    struct FragUBOData
    {
      Vector3 LightDirection;
      Vector3 LightColor;
      Vector4 MatAmbient;
      Vector4 MatSpecular;
      float MatShininess{0};

      FragUBOData() = default;
      FragUBOData(const Vector3& lightColor, const Vector4& matAmbient, const Vector4& matSpecular, const float matShininess)
        : LightColor(lightColor)
        , MatAmbient(matAmbient)
        , MatSpecular(matSpecular)
        , MatShininess(matShininess)
      {
      }
    };

    std::shared_ptr<IProfilerService> m_profilerService;
    ScopedProfilerCustomCounterHandle m_hCounterBind;
    ScopedProfilerCustomCounterHandle m_hCounterEnable;
    ScopedProfilerCustomCounterHandle m_hCounterDraw;
    ScopedProfilerCustomCounterHandle m_hCounterTotal;
    Graphics3D::ArcballCamera m_camera;

    Resources m_resources;

    std::shared_ptr<Graphics3D::SceneNode> m_rootNode;
    ModelSceneUtil::RenderConfig m_renderConfig;
    Vector3 m_rotationSpeedOld;
    Vector3 m_storedStartRotation;

    Matrix m_matrixWorld;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Vector3 m_lightDirection;

    VertexUBOData m_vertexUboData;
    FragUBOData m_fragUboData;

    bool m_wireframe{false};

  public:
    explicit ModelViewer(const DemoAppConfig& config);
    ~ModelViewer() final;

  protected:
    void OnKeyEvent(const KeyEvent& event) final;
    void OnMouseButtonEvent(const MouseButtonEvent& event) final;
    void OnMouseMoveEvent(const MouseMoveEvent& event) final;
    void OnMouseWheelEvent(const MouseWheelEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void Draw(const FrameInfo& frameInfo) final;

  private:
    void DrawMeshes();
    void DrawMeshesAndProfile();
    void DrawMeshesUsingNodes(const Graphics3D::SceneNode* pNode, const Matrix& parentMatrix);

    void PrepareShader(const std::shared_ptr<IContentManager>& contentManager, const bool useDiffuse, const bool useGlossMap,
                       const bool useSpecularMap, const bool useNormalMap, const std::string& baseShaderName, const bool requireVertexNormal = true);

    bool PrepareTextures(const IContentManager& contentManager, const ModelSceneUtil::ModelLoaderConfig& config);
  };
}

#endif
