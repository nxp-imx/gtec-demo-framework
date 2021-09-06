#ifndef FSLSIMPLEUI_APP_UIDEMOAPPEXTENSIONCREATEINFO_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPEXTENSIONCREATEINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslSimpleUI/App/UIDemoAppRenderCreateInfo.hpp>
#include <memory>
#include <utility>


namespace Fsl
{
  class DemoPerformanceCapture;

  namespace UI
  {
    class IExternalModuleFactory;
    class IRenderSystemFactory;
  }

  struct UIDemoAppExtensionCreateInfo
  {
    ServiceProvider DemoServiceProvider;
    const DemoWindowMetrics WindowMetrics;
    const UIDemoAppRenderCreateInfo RenderCreateInfo;

    std::shared_ptr<DemoPerformanceCapture> Profiler;           // This can be null
    UI::IRenderSystemFactory* pRenderSystemFactory{nullptr};    // This can be null

    ReadOnlySpan<std::shared_ptr<UI::IExternalModuleFactory>> ExternalModuleFactories;

    UIDemoAppExtensionCreateInfo(const ServiceProvider& demoServiceProvider, const DemoWindowMetrics& windowMetrics)
      : DemoServiceProvider(demoServiceProvider)
      , WindowMetrics(windowMetrics)
    {
    }

    UIDemoAppExtensionCreateInfo(const ServiceProvider& demoServiceProvider, const DemoWindowMetrics& windowMetrics,
                                 const UIDemoAppRenderCreateInfo& renderCreateInfo)
      : DemoServiceProvider(demoServiceProvider)
      , WindowMetrics(windowMetrics)
      , RenderCreateInfo(renderCreateInfo)
    {
    }

    UIDemoAppExtensionCreateInfo(const ServiceProvider& demoServiceProvider, const DemoWindowMetrics& windowMetrics,
                                 const UIDemoAppRenderCreateInfo& renderCreateInfo, std::shared_ptr<DemoPerformanceCapture> profiler)
      : DemoServiceProvider(demoServiceProvider)
      , WindowMetrics(windowMetrics)
      , RenderCreateInfo(renderCreateInfo)
      , Profiler(std::move(profiler))
    {
    }

    UIDemoAppExtensionCreateInfo(const ServiceProvider& demoServiceProvider, const DemoWindowMetrics& windowMetrics,
                                 const UIDemoAppRenderCreateInfo& renderCreateInfo, std::shared_ptr<DemoPerformanceCapture> profiler,
                                 const ReadOnlySpan<std::shared_ptr<UI::IExternalModuleFactory>> externalModuleFactories)
      : DemoServiceProvider(demoServiceProvider)
      , WindowMetrics(windowMetrics)
      , RenderCreateInfo(renderCreateInfo)
      , Profiler(std::move(profiler))
      , ExternalModuleFactories(externalModuleFactories)
    {
    }
  };
}

#endif
