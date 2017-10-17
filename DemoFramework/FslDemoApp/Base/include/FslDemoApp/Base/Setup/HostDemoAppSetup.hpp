#ifndef FSLDEMOAPP_BASE_SETUP_HOSTDEMOAPPSETUP_HPP
#define FSLDEMOAPP_BASE_SETUP_HOSTDEMOAPPSETUP_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslDemoApp/Base/Setup/HostCustomization.hpp>

namespace Fsl
{
  class ExceptionMessageFormatter;
  class IDemoHostRegistry;
  class IDemoAppRegistry;
  class ServiceRegistry;


  //! @WARNING do not store this, its intended for short on stack storage during a method call only!
  class HostDemoAppSetup
  {
  public:
    HostDemoAppSetup(const HostDemoAppSetup&) = delete;
    HostDemoAppSetup(HostDemoAppSetup&&other) = delete;
    HostDemoAppSetup& operator=(const HostDemoAppSetup&) = delete;
    HostDemoAppSetup& operator=(HostDemoAppSetup&&) = delete;

    IDemoHostRegistry& TheHostRegistry;
    ServiceRegistry& TheServiceRegistry;
    IDemoAppRegistry& TheDemoAppRegistry;
    ExceptionMessageFormatter& CustomExceptionFormatter;
    HostCustomization CustomizeHost;

    HostDemoAppSetup(IDemoHostRegistry& rHostRegistry, ServiceRegistry& rServiceRegistry, IDemoAppRegistry& rDemoAppRegistry, ExceptionMessageFormatter& rCustomExceptionFormatter)
      : TheHostRegistry(rHostRegistry)
      , TheServiceRegistry(rServiceRegistry)
      , TheDemoAppRegistry(rDemoAppRegistry)
      , CustomExceptionFormatter(rCustomExceptionFormatter)
      , CustomizeHost()
    {
    }
  };
}

#endif
