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
#include <FslBase/IO/PathView.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/String/UTF8String.hpp>

namespace Fsl::IO
{
  // A class do manager file paths
  // The current implementation uses UTF8String's internally but that could change in the future.
  // Path directory separators are also always represented as '/' never as '\'
  class Path
  {
    UTF8String m_content;

    Path(std::string str, const bool bUnchecked)
      : m_content(std::move(str))
    {
      FSL_PARAM_NOT_USED(bUnchecked);
    }


    Path(UTF8String str, const bool bUnchecked)
      : m_content(std::move(str))
    {
      FSL_PARAM_NOT_USED(bUnchecked);
    }

  public:
    // move assignment operator
    Path& operator=(Path&& other) noexcept
    {
      if (this != &other)
      {
        m_content = std::move(other.m_content);
      }
      return *this;
    }

    // move constructor
    Path(Path&& other) noexcept
      : m_content(std::move(other.m_content))
    {
    }

    // Request that the compiler generates a copy constructor and assignment operator
    Path(const Path&) = default;
    Path& operator=(const Path&) = default;


    Path() noexcept = default;

    Path(const char* const psz);       // NOLINT(google-explicit-constructor)
    Path(const StringViewLite str);    // NOLINT(google-explicit-constructor)
    Path(const PathView str);          // NOLINT(google-explicit-constructor)
    explicit Path(std::string str);
    explicit Path(UTF8String str);
    ~Path() noexcept = default;

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

    bool StartsWith(const StringViewLite path) const
    {
      return m_content.StartsWith(path);
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

    bool EndsWith(const StringViewLite path) const
    {
      return m_content.EndsWith(path);
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

    const UTF8String& AsUTF8String() const
    {
      return m_content;
    }

    const std::string& ToUTF8String() const
    {
      return m_content.ToUTF8String();
    }

    std::string ToAsciiString() const;


    Path& operator=(const StringViewLite str);
    Path& operator=(const PathView str);

    Path& operator=(const char* const psz)
    {
      *this = StringViewLite(psz);
      return *this;
    }

    Path& operator=(const UTF8String& str)
    {
      *this = str.AsStringViewLite();
      return *this;
    }

    int32_t GetByteSize() const
    {
      return m_content.GetByteSize();
    }

    void Clear()
    {
      m_content.Clear();
    }

    // @brief append count copies of char 'ch' to the end of the string
    void Append(const std::size_t count, const char ch);

    //! @brief append the string at the end of the string
    void Append(const char* const psz)
    {
      Append(StringViewLite(psz));
    }

    //! @brief append the string at the end of the string
    void Append(const StringViewLite str);
    //! @brief append the string at the end of the string
    void Append(const PathView str);
    //! @brief append the string at the end of the string
    void Append(const UTF8String& str)
    {
      Append(str.AsStringViewLite());
    }

    //! @brief append the string at the end of the string
    void Append(const Path& str)
    {
      Append(str.AsPathView());
    }

    // @brief Insert count copies of char 'ch' at the beginning at the current string
    void Prepend(const std::size_t count, const char ch);

    // @brief Insert the string at the beginning at the current string
    void Prepend(const char* const psz)
    {
      Prepend(StringViewLite(psz));
    }

    // @brief Insert the string at the beginning at the current string
    void Prepend(const PathView str);
    // @brief Insert the string at the beginning at the current string
    void Prepend(const StringViewLite str);
    // @brief Insert the string at the beginning at the current string
    void Prepend(const UTF8String& str)
    {
      Prepend(str.AsStringViewLite());
    }

    // @brief Insert the string at the beginning at the current string
    void Prepend(const Path& str)
    {
      Prepend(str.AsPathView());
    }


    // Path ToLowerInvariant() const;
    // Path ToUpperInvariant() const;

    //! @brief Check is the path is considered rooted
    static bool IsPathRooted(const PathView path);

    static bool IsPathRooted(const Path& path)
    {
      return IsPathRooted(path.AsPathView());
    }

    //! @brief Combine two strings into a path
    //! @param path1 = the first path
    //! @param path2 = the second path
    //! @return the combined path
    static Path Combine(const PathView path1, const PathView path2);

    //! @brief Combine two strings into a path
    //! @param path1 = the first path
    //! @param path2 = the second path
    //! @return the combined path
    static Path Combine(const Path& path1, const Path& path2)
    {
      return Combine(path1.AsPathView(), path2.AsPathView());
    }

    //! @brief Combine two strings into a path
    //! @param path1 = the first path
    //! @param path2 = the second path
    //! @return the combined path
    static Path Combine(const PathView path1, const Path& path2)
    {
      return Combine(path1, path2.AsPathView());
    }

    //! @brief Combine two strings into a path
    //! @param path1 = the first path
    //! @param path2 = the second path
    //! @return the combined path
    static Path Combine(const Path& path1, const PathView path2)
    {
      return Combine(path1.AsPathView(), path2);
    }

    //! @brief Get the directory name for the specified path string
    //! @param path = the path to extract the directory name from
    //! @return the directory name (or a empty string if the supplied path doesn't contain directory information).
    static PathView GetDirectoryNameView(const PathView path);

    //! @brief Get the directory name for the specified path string
    //! @param path = the path to extract the directory name from
    //! @return the directory name (or a empty string if the supplied path doesn't contain directory information).
    static Path GetDirectoryName(const PathView path)
    {
      return GetDirectoryNameView(path);
    }

    static Path GetDirectoryName(const Path& path)
    {
      return GetDirectoryNameView(path.AsPathView());
    }


    //! @brief Get the file name for the specified path string
    //! @param path = the path to extract the filename name from
    //! @return the file name (or a empty string if the supplied path doesn't contain filename information).
    static PathView GetFileNameView(const PathView path);

    //! @brief Get the file name for the specified path string
    //! @param path = the path to extract the filename name from
    //! @return the file name (or a empty string if the supplied path doesn't contain filename information).
    static Path GetFileName(const PathView path)
    {
      return GetFileNameView(path);
    }

    //! @brief Get the file name for the specified path string
    //! @param path = the path to extract the filename name from
    //! @return the file name (or a empty string if the supplied path doesn't contain filename information).
    static Path GetFileName(const Path& path)
    {
      return GetFileNameView(path.AsPathView());
    }

    //! @brief The result of GetFileName minus the last "." and all characters following it.
    static PathView GetFileNameWithoutExtensionView(const PathView path);

    //! @brief The result of GetFileName minus the last "." and all characters following it.
    static Path GetFileNameWithoutExtension(const PathView path)
    {
      return GetFileNameWithoutExtensionView(path);
    }

    //! @brief The result of GetFileName minus the last "." and all characters following it.
    static Path GetFileNameWithoutExtension(const Path& path)
    {
      return GetFileNameWithoutExtensionView(path.AsPathView());
    }

    //! @brief Get the the extension of the specified path string
    //! @param path = the path to extract the extension from
    //! @return the extension including the '.' (or a empty string if the supplied path doesn't contain a extension).
    static PathView GetExtensionView(const PathView path);

    //! @brief Get the the extension of the specified path string
    //! @param path = the path to extract the extension from
    //! @return the extension including the '.' (or a empty string if the supplied path doesn't contain a extension).
    static Path GetExtension(const PathView path)
    {
      return GetExtensionView(path);
    }

    //! @brief Get the the extension of the specified path string
    //! @param path = the path to extract the extension from
    //! @return the extension including the '.' (or a empty string if the supplied path doesn't contain a extension).
    static Path GetExtension(const Path& path)
    {
      return GetExtensionView(path.AsPathView());
    }

    //! @brief Get the fully qualified location of a path (the absolute path).
    //! @param path = the path that should be made absolute.
    //! @return the fully qualified path
    static Path GetFullPath(const PathView path);

    //! @brief Get the fully qualified location of a path (the absolute path).
    //! @param path = the path that should be made absolute.
    //! @return the fully qualified path
    static Path GetFullPath(const Path& path);

    // NOTE: there is no operator+ or += that takes a char as that can lead subtle bugs

    Path& operator+=(const char* const pszRhs)
    {
      Append(StringViewLite(pszRhs));
      return *this;
    }

    Path& operator+=(const PathView rhs)
    {
      Append(rhs);
      return *this;
    }

    Path& operator+=(const StringViewLite rhs)
    {
      Append(rhs);
      return *this;
    }

    Path& operator+=(const UTF8String& rhs)
    {
      Append(rhs);
      return *this;
    }

    Path& operator+=(const Path& rhs)
    {
      Append(rhs);
      return *this;
    }

    inline friend Path operator+(Path lhs, const char* const pszRhs)
    {
      lhs.Append(StringViewLite(pszRhs));
      return lhs;
    }

    inline friend Path operator+(Path lhs, const PathView rhs)
    {
      lhs.Append(rhs);
      return lhs;
    }

    inline friend Path operator+(Path lhs, const StringViewLite rhs)
    {
      lhs.Append(rhs);
      return lhs;
    }

    inline friend Path operator+(Path lhs, const Path& rhs)
    {
      lhs.Append(rhs);
      return lhs;
    }

    inline friend Path operator+(const char* const pszLhs, Path rhs)
    {
      rhs.Prepend(StringViewLite(pszLhs));
      return rhs;
    }

    inline friend Path operator+(const StringViewLite lhs, Path rhs)
    {
      rhs.Prepend(lhs);
      return rhs;
    }

    inline friend Path operator+(const PathView lhs, Path rhs)
    {
      rhs.Prepend(lhs);
      return rhs;
    }

    inline StringViewLite AsStringViewLite() const noexcept
    {
      return m_content.AsStringViewLite();
    }

    inline std::string_view AsStringView() const noexcept
    {
      return m_content.AsStringView();
    }

    inline PathView AsPathView() const noexcept
    {
      return PathView(m_content.AsStringViewLite(), OptimizationCheckFlag::NoCheck);
    }
  };


  // Operator ==

  inline bool operator==(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.AsPathView() == rhs.AsPathView();
  }

  inline bool operator==(const Path& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsPathView() == pszRhs;
  }

  inline bool operator==(const Path& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsPathView() == rhs;
  }

  inline bool operator==(const Path& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsPathView() == StringViewLite(rhs);
  }

  inline bool operator==(const Path& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsPathView() == rhs.AsStringViewLite();
  }

  inline bool operator==(const char* const pszLhs, const Path& rhs) noexcept
  {
    return pszLhs == rhs.AsPathView();
  }

  inline bool operator==(const StringViewLite lhs, const Path& rhs) noexcept
  {
    return lhs == rhs.AsPathView();
  }

  inline bool operator==(const std::string& lhs, const Path& rhs) noexcept
  {
    return StringViewLite(lhs) == rhs.AsPathView();
  }

  inline bool operator==(const UTF8String& lhs, const Path& rhs) noexcept
  {
    return lhs.AsStringViewLite() == rhs.AsPathView();
  }

  // Operator !=

  inline bool operator!=(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.AsPathView() != rhs.AsPathView();
  }


  inline bool operator!=(const Path& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsPathView() != pszRhs;
  }

  inline bool operator!=(const Path& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsPathView() != rhs;
  }

  inline bool operator!=(const Path& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsPathView() != StringViewLite(rhs);
  }

  inline bool operator!=(const Path& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsPathView() != rhs.AsStringViewLite();
  }

  inline bool operator!=(const char* const pszLhs, const Path& rhs) noexcept
  {
    return pszLhs != rhs.AsPathView();
  }

  inline bool operator!=(const StringViewLite lhs, const Path& rhs) noexcept
  {
    return lhs != rhs.AsPathView();
  }

  inline bool operator!=(const std::string& lhs, const Path& rhs) noexcept
  {
    return StringViewLite(lhs) != rhs.AsPathView();
  }

  inline bool operator!=(const UTF8String& lhs, const Path& rhs) noexcept
  {
    return lhs.AsStringViewLite() != rhs.AsPathView();
  }

  // Operator <

  inline bool operator<(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.AsPathView() < rhs.AsPathView();
  }

  inline bool operator<(const Path& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsPathView() < pszRhs;
  }

  inline bool operator<(const Path& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsPathView() < rhs;
  }

  inline bool operator<(const Path& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsPathView() < StringViewLite(rhs);
  }

  inline bool operator<(const Path& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsPathView() < rhs.AsStringViewLite();
  }

  inline bool operator<(const char* const pszLhs, const Path& rhs) noexcept
  {
    return pszLhs < rhs.AsPathView();
  }

  inline bool operator<(const StringViewLite lhs, const Path& rhs) noexcept
  {
    return lhs < rhs.AsPathView();
  }

  inline bool operator<(const std::string& lhs, const Path& rhs) noexcept
  {
    return StringViewLite(lhs) < rhs.AsPathView();
  }

  inline bool operator<(const UTF8String& lhs, const Path& rhs) noexcept
  {
    return lhs.AsStringViewLite() < rhs.AsPathView();
  }

  // Operator <=

  inline bool operator<=(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.AsPathView() <= rhs.AsPathView();
  }

  inline bool operator<=(const Path& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsPathView() <= pszRhs;
  }

  inline bool operator<=(const Path& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsPathView() <= rhs;
  }

  inline bool operator<=(const Path& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsPathView() <= StringViewLite(rhs);
  }

  inline bool operator<=(const Path& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsPathView() <= rhs.AsStringViewLite();
  }

  inline bool operator<=(const char* const pszLhs, const Path& rhs) noexcept
  {
    return pszLhs <= rhs.AsPathView();
  }

  inline bool operator<=(const StringViewLite lhs, const Path& rhs) noexcept
  {
    return lhs <= rhs.AsPathView();
  }

  inline bool operator<=(const std::string& lhs, const Path& rhs) noexcept
  {
    return StringViewLite(lhs) <= rhs.AsPathView();
  }

  inline bool operator<=(const UTF8String& lhs, const Path& rhs) noexcept
  {
    return lhs.AsStringViewLite() <= rhs.AsPathView();
  }

  // Operator >

  inline bool operator>(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.AsPathView() > rhs.AsPathView();
  }

  inline bool operator>(const Path& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsPathView() > pszRhs;
  }

  inline bool operator>(const Path& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsPathView() > rhs;
  }

  inline bool operator>(const Path& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsPathView() > StringViewLite(rhs);
  }

  inline bool operator>(const Path& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsPathView() > rhs.AsStringViewLite();
  }

  inline bool operator>(const char* const pszLhs, const Path& rhs) noexcept
  {
    return pszLhs > rhs.AsPathView();
  }

  inline bool operator>(const StringViewLite lhs, const Path& rhs) noexcept
  {
    return lhs > rhs.AsPathView();
  }

  inline bool operator>(const std::string& lhs, const Path& rhs) noexcept
  {
    return StringViewLite(lhs) > rhs.AsPathView();
  }

  inline bool operator>(const UTF8String& lhs, const Path& rhs) noexcept
  {
    return lhs.AsStringViewLite() > rhs.AsPathView();
  }

  // Operator >=

  inline bool operator>=(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.AsPathView() >= rhs.AsPathView();
  }

  inline bool operator>=(const Path& lhs, const char* const pszRhs) noexcept
  {
    return lhs.AsPathView() >= pszRhs;
  }

  inline bool operator>=(const Path& lhs, const StringViewLite rhs) noexcept
  {
    return lhs.AsPathView() >= rhs;
  }

  inline bool operator>=(const Path& lhs, const std::string& rhs) noexcept
  {
    return lhs.AsPathView() >= StringViewLite(rhs);
  }

  inline bool operator>=(const Path& lhs, const UTF8String& rhs) noexcept
  {
    return lhs.AsPathView() >= rhs.AsStringViewLite();
  }

  inline bool operator>=(const char* const pszLhs, const Path& rhs) noexcept
  {
    return pszLhs >= rhs.AsPathView();
  }

  inline bool operator>=(const StringViewLite lhs, const Path& rhs) noexcept
  {
    return lhs >= rhs.AsPathView();
  }

  inline bool operator>=(const std::string& lhs, const Path& rhs) noexcept
  {
    return StringViewLite(lhs) >= rhs.AsPathView();
  }

  inline bool operator>=(const UTF8String& lhs, const Path& rhs) noexcept
  {
    return lhs.AsStringViewLite() >= rhs.AsPathView();
  }
}

#endif
