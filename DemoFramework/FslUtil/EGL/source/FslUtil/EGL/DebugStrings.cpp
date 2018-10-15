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

#include <FslUtil/EGL/DebugStrings.hpp>

namespace Fsl
{
  namespace EGL
  {
    namespace Debug
    {
      extern const char* ErrorCodeToString(const EGLenum errorCode)
      {
        switch (errorCode)
        {
        case EGL_SUCCESS:
          return "EGL_SUCCESS";
        case EGL_BAD_ACCESS:
          return "EGL_BAD_ACCESS";
        case EGL_BAD_ALLOC:
          return "EGL_BAD_ALLOC";
        case EGL_BAD_ATTRIBUTE:
          return "EGL_BAD_ATTRIBUTE";
        case EGL_BAD_CONFIG:
          return "EGL_BAD_CONFIG";
        case EGL_BAD_CONTEXT:
          return "EGL_BAD_CONTEXT";
        case EGL_BAD_CURRENT_SURFACE:
          return "EGL_BAD_CURRENT_SURFACE";
        case EGL_BAD_DISPLAY:
          return "EGL_BAD_DISPLAY";
        case EGL_BAD_MATCH:
          return "EGL_BAD_MATCH";
        case EGL_BAD_NATIVE_PIXMAP:
          return "EGL_BAD_NATIVE_PIXMAP";
        case EGL_BAD_NATIVE_WINDOW:
          return "EGL_BAD_NATIVE_WINDOW";
        case EGL_BAD_PARAMETER:
          return "EGL_BAD_PARAMETER";
        case EGL_BAD_SURFACE:
          return "EGL_BAD_SURFACE";
        case EGL_NOT_INITIALIZED:
          return "EGL_NOT_INITIALIZED ";
        default:
          return "Unknown";
        }
      }
    };
  }
}
