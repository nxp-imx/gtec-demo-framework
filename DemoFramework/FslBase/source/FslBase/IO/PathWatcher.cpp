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

#include <FslBase/Log/Log.hpp>
#include <FslBase/IO/PathWatcher.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Directory.hpp>
#include <FslBase/System/Platform/PlatformFileSystem.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <list>
#include <utility>

namespace Fsl
{
  namespace IO
  {
    class PathWatcherInternalRecord
    {
    public:
      IO::Path FullPath;
      std::shared_ptr<PlatformPathMonitorToken> Token;

      PathWatcherInternalRecord(const Path& fullPath, std::shared_ptr<PlatformPathMonitorToken> token)
        : Token(std::move(token))
      {
      }

      bool CheckForChanges()
      {
        if (!Token)
        {
          throw NotSupportedException("");
        }
        return PlatformFileSystem::CheckPathForChanges(Token);
      }
    };

    namespace
    {
      struct FullPathComp
      {
        const Path& FullPath;
        explicit FullPathComp(const Path& fullPath)
          : FullPath(fullPath)
        {
        }

        inline bool operator()(const std::shared_ptr<PathWatcherInternalRecord>& entry) const
        {
          return entry->FullPath == FullPath;
        }
      };
    }


    PathWatcher::PathWatcher() = default;
    PathWatcher::~PathWatcher() = default;


    void PathWatcher::Add(const IO::Path& fullPath)
    {
      if (!TryAdd(fullPath))
      {
        throw NotSupportedException("PathWatcher not supported");
      }
    }


    bool PathWatcher::TryAdd(const IO::Path& fullPath)
    {
      if (!Path::IsPathRooted(fullPath))
      {
        throw std::invalid_argument("Path is not rooted");
      }

      if (std::find_if(m_paths.begin(), m_paths.end(), FullPathComp(fullPath)) != m_paths.end())
      {
        return false;
      }

      std::shared_ptr<PlatformPathMonitorToken> token = PlatformFileSystem::CreatePathMonitorToken(fullPath);
      if (!token)
      {
        return false;
      }

      const auto record = std::make_shared<PathWatcherInternalRecord>(fullPath, token);
      m_paths.push_back(record);
      return true;
    }


    void PathWatcher::Remove(const IO::Path& fullPath)
    {
      m_paths.remove_if(FullPathComp(fullPath));
    }


    bool PathWatcher::Check()
    {
      for (auto itr = m_paths.begin(); itr != m_paths.end(); ++itr)
      {
        if ((*itr)->CheckForChanges())
        {
          return true;
        }
      }
      return false;
    }
  }
}
