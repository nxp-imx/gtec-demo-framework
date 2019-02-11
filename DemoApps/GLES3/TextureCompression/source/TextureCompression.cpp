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

#include "TextureCompression.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslSimpleUI/Base/Control/Extended/Texture2DImage.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <GLES3/gl3.h>


namespace Fsl
{
  using namespace UI;
  using namespace GLES3;

  namespace
  {
    const auto TEXTURE_PATH = "Textures/GPUSdkTest";

    IO::Path ToTexPath(const char* const psz)
    {
      return IO::Path::Combine(TEXTURE_PATH, psz);
    }

    struct CreateContext
    {
      std::shared_ptr<IContentManager> ContentManager;
      std::shared_ptr<IGraphicsService> GraphicsService;
      std::shared_ptr<UI::WindowContext> WindowContext;
    };

    std::shared_ptr<BaseWindow> CreateTextureControl(const CreateContext& context, const IO::Path& path,
                                                     PixelFormat switchPF = PixelFormat::Undefined)
    {
      // Since we are loading a compressed texture the 'contentManager' wont modify it and
      // the KTX loader does not report the origin correctly and always returns 'UpperLeft',
      // so we 'request' that origin then override it below since we know the textures are stored with LowerLeft origin
      auto texture = context.ContentManager->ReadTexture(path, switchPF, BitmapOrigin::UpperLeft);
      texture.OverrideOrigin(BitmapOrigin::LowerLeft);

      try
      {
        Texture2D sourceTexture(context.GraphicsService->GetNativeGraphics(), texture, Texture2DFilterHint::Smooth);

        auto label = std::make_shared<Label>(context.WindowContext);
        label->SetAlignmentX(ItemAlignment::Center);
        label->SetAlignmentY(ItemAlignment::Far);
        label->SetContent(Fsl::Debug::ToString(texture.GetPixelFormat()));

        auto tex = std::make_shared<Texture2DImage>(context.WindowContext);
        tex->SetScalePolicy(UI::ItemScalePolicy::FitKeepAR);
        tex->SetContent(sourceTexture);
        tex->SetAlignmentX(ItemAlignment::Center);
        tex->SetAlignmentY(ItemAlignment::Center);
        // tex1->SetWidth(256);
        // tex1->SetHeight(256);

        auto stack = std::make_shared<StackLayout>(context.WindowContext);
        stack->SetLayoutOrientation(LayoutOrientation::Vertical);
        stack->AddChild(label);
        stack->AddChild(tex);
        return stack;
      }
      catch (const std::exception&)
      {
        FSLLOG("Failed to create texture from source: '" << path << "' with pixel format: " << texture.GetPixelFormat());
        throw;
      }
    }
  }

  TextureCompression::TextureCompression(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))    // Prepare the extension
  {
    // https://developer.android.com/guide/topics/graphics/opengl.html

    // ASTC (Adaptive scalable texture compression)

    // https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_texture_compression_astc_hdr.txt
    const bool hasASTC_LDR = GLUtil::HasExtension("GL_KHR_texture_compression_astc_ldr");
    const bool hasASTC_HDR = GLUtil::HasExtension("GL_KHR_texture_compression_astc_hdr");
    // https://www.khronos.org/registry/OpenGL/extensions/OES/OES_texture_compression_astc.txt
    // Extends GL_KHR_texture_compression_astc_ldr and GL_KHR_texture_compression_astc_hdr
    const bool hasASTC_OES = GLUtil::HasExtension("GL_OES_texture_compression_astc");
    // https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_texture_compression_astc_sliced_3d.txt
    // Extends GL_KHR_texture_compression_astc_ldr
    const bool hasASTC_Sliced3D = GLUtil::HasExtension("GL_KHR_texture_compression_astc_sliced_3d");
    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_astc_decode_mode.txt
    // Requires GL_KHR_texture_compression_astc_hdr, GL_KHR_texture_compression_astc_ldr or GL_OES_texture_compression_astc
    const bool hasASTC_DecodeMode = GLUtil::HasExtension("GL_EXT_texture_compression_astc_decode_mode");
    const bool hasASTC_DecodeModeRGB9E5 = GLUtil::HasExtension("GL_EXT_texture_compression_astc_decode_mode_rgb9e5");

    // ATITC (ATI texture compression), also known as ATC
    //
    const bool hasATITC1 = GLUtil::HasExtension("GL_AMD_compressed_ATC_texture");
    const bool hasATITC2 = GLUtil::HasExtension("GL_ATI_texture_compression_atitc");
    const bool hasATITC = hasATITC1 || hasATITC2;

    // ETC1 (Ericsson Texture Compression)
    const bool hasETC1 = GLUtil::HasExtension("GL_OES_compressed_ETC1_RGB8_texture");

    // ETC2 is mandatory in OpenGL ES3
    const bool hasETC2 = true;

    // PVRTC - PowerVR texture compression
    const bool hasPVRTC = GLUtil::HasExtension("GL_IMG_texture_compression_pvrtc");

    // PVRTC2 - PowerVR texture compression
    const bool hasPVRTC2 = GLUtil::HasExtension("GL_IMG_texture_compression_pvrtc2");

    // S3TC (S3 texture compression), also called DXTn, DXTC or BCn
    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_s3tc.txt
    const bool hasS3TC = GLUtil::HasExtension("GL_EXT_texture_compression_s3tc");
    const bool hasS3TC_dxt1 = GLUtil::HasExtension("GL_EXT_texture_compression_dxt1");


    FSLLOG("Compression extensions");
    FSLLOG("ASTC");
    FSLLOG("- GL_KHR_texture_compression_astc_ldr: " << hasASTC_LDR);
    FSLLOG("- GL_KHR_texture_compression_astc_hdr: " << hasASTC_HDR);
    FSLLOG("- GL_OES_texture_compression_astc: " << hasASTC_OES);
    FSLLOG("- GL_KHR_texture_compression_astc_sliced_3d: " << hasASTC_Sliced3D);
    FSLLOG("- GL_EXT_texture_compression_astc_decode_mode: " << hasASTC_DecodeMode);
    FSLLOG("- GL_EXT_texture_compression_astc_decode_mode_rgb9e5: " << hasASTC_DecodeModeRGB9E5);

    FSLLOG("ATITC/ATC");
    FSLLOG("- GL_AMD_compressed_ATC_texture: " << hasATITC1);
    FSLLOG("- GL_AMD_compressed_ATC_texture: " << hasATITC2);

    FSLLOG("ETC1");
    FSLLOG("- GL_OES_compressed_ETC1_RGB8_texture: " << hasETC1);

    FSLLOG("ETC2");
    FSLLOG("- Mandatory in OpenGL ES 3");

    FSLLOG("PVRTC");
    FSLLOG("- GL_IMG_texture_compression_pvrtc: " << hasPVRTC);

    FSLLOG("PVRTC2");
    FSLLOG("- GL_IMG_texture_compression_pvrtc2: " << hasPVRTC2);

    FSLLOG("S3TC");
    FSLLOG("- GL_EXT_texture_compression_s3tc: " << hasS3TC);
    FSLLOG("- GL_EXT_texture_compression_dxt1: " << hasS3TC_dxt1);

    FSLLOG("");
    FSLLOG("");

    FSLLOG("ASTC (Adaptive scalable texture compression)");
    FSLLOG("- LDR: " << hasASTC_LDR);
    FSLLOG("- HDR: " << hasASTC_HDR);
    FSLLOG("- OES: " << hasASTC_OES);
    FSLLOG("- Sliced3D: " << hasASTC_Sliced3D);
    FSLLOG("- DecodeMode: " << hasASTC_DecodeMode);
    FSLLOG("- DecodeModeRGB9E5: " << hasASTC_DecodeModeRGB9E5);
    FSLLOG("ETC1 (Ericsson Texture Compression): " << hasETC1);
    FSLLOG("ETC2 (Ericsson Texture Compression): " << hasETC2);
    FSLLOG("PVRTC - PowerVR texture compression: " << hasPVRTC);
    FSLLOG("PVRTC2 - PowerVR texture compression: " << hasPVRTC2);
    FSLLOG("ATITC (ATI texture compression) also known as ATC: " << hasATITC);
    FSLLOG("S3TC (S3 texture compression) DXTn, DXTC or BCn texture compression");
    FSLLOG("- All: " << hasS3TC);
    FSLLOG("- DXT1 only: " << hasS3TC_dxt1);

    CreateUI(config.DemoServiceProvider);
  }


  TextureCompression::~TextureCompression() = default;


  void TextureCompression::Update(const DemoTime& demoTime)
  {
  }


  void TextureCompression::Draw(const DemoTime& demoTime)
  {
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void TextureCompression::CreateUI(const ServiceProvider& serviceProvider)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    CreateContext createContext;
    createContext.ContentManager = GetContentManager();
    createContext.GraphicsService = serviceProvider.Get<IGraphicsService>();
    // Next up we prepare the actual UI
    createContext.WindowContext = m_uiExtension->GetContext();

    auto tex0 = CreateTextureControl(createContext, ToTexPath("CustomTexture_R8G8B8A8_flipped.ktx"), PixelFormat::R8G8B8A8_UNORM);
    auto tex1 = CreateTextureControl(createContext, ToTexPath("CustomTexture_R8G8B8A8_flipped.ktx"), PixelFormat::R8G8B8A8_SRGB);
    auto tex2 = CreateTextureControl(createContext, ToTexPath("CustomTexture_ETC2_RGB_flipped.ktx"), PixelFormat::ETC2_R8G8B8_UNORM_BLOCK);
    auto tex3 = CreateTextureControl(createContext, ToTexPath("CustomTexture_ETC2_RGB_flipped.ktx"), PixelFormat::ETC2_R8G8B8_SRGB_BLOCK);
    auto tex4 = CreateTextureControl(createContext, ToTexPath("CustomTexture_ETC2_RGB_A1_flipped.ktx"), PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK);
    auto tex5 = CreateTextureControl(createContext, ToTexPath("CustomTexture_ETC2_RGB_A1_flipped.ktx"), PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK);
    auto tex6 = CreateTextureControl(createContext, ToTexPath("CustomTexture_ETC2_RGBA_flipped.ktx"), PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK);
    auto tex7 = CreateTextureControl(createContext, ToTexPath("CustomTexture_ETC2_RGBA_flipped.ktx"), PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK);

    auto wrapLayout = std::make_shared<WrapLayout>(createContext.WindowContext);
    wrapLayout->SetLayoutOrientation(LayoutOrientation::Horizontal);
    wrapLayout->SetSpacing(Vector2(4, 4));
    wrapLayout->SetAlignmentX(ItemAlignment::Center);
    wrapLayout->SetAlignmentY(ItemAlignment::Center);
    wrapLayout->AddChild(tex0);
    wrapLayout->AddChild(tex1);
    wrapLayout->AddChild(tex2);
    wrapLayout->AddChild(tex3);
    wrapLayout->AddChild(tex4);
    wrapLayout->AddChild(tex5);
    wrapLayout->AddChild(tex6);
    wrapLayout->AddChild(tex7);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<FillLayout>(createContext.WindowContext);
    fillLayout->AddChild(wrapLayout);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }
}
