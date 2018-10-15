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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/OpenVG/Common.hpp>
#include <FslUtil/OpenVG/VGCheck.hpp>
#include <FslUtil/OpenVG/VGPathBuffer.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace OpenVG
  {
    namespace
    {
      VGPath CreatePath(const std::vector<VGfloat>& vertices, const std::vector<VGubyte>& segments)
      {
        const float bias = 0.0f;
        const float scale = 1.0f;
        VGPath hPath = VG_INVALID_HANDLE;
        try
        {
          hPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, scale, bias, static_cast<VGint>(segments.size()),
                               static_cast<VGint>(vertices.size()), VG_PATH_CAPABILITY_ALL);
          FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
          vgAppendPathData(hPath, static_cast<VGint>(segments.size()), &segments[0], &vertices[0]);
          FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
        }
        catch (const std::exception&)
        {
          if (hPath != VG_INVALID_HANDLE)
          {
            vgDestroyPath(hPath);
          }
          throw;
        }
        return hPath;
      }
    }


    VGPathBuffer::VGPathBuffer()
      : m_path(VG_INVALID_HANDLE)

    {
    }


    VGPathBuffer::VGPathBuffer(const std::vector<VGfloat>& vertices, const std::vector<VGubyte>& segments)
      : VGPathBuffer()
    {
      try
      {
        Reset(vertices, segments);
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }


    VGPathBuffer::VGPathBuffer(const std::vector<Vector2>& vertices, const std::vector<VGubyte>& segments)
      : VGPathBuffer()
    {
      try
      {
        Reset(vertices, segments);
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }


    VGPathBuffer::VGPathBuffer(const std::vector<VGfloat>& vertices, const uint32_t startVertex, const uint32_t vertexCount,
                               const std::vector<VGubyte>& segments)
      : VGPathBuffer()
    {
      try
      {
        Reset(vertices, startVertex, vertexCount, segments);
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }


    VGPathBuffer::VGPathBuffer(const std::vector<Vector2>& vertices, const uint32_t startVertex, const uint32_t vertexCount,
                               const std::vector<VGubyte>& segments)
      : VGPathBuffer()
    {
      try
      {
        Reset(vertices, startVertex, vertexCount, segments);
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }

    VGPathBuffer::~VGPathBuffer()
    {
      Reset();
    }


    void VGPathBuffer::Reset()
    {
      if (m_path != VG_INVALID_HANDLE)
      {
        vgDestroyPath(m_path);
        m_path = VG_INVALID_HANDLE;
      }
    }


    void VGPathBuffer::Reset(const std::vector<VGfloat>& vertices, const std::vector<VGubyte>& segments)
    {
      if (vertices.size() > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("Vertex count overflowed a uint32_t");
      }

      Reset(vertices, 0, static_cast<uint32_t>(vertices.size()), segments);
    }


    void VGPathBuffer::Reset(const std::vector<Vector2>& vertices, const std::vector<VGubyte>& segments)
    {
      if (vertices.size() > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("Vertex count overflowed a uint32_t");
      }

      Reset(vertices, 0, static_cast<uint32_t>(vertices.size()), segments);
    }


    void VGPathBuffer::Reset(const std::vector<VGfloat>& vertices, const uint32_t startVertex, const uint32_t vertexCount,
                             const std::vector<VGubyte>& segments)
    {
      assert((startVertex + vertexCount) <= vertices.size());

      if (m_path != VG_INVALID_HANDLE)
      {
        vgDestroyPath(m_path);
        m_path = VG_INVALID_HANDLE;
      }

      m_vertices.resize(vertexCount);
      std::copy(vertices.begin() + startVertex, vertices.begin() + (startVertex + vertexCount), m_vertices.begin());

      m_segments = segments;

      m_path = CreatePath(m_vertices, m_segments);
    }


    void VGPathBuffer::Reset(const std::vector<Vector2>& vertices, const uint32_t startVertex, const uint32_t vertexCount,
                             const std::vector<VGubyte>& segments)
    {
      assert((startVertex + vertexCount) <= static_cast<int>(vertices.size()));

      if (m_path != VG_INVALID_HANDLE)
      {
        vgDestroyPath(m_path);
        m_path = VG_INVALID_HANDLE;
      }

      m_vertices.resize(vertexCount * 2);
      uint32_t dstIdx = 0;
      for (uint32_t i = 0; i < vertexCount; ++i)
      {
        m_vertices[dstIdx + 0] = vertices[startVertex + i].X;
        m_vertices[dstIdx + 1] = vertices[startVertex + i].Y;
        dstIdx += 2;
      }

      m_segments = segments;

      m_path = CreatePath(m_vertices, m_segments);
    }


    VGPath VGPathBuffer::GetHandle() const
    {
      return m_path;
    }
  }
}
