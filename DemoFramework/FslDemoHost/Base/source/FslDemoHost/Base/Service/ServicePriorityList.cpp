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

#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>

namespace Fsl
{
  Priority ServicePriorityList::BitmapConverterService()
  {
    return Priority::Max() - 10;
  }

  Priority ServicePriorityList::ImageConverterLibraryService()
  {
    return Priority::Max() - 9;
  }

  Priority ServicePriorityList::EGLHostService()
  {
    return Priority::Max() - 40;
  }

  Priority ServicePriorityList::WindowHostService()
  {
    return Priority::Max() - 30;
  }

  Priority ServicePriorityList::VulkanHostService()
  {
    return Priority::Max() - 30;
  }

  Priority ServicePriorityList::NativeGraphicsService()
  {
    return Priority::Max() - 40;
  }

  Priority ServicePriorityList::ImageLibraryService()
  {
    return Priority::Max() - 20;
  }

  Priority ServicePriorityList::ImageBasicService()
  {
    return Priority::Max() - 30;
  }

  Priority ServicePriorityList::AsyncImageService()
  {
    return Priority::Max() - 40;
  }

  Priority ServicePriorityList::ImageService()
  {
    return Priority::Max() - 50;
  }

  Priority ServicePriorityList::EventsService()
  {
    return Priority::Max() - 20;
  }


  Priority ServicePriorityList::NativeWindowEventsService()
  {
    return Priority::Max() - 10;
  }


  Priority ServicePriorityList::TestService()
  {
    return Priority::Min();
  }


  Priority ServicePriorityList::PlatformControlService()
  {
    return Priority::Max() - 10;
  }


  Priority ServicePriorityList::ProfilerService()
  {
    return Priority::Max() - 10;
  }


  Priority ServicePriorityList::ContentMonitor()
  {
    return Priority::Min();
  }

  Priority ServicePriorityList::Options()
  {
    return Priority::Min();
  }

  Priority ServicePriorityList::HostInfo()
  {
    return Priority::Max();
  }
}
