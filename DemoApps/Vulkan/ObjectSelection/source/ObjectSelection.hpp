#ifndef VULKAN_OBJECTSELECTION_OBJECTSELECTION_HPP
#define VULKAN_OBJECTSELECTION_OBJECTSELECTION_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include <FslBase/Math/Matrix3Std140.hpp>
#include <FslBase/Math/Viewport.hpp>
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>
#include <FslUtil/Vulkan1_0/Managed/Draw/VMLineDraw.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUSegmentedBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/ObjectSelection/MenuUI.hpp>
#include <array>
#include <deque>
#include <utility>

namespace Fsl
{
  class ObjectSelection : public VulkanBasic::DemoAppVulkanBasic
  {
    struct ProjectionUBOData
    {
      Matrix MatViewProjection;
    };

    struct LightUBOData
    {
      Vector4 LightDirection;
      Vector4 LightColor;
      Vector4 AmbientColor;
    };

    struct ObjUboData
    {
      // Matrix WorldView;
      Matrix WorldViewProjection;
      Matrix3Std140 NormalMatrix;
    };

  public:
    struct Mesh
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      Vulkan::VMIndexBuffer IndexBuffer;
      std::array<VkVertexInputAttributeDescription, 3> VertexAttributeDescription{};
      VkVertexInputBindingDescription VertexInputBindingDescription{};

      BoundingBox TheBoundingBox;
    };

  private:
    struct ProgramDirectionalLight
    {
      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;
    };

    struct ProgramColor
    {
      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;
    };

    struct MeshObject
    {
      uint32_t MeshIndex = 0;
      Matrix WorldMatrix;
      Matrix WorldViewMatrix;
      Matrix WorldViewProjectionMatrix;
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

    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      Vulkan::VUBufferMemory PlaneVertUboBuffer;
      VkDescriptorSet PlaneDescriptorSet;
      Vulkan::VUSegmentedBufferMemory ObjectTransformVertUboBuffer;
      VkDescriptorSet ObjectTransformDescriptorSet;
      Vulkan::VUBufferMemory ObjectVertUboBuffer;
      VkDescriptorSet ObjectDescriptorSet;
    };

    struct Resources
    {
      std::shared_ptr<Vulkan::VMBufferManager> BufferManager;

      Vulkan::VUTexture Texture;
      Vulkan::VUTexture TextureChessboard;

      std::vector<Mesh> Meshes;
      Mesh MeshPlane;
      MeshObject PlaneObject;

      ProgramDirectionalLight ProgDirectionalLight;

      VertexDeclaration VertexDeclLine;
      ProgramColor ProgramSolidColor;

      ProgramColor ProgramTextured;

      std::vector<MeshObject> Objects;

      Graphics3D::LineBuilder LineBuild;
      Vulkan::VMLineDraw LineDraw;

      RapidVulkan::DescriptorSetLayout PlaneDescriptorSetLayout;
      RapidVulkan::DescriptorSetLayout ObjectTransformDescriptorSetLayout;
      RapidVulkan::DescriptorSetLayout ObjectDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::vector<FrameResources> MainFrameResources;
      RapidVulkan::PipelineLayout PlanePipelineLayout;
      RapidVulkan::PipelineLayout ObjectPipelineLayout;

      Resources(const std::size_t meshCapacity, VertexDeclaration vertexDeclaration)
        : Meshes(meshCapacity)
        , VertexDeclLine(std::move(vertexDeclaration))
      {
      }
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline PlanePipeline;
      RapidVulkan::GraphicsPipeline ObjectPipeline;
    };

    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled;
    bool m_rightMouseDown = false;
    MenuUI m_menuUI;
    Graphics3D::FirstPersonCamera m_camera;
    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;

    Resources m_resources;
    DependentResources m_dependentResources;

    Viewport m_viewPort;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Matrix m_matrixViewProjection;

    Vector3 m_mousePositionNear;
    Vector3 m_mousePositionFar;

    std::deque<HitRecord> m_pickScratchpad;

    bool m_hasSelectedObject;
    std::size_t m_selectedIndex;

    ProjectionUBOData m_linesVertexUboData;
    ProjectionUBOData m_planeVertexUboData;

    std::vector<ObjUboData> m_objVertexUboData;
    LightUBOData m_lightFragUboData;

  public:
    explicit ObjectSelection(const DemoAppConfig& config);

  protected:
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnKeyEvent(const KeyEvent& event) override;

    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

  private:
    void UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState);

    bool CheckCollision(const PxPoint2& screenSpacePosition);

    void DrawMeshes(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void DrawPlaneMesh(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void DrawMesh(const Mesh& mesh, const bool bindMesh, const VkCommandBuffer commandBuffer);

    void DrawDebugData(const VkCommandBuffer commandBuffer, const uint32_t frameIndex);

    void CreateTextures(const std::shared_ptr<IContentManager>& contentManager);

    static Mesh PreparePlaneMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const Vulkan::VUTexture& texture);
    static void PrepareMeshes(std::vector<Mesh>& rMeshes, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                              const Vulkan::VUTexture& texture);
    static Mesh CreatePlaneMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const Procedural::BasicMesh& mesh);
    static Mesh CreateMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const Procedural::BasicMesh& mesh);
    static ProgramDirectionalLight PrepareDirectionalLightProgram(const VkDevice device, const std::shared_ptr<IContentManager>& contentManager);
    static ProgramColor PrepareSolidColorProgram(const VkDevice device, const std::shared_ptr<IContentManager>& contentManager);
    static ProgramColor PrepareTexturedProgram(const VkDevice device, const std::shared_ptr<IContentManager>& contentManager);
    static void GenerateObjects(std::vector<MeshObject>& rObjects, const uint32_t objectCount, const bool useRandomSeed);
  };
}

#endif
