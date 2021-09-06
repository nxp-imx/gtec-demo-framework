#ifndef FSLBASE_IO_DIRECTORY_HPP
#define FSLBASE_IO_DIRECTORY_HPP
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
#include <FslBase/IO/Path.hpp>
#include <FslBase/IO/SearchOptions.hpp>

namespace Fsl
{
  namespace IO
  {
    class PathDeque;

    class Directory
    {
    public:
      Directory(const Directory&) = delete;
      Directory& operator=(const Directory&) = delete;

      //! @brief Create the given directory (renamed form CreateDirectory to prevent issues with windows.h>
      //! @note Any and all directories specified in path are created, unless they already exist or unless some part of path is invalid
      static void CreateDir(const Path& path);

      //! @brief Check if a directory exists
      static bool Exists(const Path& path);

      //! @brief Get the current working directory
      //! @note Could not be named getCurrentDirectory because it clashes with a windows.h define.
      static Path GetCurrentWorkingDirectory();

      //! @brief Get the files under the path directory
      static void GetFiles(PathDeque& rResult, const Path& path, const SearchOptions searchOptions);

      static bool TryGetFiles(PathDeque& rResult, const Path& path, const SearchOptions searchOptions);
    };
  }
}

#endif
