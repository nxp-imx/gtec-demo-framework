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

#include "DeviceBuffer.hpp"
#include <FslBase/Exceptions.hpp>
#include <utility>

namespace Fsl
{
  // move assignment operator
  DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& other)
  {
    if (this != &other)
    {
      m_buffer = std::move(other.m_buffer);
      m_memory = std::move(other.m_memory);
      //m_imageFormat = other.m_imageFormat;

      // Remove the data from other
      //other.m_imageFormat = VK_FORMAT_UNDEFINED;
    }
    return *this;
  }


  // Transfer ownership from other to this
  DeviceBuffer::DeviceBuffer(DeviceBuffer&& other)
    : m_buffer(std::move(other.m_buffer))
    , m_memory(std::move(other.m_memory))
    //, m_imageFormat(other.m_imageFormat)
  {
    // Remove the data from other
    //other.m_imageFormat = VK_FORMAT_UNDEFINED;
  }


  DeviceBuffer::DeviceBuffer()
    : m_buffer()
    , m_memory()
    //, m_imageFormat(VK_FORMAT_UNDEFINED)
  {
  }


  DeviceBuffer::DeviceBuffer(Vulkan::BufferEx&& buffer, Vulkan::DeviceMemoryEx&& deviceMemory) //, const VkFormat imageFormat)
    : m_buffer(std::move(buffer))
    , m_memory(std::move(deviceMemory))
    //, m_imageFormat(imageFormat)
  {
  }

  void DeviceBuffer::Reset()
  {
    m_buffer.Reset();
    m_memory.Reset();
    //m_imageFormat = VK_FORMAT_UNDEFINED;
  }


  void DeviceBuffer::Reset(Vulkan::BufferEx&& buffer, Vulkan::DeviceMemoryEx&& deviceMemory) //, const VkFormat imageFormat)
  {
    m_buffer = std::move(buffer);
    m_memory = std::move(deviceMemory);
    //m_imageFormat = imageFormat;
  }



}
