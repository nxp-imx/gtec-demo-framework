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

#include "TextureCompression.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslSimpleUI/Base/Control/Extended/Texture2DImage.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/DebugStrings.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Debug/Strings/VkFormatFeatureFlagBits.hpp>
#include <vulkan/vulkan.h>


namespace Fsl
{
  using namespace UI;
  using namespace Vulkan;

  namespace
  {
    const auto TEXTURE_PATH = "Textures/GPUSdkTest";

    struct CreateContext
    {
      VUPhysicalDeviceRecord PhysicalDevice;
      std::shared_ptr<IContentManager> ContentManager;
      std::shared_ptr<IGraphicsService> GraphicsService;
      std::shared_ptr<UI::WindowContext> WindowContext;
    };


    std::shared_ptr<BaseWindow> CreateTextureControl(const CreateContext& context, const Texture& texture, const std::string& caption)
    {
      Texture2D sourceTexture(context.GraphicsService->GetNativeGraphics(), texture, Texture2DFilterHint::Smooth);

      auto label = std::make_shared<Label>(context.WindowContext);
      label->SetAlignmentX(ItemAlignment::Center);
      label->SetAlignmentY(ItemAlignment::Far);
      label->SetContent(caption);

      auto tex = std::make_shared<Texture2DImage>(context.WindowContext);
      tex->SetScalePolicy(UI::ItemScalePolicy::FitKeepAR);
      tex->SetContent(sourceTexture);
      tex->SetAlignmentX(ItemAlignment::Center);
      tex->SetAlignmentY(ItemAlignment::Center);
      tex->SetBlendState(BlendState::AlphaBlend);
      // tex1->SetWidth(256);
      // tex1->SetHeight(256);

      auto stack = std::make_shared<StackLayout>(context.WindowContext);
      stack->SetLayoutOrientation(LayoutOrientation::Vertical);
      stack->AddChild(label);
      stack->AddChild(tex);
      return stack;
    }

    std::string GetBitflagsString(const VkFormatFeatureFlags flags)
    {
      std::stringstream stream;
      std::size_t foundCount = 0;

      if (flags > 0)
      {
        stream << "0x" << std::hex << flags << " (";
        uint32_t bitFlags = flags;
        for (uint32_t i = 0; i <= 32; ++i)
        {
          if ((bitFlags & 0x80000000u) != 0)
          {
            auto flag = static_cast<VkFormatFeatureFlagBits>(0x80000000u >> i);
            if (foundCount > 0)
            {
              stream << "|";
            }
            auto pszFlag = RapidVulkan::Debug::TryToString(flag);
            if (pszFlag != nullptr)
            {
              stream << pszFlag;
            }
            else
            {
              stream << "0x" << std::hex << flag;
            }

            ++foundCount;
          }
          bitFlags <<= 1;
        }
        stream << ")";
      }
      else
      {
        stream << "0";
      }
      return stream.str();
    }

    std::shared_ptr<BaseWindow> CreateTextureControl(const CreateContext& context, const Texture& texture, const Texture& notSupportedTexture)
    {
      const auto pixelFormat = texture.GetPixelFormat();
      const auto vulkanPixelFormat = ConvertUtil::Convert(pixelFormat);
      auto properties = context.PhysicalDevice.GetPhysicalDeviceFormatProperties(vulkanPixelFormat);
      bool isSupported = properties.optimalTilingFeatures != 0 && properties.linearTilingFeatures != 0;

      FSLLOG("- " << Vulkan::Debug::ToString(vulkanPixelFormat) << ": " << isSupported);
      if (Fsl::Logger::GetLogLevel() >= LogType::Verbose)
      {
        FSLLOG("  - properties.linearTilingFeatures: " << GetBitflagsString(properties.linearTilingFeatures));
        FSLLOG("  - properties.optimalTilingFeatures: " << GetBitflagsString(properties.optimalTilingFeatures));
        FSLLOG("  - properties.bufferFeatures: " << GetBitflagsString(properties.bufferFeatures));
      }

      std::string caption(Fsl::Debug::ToString(pixelFormat));
      if (StringUtil::StartsWith(caption, "PixelFormat::"))
      {
        StringUtil::Replace(caption, "PixelFormat::", "");
      }

      if (isSupported)
      {
        return CreateTextureControl(context, texture, caption);
      }

      return CreateTextureControl(context, notSupportedTexture, caption);
    }


    void CreateTextureControlsIfSupported(std::deque<std::shared_ptr<BaseWindow>>& rTextures, const CreateContext& context, const IO::Path& path,
                                          const PixelFormat switchPF, const Texture& notSupportedTexture)
    {
      IO::Path newPath = IO::Path::Combine(TEXTURE_PATH, path);

      //  If no format feature flags are supported, the format itself is not supported, and images of that format cannot be created.
      auto texture = context.ContentManager->ReadTexture(newPath, switchPF, BitmapOrigin::UpperLeft);
      try
      {
        rTextures.push_back(CreateTextureControl(context, texture, notSupportedTexture));

        auto newPixelFormat = PixelFormatUtil::TryTransform(switchPF, PixelFormatFlags::NF_Srgb);
        if (newPixelFormat != PixelFormat::Undefined)
        {
          texture.SetCompatiblePixelFormat(newPixelFormat);
          // auto properties = context.PhysicalDevice.GetPhysicalDeviceFormatProperties(ConvertUtil::Convert(newPixelFormat));
          rTextures.push_back(CreateTextureControl(context, texture, notSupportedTexture));
        }
      }
      catch (const std::exception&)
      {
        FSLLOG("Failed to create texture from source: '" << path << "' with pixel format: " << texture.GetPixelFormat());
        throw;
      }
    }
  }


  TextureCompression::TextureCompression(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))    // Prepare the extension
  {
    // ASTC (Adaptive scalable texture compression)
    const bool hasASTC_LDR = m_deviceActiveFeatures.textureCompressionASTC_LDR != VK_FALSE;
    const bool hasETC2 = m_deviceActiveFeatures.textureCompressionETC2 != VK_FALSE;
    const bool hasBC = m_deviceActiveFeatures.textureCompressionBC != VK_FALSE;

    // FSLLOG("FIX: implement extension check");
    // const bool hasASTC_DecodeMode = false;

    // VK_KHR_image_format_list

    FSLLOG("Compression extensions");
    FSLLOG("ASTC");
    FSLLOG("- textureCompressionASTC_LDR: " << hasASTC_LDR);
    // FSLLOG("- " << VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME << ": " << hasASTC_DecodeMode);


    FSLLOG("ASTC (Adaptive scalable texture compression)");
    FSLLOG("- LDR: " << hasASTC_LDR);
    // FSLLOG("- HDR: " << hasASTC_HDR);
    // FSLLOG("- DecodeMode: " << hasASTC_DecodeMode);
    FSLLOG("ETC2 (Ericsson Texture Compression): " << hasETC2);
    FSLLOG("S3TC (S3 texture compression) DXTn, DXTC or BCn texture compression: " << hasBC);

    CreateUI(config.DemoServiceProvider);
  }


  void TextureCompression::Update(const DemoTime& demoTime)
  {
  }


  void TextureCompression::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;

    auto hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.4f;
      clearColorValue.float32[1] = 0.4f;
      clearColorValue.float32[2] = 0.4f;
      clearColorValue.float32[3] = 1.0f;

      VkClearValue clearValues[1] = {clearColorValue};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = clearValues;

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        // Calling this last allows the UI to draw on top of everything.
        m_uiExtension->Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }


  VkRenderPass TextureCompression::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void TextureCompression::OnFreeResources()
  {
    m_dependentResources = {};
  }


  void TextureCompression::CreateUI(const ServiceProvider& serviceProvider)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    CreateContext createContext;
    createContext.PhysicalDevice = m_physicalDevice;
    createContext.ContentManager = GetContentManager();
    createContext.GraphicsService = serviceProvider.Get<IGraphicsService>();
    // Next up we prepare the actual UI
    createContext.WindowContext = m_uiExtension->GetContext();

    auto texDefault =
      createContext.ContentManager->ReadTexture("Textures/NotSupported/NotSupported_pre.png", PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);


    std::deque<std::shared_ptr<BaseWindow>> textures;

    FSLLOG("Creating textures:");

    // Uncompressed
    CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_R8G8B8A8.ktx", PixelFormat::R8G8B8A8_UNORM, texDefault);


    // if (m_deviceActiveFeatures.textureCompressionASTC_LDR)
    {
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_4x4.ktx", PixelFormat::ASTC_4x4_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_5x4.ktx", PixelFormat::ASTC_5x4_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_5x5.ktx", PixelFormat::ASTC_5x5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_6x5.ktx", PixelFormat::ASTC_6x5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_6x6.ktx", PixelFormat::ASTC_6x6_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_8x5.ktx", PixelFormat::ASTC_8x5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_8x6.ktx", PixelFormat::ASTC_8x6_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_8x8.ktx", PixelFormat::ASTC_8x8_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x5.ktx", PixelFormat::ASTC_10x5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x6.ktx", PixelFormat::ASTC_10x6_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x8.ktx", PixelFormat::ASTC_10x8_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x10.ktx", PixelFormat::ASTC_10x10_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_12x10.ktx", PixelFormat::ASTC_12x10_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_12x12.ktx", PixelFormat::ASTC_12x12_UNORM_BLOCK, texDefault);
    }
    // if (m_deviceActiveFeatures.textureCompressionBC)
    {
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC1.ktx", PixelFormat::BC1_RGB_UNORM_BLOCK, texDefault);
      // CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC1_RGBA.ktx", PixelFormat::BC1_RGBA_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC2.ktx", PixelFormat::BC2_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC3.ktx", PixelFormat::BC3_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC4.dds", PixelFormat::BC4_UNORM_BLOCK, texDefault);
      // NOTE: 'GLI' loader is failing with a assertion (so its disabled for now)
      // CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC5.dds", PixelFormat::BC5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC6H.dds", PixelFormat::BC6H_UFLOAT_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC6H_SF.dds", PixelFormat::BC6H_SFLOAT_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_BC7.ktx", PixelFormat::BC7_UNORM_BLOCK, texDefault);
    }
    // if (m_deviceActiveFeatures.textureCompressionETC2)
    {
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ETC2_RGB.ktx", PixelFormat::ETC2_R8G8B8_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ETC2_RGB_A1.ktx", PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ETC2_RGBA.ktx", PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK, texDefault);
    }

    auto wrapLayout = std::make_shared<WrapLayout>(createContext.WindowContext);
    wrapLayout->SetLayoutOrientation(LayoutOrientation::Horizontal);
    wrapLayout->SetSpacing(Vector2(4, 4));
    wrapLayout->SetAlignmentX(ItemAlignment::Center);
    wrapLayout->SetAlignmentY(ItemAlignment::Center);

    for (const auto& tex : textures)
    {
      wrapLayout->AddChild(tex);
    }

    m_scrollable = std::make_shared<VerticalScroller>(createContext.WindowContext);
    m_scrollable->SetContent(wrapLayout);
    m_scrollable->SetScrollPadding(ThicknessF(0, 32, 0, 32));
    // scrollLayout->SetAlignmentX(ItemAlignment::Stretch);
    // scrollLayout->SetAlignmentY(ItemAlignment::Stretch);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<FillLayout>(createContext.WindowContext);
    fillLayout->AddChild(m_scrollable);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }
}
