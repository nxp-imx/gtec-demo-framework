#ifdef _WIN32
/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/System/Platform/PlatformFileSystem.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/PathDeque.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/System/Platform/PlatformWin32.hpp>
#include <windows.h>


namespace Fsl
{
  namespace IO
  {
    struct FileData
    {
      bool IsDataValid;
      _WIN32_FILE_ATTRIBUTE_DATA AttributeData;

      FileData()
        : IsDataValid(false)
      {
      }

      bool operator==(const FileData& rhs) const
      {
        if (!IsDataValid)
          return ! rhs.IsDataValid;

        return AttributeData.dwFileAttributes == rhs.AttributeData.dwFileAttributes &&
               AttributeData.nFileSizeLow == rhs.AttributeData.nFileSizeLow &&
               AttributeData.nFileSizeHigh == rhs.AttributeData.nFileSizeHigh &&
               CompareFileTime(&AttributeData.ftLastWriteTime, &rhs.AttributeData.ftLastWriteTime) == 0;
      }

      bool operator!=(const FileData& rhs) const
      {
        return !(*this == rhs);
      }
    };

    class PlatformPathMonitorToken
    {
    public:
      Path FullPath;
      FileData Data;

      PlatformPathMonitorToken(const Path& fullPath)
        : FullPath(fullPath)
        , Data()
      {
      }
    };

    namespace
    {
      void ExtractData(FileData& rData, const Path& fullPath)
      {
        rData = FileData();
        rData.IsDataValid = (GetFileAttributesEx(fullPath.ToWString().c_str(), GetFileExInfoStandard, &rData.AttributeData) != 0);
      }



      void GetFilesInDirectory(PathDeque& rResult, const IO::Path& path, const bool includeSubdirectories)
      {
        const std::wstring searchPath = path.ToWString() + L"/*";

        WIN32_FIND_DATA fd;
        HANDLE hFind = ::FindFirstFile(searchPath.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
          try
          {
            do
            {
              // read all (real) files in current folder
              // , delete '!' read other 2 default folder . and ..
              if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
              {
                const std::string strFilename(PlatformWin32::Narrow(std::wstring(fd.cFileName)));
                rResult.push_back(std::make_shared<Path>(Path::Combine(path, strFilename)));
              }
              else if (includeSubdirectories && !(fd.cFileName[0] == L'.' && fd.cFileName[1] == 0) && !(fd.cFileName[0] == L'.' && fd.cFileName[1] == L'.' && fd.cFileName[2] == 0))
              {
                const std::string strFilename(PlatformWin32::Narrow(std::wstring(fd.cFileName)));
                const Path subDir = Path::Combine(path, strFilename);
                GetFilesInDirectory(rResult, subDir, true);
              }
            } while (::FindNextFile(hFind, &fd));
            ::FindClose(hFind);
          }
          catch (const std::exception&)
          {
            ::FindClose(hFind);
            throw;
          }
        }
      }
    }


    bool PlatformFileSystem::TryGetAttributes(const Path& path, FileAttributes& rAttributes)
    {
      DWORD res = GetFileAttributes(path.ToWString().c_str());
      if (res == INVALID_FILE_ATTRIBUTES)
        return false;

      rAttributes = FileAttributes();
      if ((res & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        rAttributes.SetFlag(FileAttributes::Directory);
      else
        rAttributes.SetFlag(FileAttributes::File);
      return true;
    }


    std::shared_ptr<PlatformPathMonitorToken> PlatformFileSystem::CreatePathMonitorToken(const Path& fullPath)
    {
      if (!Path::IsPathRooted(fullPath))
        throw std::invalid_argument("path must be rooted");

      auto result = std::make_shared<PlatformPathMonitorToken>(fullPath);
      ExtractData(result->Data, fullPath);
      return result;
    }


    bool PlatformFileSystem::CheckPathForChanges(const std::shared_ptr<PlatformPathMonitorToken>& token)
    {
      if (!token)
        throw std::invalid_argument("token can not be null");

      FileData data;
      ExtractData(data, token->FullPath);

      if (data != token->Data)
      {
        token->Data = data;
        return true;
      }
      return false;
    }


    void PlatformFileSystem::GetFiles(PathDeque& rResult, const Path& path, const SearchOptions searchOptions)
    {
      rResult.clear();
      FileAttributes attr;
      if (! TryGetAttributes(path, attr) || ! attr.HasFlag(FileAttributes::Directory))
        throw DirectoryNotFoundException(path.ToAsciiString());

      switch (searchOptions)
      {
      case SearchOptions::TopDirectoryOnly:
        GetFilesInDirectory(rResult, path, false);
        break;
      case SearchOptions::AllDirectories:
        GetFilesInDirectory(rResult, path, true);
        break;
      default:
        throw NotSupportedException("Unknown search option");
      }
    }


    void PlatformFileSystem::CreateDir(const Path& path)
    {
      const std::wstring name = path.ToWString();
      auto res = CreateDirectory(name.c_str(), nullptr);
      if (res == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
        throw IOException("Failed to create directory");
    }

  }
}

#endif
