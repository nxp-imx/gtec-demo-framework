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

#include <FslUtil/OpenGLES2/GLTexture.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslGraphics/Bitmap/RawCubeBitmap.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/CubeMapFace.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <FslUtil/OpenGLES2/GLRawBitmapUtil.hpp>

#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace GLES2
  {
    namespace
    {
      struct FaceToTargetRecord
      {
        uint32_t Face;
        GLenum Target;

        FaceToTargetRecord()
          : Face(0)
          , Target(0)
        {
        }

        FaceToTargetRecord(const uint32_t& face, const GLenum target)
          : Face(face)
          , Target(target)
        {
        }
      };


      FaceToTargetRecord g_normalFaceTargetMapping[1] =
      {
        FaceToTargetRecord(0, GL_TEXTURE_2D),
      };


      FaceToTargetRecord g_cubeFaceTargetMapping[6] =
      {
        FaceToTargetRecord(CubeMapFace::PosX, GL_TEXTURE_CUBE_MAP_POSITIVE_X),
        FaceToTargetRecord(CubeMapFace::NegX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X),
        FaceToTargetRecord(CubeMapFace::PosY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y),
        FaceToTargetRecord(CubeMapFace::NegY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y),
        FaceToTargetRecord(CubeMapFace::PosZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z),
        FaceToTargetRecord(CubeMapFace::NegZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z),
      };
    }


    // move assignment operator
    GLTexture& GLTexture::operator=(GLTexture&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_handle = other.m_handle;
        m_target = other.m_target;
        m_extent = other.m_extent;

        // Remove the data from other
        other.m_handle = GLValues::INVALID_HANDLE;
        other.m_target = GL_TEXTURE_2D;
        other.m_extent = Extent3D();
      }
      return *this;
    }


    // Transfer ownership from other to this
    GLTexture::GLTexture(GLTexture&& other)
      : m_handle(other.m_handle)
      , m_target(other.m_target)
      , m_extent(other.m_extent)
    {
      // Remove the data from other
      other.m_handle = GLValues::INVALID_HANDLE;
      other.m_target = GL_TEXTURE_2D;
      other.m_extent = Extent3D();
    }


    GLTexture::GLTexture()
      : m_handle(GLValues::INVALID_HANDLE)
      , m_target(GL_TEXTURE_2D)
      , m_extent()
    {
    }


    GLTexture::GLTexture(const GLuint handle, const Extent3D& extent, const GLenum target)
      : m_handle(handle)
      , m_target(target)
      , m_extent(extent)
    {
      if (target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP)
        throw std::invalid_argument("Unsupported texture target type");
    }


    GLTexture::GLTexture(const Bitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
      : GLTexture()
    {
      Reset(bitmap, textureParameters, textureFlags);
    }


    GLTexture::GLTexture(const RawBitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
      : GLTexture()
    {
      Reset(bitmap, textureParameters, textureFlags);
    }


    GLTexture::GLTexture(const Bitmap& bitmapPosX, const Bitmap& bitmapNegX,
                         const Bitmap& bitmapPosY, const Bitmap& bitmapNegY,
                         const Bitmap& bitmapPosZ, const Bitmap& bitmapNegZ,
                         const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
      : GLTexture()
    {
      Reset(bitmapPosX, bitmapNegX, bitmapPosY, bitmapNegY, bitmapPosZ, bitmapNegZ, textureParameters, textureFlags);
    }


    GLTexture::GLTexture(const RawBitmap& bitmapPosX, const RawBitmap& bitmapNegX,
                         const RawBitmap& bitmapPosY, const RawBitmap& bitmapNegY,
                         const RawBitmap& bitmapPosZ, const RawBitmap& bitmapNegZ,
                         const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
      : GLTexture()
    {
      Reset(RawCubeBitmap(bitmapPosX, bitmapNegX, bitmapPosY, bitmapNegY, bitmapPosZ, bitmapNegZ), textureParameters, textureFlags);
    }


    GLTexture::GLTexture(const RawCubeBitmap& cubeBitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
      : GLTexture()
    {
      Reset(cubeBitmap, textureParameters, textureFlags);
    }


    GLTexture::GLTexture(const Texture& texture, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
      : GLTexture()
    {
      Reset(texture, textureParameters, textureFlags);
    }


    GLTexture::GLTexture(const RawTexture& texture, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
      : GLTexture()
    {
      Reset(texture, textureParameters, textureFlags);
    }


    GLTexture::~GLTexture()
    {
      Reset();
    }


    void GLTexture::Reset()
    {
      if (m_handle != GLValues::INVALID_HANDLE)
      {
        glDeleteTextures(1, &m_handle);
        m_handle = GLValues::INVALID_HANDLE;
        m_target = GL_TEXTURE_2D;
        m_extent = Extent3D();
      }
    }


    void GLTexture::Reset(const GLuint handle, const Extent3D& extent, const GLenum target)
    {
      if (target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP)
        throw std::invalid_argument("Unsupported texture target type");

      Reset();
      m_handle = handle;
      m_target = target;
      m_extent = extent;
    }


    void GLTexture::Reset(const Bitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      Reset();
      SetData(bitmap, textureParameters, textureFlags);
    }


    void GLTexture::Reset(const RawBitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      Reset();
      SetData(bitmap, textureParameters, textureFlags);
    }


    void GLTexture::Reset(const Bitmap& bitmapPosX, const Bitmap& bitmapNegX,
                          const Bitmap& bitmapPosY, const Bitmap& bitmapNegY,
                          const Bitmap& bitmapPosZ, const Bitmap& bitmapNegZ,
                          const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      Reset();
      SetData(bitmapPosX, bitmapNegX, bitmapPosY, bitmapNegY, bitmapPosZ, bitmapNegZ, textureParameters, textureFlags);
    }


    void GLTexture::Reset(const RawBitmap& bitmapPosX, const RawBitmap& bitmapNegX,
                          const RawBitmap& bitmapPosY, const RawBitmap& bitmapNegY,
                          const RawBitmap& bitmapPosZ, const RawBitmap& bitmapNegZ,
                          const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      Reset();
      SetData(RawCubeBitmap(bitmapPosX, bitmapNegX, bitmapPosY, bitmapNegY, bitmapPosZ, bitmapNegZ), textureParameters, textureFlags);
    }


    void GLTexture::Reset(const RawCubeBitmap& cubeBitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      Reset();
      SetData(cubeBitmap, textureParameters, textureFlags);
    }


    void GLTexture::Reset(const Texture& texture, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      if (IsValid())
        Reset();

      SetData(texture, textureParameters, textureFlags);
    }


    void GLTexture::Reset(const RawTexture& texture, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      if (IsValid())
        Reset();

      SetData(texture, textureParameters, textureFlags);
    }


    void GLTexture::SetData(const Bitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      RawBitmap rawBitmap;
      Bitmap::ScopedDirectAccess scopedAccess(bitmap, rawBitmap);
      SetData(rawBitmap, textureParameters, textureFlags);
    }


    void GLTexture::SetData(const RawBitmap& bitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      if (!bitmap.IsValid())
        throw std::invalid_argument("The bitmaps must be valid");
      if (!textureParameters.IsValid())
        throw std::invalid_argument("The texture parameters are invalid");

      if (bitmap.GetOrigin() != BitmapOrigin::LowerLeft && !textureFlags.IsEnabled(TextureFlags::AllowAnyBitmapOrigin))
      {
        FSLLOG_WARNING("The supplied texture is not using LowerLeft corner as it's origin as OpenGLES expects, causing a software flip before upload (performance loss)");
        Bitmap tmpBitmap(bitmap, BitmapOrigin::LowerLeft);
        assert(tmpBitmap.GetOrigin() == BitmapOrigin::LowerLeft);
        RawBitmap tmpRawBitmap;
        Bitmap::ScopedDirectAccess directAccess(tmpBitmap, tmpRawBitmap);
        SetData(tmpRawBitmap, textureParameters, textureFlags);
        return;
      }

      const GLRawBitmapUtil::Result result = GLRawBitmapUtil::Convert(bitmap, textureFlags.IsEnabled(TextureFlags::ExactFormat));

      if (m_target != GL_TEXTURE_2D)
      {
        Reset();
      }

      if (m_handle == GLValues::INVALID_HANDLE)
      {
        GL_CHECK(glGenTextures(1, &m_handle));
      }

      GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_handle));
      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, result.Alignment));

      GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, result.InternalFormat, bitmap.Width(), bitmap.Height(), 0, result.Format, result.Type, bitmap.Content()));

      if (textureFlags.IsEnabled(TextureFlags::GenerateMipMaps))
      {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
      }

      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureParameters.MinFilter));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureParameters.MagFilter));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureParameters.WrapS));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureParameters.WrapT));

      m_extent = Extent3D(bitmap.GetExtent(), 1);
      m_target = GL_TEXTURE_2D;
    }


    void GLTexture::SetData(const Bitmap& bitmapPosX, const Bitmap& bitmapNegX,
                            const Bitmap& bitmapPosY, const Bitmap& bitmapNegY,
                            const Bitmap& bitmapPosZ, const Bitmap& bitmapNegZ,
                            const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      RawBitmap rawBitmapPosX, rawBitmapNegX, rawBitmapPosY, rawBitmapNegY, rawBitmapPosZ, rawBitmapNegZ;
      Bitmap::ScopedDirectAccess scopedAccessXP(bitmapPosX, rawBitmapPosX);
      Bitmap::ScopedDirectAccess scopedAccessXN(bitmapNegX, rawBitmapNegX);
      Bitmap::ScopedDirectAccess scopedAccessYP(bitmapPosY, rawBitmapPosY);
      Bitmap::ScopedDirectAccess scopedAccessYN(bitmapNegY, rawBitmapNegY);
      Bitmap::ScopedDirectAccess scopedAccessZP(bitmapPosZ, rawBitmapPosZ);
      Bitmap::ScopedDirectAccess scopedAccessZN(bitmapNegZ, rawBitmapNegZ);
      SetData(RawCubeBitmap(rawBitmapPosX, rawBitmapNegX, rawBitmapPosY, rawBitmapNegY, rawBitmapPosZ, rawBitmapNegZ), textureParameters, textureFlags);
    }


    void GLTexture::SetData(const RawBitmap& bitmapPosX, const RawBitmap& bitmapNegX,
                            const RawBitmap& bitmapPosY, const RawBitmap& bitmapNegY,
                            const RawBitmap& bitmapPosZ, const RawBitmap& bitmapNegZ,
                            const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      SetData(RawCubeBitmap(bitmapPosX, bitmapNegX, bitmapPosY, bitmapNegY, bitmapPosZ, bitmapNegZ), textureParameters, textureFlags);
    }


    void GLTexture::SetData(const RawCubeBitmap& cubeBitmap, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      if (!cubeBitmap.IsValid())
        throw std::invalid_argument("The cube bitmap must be valid");

      if (!textureParameters.IsValid())
        throw std::invalid_argument("The texture parameters are invalid");

      if (cubeBitmap.GetOrigin() != BitmapOrigin::LowerLeft && !textureFlags.IsEnabled(TextureFlags::AllowAnyBitmapOrigin))
      {
        FSLLOG_WARNING("The supplied textures is not using LowerLeft corner as it's origin as OpenGLES expects, causing a software flip before upload (performance loss)");
        Bitmap tmpPosX(cubeBitmap.GetPosX(), BitmapOrigin::LowerLeft);
        Bitmap tmpNegX(cubeBitmap.GetNegX(), BitmapOrigin::LowerLeft);
        Bitmap tmpPosY(cubeBitmap.GetPosY(), BitmapOrigin::LowerLeft);
        Bitmap tmpNegY(cubeBitmap.GetNegY(), BitmapOrigin::LowerLeft);
        Bitmap tmpPosZ(cubeBitmap.GetPosZ(), BitmapOrigin::LowerLeft);
        Bitmap tmpNegZ(cubeBitmap.GetNegZ(), BitmapOrigin::LowerLeft);
        assert(tmpPosX.GetOrigin() == BitmapOrigin::LowerLeft);

        RawBitmap tmpRawPosX, tmpRawNegX;
        RawBitmap tmpRawPosY, tmpRawNegY;
        RawBitmap tmpRawPosZ, tmpRawNegZ;

        Bitmap::ScopedDirectAccess directAccessPosX(tmpPosX, tmpRawPosX);
        Bitmap::ScopedDirectAccess directAccessNegX(tmpNegX, tmpRawNegX);
        Bitmap::ScopedDirectAccess directAccessPosY(tmpPosY, tmpRawPosY);
        Bitmap::ScopedDirectAccess directAccessNegY(tmpNegY, tmpRawNegY);
        Bitmap::ScopedDirectAccess directAccessPosZ(tmpPosZ, tmpRawPosZ);
        Bitmap::ScopedDirectAccess directAccessNegZ(tmpNegZ, tmpRawNegZ);
        // We switch the PosY and NegY since we are doing a flip
        SetData(tmpRawPosX, tmpRawNegX, tmpRawNegY, tmpRawPosY, tmpRawPosZ, tmpRawNegZ, textureParameters, textureFlags);
        return;
      }

      const GLRawBitmapUtil::Result resultPosX = GLRawBitmapUtil::Convert(cubeBitmap.GetPosX(), textureFlags.IsEnabled(TextureFlags::ExactFormat));
      const GLRawBitmapUtil::Result resultNegX = GLRawBitmapUtil::Convert(cubeBitmap.GetNegX(), textureFlags.IsEnabled(TextureFlags::ExactFormat));
      const GLRawBitmapUtil::Result resultPosY = GLRawBitmapUtil::Convert(cubeBitmap.GetPosY(), textureFlags.IsEnabled(TextureFlags::ExactFormat));
      const GLRawBitmapUtil::Result resultNegY = GLRawBitmapUtil::Convert(cubeBitmap.GetNegY(), textureFlags.IsEnabled(TextureFlags::ExactFormat));
      const GLRawBitmapUtil::Result resultPosZ = GLRawBitmapUtil::Convert(cubeBitmap.GetPosZ(), textureFlags.IsEnabled(TextureFlags::ExactFormat));
      const GLRawBitmapUtil::Result resultNegZ = GLRawBitmapUtil::Convert(cubeBitmap.GetNegZ(), textureFlags.IsEnabled(TextureFlags::ExactFormat));


      const GLint texFormat = resultPosX.Format;
      const GLint texFormatInternal = resultPosX.InternalFormat;

      if (m_target != GL_TEXTURE_CUBE_MAP)
      {
        Reset();
      }

      if (m_handle == GLValues::INVALID_HANDLE)
      {
        GL_CHECK(glGenTextures(1, &m_handle));
      }

      const int texWidth = cubeBitmap.Width();
      const int texHeight = cubeBitmap.Height();

      GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle));
      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, resultPosX.Alignment));
      GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, texFormatInternal, texWidth, texHeight, 0, texFormat, resultPosX.Type, cubeBitmap.GetPosX().Content()));
      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, resultNegX.Alignment));
      GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, texFormatInternal, texWidth, texHeight, 0, texFormat, resultNegX.Type, cubeBitmap.GetNegX().Content()));

      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, resultPosY.Alignment));
      GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, texFormatInternal, texWidth, texHeight, 0, texFormat, resultPosY.Type, cubeBitmap.GetPosY().Content()));
      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, resultNegY.Alignment));
      GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, texFormatInternal, texWidth, texHeight, 0, texFormat, resultNegY.Type, cubeBitmap.GetNegY().Content()));

      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, resultPosZ.Alignment));
      GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, texFormatInternal, texWidth, texHeight, 0, texFormat, resultPosZ.Type, cubeBitmap.GetPosZ().Content()));
      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, resultNegZ.Alignment));
      GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, texFormatInternal, texWidth, texHeight, 0, texFormat, resultNegZ.Type, cubeBitmap.GetNegZ().Content()));

      if (textureFlags.IsEnabled(TextureFlags::GenerateMipMaps))
      {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
      }

      GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, textureParameters.MinFilter));
      GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, textureParameters.MagFilter));
      GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, textureParameters.WrapS));
      GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, textureParameters.WrapT));

      m_extent = Extent3D(texWidth, texHeight, 1);
      m_target = GL_TEXTURE_CUBE_MAP;
    }


    void GLTexture::SetData(const Texture& texture, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      if (!texture.IsValid())
        throw std::invalid_argument("The texture must be valid");

      RawTexture rawTexture;
      Texture::ScopedDirectAccess directAccessSrc(texture, rawTexture);
      return SetData(rawTexture, textureParameters, textureFlags);
    }


    void GLTexture::SetData(const RawTexture& texture, const GLTextureParameters& textureParameters, const TextureFlags& textureFlags)
    {
      if (!texture.IsValid())
        throw std::invalid_argument("The texture must be valid");

      const auto texPixelFormat = texture.GetPixelFormat();

      if (PixelFormatUtil::IsCompressed(texPixelFormat))
        throw NotSupportedException("Compressed texture formats not supported");

      if (!textureParameters.IsValid())
        throw std::invalid_argument("The texture parameters are invalid");


      FSLLOG_DEBUG_WARNING_IF(texture.GetBitmapOrigin() != BitmapOrigin::LowerLeft && !textureFlags.IsEnabled(TextureFlags::AllowAnyBitmapOrigin), "The supplied textures is not using LowerLeft corner as it's origin as OpenGLES expects");

      const FaceToTargetRecord* pFaceTargetMapping;
      GLenum target;
      switch (texture.GetTextureType())
      {
      case TextureType::Tex1D:
      case TextureType::Tex2D:
        target = GL_TEXTURE_2D;
        pFaceTargetMapping = g_normalFaceTargetMapping;
        static_assert(sizeof(g_normalFaceTargetMapping) / sizeof(FaceToTargetRecord), "we expect 1 face for normal textures");
        if (texture.GetFaces() != 1)
          throw NotSupportedException("We expected one face for normal textures");
        break;
      case TextureType::TexCube:
        target = GL_TEXTURE_CUBE_MAP;
        pFaceTargetMapping = g_cubeFaceTargetMapping;
        static_assert(sizeof(g_cubeFaceTargetMapping) / sizeof(FaceToTargetRecord), "we expect 6 faces for cube textures");
        if (texture.GetFaces() != 6)
          throw NotSupportedException("We expected six face for cube textures");
        break;
      default:
        throw NotSupportedException("TextureType is not supported");
      }

      if (m_target != target)
      {
        Reset();
      }

      try
      {
        if (m_handle == GLValues::INVALID_HANDLE)
        {
          GL_CHECK(glGenTextures(1, &m_handle));
        }

        const auto textureInfo = texture.GetTextureInfo();
        const uint8_t* pContent = static_cast<const uint8_t*>(texture.GetContent());
        const auto srcPixelFormat = texture.GetPixelFormat();
        const uint32_t srcBytesPerPixel = PixelFormatUtil::GetBytesPerPixel(srcPixelFormat);

        GL_CHECK(glBindTexture(target, m_handle));

        for (uint32_t face = 0; face < textureInfo.Faces; ++face)
        {
          for (uint32_t level = 0; level < textureInfo.Levels; ++level)
          {
            const auto extent = texture.GetExtent(level);
            const auto srcStride = PixelFormatLayoutUtil::CalcMinimumStride(extent.Width, srcBytesPerPixel);

            const auto rawBlob = texture.GetTextureBlob(level, face);

            const GLRawBitmapUtil::Result result = GLRawBitmapUtil::Convert(srcPixelFormat, extent.Width, srcStride, textureFlags.IsEnabled(TextureFlags::ExactFormat));

            // Verify our nasty little assumption
            assert(pFaceTargetMapping[face].Face == face);

            GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, result.Alignment));
            GL_CHECK(glTexImage2D(pFaceTargetMapping[face].Target, level, result.InternalFormat, extent.Width, extent.Height, 0, result.Format, result.Type, pContent + rawBlob.Offset));
          }
        }

        if (textureFlags.IsEnabled(TextureFlags::GenerateMipMaps) || texture.GetLevels() == 1)
        {
          GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
        }

        GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, textureParameters.MinFilter));
        GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, textureParameters.MagFilter));
        GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, textureParameters.WrapS));
        GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, textureParameters.WrapT));

        m_extent = texture.GetExtent();
        m_target = target;
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }


    Point2 GLTexture::GetSize() const
    {
      FSLLOG_DEBUG_WARNING_IF(m_extent.Depth != 1, "GetSize called on a 3D texture, use GetExtent")
      return Point2(m_extent.Width, m_extent.Height);
    }


    NativeTextureArea GLTexture::CalcTextureArea(const TextureRectangle& textureRectangle, const int timesX, const int timesY)
    {
      const Rectangle srcRect = textureRectangle.GetRectangle();
      const Point2 sizeTex = textureRectangle.GetTextureSize();

      assert(timesX == 1 || (timesX != 1 && (srcRect.X() == 0 || srcRect.Width() == sizeTex.X)));
      assert(timesY == 1 || (timesY != 1 && (srcRect.Y() == 0 || srcRect.Height() == sizeTex.Y)));

      assert(static_cast<float>(sizeTex.X) >= 0.0f);
      assert(static_cast<float>(sizeTex.Y) >= 0.0f);
      return NativeTextureArea(srcRect.Left() == 0 ? 0.0f : srcRect.Left() / static_cast<float>(sizeTex.X),
                               1.0f - (srcRect.Top() == 0 ? 0.0f : srcRect.Top() / static_cast<float>(sizeTex.Y)),
                               srcRect.Right() == sizeTex.X ? 1.0f * timesX : srcRect.Right() / static_cast<float>(sizeTex.X),
                               1.0f - (srcRect.Bottom() == sizeTex.Y ? 1.0f * timesY : srcRect.Bottom() / static_cast<float>(sizeTex.Y)));
    }
  }

}
