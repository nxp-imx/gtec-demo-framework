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

#include <FslSimpleUI/Render/IMBatch/DefaultRenderSystemFactory.hpp>
#include <FslSimpleUI/Render/IMBatch/RenderSystemFactory.hpp>
#include "FlexRenderSystem.hpp"
#include "RenderSystem.hpp"

namespace Fsl::UI::RenderIMBatch
{
  namespace
  {
    namespace NormalRenderOptionFlags
    {
      constexpr auto AvailableOptions = RenderOptionFlags::NoFlags;
      constexpr auto Settings = RenderOptionFlags::Batch | RenderOptionFlags::FillBuffers | RenderOptionFlags::DrawReorder;
    }


    namespace FlexRenderOptionFlags
    {
      constexpr auto AvailableOptions = RenderOptionFlags::Batch | RenderOptionFlags::FillBuffers | RenderOptionFlags::DepthBuffer |
                                        RenderOptionFlags::DrawReorder | RenderOptionFlags::PreferFastReorder;
      constexpr auto Settings = RenderOptionFlags::Batch | RenderOptionFlags::FillBuffers | RenderOptionFlags::DepthBuffer |
                                RenderOptionFlags::DrawReorder | RenderOptionFlags::PreferFastReorder;
    }
  }


  RenderSystemInfo RenderSystemFactory::GetInfo() const
  {
    return GetRenderInfo(m_renderSystemType);
  }


  std::unique_ptr<IRenderSystem> RenderSystemFactory::Create(const RenderSystemCreateInfo& createInfo) const
  {
    switch (m_renderSystemType)
    {
    case RenderSystemType::Default:
      {
        DefaultRenderSystemFactory factory;
        return factory.Create(createInfo);
      }
    case RenderSystemType::Normal:
      return std::make_unique<RenderSystem>(createInfo);
    case RenderSystemType::Flex:
      return std::make_unique<FlexRenderSystem>(createInfo);
    }
    throw NotSupportedException("Unsupported render system type");
  }


  VertexDeclarationSpan RenderSystemFactory::GetVertexDeclarationSpan() const
  {
    return RenderSystem::GetVertexDeclarationSpan();
  }


  RenderSystemInfo RenderSystemFactory::GetRenderInfo(const RenderSystemType renderSystemType)
  {
    switch (renderSystemType)
    {
    case RenderSystemType::Default:
      return DefaultRenderSystemFactory::GetRenderInfo();
    case RenderSystemType::Normal:
      return {NormalRenderOptionFlags::AvailableOptions, NormalRenderOptionFlags::Settings, NormalRenderOptionFlags::Settings};
    case RenderSystemType::Flex:
      return {FlexRenderOptionFlags::AvailableOptions, FlexRenderOptionFlags::Settings, FlexRenderOptionFlags::Settings};
    }
    throw NotSupportedException("Unsupported render system type");
  }
}
