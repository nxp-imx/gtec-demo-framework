#ifndef FSLBASE_SYSTEM_PLATFORM_PLATFORMFILESYSTEM_HPP
#define FSLBASE_SYSTEM_PLATFORM_PLATFORMFILESYSTEM_HPP
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
#include <FslBase/IO/SearchOptions.hpp>
#include <memory>

namespace Fsl
{
  namespace IO
  {
    class PathDeque;
    class PlatformPathMonitorToken;

    //! @note Be very careful with what is used here as its the bottom layer.
    class PlatformFileSystem : private Noncopyable
    {
    public:
      //! @brief get the attributes associated with the file or directory
      //! @return true if the attributes could be found, false if the path couldn't be located
      static bool TryGetAttributes(const Path& path, FileAttributes& rAttributes);

      //! @brief Create a platform specific token that can be used to monitor a path for changes
      //! @return return the platform specific token or null if not supported
      //! @note Experimental interface, might change.
      static std::shared_ptr<PlatformPathMonitorToken> CreatePathMonitorToken(const Path& path);

      //! @brief Check if the token was changed since the last time
      //! @return true if changed, false if not
      //! @note Experimental interface, might change.
      static bool CheckPathForChanges(const std::shared_ptr<PlatformPathMonitorToken>& token);


      //! @brief Get the files under the path directory
      static void GetFiles(PathDeque& rResult, const Path& path, const SearchOptions searchOptions);

      //! @brief Create the directory (is not recursive, so its allowed to fail if the parent directory doesn't exist)
      //! @note  However it's perfectly valid for the directory to already exist, this should be silently ignored.
      //!        Called CreateDir instead of CreateDirectory to prevent windows header conflicts
      static void CreateDir(const Path& path);
    };
  }
}

#endif
