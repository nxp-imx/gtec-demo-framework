/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslSimpleUI/Base/Control/ValueLabel.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>
#include <cstdio>

namespace Fsl
{
  using namespace std;

  namespace UI
  {
    namespace
    {
      // -2147483648
      //  2147483647
      // 000000000011
      // 123456789012
      const int BUFFER_SIZE = 12;
    }


    ValueLabel::ValueLabel(const std::shared_ptr<WindowContext>& context)
      : LabelBase(context)
    {
    }


    void ValueLabel::SetContent(const int32_t value)
    {
      if (value != m_content)
      {
        m_content = value;
        // convert to a string
        char tmp[BUFFER_SIZE];
#ifdef _WIN32
        int charsWritten = sprintf_s(tmp, BUFFER_SIZE, "%d", m_content);
#else
        int charsWritten = snprintf(tmp, BUFFER_SIZE, "%d", m_content);
#endif
        if (charsWritten < 0 || charsWritten >= BUFFER_SIZE)
          throw std::invalid_argument("number conversion failed");

        DoSetContent(tmp);
      }
    }



    Vector2 ValueLabel::MeasureRenderedValue(const int32_t value)
    {
      // convert to a string
      char tmp[BUFFER_SIZE];
#ifdef _WIN32
      int charsWritten = sprintf_s(tmp, BUFFER_SIZE, "%d", value);
#else
      int charsWritten = snprintf(tmp, BUFFER_SIZE, "%d", value);
#endif
      if (charsWritten < 0 || charsWritten >= BUFFER_SIZE)
        throw std::invalid_argument("number conversion failed");
      return DoMeasureRenderedString(tmp);
    }
  }
}
