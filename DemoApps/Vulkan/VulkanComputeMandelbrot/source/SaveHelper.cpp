/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include "SaveHelper.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace
  {
  //  void ImageDataSaveTga(const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer)
  //  {
  //    if (!imageData.get())
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    if (!(imageData->getFormat() == VK_FORMAT_R8_UNORM || imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8_UNORM || imageData->getFormat() == VK_FORMAT_R8G8B8A8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8A8_UNORM))
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    uint8_t bitsPerPixel;

  //    if (imageData->getFormat() == VK_FORMAT_R8_UNORM)
  //    {
  //      bitsPerPixel = 8;
  //    }
  //    else if (imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8_UNORM)
  //    {
  //      bitsPerPixel = 24;
  //    }
  //    else if (imageData->getFormat() == VK_FORMAT_R8G8B8A8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8A8_UNORM)
  //    {
  //      bitsPerPixel = 32;
  //    }
  //    else
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    size_t numberChannels = bitsPerPixel / 8;

  //    VkExtent3D currentExtent;
  //    size_t offset;
  //    if (!imageData->getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer))
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    size_t size = currentExtent.width * currentExtent.height * currentExtent.depth * numberChannels + 18;

  //    // 18 bytes is the size of the header.
  //    IBinaryBufferSP buffer = IBinaryBufferSP(new BinaryBuffer(size));

  //    if (!buffer.get() || buffer->getSize() != size)
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    //

  //    uint8_t header[12];

  //    std::memset(header, 0, 12);

  //    if (bitsPerPixel == 8)
  //    {
  //      header[2] = 3;
  //    }
  //    else
  //    {
  //      header[2] = 2;
  //    }

  //    //

  //    if (buffer->write(header, 1, 12) != 12)
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    uint16_t width = static_cast<uint16_t>(currentExtent.width);
  //    uint16_t height = static_cast<uint16_t>(currentExtent.height);
  //    uint16_t depth = 1;

  //    if (buffer->write(&width, 2, 1) != 1)
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    if (buffer->write(&height, 2, 1) != 1)
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    if (buffer->write(&bitsPerPixel, 1, 1) != 1)
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    header[0] = 0;

  //    if (buffer->write(header, 1, 1) != 1)
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    //

  //    std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[width * height * depth * numberChannels]);

  //    if (!data.get())
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    memcpy(&data[0], &(imageData->getByteData()[offset]), width * height * depth * numberChannels);

  //    if (imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM || imageData->getFormat() == VK_FORMAT_R8G8B8A8_UNORM)
  //    {
  //      imageDataSwapRedBlueChannel(numberChannels, &data[0], width * height * depth);
  //    }

  //    if (buffer->write(&data[0], 1, width * height * depth * numberChannels) != width * height * depth * numberChannels)
  //    {
  //      return IBinaryBufferSP();
  //    }

  //    return buffer;
  //  }
  }


  void SaveHelper::Save(const IO::Path& path, const ImageData& imageData)
  {
    throw NotImplementedException();
    //if (imageData.GetImageType() != VK_IMAGE_TYPE_2D || imageData.GetWidth() < 1 || imageData.GetHeight() < 1 || imageData.GetDepth() != 1 || mipLevel >= imageData.GetMipLevels() || arrayLayer >= imageData.GetArrayLayers())
    //  throw NotSupportedException("imageData not supported");

    //const auto imageFormat = ImageFormatUtil::TryDetectImageFormatFromExtension(path);
    //if (imageFormat == ImageFormat::Invalid)
    //  throw NotSupportedException("Unknown image format");


    //if (imageFormat == ImageFormat::Tga)
    //{
    //  auto buffer = imageDataSaveTga(imageData, mipLevel, arrayLayer);

    //  if (!buffer.get())
    //  {
    //    return VK_FALSE;
    //  }

    //  return fileSaveBinary(filename, buffer);
    //}
    //else if (imageFormat == ImageFormat::Hdr)
    //{
    //  auto buffer = imageDataSaveHdr(imageData, mipLevel, arrayLayer);

    //  if (!buffer.get())
    //  {
    //    return VK_FALSE;
    //  }

    //  return fileSaveBinary(filename, buffer);
    //}
    //else if (lowerCaseExtension == ".data")
    //{
    //  VkExtent3D currentExtent;
    //  size_t offset;

    //  if (!imageData->getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer))
    //  {
    //    return VK_FALSE;
    //  }

    //  size_t currentSize = (size_t)currentExtent.width * (size_t)currentExtent.height * (size_t)imageData->getNumberChannels() * (size_t)imageData->getBytesPerChannel();

    //  return fileSaveBinaryData(filename, &imageData->getByteData()[offset], currentSize);
    //}
  }
}
