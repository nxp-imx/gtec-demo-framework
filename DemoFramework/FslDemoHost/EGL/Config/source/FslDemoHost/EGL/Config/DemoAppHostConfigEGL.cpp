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

#include <FslDemoHost/EGL/Config/DemoAppHostConfigEGL.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>

namespace Fsl
{
  DemoAppHostConfigEGL::DemoAppHostConfigEGL(const EGLint* pEglConfigAttribs, const ConfigControl configControl)
    : DemoAppHostConfigEGL(pEglConfigAttribs, nullptr, configControl)
  {
  }


  DemoAppHostConfigEGL::DemoAppHostConfigEGL(const EGLint* pEglConfigAttribs, const EGLint* pEglCreateWindowAttribs,
                                             const ConfigControl configControl)
    : m_pEglConfigAttribs(pEglConfigAttribs)
    , m_pEglCreateWindowAttribs(pEglCreateWindowAttribs)
    , m_callbackGetCreateWindowSurfaceAttribs()
    , m_configControl(configControl)
    , m_isMinimumMinorVersionSet(false)
    , m_minimumMiniorVersion(0)
  {

  }


  void DemoAppHostConfigEGL::SetCallbackGetCreateWindowSurfaceAttribs(const EGLCallbackGetCreateWindowSurfaceAttribs& callback)
  {
    FSLLOG_DEBUG_WARNING_IF(m_pEglCreateWindowAttribs != nullptr && callback, "EglCreateWindowAttribs set in constructor will be overridden by callback result");
    m_callbackGetCreateWindowSurfaceAttribs = callback;
  }


  const EGLint* DemoAppHostConfigEGL::GetEglConfigAttribs() const
  {
    return m_pEglConfigAttribs;
  }


  const EGLint* DemoAppHostConfigEGL::GetEglCreateWindowAttribs(const EGLDisplay display, const DemoAppHostCreateWindowSurfaceInfoEGL& createInfo) const
  {
    if( !m_callbackGetCreateWindowSurfaceAttribs)
      return m_pEglCreateWindowAttribs;
    return m_callbackGetCreateWindowSurfaceAttribs(display, createInfo, GetUserTag());
  }


  ConfigControl DemoAppHostConfigEGL::GetConfigControl() const
  {
    return m_configControl;
  }


  bool DemoAppHostConfigEGL::IsMinimumMiniorVersionSet() const
  {
    return m_isMinimumMinorVersionSet;
  }


  int32_t DemoAppHostConfigEGL::GetMinimumMiniorVersion() const
  {
    FSLLOG_WARNING_IF(!m_isMinimumMinorVersionSet, "Minimum minor version not set");
    return m_minimumMiniorVersion;
  }


  void DemoAppHostConfigEGL::SetMinimumMiniorVersion(const int32_t minimumMiniorVersion)
  {
    m_minimumMiniorVersion = minimumMiniorVersion;
    m_isMinimumMinorVersionSet = true;
  }


  void DemoAppHostConfigEGL::AddExtensionRequest(const ExtensionType type, const std::string& name, const ExtensionPrecense precense)
  {
    m_extensionRequests.push_back(ExtensionRequestRecord(type, name, precense));
  }


  void DemoAppHostConfigEGL::ExtractExtensionRequests(std::deque<ExtensionRequestRecord>& rTarget, const ExtensionType extensionType)
  {
    for (const auto& entry : m_extensionRequests)
    {
      if (entry.Type == extensionType)
        rTarget.push_back(entry);
    }
  }
}
