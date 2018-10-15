#ifndef FSLDEMOHOST_EGL_CONFIG_DEMOAPPHOSTCONFIGEGL_HPP
#define FSLDEMOHOST_EGL_CONFIG_DEMOAPPHOSTCONFIGEGL_HPP
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
#include <FslDemoApp/Shared/Host/ConfigControl.hpp>
#include <FslDemoApp/Shared/Host/ExtensionRequestRecord.hpp>
#include <FslDemoHost/EGL/Config/DemoAppHostCreateWindowSurfaceInfoEGL.hpp>
#include <EGL/egl.h>
#include <deque>
#include <functional>
#include <memory>

namespace Fsl
{
  using EGLCallbackGetCreateWindowSurfaceAttribs =
    std::function<const EGLint*(const EGLDisplay, const DemoAppHostCreateWindowSurfaceInfoEGL&, const std::shared_ptr<ITag>&)>;

  class DemoAppHostConfigEGL : public DemoAppHostConfig
  {
    const EGLint* m_pEglConfigAttribs;
    const EGLint* m_pEglCreateWindowAttribs;
    EGLCallbackGetCreateWindowSurfaceAttribs m_callbackGetCreateWindowSurfaceAttribs;
    ConfigControl m_configControl;
    bool m_isMinimumMinorVersionSet;
    int32_t m_minimumMiniorVersion;
    std::deque<ExtensionRequestRecord> m_extensionRequests;

  public:
    DemoAppHostConfigEGL(const EGLint* pEglConfigAttribs, const ConfigControl configControl = ConfigControl::Default);
    DemoAppHostConfigEGL(const EGLint* pEglConfigAttribs, const EGLint* pEglCreateWindowAttribs,
                         const ConfigControl configControl = ConfigControl::Default);

    //! @brief Configure a callback that can be invoked to provide a
    //! @note  If m_pEglCreateWindowAttribs isn't a nullptr then it will be ignored and the result of callback function will be used instead.
    //!        A warning will be logged in debug builds if this occurs.
    void SetCallbackGetCreateWindowSurfaceAttribs(const EGLCallbackGetCreateWindowSurfaceAttribs& callback);

    const EGLint* GetEglConfigAttribs() const;
    const EGLint* GetEglCreateWindowAttribs(const EGLDisplay display, const DemoAppHostCreateWindowSurfaceInfoEGL& createInfo) const;
    ConfigControl GetConfigControl() const;

    bool IsMinimumMiniorVersionSet() const;
    int32_t GetMinimumMiniorVersion() const;
    void SetMinimumMiniorVersion(const int32_t minimumMiniorVersion);

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
