#ifndef FSLUTIL_VULKAN1_0_UTIL_VMVERTEXBUFFERUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_VMVERTEXBUFFERUTIL_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace VMVertexBufferUtil
    {
      inline void FillVertexInputAttributeDescription(VkVertexInputAttributeDescription* const pDst, const VertexElementUsage* const pShaderBindOrder,
                                                      const std::size_t entries, const VMVertexBuffer& vertexBuffer)
      {
        assert(pDst != nullptr);
        assert(pShaderBindOrder != nullptr);

        for (std::size_t i = 0; i < entries; ++i)
        {
          const auto& vertexDeclElement = vertexBuffer.GetVertexElement(pShaderBindOrder[i], 0);

          pDst[i].location = static_cast<uint32_t>(i);
          pDst[i].binding = 0;
          pDst[i].format = vertexDeclElement.NativeFormat;
          pDst[i].offset = vertexDeclElement.Offset;
        }
      }

      inline void FillVertexInputAttributeDescription(std::vector<VkVertexInputAttributeDescription>& rDstDescription,
                                                      const std::vector<VertexElementUsage>& shaderBindOrder, const VMVertexBuffer& vertexBuffer)
      {
        rDstDescription.resize(shaderBindOrder.size());
        FillVertexInputAttributeDescription(rDstDescription.data(), shaderBindOrder.data(), shaderBindOrder.size(), vertexBuffer);
      }

      template <std::size_t TSize>
      inline void FillVertexInputAttributeDescription(std::vector<VkVertexInputAttributeDescription>& rDstDescription,
                                                      const std::array<VertexElementUsage, TSize>& shaderBindOrder,
                                                      const VMVertexBuffer& vertexBuffer)
      {
        rDstDescription.resize(shaderBindOrder.size());
        FillVertexInputAttributeDescription(rDstDescription.data(), shaderBindOrder.data(), shaderBindOrder.size(), vertexBuffer);
      }

      template <std::size_t TSize>
      inline void FillVertexInputAttributeDescription(std::array<VkVertexInputAttributeDescription, TSize>& rDstDescription,
                                                      const std::array<VertexElementUsage, TSize>& shaderBindOrder,
                                                      const VMVertexBuffer& vertexBuffer)
      {
        FillVertexInputAttributeDescription(rDstDescription.data(), shaderBindOrder.data(), shaderBindOrder.size(), vertexBuffer);
      }

      template <std::size_t TSize>
      inline void FillVertexInputAttributeDescription(std::array<VkVertexInputAttributeDescription, TSize>& rDstDescription,
                                                      const std::vector<VertexElementUsage>& shaderBindOrder, const VMVertexBuffer& vertexBuffer)
      {
        if (shaderBindOrder.size() != rDstDescription.size())
        {
          throw std::invalid_argument("rDstDescription.size() != shaderBindOrder.size()");
        }
        FillVertexInputAttributeDescription(rDstDescription.data(), shaderBindOrder.data(), shaderBindOrder.size(), vertexBuffer);
      }
    }
  }
}

#endif
