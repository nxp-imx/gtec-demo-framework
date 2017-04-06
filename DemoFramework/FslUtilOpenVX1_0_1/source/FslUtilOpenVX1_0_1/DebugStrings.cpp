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

// Based on RapidOpenVX https://github.com/Unarmed1000/RapidOpenVX with permission.

#include <FslUtilOpenVX1_0_1/DebugStrings.hpp>

namespace Fsl
{
  namespace OpenVX
  {
    namespace Debug
    {
      const char* ErrorCodeToString(const vx_status errorCode)
      {
        switch (errorCode)
        {
        case VX_ERROR_REFERENCE_NONZERO:
          return "VX_ERROR_REFERENCE_NONZERO";
        case VX_ERROR_MULTIPLE_WRITERS:
          return "VX_ERROR_MULTIPLE_WRITERS";
        case VX_ERROR_GRAPH_ABANDONED:
          return "VX_ERROR_GRAPH_ABANDONED";
        case VX_ERROR_GRAPH_SCHEDULED:
          return "VX_ERROR_GRAPH_SCHEDULED";
        case VX_ERROR_INVALID_SCOPE:
          return "VX_ERROR_INVALID_SCOPE";
        case VX_ERROR_INVALID_NODE:
          return "VX_ERROR_INVALID_NODE";
        case VX_ERROR_INVALID_GRAPH:
          return "VX_ERROR_INVALID_GRAPH";
        case VX_ERROR_INVALID_VALUE:
          return "VX_ERROR_INVALID_VALUE";
        case VX_ERROR_INVALID_DIMENSION:
          return "VX_ERROR_INVALID_DIMENSION";
        case VX_ERROR_INVALID_FORMAT:
          return "VX_ERROR_INVALID_FORMAT";
        case VX_ERROR_INVALID_LINK:
          return "VX_ERROR_INVALID_LINK";
        case VX_ERROR_INVALID_REFERENCE:
          return "VX_ERROR_INVALID_REFERENCE";
        case VX_ERROR_INVALID_MODULE:
          return "VX_ERROR_INVALID_MODULE";
        case VX_ERROR_INVALID_PARAMETERS:
          return "VX_ERROR_INVALID_PARAMETERS";
        case VX_ERROR_OPTIMIZED_AWAY:
          return "VX_ERROR_OPTIMIZED_AWAY";
        case VX_ERROR_NO_MEMORY:
          return "VX_ERROR_NO_MEMORY";
        case VX_ERROR_NO_RESOURCES:
          return "VX_ERROR_NO_RESOURCES";
        case VX_ERROR_NOT_COMPATIBLE:
          return "VX_ERROR_NOT_COMPATIBLE";
        case VX_ERROR_NOT_ALLOCATED:
          return "VX_ERROR_NOT_ALLOCATED";
        case VX_ERROR_NOT_SUFFICIENT:
          return "VX_ERROR_NOT_SUFFICIENT";
        case VX_ERROR_NOT_SUPPORTED:
          return "VX_ERROR_NOT_SUPPORTED";
        case VX_ERROR_NOT_IMPLEMENTED:
          return "VX_ERROR_NOT_IMPLEMENTED";
        case VX_FAILURE:
          return "VX_FAILURE";
        case VX_SUCCESS:
          return "VX_SUCCESS";
        default:
          return "Unknown";
        }
      }
    }
  }
}
