#ifndef FSLBASE_IO_FILE_HPP
#define FSLBASE_IO_FILE_HPP
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
#include <FslBase/Noncopyable.hpp>
#include <FslBase/IO/FileAttributes.hpp>
#include <FslBase/IO/Path.hpp>
#include <string>
#include <vector>


namespace Fsl
{
  namespace IO
  {
    class File : private Noncopyable
    {
    public:
      //! @brief Check if a file exists
      static bool Exists(const Path& path);

      //! @brief get the attributes associated with the file or directory
      //! @throws a NotFoundException if the path can't be located
      static FileAttributes GetAttributes(const Path& path);

      //! @brief get the attributes associated with the file or directory
      //! @return true if the attributes could be found, false if the path couldn't be located
      static bool TryGetAttributes(const Path& path, FileAttributes& rAttributes);

      //! @brief Get the current length of a file.
      //! @throws IOException if the file isn't found.
      static uint64_t GetLength(const Path& path);

      //! @brief Read the entire content of the given file into a string.
      //! @throws IOException if the file isn't found or something goes wrong reading it.
      static std::string ReadAllText(const Path& path);

      //! @brief Read the entire content of the given file into a string.
      //! @throws IOException if the file isn't found or something goes wrong reading it.
      static bool TryReadAllText(std::string& rDst, const Path& path);

      //! @brief Read the entire content of the given file into a binary array.
      //! @param rTargetArray the array to load the content into. The array will be resized to fit the file content
      //! @param path to the file that should be loaded
      //! @throws IOException if the file isn't found or something goes wrong reading it.
      static void ReadAllBytes(std::vector<uint8_t>& rTargetArray, const Path& path);

      //! @brief Read the entire content of the given file into a binary array.
      //! @param pDstArray the array to load the content into (if == nullptr a exception will be thrown)
      //! @param cbDstArray the size of pDstArray in bytes (if too small to fit the fill a exception will be thrown).
      //! @param path to the file that should be loaded
      //! @return the number of bytes that was read.
      //! @throws IOException if the file isn't found or something goes wrong reading it.
      static uint64_t ReadAllBytes(void* pDstArray, const uint64_t cbDstArray, const Path& path);

      //! @brief Read the entire content of the given file into a binary array.
      //! @param path to the file that should be loaded
      //! @throws IOException if the file isn't found or something goes wrong reading it.
      static std::vector<uint8_t> ReadBytes(const Path& path);

      //! @brief Read the requested content of the given file into a binary array.
      //! @param rTargetArray the array to load the content into. The array will be resized to fit the file content
      //! @param path the path to load the content from
      //! @param fileOffset the offset from the beginning of the file where the read should occur (if less than zero or greater than the length of the
      //! file this throws a exception)
      //! @param bytesToRead the number of bytes to read (if the end of file is encountered the read operation will fail with a exception)
      //! @throws IOException if the file isn't found or something goes wrong reading it.
      static void ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& path, const uint64_t fileOffset, const uint64_t bytesToRead);

      //! @brief Read the entire content of the given file into a binary array.
      //! @param pDstArray the array to load the content into (if == nullptr a exception will be thrown)
      //! @param cbDstArray the size of pDstArray in bytes (if too small to fit the fill a exception will be thrown).
      //! @param dstStartIndex the index inside pDstArray the write should start  (if < 0 or >= cbDstArray a exception will be thrown).
      //! @param path the path to load the content from
      //! @param fileOffset the offset from the beginning of the file where the read should occur (if less than zero or greater than the length of the
      //! file this throws a exception)
      //! @param bytesToRead the number of bytes to read (if the end of file is encountered the read operation will fail with a exception)
      //! @return the number of bytes that was read.
      //! @throws IOException if the file isn't found or something goes wrong reading it.
      static uint64_t ReadBytes(void* pDstArray, const uint64_t cbDstArray, const uint64_t dstStartIndex, const IO::Path& path,
                                const uint64_t fileOffset, const uint64_t bytesToRead);

      //! @brief Write the entire content of the string to the given file.
      //! @note If the target file exists its overwritten
      static void WriteAllText(const IO::Path& path, const std::string& content);

      //! @brief Write the entire content of the array to the given file.
      //! @note If the target file exists its overwritten
      static void WriteAllBytes(const IO::Path& path, const std::vector<uint8_t>& content)
      {
        WriteAllBytes(path, content.data(), content.size());
      }

      static void WriteAllBytes(const IO::Path& path, const void* const pContent, const std::size_t contentSizeInBytes);
    };
  }
}

#endif
