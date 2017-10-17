#ifndef GLES2_MODELVIEWER_MODELVIEWER_HPP
#define GLES2_MODELVIEWER_MODELVIEWER_HPP
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
#include <FslDemoApp/Base/Service/Profiler/IProfilerService.hpp>
#include <FslDemoApp/Base/Service/Profiler/ScopedProfilerCustomCounterHandle.hpp>
#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <FslUtil/OpenGLES2/GLIndexBufferArray.hpp>
#include <FslUtil/OpenGLES2/GLTexture.hpp>
#include <FslUtil/OpenGLES2/GLVertexBufferArray.hpp>
#include <vector>
#include <FslGraphics3D/BasicScene/Camera/ArcballCamera.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    class SceneNode;
  }

  class IContentManager;

  class ModelViewer : public DemoAppGLES2
  {
    std::shared_ptr<IProfilerService> m_profilerService;
    ScopedProfilerCustomCounterHandle m_hCounterBind;
    ScopedProfilerCustomCounterHandle m_hCounterEnable;
    ScopedProfilerCustomCounterHandle m_hCounterDraw;
    ScopedProfilerCustomCounterHandle m_hCounterTotal;
    Graphics3D::ArcballCamera m_camera;
    GLES2::GLProgram m_program;
    GLES2::GLTexture m_texture;
    GLES2::GLTexture m_textureSpecular;
    GLES2::GLTexture m_textureNormal;
    GLES2::GLIndexBufferArray m_indexBuffers;
    GLES2::GLVertexBufferArray m_vertexBuffers;

    GLint m_locWorld;
    GLint m_locWorldView;
    GLint m_locWorldViewProjection;
    GLint m_locNormalMatrix;
    GLint m_locTexture0;
    GLint m_locTextureSpecular;
    GLint m_locTextureNormal;
    GLint m_locLightDirection;
    GLint m_locLightColor;
    GLint m_locMatAmbient;
    GLint m_locMatSpecular;
    GLint m_locMatShininess;

    std::vector<GLES2::GLVertexAttribLink> m_attribLink;

    std::shared_ptr<Graphics3D::SceneNode> m_rootNode;
    Vector3 m_rotationSpeedOld;
    Vector3 m_rotationSpeed;
    Vector3 m_rotation;
    Vector3 m_storedStartRotation;
    Vector3 m_lightDirection;
    Vector3 m_lightColor;
    Vector4 m_matAmbient;
    Vector4 m_matSpecular;
    float m_matShininess;

    Matrix m_matrixWorld;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Matrix m_matrixWorldView;
    Matrix m_matrixWorldViewProjection;
    Vector3 m_cameraSpaceLightDirection;
    Matrix3 m_matrixNormal;

    bool m_allowBackfaceCull;
  public:
    ModelViewer(const DemoAppConfig& config);
    ~ModelViewer();
  protected:
    virtual void OnKeyEvent(const KeyEvent& event) override;
    virtual void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    virtual void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    virtual void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void DrawMeshes();
    void DrawMeshesAndProfile();
    void DrawMeshesUsingNodes(const Graphics3D::SceneNode* pNode, const Matrix& parentMatrix);

    void PrepareShader(const std::shared_ptr<IContentManager>& contentManager, const bool useGlossMap, const bool useSpecularMap, const bool useNormalMap);
  };
}

#endif
