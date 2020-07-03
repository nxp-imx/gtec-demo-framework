/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include "DFNativeBatchCamera.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/Camera/Platform/PlatformCameraSystem.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    //! @brief Capture a frame into the native texture, this method is slow and requires multiple copies.
    //!        The copies will most likely occur
    //!        - in the camera adapter to the 'rScratchpadBitmap'
    //!        - from the rScratchpadBitmap to the rDstTexture
    bool TryCaptureFrame(Helios::Camera& rCamera, GLES3::GLTexture& rDstTexture, Bitmap& rScratchpadBitmap, uint32_t& rCameraFrameId)
    {
      bool hasNewFrame = false;
      {    // Capture a frame into a bitmap
        RawBitmapEx rawBitmap;
        Bitmap::ScopedDirectAccess directAccess(rScratchpadBitmap, rawBitmap);
        hasNewFrame = rCamera.TryRender(rawBitmap, rCameraFrameId);
      }

      if (hasNewFrame)
      {    // Update the native texture (slow)
        GLTextureParameters textureParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        rDstTexture.SetData(rScratchpadBitmap, textureParameters);
      }
      return hasNewFrame;
    }
  }


  DFNativeBatchCameraRender::DFNativeBatchCameraRender()
    : m_cameraSystem(Helios::PlatformCameraSystem::CreateCameraSystem())
    , m_camera(m_cameraSystem.Create())
    , m_cameraFrameBitmap(m_camera.GetExtent(), m_camera.GetPixelFormat(), m_camera.GetStride(), BitmapOrigin::LowerLeft)
    , m_cameraFrameId(0)
  {
    // Set the initial image
    GLTextureParameters textureParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    m_nativeTexture.SetData(m_cameraFrameBitmap, textureParameters);

    FSLLOG3_WARNING("FIX: Some of the camera adapters do not obey our origin request so the image is upside down");
  }


  void DFNativeBatchCameraRender::Draw(const DemoTime& /*demoTime*/, const std::shared_ptr<GLES3::NativeBatch2D>& nativeBatch, const PxSize2D& resPx)
  {
    TryCaptureFrame(m_camera, m_nativeTexture, m_cameraFrameBitmap, m_cameraFrameId);

    const auto nativeTextureSize = m_nativeTexture.GetSize();

    nativeBatch->Begin(BlendState::Opaque);

    // const Point2 nativeTexSize = m_nativeTexture.GetSize();

    // GLES3 native texture as native resolution
    nativeBatch->Draw(m_nativeTexture, Vector2(0, 0), Color::White());

    // GLES3 native texture handle at top right corner scaled to a 4th of its size
    PxPoint2 scaledSize(nativeTextureSize.Width() / 4, nativeTextureSize.Height() / 4);
    PxRectangle dstRectangle(resPx.Width() - scaledSize.X, 0, scaledSize.X, scaledSize.Y);
    nativeBatch->Draw(GLES3::GLTextureInfo(m_nativeTexture.Get(), m_nativeTexture.GetSize()), dstRectangle, Color::White());

    // API independent texture
    // const Point2 texSize = m_texture2D.GetSize();
    // m_nativeBatch->Draw(m_texture2D, Vector2(res.X - texSize.X, offsetY), Color::White());
    // m_nativeBatch->Draw(m_texture2D, Rectangle(res.X - texSize.X - texSize.X / 2, offsetY + texSize.Y, texSize.X / 2, texSize.Y / 2),
    // Color::White());

    nativeBatch->End();
  }
}
