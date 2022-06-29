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

#include "T3DStressTest.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxSize2D.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/System/Threading/Thread.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics3D/Procedural/MeshBuilder.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/VUTextureUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/T3DStressTest/FurTexture.hpp>
#include <Shared/T3DStressTest/OptionParser.hpp>
#include <vulkan/vulkan.h>
#include <ctime>

namespace Fsl
{
  namespace
  {
    // const auto VERTEX_BUFFER_BIND_ID = 0;

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      return setup;
    }

    void CreateInstancedMesh(Procedural::BasicMesh& rMesh, const std::size_t desiredInstanceCount, const bool shareInstanceVertices)
    {
      using BasicMeshBuilder = Procedural::MeshBuilder<Procedural::BasicMesh::vertex_type, Procedural::BasicMesh::index_type>;

      BasicMeshBuilder meshBuilder(rMesh.GetPrimitiveType());
      const auto instances = meshBuilder.TryAppendInstances(rMesh, desiredInstanceCount, shareInstanceVertices);
      meshBuilder.ExtractTo(rMesh);

      if (instances != desiredInstanceCount)
      {
        FSLLOG3_WARNING("Created: {} instead of the requested: {}", instances, desiredInstanceCount);
      }
    }

    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Bitmap& bitmap,
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
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(bitmap, samplerCreateInfo);
    }


    //! Create the main texture
    Vulkan::VUTexture CreateMainTexture(const std::shared_ptr<IContentManager>& contentManager, const Vulkan::VUDevice& device,
                                        const Vulkan::VUDeviceQueueRecord& deviceQueue)
    {
      Bitmap bitmap;
      contentManager->Read(bitmap, "Seamless.jpg", PixelFormat::R8G8B8A8_UNORM);
      return CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }

    //! Create the fur 'density' bitmap
    Vulkan::VUTexture CreateFurDensityTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const int furTextureDim,
                                              const float hairDensity, const int layerCount)
    {
      // Create the fur 'density' bitmap
      const std::vector<uint8_t> furBitmapContent = FurTexture::Generate(furTextureDim, furTextureDim, hairDensity, layerCount);
      const RawBitmap furBitmap(furBitmapContent.data(), furTextureDim, furTextureDim, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      return CreateTexture(device, deviceQueue, furBitmap, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t uniformBuffers,
                                                     const uint32_t combinedImageSamplers, const uint32_t maxSets, const uint32_t frameCount)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = uniformBuffers * frameCount;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = combinedImageSamplers * frameCount;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = maxSets * frameCount;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }

    Procedural::BasicMesh CreateMesh(const PxSize2D& tex1Size, const int textureRepeatCount, const Point2& vertexCount, int instanceCount,
                                     const bool shareInstanceVertices, const bool useTriangleStrip)
    {
      FSLLOG3_INFO("tex1Size: {} textureRepeatCount: {} vertexCount: {} instanceCount: {} shareInstanceVertices: {} useTriangleStrip: {}", tex1Size,
                   textureRepeatCount, vertexCount, instanceCount, shareInstanceVertices, useTriangleStrip);

      TextureRectangle texRect(PxRectangle(0, 0, tex1Size.Width(), tex1Size.Height()), tex1Size);
      const NativeTextureArea texArea(Vulkan::VUTextureUtil::CalcTextureArea(texRect, textureRepeatCount, textureRepeatCount));

      Procedural::BasicMesh mesh;
      if (useTriangleStrip)
      {
        mesh = Procedural::SegmentedQuadGenerator::GenerateStrip(Vector3::Zero(), 100, 100, vertexCount.X - 1, vertexCount.Y - 1, texArea,
                                                                 WindingOrder::CCW);
      }
      else
      {
        mesh = Procedural::SegmentedQuadGenerator::GenerateList(Vector3::Zero(), 100, 100, vertexCount.X - 1, vertexCount.Y - 1, texArea,
                                                                WindingOrder::CCW);
      }

      // OpenGL ES expects that the index count is <= 0xFFFF
      if (mesh.GetIndexCount() > 0xFFFF)
      {
        throw NotSupportedException("Maximum IndexCount exceeded");
      }

      // Create instances if so desired
      if (instanceCount > 1)
      {
        CreateInstancedMesh(mesh, instanceCount, shareInstanceVertices);
      }

      FSLLOG3_INFO("Mesh indices: {} Mesh vertices: {}", mesh.GetIndexCount(), mesh.GetVertexCount());
      return mesh;
    }
  }


  T3DStressTest::T3DStressTest(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_config(config.GetOptions<OptionParser>()->GetConfig())
    , m_bufferManager(
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
    , m_xAngle(0)
    , m_yAngle(0)
    , m_zAngle(0)
    , m_gravity(0, -1.0f, 0)
    , m_radians(0.0f)
  {
    switch (m_config.GetRenderMode())
    {
    case RenderMode::MultiPass:
      FSLLOG3_INFO("RenderMode: MultiPass");
      break;
    case RenderMode::Instanced:
      FSLLOG3_INFO("RenderMode: Instanced");
      break;
    default:
      break;
    }
    FSLLOG3_INFO("ManualInstances: {} LayerCount: {} HairDensity: {} DepthTest: {}", m_config.GetInstanceCount(), m_config.GetLayerCount(),
                 m_config.GetHairDensity(), m_config.GetEnableDepthTest());

    auto contentManager = GetContentManager();

    const int furTextureDim = m_config.GetFurTextureDimensions();

    m_resources.Tex1 = CreateMainTexture(contentManager, m_device, m_deviceQueue);
    m_resources.Tex2 = CreateFurDensityTexture(m_device, m_deviceQueue, furTextureDim, m_config.GetHairDensity(), m_config.GetLayerCount());

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, 5, 9, 6, maxFramesInFlight);

    {
      Point2 vertexCount(m_config.GetVertexCountX(), m_config.GetVertexCountY());
      PxSize2D tex1Size(static_cast<int32_t>(m_resources.Tex1.GetExtent().width), static_cast<int32_t>(m_resources.Tex1.GetExtent().height));
      auto mesh = CreateMesh(tex1Size, m_config.GetTextureRepeatCount(), vertexCount, m_config.GetInstanceCount(),
                             m_config.GetShareInstanceVertices(), m_config.GetUseTriangleStrip());
      m_resources.MeshStuff = std::make_unique<MeshStuffRecord>(
        *contentManager, m_device, m_bufferManager, m_resources.MainDescriptorPool, maxFramesInFlight, mesh, m_resources.Tex1, m_resources.Tex2,
        m_config.GetUseHighShaderPrecision(), m_config.GetLightCount(), m_config.GetEnableDepthTest());
    }

    {
      Vector3 lightDirection(0.0f, 0.0f, -1.0f);
      lightDirection.Normalize();
      Vector3 lightColor(0.9f, 0.9f, 0.9f);
      Vector3 ambientColor(0.2f, 0.2f, 0.2f);

      {    // Prepare the shader
        auto& rRender = m_resources.MeshStuff->RenderVB;
        rRender.SetMaxHairLength(m_config.GetHairLength());
        for (int i = 0; i < m_config.GetLightCount(); ++i)
        {
          rRender.SetLightDirection(i, lightDirection);
          rRender.SetLightColor(i, lightColor);
        }
        rRender.SetLightAmbientColor(ambientColor);
      }
      {    // Prepare the shader
        auto& rRender = m_resources.MeshStuff->RenderInstanced;
        rRender.SetMaxHairLength(m_config.GetHairLength());
        for (int i = 0; i < m_config.GetLightCount(); ++i)
        {
          rRender.SetLightDirection(i, lightDirection);
          rRender.SetLightColor(i, lightColor);
        }
        rRender.SetLightAmbientColor(ambientColor);
        rRender.SetInstanceCount(m_config.GetLayerCount());
      }
    }
  }


  void T3DStressTest::FixedUpdate(const DemoTime& /*demoTime*/)
  {
    Vector3 forceDirection(std::sin(m_radians), 0, 0);
    m_displacement = m_gravity + forceDirection;

    m_radians += 0.01f;

    m_world = Matrix::CreateRotationX(MathHelper::TO_RADS * (static_cast<float>(m_xAngle) / 100.0f));
    m_world *= Matrix::CreateRotationY(MathHelper::TO_RADS * (static_cast<float>(m_yAngle) / 100.0f));
    m_world *= Matrix::CreateRotationZ(MathHelper::TO_RADS * (static_cast<float>(m_zAngle) / 100.0f));

    // Pull the camera back from the cube
    m_view = Matrix::CreateTranslation(0.0f, 0.0f, -m_config.GetCameraDistance());


    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    m_perspective = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), GetWindowAspectRatio(), 1, 100.0f) * vulkanClipMatrix;
    m_MVP = m_world * m_view * m_perspective;

    // m_xAngle += 10;
    // m_yAngle += 5;
    // m_zAngle += 6;

    if (m_xAngle >= 36000)
    {
      m_xAngle -= 36000;
    }
    if (m_xAngle < 0)
    {
      m_xAngle += 36000;
    }
    if (m_yAngle >= 36000)
    {
      m_yAngle -= 30600;
    }
    if (m_yAngle < 0)
    {
      m_yAngle += 36000;
    }
    if (m_zAngle >= 36000)
    {
      m_zAngle -= 36000;
    }
    if (m_zAngle < 0)
    {
      m_zAngle += 36000;
    }
  }

  void T3DStressTest::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;
    const auto frameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{1.0f, 0.0f, 0.0f, 1.0f}};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        DrawToCommandBuffer(hCmdBuffer, currentFrameIndex, frameIndex);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }

  AppDrawResult T3DStressTest::TrySwapBuffers(const FrameInfo& frameInfo)
  {
    ForceFinishEachFrame();
    return DemoAppVulkanBasic::TrySwapBuffers(frameInfo);
  }

  VkRenderPass T3DStressTest::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    FSLLOG3_VERBOSE3("OnBuildResources Begin");
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    const VkRenderPass renderPass = m_dependentResources.MainRenderPass.Get();
    m_resources.MeshStuff->OnBuildResources(context, renderPass);
    FSLLOG3_VERBOSE3("OnBuildResources End");
    return renderPass;
  }

  void T3DStressTest::OnFreeResources()
  {
    FSLLOG3_VERBOSE3("OnFreeResources Begin");
    m_resources.MeshStuff->OnFreeResources();
    m_dependentResources = {};
    FSLLOG3_VERBOSE3("OnFreeResources End");
  }

  void T3DStressTest::DrawToCommandBuffer(const VkCommandBuffer hCmdBuffer, const uint32_t /*cmdBufferIndex*/, const uint32_t frameIndex)
  {
    bool bypassRender = false;
    if (m_config.GetToggleMinMax())
    {
      std::time_t result = std::time(nullptr);
      if ((result % 10) < 5)
      {
        bypassRender = true;
        Thread::SleepMilliseconds(16);
      }
    }
    if (!bypassRender)
    {
      // Draw the fur mesh
      if (m_config.GetRenderMode() == RenderMode::MultiPass)
      {
        auto& rRender = m_resources.MeshStuff->RenderVB;
        rRender.SetWorld(m_world);
        rRender.SetView(m_view);
        rRender.SetProjection(m_perspective);
        rRender.SetDisplacement(m_displacement);

        float layerAdd = (m_config.GetLayerCount() > 1 ? 1.0f / static_cast<float>(m_config.GetLayerCount() - 1) : 1);
        float layer = 0.0f;

        rRender.Bind(hCmdBuffer, frameIndex);
        const int layerCount = m_config.GetLayerCount();
        for (int i = 0; i < layerCount; ++i)
        {
          rRender.SetCurrentLayer(hCmdBuffer, layer);

          rRender.Draw(hCmdBuffer);
          layer += layerAdd;
        }
        rRender.Unbind();
      }
      else if (m_config.GetRenderMode() == RenderMode::Instanced)
      {
        auto& rRender = m_resources.MeshStuff->RenderInstanced;
        rRender.SetWorld(m_world);
        rRender.SetView(m_view);
        rRender.SetProjection(m_perspective);
        rRender.SetDisplacement(m_displacement);

        rRender.Bind(hCmdBuffer, frameIndex);
        rRender.Draw(hCmdBuffer);
        rRender.Unbind();
      }
      else
      {
        throw NotSupportedException("RenderMode not supported");
      }

      // Draw normals
      if (m_config.GetShowNormals())
      {
        auto& rRender = m_resources.MeshStuff->RenderNormals;

        rRender.SetWorldViewProjection(m_MVP);

        rRender.Bind(hCmdBuffer, frameIndex);
        rRender.Draw(hCmdBuffer);
        rRender.Unbind();
      }
    }
  }

  void T3DStressTest::ForceFinishEachFrame()
  {
    if (m_config.GetForceFinishEachFrame())
    {
      vkDeviceWaitIdle(m_device.Get());
    }
  }
}
