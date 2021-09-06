/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslNativeWindow/Platform/PlatformNativeWindow.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>

namespace Fsl
{
  namespace
  {
    //! A simple selector that allows slightly too low DPI devices to enter density bucket its technically too small for
    uint32_t SelectDensityBucket(const Vector2& exactDpi)
    {
      const auto dpi =
        static_cast<uint32_t>(std::max(EqualHelper::IsAlmostEqual(exactDpi.X, exactDpi.Y) ? exactDpi.X : ((exactDpi.X + exactDpi.Y) / 2.0f), 1.0f));

      // if (dpi >= 140u)
      //{
      //  return std::max((dpi + 20u) / 80u, 1u) * 80u;
      //}
      const uint32_t initialBucket = (dpi / 16u) * 16;
      if (initialBucket <= 80u)
      {
        return 80u;
      }
      if ((initialBucket % 160u) == 0)
      {
        return initialBucket;
      }
      if (((initialBucket + 16u) % 160u) == 0)
      {
        return initialBucket + 16u;
      }
      if (((initialBucket - 16u) % 160u) == 0)
      {
        return initialBucket - 16u;
      }
      return initialBucket;
    }

    NativeWindowCapabilityFlags PatchFlags(const NativeWindowCapabilityFlags flags, const Optional<Point2U>& forcedActualDpi,
                                           const Optional<uint32_t>& forcedDensityDpi)
    {
      NativeWindowCapabilityFlags finalFlags = flags;
      finalFlags = finalFlags | (forcedActualDpi.HasValue() ? NativeWindowCapabilityFlags::GetDpi : NativeWindowCapabilityFlags::NoFlags);
      finalFlags = finalFlags | (forcedDensityDpi.HasValue() ? NativeWindowCapabilityFlags::GetDensityDpi : NativeWindowCapabilityFlags::NoFlags);
      return finalFlags;
    }
  }

  PlatformNativeWindow::PlatformNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                                             const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams,
                                             const NativeWindowCapabilityFlags capabilityFlags)
    : m_eventQueue(nativeWindowSetup.GetEventQueue())
    , m_forcedActualDpi(nativeWindowSetup.GetConfig().GetForcedActualDpi())
    , m_forcedDensityDpi(nativeWindowSetup.GetConfig().GetForcedDensityDpi())
    , m_capabilityFlags(PatchFlags(capabilityFlags, m_forcedActualDpi, m_forcedDensityDpi))
    , m_platformDisplay(platformWindowParams.PlatformDisplay)
    , m_platformWindow{}
  {
    FSL_PARAM_NOT_USED(pPlatformCustomWindowAllocationParams);
  }


  PlatformNativeWindow::~PlatformNativeWindow() = default;


  NativeWindowMetrics PlatformNativeWindow::GetWindowMetrics() const
  {
    PxExtent2D windowExtent;
    if (!TryGetExtent(windowExtent))
    {
      windowExtent = {};
      FSLLOG3_WARNING("NativeWindow TryGetExtent failed, so using default extent of {}", windowExtent);
    }

    Vector2 exactDpi;
    bool gotExactDPI = NativeWindowCapabilityFlagsUtil::IsFlagged(m_capabilityFlags, NativeWindowCapabilityFlags::GetDpi);
    if (!gotExactDPI || !TryGetDpi(exactDpi))
    {
      FSLLOG3_WARNING_IF(gotExactDPI, "NativeWindow.TryGetDpi failed");
      gotExactDPI = false;
      exactDpi = Vector2(160, 160);
    }

    uint32_t densityDpi{};
    bool gotDensityDPI = NativeWindowCapabilityFlagsUtil::IsFlagged(m_capabilityFlags, NativeWindowCapabilityFlags::GetDensityDpi);
    if (!gotDensityDPI || !TryGetDensityDpi(densityDpi))
    {
      // densityDpi = uint32_t(exactDpi.X);
      densityDpi = SelectDensityBucket(exactDpi);

      // Conditional logging depending on severity
      if (gotDensityDPI)
      {
        FSLLOG3_WARNING("NativeWindow.TryGetDensityDpi failed so using custom density dpi of {} based on physical dpi {} instead", densityDpi,
                        exactDpi);
      }
      else
      {
        FSLLOG3_VERBOSE3_IF(!m_loggedOnceGetWindowMetrics && gotExactDPI,
                            "NativeWindow did not support TryGetDensityDpi so using custom density dpi of {} based on physical dpi {} instead",
                            densityDpi, exactDpi);
        FSLLOG3_VERBOSE3_IF(
          !m_loggedOnceGetWindowMetrics && !gotExactDPI,
          "NativeWindow did not support TryGetDpi and TryGetDensityDpi so using custom density dpi of {} based on physical dpi {} instead",
          densityDpi, exactDpi);
      }
      gotDensityDPI = false;
    }
    else if (!gotExactDPI)
    {
      // got the density DPI, but the actual dpi failed
      exactDpi = Vector2(densityDpi, densityDpi);
      FSLLOG3_VERBOSE3("Using density DPI as dpi");
    }

    m_loggedOnceGetWindowMetrics = true;
    return {windowExtent, exactDpi, densityDpi};
  }


  bool PlatformNativeWindow::TryGetExtent(PxExtent2D& rExtent) const
  {
    PxPoint2 size;
    if (!TryGetNativeSize(size))
    {
      rExtent = {};
      return false;
    }
    rExtent = TypeConverter::UncheckedTo<PxExtent2D>(PxPoint2(std::max(size.X, 0), std::max(size.Y, 0)));
    return true;
  }


  bool PlatformNativeWindow::TryGetDpi(Vector2& rDPI) const
  {
    if (m_forcedActualDpi.HasValue())
    {
      rDPI = Vector2(m_forcedActualDpi->X, m_forcedActualDpi->Y);
      return true;
    }
    return TryGetNativeDpi(rDPI);
  }


  bool PlatformNativeWindow::TryGetDensityDpi(uint32_t& rDensityDpi) const
  {
    if (m_forcedDensityDpi.HasValue())
    {
      rDensityDpi = *m_forcedDensityDpi;
      return true;
    }
    return TryGetNativeDensityDpi(rDensityDpi);
  }


  std::shared_ptr<INativeWindowEventQueue> PlatformNativeWindow::TryGetEventQueue()
  {
    return m_eventQueue.lock();
  }
}    // namespace Fsl
