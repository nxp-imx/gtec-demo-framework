#ifndef FSLBASE_IO_PATHWATCHER_HPP
#define FSLBASE_IO_PATHWATCHER_HPP
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

#include <FslBase/IO/Path.hpp>
#include <memory>
#include <list>

namespace Fsl
{
  namespace IO
  {
    class PathWatcherInternalRecord;

    //! @note Experimental class, might change it is probably also quite slow compared to proper OS change notifications.
    class PathWatcher
    {
    public:
      std::list<std::shared_ptr<PathWatcherInternalRecord>> m_paths;

    public:
      PathWatcher();
      ~PathWatcher();

      //! @brief Add the path to be watched. If the path is already being watched this does nothing.
      //! @note  If the path doesn't exist it will still be watched.
      //! @throws std::invalid_argument if the path isn't absolute.
      //! @throws NotSupportedException if unsupported on this platform
      void Add(const IO::Path& fullPath);

      //! @brief Add the path to be watched. If the path is already being watched this does nothing.
      //! @return true if added, false if path watching isn't supported on this platform or if it was already added
      //! @note  If the path doesn't exist it will still be watched.
      //! @throws std::invalid_argument if the path isn't absolute.
      bool TryAdd(const IO::Path& fullPath);

      //! @brief Remove a path from being watched. If the path is not being watched this does nothing.
      void Remove(const IO::Path& fullPath);

      //! @brief Perform a check
      //! @return true if something was changed.
      bool Check();
    };
  }
}


#endif
