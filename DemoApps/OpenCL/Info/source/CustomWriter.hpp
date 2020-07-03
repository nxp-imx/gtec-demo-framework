#ifndef OPENCL_INFO_CUSTOMWRITER_HPP
#define OPENCL_INFO_CUSTOMWRITER_HPP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <fmt/format.h>

namespace Fsl
{
  class CustomWriter
  {
    uint32_t m_currentIndent{0};
    std::string m_indent;
    fmt::memory_buffer m_buf;

  public:
    void Print(const char* const psz) noexcept
    {
      Logger::WriteLine(LogType::Info, psz);
    }

    template <typename... Args>
    void Print(const char* const pszFormat, const Args&... args) noexcept
    {
      try
      {
        m_buf.clear();
        fmt::format_to(m_buf, m_indent);
        fmt::format_to(m_buf, pszFormat, args...);
        m_buf.push_back(0);
        Logger::WriteLine(LogType::Info, m_buf.data());
      }
      catch(const std::exception& ex)
      {
        // This print is not supposed to throw, so the best thing we can do is just try to log the exception :/
        Logger::WriteLine(LogType::Info, ex.what());
      }
    }

    void PushIndent()
    {
      if (m_currentIndent >= 0x10)
      {
        return;
      }
      ++m_currentIndent;
      RebuildIndentString();
    }

    void PopIndent()
    {
      if (m_currentIndent == 0)
      {
        return;
      }

      --m_currentIndent;
      RebuildIndentString();
    }

  private:
    void RebuildIndentString()
    {
      m_indent.clear();
      for (uint32_t i = 0; i < m_currentIndent; ++i)
      {
        m_indent += "  ";
      }
    }
  };
}

#endif
