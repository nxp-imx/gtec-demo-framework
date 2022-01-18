/*
 * Vulkan Example - Dynamic terrain tessellation
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'DynamicTerrainTesselation' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)


#include "DynamicTerrainTessellation.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/Sampler.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Fsl
{
  namespace
  {
    const uint32_t VERTEX_BUFFER_BIND_ID = 0;
    const uint32_t PATCH_SIZE = 64;
    const float UV_SCALE = 1.0f;

#ifdef __ANDROID__
    const float TESSELLATION_FACTOR = 0.20f;
#else
    const float TESSELLATION_FACTOR = 0.75f;
#endif


    // Vertex layout for this example
    const std::vector<Willems::MeshLoader::VertexLayout> g_vertexLayout = {Willems::MeshLoader::VertexLayout::VERTEX_LAYOUT_POSITION,
                                                                           Willems::MeshLoader::VertexLayout::VERTEX_LAYOUT_NORMAL,
                                                                           Willems::MeshLoader::VertexLayout::VERTEX_LAYOUT_UV};

    // Encapsulate height map data for easy sampling
    struct HeightMap
    {
    private:
      uint32_t Dim;
      std::vector<uint16_t> HeightData;
      uint32_t Scale;

    public:
      HeightMap(const Bitmap& srcBitmap, const uint32_t patchSize)
        : Dim(srcBitmap.GetExtent().Width)
        , HeightData(Dim * Dim)
        , Scale(Dim / patchSize)
      {
        assert(srcBitmap.GetExtent().Width == srcBitmap.GetExtent().Height);

        RawBitmap rawBitmap;
        Bitmap::ScopedDirectAccess directAccess(srcBitmap, rawBitmap);

        assert(PixelFormatUtil::CalcMinimumStride(rawBitmap.Width(), rawBitmap.GetPixelFormat()) == rawBitmap.Stride());
        assert(rawBitmap.GetByteSize() <= (HeightData.size() * sizeof(uint16_t)));
        std::memcpy(HeightData.data(), rawBitmap.Content(), rawBitmap.GetByteSize());
      };

      float GetHeight(const uint32_t x, const uint32_t y) const
      {
        assert(x < Dim);
        assert(y < Dim);

        glm::ivec2 rpos = glm::ivec2(x, y) * glm::ivec2(Scale);
        rpos.x = std::max(0, std::min(rpos.x, static_cast<int>(Dim) - 1));
        rpos.y = std::max(0, std::min(rpos.y, static_cast<int>(Dim) - 1));
        rpos /= glm::ivec2(Scale);
        return float(*(HeightData.data() + (rpos.x + (rpos.y * Dim)) * Scale)) / 65535.0f;
      }
    };
  }


  DynamicTerrainTessellation::DynamicTerrainTessellation(const DemoAppConfig& config)
    : VulkanWillemsMeshDemoApp(config)
    , m_tessellation(true)
    , m_wireframe(false)
  {
    m_pipelineStats[0] = 0;
    m_pipelineStats[1] = 0;

    m_uboTess.TessellationFactor = TESSELLATION_FACTOR;

    const auto screenExtent = GetScreenExtent();

    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Dynamic terrain tessellation";

    m_camera.Type = Willems::Camera::CameraType::FirstPerson;
    m_camera.SetPerspective(60.0f, static_cast<float>(screenExtent.Width) / static_cast<float>(screenExtent.Height), 0.1f, 512.0f);
    m_camera.SetRotation(glm::vec3(-12.0f, 159.0f, 0.0f));
    m_camera.SetTranslation(glm::vec3(18.0f, 22.5f, 57.5f));

    m_camera.MovementSpeed = 7.5f;
    // Support for tessellation shaders is optional, so check first
    if (m_deviceFeatures.tessellationShader == VK_FALSE)
    {
      throw NotSupportedException("Selected GPU does not support tessellation shaders!");
    }

    FSLLOG3_WARNING_IF(!m_deviceFeatures.fillModeNonSolid, "Selected GPU does not support non solid fill mode");
    FSLLOG3_WARNING_IF(!m_deviceFeatures.pipelineStatisticsQuery, "Selected GPU does not support pipeline statistics query");
  }


  DynamicTerrainTessellation::~DynamicTerrainTessellation()
  {
    SafeWaitForDeviceIdle();
  }


  void DynamicTerrainTessellation::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }


    if (event.IsPressed())
    {
      switch (event.GetKey())
      {
      case VirtualKey::UpArrow:
      case VirtualKey::Add:
      case VirtualKey::GamePadButtonRightShoulder:
        ChangeTessellationFactor(0.05f);
        break;
      case VirtualKey::DownArrow:
      case VirtualKey::Subtract:
      case VirtualKey::GamePadButtonLeftShoulder:
        ChangeTessellationFactor(-0.05f);
        break;
      case VirtualKey::F:
      case VirtualKey::GamePadButtonA:
        ToggleWireframe();
        break;
      case VirtualKey::T:
      case VirtualKey::GamePadButtonX:
        ToggleTessellation();
        break;
      default:
        break;
      }
    }

    VulkanWillemsMeshDemoApp::OnKeyEvent(event);
  }


  // This ensures the flow is the same as in the original sample
  void DynamicTerrainTessellation::Prepare()
  {
    VulkanWillemsMeshDemoApp::Prepare();
    LoadMeshes();
    LoadTextures();
    GenerateTerrain();
    if (m_deviceFeatures.pipelineStatisticsQuery != VK_FALSE)
    {
      SetupQueryResultBuffer();
    }
    SetupVertexDescriptions();
    PrepareUniformBuffers();
    SetupDescriptorSetLayouts();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSets();
    BuildCommandBuffers();
  }


  void DynamicTerrainTessellation::GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay)
  {
    const auto screenExtent = GetScreenExtent();

    std::stringstream ss;
    ss << std::setprecision(2) << std::fixed << m_uboTess.TessellationFactor;

    const auto screenWidth = static_cast<float>(screenExtent.Width);

    rTextOverlay.AddText("Tessellation factor: " + ss.str() + " (numpad +/-)", 5.0f, 85.0f, Willems::VulkanTextOverlay::TextAlign::Left);
    if (m_deviceFeatures.fillModeNonSolid != VK_FALSE)
    {
      rTextOverlay.AddText("Press 'f' to toggle wireframe", 5.0f, 100.0f, Willems::VulkanTextOverlay::TextAlign::Left);
    }
    rTextOverlay.AddText("Press 't' to toggle tessellation", 5.0f, 115.0f, Willems::VulkanTextOverlay::TextAlign::Left);
    rTextOverlay.AddText("pipeline stats:", screenWidth - 5.0f, 5.0f, Willems::VulkanTextOverlay::TextAlign::Right);
    if (m_deviceFeatures.pipelineStatisticsQuery != VK_FALSE)
    {
      rTextOverlay.AddText("VS:" + ToString(m_pipelineStats[0]), screenWidth - 5.0f, 20.0f, Willems::VulkanTextOverlay::TextAlign::Right);
      rTextOverlay.AddText("TE:" + ToString(m_pipelineStats[1]), screenWidth - 5.0f, 35.0f, Willems::VulkanTextOverlay::TextAlign::Right);
    }
  }


  void DynamicTerrainTessellation::OnViewChanged()
  {
    VulkanWillemsMeshDemoApp::OnViewChanged();

    UpdateUniformBuffers();
  }


  void DynamicTerrainTessellation::BuildCommandBuffers()
  {
    const auto screenExtent = TypeConverter::UncheckedTo<VkExtent2D>(GetScreenExtent());

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = m_defaultClearColor;
    clearValues[1].color = {{0.2f, 0.2f, 0.2f, 0.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    VkViewport viewport{};
    viewport.width = static_cast<float>(screenExtent.width);
    viewport.height = static_cast<float>(screenExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = screenExtent;

    for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
    {
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      {
        m_drawCmdBuffers.Begin(i, cmdBufInfo);

        if (m_deviceFeatures.pipelineStatisticsQuery != VK_FALSE)
        {
          vkCmdResetQueryPool(m_drawCmdBuffers[i], m_queryPool.Get(), 0, 2);
        }

        {
          vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
          vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);
          vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);
          vkCmdSetLineWidth(m_drawCmdBuffers[i], 1.0f);

          VkDeviceSize offsets = 0;

          // Skysphere
          vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Skysphere.Get());
          vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayouts.Skysphere.Get(), 0, 1,
                                  &m_descriptorSets.Skysphere, 0, nullptr);
          vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_meshes.Skysphere.GetVertices().GetBufferPointer(), &offsets);
          vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_meshes.Skysphere.GetIndices().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
          vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Skysphere.GetIndexCount(), 1, 0, 0, 0);

          // Terrain
          if (m_deviceFeatures.pipelineStatisticsQuery != VK_FALSE)
          {
            // Begin pipeline statistics query
            vkCmdBeginQuery(m_drawCmdBuffers[i], m_queryPool.Get(), 0, VK_QUERY_CONTROL_PRECISE_BIT);
          }
          // Render
          vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_wireframe ? m_pipelines.Wireframe.Get() : m_pipelines.Terrain.Get());
          vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayouts.Terrain.Get(), 0, 1,
                                  &m_descriptorSets.Terrain, 0, nullptr);
          vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_meshes.Terrain.GetVertices().GetBufferPointer(), &offsets);
          vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_meshes.Terrain.GetIndices().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
          vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Terrain.GetIndexCount(), 1, 0, 0, 0);
          if (m_deviceFeatures.pipelineStatisticsQuery != VK_FALSE)
          {
            // End pipeline statistics query
            vkCmdEndQuery(m_drawCmdBuffers[i], m_queryPool.Get(), 0);
          }

          DrawUI(m_drawCmdBuffers[i]);
          vkCmdEndRenderPass(m_drawCmdBuffers[i]);
        }

        RAPIDVULKAN_CHECK(vkEndCommandBuffer(m_drawCmdBuffers[i]));
      }
    }
  }


  void DynamicTerrainTessellation::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
  }


  void DynamicTerrainTessellation::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    if (!TryPrepareFrame())
    {
      return;
    }

    m_submitInfo.commandBufferCount = 1;
    m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

    m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

    SubmitFrame();

    if (m_deviceFeatures.pipelineStatisticsQuery != VK_FALSE)
    {
      // Read query results for displaying in next frame
      GetQueryResults();
    }
  }


  void DynamicTerrainTessellation::LoadMeshes()
  {
    FSLLOG3_INFO("LoadMeshes");
    m_meshes.Skysphere = LoadMesh("Models/Geosphere/geosphere.obj", g_vertexLayout, 1.0f);
  }


  void DynamicTerrainTessellation::LoadTextures()
  {
    FSLLOG3_INFO("LoadTextures");

    if (m_deviceFeatures.textureCompressionBC != VK_FALSE)
    {
      FSLLOG3_INFO("Using BC compression");
      m_textures.SkySphere = m_textureLoader->LoadTexture("Textures/SphereMap/Sky/skysphere_bc3.ktx");
      // Terrain textures are stored in a texture array with layers corresponding to terrain height
      m_textures.TerrainArray = m_textureLoader->LoadTextureArray("Textures/Terrain/terrain_texturearray_bc3.ktx");
    }
    else if (m_deviceFeatures.textureCompressionETC2 != VK_FALSE)
    {
      FSLLOG3_INFO("Using ETC2 compression");
      m_textures.SkySphere = m_textureLoader->LoadTexture("Textures/SphereMap/Sky/skysphere_etc2.ktx");
      // Terrain textures are stored in a texture array with layers corresponding to terrain height
      m_textures.TerrainArray = m_textureLoader->LoadTextureArray("Textures/Terrain/terrain_texturearray_etc2.ktx");
    }
    else
    {
      FSLLOG3_INFO("Using no compression");
      m_textures.SkySphere = m_textureLoader->LoadTexture("Textures/SphereMap/Sky/skysphere.png", VK_FORMAT_R8G8B8A8_UNORM);
      // Terrain textures are stored in a texture array with layers corresponding to terrain height
      m_textures.TerrainArray = m_textureLoader->LoadTextureArray("Textures/Terrain/terrain_texturearray.ktx", VK_FORMAT_R8G8B8A8_UNORM);
    }

    // Height data is stored in a one-channel texture
    VkImageFormatProperties imageFormatProperties{};
    if (m_physicalDevice.TryGetPhysicalDeviceImageFormatProperties(VK_FORMAT_R16_UNORM, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
                                                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 0, imageFormatProperties))
    {
      FSLLOG3_INFO("Heightmap using R16");
      m_textures.HeightMap = m_textureLoader->LoadTexture("Textures/Terrain/terrain_heightmap_r16.ktx", VK_FORMAT_R16_UNORM);
    }
    else
    {
      FSLLOG3_INFO("Heightmap using R8");
      m_textures.HeightMap = m_textureLoader->LoadTexture("Textures/Terrain/terrain_heightmap_r8.ktx", VK_FORMAT_R8_UNORM);
    }

    // Setup a mirroring sampler for the height map
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    samplerInfo.addressModeV = samplerInfo.addressModeU;
    samplerInfo.addressModeW = samplerInfo.addressModeU;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(m_textures.HeightMap.GetLevels());
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    RapidVulkan::Sampler heightSampler(m_device.Get(), samplerInfo);
    m_textures.HeightMap.SetSampler(std::move(heightSampler));


    samplerInfo = VkSamplerCreateInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = samplerInfo.addressModeU;
    samplerInfo.addressModeW = samplerInfo.addressModeU;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(m_textures.TerrainArray.GetLevels());
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    if (m_deviceFeatures.samplerAnisotropy != VK_FALSE)
    {
      samplerInfo.maxAnisotropy = 4.0f;
      samplerInfo.anisotropyEnable = VK_TRUE;
    }
    RapidVulkan::Sampler terrainSampler(m_device.Get(), samplerInfo);
    m_textures.TerrainArray.SetSampler(std::move(terrainSampler));
  }


  void DynamicTerrainTessellation::GenerateTerrain()
  {
    FSLLOG3_INFO("GenerateTerrain");

    struct Vertex
    {
      glm::vec3 pos;
      glm::vec3 normal;
      glm::vec2 uv;
    };


    std::vector<Vertex> vertices(PATCH_SIZE * PATCH_SIZE * 4);

    const float wx = 2.0f;
    const float wy = 2.0f;

    {
      auto* pDst = vertices.data();
      for (uint32_t x = 0; x < PATCH_SIZE; ++x)
      {
        for (uint32_t y = 0; y < PATCH_SIZE; ++y)
        {
          const uint32_t index = (x + y * PATCH_SIZE);
          assert(index < vertices.size());
          pDst[index].pos[0] = (x * wx) + (wx / 2.0f) - ((static_cast<float>(PATCH_SIZE) * wx) / 2.0f);
          pDst[index].pos[1] = 0.0f;
          pDst[index].pos[2] = (y * wy) + (wy / 2.0f) - ((static_cast<float>(PATCH_SIZE) * wy) / 2.0f);
          pDst[index].uv = glm::vec2(static_cast<float>(x) / PATCH_SIZE, static_cast<float>(y) / PATCH_SIZE) * UV_SCALE;
        }
      }
    }

    const Bitmap heightBitmap = GetContentManager()->ReadBitmap("Textures/Terrain/terrain_heightmap_r16.ktx");

    // Calculate normals from height map using a sobel filter
    HeightMap heightMap(heightBitmap, PATCH_SIZE);
    for (uint32_t x = 0; x < PATCH_SIZE; ++x)
    {
      for (uint32_t y = 0; y < PATCH_SIZE; ++y)
      {
        // Get height samples centered around current position
        float heights[3][3]{};    // NOLINT(modernize-avoid-c-arrays)
        for (uint32_t hx = -1; hx <= 1; ++hx)
        {
          for (uint32_t hy = -1; hy <= 1; ++hy)
          {
            heights[hx + 1][hy + 1] = heightMap.GetHeight(x + hx, y + hy);
          }
        }

        // Calcualte the normal
        glm::vec3 normal;
        // Gx sobel filter
        normal.x = heights[0][0] - heights[2][0] + 2.0f * heights[0][1] - 2.0f * heights[2][1] + heights[0][2] - heights[2][2];
        // Gy sobel filter
        normal.z = heights[0][0] + 2.0f * heights[1][0] + heights[2][0] - heights[0][2] - 2.0f * heights[1][2] - heights[2][2];
        // Calculate missing up component of the normal using the filtered x and y axis
        // The first value controls the bump strength
        normal.y = 0.25f * std::sqrt(1.0f - normal.x * normal.x - normal.z * normal.z);

        vertices[x + y * PATCH_SIZE].normal = glm::normalize(normal * glm::vec3(2.0f, 1.0f, 2.0f));
      }
    }

    // Indices
    const uint32_t w = (PATCH_SIZE - 1);
    std::vector<uint32_t> indices(w * w * 4);
    {
      auto* pDst = indices.data();
      for (uint32_t x = 0; x < w; ++x)
      {
        for (uint32_t y = 0; y < w; ++y)
        {
          const uint32_t index = (x + y * w) * 4;
          assert((index + 3) < indices.size());
          pDst[index] = (x + y * PATCH_SIZE);
          pDst[index + 1] = indices[index] + PATCH_SIZE;
          pDst[index + 2] = indices[index + 1] + 1;
          pDst[index + 3] = indices[index] + 1;
        }
      }
    }
    // FIX:
    // meshes.terrain.indexCount = (PATCH_SIZE - 1) * (PATCH_SIZE - 1) * 4;
    const auto terrainIndexCount = (PATCH_SIZE - 1) * (PATCH_SIZE - 1) * 4;

    const uint32_t vertexBufferSize = (PATCH_SIZE * PATCH_SIZE * 4) * sizeof(Vertex);
    const uint32_t indexBufferSize = (w * w * 4) * sizeof(uint32_t);

    struct Buffers
    {
      RapidVulkan::Buffer Buffer;
      RapidVulkan::Memory Memory;
    };

    Buffers vertexStaging;
    Buffers indexStaging;
    Buffers terrainVertices;
    Buffers terrainIndices;

    // Create staging buffers
    // Vertex data
    CreateBuffer(vertexStaging.Buffer, vertexStaging.Memory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBufferSize,
                 vertices.data());

    // Index data
    CreateBuffer(indexStaging.Buffer, indexStaging.Memory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, indexBufferSize,
                 indices.data());

    CreateBuffer(terrainVertices.Buffer, terrainVertices.Memory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBufferSize, nullptr);

    CreateBuffer(terrainIndices.Buffer, terrainIndices.Memory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBufferSize, nullptr);

    // Copy from staging buffers
    auto copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    VkBufferCopy copyRegion{};

    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd.Get(), vertexStaging.Buffer.Get(), terrainVertices.Buffer.Get(), 1, &copyRegion);

    copyRegion.size = indexBufferSize;
    vkCmdCopyBuffer(copyCmd.Get(), indexStaging.Buffer.Get(), terrainIndices.Buffer.Get(), 1, &copyRegion);

    FlushCommandBuffer(copyCmd, m_deviceQueue.Queue, true);

    // Transfer local stack data to m_meshes.Terrain
    Willems::MeshLoader::MeshDescriptor meshDescriptor(UncheckedNumericCast<uint32_t>(vertices.size()), terrainIndexCount);
    Willems::MeshLoader::MeshBufferInfo meshVertices(std::move(terrainVertices.Buffer), std::move(terrainVertices.Memory), vertices.size());
    Willems::MeshLoader::MeshBufferInfo meshIndices(std::move(terrainIndices.Buffer), std::move(terrainIndices.Memory), indices.size());
    glm::vec3 meshDim;
    m_meshes.Terrain.Reset(meshDescriptor, std::move(meshVertices), std::move(meshIndices), terrainIndexCount, meshDim);
  }


  // Setup pool and buffer for storing pipeline statistics results
  void DynamicTerrainTessellation::SetupQueryResultBuffer()
  {
    FSLLOG3_INFO("SetupQueryResultBuffer");
    assert(m_deviceFeatures.pipelineStatisticsQuery);

    const uint32_t bufSize = 2 * sizeof(uint64_t);

    VkMemoryAllocateInfo memAlloc{};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = bufSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    // Results are saved in a host visible buffer for easy access by the application
    m_queryResult.Buffer.Reset(m_device.Get(), bufferCreateInfo);

    VkMemoryRequirements memReqs = m_queryResult.Buffer.GetBufferMemoryRequirements();
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = m_vulkanDevice.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    m_queryResult.Memory.Reset(m_device.Get(), memAlloc);

    RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), m_queryResult.Buffer.Get(), m_queryResult.Memory.Get(), 0));

    // Create query pool
    VkQueryPoolCreateInfo queryPoolInfo{};
    queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    queryPoolInfo.queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;
    queryPoolInfo.pipelineStatistics =
      VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT | VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT;
    queryPoolInfo.queryCount = 2;

    m_queryPool.Reset(m_device.Get(), queryPoolInfo);
  }


  void DynamicTerrainTessellation::SetupVertexDescriptions()
  {
    FSLLOG3_INFO("SetupVertexDescriptions");

    // Binding description
    m_vertices.BindingDescriptions.resize(1);
    // Lookup of initializer 'vertexInputBindingDescription'
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = Willems::MeshLoader::VertexSize(g_vertexLayout);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.resize(3);

    // Location 0 : Position
    m_vertices.AttributeDescriptions[0].location = 0;
    m_vertices.AttributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[0].offset = 0;

    // Location 1 : Normals
    m_vertices.AttributeDescriptions[1].location = 1;
    m_vertices.AttributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[1].offset = sizeof(float) * 3;

    // Location 2 : Texture coordinates
    m_vertices.AttributeDescriptions[2].location = 2;
    m_vertices.AttributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    m_vertices.AttributeDescriptions[2].offset = sizeof(float) * 6;

    // Lookup of initializer 'pipelineVertexInputStateCreateInfo'
    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.vertexBindingDescriptionCount = UncheckedNumericCast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }


  void DynamicTerrainTessellation::PrepareUniformBuffers()
  {
    FSLLOG3_INFO("PrepareUniformBuffers");
    // Shared tessellation shader stages uniform buffer
    CreateBuffer(m_uniformData.TerrainTessellation.Buffer, m_uniformData.TerrainTessellation.Memory, m_uniformData.TerrainTessellation.Descriptor,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_uboTess),
                 nullptr);

    // Skysphere vertex shader uniform buffer
    CreateBuffer(m_uniformData.SkysphereVertex.Buffer, m_uniformData.SkysphereVertex.Memory, m_uniformData.SkysphereVertex.Descriptor,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_uboVS),
                 nullptr);

    UpdateUniformBuffers();
  }


  void DynamicTerrainTessellation::UpdateUniformBuffers()
  {
    const auto screenExtent = GetScreenExtent();

    // Tessellation
    m_uboTess.Projection = m_camera.Matrices.Perspective;
    m_uboTess.Modelview = m_camera.Matrices.View * glm::mat4(1.0f);
    m_uboTess.LightPos.y = -0.5f - m_uboTess.DisplacementFactor;    // todo: Not used yet
    m_uboTess.ViewportDim = glm::vec2(static_cast<float>(screenExtent.Width), static_cast<float>(screenExtent.Height));

    m_frustum.Update(m_uboTess.Projection * m_uboTess.Modelview);
    std::memcpy(m_uboTess.FrustumPlanes, m_frustum.Planes.data(), sizeof(glm::vec4) * 6);

    const float savedFactor = m_uboTess.TessellationFactor;
    if (!m_tessellation)
    {
      // Setting this to zero sets all tessellation factors to 1.0 in the shader
      m_uboTess.TessellationFactor = 0.0f;
    }

    {
      m_uniformData.TerrainTessellation.Map(0, sizeof(m_uboTess));
      std::memcpy(m_uniformData.TerrainTessellation.pMapped, &m_uboTess, sizeof(m_uboTess));
      m_uniformData.TerrainTessellation.Unmap();
    }

    if (!m_tessellation)
    {
      m_uboTess.TessellationFactor = savedFactor;
    }

    // Skysphere vertex shader
    m_uboVS.mvp = m_camera.Matrices.Perspective * glm::mat4(glm::mat3(m_camera.Matrices.View));

    {
      m_uniformData.SkysphereVertex.Map(0, sizeof(m_uboVS));
      std::memcpy(m_uniformData.SkysphereVertex.pMapped, &m_uboVS, sizeof(m_uboVS));
      m_uniformData.SkysphereVertex.Unmap();
    }
  }


  void DynamicTerrainTessellation::SetupDescriptorSetLayouts()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(3);

    // Terrain
    // Binding 0 : Shared Tessellation shader ubo
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    // Binding 1 : Height map
    setLayoutBindings[1].binding = 1;
    setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[1].descriptorCount = 1;
    setLayoutBindings[1].stageFlags =
      VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // Binding 3 : Terrain texture array layers
    setLayoutBindings[2].binding = 2;
    setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[2].descriptorCount = 1;
    setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_descriptorSetLayouts.Terrain.Reset(m_device.Get(), descriptorLayout);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.Terrain.GetPointer();

    m_pipelineLayouts.Terrain.Reset(m_device.Get(), pipelineLayoutCreateInfo);


    // Skysphere
    setLayoutBindings.resize(2);
    // Binding 0 : Vertex shader ubo
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // Binding 1 : Color map
    setLayoutBindings[1].binding = 1;
    setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[1].descriptorCount = 1;
    setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    descriptorLayout = VkDescriptorSetLayoutCreateInfo{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_descriptorSetLayouts.Skysphere.Reset(m_device.Get(), descriptorLayout);

    pipelineLayoutCreateInfo = VkPipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.Skysphere.GetPointer();

    m_pipelineLayouts.Skysphere.Reset(m_device.Get(), pipelineLayoutCreateInfo);
  }


  void DynamicTerrainTessellation::PreparePipelines()
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.flags = 0;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.blendEnable = VK_FALSE;
    blendAttachmentState.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &blendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.front.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.flags = 0;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.flags = 0;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicStateEnables.size());
    dynamicState.pDynamicStates = dynamicStateEnables.data();

    // We render the terrain as a grid of quad patches
    VkPipelineTessellationStateCreateInfo tessellationState{};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationState.patchControlPoints = 4;

    // Terrain tessellation pipeline
    std::array<VkPipelineShaderStageCreateInfo, 4> shaderStages{};
    shaderStages[0] = LoadShader("shaders/terrain.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/terrain.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    shaderStages[2] = LoadShader("shaders/terrain.tesc.spv", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    shaderStages[3] = LoadShader("shaders/terrain.tese.spv", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.layout = m_pipelineLayouts.Terrain.Get();
    pipelineCreateInfo.renderPass = m_renderPass.Get();
    pipelineCreateInfo.pVertexInputState = &m_vertices.InputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pTessellationState = &tessellationState;
    pipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.renderPass = m_renderPass.Get();

    m_pipelines.Terrain.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);

    // Terrain wireframe pipeline
    rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
    m_pipelines.Wireframe.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);

    // Skysphere pipeline
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    // Revert to triangle list topology
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Reset tessellation state
    pipelineCreateInfo.pTessellationState = nullptr;
    // Don't write to depth buffer
    depthStencilState.depthWriteEnable = VK_FALSE;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.layout = m_pipelineLayouts.Skysphere.Get();
    shaderStages[0] = LoadShader("shaders/skysphere.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/skysphere.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    m_pipelines.Skysphere.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
  }


  void DynamicTerrainTessellation::SetupDescriptorPool()
  {
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 3;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 3;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 2;
    descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void DynamicTerrainTessellation::SetupDescriptorSets()
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayouts.Terrain.GetPointer();

    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSets.Terrain));

    std::vector<VkWriteDescriptorSet> writeDescriptorSets(3);
    // Binding 0 : Shared tessellation shader ubo
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSets.Terrain;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformData.TerrainTessellation.Descriptor;
    // Binding 1 : Displacement map
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSets.Terrain;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = m_textures.HeightMap.GetImageDescriptorPointer();
    // Binding 2 : Color map (alpha channel)
    writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[2].pNext = nullptr;
    writeDescriptorSets[2].dstSet = m_descriptorSets.Terrain;
    writeDescriptorSets[2].dstBinding = 2;
    writeDescriptorSets[2].descriptorCount = 1;
    writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[2].pImageInfo = m_textures.TerrainArray.GetImageDescriptorPointer();

    vkUpdateDescriptorSets(m_device.Get(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

    // Skysphere allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayouts.Skysphere.GetPointer();

    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSets.Skysphere));

    writeDescriptorSets.clear();
    writeDescriptorSets.resize(2);
    // Binding 0 : Vertex shader ubo
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSets.Skysphere;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformData.SkysphereVertex.Descriptor;
    // Binding 1 : Fragment shader color map
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSets.Skysphere;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = m_textures.SkySphere.GetImageDescriptorPointer();

    vkUpdateDescriptorSets(m_device.Get(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
  }


  void DynamicTerrainTessellation::ReBuildCommandBuffers()
  {
    if (!CheckCommandBuffers())
    {
      DestroyCommandBuffers();
      CreateCommandBuffers();
    }
    BuildCommandBuffers();
  }


  void DynamicTerrainTessellation::ChangeTessellationFactor(const float delta)
  {
    m_uboTess.TessellationFactor += delta;
    m_uboTess.TessellationFactor = std::max(0.25f, std::min(m_uboTess.TessellationFactor, 4.0f));
    UpdateUniformBuffers();
    UpdateTextOverlay();
  }


  void DynamicTerrainTessellation::ToggleWireframe()
  {
    if (m_deviceFeatures.fillModeNonSolid == VK_FALSE)
    {
      FSLLOG3_INFO("The device does not support the non solid fill mode, so the wireframe toggle is disabled");
      return;
    }

    m_wireframe = !m_wireframe;
    ReBuildCommandBuffers();
    UpdateUniformBuffers();
  }

  void DynamicTerrainTessellation::ToggleTessellation()
  {
    m_tessellation = !m_tessellation;
    UpdateUniformBuffers();
  }


  void DynamicTerrainTessellation::GetQueryResults()
  {
    assert(m_deviceFeatures.pipelineStatisticsQuery);

    // We use vkGetQueryResults to copy the results into a host visible buffer
    vkGetQueryPoolResults(m_device.Get(), m_queryPool.Get(), 0, 1, sizeof(m_pipelineStats), m_pipelineStats, sizeof(uint64_t),
                          VK_QUERY_RESULT_64_BIT);
  }
}
