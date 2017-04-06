#ifndef FSLGRAPHICS_VERTICES_VERTEXDECLARATION_HPP
#define FSLGRAPHICS_VERTICES_VERTEXDECLARATION_HPP
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

#include <FslGraphics/Vertices/VertexElementEx.hpp>
#include <vector>

namespace Fsl
{
  class VertexDeclaration
  {
    std::vector<VertexElementEx> m_elements;
    uint32_t m_vertexStride;
  public:
    // move assignment operator
    VertexDeclaration& operator=(VertexDeclaration&& other);
    // move constructor
    VertexDeclaration(VertexDeclaration&& other);
    // Request that the compiler generates a copy constructor and assignment operator
    VertexDeclaration(const VertexDeclaration &) = default;
    VertexDeclaration& operator= (const VertexDeclaration&) = default;


    VertexDeclaration();

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    VertexDeclaration(const VertexElementEx*const pElements, const std::size_t elementCount, const uint32_t vertexStride);

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    void Reset(const VertexElementEx*const pElements, const std::size_t elementCount, const uint32_t vertexStride);

    //! @brief Get the vertex stride
    uint32_t VertexStride() const
    {
      return m_vertexStride;
    }

    //! @brief Get the number of elements
    uint32_t Count() const;

    VertexElementEx At(const int32_t index) const
    {
      return m_elements[index];
    }

    VertexElementEx At(const std::size_t index) const
    {
      return m_elements[index];
    }

    //! @brief Get direct access to the elements
    const VertexElementEx* DirectAccess() const;

    //! @brief Get the element index of for the given usage and usageIndex (if not found a NotFoundException is thrown)
    int32_t VertexElementGetIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const;

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    int32_t VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const;

    //! @brief Get the element for the given usage and usageIndex (if not found a NotFoundException is thrown)
    VertexElementEx VertexElementGet(const VertexElementUsage usage, const uint32_t usageIndex) const;

    bool operator==(const VertexDeclaration& rhs) const;
    bool operator!=(const VertexDeclaration& rhs) const;
  };
}

#endif
