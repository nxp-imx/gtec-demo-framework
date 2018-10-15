#ifndef FSLDEMOHOST_OPENGLES2_DEMOHOSTOPENGLES2_HPP
#define FSLDEMOHOST_OPENGLES2_DEMOHOSTOPENGLES2_HPP
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

#ifndef FSL_ENABLE_GRAPHICS_ES2
#error Graphics.ES2 support has not been enabled (define FSL_ENABLE_GRAPHICS_ES2) to enable it
#endif

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoHost/EGL/EGLDemoHost.hpp>
#include <FslUtil/OpenGLES2/GLUtil.hpp>
#include <GLES2/gl2.h>
#include <algorithm>

namespace Fsl
{
  class DemoHostGLES2 : public EGLDemoHost
  {
  public:
    DemoHostGLES2(const DemoHostConfig& demoHostConfig)
      : EGLDemoHost(demoHostConfig)
    {
    }

  protected:
    //! @brief Called when the DemoHost API initialization has been completed
    virtual void OnAPIInitialized() override
    {
      if (IsLogExtensionsEnabled())
        DoLogExtensions();

      DoCheckExtensions(GetExtensionRequests());
    }

    //! @brief Called just before the API is being shutdown
    virtual void OnAPIShutdown() override
    {
    }

  private:
    void DoLogExtensions()
    {
      auto extensions = GLES2::GLUtil::GetExtensions();
      std::sort(extensions.begin(), extensions.end());
      FSLLOG("OpenGL ES2 Extensions");
      for (const auto& entry : extensions)
      {
        FSLLOG("- " << entry);
      }
    }

    void DoCheckExtensions(const std::deque<ExtensionRequestRecord>& extensionRequests)
    {
      const auto extensions = GLES2::GLUtil::GetExtensions();
      for (const auto& request : extensionRequests)
      {
        if (request.Type == ExtensionType::OpenGLES && std::find(extensions.begin(), extensions.end(), request.Name) == extensions.end())
        {
          // Request not found, lets check how severe it is
          switch (request.Precense)
          {
          case ExtensionPrecense::Mandatory:
            throw std::runtime_error(std::string("Required extension '") + request.Name + "' not found");
          case ExtensionPrecense::Optional:
            FSLLOG_DEBUG("Optional extension '" << request.Name << "' not available.");
            break;
          default:
            throw NotSupportedException("Unsupported ExtensionPrecense");
          }
        }
      }
    }
  };
}


#endif
