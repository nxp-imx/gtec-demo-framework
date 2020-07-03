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

#include <FslBase/Exceptions.hpp>
#include "Conversion.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

namespace Fsl
{
  namespace SceneFormat
  {
    namespace
    {
      bool SortByOffset(const VertexElementEx& lhs, const VertexElementEx& rhs)
      {
        return lhs.Offset < rhs.Offset;
      }
    }


    SceneFormat::PrimitiveType Conversion::Convert(const Fsl::PrimitiveType value)
    {
      switch (value)
      {
      case Fsl::PrimitiveType::LineList:
        return SceneFormat::PrimitiveType::LineList;
      case Fsl::PrimitiveType::LineStrip:
        return SceneFormat::PrimitiveType::LineStrip;
      case Fsl::PrimitiveType::TriangleList:
        return SceneFormat::PrimitiveType::TriangleList;
      case Fsl::PrimitiveType::TriangleStrip:
        return SceneFormat::PrimitiveType::TriangleStrip;
      default:
        throw NotSupportedException("Unsupported primitive type");
      }
    }


    Fsl::PrimitiveType Conversion::Convert(const SceneFormat::PrimitiveType value)
    {
      switch (value)
      {
      case SceneFormat::PrimitiveType::LineList:
        return Fsl::PrimitiveType::LineList;
      case SceneFormat::PrimitiveType::LineStrip:
        return Fsl::PrimitiveType::LineStrip;
      case SceneFormat::PrimitiveType::TriangleList:
        return Fsl::PrimitiveType::TriangleList;
      case SceneFormat::PrimitiveType::TriangleStrip:
        return Fsl::PrimitiveType::TriangleStrip;
      default:
        throw NotSupportedException("Unsupported primitive type");
      }
    }


    SceneFormat::VertexElementFormat Conversion::Convert(const Fsl::VertexElementFormat value)
    {
      switch (value)
      {
      case Fsl::VertexElementFormat::Single:
        return SceneFormat::VertexElementFormat::Single;
      case Fsl::VertexElementFormat::Vector2:
        return SceneFormat::VertexElementFormat::Vector2;
      case Fsl::VertexElementFormat::Vector3:
        return SceneFormat::VertexElementFormat::Vector3;
      case Fsl::VertexElementFormat::Vector4:
        return SceneFormat::VertexElementFormat::Vector4;
      default:
        throw NotSupportedException("Unsupported vertex element format");
      }
    }


    Fsl::VertexElementFormat Conversion::Convert(const SceneFormat::VertexElementFormat value)
    {
      switch (value)
      {
      case SceneFormat::VertexElementFormat::Single:
        return Fsl::VertexElementFormat::Single;
      case SceneFormat::VertexElementFormat::Vector2:
        return Fsl::VertexElementFormat::Vector2;
      case SceneFormat::VertexElementFormat::Vector3:
        return Fsl::VertexElementFormat::Vector3;
      case SceneFormat::VertexElementFormat::Vector4:
        return Fsl::VertexElementFormat::Vector4;
      default:
        throw NotSupportedException("Unsupported vertex element format");
      }
    }


    SceneFormat::VertexElementUsage Conversion::Convert(const Fsl::VertexElementUsage value)
    {
      switch (value)
      {
      case Fsl::VertexElementUsage::Color:
        return SceneFormat::VertexElementUsage::Color;
      case Fsl::VertexElementUsage::Normal:
        return SceneFormat::VertexElementUsage::Normal;
      case Fsl::VertexElementUsage::Position:
        return SceneFormat::VertexElementUsage::Position;
      case Fsl::VertexElementUsage::TextureCoordinate:
        return SceneFormat::VertexElementUsage::TextureCoordinate;
      case Fsl::VertexElementUsage::Tangent:
        return SceneFormat::VertexElementUsage::Tangent;
      case Fsl::VertexElementUsage::Bitangent:
        return SceneFormat::VertexElementUsage::Bitangent;
      default:
        throw NotSupportedException("Unsupported vertex element usage");
      }
    }


    Fsl::VertexElementUsage Conversion::Convert(const SceneFormat::VertexElementUsage value)
    {
      switch (value)
      {
      case SceneFormat::VertexElementUsage::Color:
        return Fsl::VertexElementUsage::Color;
      case SceneFormat::VertexElementUsage::Normal:
        return Fsl::VertexElementUsage::Normal;
      case SceneFormat::VertexElementUsage::Position:
        return Fsl::VertexElementUsage::Position;
      case SceneFormat::VertexElementUsage::TextureCoordinate:
        return Fsl::VertexElementUsage::TextureCoordinate;
      case SceneFormat::VertexElementUsage::Tangent:
        return Fsl::VertexElementUsage::Tangent;
      case SceneFormat::VertexElementUsage::Bitangent:
        return Fsl::VertexElementUsage::Bitangent;
      default:
        throw NotSupportedException("Unsupported vertex element usage");
      }
    }


    SFVertexElement Conversion::Convert(const VertexElementEx& element)
    {
      if (element.UsageIndex > std::numeric_limits<uint8_t>::max())
      {
        throw NotSupportedException("The usage index is not supported");
      }

      // We dont convert the offset as we assume the converted elements have been sorted and tightly packed
      return {Convert(element.Format), Convert(element.Usage), static_cast<uint8_t>(element.UsageIndex)};
    }


    SFVertexDeclaration Conversion::Convert(const VertexDeclaration& vertexDeclaration)
    {
      const auto count = static_cast<size_t>(vertexDeclaration.Count());

      // Sort the elements based on the offset (low -> high)
      SFVertexDeclaration decl(count);
      std::vector<VertexElementEx> sortedElements(count);
      for (std::size_t i = 0; i < count; ++i)
      {
        sortedElements[i] = vertexDeclaration.At(i);
      }
      std::sort(sortedElements.begin(), sortedElements.end(), SortByOffset);

      // Now convert the sorted elements
      for (std::size_t i = 0; i < count; ++i)
      {
        decl.Elements[i] = Convert(sortedElements[i]);
      }

      // We don't convert the vertex stride as we assume it be tightly packed so there is no need for a stride.
      return decl;
    }
  }
}
