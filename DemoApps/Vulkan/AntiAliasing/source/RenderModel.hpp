#ifndef VULKAN_ANTIALIASING_RENDERMODEL_HPP
#define VULKAN_ANTIALIASING_RENDERMODEL_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslDemoApp/Vulkan/Basic/DrawContext.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceQueueRecord.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  class ITextureService;

  class RenderModel final
  {
    struct SubMeshRecord
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      Vulkan::VMIndexBuffer IndexBuffer;
      uint32_t VertexCount{0};
      uint32_t IndexCount{0};

      SubMeshRecord() = default;
      SubMeshRecord(Vulkan::VMVertexBuffer vertexBuffer, Vulkan::VMIndexBuffer indexBuffer, const uint32_t vertexCount, const uint32_t indexCount)
        : VertexBuffer(std::move(vertexBuffer))
        , IndexBuffer(std::move(indexBuffer))
        , VertexCount(vertexCount)
        , IndexCount(indexCount)
      {
      }

      SubMeshRecord& operator=(SubMeshRecord&& other) noexcept = default;
      SubMeshRecord(SubMeshRecord&& other) noexcept = default;
    };

    struct ModelRecord
    {
      std::size_t VertexCount{0};
      std::size_t IndexCount{0};
      std::vector<SubMeshRecord> SubMeshes;

      VertexDeclaration VertexDecl;
      std::array<VkVertexInputAttributeDescription, 5> VertexAttributeDescription{};
      VkVertexInputBindingDescription VertexInputBindingDescription{};

      ModelRecord() = default;
      ModelRecord(const std::size_t vertexCount, const std::size_t indexCount, std::vector<SubMeshRecord> subMeshes,
                  VertexDeclarationSpan vertexDeclarationSpan, const std::array<VkVertexInputAttributeDescription, 5>& vertexAttributeDescription,
                  const VkVertexInputBindingDescription& vertexInputBindingDescription)
        : VertexCount(vertexCount)
        , IndexCount(indexCount)
        , SubMeshes(std::move(subMeshes))
        , VertexDecl(vertexDeclarationSpan)
        , VertexAttributeDescription(vertexAttributeDescription)
        , VertexInputBindingDescription(vertexInputBindingDescription)
      {
      }
    };

    struct MaterialTextureRecord
    {
      Vulkan::VUTexture Diffuse;
      Vulkan::VUTexture Specular;
      Vulkan::VUTexture Normal;
    };

    struct MaterialShaderRecord
    {
      RapidVulkan::ShaderModule VertShader;
      RapidVulkan::ShaderModule FragShader;
    };

    struct MaterialRecord
    {
      MaterialShaderRecord Shader;
      MaterialTextureRecord Texture;
    };

    struct ModelRenderRecord
    {
      ModelRecord Model;
      MaterialRecord Material;

      ModelRenderRecord() = default;
      ModelRenderRecord(ModelRecord model, MaterialRecord material)
        : Model(std::move(model))
        , Material(std::move(material))
      {
      }
    };

    struct Matrices
    {
      Matrix World;
      Matrix View;
      Matrix Proj;
    };

    struct UBOData
    {
      Matrix MatWorldView;
      Matrix MatWorldViewProjection;
      Matrix NormalMatrix;
      Vector4 LightDirection;
      Vector4 LightColor;
      Vector4 MatAmbient;
      Vector4 MatSpecular;
      float MatShininess{0};

      UBOData() = default;
      UBOData(const Vector4& lightColor, const Vector4& matAmbient, const Vector4& matSpecular, const float matShininess)
        : LightColor(lightColor)
        , MatAmbient(matAmbient)
        , MatSpecular(matSpecular)
        , MatShininess(matShininess)
      {
      }
    };

    struct FrameResources
    {
      Vulkan::VUBufferMemory UboBuffer;
      VkDescriptorSet DescriptorSet;
    };

    struct Resources
    {
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      RapidVulkan::PipelineLayout MainPipelineLayout;
      ModelRenderRecord ModelRecord;
      std::vector<FrameResources> MainFrameResources;

      Resources() = default;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;
    };

    struct DependentResources
    {
      RapidVulkan::GraphicsPipeline Pipeline;

      DependentResources() = default;
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      void Reset() noexcept
      {
        // Reset in destruction order
        Pipeline.Reset();
      }
    };

    Resources m_resources;
    DependentResources m_dependentResources;

    Vector3 m_rotationSpeed;
    Vector3 m_rotation;
    Matrices m_matrices;
    Vector3 m_lightDirection;

    UBOData m_uboData;

  public:
    RenderModel(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, ITextureService* pTextureService, IContentManager& contentManager,
                const IO::PathView srcModelPath, const IO::PathView shaderPath, const IO::Path& diffuseTexturePath,
                const IO::Path& specularTexturePath, const IO::Path& normalTexturePath, const float modelScale, const uint32_t maxFramesInFlight);

    void BuildResources(const VkRenderPass renderPass, const uint32_t subpass, const VkSampleCountFlagBits sampleCount);
    void FreeResources();

    void Update(const DemoTime& demoTime, const bool isPaused);
    void Draw(const VkCommandBuffer hCmdBuffer, const VulkanBasic::DrawContext& drawContext, const DemoTime& demoTime,
              const PxRectangle windowRectanglePx);

  private:
    void DrawModel(const VkCommandBuffer hCmdBuffer, const VulkanBasic::DrawContext& drawContext, const ModelRenderRecord& modelRecord,
                   const PxRectangle windowRectanglePx);

    static ModelRenderRecord LoadModel(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                       const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, ITextureService* pTextureService,
                                       IContentManager& contentManager, const IO::PathView srcModelPath, const IO::PathView shaderPath,
                                       const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                                       const float modelScale);
    static ModelRecord LoadModel(const Vulkan::VUDevice& device, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                 const IO::Path& contentPath, const IO::PathView srcModelPath, const float modelScale);

    static MaterialRecord LoadMaterial(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                       ITextureService* pTextureService, IContentManager& contentManager, const IO::PathView shaderPath,
                                       const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                                       const VertexDeclarationSpan vertexDeclarationSpan);
    static MaterialShaderRecord LoadShader(const Vulkan::VUDevice& device, IContentManager& contentManager, const IO::PathView shaderPath,
                                           const VertexDeclarationSpan vertexDeclarationSpan);
    static MaterialTextureRecord LoadTextures(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                              ITextureService* pTextureService, IContentManager& contentManager, const IO::Path& diffuseTexturePath,
                                              const IO::Path& specularTexturePath, const IO::Path& normalTexturePath);
  };
}

#endif
