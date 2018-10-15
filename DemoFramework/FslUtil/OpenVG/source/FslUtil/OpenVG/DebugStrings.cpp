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

#include <FslUtil/OpenVG/DebugStrings.hpp>

namespace Fsl
{
  namespace OpenVG
  {
    namespace Debug
    {
      const char* ErrorCodeToString(const VGErrorCode errorCode)
      {
        switch (errorCode)
        {
        case VG_NO_ERROR:
          return "VG_NO_ERROR";
        case VG_BAD_HANDLE_ERROR:
          return "VG_BAD_HANDLE_ERROR";
        case VG_ILLEGAL_ARGUMENT_ERROR:
          return "VG_ILLEGAL_ARGUMENT_ERROR";
        case VG_OUT_OF_MEMORY_ERROR:
          return "VG_OUT_OF_MEMORY_ERROR";
        case VG_PATH_CAPABILITY_ERROR:
          return "VG_PATH_CAPABILITY_ERROR";
        case VG_UNSUPPORTED_IMAGE_FORMAT_ERROR:
          return "VG_UNSUPPORTED_IMAGE_FORMAT_ERROR";
        case VG_UNSUPPORTED_PATH_FORMAT_ERROR:
          return "VG_UNSUPPORTED_PATH_FORMAT_ERROR";
        case VG_IMAGE_IN_USE_ERROR:
          return "VG_IMAGE_IN_USE_ERROR";
        case VG_NO_CONTEXT_ERROR:
          return "VG_NO_CONTEXT_ERROR";
        case VG_ERROR_CODE_FORCE_SIZE:
          return "VG_ERROR_CODE_FORCE_SIZE";
        default:
          return "Unknown";
        }
      }
    };
  }
}
