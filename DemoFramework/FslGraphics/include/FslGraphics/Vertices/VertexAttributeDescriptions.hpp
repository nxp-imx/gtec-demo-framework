#ifndef FSLGRAPHICS_VERTICES_VERTEXATTRIBUTEDESCRIPTIONS_HPP
#define FSLGRAPHICS_VERTICES_VERTEXATTRIBUTEDESCRIPTIONS_HPP
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

#include <FslGraphics/Vertices/VertexAttributeDescription.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptionSpan.hpp>
#include <initializer_list>
#include <vector>

namespace Fsl
{
  class VertexAttributeDescriptions
  {
    std::vector<VertexAttributeDescription> m_elements;

  public:
    // move assignment operator
    VertexAttributeDescriptions& operator=(VertexAttributeDescriptions&& other) noexcept;
    // move constructor
    VertexAttributeDescriptions(VertexAttributeDescriptions&& other) noexcept;
    // Request that the compiler generates a copy constructor and assignment operator
    VertexAttributeDescriptions(const VertexAttributeDescriptions&) = default;
    VertexAttributeDescriptions& operator=(const VertexAttributeDescriptions&) = default;

    VertexAttributeDescriptions();

    //! @brief Create a vertex declaration based on the given elements.
    explicit VertexAttributeDescriptions(VertexAttributeDescriptionSpan span);

    explicit VertexAttributeDescriptions(const ReadOnlySpan<VertexAttributeDescription> span);
    VertexAttributeDescriptions(const std::initializer_list<VertexAttributeDescription> list);


    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    void Reset(VertexAttributeDescriptionSpan span);
    void Reset(const ReadOnlySpan<VertexAttributeDescription> span);

    //! @brief Get the number of elements
    uint32_t Count() const noexcept;

    VertexAttributeDescription At(const std::size_t index) const
    {
      return m_elements[index];
    }

    //! @brief Get direct access to the elements
    const VertexAttributeDescription* DirectAccess() const noexcept;

    //! @brief Get the element index of for the given usage and usageIndex (if not found a NotFoundException is thrown)
    int32_t VertexElementGetIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const;

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    int32_t VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const noexcept;

    //! @brief Get the element for the given usage and usageIndex (if not found a NotFoundException is thrown)
    VertexAttributeDescription VertexElementGet(const VertexElementUsage usage, const uint32_t usageIndex) const;

    VertexAttributeDescriptionSpan AsSpan() const noexcept;

    bool operator==(const VertexAttributeDescriptions& rhs) const noexcept;
    bool operator!=(const VertexAttributeDescriptions& rhs) const noexcept;
  };
}

#endif
