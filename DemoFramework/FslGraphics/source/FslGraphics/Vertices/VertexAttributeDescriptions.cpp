/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptions.hpp>
#include <FslGraphics/Vertices/VertexElementFormatUtil.hpp>
#include <algorithm>
#include <cassert>
#include <limits>
#include <string>

namespace Fsl
{
  namespace
  {
    bool Contains(const std::vector<VertexAttributeDescription>& elements, const VertexElementUsage usage, const uint32_t usageIndex,
                  const uint32_t ignoreIndex) noexcept
    {
      assert(elements.size() <= std::numeric_limits<uint32_t>::max());
      const auto count = static_cast<uint32_t>(elements.size());

      for (uint32_t i = 0; i < count; ++i)
      {
        if (i != ignoreIndex && elements[i].Usage == usage && elements[i].UsageIndex == usageIndex)
        {
          return true;
        }
      }
      return false;
    }

    void VerifyElements(const std::vector<VertexAttributeDescription>& elements)
    {
      // We expect the element offsets to be in order: smallest -> largest
      uint32_t maxLocation = 0;
      const auto count = static_cast<uint32_t>(elements.size());
      for (uint32_t i = 0; i < count; ++i)
      {
        auto location = elements[i].Location;
        if (location < maxLocation)
        {
          throw NotSupportedException("The offsets should be in order: smallest -> largest");
        }

        maxLocation = location;

        if (Contains(elements, elements[i].Usage, elements[i].UsageIndex, i))
        {
          throw NotSupportedException("Duplicated elements: A usage and usageIndex can only be listed once");
        }
      }
    }


    void CopyElements(std::vector<VertexAttributeDescription>& rDstElements, const ReadOnlySpan<VertexAttributeDescription> srcSpan) noexcept
    {
      assert(rDstElements.size() == srcSpan.size());
      assert(srcSpan.size() <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));

      uint32_t maxLocation = 0;
      uint32_t dstIndex = 0;
      for (std::size_t i = 0; i < srcSpan.size(); ++i)
      {
        const auto currentLocation = srcSpan[i].Location;
        if (currentLocation >= maxLocation)
        {
          maxLocation = currentLocation;
          rDstElements[dstIndex] = srcSpan[i];
        }
        else
        {
          // back track and make room until we find a location that is smaller
          assert(dstIndex <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
          auto tmpIndex = static_cast<int32_t>(dstIndex) - 1;
          while (tmpIndex >= 0 && currentLocation < rDstElements[tmpIndex].Location)
          {
            rDstElements[tmpIndex + 1] = rDstElements[tmpIndex];
            --tmpIndex;
          }
          if (tmpIndex < 0)
          {
            tmpIndex = 0;
          }
          rDstElements[tmpIndex] = srcSpan[i];
        }
        ++dstIndex;
      }
    }

    void CopyElements(std::vector<VertexAttributeDescription>& rDstElements,
                      const std::initializer_list<VertexAttributeDescription>& srcList) noexcept
    {
      assert(rDstElements.size() == srcList.size());
      assert(srcList.size() <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));

      uint32_t maxLocation = 0;
      uint32_t dstIndex = 0;
      for (const auto& srcEntry : srcList)
      {
        const auto currentLocation = srcEntry.Location;
        if (currentLocation >= maxLocation)
        {
          maxLocation = currentLocation;
          rDstElements[dstIndex] = srcEntry;
        }
        else
        {
          // back track and make room until we find a location that is smaller
          assert(dstIndex <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
          auto tmpIndex = static_cast<int32_t>(dstIndex) - 1;
          while (tmpIndex >= 0 && currentLocation < rDstElements[tmpIndex].Location)
          {
            rDstElements[tmpIndex + 1] = rDstElements[tmpIndex];
            --tmpIndex;
          }
          if (tmpIndex < 0)
          {
            tmpIndex = 0;
          }
          rDstElements[tmpIndex] = srcEntry;
        }
        ++dstIndex;
      }
    }
  }

  // move assignment operator
  VertexAttributeDescriptions& VertexAttributeDescriptions::operator=(VertexAttributeDescriptions&& other) noexcept
  {
    if (this != &other)
    {
      // Claim ownership here
      m_elements = std::move(other.m_elements);

      // Remove the data from other
    }
    return *this;
  }


  // Transfer ownership from other to this
  VertexAttributeDescriptions::VertexAttributeDescriptions(VertexAttributeDescriptions&& other) noexcept
    : m_elements(std::move(other.m_elements))
  {
    // Remove the data from other
  }


  VertexAttributeDescriptions::VertexAttributeDescriptions() = default;


  VertexAttributeDescriptions::VertexAttributeDescriptions(VertexAttributeDescriptionSpan span)
    : m_elements(span.Count())
  {
    assert(span.Count() <= std::numeric_limits<uint32_t>::max());
    for (std::size_t i = 0; i < span.Count(); ++i)
    {
      m_elements[i] = span[i];
    }
    // We do not need to verify elements as the span has been validated
  }


  VertexAttributeDescriptions::VertexAttributeDescriptions(const ReadOnlySpan<VertexAttributeDescription> span)
    : m_elements(span.size())
  {
    if (span.size() > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("We only support 32bit of elements");
    }

    CopyElements(m_elements, span);
    VerifyElements(m_elements);
  }


  VertexAttributeDescriptions::VertexAttributeDescriptions(const std::initializer_list<VertexAttributeDescription> list)
    : m_elements(list.size())
  {
    if (list.size() > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("We only support 32bit of elements");
    }

    CopyElements(m_elements, list);
    VerifyElements(m_elements);
  }


  void VertexAttributeDescriptions::Reset(VertexAttributeDescriptionSpan span)
  {
    assert(span.Count() <= std::numeric_limits<uint32_t>::max());
    m_elements.resize(span.size());

    for (std::size_t i = 0; i < span.Count(); ++i)
    {
      m_elements[i] = span[i];
    }
    // We do not need to verify elements as the span has been validated
  }


  void VertexAttributeDescriptions::Reset(const ReadOnlySpan<VertexAttributeDescription> span)
  {
    if (span.size() > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("We only support 32bit of elements");
    }

    m_elements.resize(span.size());

    CopyElements(m_elements, span);
    VerifyElements(m_elements);
  }


  uint32_t VertexAttributeDescriptions::Count() const noexcept
  {
    return static_cast<uint32_t>(m_elements.size());
  }


  const VertexAttributeDescription* VertexAttributeDescriptions::DirectAccess() const noexcept
  {
    return m_elements.data();
  }


  int32_t VertexAttributeDescriptions::VertexElementGetIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const
  {
    const int32_t index = VertexElementIndexOf(usage, usageIndex);
    if (index < 0)
    {
      throw NotFoundException("Could not locate a vertex element of the requested type");
    }
    return index;
  }


  int32_t VertexAttributeDescriptions::VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const noexcept
  {
    for (std::size_t i = 0; i < m_elements.size(); ++i)
    {
      if (m_elements[i].Usage == usage && m_elements[i].UsageIndex == usageIndex)
      {
        return static_cast<int32_t>(i);
      }
    }
    return -1;
  }


  VertexAttributeDescription VertexAttributeDescriptions::VertexElementGet(const VertexElementUsage usage, const uint32_t usageIndex) const
  {
    for (auto element : m_elements)
    {
      if (element.Usage == usage && element.UsageIndex == usageIndex)
      {
        return element;
      }
    }
    throw NotFoundException("Could not locate a vertex element of the requested type");
  }


  VertexAttributeDescriptionSpan VertexAttributeDescriptions::AsSpan() const noexcept
  {
    return {ReadOnlySpanUtil::AsSpan(m_elements), OptimizationCheckFlag::NoCheck};
  }


  bool VertexAttributeDescriptions::operator==(const VertexAttributeDescriptions& rhs) const noexcept
  {
    if (m_elements.size() != rhs.m_elements.size())
    {
      return false;
    }

    for (std::size_t i = 0; i < m_elements.size(); ++i)
    {
      if (m_elements[i] != rhs.m_elements[i])
      {
        return false;
      }
    }
    return true;
  }


  bool VertexAttributeDescriptions::operator!=(const VertexAttributeDescriptions& rhs) const noexcept
  {
    return !(*this == rhs);
  }
}
