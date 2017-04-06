#ifndef FSLUTILOPENVX1_0_1_UTIL_HPP
#define FSLUTILOPENVX1_0_1_UTIL_HPP
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

#include <FslUtilOpenVX1_0_1/Exceptions.hpp>
#include <cassert>
#include <string>
#include <VX/vx.h>

namespace Fsl
{
  namespace OpenVX
  {
    class Util
    {
    public:
      static inline void Check(const vx_status status)
      {
        if (status != VX_SUCCESS)
        {
          throw OpenVXStatusErrorException("Unknown", status);
        }
      }

      static inline void Check(const vx_status status, const char*const pszMessage)
      {
        if (status != VX_SUCCESS)
        {
          assert(pszMessage != nullptr);
          throw OpenVXStatusErrorException(ToNiceMessage(pszMessage, status), status);
        }
      }


      static inline void Check(const vx_status status, const std::string& message)
      {
        if (status != VX_SUCCESS)
        {
          throw OpenVXStatusErrorException(ToNiceMessage(message, status), status);
        }
      }

      static inline void Check(const vx_status status, const char*const pszMessage, const char*const pszFileName, const int lineNumber)
      {
        if (status != VX_SUCCESS)
        {
          assert(pszMessage != nullptr);
          assert(pszFileName != nullptr);
          throw OpenVXStatusErrorException(ToNiceMessage(pszMessage, status, pszFileName, lineNumber), status, pszFileName, lineNumber);
        }
      }

      static inline void Check(const vx_status status, const char*const pszMessage, const std::string& fileName, const int lineNumber)
      {
        if (status != VX_SUCCESS)
        {
          assert(pszMessage != nullptr);
          throw OpenVXStatusErrorException(ToNiceMessage(pszMessage, status, fileName, lineNumber), status, fileName, lineNumber);
        }
      }


      static inline void Check(const vx_status status, const std::string& message, const char*const pszFileName, const int lineNumber)
      {
        if (status != VX_SUCCESS)
        {
          assert(pszFileName != nullptr);
          throw OpenVXStatusErrorException(ToNiceMessage(message, status, pszFileName, lineNumber), status, pszFileName, lineNumber);
        }
      }

      static inline void Check(const vx_status status, const std::string& message, const std::string& fileName, const int lineNumber)
      {
        if (status != VX_SUCCESS)
        {
          throw OpenVXStatusErrorException(ToNiceMessage(message, status, fileName, lineNumber), status, fileName, lineNumber);
        }
      }

      template<typename TReturnType>
      static inline void Check(const TReturnType* pResult)
      {
        if (pResult == nullptr)
        {
          throw OpenVXCreateException("Unknown");
        }
      }

      template<typename TReturnType>
      static inline void Check(const TReturnType* pResult, const char*const pszMessage)
      {
        if (pResult == nullptr)
        {
          assert(pszMessage != nullptr);
          throw OpenVXCreateException(pszMessage);
        }
      }


      template<typename TReturnType>
      static inline void Check(const TReturnType* pResult, const std::string& message)
      {
        if (pResult == nullptr)
        {
          throw OpenVXCreateException(message);
        }
      }

      template<typename TReturnType>
      static inline void Check(const TReturnType* pResult, const char*const pszMessage, const char*const pszFileName, const int lineNumber)
      {
        if (pResult == nullptr)
        {
          assert(pszMessage != nullptr);
          assert(pszFileName != nullptr);
          throw OpenVXCreateException(ToNiceMessage(pszMessage, pszFileName, lineNumber), pszFileName, lineNumber);
        }
      }

      template<typename TReturnType>
      static inline void Check(const TReturnType* pResult, const char*const pszMessage, const std::string& fileName, const int lineNumber)
      {
        if (pResult == nullptr)
        {
          assert(pszMessage != nullptr);
          throw OpenVXCreateException(ToNiceMessage(pszMessage, fileName, lineNumber), fileName, lineNumber);
        }
      }

      template<typename TReturnType>
      static inline void Check(const TReturnType* pResult, const std::string& message, const char*const pszFileName, const int lineNumber)
      {
        if (pResult == nullptr)
        {
          assert(pszFileName != nullptr);
          throw OpenVXCreateException(ToNiceMessage(message, pszFileName, lineNumber), pszFileName, lineNumber);
        }
      }

      template<typename TReturnType>
      static inline void Check(const TReturnType* pResult, const std::string& message, const std::string& fileName, const int lineNumber)
      {
        if (pResult == nullptr)
        {
          throw OpenVXCreateException(ToNiceMessage(message, fileName, lineNumber), fileName, lineNumber);
        }
      }
    private:
      static std::string ToNiceMessage(const std::string& message, const vx_status errorCode);
      static std::string ToNiceMessage(const std::string& message, const vx_status errorCode, const std::string& fileName, const int lineNumber);
      static std::string ToNiceMessage(const std::string& message, const std::string& fileName, const int lineNumber);
    };
  }
}

#endif
