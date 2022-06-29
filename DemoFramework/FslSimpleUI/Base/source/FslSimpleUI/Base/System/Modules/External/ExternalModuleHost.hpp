#ifndef FSLSIMPLEUI_BASE_SYSTEM_MODULES_EXTERNAL_EXTERNALMODULEHOST_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_MODULES_EXTERNAL_EXTERNALMODULEHOST_HPP
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

#include <FslSimpleUI/Base/Module/IExternalModuleHost.hpp>
#include <FslSimpleUI/Base/Module/IWindowClickInputTargetLocater.hpp>
#include <utility>
#include "../IModuleHost.hpp"
#include "ExternalWindowBasicInfo.hpp"
#include "ExternalWindowClickInputTargetLocater.hpp"

namespace Fsl::UI
{
  class ExternalModuleHost final : public IExternalModuleHost
  {
    std::shared_ptr<ExternalWindowClickInputTargetLocater> m_targetLocator;
    std::shared_ptr<ExternalWindowBasicInfo> m_basicInfo;

  public:
    explicit ExternalModuleHost(const std::shared_ptr<IModuleHost>& moduleHost)
      : m_targetLocator(std::make_shared<ExternalWindowClickInputTargetLocater>(moduleHost->GetTargetLocater()))
      , m_basicInfo(std::make_shared<ExternalWindowBasicInfo>(moduleHost->GetBasicInfo()))
    {
    }

    ~ExternalModuleHost() final
    {
      m_basicInfo->Dispose();
      m_targetLocator->Dispose();
    }

    // From IExternalModuleHost
    std::shared_ptr<IWindowClickInputTargetLocater> GetTargetLocater() const final
    {
      return m_targetLocator;
    }

    std::shared_ptr<IWindowBasicInfo> GetWindowInfo() const final
    {
      return m_basicInfo;
    }
  };
}

#endif
