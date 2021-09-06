#ifndef VULKAN_SYSTEM_DFNATIVEBATCH2D_DFNATIVEBATCH2D_HPP
#define VULKAN_SYSTEM_DFNATIVEBATCH2D_DFNATIVEBATCH2D_HPP
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

#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeBatch2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <Shared/System/DFNativeBatch2D/TextureAtlasScene.hpp>

namespace Fsl
{
  class DFNativeBatch2D : public VulkanBasic::DemoAppVulkanBasic
  {
    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
    };

    DependentResources m_dependentResources;
    std::shared_ptr<TextureAtlasScene> m_scene1;
    std::shared_ptr<IGraphicsService> m_graphics;
    // A batcher that can be used API independently or with the native API
    std::shared_ptr<Vulkan::NativeBatch2D> m_nativeBatch;
    Texture2D m_texture2D;
    Vulkan::VUTexture m_nativeTexture;

  public:
    explicit DFNativeBatch2D(const DemoAppConfig& config);

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;
  };
}

#endif
