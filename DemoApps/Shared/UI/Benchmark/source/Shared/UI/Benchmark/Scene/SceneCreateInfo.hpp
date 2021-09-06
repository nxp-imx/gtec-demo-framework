#ifndef SHARED_UI_BENCHMARK_SCENE_SCENECREATEINFO_HPP
#define SHARED_UI_BENCHMARK_SCENE_SCENECREATEINFO_HPP
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

#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/UI/Benchmark/DemoAppExtensionForwarder.hpp>
#include <Shared/UI/Benchmark/Persistence/AppSettings.hpp>
#include <Shared/UI/Benchmark/SceneAppInfo.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  class IBasicGpuProfiler;

  struct SceneCreateInfo
  {
    SceneAppInfo AppInfo;
    ServiceProvider DemoServiceProvider;
    DemoWindowMetrics WindowMetrics;
    std::shared_ptr<DemoAppExtensionForwarder> Forwarder;
    std::shared_ptr<UI::Theme::IThemeControlFactory> ControlFactory;
    std::shared_ptr<UI::Layout> RootLayout;
    std::shared_ptr<AppSettings> Settings;
    std::shared_ptr<const UIDemoAppExtension> UIExtension;
    std::shared_ptr<IBasicGpuProfiler> GpuProfiler;    // This can be null


    SceneCreateInfo(SceneAppInfo appInfo, const ServiceProvider& demoServiceProvider, const DemoWindowMetrics& windowMetrics,
                    std::shared_ptr<DemoAppExtensionForwarder> forwarder, std::shared_ptr<UI::Theme::IThemeControlFactory> controlFactory,
                    std::shared_ptr<UI::Layout> rootLayout, std::shared_ptr<AppSettings> settings,
                    std::shared_ptr<const UIDemoAppExtension> uiExtension, std::shared_ptr<IBasicGpuProfiler> gpuProfiler)
      : AppInfo(std::move(appInfo))
      , DemoServiceProvider(demoServiceProvider)
      , WindowMetrics(windowMetrics)
      , Forwarder(std::move(forwarder))
      , ControlFactory(std::move(controlFactory))
      , RootLayout(std::move(rootLayout))
      , Settings(std::move(settings))
      , UIExtension(std::move(uiExtension))
      , GpuProfiler(std::move(gpuProfiler))
    {
    }
  };
}

#endif
