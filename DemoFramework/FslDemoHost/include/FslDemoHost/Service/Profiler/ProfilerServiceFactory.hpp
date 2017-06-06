#ifndef FSLDEMOHOST_SERVICE_PROFILER_PROFILERSERVICEFACTORY_HPP
#define FSLDEMOHOST_SERVICE_PROFILER_PROFILERSERVICEFACTORY_HPP
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

#include <FslDemoHost/Service/ThreadLocal/IThreadLocalSingletonServiceFactory.hpp>
#include <FslDemoHost/Service/Profiler/ProfilerService.hpp>
#include <FslDemoHost/Service/Profiler/ProfilerServiceOptionParser.hpp>

namespace Fsl
{

  class ProfilerServiceFactory : public IThreadLocalSingletonServiceFactory
  {
    ServiceCaps::Flags m_flags;
    std::shared_ptr<ProfilerServiceOptionParser> m_optionParser;
  public:
    ProfilerServiceFactory()
      : m_flags(ServiceCaps::Default)
      , m_optionParser(std::make_shared<ProfilerServiceOptionParser>())
    {

    }


    virtual std::shared_ptr<AServiceOptionParser> GetOptionParser() const override
    {
      return m_optionParser;
    }


    virtual ServiceCaps::Flags GetFlags() const override
    {
      return m_flags;
    }

    
    virtual void FillInterfaceType(ServiceSupportedInterfaceDeque& rServiceInterfaceTypeDeque) const override
    {
      rServiceInterfaceTypeDeque.push_back(TypeInfo(typeid(IProfilerService)));
      rServiceInterfaceTypeDeque.push_back(TypeInfo(typeid(IProfilerServiceControl)));
    }


    virtual std::shared_ptr<IService> Allocate(ServiceProvider& provider) override
    {
      return std::make_shared<ProfilerService>(provider, m_optionParser);
    }
  };
}

#endif
