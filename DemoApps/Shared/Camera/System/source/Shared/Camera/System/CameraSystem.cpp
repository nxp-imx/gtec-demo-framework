/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <Shared/Camera/System/CameraSystem.hpp>
#include <Shared/Camera/Adapter/Interface/ICameraSystemAdapter.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtExtent2D.hpp>
#include <FslBase/String/ToString.hpp>
#include <algorithm>
#include <cassert>
#include <memory>
#include <unordered_set>

namespace Fsl
{
  namespace Helios
  {
    namespace
    {
      struct CameraTypeHash
      {
        std::size_t operator()(const CameraType& val) const noexcept
        {
          return static_cast<std::size_t>(val);
        }
      };


      std::shared_ptr<ICameraSystemAdapter> GetDefaultCameraFactory(const std::vector<std::shared_ptr<ICameraSystemAdapter>>& cameraAdapterSystems)
      {
        if (cameraAdapterSystems.empty())
        {
          throw std::invalid_argument("No cameras available");
        }

        // for now just return the first camera
        return cameraAdapterSystems.front();
      }

      Camera CreateNow(const std::shared_ptr<ICameraSystemAdapter>& nativeCameraFactory, const CameraType cameraType,
                       const CameraAdapterAllocateInfo& allocateInfo)
      {
        assert(nativeCameraFactory);

        auto nativeCamera = nativeCameraFactory->Allocate(allocateInfo);
        if (!nativeCamera)
        {
          throw std::runtime_error("Native camera factory incorrectly returned null");
        }

        const auto nativeConfig = nativeCamera->GetConfig();

        if (allocateInfo.Flags.IsEnabled(CameraAdapterAllocateFlags::CustomExtent) && nativeConfig.Extent != allocateInfo.Extent)
        {
          FSLLOG3_WARNING("CameraAdapter extent different than requested: {} vs {}", nativeConfig.Extent, allocateInfo.Extent);
        }

        CameraConfig finalCameraConfig(cameraType, nativeConfig.Extent, nativeConfig.ActivePixelFormat, nativeConfig.Stride);
        return Camera(finalCameraConfig, nativeCamera);
      }

      void ValidateAdapterSystems(const std::vector<std::shared_ptr<ICameraSystemAdapter>>& cameraAdapterSystems)
      {
        std::unordered_set<CameraType, CameraTypeHash> uniqueTypes(cameraAdapterSystems.size());
        for (auto& entry : cameraAdapterSystems)
        {
          if (!entry)
          {
            throw std::invalid_argument("cameraAdapterSystems contained a null factory");
          }
          const auto cameraType = entry->GetCameraType();
          if (uniqueTypes.find(entry->GetCameraType()) != uniqueTypes.end())
          {
            throw std::invalid_argument(std::string("cameraAdapterSystems factory type already registered") + ToString(static_cast<int>(cameraType)));
          }
          uniqueTypes.insert(cameraType);
        }
      }
    }

    CameraSystem::CameraSystem(const std::vector<std::shared_ptr<ICameraSystemAdapter>>& cameraAdapterSystems)
      : m_cameraAdapterSystems(cameraAdapterSystems)
    {
      ValidateAdapterSystems(cameraAdapterSystems);
    }


    CameraSystem::~CameraSystem() = default;


    Camera CameraSystem::Create()
    {
      auto defaultCameraFactory = GetDefaultCameraFactory(m_cameraAdapterSystems);
      const auto cameraType = defaultCameraFactory->GetCameraType();

      CameraAdapterAllocateInfo allocateInfo;
      return CreateNow(defaultCameraFactory, cameraType, allocateInfo);
    }


    Camera CameraSystem::Create(const Extent2D& cameraExtent)
    {
      auto defaultCameraFactory = GetDefaultCameraFactory(m_cameraAdapterSystems);
      const auto cameraType = defaultCameraFactory->GetCameraType();

      CameraAdapterAllocateInfo allocateInfo(CameraAdapterAllocateFlags::CustomExtent, cameraExtent);
      return CreateNow(defaultCameraFactory, cameraType, allocateInfo);
    }


    Camera CameraSystem::Create(const CameraType cameraType, const Extent2D& cameraExtent)
    {
      auto itrFind = std::find_if(m_cameraAdapterSystems.begin(), m_cameraAdapterSystems.end(),
                                  [cameraType](const std::shared_ptr<ICameraSystemAdapter>& val) { return (val->GetCameraType() == cameraType); });
      if (itrFind == m_cameraAdapterSystems.end())
      {
        throw NotFoundException("Camera type not found");
      }

      CameraAdapterAllocateInfo allocateInfo(CameraAdapterAllocateFlags::CustomExtent, cameraExtent);
      return CreateNow(*itrFind, cameraType, allocateInfo);
    }
  }
}
