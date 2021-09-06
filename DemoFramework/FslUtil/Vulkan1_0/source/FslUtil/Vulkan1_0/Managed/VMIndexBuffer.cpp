/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace Vulkan
  {
    void VMIndexBuffer::Reset(const std::shared_ptr<VMBufferManager>& bufferManager, ReadOnlyFlexSpan indexSpan, const VMBufferUsage usage)
    {
      if (!bufferManager)
      {
        throw std::invalid_argument("bufferManager can not be null");
      }
      if (indexSpan.size() > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("element counts larger than a uint32_t is not supported");
      }
      if (indexSpan.stride() != 2 && indexSpan.stride() != 4)
      {
        throw NotSupportedException("elementStride must be greater than zero");
      }

      Reset();

      try
      {
        m_indexBuffer = bufferManager->CreateBuffer(indexSpan, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, usage);
        m_bufferManager = bufferManager;
        m_indexCount = UncheckedNumericCast<uint32_t>(indexSpan.size());
        m_elementStride = UncheckedNumericCast<uint32_t>(indexSpan.stride());
        m_usage = usage;
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }

    void VMIndexBuffer::Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity, const uint32_t elementStride)
    {
      if (!bufferManager)
      {
        throw std::invalid_argument("bufferManager can not be null");
      }
      if (elementCapacity > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("element counts larger than a uint32_t is not supported");
      }

      Reset();

      try
      {
        m_indexBuffer = bufferManager->CreateDynamicBuffer(elementCapacity, elementStride, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        m_bufferManager = bufferManager;
        m_indexCount = UncheckedNumericCast<uint32_t>(elementCapacity);
        m_elementStride = elementStride;
        m_usage = VMBufferUsage::DYNAMIC;
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }

    void VMIndexBuffer::SetData(ReadOnlyFlexSpan indexSpan)
    {
      if (indexSpan.size() > m_indexCount)
      {
        throw std::invalid_argument("out of bounds");
      }
      if (indexSpan.stride() != m_elementStride)
      {
        throw std::invalid_argument("elementStride must match the VMIndexBuffer element stride");
      }

      m_indexBuffer.Upload(0u, indexSpan.data(), indexSpan.byte_size());
    }
  }
}
