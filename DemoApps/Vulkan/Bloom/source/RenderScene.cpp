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

#include "RenderScene.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslDemoApp/Base/Service/Texture/ITextureService.hpp>
#include <FslGraphics3D/SceneFormat/BasicSceneFormat.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTangentTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>

namespace Fsl
{
  using BasicMesh = Graphics3D::GenericMesh<VertexPositionNormalTangentTexture, uint16_t>;
  using BasicScene = Graphics3D::GenericScene<BasicMesh>;

  namespace
  {
    const auto MODELS_PATH = "Models";

    const uint32_t VERTEX_BUFFER_BIND_ID = 0;

    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Texture& texture,
                                    const VkFilter filter, const VkSamplerAddressMode addressMode)
    {
      Vulkan::VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = filter;
      samplerCreateInfo.minFilter = filter;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = addressMode;
      samplerCreateInfo.addressModeV = addressMode;
      samplerCreateInfo.addressModeW = addressMode;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = static_cast<float>(texture.GetLevels());
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(texture, samplerCreateInfo);
    }

    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Bitmap& bitmap,
                                    const VkFilter filter, const VkSamplerAddressMode addressMode, const bool generateMipMaps,
                                    ITextureService* pTextureService)
    {
      if (generateMipMaps && pTextureService != nullptr)
      {
        FSLLOG3_INFO("- Generating mipmaps");
        Optional<Texture> result = pTextureService->TryGenerateMipMaps(bitmap, TextureMipMapFilter::Box);
        if (result.HasValue())
        {
          FSLLOG3_INFO("- Creating texture");
          return CreateTexture(device, deviceQueue, *result, filter, addressMode);
        }

        FSLLOG3_WARNING("- Failed to generate mipmaps");
      }

      FSLLOG3_INFO("- Creating texture");
      Vulkan::VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = filter;
      samplerCreateInfo.minFilter = filter;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = addressMode;
      samplerCreateInfo.addressModeV = addressMode;
      samplerCreateInfo.addressModeW = addressMode;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(bitmap, samplerCreateInfo);
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 4> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 1 : Fragment shader image sampler
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 2 : Fragment shader image sampler
      setLayoutBindings[2].binding = 2;
      setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[2].descriptorCount = 1;
      setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 3 : Fragment shader image sampler
      setLayoutBindings[3].binding = 3;
      setLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[3].descriptorCount = 1;
      setLayoutBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return RapidVulkan::DescriptorSetLayout(device.Get(), descriptorLayout);
    }

    Vulkan::VUBufferMemory CreateUBO(const Vulkan::VUDevice& device, const VkDeviceSize size)
    {
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = size;
      bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      Vulkan::VUBufferMemory buffer(device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      // Keep the buffer mapped as we update it each frame
      buffer.Map();
      return buffer;
    }


    VkDescriptorSet CreateDescriptorSet(const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout)
    {
      assert(descriptorPool.IsValid());
      assert(descriptorSetLayout.IsValid());

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet = nullptr;
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      return descriptorSet;
    }


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                        const Vulkan::VUTexture& texture, const Vulkan::VUTexture& textureNormal,
                                        const Vulkan::VUTexture& textureSpecular)

    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(vertUboBuffer.IsValid());
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 4> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      assert(texture.IsValid());
      // Binding 1 : Fragment shader texture sampler
      auto textureImageInfo1 = texture.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo1;

      // Binding 2 : Fragment shader texture sampler
      const auto& rTexture2 = textureNormal.IsValid() ? textureNormal : texture;
      auto textureImageInfo2 = rTexture2.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[2].pImageInfo = &textureImageInfo2;

      // Binding 3 : Fragment shader texture sampler
      const auto& rTexture3 = textureSpecular.IsValid() ? textureSpecular : texture;
      auto textureImageInfo3 = rTexture3.GetDescriptorImageInfo();
      writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[3].dstSet = descriptorSet;
      writeDescriptorSets[3].dstBinding = 3;
      writeDescriptorSets[3].descriptorCount = 1;
      writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[3].pImageInfo = &textureImageInfo3;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
      return descriptorSet;
    }

    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return RapidVulkan::PipelineLayout(descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo);
    }
  }


  RenderScene::RenderScene(const DemoAppConfig& config, const int32_t sceneId, const Vulkan::VUDevice& device,
                           const Vulkan::VUDeviceQueueRecord& deviceQueue, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                           const RapidVulkan::DescriptorPool& descriptorPool, const uint32_t maxFrames)
    : m_lightDirection(1.0f, 1.0f, 1.0f)
    //, m_uboData(Vector4(0.8f, 0.8f, 0.8f, 1.0f), Vector4(0.2f, 0.2f, 0.2f, 0.2f), Vector4(1, 1, 1, 1), 80.0f)
    , m_uboData(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0, 0, 0, 0), 80.0f)
  {
    m_lightDirection.Normalize();

    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();
    auto textureService = config.DemoServiceProvider.TryGet<ITextureService>();

    IO::Path strFileName;
    IO::Path strTextureFileName;
    IO::Path strTextureGloss;
    IO::Path strTextureSpecular;
    IO::Path strTextureNormal;
    switch (sceneId)
    {
    case 1:
      strFileName = "Knight2/armor.fsf";
      strTextureFileName = "Knight2/armor_default_color.jpg";
      strTextureSpecular = "Knight2/armor_default_metalness.jpg";
      strTextureNormal = "Knight2/armor_default_nmap.jpg";
      strTextureGloss = "Knight2/armor_default_rough.jpg";
      break;
    case 0:
    case 2:
      strFileName = "FuturisticCar/FuturisticCar.fsf";
      strTextureFileName = "FuturisticCar/Futuristic_Car_C.jpg";
      strTextureSpecular = "FuturisticCar/Futuristic_Car_S.jpg";
      strTextureNormal = "FuturisticCar/Futuristic_Car_N.jpg";
      break;
    default:
      throw NotSupportedException("Scene not supported");
    }

    auto contentPath = contentManager->GetContentPath();
    contentPath = IO::Path::Combine(contentPath, MODELS_PATH);
    const auto fullModelPath = IO::Path::Combine(contentPath, strFileName);

    FSLLOG3_INFO("Loading scene '{}'", fullModelPath);
    SceneFormat::BasicSceneFormat sceneFormat;
    auto scene = sceneFormat.Load<BasicScene>(fullModelPath);


    FSLLOG3_INFO("Preparing textures");
    {    // prepare textures
      const auto bitmapOrigin = BitmapOrigin::LowerLeft;

      Bitmap bitmap;
      auto texturePath = IO::Path::Combine(MODELS_PATH, strTextureFileName);

      if (strTextureGloss.IsEmpty())
      {
        FSLLOG3_INFO("- Diffuse '{}'", texturePath);
        contentManager->Read(bitmap, texturePath, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin);
      }
      else
      {
        Bitmap bitmapGloss;
        auto glossTexturePath = IO::Path::Combine(MODELS_PATH, strTextureGloss);
        FSLLOG3_INFO("- Diffuse '{}'", texturePath);
        contentManager->Read(bitmap, texturePath, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin);
        FSLLOG3_INFO("- Gloss '{}'", glossTexturePath);
        contentManager->Read(bitmapGloss, glossTexturePath, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin);
        FSLLOG3_INFO("- Combining texture");
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

      // GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      // m_texture.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);
      m_resources.Texture = CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, true, textureService.get());

      if (!strTextureSpecular.IsEmpty())
      {
        auto specTexturePath = IO::Path::Combine(MODELS_PATH, strTextureSpecular);
        FSLLOG3_INFO("- Specular '{}'", specTexturePath);
        contentManager->Read(bitmap, specTexturePath, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin);
        // m_textureSpecular.SetData(bitmap, texParams, TextureFlags::GenerateMipMaps);
        m_resources.TextureSpecular =
          CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, true, textureService.get());
      }

      if (!strTextureNormal.IsEmpty())
      {
        auto normTexturePath = IO::Path::Combine(MODELS_PATH, strTextureNormal);
        FSLLOG3_INFO("- Normal '{}'", normTexturePath);
        contentManager->Read(bitmap, normTexturePath, PixelFormat::R8G8B8A8_UNORM, bitmapOrigin);
        m_resources.TextureNormal =
          CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, true, textureService.get());
      }
    }

    FSLLOG3_INFO("Preparing shaders");
    PrepareShader(device.Get(), contentManager, m_resources.TextureSpecular.IsValid(), !strTextureGloss.IsEmpty(),
                  m_resources.TextureNormal.IsValid());

    FSLLOG3_INFO("Creating mesh");

    // Create index and vertex buffers for all the meshes.
    {
      std::size_t vertexCount = 0;
      std::size_t indexCount = 0;
      for (const auto& mesh : scene->Meshes)
      {
        vertexCount += mesh->GetVertexCount();
        indexCount += mesh->GetIndexCount();
      }
      FSLLOG3_INFO("Total vertex count: {}, Total index count: {}, SubMesh count: {}", vertexCount, indexCount, scene->GetMeshCount());

      // copy the vertices and indices to one large buffer
      std::vector<VertexPositionNormalTangentTexture> vertices(vertexCount);
      std::vector<uint16_t> indices(indexCount);
      {
        uint32_t vertexOffset = 0u;
        uint32_t indexOffset = 0u;
        const auto meshCount = scene->GetMeshCount();
        for (int32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
          const auto& mesh = scene->Meshes[meshIndex];
          const auto meshVertexCount = mesh->GetVertexCount();
          const auto meshIndexCount = mesh->GetIndexCount();
          const auto& srcVertices = mesh->GetVertexArray();
          auto startVertexOffset = vertexOffset;
          for (std::size_t i = 0; i < meshVertexCount; ++i)
          {
            vertices[vertexOffset] = srcVertices[i];
            ++vertexOffset;
          }
          const auto& srcIndices = mesh->GetIndexArray();
          for (std::size_t i = 0; i < meshIndexCount; ++i)
          {
            indices[indexOffset] = startVertexOffset + srcIndices[i];
            ++indexOffset;
          }
        }
      }

      std::array<VertexElementUsage, 4> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Normal, VertexElementUsage::Tangent,
                                                           VertexElementUsage::TextureCoordinate};

      m_resources.Mesh.VertexBuffer.Reset(bufferManager, vertices, Vulkan::VMBufferUsage::STATIC);
      m_resources.Mesh.IndexBuffer.Reset(bufferManager, indices, Vulkan::VMBufferUsage::STATIC);

      Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(m_resources.Mesh.VertexAttributeDescription, shaderBindOrder,
                                                                      m_resources.Mesh.VertexBuffer);
      m_resources.Mesh.VertexInputBindingDescription.binding = 0;
      m_resources.Mesh.VertexInputBindingDescription.stride = m_resources.Mesh.VertexBuffer.GetElementStride();
      m_resources.Mesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    {    // Prepare frame resources
      m_resources.SceneDescriptorSetLayout = CreateDescriptorSetLayout(device);

      m_resources.SceneFrameResources.resize(maxFrames);
      for (auto& rFrame : m_resources.SceneFrameResources)
      {
        rFrame.UboBuffer = CreateUBO(device, sizeof(UBOData));
        rFrame.DescriptorSet = CreateDescriptorSet(descriptorPool, m_resources.SceneDescriptorSetLayout);
        UpdateDescriptorSet(device.Get(), rFrame.DescriptorSet, rFrame.UboBuffer, m_resources.Texture, m_resources.TextureNormal,
                            m_resources.TextureSpecular);
      }
    }

    m_resources.ScenePipelineLayout = CreatePipelineLayout(m_resources.SceneDescriptorSetLayout);
  }


  RenderScene::~RenderScene() = default;


  void RenderScene::OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass hRenderPass)
  {
    m_dependentResources.Pipeline = CreatePipeline(m_resources.ScenePipelineLayout, context.SwapchainImageExtent, m_resources.VertShader.Get(),
                                                   m_resources.FragShader.Get(), m_resources.Mesh, hRenderPass, 0, true);
  }

  void RenderScene::OnFreeResources()
  {
    m_dependentResources = {};
  }


  void RenderScene::Update(const DemoTime& demoTime, const Matrix& cameraViewMatrix, const Matrix& cameraRotation, const Vector3& rotation,
                           const PxSize2D& windowSizePx)
  {
    FSL_PARAM_NOT_USED(demoTime);

    m_matrixWorld = Matrix::CreateRotationX(rotation.X) * Matrix::CreateRotationY(rotation.Y) * Matrix::CreateRotationZ(rotation.Z);
    m_matrixView = cameraViewMatrix;

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f),
                                                              windowSizePx.Width() / static_cast<float>(windowSizePx.Height()), 1, 1000.0f) *
                         vulkanClipMatrix;
    m_uboData.MatWorldView = m_matrixWorld * m_matrixView;
    m_uboData.MatWorldViewProjection = m_uboData.MatWorldView * m_matrixProjection;

    // Transform to camera space
    const auto cameraSpaceLightDirection = Vector4::Transform(m_lightDirection, cameraRotation);
    m_uboData.LightDirection = Vector4(cameraSpaceLightDirection.X, cameraSpaceLightDirection.Y, cameraSpaceLightDirection.Z, 0.0f);
    m_uboData.LightDirection.Normalize();

    m_uboData.NormalMatrix = Matrix::Transpose(Matrix::Invert(m_uboData.MatWorldView));
  }

  void RenderScene::PreDraw(const uint32_t frameIndex, const VkCommandBuffer /*hCmdBuffer*/)
  {
    // Upload the changes
    m_resources.SceneFrameResources[frameIndex].UboBuffer.Upload(0, &m_uboData, sizeof(UBOData));
  }

  void RenderScene::Draw(const uint32_t frameIndex, const VkCommandBuffer hCmdBuffer)
  {
    const auto& frame = m_resources.SceneFrameResources[frameIndex];

    vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.ScenePipelineLayout.Get(), 0, 1, &frame.DescriptorSet, 0,
                            nullptr);
    vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(hCmdBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.Mesh.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdBindIndexBuffer(hCmdBuffer, m_resources.Mesh.IndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(hCmdBuffer, m_resources.Mesh.IndexBuffer.GetIndexCount(), 1, 0, 0, 0);
  }

  void RenderScene::PrepareShader(const VkDevice device, const std::shared_ptr<IContentManager>& contentManager, const bool useSpecMap,
                                  const bool useGlossMap, const bool useNormalMap)
  {
    IO::Path shaderPath = "Shaders";

    std::string baseShaderName("PerPixelDirectionalSpecular");
    if (useSpecMap)
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

    const IO::Path strVertShaderFilename(baseShaderName + ".vert.spv");
    const IO::Path strFragShaderFilename(baseShaderName + ".frag.spv");

    const IO::Path strVertShaderPath = IO::Path::Combine(shaderPath, strVertShaderFilename);
    const IO::Path strFragShaderPath = IO::Path::Combine(shaderPath, strFragShaderFilename);

    m_resources.VertShader.Reset(device, 0, contentManager->ReadBytes(strVertShaderPath));
    m_resources.FragShader.Reset(device, 0, contentManager->ReadBytes(strFragShaderPath));
  }

  RapidVulkan::GraphicsPipeline RenderScene::CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                            const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                            const ModelMesh& mesh, const VkRenderPass renderPass, const uint32_t subpass,
                                                            const bool cullEnabled)
  {
    assert(pipelineLayout.IsValid());
    assert(vertexShaderModule != VK_NULL_HANDLE);
    assert(fragmentShaderModule != VK_NULL_HANDLE);
    assert(renderPass != VK_NULL_HANDLE);
    assert(!mesh.VertexAttributeDescription.empty());

    std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
    pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
    pipelineShaderStageCreateInfo[0].pName = "main";
    pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

    pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
    pipelineShaderStageCreateInfo[1].pName = "main";
    pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
    pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &mesh.VertexInputBindingDescription;
    pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(mesh.VertexAttributeDescription.size());
    pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = mesh.VertexAttributeDescription.data();

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{{0, 0}, extent};

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
    pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfo.viewportCount = 1;
    pipelineViewportStateCreateInfo.pViewports = &viewport;
    pipelineViewportStateCreateInfo.scissorCount = 1;
    pipelineViewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
    pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineRasterizationStateCreateInfo.cullMode = (cullEnabled ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE);
    pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
    pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
    pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
    pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
    pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
    pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
    pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    pipelineColorBlendStateCreateInfo.attachmentCount = 1;
    pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
    pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = {};
    depthStencilStateCreateInfo.back = {};
    depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

    std::array<VkDynamicState, 2> dynamicState = {VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT};
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.flags = 0;
    pipelineDynamicStateCreateInfo.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicState.size());
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState.data();

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(pipelineShaderStageCreateInfo.size());
    graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
    graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pTessellationState = nullptr;
    graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = subpass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = 0;

    return RapidVulkan::GraphicsPipeline(pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo);
  }
}
