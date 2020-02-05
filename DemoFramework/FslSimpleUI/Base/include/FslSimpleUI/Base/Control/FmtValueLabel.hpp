#ifndef FSLSIMPLEUI_BASE_CONTROL_FMTVALUELABEL_HPP
#define FSLSIMPLEUI_BASE_CONTROL_FMTVALUELABEL_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslSimpleUI/Base/Control/LabelBase.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <fmt/format.h>
#include <cstring>
#include <string>

namespace Fsl
{
  class AtlasFont;

  namespace UI
  {
    template <typename TValue>
    class FmtValueLabel : public LabelBase
    {
    public:
      using value_type = TValue;

    private:
      value_type m_content{};
      std::string m_format;
      mutable fmt::memory_buffer m_buffer;
      mutable bool m_cacheIsValid{false};

    public:
      FmtValueLabel(const std::shared_ptr<WindowContext>& context)
        : LabelBase(context)
      {
      }

      value_type GetContent() const
      {
        return m_content;
      }

      const std::string& GetFormatString() const
      {
        return m_format;
      }

      void SetFormatString(const char* const psz)
      {
        if (psz == nullptr)
        {
          throw std::invalid_argument("psz can not be null");
        }

        SetFormatString(StringViewLite(psz, std::strlen(psz)));
      }

      void SetFormatString(const StringViewLite& value)
      {
        if (value != StringViewLite(m_format.data(), m_format.size()))
        {
          m_format.assign(value.data(), value.size());
          m_cacheIsValid = false;
          PropertyUpdated(PropertyType::Content);
        }
      }

      void SetFormatString(const std::string& strFormat)
      {
        if (strFormat != m_format)
        {
          m_format = strFormat;
          m_cacheIsValid = false;
          PropertyUpdated(PropertyType::Content);
        }
      }

      void SetContent(const value_type value)
      {
        if (m_content != value)
        {
          m_content = value;
          m_cacheIsValid = false;
          PropertyUpdated(PropertyType::Content);
        }
      }

    protected:
      StringViewLite DoGetContent() const override
      {
        if (!m_cacheIsValid)
        {
          m_buffer.clear();
          fmt::format_to(m_buffer, m_format, m_content);
          m_cacheIsValid = true;
        }
        return StringViewLite(m_buffer.data(), m_buffer.size());
      }
    };
  }
}

#endif
