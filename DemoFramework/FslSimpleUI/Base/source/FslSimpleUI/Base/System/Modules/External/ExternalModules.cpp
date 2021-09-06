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

#include "ExternalModules.hpp"
#include <FslSimpleUI/Base/Module/AExternalModule.hpp>
#include <FslSimpleUI/Base/Module/ExternalModuleCreateInfo.hpp>
#include <FslSimpleUI/Base/Module/IExternalModuleFactory.hpp>
#include "ExternalModuleHost.hpp"
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    ExternalModules::ExternalModules(const std::shared_ptr<IModuleHost>& moduleHost,
                                     const ReadOnlySpan<std::shared_ptr<IExternalModuleFactory>> factories)
      : m_externalModuleHost(std::make_unique<ExternalModuleHost>(moduleHost))
      , m_modules(factories.size())
    {
      ExternalModuleCreateInfo createInfo(*m_externalModuleHost);
      for (std::size_t i = 0; i < factories.size(); ++i)
      {
        const auto& factory = factories[i];
        m_modules[i] = ExternalModuleRecord(factory->GetId(), factory->CreateModule(createInfo));
      }
    }

    ExternalModules::~ExternalModules() = default;


    void ExternalModules::ModuleOnTreeNodeAdd(const std::shared_ptr<TreeNode>& node)
    {
      if (node)
      {
        for (auto& rEntry : m_modules)
        {
          rEntry.Module->OnWindowAdd(node->GetWindow());
        }
      }
    }

    void ExternalModules::ModuleOnTreeNodeDispose(const std::shared_ptr<TreeNode>& node)
    {
      if (node)
      {
        for (auto& rEntry : m_modules)
        {
          rEntry.Module->OnWindowDispose(node->GetWindow());
        }
      }
    }

    std::shared_ptr<AExternalModule> ExternalModules::GetExternalModule(const ExternalModuleId moduleId) const
    {
      for (const ExternalModuleRecord& entry : m_modules)
      {
        if (entry.Id == moduleId)
        {
          return entry.Module;
        }
      }
      throw NotFoundException("External module not found");
    }

  }
}
