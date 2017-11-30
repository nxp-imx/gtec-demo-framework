#ifndef FSLDEMOAPP_BASE_HOST_DEMOAPPHOSTCONFIGEGL_HPP
#define FSLDEMOAPP_BASE_HOST_DEMOAPPHOSTCONFIGEGL_HPP
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

#include <FslDemoApp/Base/Host/DemoAppHostConfig.hpp>
#include <FslDemoApp/Base/Host/ConfigControl.hpp>
#include <FslDemoApp/Base/Host/ExtensionRequestRecord.hpp>
#include <deque>

namespace Fsl
{
  class DemoAppHostConfigEGL
    : public DemoAppHostConfig
  {
    const int32_t* m_pEglConfigAttribs;
    ConfigControl m_configControl;
    bool m_isMinimumMinorVersionSet;
    int32_t m_minimumMiniorVersion;
    std::deque<ExtensionRequestRecord> m_extensionRequests;
  public:
    DemoAppHostConfigEGL(const int32_t* pEglConfigAttribs, const ConfigControl configControl = Fsl::ConfigControl::Default);

    const int32_t* GetEglConfigAttribs() const;
    ConfigControl GetConfigControl() const;

    bool IsMinimumMiniorVersionSet() const;
    int32_t GetMinimumMiniorVersion() const;
    void SetMinimumMiniorVersion(const int32_t minimumLesserVersion);

    //! @brief Add a extension request that the host will validate.
    //! @note  Beware it will only check for the precense of the extension not activate it in any way.
    void AddExtensionRequest(const ExtensionType type, const std::string& name, const ExtensionPrecense precense);

    std::deque<ExtensionRequestRecord> GetExtensionRequests() const
    {
      return m_extensionRequests;
    }

    void ExtractExtensionRequests(std::deque<ExtensionRequestRecord>& rTarget, const ExtensionType extensionType);
  };
}

#endif
