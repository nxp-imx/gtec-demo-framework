#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSBASIC2D_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSBASIC2D_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslDemoService/NativeGraphics/Base/INativeGraphicsBasic2D.hpp>
#include <FslUtil/Vulkan1_0/Batch/Batch2D.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatch.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <memory>

namespace Fsl
{
  namespace Vulkan
  {
    class NativeGraphicsBasic2D : public INativeGraphicsBasic2D
    {
      struct Resources
      {
        bool IsValid = false;
        std::shared_ptr<VulkanImageCreator> ImageCreator;
        VUTexture FontTexture;
      };

      Batch2D m_batch2D;
      Point2 m_currentResolution;
      Point2 m_fontSize;
      Rectangle m_fillPixelRect;
      bool m_inBegin;
      Rectangle m_charRects[128 - 33];

      Resources m_resources;

    public:
      NativeGraphicsBasic2D(const std::shared_ptr<QuadBatch>& quadBatch, const Point2& currentResolution);
      ~NativeGraphicsBasic2D() override;

      // From INativeGraphicsBasic2D
      void SetScreenResolution(const Point2& currentResolution) override;
      void Begin() override;
      void End() override;
      void DrawPoints(const Vector2* const pDst, const uint32_t length, const Color& color) override;
      void DrawString(const char* const characters, const uint32_t length, const Vector2& dstPosition) override;
      Point2 FontSize() const override;

      void VulkanDeviceInit(const VUDevice& device, const VkQueue queue, const uint32_t queueFamilyIndex,
                            const std::shared_ptr<VulkanImageCreator>& imageCreator);
      void VulkanDeviceShutdown() noexcept;

    private:
      VUTexture CreateFontTexture(VulkanImageCreator& rImageCreator);
    };
  }
}

#endif
