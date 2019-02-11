/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/String/StringUtil.hpp>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <sstream>

namespace Fsl
{
  namespace StringUtil
  {
    bool Contains(const std::string& src, const char ch)
    {
      return std::find(src.begin(), src.end(), ch) != src.end();
    }


    bool Contains(const std::string& src, const std::string& str)
    {
      if (!str.empty())
      {
        return std::search(src.begin(), src.end(), str.begin(), str.end()) != src.end();
      }
      return true;
    }


    bool StartsWith(const std::string& src, const char ch)
    {
      return (!src.empty() ? src.front() == ch : false);
    }


    bool StartsWith(const std::string& source, const std::string& value)
    {
      return (source.compare(0, value.size(), value) == 0);
    }


    bool EndsWith(const std::string& src, const char ch)
    {
      return (!src.empty() ? src.back() == ch : false);
    }


    bool EndsWith(const std::string& source, const std::string& value)
    {
      return (source.size() >= value.size() && source.compare(source.size() - value.size(), value.size(), value) == 0);
    }


    int32_t IndexOf(const std::string& source, const char ch)
    {
      const std::size_t index = source.find(ch);
      assert(index == std::string::npos || index <= static_cast<std::size_t>(std::numeric_limits<int32_t>::max()));
      return (index != std::string::npos ? static_cast<int32_t>(index) : -1);
    }


    int32_t IndexOf(const std::string& source, const char ch, const int32_t fromIndex)
    {
      const std::size_t index = source.find(ch, fromIndex);
      assert(index == std::string::npos || index <= static_cast<std::size_t>(std::numeric_limits<int32_t>::max()));
      return (index != std::string::npos ? static_cast<int32_t>(index) : -1);
    }


    int32_t LastIndexOf(const std::string& src, const char ch)
    {
      const size_t index = src.rfind(ch);
      return (index != std::string::npos ? static_cast<int32_t>(index) : -1);
    }


    void Replace(std::string& rSrc, const char from, const char to)
    {
      std::replace(rSrc.begin(), rSrc.end(), from, to);
    }


    void Replace(std::string& rStr, const std::string& oldValue, const std::string& newValue)
    {
      if (oldValue.empty())
      {
        return;
      }

      for (std::size_t pos = 0;; pos += newValue.length())
      {
        pos = rStr.find(oldValue, pos);
        if (pos == std::string::npos)
        {
          break;
        }
        rStr.erase(pos, oldValue.length());
        rStr.insert(pos, newValue);
      }
    }

    // std::string Replace(const std::string& str, const std::string& oldValue, const std::string& newValue)
    //{
    //}

    std::vector<std::string> Split(const std::string& str, const char delimiterChar, const bool removeEmpty)
    {
      std::vector<std::string> result;
      if (str.empty())
      {
        return result;
      }

      std::string::size_type pos;
      std::string::size_type lastPos = 0;
      const std::string::size_type length = str.length();

      while (lastPos < (length + 1))
      {
        pos = str.find(delimiterChar, lastPos);
        if (pos == std::string::npos)
        {
          pos = length;
        }

        if (pos != lastPos || !removeEmpty)
        {
          result.emplace_back(str.data() + lastPos, pos - lastPos);
        }

        lastPos = pos + 1;
      }
      return result;
    }
  }
}
