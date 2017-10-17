#ifndef FSLDEMOHOST_BASE_SERVICE_OPTIONS_OPTIONSSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_OPTIONS_OPTIONSSERVICE_HPP
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

#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <FslDemoApp/Base/Service/Options/IOptions.hpp>
#include <FslDemoHost/Base/Service/Options/IOptionsServiceControl.hpp>
#include <map>

namespace Fsl
{

  class OptionsService
    : public ThreadLocalService
    , public IOptions
    , public IOptionsServiceControl
  {
    std::map<OptionParserId, std::shared_ptr<IOptionParser> > m_optionParsers;
  public:
    OptionsService(const ServiceProvider& serviceProvider);

    // From IOptions
    virtual std::shared_ptr<IOptionParser> GetOptionParser(const OptionParserId& optionParserId) const override;
    virtual std::shared_ptr<IOptionParser> TryGetOptionParser(const OptionParserId& optionParserId) const override;
    virtual std::shared_ptr<IOptionParser> GetOptionParser(const std::function<bool(const std::shared_ptr<IOptionParser>&)>& matchFunction) const override;
    virtual std::shared_ptr<IOptionParser> TryGetOptionParser(const std::function<bool(const std::shared_ptr<IOptionParser>&)>& matchFunction) const override;

    // From IOptionsServiceControl
    virtual void AddOptionParser(const std::shared_ptr<IOptionParser>& optionParser) override;

  };
}

#endif
