/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include "HDR01_BasicToneMapping.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslSimpleUI/Base/Control/Background9Slice.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/HDR/BasicScene/API/Vulkan/CommonMethods.hpp>
#include <Shared/HDR/BasicScene/API/Vulkan/SimpleMeshUtil.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <cmath>

namespace Fsl
{
  using namespace Vulkan;
  using namespace UI;

  namespace
  {
    const auto VERTEX_BUFFER_BIND_ID = 0;

    const Vector3 DEFAULT_CAMERA_POSITION(0.0f, 0.0f, 0.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(0.0f, 0.0f, -4.0f);


    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      return setup;
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 3> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

      // Binding 1 : Fragment shader uniform buffer
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 2 : Fragment shader image sampler
      setLayoutBindings[2].binding = 2;
      setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[2].descriptorCount = 1;
      setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return RapidVulkan::DescriptorSetLayout(device.Get(), descriptorLayout);
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count * 2;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return RapidVulkan::DescriptorPool(device.Get(), descriptorPoolInfo);
    }


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                        const Vulkan::VUBufferMemory& fragUboBuffer, const Vulkan::VUTexture& texture)

    {
      assert(descriptorSet != nullptr);
      assert(vertUboBuffer.IsValid());
      assert(fragUboBuffer.IsValid());
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      // Binding 1 : Vertex shader uniform buffer
      auto fragUboBufferInfo = fragUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[1].pBufferInfo = &fragUboBufferInfo;

      // Binding 2 : Fragment shader texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[2].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

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


  HDR01_BasicToneMapping::HDR01_BasicToneMapping(const DemoAppConfig& config)
    : DemoAppVulkanBasic(config, CreateSetup())
    , m_bufferManager(std::make_shared<VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex))
    , m_menuUI(config)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());

    const auto contentManager = GetContentManager();

    m_fragmentUboData = PrepareLights();

    auto textureFilename = CommonMethods::GetTextureFile(m_deviceActiveFeatures);

    m_resources.TexSRGB = CommonMethods::CreateTexture(m_device, m_deviceQueue, contentManager, textureFilename);

    m_resources.VertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shader.vert.spv"));
    m_resources.LDRFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("ShaderLDR.frag.spv"));
    m_resources.HDRFragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("ShaderHDR.frag.spv"));

    std::array<VertexElementUsage, 3> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Normal,
                                                         VertexElementUsage::TextureCoordinate};
    m_resources.MeshTunnel = SimpleMeshUtil::CreateTunnelVertexArray(m_bufferManager, shaderBindOrder);
    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);
    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight);
    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.VertUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(VertexUBOData));
      rFrame.FragUboBuffer = CommonMethods::CreateUBO(m_device, sizeof(FragmentUBOData));
      rFrame.DescriptorSet = CommonMethods::CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSet, rFrame.VertUboBuffer, rFrame.FragUboBuffer, m_resources.TexSRGB);
    }
    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);
  }


  void HDR01_BasicToneMapping::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);
  }


  void HDR01_BasicToneMapping::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
    {
      const bool mouseCapture = event.IsPressed();
      if (m_demoAppControl->TryEnableMouseCaptureMode(mouseCapture))
      {
        m_mouseCaptureEnabled = mouseCapture;
      }
      else
      {
        m_mouseCaptureEnabled = false;
      }
      event.Handled();
      break;
    }
    case VirtualMouseButton::Middle:
      if (event.IsPressed())
      {
        m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void HDR01_BasicToneMapping::Update(const DemoTime& demoTime)
  {
    UpdateInput(demoTime);
    m_menuUI.Update(demoTime);

    const auto screenResolution = GetScreenResolution();
    m_vertexUboData.MatModel = Matrix::GetIdentity();
    m_vertexUboData.MatView = m_camera.GetViewMatrix();
    float aspect = static_cast<float>(screenResolution.X) / screenResolution.Y;    // ok since we divide both by two when we show four screens

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = MatrixUtil::GetClipMatrix();

    m_vertexUboData.MatProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 100.0f) * vulkanClipMatrix;
  }


  void HDR01_BasicToneMapping::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers,
                                          const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t frameIndex = drawContext.CurrentFrameIndex;
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;

    // Upload the changes
    m_fragmentUboData.Exposure = m_menuUI.GetExposure();
    m_resources.MainFrameResources[frameIndex].VertUboBuffer.Upload(0, &m_vertexUboData, sizeof(VertexUBOData));
    m_resources.MainFrameResources[frameIndex].FragUboBuffer.Upload(0, &m_fragmentUboData, sizeof(FragmentUBOData));

    auto hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        DrawToCommandBuffer(m_resources.MainFrameResources[frameIndex], hCmdBuffer);

        // Calling this last allows the UI to draw on top of everything.
        m_menuUI.Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }


  VkRenderPass HDR01_BasicToneMapping::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    m_dependentResources.PipelineHDR =
      CommonMethods::CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.VertShaderModule.Get(),
                                    m_resources.HDRFragShaderModule.Get(), m_resources.MeshTunnel, m_dependentResources.MainRenderPass.Get(), 0);
    m_dependentResources.PipelineLDR =
      CommonMethods::CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.VertShaderModule.Get(),
                                    m_resources.LDRFragShaderModule.Get(), m_resources.MeshTunnel, m_dependentResources.MainRenderPass.Get(), 0);
    return m_dependentResources.MainRenderPass.Get();
  }


  void HDR01_BasicToneMapping::OnFreeResources()
  {
    m_dependentResources = {};
  }

  void HDR01_BasicToneMapping::DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    auto res = GetScreenExtent();

    VkRect2D scissor{{0, 0}, ConvertUtil::Convert(res)};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    const auto splitX = static_cast<uint32_t>(std::round(m_menuUI.SplitX.GetValue() * res.Width));
    const uint32_t remainderX = std::min(std::max(res.Width - splitX, 0u), res.Width);

    const bool inTransition = !m_menuUI.SplitX.IsCompleted();
    const bool useClip = m_menuUI.GetState() == SceneState::Split2 || inTransition;
    const bool showingScene1 = useClip || m_menuUI.GetState() == SceneState::Scene1;
    const bool showingScene2 = useClip || m_menuUI.GetState() == SceneState::Scene2;

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &frame.DescriptorSet, 0,
                            nullptr);
    if (showingScene1)
    {
      if (useClip)
      {
        VkRect2D scissor{{0, 0}, {splitX, res.Height}};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
      }
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineLDR.Get());
      DrawScene(frame, commandBuffer);
    }
    if (showingScene2)
    {
      if (useClip)
      {
        VkRect2D scissor{{static_cast<int32_t>(splitX), 0}, {remainderX, res.Height}};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
      }
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineHDR.Get());
      DrawScene(frame, commandBuffer);
    }
  }


  void HDR01_BasicToneMapping::UpdateInput(const DemoTime& demoTime)
  {
    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
    m_menuUI.UpdateExposureInput(demoTime, keyboardState);
  }


  void HDR01_BasicToneMapping::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      if (!m_mouseCaptureEnabled)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X, -mouseState.RawPosition.Y);
          m_camera.Rotate(rawPosition);
        }
      }
    }

    // Keyboard camera movement
    const float movementSpeed = 2.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::W))
    {
      m_camera.MoveForward(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::S))
    {
      m_camera.MoveBackwards(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::A))
    {
      m_camera.MoveLeft(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::D))
    {
      m_camera.MoveRight(movementSpeed);
    }
  }


  void HDR01_BasicToneMapping::DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.MeshTunnel.VertexBuffer.GetBufferPointer(), offsets);
    vkCmdDraw(commandBuffer, m_resources.MeshTunnel.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  HDR01_BasicToneMapping::FragmentUBOData HDR01_BasicToneMapping::PrepareLights()
  {
    FragmentUBOData fragmentUBO;
    // lighting info
    fragmentUBO.LightPositions[0] = Vector4(0.0f, 0.0f, -31.5f, 1.0f);
    fragmentUBO.LightPositions[1] = Vector4(-1.4f, -1.9f, -9.0f, 1.0f);
    fragmentUBO.LightPositions[2] = Vector4(0.0f, -1.8f, -4.0f, 1.0f);
    // fragmentUBO.LightPositions[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    fragmentUBO.LightPositions[3] = Vector4(0.8f, -1.7f, -6.0f, 1.0f);

    fragmentUBO.LightColors[0] = Vector4(200.0f, 200.0f, 200.0f, 0.0f);
    fragmentUBO.LightColors[1] = Vector4(0.1f, 0.0f, 0.0f, 0.0f);
    fragmentUBO.LightColors[2] = Vector4(0.0f, 0.0f, 0.2f, 0.0f);
    fragmentUBO.LightColors[3] = Vector4(0.0f, 0.1f, 0.0f, 0.0f);
    // fragmentUBO.LightColors[3] = Vector4(221.0f, 1.1f, 1.0f)

    fragmentUBO.Exposure = 1.0f;

    return fragmentUBO;
  }
}
