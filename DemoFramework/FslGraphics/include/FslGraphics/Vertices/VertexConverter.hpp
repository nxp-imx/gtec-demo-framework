#ifndef FSLGRAPHICS_VERTICES_VERTEXCONVERTER_HPP
#define FSLGRAPHICS_VERTICES_VERTEXCONVERTER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <cstdlib>

namespace Fsl
{
  class VertexConverter
  {
  public:
    //! @brief Convert from one vertex format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst vertices that should be written to
    //! @param dstVertexCapacity the number of vertices that pDst array can hold.
    //! @param pSrc the src vertices that should be converted. This points to the start of the first element.
    //! @param srcVertexCount the number of source vertices that should be converted.
    //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst
    //! format will be ignored (we utilize the default values of the TDstVertexFormat).
    template <typename TDstVertexFormat, typename TSrcVertexFormat>
    static void Convert(TDstVertexFormat* const pDst, const std::size_t dstVertexCapacity, const TSrcVertexFormat* const pSrc,
                        const std::size_t srcVertexCount)
    {
      Convert(pDst, dstVertexCapacity, pSrc, srcVertexCount, TDstVertexFormat());
    }

    //! @brief Convert from one vertex format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst vertices that should be written to
    //! @param dstVertexCapacity the number of vertices that pDst array can hold.
    //! @param pSrc the src vertices that should be converted. This points to the start of the first element.
    //! @param srcVertexStartIndex the index of the first source vertex that should be converted.
    //! @param srcVertexCount the number of source vertices that should be converted.
    //! @param defaultValue is used to fill in required fields in a dst vertex which are missing from a src vertex.
    //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst
    //! format will be ignored (we utilize the default values of the TDstVertexFormat).
    template <typename TDstVertexFormat, typename TSrcVertexFormat>
    static void Convert(TDstVertexFormat* const pDst, const std::size_t dstVertexCapacity, const TSrcVertexFormat* const pSrc,
                        const std::size_t srcVertexCapacity, const std::size_t srcVertexStartIndex, const std::size_t srcVertexCount)
    {
      Convert(pDst, dstVertexCapacity, pSrc, srcVertexCapacity, srcVertexStartIndex, srcVertexCount, TDstVertexFormat());
    }


    //! @brief Convert from one vertex format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst vertices that should be written to
    //! @param dstVertexCapacity the number of vertices that pDst array can hold.
    //! @param pSrc the src vertices that should be converted. This points to the start of the first element.
    //! @param srcVertexCount the number of source vertices that should be converted.
    //! @param defaultValue is used to fill in required fields in a dst vertex which are missing from a src vertex.
    //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst
    //! format will be ignored.
    template <typename TDstVertexFormat, typename TSrcVertexFormat>
    static void Convert(TDstVertexFormat* const pDst, const std::size_t dstVertexCapacity, const TSrcVertexFormat* const pSrc,
                        const std::size_t srcVertexCount, const TDstVertexFormat& dstDefaultValue)
    {
      const VertexDeclarationSpan dstVertexDeclaration = TDstVertexFormat::AsVertexDeclarationSpan();
      const VertexDeclarationSpan srcVertexDeclaration = TSrcVertexFormat::AsVertexDeclarationSpan();
      GenericConvert(pDst, dstVertexDeclaration.VertexStride() * dstVertexCapacity, dstVertexDeclaration, pSrc,
                     srcVertexDeclaration.VertexStride() * srcVertexCount, srcVertexDeclaration, srcVertexCount, &dstDefaultValue,
                     sizeof(TDstVertexFormat));
    }


    //! @brief Convert from one vertex format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst vertices that should be written to
    //! @param dstVertexCapacity the number of vertices that pDst array can hold.
    //! @param pSrc the src vertices that should be converted. This points to the start of the first element.
    //! @param srcVertexStartIndex the index of the first source vertex that should be converted.
    //! @param srcVertexCount the number of source vertices that should be converted.
    //! @param defaultValue is used to fill in required fields in a dst vertex which are missing from a src vertex.
    //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst
    //! format will be ignored.
    template <typename TDstVertexFormat, typename TSrcVertexFormat>
    static void Convert(TDstVertexFormat* const pDst, const std::size_t dstVertexCapacity, const TSrcVertexFormat* const pSrc,
                        const std::size_t srcVertexCapacity, const std::size_t srcVertexStartIndex, const std::size_t srcVertexCount,
                        const TDstVertexFormat& dstDefaultValue)
    {
      const VertexDeclarationSpan dstVertexDeclaration = TDstVertexFormat::AsVertexDeclarationSpan();
      const VertexDeclarationSpan srcVertexDeclaration = TSrcVertexFormat::AsVertexDeclarationSpan();
      GenericConvert(pDst, dstVertexDeclaration.VertexStride() * dstVertexCapacity, dstVertexDeclaration, pSrc,
                     srcVertexDeclaration.VertexStride() * srcVertexCapacity, srcVertexDeclaration, srcVertexStartIndex, srcVertexCount,
                     &dstDefaultValue, sizeof(TDstVertexFormat));
    }


    //! @brief Convert from one vertex format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst vertices that should be written to (of the format described in dstVertexDeclaration). This points to the start of the
    //! first element.
    //! @param cbDst the number of bytes the pDst array can hold.
    //! @param dstVertexDeclaration the format of the dst vertices.
    //! @param pSrc the src vertices that should be converted. This points to the start of the first element.
    //! @param cbSrc the number of bytes the pSrc array holds.
    //! @param srcVertexCount the number of source vertices that should be converted.
    //! @param pDstDefaultValues points to one dst type vertex that holds the default values which is used to fill in required fields in a dst vertex
    //! which are missing from a src vertex
    //! @param cbDstDefaultValues the number of bytes used for the default vertex.
    //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst
    //! format will be ignored.
    static void GenericConvert(void* const pDst, const std::size_t cbDst, VertexDeclarationSpan dstVertexDeclaration, const void* const pSrc,
                               const std::size_t cbSrc, VertexDeclarationSpan srcVertexDeclaration, const std::size_t srcVertexCount,
                               const void* const pDstDefaultValues, const uint32_t cbDstDefaultValues);

    //! @brief Convert from one vertex format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst vertices that should be written to (of the format described in dstVertexDeclaration). This points to the start of the
    //! first element.
    //! @param cbDst the number of bytes the pDst array can hold.
    //! @param dstVertexDeclaration the format of the dst vertices.
    //! @param pSrc the src vertices that should be converted. This points to the start of the first element.
    //! @param cbSrc the number of bytes the pSrc array holds.
    //! @param srcVertexStartIndex the index of the first source vertex that should be converted.
    //! @param srcVertexCount the number of source vertices that should be converted.
    //! @param pDstDefaultValues points to one dst type vertex that holds the default values which is used to fill in required fields in a dst vertex
    //! which are missing from a src vertex
    //! @param cbDstDefaultValues the number of bytes used for the default vertex.
    //! @note Fields that are present in dst but not in src are filled with the supplied default values. Src fields that isn't present in the dst
    //! format will be ignored.
    static void GenericConvert(void* const pDst, const std::size_t cbDst, VertexDeclarationSpan dstVertexDeclaration, const void* const pSrc,
                               const std::size_t cbSrc, VertexDeclarationSpan srcVertexDeclaration, const std::size_t srcVertexStartIndex,
                               const std::size_t srcVertexCount, const void* const pDstDefaultValues, const uint32_t cbDstDefaultValues);


    static void GenericElementCopy(void* const pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset,
                                   const void* const pSrc, const std::size_t cbSrc, const std::size_t srcStride, const std::size_t srcEntryOffset,
                                   const std::size_t cbEntry, const std::size_t numEntries);
    static void GenericElementFill(void* const pDst, const std::size_t cbDst, const std::size_t dstStride, const std::size_t dstEntryOffset,
                                   const void* const pSrcValue, const std::size_t cbSrc, const std::size_t srcEntryOffset, const std::size_t cbEntry,
                                   const std::size_t numEntries);
  };
}

#endif
