/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include "InputRecordingManager.hpp"
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Persistent/IPersistentDataManager.hpp>
//#include <FslBase/System/HighResolutionTimer.hpp>
#include <Shared/UI/Benchmark/Persistence/Input/AppInputCommandListPersistence.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr auto DefaultInputRecordingFilename = "Default.ncl";
      constexpr auto CustomInputRecordingFilename = "Custom.ncl";
    }

    bool TrySave(const IO::Path& path, const AppInputCommandList& commandList)
    {
      try
      {
        // HighResolutionTimer timer;
        // auto start = timer.GetTime();
        AppInputCommandListPersistence::Save(path, commandList);
        // auto end = timer.GetTime();
        // FSLLOG3_INFO("Time to save: {} microseconds", end - start);
        return true;
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Failed to save: {}", ex.what());
        return false;
      }
    }

    Optional<AppInputCommandList> TryLoadInputCommandList(const ServiceProvider& serviceProvider, const IO::Path& persistentDataFile,
                                                          const bool forceDefaultSequence)
    {
      // Try to load the custom recording first
      Optional<AppInputCommandList> res;
      if (!forceDefaultSequence)
      {
        res = AppInputCommandListPersistence::TryLoad(persistentDataFile);
        if (res.HasValue())
        {
          FSLLOG3_INFO("Custom input recording loaded from '{}'", persistentDataFile);
          return res;
        }
      }

      // fall back to load the default recording
      auto contentManager = serviceProvider.Get<IContentManager>();
      auto contentPath = contentManager->GetContentPath();
      auto defaultRecordingPath = IO::Path::Combine(contentPath, LocalConfig::DefaultInputRecordingFilename);

      res = AppInputCommandListPersistence::TryLoad(defaultRecordingPath);
      FSLLOG3_WARNING_IF(!res.HasValue(), "Default input recording not found at '{}'", defaultRecordingPath);
      FSLLOG3_INFO_IF(res.HasValue(), "Default input recording loaded from '{}'", defaultRecordingPath);
      return res;
    }
  }

  InputRecordingManager::InputRecordingManager(const ServiceProvider& serviceProvider, const bool forceDefaultSequence)
    : m_persistentDataFile(
        IO::Path::Combine(serviceProvider.Get<IPersistentDataManager>()->GetPersistentDataPath(), LocalConfig::CustomInputRecordingFilename))
    , m_commandList(TryLoadInputCommandList(serviceProvider, m_persistentDataFile, forceDefaultSequence))
  {
  }

  InputRecordingManager::~InputRecordingManager() = default;

  Optional<AppInputCommandList> InputRecordingManager::TryGetRecording()
  {
    return m_commandList;
  }

  bool InputRecordingManager::TrySetRecording(const AppInputCommandList& commandList)
  {
    if (!TrySave(m_persistentDataFile, commandList))
    {
      return false;
    }
    m_commandList = commandList;
    return true;
  }

}
