#ifndef FSLGRAPHICS_EXCEPTIONS_HPP
#define FSLGRAPHICS_EXCEPTIONS_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Vertices/VertexElementFormat.hpp>
#include <string>

namespace Fsl
{
  class UnsupportedStrideException : public GraphicsException
  {
    int32_t m_stride;

  public:
    UnsupportedStrideException(const std::string& str, const int32_t stride)
      : GraphicsException(str)
      , m_stride(stride)
    {
    }

    explicit UnsupportedStrideException(const int32_t stride)
      : GraphicsException("Unsupported stride")
      , m_stride(stride)
    {
    }

    int32_t GetStride() const
    {
      return m_stride;
    }
  };

  class UnsupportedStrideExceptionEx : public GraphicsException
  {
    std::size_t m_stride;

  public:
    UnsupportedStrideExceptionEx(const std::string& str, const std::size_t stride)
      : GraphicsException(str)
      , m_stride(stride)
    {
    }

    explicit UnsupportedStrideExceptionEx(const std::size_t stride)
      : GraphicsException("Unsupported stride")
      , m_stride(stride)
    {
    }

    std::size_t GetStride() const
    {
      return m_stride;
    }
  };


  class UnsupportedAlignmentException : public GraphicsException
  {
    int m_alignment;

  public:
    UnsupportedAlignmentException(const std::string& str, const int alignment)
      : GraphicsException(str)
      , m_alignment(alignment)
    {
    }

    explicit UnsupportedAlignmentException(const int alignment)
      : GraphicsException("Unsupported alignment")
      , m_alignment(alignment)
    {
    }

    int GetAlignment() const
    {
      return m_alignment;
    }
  };


  class UnsupportedPixelFormatException : public GraphicsException
  {
    PixelFormat m_pixelFormat;

  public:
    UnsupportedPixelFormatException(const std::string& str, const PixelFormat pixelFormat)
      : GraphicsException(str)
      , m_pixelFormat(pixelFormat)
    {
    }

    explicit UnsupportedPixelFormatException(const PixelFormat pixelFormat)
      : GraphicsException("Unsupported pixel format")
      , m_pixelFormat(pixelFormat)
    {
    }

    PixelFormat GetPixelFormat() const
    {
      return m_pixelFormat;
    }
  };

  class UnsupportedVertexElementFormatException : public GraphicsException
  {
    VertexElementFormat m_vertexElementFormat;

  public:
    UnsupportedVertexElementFormatException(const std::string& str, const VertexElementFormat vertexElementFormat)
      : GraphicsException(str)
      , m_vertexElementFormat(vertexElementFormat)
    {
    }

    explicit UnsupportedVertexElementFormatException(const VertexElementFormat vertexElementFormat)
      : GraphicsException("Unsupported vertex element format")
      , m_vertexElementFormat(vertexElementFormat)
    {
    }

    VertexElementFormat GetVertexElementFormat() const
    {
      return m_vertexElementFormat;
    }
  };


  class UnsupportedPrimitiveTypeException : public GraphicsException
  {
  public:
    explicit UnsupportedPrimitiveTypeException(const std::string& str)
      : GraphicsException(str)
    {
    }
  };


  class UnsupportedPixelFormatConversionException : public ConversionException
  {
    PixelFormat m_fromPixelFormat;
    PixelFormat m_toPixelFormat;

  public:
    UnsupportedPixelFormatConversionException(const std::string& str, const PixelFormat fromPixelFormat, const PixelFormat toPixelFormat)
      : ConversionException(str)
      , m_fromPixelFormat(fromPixelFormat)
      , m_toPixelFormat(toPixelFormat)
    {
    }

    UnsupportedPixelFormatConversionException(const PixelFormat fromPixelFormat, const PixelFormat toPixelFormat)
      : ConversionException("UnsupportedPixelFormatConversionException")
      , m_fromPixelFormat(fromPixelFormat)
      , m_toPixelFormat(toPixelFormat)
    {
    }

    PixelFormat GetFromPixelFormat() const
    {
      return m_fromPixelFormat;
    }

    PixelFormat GetToPixelFormat() const
    {
      return m_toPixelFormat;
    }
  };
}

#endif
