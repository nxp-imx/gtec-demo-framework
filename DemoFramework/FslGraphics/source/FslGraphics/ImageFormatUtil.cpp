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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/ImageFormatUtil.hpp>
#include <cassert>
#include <cctype>

namespace Fsl
{
  ImageFormat ImageFormatUtil::TryDetectImageFormatFromExtension(const IO::Path& path)
  {
    std::string extension = IO::Path::GetExtension(path).ToAsciiString();

    static_assert('a' > 'A', "We expect ASCII");

    // This should be ok as we are working on ASCII chars
    for (auto & rC : extension)
    {
      if (rC >= 'A' && rC <= 'Z')
        rC += ('a' - 'A');
    }

    if(extension == ".jpg" || extension == ".jpeg")
      return ImageFormat::Jpeg;
    else if (extension == ".png")
      return ImageFormat::Png;
    else if (extension == ".dds")
      return ImageFormat::DDS;
    else if (extension == ".bmp")
      return ImageFormat::Bmp;
    else if (extension == ".hdr")
      return ImageFormat::Hdr;
    else if (extension == ".ktx")
      return ImageFormat::KTX;
    else if (extension == ".tga")
      return ImageFormat::Tga;
    return ImageFormat::Undefined;
  }
}
