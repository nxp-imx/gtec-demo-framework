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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Exceptions.hpp>
#include "../System/Platform/Platform.hpp"
#include <algorithm>
#include <cassert>
#include <utility>
//#include <locale>

namespace Fsl
{
  namespace IO
  {
    // move assignment operator
    Path& Path::operator=(Path&& other) noexcept
    {
      if (this != &other)
      {
        m_content = std::move(other.m_content);
      }
      return *this;
    }


    // Transfer ownership from other to this
    Path::Path(Path&& other) noexcept
      : m_content(std::move(other.m_content))
    {
    }


    Path::Path(const std::string& str, const bool bUnchecked)
      : m_content(str)
    {
    }


    Path::Path(UTF8String str)
      : m_content(std::move(str))
    {
      m_content.Replace('\\', '/');
    }


    Path::Path(const std::string& str)
      : m_content(str)
    {
      m_content.Replace('\\', '/');
    }


    Path::Path(const char* const psz)
      : m_content(psz)
    {
      m_content.Replace('\\', '/');
    }


    Path::~Path() = default;


    std::string Path::ToAsciiString() const
    {
      return m_content.ToAsciiString();
    }


    // Path Path::ToLowerInvariant() const
    //{

    //}


    // Path Path::ToUpperInvariant() const
    //{
    //}


    bool Path::IsPathRooted(const Path& path)
    {
      // A fairly simple check for rooted paths
      return !path.IsEmpty() && (path.StartsWith('/') || path.Contains(':'));
    }


    Path Path::Combine(const Path& path1, const Path& path2)
    {
      if (Path::IsPathRooted(path2))
      {
        return path2;
      }

      if (path1.IsEmpty())
      {
        return path2;
      }
      if (path2.IsEmpty())
      {
        return path1;
      }
      if (!path1.EndsWith('/'))
      {
        return Path(path1.ToUTF8String() + '/' + path2.ToUTF8String(), true);
      }


      return Path(path1.ToUTF8String() + path2.ToUTF8String(), true);
    }


    Path Path::GetDirectoryName(const Path& path)
    {
      const int32_t index = path.m_content.LastIndexOf('/');
      if (index <= 0)
      {
        return Path();
      }
      return Path(UTF8String(path.m_content, 0, index));
    }


    Path Path::GetFileName(const Path& path)
    {
      int32_t index = path.m_content.LastIndexOf('/');
      if (index < 0)
      {
        return path;
      }
      // +1 to skip the '/'
      ++index;
      return Path(UTF8String(path.m_content, index, path.m_content.GetByteSize() - index));
    }


    Path Path::GetExtension(const Path& path)
    {
      const int32_t dotIndex = path.m_content.LastIndexOf('.');
      if (dotIndex < 0)
      {
        return Path();
      }

      const int32_t index = std::max(path.m_content.LastIndexOf('/'), 0);
      if (dotIndex < index)
      {
        return Path();
      }


      return Path(path.m_content.ToUTF8String().substr(dotIndex, path.GetByteSize() - dotIndex));
    }


    Path Path::GetFullPath(const Path& path)
    {
      return Path(Platform::GetFullPath(path.m_content.ToUTF8String()));
    }
  }
}
