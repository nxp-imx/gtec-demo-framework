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
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslSimpleUI/Base/Control/Extended/Texture2DImage.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslUtil/OpenGLES3/DebugStrings.hpp>
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

    struct TextureCapabilities
    {
      // ASTC (Adaptive scalable texture compression)
      // https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_texture_compression_astc_hdr.txt
      bool HasASTC_LDR{false};
      bool HasASTC_HDR{false};
      // https://www.khronos.org/registry/OpenGL/extensions/OES/OES_texture_compression_astc.txt
      // Extends GL_KHR_texture_compression_astc_ldr and GL_KHR_texture_compression_astc_hdr
      bool HasASTC_OES{false};
      // https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_texture_compression_astc_sliced_3d.txt
      // Extends GL_KHR_texture_compression_astc_ldr
      bool HasASTC_Sliced3D{false};
      // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_astc_decode_mode.txt
      // Requires GL_KHR_texture_compression_astc_hdr, GL_KHR_texture_compression_astc_ldr or GL_OES_texture_compression_astc
      bool HasASTC_DecodeMode{false};
      bool HasASTC_DecodeModeRGB9E5{false};

      // ATITC (ATI texture compression), also known as ATC
      bool HasATITC1{false};
      bool HasATITC2{false};

      // ETC1 (Ericsson Texture Compression)
      bool HasETC1{false};

      // PVRTC - PowerVR texture compression
      bool HasPVRTC{false};

      // PVRTC2 - PowerVR texture compression
      bool HasPVRTC2{false};

      // S3TC (S3 texture compression), also called DXTn, DXTC or BCn
      // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_s3tc.txt
      bool HasS3TC{false};
      bool HasS3TC_dxt1{false};
    };

    struct CreateContext
    {
      std::shared_ptr<IContentManager> ContentManager;
      std::shared_ptr<IGraphicsService> GraphicsService;
      std::shared_ptr<UI::WindowContext> WindowContext;
      TextureCapabilities TextureCaps;
    };

    TextureCapabilities GetTextureCapabilities()
    {
      TextureCapabilities caps{};
      // ASTC (Adaptive scalable texture compression)
      caps.HasASTC_LDR = GLUtil::HasExtension("GL_KHR_texture_compression_astc_ldr");
      caps.HasASTC_HDR = GLUtil::HasExtension("GL_KHR_texture_compression_astc_hdr");
      caps.HasASTC_OES = GLUtil::HasExtension("GL_OES_texture_compression_astc");
      caps.HasASTC_Sliced3D = GLUtil::HasExtension("GL_KHR_texture_compression_astc_sliced_3d");
      caps.HasASTC_DecodeMode = GLUtil::HasExtension("GL_EXT_texture_compression_astc_decode_mode");
      caps.HasASTC_DecodeModeRGB9E5 = GLUtil::HasExtension("GL_EXT_texture_compression_astc_decode_mode_rgb9e5");

      // ATITC (ATI texture compression), also known as ATC
      caps.HasATITC1 = GLUtil::HasExtension("GL_AMD_compressed_ATC_texture");
      caps.HasATITC2 = GLUtil::HasExtension("GL_ATI_texture_compression_atitc");

      // ETC1 (Ericsson Texture Compression)
      caps.HasETC1 = GLUtil::HasExtension("GL_OES_compressed_ETC1_RGB8_texture");

      // ETC2 is mandatory in OpenGL ES3

      // PVRTC - PowerVR texture compression
      caps.HasPVRTC = GLUtil::HasExtension("GL_IMG_texture_compression_pvrtc");

      // PVRTC2 - PowerVR texture compression
      caps.HasPVRTC2 = GLUtil::HasExtension("GL_IMG_texture_compression_pvrtc2");

      // S3TC (S3 texture compression), also called DXTn, DXTC or BCn
      // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_s3tc.txt
      caps.HasS3TC = GLUtil::HasExtension("GL_EXT_texture_compression_s3tc");
      caps.HasS3TC_dxt1 = GLUtil::HasExtension("GL_EXT_texture_compression_dxt1");
      return caps;
    }


    std::shared_ptr<BaseWindow> CreateTextureControl(const CreateContext& context, const Texture& texture, const std::string& caption)
    {
      Texture2D sourceTexture(context.GraphicsService->GetNativeGraphics(), texture, Texture2DFilterHint::Smooth);

      auto label = std::make_shared<Label>(context.WindowContext);
      label->SetAlignmentX(ItemAlignment::Center);
      label->SetAlignmentY(ItemAlignment::Far);
      label->SetContent(caption);

      auto tex = std::make_shared<Texture2DImage>(context.WindowContext);
      tex->SetScalePolicy(UI::ItemScalePolicy::FitKeepAR);
      tex->SetContent(sourceTexture);
      tex->SetAlignmentX(ItemAlignment::Center);
      tex->SetAlignmentY(ItemAlignment::Center);
      tex->SetBlendState(BlendState::AlphaBlend);
      // tex1->SetWidth(256);
      // tex1->SetHeight(256);

      auto stack = std::make_shared<StackLayout>(context.WindowContext);
      stack->SetLayoutOrientation(LayoutOrientation::Vertical);
      stack->AddChild(label);
      stack->AddChild(tex);
      return stack;
    }

    //! This assumes all uncompressed formats are supported (which most likely is not true, but its good enough for the current use case)
    //! Querying the extensions on all format lookups is also slow and ought to be cached instead
    bool IsSupported(const TextureCapabilities& textureCaps, const PixelFormat pixelFormat)
    {
      const bool isCompressed = PixelFormatUtil::IsCompressed(pixelFormat);
      if (!isCompressed)
      {
        return true;
      }

      switch (PixelFormatUtil::GetPixelFormatLayout(pixelFormat))
      {
      case PixelFormatLayout::ASTC_4x4_BLOCK:
      case PixelFormatLayout::ASTC_5x4_BLOCK:
      case PixelFormatLayout::ASTC_5x5_BLOCK:
      case PixelFormatLayout::ASTC_6x5_BLOCK:
      case PixelFormatLayout::ASTC_6x6_BLOCK:
      case PixelFormatLayout::ASTC_8x5_BLOCK:
      case PixelFormatLayout::ASTC_8x6_BLOCK:
      case PixelFormatLayout::ASTC_8x8_BLOCK:
      case PixelFormatLayout::ASTC_10x5_BLOCK:
      case PixelFormatLayout::ASTC_10x6_BLOCK:
      case PixelFormatLayout::ASTC_10x8_BLOCK:
      case PixelFormatLayout::ASTC_10x10_BLOCK:
      case PixelFormatLayout::ASTC_12x10_BLOCK:
      case PixelFormatLayout::ASTC_12x12_BLOCK:
        return textureCaps.HasASTC_LDR;
      case PixelFormatLayout::ETC2_R8G8B8_BLOCK:
      case PixelFormatLayout::ETC2_R8G8B8A1_BLOCK:
      case PixelFormatLayout::ETC2_R8G8B8A8_BLOCK:
        return true;
      default:
        return false;
      }
    }


    std::shared_ptr<BaseWindow> CreateTextureControl(const CreateContext& context, const Texture& texture, const Texture& notSupportedTexture)
    {
      const auto pixelFormat = texture.GetPixelFormat();
      bool isSupported = IsSupported(context.TextureCaps, pixelFormat);

      std::string caption(Fsl::Debug::ToString(pixelFormat));
      if (StringUtil::StartsWith(caption, "PixelFormat::"))
      {
        StringUtil::Replace(caption, "PixelFormat::", "");
      }

      FSLLOG("- " << caption << ": " << isSupported);
      if (Fsl::Logger::GetLogLevel() >= LogType::Verbose)
      {
        //  FSLLOG("  - properties.linearTilingFeatures: " << GetBitflagsString(properties.linearTilingFeatures));
        //  FSLLOG("  - properties.optimalTilingFeatures: " << GetBitflagsString(properties.optimalTilingFeatures));
        //  FSLLOG("  - properties.bufferFeatures: " << GetBitflagsString(properties.bufferFeatures));
      }

      if (isSupported)
      {
        return CreateTextureControl(context, texture, caption);
      }

      return CreateTextureControl(context, notSupportedTexture, caption);
    }


    void CreateTextureControlsIfSupported(std::deque<std::shared_ptr<BaseWindow>>& rTextures, const CreateContext& context, const IO::Path& path,
                                          const PixelFormat switchPF, const Texture& notSupportedTexture)
    {
      auto newPath = IO::Path::Combine(TEXTURE_PATH, path);

      // If we are loading a compressed texture the 'contentManager' wont modify it and
      // the KTX loader does not report the origin correctly and always returns 'UpperLeft',
      // so we 'request' that origin then override it below since we know the textures are stored with LowerLeft origin
      auto texture = context.ContentManager->ReadTexture(newPath, switchPF, BitmapOrigin::UpperLeft);
      texture.OverrideOrigin(BitmapOrigin::LowerLeft);

      try
      {
        rTextures.push_back(CreateTextureControl(context, texture, notSupportedTexture));

        auto newPixelFormat = PixelFormatUtil::TryTransform(switchPF, PixelFormatFlags::NF_Srgb);
        if (newPixelFormat != PixelFormat::Undefined)
        {
          texture.SetCompatiblePixelFormat(newPixelFormat);
          // auto properties = context.PhysicalDevice.GetPhysicalDeviceFormatProperties(ConvertUtil::Convert(newPixelFormat));
          rTextures.push_back(CreateTextureControl(context, texture, notSupportedTexture));
        }
      }
      catch (const std::exception&)
      {
        FSLLOG("Failed to create texture from source: '" << path << "' with pixel format: " << texture.GetPixelFormat());
        throw;
      }
    }

    std::string TextureFormatToString(const GLint format)
    {
      auto psz = GLES3::Debug::TryTextureFormatToString(format);
      return (psz != nullptr ? psz : "Unknown");
    }
  }

  TextureCompression::TextureCompression(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))    // Prepare the extension
  {
    // https://developer.android.com/guide/topics/graphics/opengl.html

    auto compressedTextureFormats = GLUtil::GetCompressedTextureFormats();
    FSLLOG("Compressed texture formats:");
    for (auto format : compressedTextureFormats)
    {
      FSLLOG("- Format: 0x" << std::hex << (GLint)format << std::dec << " (" << TextureFormatToString(format) << ")");
    }

    const auto textureCaps = GetTextureCapabilities();

    FSLLOG("Compression extensions");
    FSLLOG("ASTC");
    FSLLOG("- GL_KHR_texture_compression_astc_ldr: " << textureCaps.HasASTC_LDR);
    FSLLOG("- GL_KHR_texture_compression_astc_hdr: " << textureCaps.HasASTC_HDR);
    FSLLOG("- GL_OES_texture_compression_astc: " << textureCaps.HasASTC_OES);
    FSLLOG("- GL_KHR_texture_compression_astc_sliced_3d: " << textureCaps.HasASTC_Sliced3D);
    FSLLOG("- GL_EXT_texture_compression_astc_decode_mode: " << textureCaps.HasASTC_DecodeMode);
    FSLLOG("- GL_EXT_texture_compression_astc_decode_mode_rgb9e5: " << textureCaps.HasASTC_DecodeModeRGB9E5);

    FSLLOG("ATITC/ATC");
    FSLLOG("- GL_AMD_compressed_ATC_texture: " << textureCaps.HasATITC1);
    FSLLOG("- GL_AMD_compressed_ATC_texture: " << textureCaps.HasATITC2);

    FSLLOG("ETC1");
    FSLLOG("- GL_OES_compressed_ETC1_RGB8_texture: " << textureCaps.HasETC1);

    FSLLOG("ETC2");
    FSLLOG("- Mandatory in OpenGL ES 3");

    FSLLOG("PVRTC");
    FSLLOG("- GL_IMG_texture_compression_pvrtc: " << textureCaps.HasPVRTC);

    FSLLOG("PVRTC2");
    FSLLOG("- GL_IMG_texture_compression_pvrtc2: " << textureCaps.HasPVRTC2);

    FSLLOG("S3TC");
    FSLLOG("- GL_EXT_texture_compression_s3tc: " << textureCaps.HasS3TC);
    FSLLOG("- GL_EXT_texture_compression_dxt1: " << textureCaps.HasS3TC_dxt1);

    FSLLOG("");
    FSLLOG("");


    std::string supportETC2("1");

    FSLLOG("ASTC (Adaptive scalable texture compression)");
    FSLLOG("- LDR: " << textureCaps.HasASTC_LDR);
    FSLLOG("- HDR: " << textureCaps.HasASTC_HDR);
    FSLLOG("- OES: " << textureCaps.HasASTC_OES);
    FSLLOG("- Sliced3D: " << textureCaps.HasASTC_Sliced3D);
    FSLLOG("- DecodeMode: " << textureCaps.HasASTC_DecodeMode);
    FSLLOG("- DecodeModeRGB9E5: " << textureCaps.HasASTC_DecodeModeRGB9E5);
    FSLLOG("ETC1 (Ericsson Texture Compression): " << textureCaps.HasETC1);
    FSLLOG("ETC2 (Ericsson Texture Compression): " << supportETC2);
    FSLLOG("PVRTC - PowerVR texture compression: " << textureCaps.HasPVRTC);
    FSLLOG("PVRTC2 - PowerVR texture compression: " << textureCaps.HasPVRTC2);
    FSLLOG("ATITC (ATI texture compression) also known as ATC: " << (textureCaps.HasATITC1 || textureCaps.HasATITC2));
    FSLLOG("S3TC (S3 texture compression) DXTn, DXTC or BCn texture compression");
    FSLLOG("- All: " << textureCaps.HasS3TC);
    FSLLOG("- DXT1 only: " << textureCaps.HasS3TC_dxt1);

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

    createContext.TextureCaps = GetTextureCapabilities();

    FSLLOG("Creating UI");

    auto texDefault =
      createContext.ContentManager->ReadTexture("Textures/NotSupported/NotSupported_pre.png", PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);

    std::deque<std::shared_ptr<BaseWindow>> textures;

    {    // Uncompressed
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_R8G8B8A8_flipped.ktx", PixelFormat::R8G8B8A8_UNORM, texDefault);
    }
    {    // ASTC
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_4x4_flipped.ktx", PixelFormat::ASTC_4x4_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_5x4_flipped.ktx", PixelFormat::ASTC_5x4_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_5x5_flipped.ktx", PixelFormat::ASTC_5x5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_6x5_flipped.ktx", PixelFormat::ASTC_6x5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_6x6_flipped.ktx", PixelFormat::ASTC_6x6_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_8x5_flipped.ktx", PixelFormat::ASTC_8x5_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_8x6_flipped.ktx", PixelFormat::ASTC_8x6_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_8x8_flipped.ktx", PixelFormat::ASTC_8x8_UNORM_BLOCK, texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x5_flipped.ktx", PixelFormat::ASTC_10x5_UNORM_BLOCK,
                                       texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x6_flipped.ktx", PixelFormat::ASTC_10x6_UNORM_BLOCK,
                                       texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x8_flipped.ktx", PixelFormat::ASTC_10x8_UNORM_BLOCK,
                                       texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_10x10_flipped.ktx", PixelFormat::ASTC_10x10_UNORM_BLOCK,
                                       texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_12x10_flipped.ktx", PixelFormat::ASTC_12x10_UNORM_BLOCK,
                                       texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ASTC_12x12_flipped.ktx", PixelFormat::ASTC_12x12_UNORM_BLOCK,
                                       texDefault);
    }
    {    // ETC2
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ETC2_RGB_flipped.ktx", PixelFormat::ETC2_R8G8B8_UNORM_BLOCK,
                                       texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ETC2_RGB_A1_flipped.ktx", PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK,
                                       texDefault);
      CreateTextureControlsIfSupported(textures, createContext, "CustomTexture_ETC2_RGBA_flipped.ktx", PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK,
                                       texDefault);
    }

    auto wrapLayout = std::make_shared<WrapLayout>(createContext.WindowContext);
    wrapLayout->SetLayoutOrientation(LayoutOrientation::Horizontal);
    wrapLayout->SetSpacing(Vector2(4, 4));
    wrapLayout->SetAlignmentX(ItemAlignment::Center);
    wrapLayout->SetAlignmentY(ItemAlignment::Center);

    for (const auto& tex : textures)
    {
      wrapLayout->AddChild(tex);
    }

    m_scrollable = std::make_shared<VerticalScroller>(createContext.WindowContext);
    m_scrollable->SetContent(wrapLayout);
    m_scrollable->SetScrollPadding(ThicknessF(0, 32, 0, 32));
    // scrollLayout->SetAlignmentX(ItemAlignment::Stretch);
    // scrollLayout->SetAlignmentY(ItemAlignment::Stretch);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<FillLayout>(createContext.WindowContext);
    fillLayout->AddChild(m_scrollable);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }
}
