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
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Log/FmtPixelFormat.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/ICustomSpriteResourceManager.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslUtil/OpenGLES3/DebugStrings.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <GLES3/gl3.h>


namespace Fsl
{
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
      IContentManager& ContentManager;
      ICustomSpriteResourceManager& CustomSpriteResourceManager;
      IGraphicsService& GraphicsService;
      std::shared_ptr<UI::WindowContext> WindowContext;
      TextureCapabilities TextureCaps;

      CreateContext(IContentManager& rContentManager, ICustomSpriteResourceManager& rCustomSpriteResourceManager, IGraphicsService& rGraphicsService,
                    std::shared_ptr<UI::WindowContext> windowContext, const TextureCapabilities& textureCaps)
        : ContentManager(rContentManager)
        , CustomSpriteResourceManager(rCustomSpriteResourceManager)
        , GraphicsService(rGraphicsService)
        , WindowContext(std::move(windowContext))
        , TextureCaps(textureCaps)
      {
      }
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


    std::shared_ptr<UI::BaseWindow> CreateTextureControl(const CreateContext& context, const Texture& texture, const std::string& caption)
    {
      constexpr UI::DpLayoutSize1D forcedSizeDp(DpValue(320));

      Texture2D sourceTexture(context.GraphicsService.GetNativeGraphics(), texture, Texture2DFilterHint::Smooth);

      std::shared_ptr<BasicImageSprite> spriteTex =
        context.CustomSpriteResourceManager.CreateCustomTextureSprite(sourceTexture.GetNative(), BlendState::AlphaBlend);


      auto label = std::make_shared<UI::Label>(context.WindowContext);
      label->SetAlignmentX(UI::ItemAlignment::Center);
      label->SetAlignmentY(UI::ItemAlignment::Far);
      label->SetContentAlignmentX(UI::ItemAlignment::Center);
      label->SetContentAlignmentY(UI::ItemAlignment::Far);
      label->SetContent(caption);
      label->SetWidth(forcedSizeDp);

      auto tex = std::make_shared<UI::Image>(context.WindowContext);
      tex->SetScalePolicy(UI::ItemScalePolicy::FitKeepAR);
      tex->SetContent(spriteTex);
      tex->SetAlignmentX(UI::ItemAlignment::Center);
      tex->SetAlignmentY(UI::ItemAlignment::Center);

      auto stack = std::make_shared<UI::StackLayout>(context.WindowContext);
      stack->SetOrientation(UI::LayoutOrientation::Vertical);
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


    std::shared_ptr<UI::BaseWindow> CreateTextureControl(const CreateContext& context, const Texture& texture, const Texture& notSupportedTexture)
    {
      const auto pixelFormat = texture.GetPixelFormat();
      bool isSupported = IsSupported(context.TextureCaps, pixelFormat);

      std::string caption(Fsl::Debug::ToString(pixelFormat));
      if (StringUtil::StartsWith(caption, "PixelFormat::"))
      {
        StringUtil::Replace(caption, "PixelFormat::", "");
      }

      FSLLOG3_INFO("- {}: {}", caption, isSupported);
      if (Fsl::LogConfig::GetLogLevel() >= LogType::Verbose)
      {
        //  FSLLOG3_INFO("  - properties.linearTilingFeatures: {}", GetBitflagsString(properties.linearTilingFeatures));
        //  FSLLOG3_INFO("  - properties.optimalTilingFeatures: {}", GetBitflagsString(properties.optimalTilingFeatures));
        //  FSLLOG3_INFO("  - properties.bufferFeatures: {}", GetBitflagsString(properties.bufferFeatures));
      }

      if (isSupported)
      {
        return CreateTextureControl(context, texture, caption);
      }

      return CreateTextureControl(context, notSupportedTexture, caption);
    }


    void CreateTextureControlsIfSupported(std::deque<std::shared_ptr<UI::BaseWindow>>& rTextures, const CreateContext& context, const IO::Path& path,
                                          const PixelFormat switchPF, const Texture& notSupportedTexture)
    {
      auto newPath = IO::Path::Combine(TEXTURE_PATH, path);

      // If we are loading a compressed texture the 'contentManager' wont modify it and
      // the KTX loader does not report the origin correctly and always returns 'UpperLeft',
      // so we 'request' that origin then override it below since we know the textures are stored with LowerLeft origin
      auto texture = context.ContentManager.ReadTexture(newPath, switchPF, BitmapOrigin::UpperLeft);
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
        FSLLOG3_INFO("Failed to create texture from source: '{}' with pixel format: {}", path, texture.GetPixelFormat());
        throw;
      }
    }

    std::string TextureFormatToString(const GLint format)
    {
      const auto* psz = GLES3::Debug::TryTextureFormatToString(format);
      return (psz != nullptr ? psz : "Unknown");
    }
  }

  TextureCompression::TextureCompression(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
  {
    // https://developer.android.com/guide/topics/graphics/opengl.html

    auto compressedTextureFormats = GLUtil::GetCompressedTextureFormats();
    FSLLOG3_INFO("Compressed texture formats:");
    for (auto format : compressedTextureFormats)
    {
      FSLLOG3_INFO("- Format: 0x{:x} ({})", static_cast<GLint>(format), TextureFormatToString(format));
    }

    const auto textureCaps = GetTextureCapabilities();

    FSLLOG3_INFO("Compression extensions");
    FSLLOG3_INFO("ASTC");
    FSLLOG3_INFO("- GL_KHR_texture_compression_astc_ldr: {}", textureCaps.HasASTC_LDR);
    FSLLOG3_INFO("- GL_KHR_texture_compression_astc_hdr: {}", textureCaps.HasASTC_HDR);
    FSLLOG3_INFO("- GL_OES_texture_compression_astc: {}", textureCaps.HasASTC_OES);
    FSLLOG3_INFO("- GL_KHR_texture_compression_astc_sliced_3d: {}", textureCaps.HasASTC_Sliced3D);
    FSLLOG3_INFO("- GL_EXT_texture_compression_astc_decode_mode: {}", textureCaps.HasASTC_DecodeMode);
    FSLLOG3_INFO("- GL_EXT_texture_compression_astc_decode_mode_rgb9e5: {}", textureCaps.HasASTC_DecodeModeRGB9E5);

    FSLLOG3_INFO("ATITC/ATC");
    FSLLOG3_INFO("- GL_AMD_compressed_ATC_texture: {}", textureCaps.HasATITC1);
    FSLLOG3_INFO("- GL_AMD_compressed_ATC_texture: {}", textureCaps.HasATITC2);

    FSLLOG3_INFO("ETC1");
    FSLLOG3_INFO("- GL_OES_compressed_ETC1_RGB8_texture: {}", textureCaps.HasETC1);

    FSLLOG3_INFO("ETC2");
    FSLLOG3_INFO("- Mandatory in OpenGL ES 3");

    FSLLOG3_INFO("PVRTC");
    FSLLOG3_INFO("- GL_IMG_texture_compression_pvrtc: {}", textureCaps.HasPVRTC);

    FSLLOG3_INFO("PVRTC2");
    FSLLOG3_INFO("- GL_IMG_texture_compression_pvrtc2: {}", textureCaps.HasPVRTC2);

    FSLLOG3_INFO("S3TC");
    FSLLOG3_INFO("- GL_EXT_texture_compression_s3tc: {}", textureCaps.HasS3TC);
    FSLLOG3_INFO("- GL_EXT_texture_compression_dxt1: {}", textureCaps.HasS3TC_dxt1);

    FSLLOG3_INFO("");
    FSLLOG3_INFO("");


    std::string supportETC2("1");

    FSLLOG3_INFO("ASTC (Adaptive scalable texture compression)");
    FSLLOG3_INFO("- LDR: {}", textureCaps.HasASTC_LDR);
    FSLLOG3_INFO("- HDR: {}", textureCaps.HasASTC_HDR);
    FSLLOG3_INFO("- OES: {}", textureCaps.HasASTC_OES);
    FSLLOG3_INFO("- Sliced3D: {}", textureCaps.HasASTC_Sliced3D);
    FSLLOG3_INFO("- DecodeMode: {}", textureCaps.HasASTC_DecodeMode);
    FSLLOG3_INFO("- DecodeModeRGB9E5: {}", textureCaps.HasASTC_DecodeModeRGB9E5);
    FSLLOG3_INFO("ETC1 (Ericsson Texture Compression): {}", textureCaps.HasETC1);
    FSLLOG3_INFO("ETC2 (Ericsson Texture Compression): {}", supportETC2);
    FSLLOG3_INFO("PVRTC - PowerVR texture compression: {}", textureCaps.HasPVRTC);
    FSLLOG3_INFO("PVRTC2 - PowerVR texture compression: {}", textureCaps.HasPVRTC2);
    FSLLOG3_INFO("ATITC (ATI texture compression) also known as ATC: {}", (textureCaps.HasATITC1 || textureCaps.HasATITC2));
    FSLLOG3_INFO("S3TC (S3 texture compression) DXTn, DXTC or BCn texture compression");
    FSLLOG3_INFO("- All: {}", textureCaps.HasS3TC);
    FSLLOG3_INFO("- DXT1 only: {}", textureCaps.HasS3TC_dxt1);

    CreateUI(config.DemoServiceProvider);
  }


  TextureCompression::~TextureCompression() = default;


  void TextureCompression::Update(const DemoTime& /*demoTime*/)
  {
  }


  void TextureCompression::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

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

    auto contentManager = GetContentManager();
    auto graphicsService = serviceProvider.Get<IGraphicsService>();

    CreateContext createContext(*contentManager, m_uiExtension->GetCustomSpriteResourceManager(), *graphicsService, m_uiExtension->GetContext(),
                                GetTextureCapabilities());

    FSLLOG3_INFO("Creating UI");

    auto texDefault =
      createContext.ContentManager.ReadTexture("Textures/NotSupported/NotSupported_pre.png", PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);

    std::deque<std::shared_ptr<UI::BaseWindow>> textures;

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

    auto wrapLayout = std::make_shared<UI::WrapLayout>(createContext.WindowContext);
    wrapLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    wrapLayout->SetSpacing(DpPoint2F::Create(4, 4));
    wrapLayout->SetAlignmentX(UI::ItemAlignment::Center);
    wrapLayout->SetAlignmentY(UI::ItemAlignment::Center);

    for (const auto& tex : textures)
    {
      wrapLayout->AddChild(tex);
    }

    m_scrollable = std::make_shared<UI::VerticalScroller>(createContext.WindowContext);
    m_scrollable->SetContent(wrapLayout);
    m_scrollable->SetScrollPadding(DpThicknessF::Create(0, 32, 0, 32));
    // scrollLayout->SetAlignmentX(ItemAlignment::Stretch);
    // scrollLayout->SetAlignmentY(ItemAlignment::Stretch);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<UI::FillLayout>(createContext.WindowContext);
    fillLayout->AddChild(m_scrollable);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }
}
