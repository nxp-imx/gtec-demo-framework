#ifndef GLES3_TESSELLATIONSAMPLE_ASCENE_HPP
#define GLES3_TESSELLATIONSAMPLE_ASCENE_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTangentTexture.hpp>
#include <FslGraphics3D/Camera/ArcballCamera.hpp>
#include <FslUtil/OpenGLES3/GLIndexBufferArray.hpp>
#include <FslUtil/OpenGLES3/GLVertexBufferArray.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include "BasicShader.hpp"
#include "RenderCameraConfig.hpp"
#include "RenderDrawConfig.hpp"
#include "RenderMaterial.hpp"
#include "RenderTessellationConfig.hpp"
#include "TessellationShader.hpp"

namespace Fsl
{
  class MouseButtonEvent;
  class MouseMoveEvent;
  class MouseWheelEvent;
  class OptionParser;

  class AScene
  {
    bool m_basicRenderingEnabled;
    bool m_cullEnabled;
    Point2 m_screenResolution;
    float m_fieldOfView;

  protected:
    using vertex_type = VertexPositionNormalTangentTexture;
    Graphics3D::ArcballCamera m_camera;
    RenderTessellationConfig m_tessellationConfig;
    RenderCameraConfig m_cameraConfig;
    RenderMaterial m_material;

    BasicShader m_basicShader;
    TessellationShader m_tessShader;

    GLES3::GLIndexBufferArray m_indexBuffers;
    GLES3::GLVertexBufferArray m_vertexBuffers;
    GLES3::GLTexture m_textureDiffuse;
    GLES3::GLTexture m_textureDiffuseWhite;
    GLES3::GLTexture m_textureDisplacement;
    GLES3::GLTexture m_textureNoDisplacement;
    GLES3::GLTexture m_textureNormals;

    Vector3 m_lightDirection;
    Vector3 m_lightColor;
    Vector3 m_rotationSpeed;
    Vector3 m_rotation;

  public:
    AScene(const DemoAppConfig& config, const std::shared_ptr<OptionParser>& options);
    virtual ~AScene() = default;

    void OnKeyEvent(const KeyEvent& event);
    void OnMouseButtonEvent(const MouseButtonEvent& event);
    void OnMouseMoveEvent(const MouseMoveEvent& event);
    void OnMouseWheelEvent(const MouseWheelEvent& event);

    virtual void Update(const DemoTime& demoTime, const RenderDrawConfig& config);
    virtual void Draw(const RenderDrawConfig& config);

    virtual RenderTessellationConfig GetRenderTessellationConfig() const
    {
      return m_tessellationConfig;
    };
    virtual void SetRenderTessellationConfig(const RenderTessellationConfig& value)
    {
      m_tessellationConfig = value;
    };
    virtual RenderMaterial GetRenderMaterial() const
    {
      return m_material;
    };
    virtual void SetRenderMaterial(const RenderMaterial& value)
    {
      m_material = value;
    };

  protected:
    void SetCullEnabled(const bool value)
    {
      m_cullEnabled = value;
    }

  private:
    void DrawBasic(const RenderDrawConfig& config);
    void DrawBasicMeshes();
  };
}

#endif
