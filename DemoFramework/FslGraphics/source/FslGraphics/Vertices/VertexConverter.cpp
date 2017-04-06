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

#include <FslGraphics/Vertices/VertexConverter.hpp>
#include <FslGraphics/Vertices/VertexElementFormatUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>
#include <cstring>

namespace Fsl
{
  namespace
  {
    int32_t IndexOf(const VertexDeclaration& vertexDeclaration, const VertexElementUsage usage, const std::size_t usageIndex)
    {
      for (int32_t i = 0; i < static_cast<int32_t>(vertexDeclaration.Count()); ++i)
      {
        const VertexElementEx element = vertexDeclaration.At(i);
        if (usage == element.Usage && usageIndex == element.UsageIndex)
          return i;
      }
      return -1;
    }

  }

  void VertexConverter::GenericConvert(void*const pDst, const std::size_t cbDst, const VertexDeclaration& dstVertexDeclaration, const void*const pSrc, const std::size_t cbSrc, const VertexDeclaration& srcVertexDeclaration, const std::size_t srcVertexCount, const void*const pDstDefaultValues, const uint32_t cbDstDefaultValues)
  {
    const uint32_t srcVertexStride = srcVertexDeclaration.VertexStride();
    const uint32_t dstVertexStride = dstVertexDeclaration.VertexStride();

    if (pDst == nullptr || pSrc == nullptr || pDstDefaultValues == nullptr || cbDstDefaultValues < dstVertexStride)
      throw std::invalid_argument("invalid argument");
    // handle copying of zero elements
    if (cbDst == cbSrc && cbSrc == 0)
      return;

    // Verify that we have room for the elements
    const auto cbDstEntries = dstVertexStride * srcVertexCount;
    const auto cbSrcEntries = srcVertexStride * srcVertexCount;
    if (cbSrc < cbSrcEntries)
      throw std::invalid_argument("out of bounds. pSrc does not hold the intended number of elements");
    if (cbDst < cbDstEntries)
      throw std::invalid_argument("out of bounds. pDst can not hold the intended number of elements");


    for (std::size_t i = 0; i < dstVertexDeclaration.Count(); ++i)
    {
      const VertexElementEx dstElement = dstVertexDeclaration.At(i);
      const int32_t srcIndex = IndexOf(srcVertexDeclaration, dstElement.Usage, dstElement.UsageIndex);
      if (srcIndex >= 0)
      {
        const VertexElementEx srcElement = srcVertexDeclaration.At(srcIndex);

        // We found it so now we just need to look at the format
        if (dstElement.Format == srcElement.Format)
        {
          // Straight forward copy
          const auto cbFormatEntry = VertexElementFormatUtil::GetBytesPerElement(dstElement.Format);
          VertexConverter::GenericElementCopy(pDst, cbDst, dstVertexStride, dstElement.Offset, pSrc, cbSrc, srcVertexStride, srcElement.Offset, cbFormatEntry, srcVertexCount);
        }
        else
        {
          // We need to do a format conversion
          throw NotImplementedException("Element format conversion not implemented");
        }
      }
      else
      {
        // Not found, so assign the default value
        const auto cbFormatEntry = VertexElementFormatUtil::GetBytesPerElement(dstElement.Format);
        VertexConverter::GenericElementFill(pDst, cbDst, dstVertexStride, dstElement.Offset, pDstDefaultValues, cbDstDefaultValues, dstElement.Offset, cbFormatEntry, srcVertexCount);
      }
    }
  }


  void VertexConverter::GenericConvert(void*const pDst, const std::size_t cbDst, const VertexDeclaration& dstVertexDeclaration, const void*const pSrc, const std::size_t cbSrc, const VertexDeclaration& srcVertexDeclaration, const std::size_t srcVertexStartIndex, const std::size_t srcVertexCount, const void*const pDstDefaultValues, const uint32_t cbDstDefaultValues)
  {
    const uint32_t srcVertexStride = srcVertexDeclaration.VertexStride();

    const auto srcOffset = srcVertexStartIndex * srcVertexStride;

    GenericConvert(pDst, cbDst, dstVertexDeclaration, reinterpret_cast<const uint8_t*>(pSrc)+srcOffset, cbSrc - srcOffset, srcVertexDeclaration, srcVertexCount, pDstDefaultValues, cbDstDefaultValues);
  }


  void VertexConverter::GenericElementCopy(void*const pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const void*const pSrc, const std::size_t cbSrc, const std::size_t srcStride, const std::size_t srcEntryOffset, const std::size_t cbEntry, const std::size_t numEntries)
  {
    if (pDst == nullptr || dstStride <= 0 || pSrc == nullptr || (dstEntryOffset + cbEntry) > dstStride || (srcEntryOffset + cbEntry) > srcStride)
      throw std::invalid_argument("invalid argument");

    const auto cbDstEntries = dstStride * numEntries;
    if ( cbDstEntries > cbDst )
      throw std::invalid_argument("dst out of bounds");

    const auto cbSrcEntries = srcStride * numEntries;
    if (cbSrcEntries > cbSrc)
      throw std::invalid_argument("src out of bounds");

    const uint8_t* pSrcBuffer = static_cast<const uint8_t*>(pSrc) + srcEntryOffset;
    uint8_t* pDstBuffer = static_cast<uint8_t*>(pDst) + dstEntryOffset;
    const uint8_t*const pDstBufferEnd = pDstBuffer + cbDstEntries;

    while ( pDstBuffer < pDstBufferEnd)
    {
      std::memcpy(pDstBuffer, pSrcBuffer, cbEntry);
      pSrcBuffer += srcStride;
      pDstBuffer += dstStride;
    }
  }


  void VertexConverter::GenericElementFill(void*const pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset, const void*const pSrcValue, const std::size_t cbSrc, const std::size_t srcEntryOffset, const std::size_t cbEntry, const std::size_t numEntries)
  {
    if (pDst == nullptr || dstStride <= 0 || pSrcValue == nullptr || (dstEntryOffset + cbEntry) > dstStride || (srcEntryOffset + cbEntry) > cbSrc)
      throw std::invalid_argument("invalid argument");

    const auto cbDstEntries = dstStride * numEntries;
    if (cbDstEntries > cbDst)
      throw std::invalid_argument("dst out of bounds");

    const uint8_t* pSrcBuffer = static_cast<const uint8_t*>(pSrcValue)+srcEntryOffset;
    uint8_t* pDstBuffer = static_cast<uint8_t*>(pDst)+dstEntryOffset;
    const uint8_t*const pDstBufferEnd = pDstBuffer + cbDstEntries;

    while (pDstBuffer < pDstBufferEnd)
    {
      std::memcpy(pDstBuffer, pSrcBuffer, cbEntry);
      pDstBuffer += dstStride;
    }
  }


}
