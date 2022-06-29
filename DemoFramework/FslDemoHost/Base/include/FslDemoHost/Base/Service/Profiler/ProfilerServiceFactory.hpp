#ifndef FSLDEMOHOST_BASE_SERVICE_PROFILER_PROFILERSERVICEFACTORY_HPP
#define FSLDEMOHOST_BASE_SERVICE_PROFILER_PROFILERSERVICEFACTORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslDemoHost/Base/Service/Profiler/ProfilerService.hpp>
#include <FslDemoHost/Base/Service/Profiler/ProfilerServiceOptionParser.hpp>
#include <FslService/Impl/ServiceSupportedInterfaceDeque.hpp>
#include <FslService/Impl/ServiceType/Local/IThreadLocalSingletonServiceFactory.hpp>

namespace Fsl
{
  class ProfilerServiceFactory final : public IThreadLocalSingletonServiceFactory
  {
    ServiceCaps::Flags m_flags{ServiceCaps::Default};
    std::shared_ptr<ProfilerServiceOptionParser> m_optionParser;

  public:
    ProfilerServiceFactory()
      : m_optionParser(std::make_shared<ProfilerServiceOptionParser>())
    {
    }


    std::shared_ptr<AServiceOptionParser> GetOptionParser() const final
    {
      return m_optionParser;
    }


    ServiceCaps::Flags GetFlags() const final
    {
      return m_flags;
    }


    void FillInterfaceType(ServiceSupportedInterfaceDeque& rServiceInterfaceTypeDeque) const final
    {
      rServiceInterfaceTypeDeque.push_back(std::type_index(typeid(IProfilerService)));
      rServiceInterfaceTypeDeque.push_back(std::type_index(typeid(IProfilerServiceControl)));
    }


    std::shared_ptr<IService> Allocate(ServiceProvider& provider) final
    {
      return std::make_shared<ProfilerService>(provider, m_optionParser);
    }
  };
}

#endif
