#ifndef FSLSIMPLEUI_BASE_SYSTEM_MODULES_EXTERNAL_EXTERNALMODULES_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_MODULES_EXTERNAL_EXTERNALMODULES_HPP
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
#include <FslSimpleUI/Base/Module/ExternalModuleId.hpp>
#include <memory>
#include <vector>
#include <utility>
#include "../IModuleCallbackReceiver.hpp"

namespace Fsl
{
  namespace UI
  {
    class AExternalModule;
    class ExternalModuleHost;
    class IModuleHost;
    class IExternalModuleFactory;

    class ExternalModules final : public IModuleCallbackReceiver
    {
      struct ExternalModuleRecord
      {
        ExternalModuleId Id;
        std::shared_ptr<AExternalModule> Module;

        ExternalModuleRecord() = default;
        ExternalModuleRecord(const ExternalModuleId id, std::shared_ptr<AExternalModule> module)
          : Id(id)
          , Module(std::move(module))
        {
        }
      };

      std::unique_ptr<ExternalModuleHost> m_externalModuleHost;
      std::vector<ExternalModuleRecord> m_modules;

    public:
      explicit ExternalModules(const std::shared_ptr<IModuleHost>& moduleHost, const ReadOnlySpan<std::shared_ptr<IExternalModuleFactory>> factories);
      ~ExternalModules() final;

      void ModuleOnTreeNodeAdd(const std::shared_ptr<TreeNode>& node) final;
      void ModuleOnTreeNodeDispose(const std::shared_ptr<TreeNode>& node) final;

      std::shared_ptr<AExternalModule> GetExternalModule(const ExternalModuleId moduleId) const;
    };
  }
}

#endif
