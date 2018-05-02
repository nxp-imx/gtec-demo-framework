#ifndef FSLDEMOAPP_BASE_HOST_DEMOAPPHOSTCONFIGWINDOW_HPP
#define FSLDEMOAPP_BASE_HOST_DEMOAPPHOSTCONFIGWINDOW_HPP
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

#include <FslBase/ITag.hpp>
#include <FslDemoApp/Base/Host/DemoAppHostConfig.hpp>
#include <FslDemoApp/Shared/Host/ConfigControl.hpp>
#include <FslDemoApp/Base/Host/DemoHostCustomWindowSystem.hpp>
#include <FslDemoApp/Base/Host/DemoHostCustomWindowSystemAllocationFunction.hpp>
#include <memory>

namespace Fsl
{
  class DemoAppHostConfigWindow
    : public DemoAppHostConfig
  {
    DemoHostCustomWindowSystemAllocationFunction m_customWindowSystemAllocator;
    std::shared_ptr<DemoAppHostConfig> m_customDemoAppHostConfig;
  public:
    DemoAppHostConfigWindow();
    DemoAppHostConfigWindow(const DemoHostCustomWindowSystemAllocationFunction& customWindowSystemAllocator);
    DemoAppHostConfigWindow(const DemoHostCustomWindowSystemAllocationFunction& customWindowSystemAllocator, const std::shared_ptr<DemoAppHostConfig>& customDemoAppHostConfig);
    DemoAppHostConfigWindow(const DemoHostCustomWindowSystemAllocationFunction& customWindowSystemAllocator, const std::shared_ptr<ITag>& userTag);
    DemoAppHostConfigWindow(const DemoHostCustomWindowSystemAllocationFunction& customWindowSystemAllocator, const std::shared_ptr<DemoAppHostConfig>& customDemoAppHostConfig, const std::shared_ptr<ITag>& userTag);

    DemoHostCustomWindowSystemAllocationFunction TryGetCustomWindowSystemAllocator() const;
    std::shared_ptr<DemoAppHostConfig> TryGetCustomDemoAppHostConfigBase() const;

    template<typename T>
    std::shared_ptr<T> TryGetCustomDemoAppHostConfig() const
    {
      return std::dynamic_pointer_cast<T>(m_customDemoAppHostConfig);
    }

    template<typename T>
    std::shared_ptr<T> GetCustomDemoAppHostConfig() const
    {
      auto config = TryGetCustomDemoAppHostConfig<T>();
      if (!config)
        throw NotSupportedException("The custom demo app host config is not of the expected type");
      return config;
    }

  };
}

#endif
