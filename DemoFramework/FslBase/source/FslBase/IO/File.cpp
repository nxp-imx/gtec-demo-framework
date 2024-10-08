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
#include <FslBase/IO/File.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/System/Platform/PlatformFileSystem.hpp>
#include <FslBase/System/Platform/PlatformPathTransform.hpp>
#include <fmt/format.h>
#include <cassert>
#include <fstream>
#include <limits>
#include <utility>
#include <vector>

namespace Fsl::IO
{
  namespace
  {
    std::size_t Convert(const std::streamoff value)
    {
      return static_cast<std::size_t>(value);
    }

    std::size_t Convert(const uint64_t value)
    {
      return static_cast<std::size_t>(value);
    }


    std::size_t GetStreamLength(std::ifstream& rStream, const Path& path)
    {
      if (!rStream.good())
      {
        throw IOException(fmt::format("File not found '{}'", path));
      }

      // Dumb C++ way of getting the stream length
      rStream.seekg(0, std::ifstream::end);
      const std::streamoff streamLength = rStream.tellg();
      rStream.seekg(0, std::ifstream::beg);
      assert(streamLength >= 0);
      return Convert(streamLength);
    }

    void StreamRead(std::ifstream& rStream, const Path& path, void* pDst, const std::size_t length)
    {
      // Read the entire content of the file
      if (length > 0)
      {
        if (!rStream.read(static_cast<char*>(pDst), NumericCast<std::streamsize>(length)))
        {
          throw IOException(fmt::format("Failed to read entire file '{}'", path));
        }
      }
    }


    void StreamWrite(std::ofstream& rStream, const Path& path, const void* pDst, const std::size_t length)
    {
      // Read the entire content of the file
      if (length > 0)
      {
        if (!rStream.write(static_cast<const char*>(pDst), NumericCast<std::streamsize>(length)))
        {
          throw IOException(fmt::format("Failed to write entire file '{}'", path));
        }
      }
    }
  }


  std::string File::ReadAllText(const Path& path)
  {
    try
    {
      std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::in | std::ios::binary);
      const std::size_t length = GetStreamLength(file, path);

      // Read the entire content of the file
      std::vector<char> content(length + 1);
      StreamRead(file, path, content.data(), length);
      content[length] = 0;

      // Convert it to a string
      return {content.data()};
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
      std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::in | std::ios::binary);
      if (!file.good())
      {
        return false;
      }

      const std::size_t length = GetStreamLength(file, path);

      // Read the entire content of the file
      std::vector<char> content(length + 1);
      StreamRead(file, path, content.data(), length);
      content[length] = 0;

      // Convert it to a string
      rDst.assign(content.data());
      return true;
    }
    catch (const std::ios_base::failure&)
    {
      return false;
    }
  }

  // std::optional<std::string> File::TryReadAllText(const Path& path)
  //{
  //  std::string res;
  //  return TryReadAllText(res, path) ? std::optional<std::string>(std::move(res)) : std::optional<std::string>();
  //}


  bool File::TryReadAllBytes(std::vector<uint8_t>& rDst, const Path& path)
  {
    try
    {
      std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::in | std::ios::binary);
      if (!file.good())
      {
        return false;
      }

      const std::size_t length = GetStreamLength(file, path);

      // Read the entire content of the file
      rDst.resize(length);
      StreamRead(file, path, rDst.data(), length);
      return true;
    }
    catch (const std::ios_base::failure&)
    {
      return false;
    }
  }

  bool File::Exists(const Path& path)
  {
    FileAttributes attr;
    if (!TryGetAttributes(path, attr))
    {
      return false;
    }
    return (attr.HasFlag(FileAttributes::File));
  }


  FileAttributes File::GetAttributes(const Path& path)
  {
    FileAttributes attributes;
    if (!TryGetAttributes(path, attributes))
    {
      throw NotFoundException("Path not found");
    }
    return attributes;
  }


  bool File::TryGetAttributes(const Path& path, FileAttributes& rAttributes)
  {
    return PlatformFileSystem::TryGetAttributes(path, rAttributes);
  }


  uint64_t File::GetLength(const Path& path)
  {
    std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::ate | std::ios::binary);
    if (!file.good())
    {
      throw IOException(fmt::format("Failed to open file '{0}'", path));
    }
    return file.tellg();
  }


  std::vector<uint8_t> File::ReadAllBytes(const Path& path)
  {
    std::vector<uint8_t> content;
    ReadAllBytes(content, path);
    return content;
  }


  void File::ReadAllBytes(std::vector<uint8_t>& rTargetArray, const Path& path)
  {
    try
    {
      std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::in | std::ios::binary);
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


  uint64_t File::ReadAllBytes(void* pDstArray, const uint64_t cbDstArray, const Path& path)
  {
    if (pDstArray == nullptr)
    {
      throw std::invalid_argument("pDstArray can not be null");
    }


    const std::size_t cbDstArrayEx = Convert(cbDstArray);

    try
    {
      std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::in | std::ios::binary);
      const std::size_t length = GetStreamLength(file, path);
      if (length > cbDstArrayEx)
      {
        throw IOException(fmt::format("Supplied array too small to hold '{0}'", path));
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


  void File::ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& path, const uint64_t fileOffset, const uint64_t bytesToRead)
  {
    const std::size_t fileOffsetEx = Convert(fileOffset);
    const std::size_t bytesToReadEx = Convert(bytesToRead);

    try
    {
      std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::in | std::ios::binary);
      const std::size_t length = GetStreamLength(file, path);

      if (fileOffsetEx >= length && (fileOffsetEx != 0 || length != 0))
      {
        throw std::invalid_argument("fileOffsetEx can not be equal or greater than the file length");
      }
      if ((fileOffsetEx + bytesToReadEx) > length)
      {
        throw std::invalid_argument("can not read outside the file");
      }

      rTargetArray.resize(bytesToReadEx);
      // seek to the desired area of the file
      file.seekg(NumericCast<std::ifstream::off_type>(fileOffsetEx), std::ifstream::beg);
      StreamRead(file, path, rTargetArray.data(), bytesToReadEx);
    }
    catch (const std::ios_base::failure& ex)
    {
      throw IOException(ex.what());
    }
  }


  uint64_t File::ReadBytes(void* pDstArray, const uint64_t cbDstArray, const uint64_t dstStartIndex, const IO::Path& path, const uint64_t fileOffset,
                           const uint64_t bytesToRead)
  {
    if (pDstArray == nullptr)
    {
      throw std::invalid_argument("pDstArray can not be null");
    }

    if (dstStartIndex > cbDstArray && (dstStartIndex != 0 || cbDstArray != 0))
    {
      throw std::invalid_argument("dstStartIndex can not greater or equal to cbDstArray");
    }

    if (bytesToRead > (cbDstArray - dstStartIndex))
    {
      throw std::invalid_argument("the requested number of bytes can not fit in the supplied dstArray at the given location");
    }

    const std::size_t fileOffsetEx = Convert(fileOffset);
    const std::size_t bytesToReadEx = Convert(bytesToRead);

    try
    {
      std::ifstream file(PlatformPathTransform::ToSystemPath(path), std::ios::in | std::ios::binary);
      const std::size_t length = GetStreamLength(file, path);

      if (fileOffsetEx >= length && (fileOffsetEx != 0 || length != 0))
      {
        throw std::invalid_argument("fileOffset can not be equal or greater than the file length");
      }
      if ((fileOffsetEx + bytesToReadEx) > length)
      {
        throw std::invalid_argument("can not read outside the file");
      }

      // seek to the desired area of the file
      file.seekg(NumericCast<std::ifstream::off_type>(fileOffsetEx), std::ifstream::beg);

      StreamRead(file, path, static_cast<uint8_t*>(pDstArray) + dstStartIndex, bytesToReadEx);
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
      std::ofstream file(PlatformPathTransform::ToSystemPath(path), std::ios::out | std::ios::binary | std::ios::trunc);
      if (!file.is_open())
      {
        throw IOException(fmt::format("Failed to open file '{}'", path));
      }
      // Write the entire content of the file
      StreamWrite(file, path, content.data(), content.size());
    }
    catch (const std::ios_base::failure& ex)
    {
      throw IOException(ex.what());
    }
  }


  void File::WriteAllBytes(const IO::Path& path, const void* const pContent, const std::size_t contentSizeInBytes)
  {
    try
    {
      std::ofstream file(PlatformPathTransform::ToSystemPath(path), std::ios::out | std::ios::binary | std::ios::trunc);
      if (!file.is_open())
      {
        throw IOException(fmt::format("Failed to open file '{}'", path));
      }

      // Write the entire content of the file
      StreamWrite(file, path, pContent, contentSizeInBytes);
    }
    catch (const std::ios_base::failure& ex)
    {
      throw IOException(ex.what());
    }
  }

}
