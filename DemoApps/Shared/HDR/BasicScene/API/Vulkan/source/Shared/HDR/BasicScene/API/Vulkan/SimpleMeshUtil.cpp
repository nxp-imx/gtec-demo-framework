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

#include <Shared/HDR/BasicScene/API/Vulkan/SimpleMeshUtil.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Array.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <Shared/HDR/BasicScene/ModelVertices.hpp>

namespace Fsl
{
  SimpleMesh SimpleMeshUtil::CreateTunnelVertexArray(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                     const std::array<VertexElementUsage, 3>& shaderBindOrder)
  {
    SimpleMesh mesh;
    auto vertices = ModelVertices::CreateTunnelVertexArray();
    mesh.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(vertices), Vulkan::VMBufferUsage::STATIC);
    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(mesh.VertexAttributeDescription, shaderBindOrder, mesh.VertexBuffer);
    mesh.VertexInputBindingDescription.binding = 0;
    mesh.VertexInputBindingDescription.stride = mesh.VertexBuffer.GetElementStride();
    mesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return mesh;
  }


  SimpleMesh SimpleMeshUtil::CreateQuadVertexArray(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                   const std::array<VertexElementUsage, 2>& shaderBindOrder)
  {
    SimpleMesh mesh;
    auto vertices = ModelVertices::CreateQuadVertexArray();
    mesh.VertexBuffer.Reset(bufferManager, ReadOnlyFlexVertexSpanUtil::AsSpan(vertices), Vulkan::VMBufferUsage::STATIC);
    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(mesh.VertexAttributeDescription, shaderBindOrder, mesh.VertexBuffer);
    mesh.VertexInputBindingDescription.binding = 0;
    mesh.VertexInputBindingDescription.stride = mesh.VertexBuffer.GetElementStride();
    mesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return mesh;
  }
}
