/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include "ShaderClock.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const uint32_t HeatMapMin = 1000;
      constexpr const uint32_t HeatMapMax = 256000;
      constexpr const uint16_t IterationsMin = 1;
      constexpr const uint16_t IterationsMax = 6000;
    }

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count * 2;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }


    bool IsHeatmapSupported(const VkPhysicalDevice device, const VkPhysicalDeviceFeatures& deviceActiveFeatures)
    {
      FSLLOG3_INFO("Checking heatmap requirements");
      bool isSupported = true;
      if (deviceActiveFeatures.shaderInt64 == VK_FALSE)
      {
        FSLLOG3_INFO("- VkPhysicalDeviceFeatures 'shaderInt64' not supported");
        isSupported = false;
      }

      {    // Instance extension
        std::array<const char*, 1> extensions = {"VK_KHR_get_physical_device_properties2"};
        if (!Vulkan::InstanceUtil::IsInstanceExtensionsAvailable(NumericCast<uint32_t>(extensions.size()), extensions.data()))
        {
          FSLLOG3_INFO("- Device extension 'VK_KHR_get_physical_device_properties2' not supported");
          isSupported = false;
        }
      }

      {    // Device extension
        std::array<const char*, 1> extensions = {"VK_KHR_shader_clock"};
        if (!Vulkan::PhysicalDeviceUtil::IsDeviceExtensionsAvailable(device, NumericCast<uint32_t>(extensions.size()), extensions.data()))
        {
          FSLLOG3_INFO("- Device extension 'VK_KHR_shader_clock' not supported");
          isSupported = false;
        }
      }

      FSLLOG3_INFO("Heatmap requirements supported: {}", isSupported);
      return isSupported;
    }
  }


  ShaderClock::ShaderClock(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_heatmapSupport(IsHeatmapSupported(m_device.GetPhysicalDevice().Device, m_deviceActiveFeatures))
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_resources(CreateResources(m_device, m_deviceQueue, GetRenderConfig()))
    , m_scene(config, m_device, m_deviceQueue, m_resources.BufferManager, m_resources.DescriptorPool, GetRenderConfig().MaxFramesInFlight,
              m_heatmapSupport)
  {
    std::shared_ptr<OptionParser> options = config.GetOptions<OptionParser>();

    m_defaultHeatmapScale = options->GetHeatmapScale();
    m_defaultIterations = options->GetIterations();
    m_ui = CreateUI(m_heatmapSupport, m_defaultHeatmapScale, m_defaultIterations);
  }

  void ShaderClock::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsHandled() && event.IsPressed())
    {
      switch (event.GetKey())
      {
      case VirtualKey::Space:
        SetDefaultSettings();
        break;
      case VirtualKey::H:
        if (m_heatmapSupport)
        {
          m_ui.SwitchHeatmap->Toggle();
        }
        break;
      case VirtualKey::LeftArrow:
        if (m_heatmapSupport)
        {
          m_ui.SliderHeatmap->SubValue(100);
        }
        break;
      case VirtualKey::RightArrow:
        if (m_heatmapSupport)
        {
          m_ui.SliderHeatmap->AddValue(100);
        }
        break;
      case VirtualKey::UpArrow:
        m_ui.SliderIterations->SubValue(5);
        break;
      case VirtualKey::DownArrow:
        m_ui.SliderIterations->AddValue(5);
        break;
      default:
        break;
      }
    }
  }

  void ShaderClock::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);

    if (theEvent->GetSource() == m_ui.SliderHeatmap)
    {
      m_scene.SetHeatmapScale(static_cast<float>(m_ui.SliderHeatmap->GetValue()));
    }
    else if (theEvent->GetSource() == m_ui.SliderIterations)
    {
      m_scene.SetMaxIterations(m_ui.SliderIterations->GetValue());
    }
    else if (theEvent->GetSource() == m_ui.SwitchHeatmap)
    {
      m_scene.ShowHeatmap(m_ui.SwitchHeatmap->IsChecked());
    }
  }

  void ShaderClock::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_ui.BtnDefault)
    {
      SetDefaultSettings();
    }
  }

  void ShaderClock::Update(const DemoTime& demoTime)
  {
    m_scene.Update(demoTime);
  }


  void ShaderClock::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    FSL_PARAM_NOT_USED(demoTime);

    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};

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
        m_scene.Draw(drawContext.CurrentFrameIndex, hCmdBuffer);

        // Calling this last allows the UI to draw on top of everything.
        m_uiExtension->Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass ShaderClock::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    m_scene.OnBuildResources(context, m_dependentResources.MainRenderPass.Get());
    return m_dependentResources.MainRenderPass.Get();
  }


  void ShaderClock::OnFreeResources()
  {
    m_scene.OnFreeResources();
    m_dependentResources = {};
  }

  ShaderClock::Resources ShaderClock::CreateResources(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                      const RenderConfig& renderConfig)
  {
    auto bufferManager =
      std::make_shared<Vulkan::VMBufferManager>(device.GetPhysicalDevice(), device.Get(), deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

    auto descriptorPool = CreateDescriptorPool(device, renderConfig.MaxFramesInFlight);
    return {bufferManager, std::move(descriptorPool)};
  }

  ShaderClock::UIRecord ShaderClock::CreateUI(const bool heatmapSupported, const uint32_t heatmapScale, const uint16_t iterations)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& uiFactory = *uiControlFactory;

    auto labelHeatmap = uiFactory.CreateLabel("Heatmap scale: ");
    labelHeatmap->SetAlignmentY(UI::ItemAlignment::Center);
    auto sliderHeatmap = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal,
                                                        ConstrainedValue<uint32_t>(heatmapScale, LocalConfig::HeatMapMin, LocalConfig::HeatMapMax));
    sliderHeatmap->SetAlignmentY(UI::ItemAlignment::Center);

    auto switchHeatmap = uiFactory.CreateSwitch("Enabled", heatmapSupported);

    auto labelIterations = uiFactory.CreateLabel("Iterations: ");
    labelIterations->SetAlignmentY(UI::ItemAlignment::Center);
    auto sliderIterations = uiFactory.CreateSliderFmtValue(
      UI::LayoutOrientation::Horizontal, ConstrainedValue<uint32_t>(iterations, LocalConfig::IterationsMin, LocalConfig::IterationsMax));
    sliderIterations->SetAlignmentY(UI::ItemAlignment::Center);

    if (!heatmapSupported)
    {
      labelHeatmap->SetEnabled(false);
      sliderHeatmap->SetEnabled(false);
      switchHeatmap->SetEnabled(false);
    }

    auto btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Set defaults");

    auto bottomGrid = std::make_shared<UI::GridLayout>(context);
    bottomGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    bottomGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 8));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddChild(labelHeatmap, 0, 0);
    bottomGrid->AddChild(sliderHeatmap, 1, 0);
    bottomGrid->AddChild(switchHeatmap, 3, 0);
    bottomGrid->AddChild(labelIterations, 0, 1);
    bottomGrid->AddChild(sliderIterations, 1, 1);
    bottomGrid->AddChild(btnDefault, 3, 1);


    auto bottomBar = uiFactory.CreateBottomBar(bottomGrid);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(bottomBar);
    return {sliderHeatmap, switchHeatmap, sliderIterations, btnDefault};
  }


  void ShaderClock::SetDefaultSettings()
  {
    m_ui.SliderHeatmap->SetValue(m_defaultHeatmapScale);
    m_ui.SliderIterations->SetValue(m_defaultIterations);
    m_ui.SwitchHeatmap->SetEnabled(m_heatmapSupport);
  }
}
