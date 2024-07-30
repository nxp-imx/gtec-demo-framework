/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslSimpleUI/Render/Builder/UIVertex.hpp>
#include <FslSimpleUI/Render/Stub/MeshManager.hpp>
#include <FslSimpleUI/Render/Stub/RenderSystem.hpp>

namespace Fsl::UI::RenderStub
{
  RenderSystem::RenderSystem()
    : m_meshManager(std::make_shared<MeshManager>())
  {
  }


  RenderSystem::~RenderSystem()
  {
    m_meshManager->Dispose();
  }


  std::shared_ptr<IMeshManager> RenderSystem::GetMeshManager() const
  {
    return m_meshManager;
  }

  DrawCommandBuffer& RenderSystem::AcquireDrawCommandBuffer(const bool clear)
  {
    FSL_PARAM_NOT_USED(clear);
    return m_commandBuffer;
  }

  void RenderSystem::ReleaseDrawCommandBuffer()
  {
  }

  void RenderSystem::Draw(RenderPerformanceCapture* const /*pPerformanceCapture*/)
  {
    m_commandBuffer.Clear();
  }

  VertexDeclarationSpan RenderSystem::GetVertexDeclarationSpan()
  {
    return UIVertex::AsVertexDeclarationSpan();
  }

}
