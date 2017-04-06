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

#include <FslBase/IO/File.hpp>
#include <FslBase/System/Platform/PlatformFileSystem.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>
#include <fstream>
#include <sstream>
#include <limits>
#include <vector>

// Nasty hack for dealing with UTF8 file names on windows,
// since its the only supported platform that doesn't allow UTF8 strings
// but instead provides its own 'hack' for opening wstring's
#ifdef _WIN32
#define PATH_GET_NAME(X) X.ToWString()
#else
#define PATH_GET_NAME(X) X.ToUTF8String()
#endif

namespace Fsl
{
  namespace IO
  {
    namespace
    {
      std::size_t Convert(const std::streamoff value)
      {
        return static_cast<size_t>(value);
      }

      std::size_t ConvertInt64(const int64_t value)
      {
        return static_cast<size_t>(value);
      }


      std::size_t GetStreamLength(std::ifstream& rStream, const Path& path)
      {
        if (!rStream.good())
        {
          std::string str("File not found '");
          str += path.ToAsciiString();
          str += "'";
          throw IOException(str);
        }

        // Dumb C++ way of getting the stream length
        rStream.seekg(0, rStream.end);
        const std::streamoff streamLength = rStream.tellg();
        rStream.seekg(0, rStream.beg);
        assert(streamLength >= 0);
        return Convert(streamLength);
      }

      void StreamRead(std::ifstream& rStream, const Path& path, void* pDst, const std::size_t length)
      {
        // Read the entire content of the file
        if( length > 0 )
          rStream.read(static_cast<char*>(pDst), length);
        if (!rStream.good())
        {
          std::string str("Failed to read entire file '");
          str += path.ToAsciiString();
          str += "'";
          throw IOException(str);
        }
      }


      void StreamWrite(std::ofstream& rStream, const Path& path, const void* pDst, const std::size_t length)
      {
        // Read the entire content of the file
        if (length > 0)
          rStream.write(static_cast<const char*>(pDst), length);
        if (!rStream.good())
        {
          std::string str("Failed to write entire file '");
          str += path.ToAsciiString();
          str += "'";
          throw IOException(str);
        }
      }
    }


    std::string File::ReadAllText(const Path& path)
    {
      try
      {
        std::ifstream file(PATH_GET_NAME(path), std::ios::in | std::ios::binary);
        const std::size_t length = GetStreamLength(file, path);

        // Read the entire content of the file
        std::vector<char> content(length + 1);
        StreamRead(file, path, content.data(), length);
        content[length] = 0;

        // Convert it to a string
        return std::string(content.data());
      }
      catch (const std::ios_base::failure& ex)
      {
        throw IOException(ex.what());
      }
    }


    bool File::TryReadAllText(std::string& rDst, const Path& path)
    {
      try
      {
        std::ifstream file(PATH_GET_NAME(path), std::ios::in | std::ios::binary);
        if (!file.good())
          return false;

        const std::size_t length = GetStreamLength(file, path);

        // Read the entire content of the file
        std::vector<char> content(length + 1);
        StreamRead(file, path, content.data(), length);
        content[length] = 0;

        // Convert it to a string
        rDst = std::string(content.data());
        return true;
      }
      catch (const std::ios_base::failure&)
      {
        return false;
      }
    }



    bool File::Exists(const Path& path)
    {
      // Use a portable way that checks if the file can be read
      // Which of course isn't exactly the same as if it exists, but it ought to
      // be good enough for our purpose.
      std::ifstream file(PATH_GET_NAME(path));
      return file.good();
    }


    FileAttributes File::GetAttributes(const Path& path)
    {
      FileAttributes attributes;
      if (!TryGetAttributes(path, attributes))
        throw NotFoundException("Path not found");
      return attributes;
    }


    bool File::TryGetAttributes(const Path& path, FileAttributes& rAttributes)
    {
      return PlatformFileSystem::TryGetAttributes(path, rAttributes);
    }


    int64_t File::GetLength(const Path& path)
    {
      std::ifstream file(PATH_GET_NAME(path), std::ios::ate | std::ios::binary);
      if (!file.good())
      {
        std::stringstream strstream;
        strstream << "Failed to open file '" << path.ToAsciiString() << "'";
        throw IOException(strstream.str());
      }
      return file.tellg();
    }


    void File::ReadAllBytes(std::vector<uint8_t>& rTargetArray, const Path& path)
    {
      try
      {
        std::ifstream file(PATH_GET_NAME(path), std::ios::in | std::ios::binary);
        const std::size_t length = GetStreamLength(file, path);
        rTargetArray.resize(length);

        // Read the entire content of the file
        StreamRead(file, path, rTargetArray.data(), length);
      }
      catch (const std::ios_base::failure& ex)
      {
        throw IOException(ex.what());
      }
    }


    int64_t File::ReadAllBytes(void* pDstArray, const int64_t cbDstArray, const Path& path)
    {
      if (pDstArray == nullptr)
        throw std::invalid_argument("pDstArray can not be null");
      if (cbDstArray < 0)
        throw std::invalid_argument("cbDstArray can not be negative");


      const std::size_t cbDstArrayEx = ConvertInt64(cbDstArray);

      try
      {
        std::ifstream file(PATH_GET_NAME(path), std::ios::in | std::ios::binary);
        const std::size_t length = GetStreamLength(file, path);
        if (length > cbDstArrayEx)
        {
          std::string str("Supplied array too small to hold '");
          str += path.ToAsciiString();
          str += "'";
          throw IOException(str);
        }

        StreamRead(file, path, pDstArray, length);
        return length;
      }
      catch (const std::ios_base::failure& ex)
      {
        throw IOException(ex.what());
      }
    }


    std::vector<uint8_t> File::ReadBytes(const Path& path)
    {
      std::vector<uint8_t> targetArray;
      ReadAllBytes(targetArray, path);
      return targetArray;
    }


    void File::ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& path, const int64_t fileOffset, const int64_t bytesToRead)
    {
      if (fileOffset < 0 )
        throw std::invalid_argument("fileOffset can not be negative");
      if (bytesToRead < 0)
        throw std::invalid_argument("bytesToRead can not be negative");

      const std::size_t fileOffsetEx = ConvertInt64(fileOffset);
      const std::size_t bytesToReadEx = ConvertInt64(bytesToRead);

      try
      {
        std::ifstream file(PATH_GET_NAME(path), std::ios::in | std::ios::binary);
        const std::size_t length = GetStreamLength(file, path);

        if (fileOffsetEx >= length && !(fileOffsetEx == 0 && length == 0))
          throw std::invalid_argument("fileOffsetEx can not be equal or greater than the file length");
        if ((fileOffsetEx + bytesToReadEx) > length)
          throw std::invalid_argument("can not read outside the file");

        rTargetArray.resize(bytesToReadEx);
        // seek to the desired area of the file
        file.seekg(fileOffsetEx, file.beg);
        StreamRead(file, path, rTargetArray.data(), bytesToReadEx);
      }
      catch (const std::ios_base::failure& ex)
      {
        throw IOException(ex.what());
      }
    }


    int64_t File::ReadBytes(void* pDstArray, const int64_t cbDstArray, const int64_t dstStartIndex, const IO::Path& path, const int64_t fileOffset, const int64_t bytesToRead)
    {
      if (pDstArray == nullptr)
        throw std::invalid_argument("pDstArray can not be null");
      if (cbDstArray < 0)
        throw std::invalid_argument("cbDstArray can not be negative");
      if (dstStartIndex < 0)
        throw std::invalid_argument("dstStartIndex can not be negative");
      if (fileOffset < 0)
        throw std::invalid_argument("fileOffset can not be negative");
      if (bytesToRead < 0)
        throw std::invalid_argument("bytesToRead can not be negative");

      if (dstStartIndex > cbDstArray && !(dstStartIndex == 0 && cbDstArray == 0))
        throw std::invalid_argument("dstStartIndex can not greater or equal to cbDstArray");

      if (bytesToRead > (cbDstArray - dstStartIndex) )
        throw std::invalid_argument("the requested number of bytes can not fit in the supplied dstArray at the given location");

      const std::size_t fileOffsetEx = ConvertInt64(fileOffset);
      const std::size_t bytesToReadEx = ConvertInt64(bytesToRead);

      try
      {
        std::ifstream file(PATH_GET_NAME(path), std::ios::in | std::ios::binary);
        const std::size_t length = GetStreamLength(file, path);

        if (fileOffsetEx >= length && !(fileOffsetEx == 0 && length == 0))
          throw std::invalid_argument("fileOffset can not be equal or greater than the file length");
        if ((fileOffsetEx + bytesToReadEx) > length)
          throw std::invalid_argument("can not read outside the file");

        // seek to the desired area of the file
        file.seekg(fileOffsetEx, file.beg);

        StreamRead(file, path, static_cast<uint8_t*>(pDstArray)+dstStartIndex, bytesToReadEx);
        return bytesToRead;
      }
      catch (const std::ios_base::failure& ex)
      {
        throw IOException(ex.what());
      }

    }


    void File::WriteAllText(const IO::Path& path, const std::string& content)
    {
      try
      {
        std::ofstream file(PATH_GET_NAME(path), std::ios::out | std::ios::binary);
        // Write the entire content of the file
        StreamWrite(file, path, content.data(), content.size());
      }
      catch (const std::ios_base::failure& ex)
      {
        throw IOException(ex.what());
      }
    }


    void File::WriteAllBytes(const IO::Path& path, const std::vector<uint8_t>& content)
    {
      try
      {
        std::ofstream file(PATH_GET_NAME(path), std::ios::out | std::ios::binary);
        // Write the entire content of the file
        StreamWrite(file, path, content.data(), content.size());
      }
      catch (const std::ios_base::failure& ex)
      {
        throw IOException(ex.what());
      }
    }


  }
}
