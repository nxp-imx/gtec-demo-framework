#ifndef FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_BASICFILEREADER_HPP
#define FSLDEMOSERVICE_CPUSTATS_IMPL_ADAPTER_LINUX_BASICFILEREADER_HPP
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

#include <cassert>
#include <cstddef>
#include <fstream>

namespace Fsl
{
  class BasicFileReader
  {
    std::ifstream m_file;

  public:
    BasicFileReader() = default;

    explicit BasicFileReader(const char* const pszFilename)
      : m_file(ValidateFilename(pszFilename), std::ios::in | std::ios::binary)
    {
    }

    void Close()
    {
      m_file.close();
    }


    void Open(const char* const pszFilename)
    {
      m_file.close();
      return m_file.open(ValidateFilename(pszFilename), std::ios::in | std::ios::binary);
    }

    inline bool IsGood() const
    {
      return m_file.good();
    }

    inline uint32_t Read(char* const pDst, const std::size_t dstSize)
    {
      assert(pDst != nullptr);
      m_file.read(pDst, dstSize);
      return static_cast<uint32_t>(m_file.gcount());
    }

  private:
    static inline const char* ValidateFilename(const char* const pszFilename)
    {
      if (pszFilename == nullptr)
      {
        throw std::invalid_argument("pszFilename can not be null");
      }
      return pszFilename;
    }
  };
}

#endif
#endif
