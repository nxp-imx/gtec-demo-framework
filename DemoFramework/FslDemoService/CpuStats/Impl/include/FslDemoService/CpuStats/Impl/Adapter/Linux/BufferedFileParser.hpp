#ifndef FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_BUFFEREDFILEPARSER_HPP
#define FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_BUFFEREDFILEPARSER_HPP
#ifdef __linux__
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

#include <FslDemoService/CpuStats/Impl/Adapter/Linux/BasicFileReader.hpp>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <fstream>

namespace Fsl
{
  template <std::size_t TN>
  class BufferedFileParser
  {
    std::array<char, TN> m_buffer;
    uint32_t m_bufferIndex{0u};
    uint32_t m_bufferLength{0u};

  public:
    BufferedFileParser() = default;

    void Clear()
    {
      m_bufferIndex = 0u;
      m_bufferLength = 0u;
    }

    //! @Check if the current position in the file starts with the given string (true if it does, false if it doesn't)
    //! @note Beware that the buffer size must be large enough to contain the entire string in psz
    bool StartsWith(BasicFileReader& rFile, const char* const psz)
    {
      assert(psz != nullptr);
      auto len = std::strlen(psz);
      assert(len <= TN);
      if (m_bufferLength < len)
      {
        FillBuffer(rFile);
      }
      if (m_bufferLength < len)
      {
        return false;
      }

      uint32_t index = m_bufferIndex;
      const char* pszSrc = psz;

      while (*pszSrc != 0 && *pszSrc == m_buffer[index])
      {
        index = (index + 1) % m_buffer.size();
        ++pszSrc;
      }
      return *pszSrc == 0;
    }

    inline bool IsGood(BasicFileReader& rFile) const
    {
      return m_bufferLength > 0 || rFile.IsGood();
    }

    bool TrySkipToNewLine(BasicFileReader& rFile)
    {
      while (IsGood(rFile))
      {
        if (m_bufferLength <= 0u)
        {
          FillBuffer(rFile);
        }
        while (m_bufferLength > 0u)
        {
          if (m_buffer[m_bufferIndex] == '\n')
          {
            // Consume the '\n'
            m_bufferIndex = (m_bufferIndex + 1) % m_buffer.size();
            --m_bufferLength;
            return true;
          }
          m_bufferIndex = (m_bufferIndex + 1) % m_buffer.size();
          --m_bufferLength;
        }
      }
      return false;
    }

    bool TrySkipCharacter(BasicFileReader& rFile, const char skipChar)
    {
      while (IsGood(rFile))
      {
        if (m_bufferLength <= 0u)
        {
          FillBuffer(rFile);
        }
        while (m_bufferLength > 0u)
        {
          if (m_buffer[m_bufferIndex] != skipChar)
          {
            return true;
          }
          m_bufferIndex = (m_bufferIndex + 1) % m_buffer.size();
          --m_bufferLength;
        }
      }
      return false;
    }

    bool TryReadLineUntil(std::size_t& rWritten, BasicFileReader& rFile, char* const pDstStart, const std::size_t dstSize, const char endChar,
                          const bool lineEndIsMatch = false)
    {
      rWritten = 0u;
      char* pDst = pDstStart;
      const char* const pDstEnd = pDst + dstSize;
      while (IsGood(rFile))
      {
        if (m_bufferLength <= 0u)
        {
          FillBuffer(rFile);
        }
        while (m_bufferLength > 0u)
        {
          if (pDst >= pDstEnd)
          {
            return false;
          }
          if (m_buffer[m_bufferIndex] == '\n' || m_buffer[m_bufferIndex] == endChar)
          {
            // This does not consume the '\n' or endChar
            rWritten = pDst - pDstStart;
            return m_buffer[m_bufferIndex] == endChar || lineEndIsMatch;
          }
          *pDst = m_buffer[m_bufferIndex];
          ++pDst;
          m_bufferIndex = (m_bufferIndex + 1) % m_buffer.size();
          --m_bufferLength;
        }
      }
      return false;
    }

  private:
    void FillBuffer(BasicFileReader& rFile)
    {
      if (m_bufferLength == 0u)
      {
        m_bufferLength = rFile.Read(m_buffer.data(), m_buffer.size());
        m_bufferIndex = 0u;
      }
      else if (m_bufferLength < m_buffer.size())
      {
        FillPartialBuffer(rFile);
        if (m_bufferLength < m_buffer.size() && rFile.IsGood())
        {
          FillPartialBuffer(rFile);
        }
      }
    }

    void FillPartialBuffer(BasicFileReader& rFile)
    {
      auto freeCapacity = m_buffer.size() - m_bufferLength;
      auto bufferEndIndex = (m_bufferIndex + m_bufferLength) % m_buffer.size();

      auto freeEndIndex = bufferEndIndex + freeCapacity;
      auto fromEntriesAtEnd = std::min(freeEndIndex, m_buffer.size()) - bufferEndIndex;
      if (fromEntriesAtEnd > 0u)
      {
        assert((bufferEndIndex + fromEntriesAtEnd) <= m_buffer.size());
        m_bufferLength += rFile.Read(m_buffer.data() + bufferEndIndex, fromEntriesAtEnd);
        assert(m_bufferLength <= m_buffer.size());
      }
    }
  };
}

#endif
#endif
