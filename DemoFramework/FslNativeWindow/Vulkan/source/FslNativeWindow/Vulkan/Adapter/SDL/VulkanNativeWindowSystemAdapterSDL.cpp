#ifdef FSL_WINDOWSYSTEM_SDL2
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include "VulkanNativeWindowSystemAdapterSDL.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslNativeWindow/Platform/Adapter/SDL/PlatformNativeWindowAdapterSDL.hpp>
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <cstring>
#include "../VulkanNativeWindowAdapter.hpp"

namespace Fsl
{
  namespace
  {
    PlatformNativeWindowParams Patch(const PlatformNativeWindowParams& initialParams)
    {
      PlatformNativeWindowParams params(initialParams);
      params.WindowFlags |= SDL_WINDOW_VULKAN;
      return params;
    }

    std::vector<const char*> GetInstanceExtensionsSDL(SDL_Window* pWindow)
    {
      unsigned int count = 0;
      if (SDL_Vulkan_GetInstanceExtensions(pWindow, &count, nullptr) == SDL_FALSE)
      {
        throw InitFailedException("SDL_Vulkan_GetInstanceExtensions failed to acquire count");
      }

      std::vector<const char*> result(count);
      if (SDL_Vulkan_GetInstanceExtensions(pWindow, &count, result.data()) == SDL_FALSE)
      {
        throw InitFailedException("SDL_Vulkan_GetInstanceExtensions failed extension names");
      }

      {    // Filter extension VK_KHR_SURFACE_EXTENSION_NAME as we add that elsewhere
        auto itr = result.begin();
        while (itr != result.end())
        {
          if (std::strcmp(VK_KHR_SURFACE_EXTENSION_NAME, *itr) == 0u)
          {
            itr = result.erase(itr);
          }
          else
          {
            ++itr;
          }
        }
      }
      return result;
    }

    std::string GetSDLSurfaceExtension()
    {
      // Since 'SDL_Vulkan_GetInstanceExtensions' for no good reason requires a window.
      // So we create a dummy window.
      SDL_Window* pWindow = SDL_CreateWindow("dummy", 0, 0, 100, 100, SDL_WINDOW_VULKAN);
      if (pWindow == nullptr)
      {
        throw InitFailedException("Failed to create dummy window");
      }
      try
      {
        std::vector<const char*> requiredExtensions = GetInstanceExtensionsSDL(pWindow);
        if (requiredExtensions.size() != 1u)
        {
          // The interface for the platform only supports returning once instance extension name
          throw InitFailedException(fmt::format("Expected GetInstanceExtensionsSDL to return one extension not {}", requiredExtensions.size()));
        }

        if (pWindow != nullptr)
        {
          SDL_DestroyWindow(pWindow);
          pWindow = nullptr;
        }
        return requiredExtensions[0];
      }
      catch (const std::exception& ex)
      {
        if (pWindow != nullptr)
        {
          SDL_DestroyWindow(pWindow);
        }
        throw;
      }
    }


    class VulkanNativeWindowAdapterSDL final : public VulkanNativeWindowAdapter<PlatformNativeWindowAdapterSDL>
    {
      // SDL_GLContext m_context{nullptr};

    public:
      VulkanNativeWindowAdapterSDL(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                   const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
        : VulkanNativeWindowAdapter<PlatformNativeWindowAdapterSDL>(nativeWindowSetup, Patch(windowParams), pPlatformCustomWindowAllocationParams)
      {
        if (SDL_Vulkan_CreateSurface(GetSdlWindow(), m_setup.Instance, &m_surface) == 0)
        {
          throw RapidVulkan::VulkanException("SDL_Vulkan_CreateSurface failed");
        }
      }

      ~VulkanNativeWindowAdapterSDL() final = default;

      PlatformNativeWindowType GetWindowType() const final
      {
        return GetPlatformWindow();
      }
    };


    std::shared_ptr<IPlatformNativeWindowAdapter>
      AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<VulkanNativeWindowAdapterSDL>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }    // namespace


  VulkanNativeWindowSystemAdapterSDL::VulkanNativeWindowSystemAdapterSDL(const NativeWindowSystemSetup& setup)
    : VulkanNativeWindowSystemAdapterTemplate<PlatformNativeWindowSystemAdapterSDL>(setup, GetSDLSurfaceExtension(), AllocateWindow)
  {
  }
}
#endif
