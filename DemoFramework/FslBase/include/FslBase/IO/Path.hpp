#ifndef FSLBASE_IO_PATH_HPP
#define FSLBASE_IO_PATH_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/String/UTF8String.hpp>

namespace Fsl
{
  namespace IO
  {
    // A class do manager file paths
    // The current implementation uses UTF8String's internally but that could change in the future.
    // Path directory separators are also always represented as '/' never as '\'
    class Path
    {
      UTF8String m_content;
      Path(const std::string& str, const bool bUnchecked);

    public:
      // move assignment operator
      Path& operator=(Path&& other) noexcept;
      // move constructor
      Path(Path&& other) noexcept;
      // Request that the compiler generates a copy constructor and assignment operator
      Path(const Path&) = default;
      Path& operator=(const Path&) = default;


      Path() = default;

      Path(UTF8String str);
      Path(const std::string& str);
      Path(const char* const psz);
      ~Path();

      bool IsEmpty() const
      {
        return m_content.IsEmpty();
      }

      bool Contains(const char ch) const
      {
        return m_content.Contains(ch);
      }

      bool Contains(const char* const psz) const
      {
        return m_content.Contains(psz);
      }

      bool Contains(const UTF8String& str) const
      {
        return m_content.Contains(str);
      }

      bool Contains(const Path& path) const
      {
        return m_content.Contains(path.m_content);
      }

      bool StartsWith(const char ch) const
      {
        return m_content.StartsWith(ch);
      }

      bool StartsWith(const char* const psz) const
      {
        return m_content.StartsWith(psz);
      }

      bool StartsWith(const UTF8String& path) const
      {
        return m_content.StartsWith(path);
      }

      bool StartsWith(const Path& path) const
      {
        return m_content.StartsWith(path.m_content);
      }

      bool EndsWith(const char ch) const
      {
        return m_content.EndsWith(ch);
      }

      bool EndsWith(const char* const psz) const
      {
        return m_content.EndsWith(psz);
      }

      bool EndsWith(const UTF8String& path) const
      {
        return m_content.EndsWith(path);
      }

      bool EndsWith(const Path& path) const
      {
        return m_content.EndsWith(path.m_content);
      }

      int32_t IndexOf(const char ch, const std::size_t fromIndex = 0) const
      {
        return m_content.IndexOf(ch, fromIndex);
      }

      std::string ToUTF8String() const
      {
        return m_content.ToUTF8String();
      };

      std::string ToAsciiString() const;

      int32_t GetByteSize() const
      {
        return m_content.GetByteSize();
      }

      // Path ToLowerInvariant() const;
      // Path ToUpperInvariant() const;

      //! @brief Check is the path is considered rooted
      static bool IsPathRooted(const Path& path);

      //! @brief Combine two strings into a path
      //! @param path1 = the first path
      //! @param path2 = the second path
      //! @return the combined path
      static Path Combine(const Path& path1, const Path& path2);

      //! @brief Get the directory name for the specified path string
      //! @param path = the path to extract the directory name from
      //! @return the directory name (or a empty string if the supplied path doesn't contain directory information).
      static Path GetDirectoryName(const Path& path);

      //! @brief Get the file name for the specified path string
      //! @param path = the path to extract the filename name from
      //! @return the file name (or a empty string if the supplied path doesn't contain filename information).
      static Path GetFileName(const Path& path);

      //! @brief Get the the extension of the specified path string
      //! @param path = the path to extract the extension from
      //! @return the extension including the '.' (or a empty string if the supplied path doesn't contain a extension).
      static Path GetExtension(const Path& path);

      //! @brief Get the fully qualified location of a path (the absolute path).
      //! @param path = the path that should be made absolute.
      //! @return the fully qualified path
      static Path GetFullPath(const Path& path);

      bool operator==(const Path& rhs) const
      {
        return m_content == rhs.m_content;
      }
      bool operator!=(const Path& rhs) const
      {
        return m_content != rhs.m_content;
      }
    };
  }
}

#endif
