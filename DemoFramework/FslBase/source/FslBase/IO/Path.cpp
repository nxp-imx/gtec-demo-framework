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

#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/IO/PathViewHelper.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <algorithm>
#include <cassert>
#include <utility>
#include "../System/Platform/Platform.hpp"
// #include <locale>

namespace Fsl::IO
{
  Path::Path(const char* const psz)
    : m_content(psz)
  {
    m_content.Replace('\\', '/');
  }

  Path::Path(const StringViewLite& str)
    : m_content(str)
  {
    m_content.Replace('\\', '/');
  }

  Path::Path(const PathView& str)
    : m_content(str)
  {
  }

  Path::Path(UTF8String str)
    : m_content(std::move(str))
  {
    m_content.Replace('\\', '/');
  }


  Path::Path(std::string str)
    : m_content(std::move(str))
  {
    m_content.Replace('\\', '/');
  }


  std::string Path::ToAsciiString() const
  {
    return m_content.ToAsciiString();
  }


  Path& Path::operator=(const StringViewLite& str)
  {
    m_content = str;
    m_content.Replace('\\', '/');
    return *this;
  }


  Path& Path::operator=(const PathView& str)
  {
    m_content = str;
    return *this;
  }

  void Path::Append(const std::size_t count, const char ch)
  {
    auto finalChar = ch != '\\' ? ch : '/';
    m_content.Append(count, finalChar);
  }

  void Path::Append(const StringViewLite& str)
  {
    m_content.Append(str);
    m_content.Replace('\\', '/');
  }

  void Path::Append(const PathView& str)
  {
    m_content.Append(str);
  }

  void Path::Prepend(const std::size_t count, const char ch)
  {
    auto finalChar = ch != '\\' ? ch : '/';
    m_content.Prepend(count, finalChar);
  }

  void Path::Prepend(const PathView& str)
  {
    m_content.Prepend(str);
  }

  void Path::Prepend(const StringViewLite& str)
  {
    m_content.Prepend(str);
    m_content.Replace('\\', '/');
  }


  // Path Path::ToLowerInvariant() const
  //{

  //}


  // Path Path::ToUpperInvariant() const
  //{
  //}


  bool Path::IsPathRooted(const PathView& path)
  {
    // A fairly simple check for rooted paths
    return !path.empty() && (path.starts_with('/') || StringUtil::Contains(path, ':'));
  }


  Path Path::Combine(const PathView& path1, const PathView& path2)
  {
    if (Path::IsPathRooted(path2))
    {
      return path2;
    }

    if (path1.empty())
    {
      return path2;
    }
    if (path2.empty())
    {
      return path1;
    }
    if (!path1.ends_with('/'))
    {
      Path newPath(path1);
      newPath.Append(1u, '/');
      newPath.Append(path2);
      return newPath;
    }

    return IO::Path(path1) + path2;
  }


  PathView Path::GetDirectoryNameView(const PathView& path)
  {
    const int32_t index = StringUtil::LastIndexOf(path, '/');
    if (index <= 0)
    {
      return {};
    }
    return path.subpath(0, index);
  }


  PathView Path::GetFileNameView(const PathView& path)
  {
    // locate the last index of '/'
    auto index = path.rfind('/');
    if (index == PathView::npos)
    {
      // not found -> so just return the path
      return path;
    }
    // +1 to skip the '/'
    ++index;
    assert(index <= path.size());
    return path.subpath(index, path.size() - index);
  }


  PathView Path::GetFileNameWithoutExtensionView(const PathView& path)
  {
    // locate the last index of '.'
    auto index = path.rfind('.');
    if (index == PathView::npos)
    {
      return GetFileNameView(path);
    }

    auto charsToSkip = (path.size() - index);
    assert(charsToSkip <= path.size());
    // locate the last index of '/'
    auto indexSlash = path.rfind('/');
    indexSlash = indexSlash != PathView::npos ? indexSlash + 1u : 0u;
    assert(indexSlash <= path.size());
    assert(charsToSkip <= (path.size() - indexSlash));
    return path.subpath(indexSlash, path.size() - indexSlash - charsToSkip);
  }


  PathView Path::GetExtensionView(const PathView& path)
  {
    // locate the last index of '.'
    auto dotIndex = path.rfind('.');
    if (dotIndex == PathView::npos)
    {
      return {};
    }

    // locate the last index of '/'
    auto indexSlash = path.rfind('/');
    if (indexSlash != PathView::npos && dotIndex < indexSlash)
    {
      return {};
    }

    assert(dotIndex <= path.size());
    return path.subpath(dotIndex, path.size() - dotIndex);
  }


  Path Path::GetFullPath(const PathView& path)
  {
    // We get the path from a external location, so we need to convert the string to a path
    return Path(Platform::GetFullPath(PathViewHelper::ToString(path)));
  }


  Path Path::GetFullPath(const Path& path)
  {
    // We get the path from a external location, so we need to convert the string to a path
    return Path(Platform::GetFullPath(path.m_content.ToUTF8String()));
  }
}
