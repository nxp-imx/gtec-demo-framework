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

#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsBasic2D.hpp>
#include <FslUtil/Vulkan1_0/Batch/Batch2D.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatch.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <array>
#include <memory>

namespace Fsl::Vulkan
{
  class NativeGraphicsBasic2D final : public INativeGraphicsBasic2D
  {
    struct Resources
    {
      bool IsValid = false;
      std::shared_ptr<VulkanImageCreator> ImageCreator;
      VUTexture FontTexture;
    };

    Batch2D m_batch2D;
    PxExtent2D m_currentExtent;
    PxSize2D m_fontSize;
    PxRectangle m_fillPixelRect;
    bool m_inBegin;
    std::array<PxRectangle, 128 - 33> m_charRects{};

    Resources m_resources;

  public:
    NativeGraphicsBasic2D(const std::shared_ptr<QuadBatch>& quadBatch, const PxExtent2D& currentExtentPx);
    ~NativeGraphicsBasic2D() final;

    // From INativeGraphicsBasic2D
    void SetScreenExtent(const PxExtent2D& extentPx) final;
    void Begin() final;
    void End() final;
    void DrawPoints(const Vector2* const pDst, const uint32_t length, const Color& color) final;
    void DrawString(const StringViewLite& strView, const Vector2& dstPosition) final;
    PxSize2D FontSize() const final;

    void VulkanDeviceInit(const std::shared_ptr<VulkanImageCreator>& imageCreator);
    void VulkanDeviceShutdown() noexcept;

  private:
    VUTexture CreateFontTexture(VulkanImageCreator& rImageCreator);
  };
}

#endif
