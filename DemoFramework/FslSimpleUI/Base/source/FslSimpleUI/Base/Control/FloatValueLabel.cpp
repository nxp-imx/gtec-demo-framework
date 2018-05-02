/****************************************************************************************************************************************************
* Copyright 2018 NXP
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

#include <FslSimpleUI/Base/Control/FloatValueLabel.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/StringCompat.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <cassert>

namespace Fsl
{
  using namespace std;

  namespace UI
  {
    namespace
    {
      // Since its floats we make room for a lot of numbers
      const int BUFFER_SIZE = 64;
      const uint32_t MAX_FORMAT_DECIMALS = 16;

      bool TryFormatToString(char* pDst, const std::size_t dstSize, const uint32_t numDecimals, const float value)
      {
        // convert to a string
        char tmpFormat[BUFFER_SIZE];
        auto charsWritten = StringCompat::sprintf_s(tmpFormat, BUFFER_SIZE, "%%.%df", numDecimals);
        if (charsWritten < 0)
        {
          FSLLOG_DEBUG_WARNING("Could not build format string");
          return false;
        }

        charsWritten = StringCompat::sprintf_s(pDst, dstSize, tmpFormat, value);
        return (charsWritten > 0 && static_cast<std::size_t>(charsWritten) < dstSize);
      }

    }


    FloatValueLabel::FloatValueLabel(const std::shared_ptr<WindowContext>& context)
      : LabelBase(context)
      , m_formattingDecimals(2)
      , m_contentCache()
      , m_cacheIsValid(false)
    {
    }


    void FloatValueLabel::SetContent(const float value)
    {
      if (EqualHelper::IsAlmostEqual(value, m_content))
        return;

      m_content = value;
      m_cacheIsValid = false;
      PropertyUpdated(PropertyType::Content);
    }


    void FloatValueLabel::SetFormatDecimals(const uint32_t value)
    {
      const auto cappedValue = std::min(value, MAX_FORMAT_DECIMALS);
      if (cappedValue == m_formattingDecimals)
        return;

      m_formattingDecimals = cappedValue;
      m_cacheIsValid = false;
      PropertyUpdated(PropertyType::Content);
    }



    Vector2 FloatValueLabel::MeasureRenderedValue(const float value)
    {
      // convert to a string
      char tmp[BUFFER_SIZE];
      if (!TryFormatToString(tmp, BUFFER_SIZE, m_formattingDecimals, value))
        return Vector2();
      return DoMeasureRenderedString(tmp);
    }


    const std::string& FloatValueLabel::DoGetContent() const
    {
      if (!m_cacheIsValid)
      {
        m_cacheIsValid = true;

        char tmp[BUFFER_SIZE];
        if (!TryFormatToString(tmp, BUFFER_SIZE, m_formattingDecimals, m_content))
          tmp[0] = 0;
        m_contentCache = tmp;
      }
      return m_contentCache;
    }
  }
}
